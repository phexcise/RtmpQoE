
// finalDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "finalDlg.h"
#include"resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HANDLE hNetMutex1;
HANDLE hThread_dl_param;
HANDLE hThread_dl_speed;
HANDLE hThread_adp_sel;
//static int adp_selection=-1;
static int recv_dnsdelay_data=0;
static __int64 dlspeed_total=0;
static __int64 dlspeed_avg=0;
static int dlspeed_checknum=0;
static int ever_stopped=0;

typedef struct delay_param
{
	int dns_delay;
	int tcp_delay;
	int rtmp_delay;
}delay_param;

// CfinalDlg �Ի���
CfinalDlg::CfinalDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CfinalDlg::IDD, pParent)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	memset(charData_pause_time,0,sizeof(charData_pause_time));
	dnsdelay1=0;
	tcpdelay1=0;
	rtmpdelay1=0;
	m_resolution_value = _T("");
	m_bpp_value = _T("");
	m_bitrate_value = _T("");
	m_pause_freq_value = _T("");
	m_paused_time_value = _T("");
}

void CfinalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIDEOCODEC, m_videocodec);
	DDX_Control(pDX, IDC_BITRATE, m_bitrate);
	DDX_Control(pDX, IDC_FORMAT, m_format);
	DDX_Control(pDX, IDC_INPUT_PROTOCOL, m_input_protocol);
	DDX_Control(pDX, IDC_RESOLUTION, m_resolution);
	DDX_Control(pDX, IDC_FRAMERATE, m_framerate);
	DDX_Control(pDX, IDC_PIXFMT, m_pixfmt);
	DDX_Control(pDX, IDC_AUDIOSAMPLERATE, m_audiosamplerate);
	DDX_Control(pDX, IDC_AUDIOCODEC, m_audiocodec);
	DDX_Control(pDX, IDC_AUDIOCHANNEL, m_audiochannel);
	DDX_Control(pDX, IDC_METADATA, m_metadata);
	DDX_Control(pDX, IDC_PAUSE_FREQ, m_paused_num);
	DDX_Control(pDX, IDC_PAUSE_TIME, m_paused_time);
	DDX_Control(pDX, IDC_TCHART1, m_chart0);
	DDX_Control(pDX, IDC_TCHART3, m_chart1);
	DDX_Control(pDX, IDC_TCHART2, m_chart2);
	DDX_Control(pDX, IDC_DOWNLOAD_SPEED, m_dl_speed);
	DDX_Control(pDX, IDC_AUDIO_BITRATE, m_a_bitrate);
	DDX_Control(pDX, IDC_PAUSE_GAP, m_pause_gap);
	DDX_Control(pDX, IDC_BUFF_TIME, m_buff_time);
	DDX_Control(pDX, IDC_BPP, m_bpp);
	DDX_Control(pDX, IDC_QP, m_qp);
	DDX_Text(pDX, IDC_RESOLUTION, m_resolution_value);
	DDX_Text(pDX, IDC_BPP, m_bpp_value);
	DDX_Text(pDX, IDC_BITRATE, m_bitrate_value);
	DDX_Text(pDX, IDC_PAUSE_FREQ, m_pause_freq_value);
	DDX_Text(pDX, IDC_PAUSE_TIME, m_paused_time_value);
}

BEGIN_MESSAGE_MAP(CfinalDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PLAY, &CfinalDlg::OnBnClickedPlay)
	ON_MESSAGE(WM_DL_PARAM,OnDlParam)//��Ϣӳ��
	ON_MESSAGE(WM_DL_SPEED,OnDlSpeed)//��Ϣӳ��
	//ON_MESSAGE(WM_ADP_SEL,OnAdpSel)//��Ϣӳ��
	ON_BN_CLICKED(IDOK, &CfinalDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SCORE, &CfinalDlg::OnBnClickedScore)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CHOOSE_DEV, &CfinalDlg::OnBnClickedChooseDev)
	ON_BN_CLICKED(IDC_WRITE, &CfinalDlg::OnBnClickedWrite)
	ON_BN_CLICKED(IDCANCEL, &CfinalDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CfinalDlg ��Ϣ�������

BOOL CfinalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	lineSeries_v_bitrate=(CSeries)m_chart0.Series(0);//һ��Ҫ�ŵ������Ϊlineseries���ڶ������֮����е�
	lineSeries_a_bitrate=(CSeries)m_chart0.Series(1);
	barSeries0= (CSeries)m_chart1.Series(0);
	lineSeries1= (CSeries)m_chart2.Series(0);

	SeriesMarks=(CMarks)barSeries0.get_Marks();
	SeriesMarks1=(CMarks)lineSeries1.get_Marks();
	//SeriesMarks1.put_Visible(TRUE);

	page0=m_chart0.get_Page();
	page1=m_chart1.get_Page();
	page2=m_chart2.get_Page();

	//�Ӵ���,Ҫ������ģ̬�Ի�����Ҫ������������Create()��ShowWindow()
	score=new Score;
	score->Create(IDD_DIALOG1,NULL);
	//adplist=new AdapterList;
	//adplist->Create(IDD_DEVLIST_DLG,NULL);
	/*get_devlist(adplist);
	adplist->ShowWindow(SW_SHOW);*/

	hNetMutex1=CreateMutex(NULL,FALSE,NULL);
	//hThread_adp_sel=CreateThread(NULL,0,adp_sel,(LPVOID)adplist->m_hWnd,0,NULL);
	hThread_dl_param=CreateThread(NULL,0,qoe_dl_param,(LPVOID)m_hWnd,0,NULL);
	hThread_dl_speed=CreateThread(NULL,0,qoe_dl_speed,(LPVOID)m_hWnd,0,NULL);
	//CloseHandle(hThread_dl_param);
	//CloseHandle(hThread_dl_speed);

	//rtmp������ַ�б�
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("KBS24-800��rtmp://news24kbs-2.gscdn.com/news24_800/news24_800.stream");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("����1̨��rtmp://pub1.guoshi.com/live/newcetv1");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("����3̨��rtmp://pub1.guoshi.com/live/newcetv3");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("������ӣ�rtmp://live.hkstv.hk.lxdns.com/live/hks");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("��ʯ�ͣ�rtmp://wowza.sinopectv.cn:1935/live/sinopec");

	//��¼����ı��
	fopen_s( &result, "result.csv", "a+" );
	//fprintf(result,"����Ƶ��,��Ƶ�ֱ��ʣ��ߴ磩,bpp,��Ƶ����,�������,����ʱ��,����ʱ��,�����Ե÷�,��Ƶ�����÷�,��������÷�\n");
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CfinalDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CfinalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CfinalDlg::OnBnClickedPlay()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ClearAllSeries0();
	ClearAllSeries1();
	ClearAllSeries2();
	score->ClearAllSeries3();
	
	play_sec_dlg=0;
	//ResumeThread(hThread_adp_sel);
	//ResumeThread(hThread_dl_param);
	//ResumeThread(hThread_dl_speed);

	CString str;
	int index;
	index=((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
	switch(index)
	{
	
	case 0:
		str="rtmp://news24kbs-2.gscdn.com/news24_800/news24_800.stream";
		break;
	case 1:
		str="rtmp://pub1.guoshi.com/live/newcetv1";
		break;
	case 2:
		str="rtmp://pub1.guoshi.com/live/newcetv3";
		break;
	case 3:
		str="rtmp://live.hkstv.hk.lxdns.com/live/hks";
		break;
	case 4:
		str="rtmp://wowza.sinopectv.cn:1935/live/sinopec";
		break;
	case -1:			//��ʱ��ȡ�û����������
		((CComboBox*)GetDlgItem(IDC_COMBO1))->GetWindowTextA(str);
	default:
		break;
	}

	rtmpurl=str.GetBuffer(str.GetLength());	//CStringתchar*
	fwrite(rtmpurl,sizeof(char),str.GetLength(),result);
	CfinalDlg* pDlg=this;
	ffplay(pDlg);
}

//�ڻ���֮ǰ��ͼ�����һ�£�����Ḳ�ǣ��������CSeries�ĺ���Clear();
//ͨ��CTchart ��get_SeriesCount�����������ͼ�����У���ȫ�������������������õ������ö���Ϊ���Ա������
void CfinalDlg::ClearAllSeries0(void)
{	
	for(long i=0;i<m_chart0.get_SeriesCount();i++)
	{
		((CSeries)m_chart0.Series(i)).Clear();
	}
}

void CfinalDlg::ClearAllSeries1(void)
{	
	for(long j=0;j<m_chart1.get_SeriesCount();j++)
	{
		((CSeries)m_chart1.Series(j)).Clear();
	}
}

void CfinalDlg::ClearAllSeries2(void)
{	
	for(long k=0;k<m_chart2.get_SeriesCount();k++)
	{
		((CSeries)m_chart2.Series(k)).Clear();
	}
}
//--------------------------------------------------------------------------------------------------------------

//��Ӧ��createthread���߳���Ӧ������ѡ��������
/*DWORD WINAPI CfinalDlg::adp_sel(LPVOID lpParameter)
{
	WaitForSingleObject(hNetMutex1,INFINITE);		
	while(1)
	{
		HWND hwnd=(HWND)lpParameter;	
		::SendMessageA(hwnd,WM_ADP_SEL,NULL,NULL);//����Ϣ���ݸ��Ի���,delay���ü�&
		//return 0;//�̺߳����з����˾����˳��߳��ˣ�ͬʱmutexҲ��Ϊ���ź�״̬
		ReleaseMutex(hNetMutex1);
		SuspendThread(hThread_adp_sel);
	}
	return 0;//�̺߳����з����˾����˳��߳��ˣ�ͬʱmutexҲ��Ϊ���ź�״̬
}

//WM_ADP_SEL��Ϣ��Ӧ�����ľ���ʵ��
LRESULT CfinalDlg::OnAdpSel(WPARAM wParam,LPARAM lParam)
{
	//get_devlist(adplist);
	//return adplist->ShowWindow(SW_SHOW);
	//return 0;
}*/

//��Ӧ��createthread���߳���Ӧ��������������ʱ��
DWORD WINAPI CfinalDlg::qoe_dl_param(LPVOID lpParameter)
{
	WaitForSingleObject(hNetMutex1,INFINITE);		
	while(1)
	{
		/*if(list_selected_dlg==-1)
		{	
			//ReleaseMutex(hNetMutex1);
			//SuspendThread(hThread_dl_param);
			continue;
		}*/
		HWND hwnd=(HWND)lpParameter;
		delay_param* dp=delay_qoe();//�������̴߳�����ִ�У���ô˳���������ģ���oncreate-��oninitdialog->createthread->������-��delay_qoe->����������
		::SendMessageA(hwnd,WM_DL_PARAM,NULL,(LPARAM)dp);//����Ϣ���ݸ��Ի���,delay���ü�&
		//return 0;//�̺߳����з����˾����˳��߳��ˣ�ͬʱmutexҲ��Ϊ���ź�״̬
		ReleaseMutex(hNetMutex1);
		if(ever_stopped)
		{
			::ResumeThread(hThread_dl_speed);
			ever_stopped=0;
		}
		::SuspendThread(hThread_dl_param);
	}
	return 0;//�̺߳����з����˾����˳��߳��ˣ�ͬʱmutexҲ��Ϊ���ź�״̬
}

//WM_DL_PARAM��Ϣ��Ӧ�����ľ���ʵ��
LRESULT CfinalDlg::OnDlParam(WPARAM wParam,LPARAM lParam)
{
	recv_dnsdelay_data=GetDlgItemInt(IDC_DNS_DELAY);
	if(recv_dnsdelay_data)
	{
		SetDlgItemInt(IDC_DNS_DELAY,0);
	}
	delay_param* dp1=(delay_param*)lParam;//Ҳ���ü�&
	dnsdelay1=dp1->dns_delay;
	recv_dnsdelay_data=dnsdelay1;
	tcpdelay1=dp1->tcp_delay;
	rtmpdelay1=dp1->rtmp_delay;
	SetDlgItemInt(IDC_DNS_DELAY,dnsdelay1);
	SetDlgItemInt(IDC_TCP_DELAY,tcpdelay1);
	SetDlgItemInt(IDC_RTMP_DELAY,rtmpdelay1);
	return 0;
}

//--------------------------------------------------------------------------------------------------------------
//��Ӧ��createthread���߳���Ӧ������������������
DWORD WINAPI CfinalDlg::qoe_dl_speed(LPVOID lpParameter)
{
		
	WaitForSingleObject(hNetMutex1,INFINITE);
	while(1)
	{	
		if(!recv_dnsdelay_data)
		{	
		//	ReleaseMutex(hNetMutex1);
			continue;
		}
		HWND hwnd=(HWND)lpParameter;
		__int64 speed=download_speed();
		dlspeed_checknum++;
		dlspeed_total+=speed;
		dlspeed_avg=dlspeed_total/dlspeed_checknum;
		::SendMessageA(hwnd,WM_DL_SPEED,0,(LPARAM)speed);//����Ϣ���ݸ��Ի���,���ü�&
		ReleaseMutex(hNetMutex1);
	}
	return 0;
}


//WM_DL_SPEED��Ϣ��Ӧ�����ľ���ʵ��
LRESULT CfinalDlg::OnDlSpeed(WPARAM wParam,LPARAM lParam)
{
	UpdateData();

	float qoe_score;
	float access_qoe_score;
	float complete_qoe_score;
	float complete_qoe_score_real;
	float stay_qoe_score;
	float stay_qoe_score_real;

	__int64 speed1=(_int64)lParam;	//Ҳ���ü�&
	if(play_sec_dlg)
	{
		lineSeries1.AddXY((double)play_sec_dlg,(double)speed1,NULL,0);	
	}
	SetDlgItemInt(IDC_DOWNLOAD_SPEED,dlspeed_avg);
	page2.put_Current(page2.get_Count());
	
	access_qoe_score = 0.195 * score->m_access_score_value;
	complete_qoe_score = score->m_video_score_value;
	complete_qoe_score_real=0.226*(score->m_video_score_value) + 0.052*(score->m_audio_score_value);
	stay_qoe_score = score->m_buf_score_value;
	stay_qoe_score_real=0.272 * score->m_buf_score_value +0.255 * score->m_sync_score_value ;
	
	qoe_score = access_qoe_score + complete_qoe_score_real + stay_qoe_score_real;
		
	score->lineSeries2.AddXY((double)play_sec_dlg,100,NULL,0);
	score->lineSeries3.AddXY((double)play_sec_dlg,100,NULL,0);
	score->lineSeries4.AddXY((double)play_sec_dlg,(double)complete_qoe_score,NULL,0);
	score->lineSeries5.AddXY((double)play_sec_dlg,(double)stay_qoe_score,NULL,0);
	score->page3.put_Current(score->page3.get_Count());
	return 0;
}

void CfinalDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CloseHandle(hThread_dl_param);
	CloseHandle(hThread_dl_speed);
	CDialogEx::OnOK();
}

void CfinalDlg::OnBnClickedScore()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//UpdateData();
	
	score->ShowWindow(SW_SHOW);

	/*CString weight;
	_CRT_FLOAT fltval;
	for(int p=0,q=1035;p<=12;p++,q++)
	{
		score->GetDlgItemText(q,weight);
		_atoflt(&fltval,weight.GetBuffer(weight.GetLength()));
		every_weight[p]=fltval.f;
	}*/
}

int CfinalDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	Welcome::ShowSplashScreen(this); //��ʾ�������� 
	/*adplist=new AdapterList;
	adplist->Create(IDD_DEVLIST_DLG,NULL);
	*/
	return 0;
}


void CfinalDlg::OnBnClickedChooseDev()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	get_devlist(adplist);
	adplist->ShowWindow(SW_SHOW);
}

void CfinalDlg::OnBnClickedWrite()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	fprintf(result,",");
	fwrite(m_resolution_value,sizeof(char),m_resolution_value.GetLength(),result);
	fprintf(result,",");
	fwrite(m_bpp_value,sizeof(char),m_bpp_value.GetLength(),result);
	fprintf(result,",");
	fwrite(m_bitrate_value,sizeof(char),m_bitrate_value.GetLength(),result);
	fprintf(result,",");
	fwrite(m_pause_freq_value,sizeof(char),m_pause_freq_value.GetLength(),result);
	fprintf(result,",");
	fwrite(m_paused_time_value,sizeof(char),m_paused_time_value.GetLength(),result);
	fprintf(result,",");
	fprintf(result,"%d",play_sec_dlg);
	fprintf(result,",");
	fprintf(result,"%5.2f",score->m_access_score_value);
	fprintf(result,",");
	fprintf(result,"%5.2f",score->m_video_score_value);
	fprintf(result,",");
	fprintf(result,"%5.2f",score->m_buf_score_value);
	fprintf(result,"\n");
	fflush( result );
}


void CfinalDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ffplay_quit();
	ClearAllSeries0();
	ClearAllSeries1();
	ClearAllSeries2();
	score->ClearAllSeries3();
	SetDlgItemText(IDC_TCP_DELAY,"");
	SetDlgItemText(IDC_DNS_DELAY,"");
	SetDlgItemText(IDC_RTMP_DELAY,"");
	SetDlgItemText(IDC_BUFF_TIME,"");
	SetDlgItemText(IDC_PAUSE_FREQ,"");
	SetDlgItemText(IDC_PAUSE_TIME,"");
	SetDlgItemText(IDC_PAUSE_GAP,"");
	SetDlgItemText(IDC_DOWNLOAD_SPEED,"");
	SetDlgItemText(IDC_FRAMERATE,"");
	SetDlgItemText(IDC_BPP,"");
	SetDlgItemText(IDC_RESOLUTION,"");
	SetDlgItemText(IDC_PIXFMT,"");
	SetDlgItemText(IDC_FORMAT,"");
	SetDlgItemText(IDC_BITRATE,"");
	SetDlgItemText(IDC_INPUT_PROTOCOL,"");
	SetDlgItemText(IDC_VIDEOCODEC,"");
	SetDlgItemText(IDC_AUDIOCHANNEL,"");
	SetDlgItemText(IDC_AUDIOSAMPLERATE,"");
	SetDlgItemText(IDC_AUDIO_BITRATE,"");
	SetDlgItemText(IDC_AUDIOCODEC,"");
	SetDlgItemText(IDC_METADATA,"");
	score->m_score_access.SetWindowTextA("100");
	score->m_score_video.SetWindowTextA("100");
	score->m_score_audio.SetWindowTextA("100");
	score->m_score_buf.SetWindowTextA("100");

	dlspeed_total=0;
	dlspeed_avg=0;
	dlspeed_checknum=0;
	recv_dnsdelay_data=0;
	play_sec_dlg=0;
	memset(charData_pause_time,0,sizeof(charData_pause_time));

	ever_stopped=1;
	::SuspendThread(hThread_dl_speed);
	::ResumeThread(hThread_dl_param);
}

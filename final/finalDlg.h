
// finalDlg.h : ͷ�ļ�
//
#pragma once
#include "stdafx.h"
#include "resource.h"//ֱ�ӹ�ϵ���ܷ���öԻ����еĸ�����Դ
#include "final.h"
#include "afxdialogex.h"
#include "ffplay.h"
#include "AdapterList.h"
#include "net_qoe.h"
#include "CTchart1.h"
#include "Score.h"
#include "afxwin.h"
#include "Welcome.h"

#define WM_DL_PARAM WM_USER+2
#define WM_DL_SPEED WM_USER+3
#define WM_ADP_SEL WM_USER+4
// CfinalDlg �Ի���
class CfinalDlg : public CDialogEx
{
// ����
public:
	CfinalDlg(CWnd* pParent = NULL);	// ��׼���캯��
	
// �Ի�������
	enum { IDD = IDD_FINAL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT  OnDlParam(WPARAM wParam,LPARAM lParam);//������Ϣ��Ӧ��������ʱҪ����������������
	afx_msg LRESULT  OnDlSpeed(WPARAM wParam,LPARAM lParam);//������Ϣ��Ӧ��������ʱҪ����������������
	//afx_msg LRESULT  OnAdpSel(WPARAM wParam,LPARAM lParam);//������Ϣ��Ӧ��������ʱҪ����������������
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedScore();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedChooseDev();
	afx_msg void OnBnClickedOk();

	VideoState* Dlg_is;
	char* rtmpurl;

	AdapterList *adplist;
	Score* score;

	CEdit m_videocodec;
	CEdit m_bitrate;
	CEdit m_format;
	CEdit m_input_protocol;
	CEdit m_resolution;
	CEdit m_framerate;
	CEdit m_pixfmt;
	CEdit m_audiosamplerate;
	CEdit m_audiocodec;
	CEdit m_audiochannel;
	CEdit m_metadata;
	CEdit m_a_bitrate;
	CEdit m_pause_gap;
	CEdit m_paused_num;
	CEdit m_paused_time;
	CEdit m_dl_speed;
	CEdit m_buff_time;
	CEdit m_bpp;
	CEdit m_qp;


	void ClearAllSeries0(void);
	void ClearAllSeries1(void);
	void ClearAllSeries2(void);
	
	CSeries lineSeries_v_bitrate;
	CSeries lineSeries_a_bitrate;
	CSeries barSeries0;
	CSeries lineSeries1;
	CPage page0;
	int charData_v_bitrate[10000];
	int charData_a_bitrate[10000];
	CPage page1;
	int charData_pause_freq[10000];
	int charData_pause_time[10000];
	CPage page2;
	int charData_dl_speed[10000];
	CTchart1 m_chart0;
	CTchart1 m_chart1;
	CTchart1 m_chart2;
	CMarks SeriesMarks;//��״ͼ�����ݱ�ʶ
	CMarks SeriesMarks1;

	static DWORD WINAPI qoe_dl_param(LPVOID lpParameter); 
	static DWORD WINAPI qoe_dl_speed(LPVOID lpParameter);
	//static DWORD WINAPI adp_sel(LPVOID lpParameter);

	int play_sec_dlg;	
	int dnsdelay1;
	int tcpdelay1;
	int rtmpdelay1;
	
	FILE* result;
	CString m_resolution_value;
	CString m_bpp_value;
	CString m_bitrate_value;
	CString m_pause_freq_value;
	CString m_paused_time_value;
	afx_msg void OnBnClickedWrite();
	afx_msg void OnBnClickedCancel();
};



// AdapterList.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "final.h"
#include "AdapterList.h"
#include "afxdialogex.h"


// AdapterList �Ի���

IMPLEMENT_DYNAMIC(AdapterList, CDialogEx)

AdapterList::AdapterList(CWnd* pParent /*=NULL*/)
	: CDialogEx(AdapterList::IDD, pParent)
{

}

AdapterList::~AdapterList()
{
}

BOOL AdapterList::OnInitDialog()
{
	CDialogEx::OnInitDialog();//��һ�����Ҫ���ܶ���
	//����ѡ���б���ߣ���ͷ����������
	DWORD dwExStyle=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE;
	//�����񣻵���ѡ�񣻸�����ʾѡ����
	//��Ƶ
	m_devlist.ModifyStyle(0,LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS);
	m_devlist.SetExtendedStyle(dwExStyle);

	m_devlist.InsertColumn(0,"No.",LVCFMT_CENTER,30,0);
	m_devlist.InsertColumn(1,"name",LVCFMT_LEFT,130,0);
	m_devlist.InsertColumn(2,"description",LVCFMT_LEFT,300,0);

	//get_devlist(this);
	return TRUE;
}

void AdapterList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_devlist);
}


BEGIN_MESSAGE_MAP(AdapterList, CDialogEx)
	ON_BN_CLICKED(IDOK, &AdapterList::OnBnClickedOk)
END_MESSAGE_MAP()


// AdapterList ��Ϣ�������
void AdapterList::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
	for(int i=0; i<m_devlist.GetItemCount(); i++)
	{
		if(m_devlist.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED )
		{
			list_selected_dlg=i;
		}
    }

	ShowWindow(SW_HIDE);
}

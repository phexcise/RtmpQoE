#pragma once
#include "afxwin.h"
#include "resource.h"
#include "stdafx.h"
#include "afxcmn.h"
//#include "net_qoe.h"���ﲻ�ܰ����������д�
//#include "finalDlg.h"

// AdapterList �Ի���

class AdapterList : public CDialogEx
{
	DECLARE_DYNAMIC(AdapterList)

public:
	AdapterList(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~AdapterList();

// �Ի�������
	enum { IDD = IDD_DEVLIST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BOOL AdapterList::OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedOk();
	CListCtrl m_devlist;
};

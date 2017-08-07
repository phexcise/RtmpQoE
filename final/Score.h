#pragma once
#include "afxwin.h"
#include "resource.h"
#include "stdafx.h"
#include "CTchart1.h"

// Score �Ի���

class Score : public CDialogEx
{
	DECLARE_DYNAMIC(Score)

public:
	Score(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Score();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BOOL Score::OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedTree2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEdit28();

	afx_msg void OnBnClickedOk();
	
	CTchart1 m_chart3;
	CSeries lineSeries2;
	CSeries lineSeries3;
	CSeries lineSeries4;
	CSeries lineSeries5;
	CPage page3;
	CMarks SeriesMarks2;
	CMarks SeriesMarks3;
	CMarks SeriesMarks4;
	CMarks SeriesMarks5;
	int charData_qoe_score[10000];
	void ClearAllSeries3(void);
	CEdit m_score_access;
	CEdit m_score_video;
	CEdit m_score_audio;
	CEdit m_score_buf;
	CEdit m_score_sync;
//	float m_video_score_value;
	float m_access_score_value;
	float m_audio_score_value;
	float m_buf_score_value;
	float m_sync_score_value;
	float m_video_score_value;
};

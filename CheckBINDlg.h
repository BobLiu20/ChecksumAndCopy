// CheckBINDlg.h : header file
//

#if !defined(AFX_CHECKBINDLG_H__9B99C5D5_1AFD_489F_81DB_5315C3A2026D__INCLUDED_)
#define AFX_CHECKBINDLG_H__9B99C5D5_1AFD_489F_81DB_5315C3A2026D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCheckBINDlg dialog

class CCheckBINDlg : public CDialog
{
// Construction
public:
	CCheckBINDlg(CWnd* pParent = NULL);	// standard constructor
	void UDisk_Refresh(void);
	void Log_ShowInList(CString strLog);

	CFont m_font;
// Dialog Data
	//{{AFX_DATA(CCheckBINDlg)
	enum { IDD = IDD_CHECKBIN_DIALOG };
	CComboBox	m_cbList;
	CListBox	m_LogList;
	CProgressCtrl m_ProgressCtrl;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckBINDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCheckBINDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonOk_OnClick();
	afx_msg void OnButtonSelectTar_OnClick();
	afx_msg void OnBUTTONSELECTUDisk_OnClick();
	afx_msg void OnButtonSelectTxt_OnClick();
	afx_msg void OnButtonAuto_OnClick();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnButton_Popup();
	afx_msg void OnStaticTips_OnClick();
	afx_msg void OnBUTTONSELECTUdiskByManual();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKBINDLG_H__9B99C5D5_1AFD_489F_81DB_5315C3A2026D__INCLUDED_)

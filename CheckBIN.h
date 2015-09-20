// CheckBIN.h : main header file for the CHECKBIN application
//

#if !defined(AFX_CHECKBIN_H__BE3ABC77_61B6_4FB4_9D25_070459E730A0__INCLUDED_)
#define AFX_CHECKBIN_H__BE3ABC77_61B6_4FB4_9D25_070459E730A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCheckBINApp:
// See CheckBIN.cpp for the implementation of this class
//

class CCheckBINApp : public CWinApp
{
public:
	CCheckBINApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckBINApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCheckBINApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKBIN_H__BE3ABC77_61B6_4FB4_9D25_070459E730A0__INCLUDED_)

// CheckBINDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CheckBIN.h"
#include "CheckBINDlg.h"
#include "Md5.h"
#include "ctime"
#include "RemovalUDisk.h"

#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib") //���û�����У������link����


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char CurrentDir_Buf[300];//�������·��
BOOL isAutoMode = FALSE;//�Ƿ����Զ�ģʽ
BOOL isRuningCopy = FALSE;
int BIN_NumCnt = 0;//ѹ������BIN�ļ�����Ŀ
BOOL isBINCheckGood = FALSE;//MD5����Ƿ���ȷ������������ɫ����

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckBINDlg dialog

CCheckBINDlg::CCheckBINDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckBINDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCheckBINDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCheckBINDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckBINDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_DEV_LIST, m_cbList);
	DDX_Control(pDX, IDC_LIST_LOG, m_LogList);
	DDX_Control(pDX, IDC_PROGRESS_PROCESS, m_ProgressCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCheckBINDlg, CDialog)
	//{{AFX_MSG_MAP(CCheckBINDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OK, OnButtonOk_OnClick)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_TAR, OnButtonSelectTar_OnClick)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_UDisk, OnBUTTONSELECTUDisk_OnClick)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_TXT, OnButtonSelectTxt_OnClick)
	ON_BN_CLICKED(IDC_BUTTON_AUTO, OnButtonAuto_OnClick)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton_Popup)
	ON_BN_CLICKED(IDC_STATIC_TIPS, OnStaticTips_OnClick)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_Udisk_ByManual, OnBUTTONSELECTUdiskByManual)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckBINDlg message handlers

BOOL CCheckBINDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	GetCurrentDirectory(300,CurrentDir_Buf);
	UDisk_Refresh();
	//LOG��ʼ
	time_t t;
	t=time(&t);
	CString t_c = ctime(&t);
	Log_ShowInList(t_c + _T("  Start..."));
	t_c.ReleaseBuffer();
	//��������
	m_font.CreateFontA(30,0,0,0,1000,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_SWISS,"Arial");
	GetDlgItem(IDC_STATIC_TIPS)->SetFont(&m_font);
	//������
	m_ProgressCtrl.SetRange(0,100);
	m_ProgressCtrl.SetPos(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCheckBINDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCheckBINDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCheckBINDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

CWinThread *m_pThread;
struct threadInfo
{
    BOOL Finish;
	BOOL isSuccess;
    CString Path_Src;
	CString Path_Tar;
}Thread_Info;
UINT ThreadFunc_Copy(LPVOID pParm)
{
	threadInfo *pInfo = (threadInfo*)pParm;
	if(!CopyFile(pInfo->Path_Src, pInfo->Path_Tar, FALSE))
		pInfo->isSuccess = 0;
	else
		pInfo->isSuccess = 1;
	pInfo->Finish = 1;
	return 0;
}

#define MAX_BIN_NUM	10
#define TEMP_SW_URL "\\Temp\\"

void CCheckBINDlg::OnButtonOk_OnClick() 
{
	char *CurrentDir_Buf_P;
	// TODO: Add your control notification handler code here
	isRuningCopy = TRUE;
	GetDlgItem(IDC_STATIC_TIPS)->SetWindowText(_T(""));
	m_ProgressCtrl.SetPos(0);
	//1��ɾ���ϵ��ļ��кʹ����µ���ʱ�ļ���
	CString CurrentDir_Buf_C;
	CurrentDir_Buf_C = _T("Cmd.exe /C mkdir \"") + (CString)CurrentDir_Buf + TEMP_SW_URL + "\"";//��������
	WinExec((LPSTR)(LPCTSTR)CurrentDir_Buf_C, SW_HIDE);
	CurrentDir_Buf_C.ReleaseBuffer();
	CurrentDir_Buf_C = _T("Cmd.exe /C del/q \"") + (CString)CurrentDir_Buf + TEMP_SW_URL + "\"";//ɾ������
	WinExec((LPSTR)(LPCTSTR)CurrentDir_Buf_C, SW_HIDE);
	CurrentDir_Buf_C.ReleaseBuffer();
	Sleep(500);//Ҫ���Ӿ���ʱ�������ܸо���������̡�����
	m_ProgressCtrl.SetPos(10);
	//2����ȡ�û����õ�ѹ����·��
	CString str7zPath;
	GetDlgItem(IDC_EDIT_SELECT_TAR)->GetWindowText(str7zPath);
	if(!PathFileExists(str7zPath))
	{
		Log_ShowInList(_T("ѹ����·����������..."));
		isRuningCopy = FALSE;
		if(isAutoMode)
			OnButtonAuto_OnClick();
		return ;
	}
	Sleep(500);//Ҫ���Ӿ���ʱ�������ܸо���������̡�����
	m_ProgressCtrl.SetPos(20);
	//3����ѹ������ʱ�ļ�
	CString strP;
	CString strCur = (CString)CurrentDir_Buf;
	strP.Format(_T("e %s -y-o%s"),"\"" + str7zPath + "\"","\"" + strCur + TEMP_SW_URL + "\"");//��ѹ����
	HINSTANCE hRet =ShellExecute(NULL,"open","\"" + strCur + ("\\Resource\\7z.exe") + "\"",strP,NULL,SW_HIDE);
	Sleep(500);//Ҫ���Ӿ���ʱ�������ܸо���������̡�����
	m_ProgressCtrl.SetPos(30);
	//4����ȡU��·��UDisk_Path
	TCHAR UDisk_Path[] = _T("?:\\");
	TCHAR UDisk_List[50];
	if(m_cbList.GetCount() == 0)
	{
		Log_ShowInList(_T("δ����U�̣�����벢��ˢ�£�"));
		isRuningCopy = FALSE;
		if(isAutoMode)
			OnButtonAuto_OnClick();
		return ;
	}
	for(int cnt = m_cbList.GetCount();cnt>=0;cnt--)
	{
		UDisk_List[0] = 0;
		m_cbList.GetLBText(cnt, UDisk_List);
		if ( UDisk_List[0] >= 'A' && UDisk_List[0] <= 'Z' ) 
		{
			UDisk_Path[0] = UDisk_List[0];// �滻U���̷�
			break;
		}
	}
	if(UDisk_Path[0] == '?')
	{
		Log_ShowInList(_T("��ЧU�̣�ȷ���Ƿ���ȷ���벢ˢ�£�"));
		isRuningCopy = FALSE;
		if(isAutoMode)
			OnButtonAuto_OnClick();
		return ;
	}
	Sleep(500);//Ҫ���Ӿ���ʱ�������ܸо���������̡�����
	Sleep(300);//�ȴ���ѹ�����
	m_ProgressCtrl.SetPos(40);
	//5����txt�ļ����ȡMD5�������ļ����֣�������MD5_TXT_Val��BINFile_Name
	CFileFind finder;
	CString BINFile_Name[MAX_BIN_NUM];
	CString MD5_TXT_Val[MAX_BIN_NUM];
	BOOL bWorking = finder.FindFile((CString)CurrentDir_Buf + TEMP_SW_URL + "*");
	BIN_NumCnt = 0;
	while (bWorking)
	{
      bWorking = finder.FindNextFile();
	  if(BIN_NumCnt < MAX_BIN_NUM)//�����ҳ�����BIN���֣�����.bin
	  {
		CString name = finder.GetFileName();
		if(name.Right(3).CompareNoCase("txt") && name.Right(1).CompareNoCase("."))
		{
			CStdioFile fileR;
			if(!fileR.Open((CString)CurrentDir_Buf + TEMP_SW_URL + name,CFile::modeRead,0))
				break;
			CString lineR;
			while(fileR.ReadString(lineR))
			{
				int len = lineR.Find(" ");
				if(len >0 )
					MD5_TXT_Val[BIN_NumCnt] = lineR.Left(len);
				len = lineR.ReverseFind(' ');
				if(len >0 )
					BINFile_Name[BIN_NumCnt] = lineR.Right(lineR.GetLength()-1-len);
				BIN_NumCnt++;
			}
			fileR.Close();
		}
	  }
	}
	if(BIN_NumCnt == 0)
	{
		Log_ShowInList(_T("δ��������ļ���ȷ��ѹ���������Ƿ���ȷ��"));
		isRuningCopy = FALSE;
		if(isAutoMode)
			OnButtonAuto_OnClick();
		return ;
	}
	finder.Close();
	Sleep(500);//Ҫ���Ӿ���ʱ�������ܸо���������̡�����
	m_ProgressCtrl.SetPos(50);
	//6������BIN��U��
	Log_ShowInList(_T("�����У������ĵȴ�...(���Խ��ʱ��Խ��)"));
	MSG msg;
	for(int a=0;a<BIN_NumCnt;a++)
	{
		Thread_Info.Path_Src = (CString)CurrentDir_Buf + _T(TEMP_SW_URL + BINFile_Name[a]);//��¼����ļ�����·��
		Thread_Info.Path_Tar = UDisk_Path + BINFile_Name[a];
		Thread_Info.Finish = FALSE;
		Thread_Info.isSuccess = FALSE;
		m_pThread = AfxBeginThread(ThreadFunc_Copy,&Thread_Info);
		if (m_pThread ==NULL)MessageBox("�����������󡣡����������´����!");
		while(::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(Thread_Info.Finish)//����!
			{
				if(!Thread_Info.isSuccess)
				{
					Log_ShowInList(_T("����BIN��U�̳���������..."));
					isRuningCopy = FALSE;
					if(isAutoMode)
						OnButtonAuto_OnClick();
					return ;
				}
				break;
			}
		}
	}
	//Sleep(500);//Ҫ���Ӿ���ʱ�������ܸо���������̡�����
	m_ProgressCtrl.SetPos(60);
	//7����ȡU���ϵ�BIN�ļ���MD5ֵ��MD5_UDisk_Val[MAX_BIN_NUM]
	CString MD5_UDisk_Val[MAX_BIN_NUM];
	for(int i = 0;i < BIN_NumCnt;i ++)
	{
		MD5_UDisk_Val[i] = ValueMD5(UDisk_Path + BINFile_Name[i]);
		MD5_UDisk_Val[i] = MD5_UDisk_Val[i].Left(MD5_UDisk_Val[i].GetLength()-1);
	}
	m_ProgressCtrl.SetPos(70);

	Sleep(500);//Ҫ���Ӿ���ʱ�������ܸо���������̡�����
	m_ProgressCtrl.SetPos(80);
	//9���Ƚ�����MD5ֵ
	for(int k = 0;k < BIN_NumCnt;k++)
	{
		if(MD5_UDisk_Val[k].CompareNoCase(MD5_TXT_Val[k]) != 0)//�����ִ�Сд
		{
			Log_ShowInList(_T("NG...") + BINFile_Name[k] + _T("  Error!(MD5 diff)****"));
			Log_ShowInList(_T("LOCAL MD5:") + MD5_TXT_Val[k]);
			Log_ShowInList(_T("UDisk MD5:") + MD5_UDisk_Val[k]);
			isBINCheckGood = FALSE;
			GetDlgItem(IDC_STATIC_TIPS)->SetWindowText(_T("NG"));
			if(isAutoMode)
				OnButtonAuto_OnClick();
			isRuningCopy = FALSE;
#if 1//�Ƿ��ڼ�⵽MD5����ʱǿ��ɾ��U�����BIN�ļ�
			for(int k = 0;k < BIN_NumCnt;k++)
			{
				CurrentDir_Buf_C.ReleaseBuffer();
				CurrentDir_Buf_C = _T("Cmd.exe /C del ") + (CString)UDisk_Path + BINFile_Name[k];
				CurrentDir_Buf_P = (LPSTR)(LPCTSTR)CurrentDir_Buf_C;
				WinExec(CurrentDir_Buf_P, SW_HIDE);
			}
#endif
			return ;
		}
	}
	isBINCheckGood = TRUE;
	Log_ShowInList(_T("PASS...�������У��ͨ��!\n"));
	GetDlgItem(IDC_STATIC_TIPS)->SetWindowText(_T("PASS"));

	Sleep(500);//Ҫ���Ӿ���ʱ�������ܸо���������̡�����
	m_ProgressCtrl.SetPos(90);
	//10���Ƿ񵯳�U��
	if(isAutoMode)
	{
		if(RemovalUDisk(UDisk_Path))
		{
			OnButtonAuto_OnClick();
			MessageBox("�Զ�����U��ʧ�ܣ����������ռ���У�\nһ��������ɱ������ڲ�ɱU�̵���U��ռ�ã�������ʱ�ر�ɱ�����\n\n��ֹͣ�Զ�ģʽ...","Help me...����U��ʧ��");
		}
	}
	Sleep(500);//Ҫ���Ӿ���ʱ�������ܸо���������̡�����
	m_ProgressCtrl.SetPos(100);
	isRuningCopy = FALSE;
}

void CCheckBINDlg::OnButtonSelectTar_OnClick() 
{
	// TODO: Add your control notification handler code here
	CString filePath;
	CFileDialog  FDlg(TRUE , ".tar" , NULL , OFN_HIDEREADONLY , "tar�ļ�(*.tar)|*.tar|�����ļ�(*.*) |*.*||" );
    if(FDlg.DoModal() == IDOK)
    {
        filePath = FDlg.GetPathName();
        UpdateData(false);
        filePath.Replace("//","////");

		GetDlgItem(IDC_EDIT_SELECT_TAR)->SetWindowText(filePath);
		GetDlgItem(IDC_EDIT_SELECT_TAR)->Invalidate();
    }
}

void CCheckBINDlg::UDisk_Refresh(void)
{
	// TODO: Add your control notification handler code here
	TCHAR	i=0, j=0, c, c0, path[MAX_PATH];
	CString s, dir;
	TCHAR VolumeName[12];
	DWORD nVolumeNameSize=12;
	DWORD SerialNumber;
	DWORD MaximumComponentLength;
	TCHAR FileSystemName[10];
	DWORD nFileSystemNameSize=10;
	DWORD FileSystemFlags;

	GetModuleFileName(NULL, path, MAX_PATH);
	c0 = toupper(path[0]);

	m_cbList.ResetContent();
	for(c='Z'; c>='A'; c--) 
	{
		dir.Format(_T("%c:"),c);
		int type=GetDriveType(dir);
		switch(type) 
		{
			case DRIVE_REMOVABLE:
				if (::GetVolumeInformation(dir, VolumeName, nVolumeNameSize, &SerialNumber, &MaximumComponentLength,
					&FileSystemFlags,FileSystemName, nFileSystemNameSize)) 
				{
					s.Format(_T("%s  %s"), dir, VolumeName);
				}
				else
				{
					s.Format(_T("%s"), dir);
				}
				m_cbList.AddString(s);
				if (c==c0) j = i;
				i++;
				break;
			default:
				break;
		}
	}
	m_cbList.SetCurSel(j);
}

void CCheckBINDlg::OnBUTTONSELECTUDisk_OnClick() 
{
	// TODO: Add your control notification handler code here
	UDisk_Refresh();
}

void CCheckBINDlg::OnButtonSelectTxt_OnClick() 
{
	// TODO: Add your control notification handler code here
	CString filePath;
	CFileDialog  FDlg(TRUE , ".txt" , NULL , OFN_HIDEREADONLY , "txt�ļ�(*.txt)|*.txt|�����ļ�(*.*) |*.*||" );
    if(FDlg.DoModal() == IDOK)
    {
        filePath = FDlg.GetPathName();
        UpdateData(false);
        filePath.Replace("//","////");

		GetDlgItem(IDC_EDIT_SELECT_TXT)->SetWindowText(filePath);
		GetDlgItem(IDC_EDIT_SELECT_TXT)->Invalidate();
    }
}

void CCheckBINDlg::Log_ShowInList(CString strLog)
{
	CTime m_time;
	m_time=CTime::GetCurrentTime();
	CString m_strTime = m_time.Format(_T("%X"));
	m_LogList.AddString(m_strTime + _T("  ") + strLog);
	m_strTime.ReleaseBuffer();
	m_LogList.SetCurSel(m_LogList.GetCount()-1);
}

void CCheckBINDlg::OnButtonAuto_OnClick() 
{
	// TODO: Add your control notification handler code here
	if(isAutoMode)
	{
		isAutoMode = FALSE;
		KillTimer(1);
		GetDlgItem(IDC_BUTTON_OK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_AUTO)->SetWindowText(_T("�Զ�"));
	}
	else
	{
		isAutoMode = TRUE;
		SetTimer(1,2000,0);
		GetDlgItem(IDC_BUTTON_OK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_AUTO)->SetWindowText(_T("ֹͣ"));
	}
}

void CCheckBINDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	UDisk_Refresh();
	if(m_cbList.GetCount() != 0 && isRuningCopy == FALSE)
	{
		OnButtonOk_OnClick();
	}
	CDialog::OnTimer(nIDEvent);
}

void CCheckBINDlg::OnClose() 
{
	CTime m_time;
	m_time=CTime::GetCurrentTime();
	CString t_c = m_time.Format(_T("%x")) + _T("  ") + m_time.Format(_T("%X")) + _T(".txt");//�ļ���
	t_c.Replace(_T(":"),_T("-"));
	t_c.Replace(_T("/"),_T("."));

	CStdioFile file;
	if(!file.Open((CString)CurrentDir_Buf + _T("\\Log\\") + t_c, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		CreateDirectory((CString)CurrentDir_Buf + _T("\\Log\\"),NULL);
		if(!file.Open((CString)CurrentDir_Buf + _T("\\Log\\") + t_c, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
		{
			CDialog::OnClose();
			return ;
		}
	}
	file.SeekToEnd(); //��ָ���Ƶ��ļ�β��
	CString strItem;
	for(int i = 0;i < m_LogList.GetCount();i ++)
	{
		m_LogList.GetText(i,strItem);
		file.WriteString(strItem + "\n");
	}
	file.Close();
	strItem.ReleaseBuffer();

	t_c.ReleaseBuffer();

	CDialog::OnClose();
}

HBRUSH CCheckBINDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if (pWnd->GetDlgCtrlID()==IDC_STATIC_TIPS)
	{
		if(isBINCheckGood)
			pDC->SetTextColor(RGB(0,255,0));  //����������ɫ
		else
			pDC->SetTextColor(RGB(255,0,0));  //����������ɫ
		pDC->SetBkMode(TRANSPARENT); //�������屳��Ϊ͸��
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CCheckBINDlg::OnButton_Popup() 
{
	// TODO: Add your control notification handler code here
	TCHAR UDisk_Path[] = _T("?:\\");
	TCHAR UDisk_List[50];
	if(m_cbList.GetCount() == 0)
	{
		return ;
	}
	m_cbList.GetLBText(m_cbList.GetCurSel(), UDisk_List);
	if ( UDisk_List[0] >= 'A' && UDisk_List[0] <= 'Z' ) 
	{
		UDisk_Path[0] = UDisk_List[0];// �滻U���̷�
		RemovalUDisk(UDisk_Path);
	}
	UDisk_Refresh();
}

void CCheckBINDlg::OnStaticTips_OnClick() 
{
	// TODO: Add your control notification handler code here
	
}

void CCheckBINDlg::OnBUTTONSELECTUdiskByManual() 
{
	// TODO: Add your control notification handler code here
	BROWSEINFO bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner =GetSafeHwnd();
	bInfo.lpszTitle = _T("��ѡ��U��(������ʹ�ô˷���ѡ��U��): ");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS; 

	LPITEMIDLIST lpDlist; //�������淵����Ϣ��IDList
	lpDlist = SHBrowseForFolder(&bInfo) ; //��ʾѡ��Ի���
	if(lpDlist != NULL) //�û�����ȷ����ť
	{
		TCHAR chPath[MAX_PATH]; //�����洢·�����ַ���
		SHGetPathFromIDList(lpDlist, chPath);//����Ŀ��ʶ�б�ת�����ַ���
		CString m_strPath = chPath; //��TCHAR���͵��ַ���ת��ΪCString���͵��ַ���
		m_cbList.ResetContent();
		m_cbList.AddString(m_strPath);
		m_cbList.SetCurSel(0);
	}
}

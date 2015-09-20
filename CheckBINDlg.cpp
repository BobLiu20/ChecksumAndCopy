// CheckBINDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CheckBIN.h"
#include "CheckBINDlg.h"
#include "Md5.h"
#include "ctime"
#include "RemovalUDisk.h"

#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib") //如果没有这行，会出现link错误


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char CurrentDir_Buf[300];//软件所处路径
BOOL isAutoMode = FALSE;//是否是自动模式
BOOL isRuningCopy = FALSE;
int BIN_NumCnt = 0;//压缩包里BIN文件的数目
BOOL isBINCheckGood = FALSE;//MD5检查是否正确，用于字体颜色设置

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
	//LOG开始
	time_t t;
	t=time(&t);
	CString t_c = ctime(&t);
	Log_ShowInList(t_c + _T("  Start..."));
	t_c.ReleaseBuffer();
	//设置字体
	m_font.CreateFontA(30,0,0,0,1000,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_SWISS,"Arial");
	GetDlgItem(IDC_STATIC_TIPS)->SetFont(&m_font);
	//进度条
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
	//1、删除老的文件夹和创建新的临时文件夹
	CString CurrentDir_Buf_C;
	CurrentDir_Buf_C = _T("Cmd.exe /C mkdir \"") + (CString)CurrentDir_Buf + TEMP_SW_URL + "\"";//创建命令
	WinExec((LPSTR)(LPCTSTR)CurrentDir_Buf_C, SW_HIDE);
	CurrentDir_Buf_C.ReleaseBuffer();
	CurrentDir_Buf_C = _T("Cmd.exe /C del/q \"") + (CString)CurrentDir_Buf + TEMP_SW_URL + "\"";//删除命令
	WinExec((LPSTR)(LPCTSTR)CurrentDir_Buf_C, SW_HIDE);
	CurrentDir_Buf_C.ReleaseBuffer();
	Sleep(500);//要求：视觉延时，让人能感觉到这个过程。。。
	m_ProgressCtrl.SetPos(10);
	//2、获取用户设置的压缩包路径
	CString str7zPath;
	GetDlgItem(IDC_EDIT_SELECT_TAR)->GetWindowText(str7zPath);
	if(!PathFileExists(str7zPath))
	{
		Log_ShowInList(_T("压缩包路径错误！请检查..."));
		isRuningCopy = FALSE;
		if(isAutoMode)
			OnButtonAuto_OnClick();
		return ;
	}
	Sleep(500);//要求：视觉延时，让人能感觉到这个过程。。。
	m_ProgressCtrl.SetPos(20);
	//3、解压缩到临时文件
	CString strP;
	CString strCur = (CString)CurrentDir_Buf;
	strP.Format(_T("e %s -y-o%s"),"\"" + str7zPath + "\"","\"" + strCur + TEMP_SW_URL + "\"");//解压参数
	HINSTANCE hRet =ShellExecute(NULL,"open","\"" + strCur + ("\\Resource\\7z.exe") + "\"",strP,NULL,SW_HIDE);
	Sleep(500);//要求：视觉延时，让人能感觉到这个过程。。。
	m_ProgressCtrl.SetPos(30);
	//4、获取U盘路径UDisk_Path
	TCHAR UDisk_Path[] = _T("?:\\");
	TCHAR UDisk_List[50];
	if(m_cbList.GetCount() == 0)
	{
		Log_ShowInList(_T("未发现U盘，请插入并点刷新！"));
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
			UDisk_Path[0] = UDisk_List[0];// 替换U盘盘符
			break;
		}
	}
	if(UDisk_Path[0] == '?')
	{
		Log_ShowInList(_T("无效U盘，确认是否正确插入并刷新！"));
		isRuningCopy = FALSE;
		if(isAutoMode)
			OnButtonAuto_OnClick();
		return ;
	}
	Sleep(500);//要求：视觉延时，让人能感觉到这个过程。。。
	Sleep(300);//等待解压缩完成
	m_ProgressCtrl.SetPos(40);
	//5、从txt文件里读取MD5和升级文件名字，保存在MD5_TXT_Val和BINFile_Name
	CFileFind finder;
	CString BINFile_Name[MAX_BIN_NUM];
	CString MD5_TXT_Val[MAX_BIN_NUM];
	BOOL bWorking = finder.FindFile((CString)CurrentDir_Buf + TEMP_SW_URL + "*");
	BIN_NumCnt = 0;
	while (bWorking)
	{
      bWorking = finder.FindNextFile();
	  if(BIN_NumCnt < MAX_BIN_NUM)//连续找出所有BIN名字，不带.bin
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
		Log_ShowInList(_T("未发现软件文件，确认压缩包内容是否正确！"));
		isRuningCopy = FALSE;
		if(isAutoMode)
			OnButtonAuto_OnClick();
		return ;
	}
	finder.Close();
	Sleep(500);//要求：视觉延时，让人能感觉到这个过程。。。
	m_ProgressCtrl.SetPos(50);
	//6、拷贝BIN到U盘
	Log_ShowInList(_T("拷贝中，请耐心等待...(软件越大时间越长)"));
	MSG msg;
	for(int a=0;a<BIN_NumCnt;a++)
	{
		Thread_Info.Path_Src = (CString)CurrentDir_Buf + _T(TEMP_SW_URL + BINFile_Name[a]);//烧录软件文件绝对路径
		Thread_Info.Path_Tar = UDisk_Path + BINFile_Name[a];
		Thread_Info.Finish = FALSE;
		Thread_Info.isSuccess = FALSE;
		m_pThread = AfxBeginThread(ThreadFunc_Copy,&Thread_Info);
		if (m_pThread ==NULL)MessageBox("发生致命错误。。。建议重新打开软件!");
		while(::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(Thread_Info.Finish)//复制!
			{
				if(!Thread_Info.isSuccess)
				{
					Log_ShowInList(_T("复制BIN到U盘出错！请重试..."));
					isRuningCopy = FALSE;
					if(isAutoMode)
						OnButtonAuto_OnClick();
					return ;
				}
				break;
			}
		}
	}
	//Sleep(500);//要求：视觉延时，让人能感觉到这个过程。。。
	m_ProgressCtrl.SetPos(60);
	//7、获取U盘上的BIN文件的MD5值，MD5_UDisk_Val[MAX_BIN_NUM]
	CString MD5_UDisk_Val[MAX_BIN_NUM];
	for(int i = 0;i < BIN_NumCnt;i ++)
	{
		MD5_UDisk_Val[i] = ValueMD5(UDisk_Path + BINFile_Name[i]);
		MD5_UDisk_Val[i] = MD5_UDisk_Val[i].Left(MD5_UDisk_Val[i].GetLength()-1);
	}
	m_ProgressCtrl.SetPos(70);

	Sleep(500);//要求：视觉延时，让人能感觉到这个过程。。。
	m_ProgressCtrl.SetPos(80);
	//9、比较两个MD5值
	for(int k = 0;k < BIN_NumCnt;k++)
	{
		if(MD5_UDisk_Val[k].CompareNoCase(MD5_TXT_Val[k]) != 0)//不区分大小写
		{
			Log_ShowInList(_T("NG...") + BINFile_Name[k] + _T("  Error!(MD5 diff)****"));
			Log_ShowInList(_T("LOCAL MD5:") + MD5_TXT_Val[k]);
			Log_ShowInList(_T("UDisk MD5:") + MD5_UDisk_Val[k]);
			isBINCheckGood = FALSE;
			GetDlgItem(IDC_STATIC_TIPS)->SetWindowText(_T("NG"));
			if(isAutoMode)
				OnButtonAuto_OnClick();
			isRuningCopy = FALSE;
#if 1//是否在检测到MD5错误时强制删除U盘里的BIN文件
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
	Log_ShowInList(_T("PASS...所有软件校验通过!\n"));
	GetDlgItem(IDC_STATIC_TIPS)->SetWindowText(_T("PASS"));

	Sleep(500);//要求：视觉延时，让人能感觉到这个过程。。。
	m_ProgressCtrl.SetPos(90);
	//10、是否弹出U盘
	if(isAutoMode)
	{
		if(RemovalUDisk(UDisk_Path))
		{
			OnButtonAuto_OnClick();
			MessageBox("自动弹出U盘失败，被其它软件占用中！\n一般是由于杀毒软件在查杀U盘导致U盘占用，建议暂时关闭杀毒软件\n\n已停止自动模式...","Help me...弹出U盘失败");
		}
	}
	Sleep(500);//要求：视觉延时，让人能感觉到这个过程。。。
	m_ProgressCtrl.SetPos(100);
	isRuningCopy = FALSE;
}

void CCheckBINDlg::OnButtonSelectTar_OnClick() 
{
	// TODO: Add your control notification handler code here
	CString filePath;
	CFileDialog  FDlg(TRUE , ".tar" , NULL , OFN_HIDEREADONLY , "tar文件(*.tar)|*.tar|所有文件(*.*) |*.*||" );
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
	CFileDialog  FDlg(TRUE , ".txt" , NULL , OFN_HIDEREADONLY , "txt文件(*.txt)|*.txt|所有文件(*.*) |*.*||" );
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
		GetDlgItem(IDC_BUTTON_AUTO)->SetWindowText(_T("自动"));
	}
	else
	{
		isAutoMode = TRUE;
		SetTimer(1,2000,0);
		GetDlgItem(IDC_BUTTON_OK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_AUTO)->SetWindowText(_T("停止"));
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
	CString t_c = m_time.Format(_T("%x")) + _T("  ") + m_time.Format(_T("%X")) + _T(".txt");//文件名
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
	file.SeekToEnd(); //把指针移到文件尾！
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
			pDC->SetTextColor(RGB(0,255,0));  //设置字体颜色
		else
			pDC->SetTextColor(RGB(255,0,0));  //设置字体颜色
		pDC->SetBkMode(TRANSPARENT); //设置字体背景为透明
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
		UDisk_Path[0] = UDisk_List[0];// 替换U盘盘符
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
	bInfo.lpszTitle = _T("请选择U盘(不建议使用此方法选择U盘): ");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS; 

	LPITEMIDLIST lpDlist; //用来保存返回信息的IDList
	lpDlist = SHBrowseForFolder(&bInfo) ; //显示选择对话框
	if(lpDlist != NULL) //用户按了确定按钮
	{
		TCHAR chPath[MAX_PATH]; //用来存储路径的字符串
		SHGetPathFromIDList(lpDlist, chPath);//把项目标识列表转化成字符串
		CString m_strPath = chPath; //将TCHAR类型的字符串转换为CString类型的字符串
		m_cbList.ResetContent();
		m_cbList.AddString(m_strPath);
		m_cbList.SetCurSel(0);
	}
}


// DllInjectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DllInject.h"
#include "DllInjectDlg.h"
#include "afxdialogex.h"

#include "x64Inject.h"
#include <shlwapi.h>
#pragma comment (lib, "shlwapi.lib")

DWORD g_dwPID = 0;
WCHAR g_szDllPath[MAX_PATH] = {0};
CWnd *pBtnInject = NULL;

BOOL AllowMeesageForVistaAbove(UINT uMessage,BOOL bAllow)
{
	typedef BOOL (WINAPI* _ChangeWindowMessageFilter)(UINT, DWORD);
	HMODULE hUserMod = LoadLibraryW(L"user32.dll");
	if (hUserMod == NULL)
	{       
		return FALSE;
	}
	_ChangeWindowMessageFilter pChangeWindowMessageFilter = (_ChangeWindowMessageFilter)GetProcAddress(hUserMod, "ChangeWindowMessageFilter");
	if (NULL == pChangeWindowMessageFilter)
	{       
		FreeLibrary(hUserMod);
		return FALSE;
	}
	BOOL bResult = pChangeWindowMessageFilter(uMessage, bAllow ? 1 : 2); // 1-MSGFLT_ADD, 2-MSGFLT_REMOVE
	if(NULL != hUserMod)
	{
		FreeLibrary(hUserMod);
	}
	return bResult;
}


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDllInjectDlg 对话框




CDllInjectDlg::CDllInjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDllInjectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDllInjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDllInjectDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_INJECT, &CDllInjectDlg::OnBnClickedBtnInject)
	ON_BN_CLICKED(IDC_BTN_BROWSE, &CDllInjectDlg::OnBnClickedBtnBrowse)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CDllInjectDlg 消息处理程序

BOOL CDllInjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//UAC拖拽处理
	AllowMeesageForVistaAbove(WM_DROPFILES,TRUE);
	AllowMeesageForVistaAbove(0x0049, TRUE ); // 0x0049 - WM_COPYGLOBALDATA

	CString strPath = L"C:\\Windows\\System32\\sfc.dll";
	SetDlgItemText(IDC_EDT_DLLPATH,strPath);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDllInjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDllInjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDllInjectDlg::OnBnClickedBtnInject()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPID;
	GetDlgItemText(IDC_EDT_PID,strPID);
	strPID.Replace(L" ", L"");
	if (strPID == L"")
	{
		AfxMessageBox(L"请填写PID！");
		return;	
	}	
	DWORD dwPID = GetDlgItemInt(IDC_EDT_PID);
	if (dwPID%4 != 0)
	{
		AfxMessageBox(L"PID不正确！");
		return;		
	}
	WCHAR szDLLPath[MAX_PATH] = {0};
	GetDlgItemTextW(IDC_EDT_DLLPATH,szDLLPath,MAX_PATH);
	if (!PathFileExistsW(szDLLPath))
	{
		AfxMessageBox(L"DLL不存在！");
		return;
	}
	g_dwPID = dwPID;
	memcpy(g_szDllPath,szDLLPath,MAX_PATH*sizeof(WCHAR));
	AfxBeginThread(ThreadProc,this);
	pBtnInject = GetDlgItem(IDC_BTN_INJECT);
	pBtnInject->EnableWindow(FALSE);
}


void CDllInjectDlg::OnBnClickedBtnBrowse()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,NULL,NULL,6UL,_T("动态链接库(*.DLL)|*.DLL||"));
	if (dlg.DoModal()==IDOK)
	{
		CString strPath = dlg.GetPathName();
		SetDlgItemText(IDC_EDT_DLLPATH,strPath);
	}
}


void CDllInjectDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值	
	UINT uCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);      
	for(UINT i=0; i<uCount; i++)
	{
		char szFilePath[MAX_PATH] = {0};
		DragQueryFileA(hDropInfo, 0, szFilePath, MAX_PATH);
		CStringA strFilePath;
		strFilePath.Format("%s",szFilePath);	
		if (strFilePath.Right(4).MakeUpper() == ".DLL")
		{
			SetDlgItemTextA(m_hWnd,IDC_EDT_DLLPATH,strFilePath);
			break;
		}
	}
	DragFinish(hDropInfo);

	CDialogEx::OnDropFiles(hDropInfo);
}


UINT CDllInjectDlg::ThreadProc(LPVOID lpVoid)
{
	Cx64Inject inject;
	if (inject.InjectDll(g_dwPID,g_szDllPath))
	{
		::MessageBox(NULL,L"注入成功",L":)",MB_ICONINFORMATION);
	}
	else
	{
		::MessageBox(NULL,L"注入失败！",L"失败",MB_ICONERROR);
	}
	pBtnInject->EnableWindow(TRUE);
	return 0;
}


BOOL CDllInjectDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return TRUE; 
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

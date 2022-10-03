
// appDlg.cpp : implementation file
//

#include "stdafx.h"
#include "generator.h"
#include "appDlg.h"
#include "afxdialogex.h"
#include "utils.h"
#include <fileapi.h>
#include "base64.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
//	void AddFile(CString& srcDir, CString& filename);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAppDlg dialog



CAppDlg::CAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GENERATOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROG_FILE, m_progFile);
	DDX_Control(pDX, IDC_PROG_ALL, m_progAll);
	DDX_Control(pDX, IDC_LIST_FILES, m_listFiles);
	DDX_Control(pDX, IDC_EDIT_OUTPUT, m_editOutput);
	DDX_Control(pDX, IDC_CHK_RECURSIVE, m_chkRecursive);
	DDX_Control(pDX, IDC_CHK_URL, m_chkUrl);
	DDX_Control(pDX, IDC_SYSLINK_BLOG, m_linkBlog);
	DDX_Control(pDX, IDC_BTN_ADD_DIR, m_btnAddDir);
	DDX_Control(pDX, IDC_BTN_ADD_FILE, m_btnAddFile);
	DDX_Control(pDX, IDC_BTN_CLEAR, m_btnClear);
	DDX_Control(pDX, IDC_BTN_COPY, m_btnCopy);
	DDX_Control(pDX, IDC_BTN_GENERATE, m_btnGenerate);
}

// NM_CLICK = -2
constexpr UINT NM_CLICK_WITHOUT_WARNING = UINT(-2);

BEGIN_MESSAGE_MAP(CAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_GENERATE, &CAppDlg::OnBnClickedGenerate)
	ON_BN_CLICKED(IDC_BTN_ADD_DIR, &CAppDlg::OnClickedBtnAddDir)
	ON_BN_CLICKED(IDC_BTN_ADD_FILE, &CAppDlg::OnClickedBtnAddFile)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CAppDlg::OnBnClickedBtnClear)
	ON_WM_DROPFILES()
	ON_NOTIFY(NM_CLICK_WITHOUT_WARNING, IDC_SYSLINK_BLOG, &CAppDlg::OnClickSyslinkBlog)
	ON_BN_CLICKED(IDC_BTN_COPY, &CAppDlg::OnBnClickedBtnCopy)
END_MESSAGE_MAP()


// CAppDlg message handlers

BOOL CAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	AppendVersionNumber();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRegKey reg;
	if (reg.Open(HKEY_CURRENT_USER, _T("Software\\Jixun.Moe\\DuGenerator"), KEY_READ) == ERROR_SUCCESS)
	{
		DWORD dwLang;
		if (reg.QueryDWORDValue(_T("LANG_ID"), dwLang) == ERROR_SUCCESS)
		{
			// SetThreadUILanguage(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED));
			SetThreadUILanguage(LANGID(dwLang));
		}

		reg.Close();
	}

	std::ignore = m_ofn_file_title.LoadString(IDS_PICK_FILE);
	std::ignore = m_ofn_dir_title.LoadString(IDS_PICK_DIR);

	m_progFile.SetRange(0, 10000);

	// 初始化目录选择
	std::ignore = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	m_editOutput.SetLimitText(0);


#if _DEBUG
	{
		char buffer[32];
		sprintf_s(buffer, sizeof(buffer), "text limit: 0x%08x\n", m_editOutput.GetLimitText());
		OutputDebugStringA(buffer);
	}
#endif

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void WINAPI _thread_process_file(CAppDlg* app)
{
	app->ProcessFiles();
}


void CAppDlg::OnBnClickedGenerate()
{
	CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(_thread_process_file), this, 0, nullptr);
}

void cb_add_file(const CString &srcDir, const CString &filename, void* extra)
{
	static_cast<CAppDlg*>(extra)->AddFile(srcDir, filename);
}

void CAppDlg::OnClickedBtnAddDir()
{
	auto dirs = OpenDirectoryDialog(m_ofn_dir_title, GetSafeHwnd());
	for(auto& dir : dirs)
	{
		EnumFiles(*dir, BST_CHECKED == m_chkRecursive.GetCheck(), cb_add_file, this);
		delete dir;
	}
}


void CAppDlg::OnClickedBtnAddFile()
{
	auto v = OpenFileDialog(this->m_ofn_file_title, GetSafeHwnd());
	for(auto file : v)
	{
		CString path(*file);
		auto pos = path.ReverseFind(_T('\\'));
		m_listFiles.AddItem(path.Left(pos), path.Right(path.GetLength() - pos - 1));
		delete file;
	}
}

void CAppDlg::AddFile(const CString& srcDir, const CString& filename)
{
	m_listFiles.AddItem(srcDir, filename);
}

void _proc_file_callback(ProcType type, double progress, CFileItem* lpItem, void* extra)
{
	static_cast<CAppDlg*>(extra)->ProcFile(type, progress, lpItem);
}

void CAppDlg::ProcessFiles()
{
	std::lock_guard<std::mutex> guard(mutex);

	OutputDebugString(_T("Begin read file..."));
	m_progAll.SetMax(m_listFiles.GetCount());
	m_progAll.SetCurrent(0);
	m_progFile.SetPos(0);
	
	m_btnAddDir.EnableWindow(FALSE);
	m_btnAddFile.EnableWindow(FALSE);
	m_btnCopy.EnableWindow(FALSE);
	m_btnClear.EnableWindow(FALSE);
	m_btnGenerate.EnableWindow(FALSE);

	m_listFiles.ProcessFiles(_proc_file_callback, this);

	m_btnAddDir.EnableWindow(TRUE);
	m_btnAddFile.EnableWindow(TRUE);
	m_btnCopy.EnableWindow(TRUE);
	m_btnClear.EnableWindow(TRUE);
	m_btnGenerate.EnableWindow(TRUE);
}

void CAppDlg::AddHashEntry(CFileItem* data)
{
	if (m_chkUrl.GetCheck() == BST_CHECKED) {
		m_editOutput.Append(data->BDLink());
	}
	else {
		m_editOutput.Append(data->DownloadCode());
	}
	m_editOutput.Append(_T("\r\n"));
}

void CAppDlg::ProcFile(ProcType proc, double progress, CFileItem* lp_item)
{
	switch(proc)
	{
	case ProcType::FILE_PROG: 
		m_progFile.SetPos(int(progress * 10000));
		break;

	case ProcType::INC_FILE:
		m_progFile.SetPos(0);
		m_progAll.Increase();
		AddHashEntry(lp_item);
		break;

	case ProcType::ERR_FILE:
		OutputDebugString(_T("Failed to process file.\n"));
		m_progAll.Increase(); 
		break;

	default: ;
	}
}

void CAppDlg::RealExit()
{
	m_listFiles.StopProcessing();

	// TODO: Clean up.

	CDialogEx::OnCancel();
}

void CAppDlg::AppendVersionNumber()
{
	CString csEntry;
	HMODULE hLib = AfxGetResourceHandle();

	HRSRC hVersion = FindResource(hLib, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	if (hVersion == nullptr) return;

	HGLOBAL hGlobal = LoadResource(hLib, hVersion);
	if (hGlobal == nullptr) {
		FreeResource(hVersion);
		return;
	}

	LPVOID versionInfo = LockResource(hGlobal);
	if (versionInfo == nullptr) {
		UnlockResource(hGlobal);
		FreeResource(hVersion);
		return;
	}

	LPVOID lpBuffer = nullptr;
	UINT dwBytes = 0;

	if (VerQueryValueW(versionInfo, L"\\StringFileInfo\\080004B0\\FileVersion", &lpBuffer, &dwBytes)) {
		CString strTitle;
		GetWindowText(strTitle);

		strTitle += _T(" (v");
		strTitle += (TCHAR*)lpBuffer;
		strTitle += _T(")");
		SetWindowText(strTitle);
	}

	UnlockResource(hGlobal);
	FreeResource(hVersion);

}

DWORD WINAPI _thread_stop_process(void* param)
{
	static_cast<CAppDlg*>(param)->RealExit();
	return 0;
}

void CAppDlg::OnCancel()
{
	// 若为 ESC 导致的关闭窗口，无视它。
	if ((GetKeyState(VK_ESCAPE) & 0x8000) != 0) {
		return;
	}

	CreateThread(nullptr, 0, _thread_stop_process, this, 0, nullptr);
}


void CAppDlg::OnBnClickedBtnClear()
{
	m_listFiles.ResetContent();
	m_progAll.Reset();
	m_editOutput.SetWindowText(_T(""));
}


void CAppDlg::OnDropFiles(HDROP hDropInfo)
{
	CString path;

	int nFilesDropped = DragQueryFile(hDropInfo, 0xFFFFFFFF, nullptr, 0);

	for (int i = 0; i<nFilesDropped; i++)
	{
		DWORD nBuffer = DragQueryFile(hDropInfo, i, nullptr, 0) + sizeof TCHAR;

		DragQueryFile(hDropInfo, i, path.GetBuffer(nBuffer), nBuffer);
		path.ReleaseBuffer();

		auto isDir = (GetFileAttributes(path) | FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
		if (isDir)
		{
			EnumFiles(path, BST_CHECKED == m_chkRecursive.GetCheck(), cb_add_file, this);
		} else
		{
			auto pos = path.ReverseFind(_T('\\'));
			m_listFiles.AddItem(path.Left(pos), path.Right(path.GetLength() - pos - 1));
		}

		OutputDebugString(path.GetString());
		OutputDebugString(_T("\r\n"));
	}

	DragFinish(hDropInfo);
	CDialogEx::OnDropFiles(hDropInfo);
}


// ReSharper disable once CppMemberFunctionMayBeStatic
// ReSharper disable once CppMemberFunctionMayBeConst
void CAppDlg::OnClickSyslinkBlog(NMHDR *pNMHDR, LRESULT *pResult)
{
	ShellExecute(nullptr, _T("open"), PNMLINK(pNMHDR)->item.szUrl, nullptr, nullptr, SW_SHOWNORMAL);
	*pResult = 0;
}


// ReSharper disable once CppMemberFunctionMayBeConst
void CAppDlg::OnBnClickedBtnCopy()
{
	CString str;
	m_editOutput.GetWindowText(str);
	CopyStringToClipboard(str);
}


// appDlg.h : header file
//

#pragma once

#include "ProgressText.h"
#include "AdvEdit.h"
#include "ListBoxEx.h"
#include "afxwin.h"
#include "afxcmn.h"

// CAppDlg dialog
class CAppDlg : public CDialogEx
{
	CString m_ofn_file_title;
	CString m_ofn_dir_title;

	std::mutex mutex;

// Construction
public:
	CAppDlg(CWnd* pParent = NULL);	// standard constructor
	void AddFile(const CString& srcDir, const CString& filename);
	void ProcessFiles();
	void AddHashEntry(CFileItem* lp_item);
	void ProcFile(ProcType proc, double progress, CFileItem* lp_item);
	void RealExit();
	void AppendVersionNumber();
	std::vector<CString*> files;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GENERATOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedGenerate();
	// Current file Progress
	CProgressCtrl m_progFile;
	CProgressText m_progAll;
	CListBoxEx m_listFiles;
	CAdvEdit m_editOutput;
	afx_msg void OnClickedBtnAddDir();
	afx_msg void OnClickedBtnAddFile();
	virtual void OnCancel();
	afx_msg void OnBnClickedBtnClear();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CButton m_chkRecursive;
	CButton m_chkUrl;
	CLinkCtrl m_linkBlog;
	afx_msg void OnClickSyslinkBlog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnCopy();
	CButton m_btnAddDir;
	CButton m_btnAddFile;
	CButton m_btnClear;
	CButton m_btnCopy;
	CButton m_btnGenerate;
};


// VPKReaderDlg.h : header file
//

#pragma once


#include "WndLayout.h"

#include "VPKFile.h"
#include "TriStateTreeCtrl.h"
#include "FileHandleCache.h"

enum
{
    Find_Flag           = 0x00,
    Find_Prev           = 0x01,
    Find_Next           = 0x02,
};

struct stExportThreadData
{
    CString strVpkDirFile;
    CString strPath;
    CList<const FileInfo*>* pFiles;
};

// CVPKReaderDlg dialog
class CVPKReaderDlg : public CDialog
{
// Construction
public:
	CVPKReaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VPKREADER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
    void AppendLog(LPCTSTR szFmt, ...);
    HTREEITEM   FindOrCreateItem(HTREEITEM hParent, LPCTSTR szPathName);
    void DestroyTreeItem(HTREEITEM hItem);
    void GetSelectedDir(HTREEITEM hItem, CList<const FileInfo*>& files);
    void ExportFiles(CList<const FileInfo*>* pFiles);
    BOOL CheckExportThread();

    BOOL FindPrevFileItem(HTREEITEM hStartItem, LPCTSTR szFilter, DWORD dwFlags, HTREEITEM& hItem, int& nListItemIndex);
    BOOL FindPrevFileItemHelper(HTREEITEM hStartItem, LPCTSTR szFilter, DWORD dwFlags, HTREEITEM& hItem, int& nListItemIndex);
    BOOL FindNextFileItem(HTREEITEM hStartItem, LPCTSTR szFilter, DWORD dwFlags, HTREEITEM& hItem, int& nListItemIndex);
    BOOL FindNextFileItemHelper(HTREEITEM hStartItem, LPCTSTR szFilter, DWORD dwFlags, HTREEITEM& hItem, int& nListItemIndex);

    BOOL FindListFileItem(LPCTSTR szFilter, DWORD dwFlags, HTREEITEM& hItem, int& nListItemIndex);

    BOOL MatchTreeItem(HTREEITEM hTreeItem, LPCTSTR szFilter, DWORD dwFlags, HTREEITEM& hItem, int& nListItemIndex);
    BOOL FileMatch(const FileInfo* pFileInfo, LPCTSTR szFilter, DWORD dwFlags);

    void ClearListSelection();

    static unsigned CALLBACK ExportThreadProc(void* lParam);
    static BOOL ExportFileContent(LPCTSTR szVpkDirFile, LPCSTR szDstPath, const FileInfo* pInfo);

// Implementation
protected:
	HICON m_hIcon;
    HACCEL m_hAccel;

    HANDLE          m_hThread;
    volatile BOOL   m_bStop;

    CEdit       m_EditLog;
    CTriStateTreeCtrl m_Tree;
    CListCtrl   m_List;
    CWndLayout  m_WndLayout;

    CList<CList<const FileInfo*>* > m_arrTreeItemDatas;

    VPKFile m_VPKFile;

    CFindReplaceDialog* m_pFindDlg;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnFileExit();
    afx_msg void OnFileOpen();
    afx_msg void OnEditFindFile();
    afx_msg void OnHelpAbout();
    afx_msg void OnDestroy();
    afx_msg void OnClose();
    afx_msg void OnTvnSelchangedTreeDir(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnFileExportSelectedDirs();
    afx_msg void OnFileExportSelectedFiles();
    afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);

    afx_msg LRESULT OnFindReplaceMsg(WPARAM wParam, LPARAM lParam);

    virtual BOOL PreTranslateMessage(MSG* pMsg);
};


// VPKReaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VPKReader.h"
#include "VPKReaderDlg.h"

#include "FileUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CVPKReaderDlg dialog




CVPKReaderDlg::CVPKReaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVPKReaderDlg::IDD, pParent)
{
    m_hThread = NULL;
    m_bStop = FALSE;
    m_hAccel = NULL;
    m_pFindDlg = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVPKReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_EDIT_LOG, m_EditLog);
    DDX_Control(pDX, IDC_TREE_DIR, m_Tree);
    DDX_Control(pDX, IDC_LIST_FILES, m_List);
}

BEGIN_MESSAGE_MAP(CVPKReaderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_COMMAND(ID_FILE_EXIT, &CVPKReaderDlg::OnFileExit)
    ON_COMMAND(ID_FILE_OPEN, &CVPKReaderDlg::OnFileOpen)
    ON_COMMAND(ID_EDIT_FINDFILE, &CVPKReaderDlg::OnEditFindFile)
    ON_COMMAND(ID_HELP_ABOUT, &CVPKReaderDlg::OnHelpAbout)
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DIR, &CVPKReaderDlg::OnTvnSelchangedTreeDir)
    ON_COMMAND(ID_FILE_EXPORTSELECTEDDIRS, &CVPKReaderDlg::OnFileExportSelectedDirs)
    ON_COMMAND(ID_FILE_EXPORTSELECTEDFILES, &CVPKReaderDlg::OnFileExportSelectedFiles)
    ON_WM_INITMENUPOPUP()
    ON_REGISTERED_MESSAGE(WM_FINDREPLACE, &CVPKReaderDlg::OnFindReplaceMsg)
END_MESSAGE_MAP()


// CVPKReaderDlg message handlers

BOOL CVPKReaderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

    m_hAccel = ::LoadAccelerators(::GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ACCELERATOR_MAIN));

    m_List.InsertColumn(0, _T("FileName"), LVCFMT_LEFT, 240);
    m_List.InsertColumn(1, _T("Length"), LVCFMT_RIGHT, 120);
    m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

    m_WndLayout.Init(m_hWnd);
    m_WndLayout.AddControlById(IDC_TREE_DIR, Layout_Left | Layout_VFill);
    m_WndLayout.AddControlById(IDC_LIST_FILES, Layout_HFill | Layout_VFill);
    m_WndLayout.AddControlById(IDC_EDIT_LOG, Layout_Bottom | Layout_HFill);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVPKReaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CVPKReaderDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVPKReaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVPKReaderDlg::AppendLog(LPCTSTR szFmt, ...)
{
    va_list args;
    va_start(args, szFmt);

    CString strLog;
    strLog.FormatV(szFmt, args);

    va_end(args);

    DWORD dwCurSel = m_EditLog.GetSel();

    DWORD dwLength = m_EditLog.GetWindowTextLength();
    m_EditLog.SetSel(dwLength, dwLength + 1, TRUE);
    m_EditLog.ReplaceSel(strLog);
    m_EditLog.SetSel(dwCurSel, TRUE);
}

void CVPKReaderDlg::OnFileExit()
{
    DestroyWindow();
}

void CVPKReaderDlg::OnFileOpen()
{
    CFileDialog dlg(TRUE, _T("*.vpk"), _T("*_dir.vpk"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("VPK Dir Files|*_dir.vpk"), this);
    if(dlg.DoModal() != IDOK)
        return;

    DestroyTreeItem(m_Tree.GetRootItem());
    m_List.DeleteAllItems();
    m_Tree.DeleteAllItems();
    m_EditLog.Clear();

    // ParseFile

    CString strFilePath = dlg.GetPathName();

    DWORD dwTickCount = ::GetTickCount();
    AppendLog(_T("Parsing file...\r\n"));
    VPKError vpkError = m_VPKFile.Open(strFilePath);
    if(VE_Ok != vpkError)
    {
        AppendLog(_T("Failed to load file, Error: %s, Path: %s\r\n"), GetVPKError(vpkError), (LPCTSTR)strFilePath);
        return;
    }
    AppendLog(_T("File Parsed\r\n"));

    dwTickCount = ::GetTickCount() - dwTickCount;
    AppendLog(_T("Elapse of parsing: %d\r\n"), dwTickCount);
    dwTickCount = ::GetTickCount();

    AppendLog(_T("FilePath: %s\r\n"), m_VPKFile.GetFilePath());
    AppendLog(_T("FileVersion: %u\r\n"), m_VPKFile.GetFileVersion());
    AppendLog(_T("PathCount: %u\r\n"), m_VPKFile.GetPathCount());
    AppendLog(_T("ExtentionCount: %u\r\n"), m_VPKFile.GetExtCount());
    AppendLog(_T("TotalFileCount: %u\r\n"), m_VPKFile.GetTotalFileCount());
    AppendLog(_T("NoneEmptyFileCount: %u\r\n"), m_VPKFile.GetNoneEmptyFileCount());

    AppendLog(_T("Generating Tree...\r\n"));

    const FileInfoList& listFileInfo = m_VPKFile.GetFileInfoList();
    POSITION posInfo = listFileInfo.GetHeadPosition();
    int nCount = listFileInfo.GetCount();

    CString strFileName = strFilePath;
    int posName = strFileName.ReverseFind(_T('\\'));
    if(posName != -1)
        strFileName = strFileName.Mid(posName + 1);

    CString strPath;
    HTREEITEM hRoot = m_Tree.InsertItem(strFileName);

    m_Tree.SetRedraw(FALSE);

    CMapStringToPtr mapPathToTreeItem;
    HTREEITEM hItem = hRoot;
    DWORD dwCreateItemTick = 0;
    void* pKey;
    CString strLowerPathName;
    for(int i=0; i<nCount; ++ i)
    {
        const FileInfo& info = listFileInfo.GetNext(posInfo);

        strLowerPathName = info.strFilePath;
        strLowerPathName.MakeLower();

        if(mapPathToTreeItem.Lookup(strLowerPathName, pKey))
        {
            hItem = (HTREEITEM)pKey;
        }
        else
        {
            hItem = hRoot;
            int tok = 0;
            strPath = info.strFilePath.Tokenize(_T("/"), tok);
            while(!strPath.IsEmpty())
            {
                DWORD dwTick = ::GetTickCount();
                hItem = FindOrCreateItem(hItem, strPath);
                dwTick = ::GetTickCount() - dwTick;
                dwCreateItemTick += dwTick;
                strPath = info.strFilePath.Tokenize(_T("/"), tok);
            }
            mapPathToTreeItem.SetAt(strLowerPathName, (void*)hItem);
        }

        CList<const FileInfo*>* pFiles = (CList<const FileInfo*>*)m_Tree.GetItemData(hItem);
        if(pFiles == NULL)
        {
            pFiles = new CList<const FileInfo*>();
            m_arrTreeItemDatas.AddTail(pFiles);
            m_Tree.SetItemData(hItem, (DWORD_PTR)pFiles);
        }
        pFiles->AddTail(&info);
    }

    dwTickCount = ::GetTickCount() - dwTickCount;
    AppendLog(_T("Elapse of generating the tree: %d\r\n"), dwTickCount);

    AppendLog(_T("Elapse of FindOrCreateItem: %d\r\n"), dwCreateItemTick);

    m_Tree.SetRedraw(TRUE);
    m_Tree.Invalidate();
    m_Tree.UpdateWindow();

    AppendLog(_T("Tree Generated\r\n"));

}

void CVPKReaderDlg::OnEditFindFile()
{
    if(m_pFindDlg == NULL || !::IsWindow(m_pFindDlg->GetSafeHwnd()))
    {
        if(m_pFindDlg == NULL)
            m_pFindDlg = new CFindReplaceDialog();
        m_pFindDlg->Create(TRUE, _T(""), NULL, FR_DOWN | FR_NOMATCHCASE | FR_NOWHOLEWORD);
    }
    m_pFindDlg->ShowWindow(SW_SHOW);
    m_pFindDlg->UpdateWindow();
    m_pFindDlg->SetActiveWindow();
}

void CVPKReaderDlg::OnHelpAbout()
{
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}

HTREEITEM CVPKReaderDlg::FindOrCreateItem(HTREEITEM hParent, LPCTSTR szPathName)
{
    HTREEITEM hChild = m_Tree.GetNextItem(hParent, TVGN_CHILD);
    while(hChild != NULL)
    {
        CString strText = m_Tree.GetItemText(hChild);
        if(strText.CompareNoCase(szPathName) == 0)
            break;
        hChild = m_Tree.GetNextSiblingItem(hChild);
    }
    if(hChild == NULL)
    {
        hChild = m_Tree.InsertItem(szPathName, hParent);
    }

    return hChild;
}

void CVPKReaderDlg::OnClose()
{
    if(m_hThread != NULL)
    {
        if(AfxMessageBox(_T("Files Exporting, do you want to quit anyway?"), MB_YESNO | MB_ICONQUESTION) != IDNO)
            return;

        ::WaitForSingleObject(m_hThread, INFINITE);
        ::CloseHandle(m_hThread);
    }
    CDialog::OnClose();
}

void CVPKReaderDlg::OnDestroy()
{
    m_VPKFile.Clear();

    m_pFindDlg = NULL;
    /*
    // No need to release m_pFindDlg, cause the system will destroy this owned window
    if(m_pFindDlg != NULL)
    {
        if(m_pFindDlg->GetSafeHwnd())
            m_pFindDlg->DestroyWindow();
        delete m_pFindDlg;
        m_pFindDlg = NULL;
    }
    */

    DestroyTreeItem(m_Tree.GetRootItem());

    CDialog::OnDestroy();
}

void CVPKReaderDlg::DestroyTreeItem(HTREEITEM hItem)
{
    if(hItem != NULL)
    {
        CList<FileInfo*>* pList = (CList<FileInfo*>*)m_Tree.GetItemData(hItem);
        if(pList != NULL)
        {
            pList->RemoveAll();
            delete pList;
        }
    }

    HTREEITEM hChild = m_Tree.GetNextItem(hItem, TVGN_CHILD);
    while(hChild != NULL)
    {
        DestroyTreeItem(hChild);
        hChild = m_Tree.GetNextSiblingItem(hChild);
    }
}

void CVPKReaderDlg::OnTvnSelchangedTreeDir(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    m_List.DeleteAllItems();

    CList<FileInfo*>* pList = (CList<FileInfo*>*)pNMTreeView->itemNew.lParam;
    if(pList != NULL)
    {
        POSITION pos = pList->GetHeadPosition();
        int nInfoCount = pList->GetCount();

        m_List.SetRedraw(FALSE);
        CString strTemp;
        for(int i=0; i<nInfoCount; ++ i)
        {
            const FileInfo* pInfo = pList->GetNext(pos);
            strTemp = pInfo->strFileName;
            m_List.InsertItem(i, strTemp);
            m_List.SetItemData(i, (DWORD_PTR)pInfo);

            strTemp.Format(_T("%d"), pInfo->EntryLength + pInfo->PreloadBytes);
            m_List.SetItemText(i, 1, strTemp);
        }
        m_List.SetRedraw(TRUE);
        m_List.Invalidate();
        m_List.UpdateWindow();
    }

    *pResult = 0;
}

void CVPKReaderDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
    if(!bSysMenu && nIndex == 0)
    {
        TriStateTreeState state = m_Tree.GetTriCheck(m_Tree.GetRootItem());
        UINT nCount = m_List.GetSelectedCount();

        pPopupMenu->EnableMenuItem(ID_FILE_EXPORTSELECTEDDIRS, ((state != TSS_UnChecked) ? MF_ENABLED : MF_DISABLED) | MF_BYCOMMAND);
        pPopupMenu->EnableMenuItem(ID_FILE_EXPORTSELECTEDFILES, ((nCount > 0) ? MF_ENABLED : MF_DISABLED) | MF_BYCOMMAND);
    }
}

BOOL CVPKReaderDlg::PreTranslateMessage(MSG* pMsg)
{
    if(::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
        return TRUE;

    if((pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
        && (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP))
    {
        return TRUE;
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void CVPKReaderDlg::OnFileExportSelectedDirs()
{
    if(!CheckExportThread())
        return;

    CList<const FileInfo*>* pFiles = new CList<const FileInfo*>();

    GetSelectedDir(m_Tree.GetRootItem(), *pFiles);

    ExportFiles(pFiles);
}

void CVPKReaderDlg::OnFileExportSelectedFiles()
{
    if(!CheckExportThread())
        return;

    POSITION pos = m_List.GetFirstSelectedItemPosition();
    if(pos == NULL)
        return;

    CList<const FileInfo*>* pFiles = new CList<const FileInfo*>();
    while(pos)
    {
        int nItem = m_List.GetNextSelectedItem(pos);
        const FileInfo* pFile = (const FileInfo*)m_List.GetItemData(nItem);
        if(pFile != NULL)
            pFiles->AddTail(pFile);
    }

    ExportFiles(pFiles);
}

void CVPKReaderDlg::GetSelectedDir(HTREEITEM hItem, CList<const FileInfo*>& files)
{
    TriStateTreeState state = m_Tree.GetTriCheck(hItem);
    if(state != TSS_UnChecked)
    {
        CList<const FileInfo*>* pFiles = (CList<const FileInfo*>*)m_Tree.GetItemData(hItem);
        if(pFiles != NULL)
            files.AddTail(pFiles);
    }

    HTREEITEM hChild = m_Tree.GetNextItem(hItem, TVGN_CHILD);
    while(hChild != NULL)
    {
        TriStateTreeState state = m_Tree.GetTriCheck(hChild);
        if(state != TSS_UnChecked)
        {
            CList<const FileInfo*>* pFiles = (CList<const FileInfo*>*)m_Tree.GetItemData(hChild);
            if(pFiles != NULL)
                files.AddTail(pFiles);
        }
        hChild = m_Tree.GetNextSiblingItem(hChild);
    }

}

void CVPKReaderDlg::ExportFiles(CList<const FileInfo*>* pFiles)
{
    BROWSEINFO bi = {0};
    bi.hwndOwner = m_hWnd;
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_STATUSTEXT | BIF_EDITBOX | BIF_NEWDIALOGSTYLE;
    PIDLIST_ABSOLUTE pidl = SHBrowseForFolder(&bi);
    if(pidl == NULL)
        return;

    TCHAR szPath[MAX_PATH] = {0};
    if(!SHGetPathFromIDList(pidl, szPath))
    {
        AfxMessageBox(_T("Wrong Folder"));
        return;
    }

    m_bStop = FALSE;

    stExportThreadData* data = new stExportThreadData;
    data->strVpkDirFile = m_VPKFile.GetFilePath();
    data->pFiles = pFiles;
    data->strPath = szPath;
    m_hThread = (HANDLE)_beginthreadex(0, 0, &ExportThreadProc, (void*)data, 0, 0);
}

BOOL CVPKReaderDlg::CheckExportThread()
{
    if(m_hThread == NULL)
        return TRUE;

    DWORD dwExitCode = 0;
    if(::GetExitCodeThread(m_hThread, &dwExitCode) && dwExitCode != STILL_ACTIVE)
    {
        ::CloseHandle(m_hThread);
        m_hThread = NULL;
    }

    if(m_hThread != NULL && AfxMessageBox(_T("Files Exporting, do you want to cancel the current job?"), MB_YESNO | MB_ICONQUESTION) != IDNO)
        return FALSE;

    m_bStop = TRUE;
    ::WaitForSingleObject(m_hThread, INFINITE);
    ::CloseHandle(m_hThread);
    m_hThread = NULL;

    return TRUE;
}

unsigned CVPKReaderDlg::ExportThreadProc(void* lParam)
{
    stExportThreadData* data = (stExportThreadData*)lParam;

    POSITION pos = data->pFiles->GetHeadPosition();
    int nCount = data->pFiles->GetCount();
    for(int i=0; i<nCount; ++ i)
    {
        const FileInfo* pInfo = data->pFiles->GetNext(pos);
        ExportFileContent(data->strVpkDirFile, data->strPath, pInfo);
    }

    CFileHandleCache::instance().Clear();

    delete data->pFiles;
    delete data;
    return 0;
}

BOOL CVPKReaderDlg::ExportFileContent(LPCTSTR szVpkDirFile, LPCSTR szDstPath, const FileInfo* pInfo)
{
    CString strFullPath(szDstPath);
    strFullPath += _T("\\");
    strFullPath += pInfo->strFilePath;

    strFullPath.Replace(_T('/'), _T('\\'));
    if(!FileUtil::MakeDirectory(strFullPath))
        return FALSE;

    strFullPath += _T("\\") + pInfo->strFileName;

    FileUtil::CFileWriteHandle hDstFile(::CreateFile(strFullPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));
    if(hDstFile == INVALID_HANDLE_VALUE)
        return FALSE;

    // Write Preload
    if(pInfo->PreloadBytes > 0)
    {
        HANDLE hSrcFile = CFileHandleCache::instance().GetFileHandle(szVpkDirFile);
        if(hSrcFile == INVALID_HANDLE_VALUE)
            return FALSE;
        if(!FileUtil::WriteFile(hSrcFile, hDstFile, pInfo->PreloadOffset, pInfo->PreloadBytes))
            return FALSE;
    }

    // Write Offset
    if(pInfo->EntryLength > 0)
    {
        if(pInfo->ArchiveIndex == 0x7fff)
        {
            HANDLE hSrcFile = CFileHandleCache::instance().GetFileHandle(szVpkDirFile);
            if(hSrcFile == INVALID_HANDLE_VALUE)
                return FALSE;
            if(!FileUtil::WriteFile(hSrcFile, hDstFile, pInfo->EntryOffset, pInfo->EntryLength))
                return FALSE;
        }
        else
        {
            CString strVpkFile = szVpkDirFile;
            int pos = strVpkFile.ReverseFind(_T('_'));
            if(pos == -1)
                return FALSE;

            strVpkFile = strVpkFile.Left(pos + 1);
            CString strTemp;
            strTemp.Format(_T("%03d.vpk"), pInfo->ArchiveIndex);
            strVpkFile += strTemp;
            HANDLE hSrcFile = CFileHandleCache::instance().GetFileHandle(strVpkFile);
            if(hSrcFile == INVALID_HANDLE_VALUE)
                return FALSE;
            if(!FileUtil::WriteFile(hSrcFile, hDstFile, pInfo->EntryOffset, pInfo->EntryLength))
                return FALSE;
        }
    }

    return TRUE;
}

LRESULT CVPKReaderDlg::OnFindReplaceMsg(WPARAM, LPARAM)
{
    if(m_pFindDlg == NULL)
        return 0;

    if(m_pFindDlg->IsTerminating())
    {
        m_pFindDlg->DestroyWindow();
        m_pFindDlg = NULL;
    }
    else if(m_pFindDlg->FindNext())
    {
        HTREEITEM hItem = NULL;
        int nListItemIndex = 0;

        CString strFilter = m_pFindDlg->GetFindString();
        if(strFilter.Find(_T("*")) == -1 || strFilter.Find(_T("?")) == -1)
            strFilter = _T("*") + strFilter + _T("*");

        DWORD dwFlags = 0;
        dwFlags |= m_pFindDlg->SearchDown() ? Find_Next : Find_Prev;
        // dwFlags |= m_pFindDlg->MatchWholeWord() ? Find_WholeMatch : 0;
        // dwFlags |= m_pFindDlg->MatchCase() ? Find_CaseSensitive : 0;

        BOOL bFound = FindListFileItem(strFilter, dwFlags, hItem, nListItemIndex);
        if(!bFound)
        {
            HTREEITEM hStartItem = m_Tree.GetSelectedItem();
            if(hStartItem == NULL)
            {
                if(dwFlags & Find_Prev)
                    dwFlags = (dwFlags | Find_Next) & (~Find_Prev);

                hStartItem = m_Tree.GetRootItem();
            }
            if(dwFlags & Find_Next)
                bFound = FindNextFileItem(hStartItem, strFilter, dwFlags, hItem, nListItemIndex);
            else
                bFound = FindPrevFileItem(hStartItem, strFilter, dwFlags, hItem, nListItemIndex);
        }
        if(!bFound)
        {
            ::MessageBox(m_pFindDlg->GetSafeHwnd(), _T("File Not Found"), _T("VPKReader"), MB_OK | MB_ICONWARNING);
        }
        else
        {
            ClearListSelection();
            // m_Tree.Select(hItem, TVGN_CARET | TVGN_FIRSTVISIBLE);
            m_Tree.SelectItem(hItem);
            m_Tree.Invalidate();
            CString strText = m_Tree.GetItemText(hItem);
            m_List.SetItemState(nListItemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            m_List.EnsureVisible(nListItemIndex, FALSE);
            m_List.SetFocus();
            m_pFindDlg->ShowWindow(SW_HIDE);
        }
    }

    return 0;
}

BOOL CVPKReaderDlg::FindPrevFileItem(HTREEITEM hStartItem, LPCTSTR szFilter, DWORD dwFlags, HTREEITEM& hItem, int& nListItemIndex)
{
    HTREEITEM hParentItem = hStartItem;
    do 
    {
        HTREEITEM hSiblingItem = m_Tree.GetPrevSiblingItem(hParentItem);
        while(hSiblingItem != NULL)
        {
            if(FindPrevFileItemHelper(hSiblingItem, szFilter, dwFlags, hItem, nListItemIndex))
                return TRUE;

            hSiblingItem = m_Tree.GetPrevSiblingItem(hSiblingItem);
        }
        hParentItem = m_Tree.GetParentItem(hParentItem);
        if(hParentItem != NULL)
        {
            if(MatchTreeItem(hParentItem, szFilter, dwFlags, hItem, nListItemIndex))
            {
                hItem = hParentItem;
                return TRUE;
            }
        }
    } while (hParentItem != NULL);

    return FALSE;
}

BOOL CVPKReaderDlg::FindPrevFileItemHelper(HTREEITEM hStartItem, LPCTSTR szFilter, DWORD dwFlags, HTREEITEM& hItem, int& nListItemIndex)
{
    CString strText = m_Tree.GetItemText(hStartItem);

    // enum childs
    HTREEITEM hChild = NULL;
    HTREEITEM hTemp = m_Tree.GetNextItem(hStartItem, TVGN_CHILD);
    while(hTemp != NULL)
    {
        hChild = hTemp;
        hTemp = m_Tree.GetNextSiblingItem(hTemp);
    }

    while(hChild != NULL)
    {
        if(FindPrevFileItemHelper(hChild, szFilter, dwFlags, hItem, nListItemIndex))
            return TRUE;
        hChild = m_Tree.GetPrevSiblingItem(hChild);
    }

    if(MatchTreeItem(hStartItem, szFilter, dwFlags, hItem, nListItemIndex))
    {
        hItem = hStartItem;
        return TRUE;
    }

    return FALSE;
}

BOOL CVPKReaderDlg::FindNextFileItem(HTREEITEM hStartItem, LPCTSTR szFilter, DWORD dwFlags, HTREEITEM& hItem, int& nListItemIndex)
{
    CString strText = m_Tree.GetItemText(hStartItem);
    // enum childs
    HTREEITEM hChild = m_Tree.GetNextItem(hStartItem, TVGN_CHILD);
    while(hChild != NULL)
    {
        if(FindNextFileItemHelper(hChild, szFilter, dwFlags, hItem, nListItemIndex))
            return TRUE;

        hChild = m_Tree.GetNextSiblingItem(hChild);
    }

    // enum siblings
    HTREEITEM hSiblingItem = m_Tree.GetNextSiblingItem(hStartItem);
    while(hSiblingItem != NULL)
    {
        if(FindNextFileItemHelper(hSiblingItem, szFilter, dwFlags, hItem, nListItemIndex))
            return TRUE;

        hSiblingItem = m_Tree.GetNextSiblingItem(hSiblingItem);
    }

    // enum parent's sibling
    HTREEITEM hParentItem = m_Tree.GetParentItem(hStartItem);
    if(hParentItem != NULL && MatchTreeItem(hParentItem, szFilter, dwFlags, hItem, nListItemIndex))
    {
        hItem = hParentItem;
        return TRUE;
    }

    while(hParentItem != NULL)
    {
        hSiblingItem = m_Tree.GetNextSiblingItem(hParentItem);
        while(hSiblingItem != NULL)
        {
            if(FindNextFileItemHelper(hSiblingItem, szFilter, dwFlags, hItem, nListItemIndex))
                return TRUE;
            hSiblingItem = m_Tree.GetNextSiblingItem(hSiblingItem);
        }
        hParentItem = m_Tree.GetParentItem(hParentItem);
    }

    return FALSE;
}

BOOL CVPKReaderDlg::FindNextFileItemHelper(HTREEITEM hStartItem, LPCTSTR szFilter, DWORD dwFlags, HTREEITEM& hItem, int& nListItemIndex)
{
    CString strText = m_Tree.GetItemText(hStartItem);
    if(MatchTreeItem(hStartItem, szFilter, dwFlags, hItem, nListItemIndex))
    {
        hItem = hStartItem;
        return TRUE;
    }

    // enum childs
    HTREEITEM hChild = m_Tree.GetNextItem(hStartItem, TVGN_CHILD);
    while(hChild != NULL)
    {
        if(MatchTreeItem(hChild, szFilter, dwFlags, hItem, nListItemIndex))
        {
            hItem = hChild;
            return TRUE;
        }

        if(FindNextFileItemHelper(hChild, szFilter, dwFlags, hItem, nListItemIndex))
            return TRUE;

        hChild = m_Tree.GetNextSiblingItem(hChild);
    }

    return FALSE;
}

BOOL CVPKReaderDlg::FindListFileItem(LPCTSTR szFilter, DWORD dwFlags, HTREEITEM& hItem, int& nListItemIndex)
{
    int nStart = 0;

    POSITION pos = m_List.GetFirstSelectedItemPosition();
    if(pos == NULL)
    {
        nStart = (dwFlags & Find_Next) ? 0 : m_List.GetItemCount() - 1;
    }
    else
    {
        nStart = m_List.GetNextSelectedItem(pos);
        nStart += (dwFlags & Find_Next) ? 1 : -1;
    }

    if(dwFlags & Find_Next)
    {
        int nEnd = m_List.GetItemCount();
        for(int i=nStart; i<nEnd; ++ i)
        {
            const FileInfo* pFile = (const FileInfo*)m_List.GetItemData(i);
            if(FileMatch(pFile, szFilter, dwFlags))
            {
                hItem = m_Tree.GetNextItem(hItem, TVGN_CARET);
                nListItemIndex = i;
                return TRUE;
            }
        }
    }
    else
    {
        for(int i=nStart; i>=0; -- i)
        {
            const FileInfo* pFile = (const FileInfo*)m_List.GetItemData(i);
            if(FileMatch(pFile, szFilter, dwFlags))
            {
                hItem = m_Tree.GetNextItem(hItem, TVGN_CARET);
                nListItemIndex = i;
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL CVPKReaderDlg::FileMatch(const FileInfo* pFileInfo, LPCTSTR szFilter, DWORD)
{
    return PathMatchSpec(pFileInfo->strFileName, szFilter);
}

void CVPKReaderDlg::ClearListSelection()
{
    POSITION pos = m_List.GetFirstSelectedItemPosition();
    if(pos == NULL)
        return;

    CArray<int> selectedItemIndex;
    selectedItemIndex.SetSize(m_List.GetSelectedCount());

    int nIndex = 0;
    while(pos)
    {
        int nItem = m_List.GetNextSelectedItem(pos);
        selectedItemIndex.SetAt(nIndex, nItem);
        ++ nIndex;
    }

    for(int i=0; i<selectedItemIndex.GetCount(); ++ i)
    {
        m_List.SetItemState(selectedItemIndex.GetAt(i), 0, LVIS_SELECTED | LVIS_FOCUSED);
    }
}

BOOL CVPKReaderDlg::MatchTreeItem(HTREEITEM hTreeItem, LPCTSTR szFilter, DWORD dwFlags, HTREEITEM&, int& nListItemIndex)
{
    CString strText = m_Tree.GetItemText(hTreeItem);
    CList<const FileInfo*>* pFiles = (CList<const FileInfo*>*)m_Tree.GetItemData(hTreeItem);
    if(pFiles == NULL)
        return FALSE;

    if(dwFlags & Find_Next)
    {
        POSITION pos = pFiles->GetHeadPosition();
        int nInfoCount = pFiles->GetCount();

        CString strTemp;
        for(int i=0; i<nInfoCount; ++ i)
        {
            const FileInfo* pInfo = pFiles->GetNext(pos);
            if(FileMatch(pInfo, szFilter, dwFlags))
            {
                nListItemIndex = i;
                return TRUE;
            }
        }
    }
    else
    {
        POSITION pos = pFiles->GetTailPosition();
        int nInfoCount = pFiles->GetCount();

        CString strTemp;
        for(int i=nInfoCount-1; i>=0; -- i)
        {
            const FileInfo* pInfo = pFiles->GetPrev(pos);
            if(FileMatch(pInfo, szFilter, dwFlags))
            {
                nListItemIndex = i;
                return TRUE;
            }
        }
    }
    return FALSE;
}

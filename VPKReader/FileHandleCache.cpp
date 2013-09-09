#include "StdAfx.h"
#include "FileHandleCache.h"

CFileHandleCache::CFileHandleCache(void)
{
}

CFileHandleCache::~CFileHandleCache(void)
{
}

CFileHandleCache& CFileHandleCache::instance()
{
    static CFileHandleCache inst;
    return inst;
}

HANDLE CFileHandleCache::GetFileHandle(LPCTSTR szFilePath)
{
    CString strFilePath(szFilePath);
    strFilePath.MakeLower();
    void* pHandle = NULL;
    if(m_mapFileHandles.Lookup(strFilePath, pHandle))
    {
        return (HANDLE)pHandle;
    }
    else
    {
        HANDLE hFile = ::CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
        {
            m_mapFileHandles.SetAt(strFilePath, (void*)hFile);
        }
        return hFile;
    }
}

void CFileHandleCache::Clear()
{
    CString strFilePath;
    void* pHandle = INVALID_HANDLE_VALUE;
    POSITION pos = m_mapFileHandles.GetStartPosition();
    while(pos != NULL)
    {
        m_mapFileHandles.GetNextAssoc(pos, strFilePath, pHandle);
        if(pHandle != INVALID_HANDLE_VALUE)
        {
            ::CloseHandle((HANDLE)pHandle);
        }
    }
    m_mapFileHandles.RemoveAll();
}


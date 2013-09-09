#pragma once

class CFileHandleCache
{
    CFileHandleCache(const CFileHandleCache&);
    CFileHandleCache& operator = (const CFileHandleCache&);
public:
    CFileHandleCache(void);
    ~CFileHandleCache(void);

    static CFileHandleCache& instance();

public:
    HANDLE GetFileHandle(LPCTSTR szFilePath);
    void Clear();

protected:
    CMapStringToPtr m_mapFileHandles;
};

#pragma once


class FileInfo
{
public:
    CString strFilePath;
    CString strFileName;
    unsigned short PreloadBytes;
    unsigned int PreloadOffset;
    unsigned short ArchiveIndex;
    unsigned int EntryOffset;
    unsigned int EntryLength;
};

typedef CList<FileInfo> FileInfoList;


enum VPKError
{
    VE_Ok,
    VE_Generic,
    VE_OpenFile,
    VE_UnexpectedEnd,
    VE_WrongVersion,
    VE_WrongSignature,
    VE_WrongTerminator,
};

LPCTSTR GetVPKError(VPKError error);

class VPKFile
{
public:
    VPKFile(void);
    ~VPKFile(void);

    VPKError Open(LPCTSTR szFilePath);

    DWORD    GetFileVersion() const;
    CString  GetFilePath() const;
    DWORD    GetPathCount() const;
    DWORD    GetExtCount() const;
    DWORD    GetTotalFileCount() const;
    DWORD    GetNoneEmptyFileCount() const;
    const FileInfoList& GetFileInfoList() const;

    void      Clear();
private:

    VPKError ParseHeader(HANDLE hFile);
    VPKError ParseFiles(HANDLE hFile);

    BOOL     ReadString(HANDLE hFile, CString& str, VPKError& vpkError);

private:
    DWORD   m_dwFileVersion;
    CString m_strFilePath;

    DWORD   m_dwExtCount;
    DWORD   m_dwPathCount;
    DWORD   m_dwTotalCount;
    DWORD   m_dwNoneEmptyCount;

    FileInfoList    m_FileInfoList;
};

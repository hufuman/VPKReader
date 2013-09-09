#include "StdAfx.h"
#include "VPKFile.h"



namespace
{

#define VPK_HEADER_SIGNATURE    0x55aa1234


#pragma pack(push, 1)
struct VPKHeader_v1
{
    unsigned int Signature;
    unsigned int Version;
    unsigned int TreeLength; // The length of the directory
};

struct VPKHeader_v2
{
    unsigned int Signature;
    unsigned int Version;
    unsigned int TreeLength; // The length of the directory tree

    int Unknown1; // 0 in CSGO
    unsigned int FooterLength;
    int Unknown3; // 48 in CSGO
    int Unknown4; // 0 in CSGO
};

struct VPKDirectoryEntry
{
    unsigned int CRC; // A 32bit CRC of the file's data.
    unsigned short PreloadBytes; // The number of bytes contained in the index file.

    // A zero based index of the archive this file's data is contained in.
    // If 0x7fff, the data follows the directory.
    unsigned short ArchiveIndex;

    // If ArchiveIndex is 0x7fff, the offset of the file data relative to the end of the directory (see the header for more details).
    // Otherwise, the offset of the data from the start of the specified archive.
    unsigned int EntryOffset;

    // If zero, the entire file is stored in the preload data.
    // Otherwise, the number of bytes stored starting at EntryOffset.
    unsigned int EntryLength;

    unsigned short Terminator;
};
#pragma pack(pop)


BOOL ReadFileEntry(HANDLE hFile, VPKDirectoryEntry& entry)
{
    DWORD dwRead;
    if(::ReadFile(hFile, (LPVOID)&entry, sizeof(entry), &dwRead, 0) && dwRead == sizeof(entry))
        return TRUE;
    else
        return FALSE;
}

}



LPCTSTR GetVPKError(VPKError error)
{

    struct
    {
        VPKError error;
        LPCTSTR  msg;
    } data[] =
    {
        {VE_Ok,             _T("Ok")},
        {VE_Generic,        _T("Unknown error")},
        {VE_OpenFile,       _T("Failed to open file")},
        {VE_UnexpectedEnd,  _T("Encountered end of file")},
        {VE_WrongVersion,   _T("Version of VPK file is not 1 or 2")},
        {VE_WrongSignature, _T("This file has different signature")},
        {VE_WrongTerminator,_T("File info is not terminated with the right signature")},
    };
    for(int i=0; i<_countof(data); ++ i)
    {
        if(data[i].error == error)
            return data[i].msg;
    }

    return _T("Fucked Error");
}


VPKFile::VPKFile(void)
{
}

VPKFile::~VPKFile(void)
{
    Clear();
}

void VPKFile::Clear()
{
    m_FileInfoList.RemoveAll();
}

VPKError VPKFile::Open(LPCTSTR szFilePath)
{
    Clear();

    HANDLE hFile = ::CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        return VE_OpenFile;
    }

    m_strFilePath = szFilePath;

    VPKError vpkError = ParseHeader(hFile);

    if(vpkError == VE_Ok)
    {
        vpkError = ParseFiles(hFile);
    }

    ::CloseHandle(hFile);
    return vpkError;
}

DWORD VPKFile::GetFileVersion() const
{
    return m_dwFileVersion;
}

CString VPKFile::GetFilePath() const
{
    return m_strFilePath;
}

DWORD VPKFile::GetPathCount() const
{
    return m_dwPathCount;
}

DWORD VPKFile::GetExtCount() const
{
    return m_dwExtCount;
}

DWORD VPKFile::GetTotalFileCount() const
{
    return m_dwTotalCount;
}

DWORD VPKFile::GetNoneEmptyFileCount() const
{
    return m_dwNoneEmptyCount;
}

const FileInfoList& VPKFile::GetFileInfoList() const
{
    return m_FileInfoList;
}

VPKError VPKFile::ParseHeader(HANDLE hFile)
{
    DWORD dwRead = 0;
    VPKHeader_v1 vpkHeader = {0};
    int reserved[4];

    if(::ReadFile(hFile, (LPVOID)&vpkHeader, sizeof(vpkHeader), &dwRead, NULL) && dwRead == sizeof(vpkHeader))
    {
        if(vpkHeader.Signature != VPK_HEADER_SIGNATURE)
            return VE_WrongSignature;

        if(vpkHeader.Version == 2)
        {
            if(::ReadFile(hFile, (LPVOID)reserved, sizeof(reserved), &dwRead, NULL) && dwRead == sizeof(reserved))
            {
                m_dwFileVersion = vpkHeader.Version;
                return VE_Ok;
            }
            else
            {
                return VE_UnexpectedEnd;
            }
        }
        else if(vpkHeader.Version == 1)
        {
            m_dwFileVersion = vpkHeader.Version;
            return VE_Ok;
        }
        else
        {
            return VE_WrongVersion;
        }
    }
    else
    {
        return VE_UnexpectedEnd;
    }
}

BOOL VPKFile::ReadString(HANDLE hFile, CString& str, VPKError& vpkError)
{
    char ch;
    DWORD dwRead;

    str = "";
    vpkError = VE_Generic;

    while(TRUE)
    {
        if(::ReadFile(hFile, &ch, 1, &dwRead, 0) && dwRead == 1)
        {
            if(ch == 0)
            {
                vpkError = VE_Ok;
                return !str.IsEmpty();
            }
            str += ch;
        }
        else
        {
            vpkError = VE_UnexpectedEnd;
            return FALSE;
        }
    }
}

VPKError VPKFile::ParseFiles(HANDLE hFile)
{
    CString extention;
    CString path;
    CString filename;
    CString fullpath;

    VPKDirectoryEntry entry;

    m_dwExtCount = 0;
    m_dwPathCount = 0;
    m_dwTotalCount = 0;
    m_dwNoneEmptyCount = 0;

    VPKError vpkError = VE_Ok;

    while(vpkError == VE_Ok)
    {
        if(!ReadString(hFile, extention, vpkError))
            break;

        ++ m_dwExtCount;
        while(vpkError == VE_Ok)
        {
            if(!ReadString(hFile, path, vpkError))
                break;

            ++ m_dwPathCount;
            while(vpkError == VE_Ok)
            {
                if(!ReadString(hFile, filename, vpkError))
                    break;

                ++ m_dwTotalCount;
                if(ReadFileEntry(hFile, entry))
                {
                    FileInfo info;
                    info.strFilePath = path;
                    info.strFileName = filename + _T(".") + extention;
                    info.PreloadOffset = 0;

                    if(entry.EntryLength + entry.PreloadBytes > 0)
                        ++ m_dwNoneEmptyCount;

                    if(entry.ArchiveIndex == 0x7FFF)
                    {
                        DWORD dwCurPos = ::SetFilePointer(hFile, 0, 0, FILE_CURRENT);
                        info.EntryOffset += dwCurPos + entry.EntryOffset + entry.PreloadBytes;
                    }
                    else
                    {
                        info.EntryOffset = entry.EntryOffset;
                    }

                    if(entry.ArchiveIndex == 0x7FFF)
                    {
                        ::SetFilePointer(hFile, entry.EntryOffset + entry.EntryLength + entry.PreloadBytes, NULL, FILE_CURRENT);
                    }
                    else if(entry.PreloadBytes > 0)
                    {
                        info.PreloadOffset = ::SetFilePointer(hFile, 0, 0, FILE_CURRENT);
                        ::SetFilePointer(hFile, entry.PreloadBytes, NULL, FILE_CURRENT);
                    }

                    if(entry.Terminator == 0xFFFF)
                    {
                        info.PreloadBytes = entry.PreloadBytes;
                        info.ArchiveIndex = entry.ArchiveIndex;
                        info.EntryLength = entry.EntryLength;

                        m_FileInfoList.AddTail(info);
                    }
                    else
                    {
                        vpkError = VE_WrongTerminator;
                    }
                }
            }
        }
    }

    return vpkError;
}




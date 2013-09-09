#pragma once

namespace FileUtil
{
    BOOL MakeDirectory(LPCTSTR szPath);
    BOOL WriteFile(HANDLE hSrcFile, HANDLE hDstFile, DWORD dwStartPos, DWORD dwCountBytes);

    class CFileWriteHandle
    {
        CFileWriteHandle(const CFileWriteHandle&);
        CFileWriteHandle& operator = (const CFileWriteHandle&);
    public:
        explicit CFileWriteHandle(HANDLE hFile)
        {
            m_hFile = hFile;
        }

        CFileWriteHandle()
        {
            m_hFile = INVALID_HANDLE_VALUE;
        }

        ~CFileWriteHandle()
        {
            if(m_hFile != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(m_hFile);
                m_hFile = INVALID_HANDLE_VALUE;
            }
        }

        operator HANDLE()
        {
            return m_hFile;
        }

    private:
        HANDLE m_hFile;
    };
};

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
        explicit CFileWriteHandle(LPCTSTR szFilePath, BOOL bDeleteOnDestructor)
        {
            m_strFilePath = szFilePath;
            m_hFile = ::CreateFile(szFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            m_bDeleteOnDestructor = bDeleteOnDestructor;
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
                if(m_bDeleteOnDestructor)
                {
                    ::DeleteFile(m_strFilePath);
                }
            }
        }

        void Close()
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
        BOOL    m_bDeleteOnDestructor;
        HANDLE  m_hFile;
        CString m_strFilePath;
    };
};

#include "StdAfx.h"
#include "FileUtil.h"

namespace FileUtil
{

    BOOL MakeDirectory(LPCTSTR szPath)
    {
        CString strToken;
        CString strOrgPath(szPath);
        CString strPath;

        int tok = 0;
        strToken = strOrgPath.Tokenize(_T("\\"), tok);
        while(!strToken.IsEmpty())
        {
            strPath += strToken + _T("\\");
            if(::GetFileAttributes(strPath) == INVALID_FILE_ATTRIBUTES)
            {
                if(!::CreateDirectory(strPath, NULL))
                    return FALSE;
            }
            strToken = strOrgPath.Tokenize(_T("\\"), tok);
        }

        return (::GetFileAttributes(szPath) != INVALID_FILE_ATTRIBUTES);
    }

    BOOL WriteFile(HANDLE hSrcFile, HANDLE hDstFile, DWORD dwStartPos, DWORD dwCountBytes)
    {
        ::SetFilePointer(hSrcFile, dwStartPos, NULL, FILE_BEGIN);
        DWORD dwRemainBytes = dwCountBytes;
        DWORD dwWritten = 0;
        DWORD dwRead = 0;
        DWORD dwExpectBytes = 0;
        BYTE byBuffer[1024];
        while(dwRemainBytes > 0)
        {
            dwExpectBytes = (dwRemainBytes > sizeof(byBuffer)) ? sizeof(byBuffer) : dwRemainBytes;
            if(::ReadFile(hSrcFile, byBuffer, dwExpectBytes, &dwRead, NULL) && dwExpectBytes == dwRead)
            {
                if(::WriteFile(hDstFile, byBuffer, dwExpectBytes, &dwWritten, NULL) && dwExpectBytes == dwWritten)
                {
                    dwRemainBytes -= dwExpectBytes;
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        return (dwRemainBytes == 0);
    }
}

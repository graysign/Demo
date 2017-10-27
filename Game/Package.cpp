#include "StdAfx.h"
#include "Package.h"

CPackage::CPackage(void)
{
    m_bufPackage      =   NULL;
    m_nCurrentLength  =   0;
    m_nLength         =   0;
}

CPackage::~CPackage(void)
{
}

void        
CPackage::PushReceivePackage(int nRead, const uv_buf_t* buf)
{
    if (m_nLength == 0  && m_nCurrentLength == 0 && buf->len >= sizeof(int))
    {
        char pszTmp[4]  =   {0};
        strcpy_s(pszTmp, sizeof(pszTmp), buf->base);
        m_nLength           =   htonl(atoi(pszTmp));
        m_nCurrentLength    =   nRead   -   4;

        m_bufPackage        =   new uv_buf_t();
        m_bufPackage->base  =   (char*)malloc(m_nLength);
        m_bufPackage->len   =   m_nLength;
        strcpy_s(m_bufPackage->base, m_nCurrentLength, buf->base + 4);

    }else{
        strcpy_s(m_bufPackage->base + m_nCurrentLength, nRead, buf->base);
        m_nCurrentLength    +=   nRead;
    }
    if (m_nCurrentLength == m_nLength)
    {
        m_nCurrentLength    =   0;
        m_nLength           =   0;
        m_queueReceivePackages.push(m_bufPackage);
        m_bufPackage        =   NULL;
    }

}

uv_buf_t*   
CPackage::PopReceivePackage()
{
   if (m_queueReceivePackages.empty())
   {
       return NULL;
   }

   uv_buf_t* pBufTmp    =   m_queueReceivePackages.front();
   m_queueReceivePackages.pop();
   return pBufTmp;
}
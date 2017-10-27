#ifndef _PACKAGE_H_
#define _PACKAGE_H_

#include <queue>
#include "stdafx.h"
#include "uv.h"
class CPackage
{
public:
    CPackage(void);
    ~CPackage(void);
public:
    void        PushReceivePackage(int nRead, const uv_buf_t* buf);
    uv_buf_t*   PopReceivePackage();
    std::queue< uv_buf_t* >        GetPackages(){return m_queueReceivePackages;}
private:
    int         m_nLength;
    int         m_nCurrentLength;
    uv_buf_t*   m_bufPackage;
    std::queue< uv_buf_t* > m_queueReceivePackages;
    std::queue< uv_buf_t* > m_queueSendPackages;
};


#endif


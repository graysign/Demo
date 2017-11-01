#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_
#include <xstring>
#include <map>
#include "uv.h" 
class CTCPServer
{
public:
    CTCPServer(int nPort, std::string ip = "0.0.0.0");
    ~CTCPServer(void);

    int    Run();
private:
    static void OnConnection(uv_stream_t *server, int status);
    static void AllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void Read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf); 

    static void RemoteConnectCb(uv_connect_t* req, int status);
    static void RemoteRead(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf); 


    static void AfterWrite(uv_write_t *req, int status);
private:
    static void* GetRemoteConnect(uv_tcp_t *client );
private:
    static int  nPackageLength;
    uv_loop_t*  m_loop;
    uv_tcp_t    m_server;
    sockaddr_in m_addr;
 
};

#endif



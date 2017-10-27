#include "StdAfx.h"
#include "TCPServer.h"

int  CTCPServer::nPackageLength  =   1460;
std::map<uv_tcp_t*, CPackage*> CTCPServer::m_mapPackage;
CTCPServer::CTCPServer(int nPort, std::string ip)
{ 
    
    m_loop    =   uv_default_loop(); 
    uv_tcp_init(m_loop, &m_server); 
    uv_ip4_addr(ip.c_str(), nPort, &m_addr);
    uv_tcp_bind(&m_server, (const struct sockaddr*)&m_addr, 0);
    int r = uv_listen((uv_stream_t*) &m_server, 1024, OnConnection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
    }

    uv_thread_t hare_id;
    uv_thread_create(&hare_id, SendMsgThread, NULL);

}

CTCPServer::~CTCPServer(void)
{
}

int
CTCPServer::Run()
{
    return uv_run(m_loop, UV_RUN_DEFAULT);
}


void 
CTCPServer::OnConnection(uv_stream_t *server, int status)
{
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), client);

    m_mapPackage.insert( std::map<uv_tcp_t*, CPackage*>::value_type(client, new CPackage()) );
    //m_mapPackage[client]    =   new CPackage();
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*) client, AllocBuffer, Read);
    }
    else {
        uv_close((uv_handle_t*) client, NULL);
    }
}

void 
CTCPServer::AllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) 
{
    buf->base = (char*)malloc(nPackageLength);
    buf->len = nPackageLength;
    printf("AllocBuffer Length[%d] \n", nPackageLength);
}
void 
CTCPServer::Read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) 
{
    printf("Read Length[%d] \n", nread);
    if (nread > 0) {
        std::map<uv_tcp_t*, CPackage*>::iterator  it =   m_mapPackage.find((uv_tcp_t*)client);
        if (it != m_mapPackage.end())
        {
            CPackage* pPackage  =  const_cast<CPackage*>(it->second) ;
            pPackage->PushReceivePackage(nread, buf);
        } 
        return;
    }
    if (nread < 0) {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t*) client, NULL);
    }

    free(buf->base);
}

typedef struct {
    uv_write_t req;
    uv_buf_t* buf;
} write_req_t;


void 
CTCPServer::Write(uv_write_t *req, int status) {
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
    free(((write_req_t*)req)->buf->base);
    delete(((write_req_t*)req)->buf);
    free(req);
}

void 
CTCPServer::SendMsgThread(void *arg) {
   while(true){
       for (std::map<uv_tcp_t*, CPackage*>::iterator it = m_mapPackage.begin(); it != m_mapPackage.end() ; it++)
       {
           uv_stream_t* client      =   (uv_stream_t*)it->first;
            CPackage*   pPackages   =   it->second;
            while(!pPackages->GetPackages().empty()){
                uv_buf_t*   buf  =  pPackages->PopReceivePackage();
                write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
                req->buf         =  buf;
            
                uv_write((uv_write_t*) req, client, buf, buf->len, Write);
           }
       }
       Sleep(100);    
    }
}


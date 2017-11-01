#include "StdAfx.h"
#include "TCPServer.h"

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

void free_write_req(uv_write_t *req) {
    write_req_t *wr = (write_req_t*) req;
    free(wr->buf.base);
    free(wr);
}


int  CTCPServer::nPackageLength  =   1460; 
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
 
    //m_mapPackage[client]    =   new CPackage();
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        GetRemoteConnect(client);
    }
    else {
        uv_close((uv_handle_t*) client, NULL);
    }
}

void 
CTCPServer::AllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) 
{
    buf->base = (char*)malloc(suggested_size);
    buf->len = suggested_size;
    //printf("AllocBuffer Length[%d] \n", nPackageLength);
}
void 
CTCPServer::Read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) 
{
    //printf("Read Length[%d] \n", nread);
    if (nread > 0) {
        //printf("%s\n", buf->base);
        write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
        req->buf = uv_buf_init(buf->base, nread);
        uv_write((uv_write_t*) req, (uv_stream_t *)client->data, &req->buf, 1, AfterWrite);  
        return;
    }
    if (nread < 0) {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
          uv_close((uv_handle_t*) client->data, NULL);
        uv_close((uv_handle_t*) client, NULL);
    }
    free(buf->base);
}

 
 

void*   
CTCPServer::GetRemoteConnect(uv_tcp_t *client )
{
    sockaddr_in bind_addr;  
    uv_ip4_addr("120.27.124.254", 80, &bind_addr); 
   
    uv_tcp_t*       remote  =   (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_connect_t*   connect =   (uv_connect_t*)malloc(sizeof(uv_connect_t));
    uv_tcp_init(uv_default_loop(),remote);  
    remote->data            =    client;
    client->data            =    remote;
    uv_tcp_connect(connect, remote, (const struct sockaddr*)&bind_addr, RemoteConnectCb);
    return remote;
}
void 
CTCPServer::RemoteConnectCb(uv_connect_t* req, int status)
{
   uv_read_start((uv_stream_t*)req->handle->data, AllocBuffer, Read);
   uv_read_start((uv_stream_t*) ((uv_tcp_t*)req->handle->data)->data, AllocBuffer, Read);

   /* char* buffer = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUpgrade-Insecure-Requests: 1\r\nAccept: text/html\r\n\r\n";
    uv_buf_t buf = uv_buf_init(buffer,strlen(buffer));


    uv_stream_t* tcp = req->handle; 
    uv_write_t* write_req   =   (uv_write_t*)malloc(sizeof(uv_write_t));  
    uv_write(write_req,tcp,&buf, 1, AfterWrite); */
}

 

void 
CTCPServer::AfterWrite(uv_write_t *req, int status)
{
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
    free_write_req(req);
}
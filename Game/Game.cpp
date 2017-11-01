// Game.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TCPServer.h"
uv_loop_t* _loop    =   NULL;

void alloc_buffer1(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) 
{
    buf->base = (char*)malloc(1460);
    buf->len = 1460; 
}

void read_cb1(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) 
{
    printf("Read Length[%d] \n", nread);
    if (nread > 0) { 
        printf("%s\n", buf->base); 
    }
    if (nread < 0) {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t*) client, NULL);
    }
    free(buf->base);
}


void write_callback(uv_write_t *req,int status)
{
    if(status == -1)
    { 
        return;
    }
    uv_read_start(req->handle,alloc_buffer1,read_cb1);
 
}

void connect_server(uv_connect_t *req,int status)
{ 
    char* buffer = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUpgrade-Insecure-Requests: 1\r\nAccept: text/html\r\n\r\n";
    uv_buf_t buf = uv_buf_init(buffer,strlen(buffer));

     
    uv_stream_t* tcp = req->handle;

    uv_write_t* write_req   =   (uv_write_t*)malloc(sizeof(uv_write_t));  
    uv_write(write_req,tcp,&buf, 1, write_callback); 
}


int _tmain(int argc, _TCHAR* argv[])
{
    CTCPServer server(80);
    server.Run();; 

    //_loop = uv_default_loop();
    //uv_tcp_t socket;
    //uv_tcp_init(_loop,&socket);
    //uv_connect_t connect;  
    //struct sockaddr_in dest; ;
    //uv_ip4_addr("120.27.124.254", 80, &dest);  

    //uv_tcp_connect(&connect,&socket,(const struct sockaddr*)&dest,connect_server);

    //uv_run(_loop,UV_RUN_DEFAULT);
    return 0;
}



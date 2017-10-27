// Game.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TCPServer.h"
int _tmain(int argc, _TCHAR* argv[])
{
    CTCPServer server(1444);
    return server.Run();; 
}


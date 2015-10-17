
#ifndef NETWORK_STDINCLUDES_HPP
#define NETWORK_STDINCLUDES_HPP

//------------------------------------------------
/**
    includes standards du reseau
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL) || defined(USES_WINDOWS8_METRO)
    #include <io.h>
    #include <winsock2.h>
    #ifdef MSVC
        #pragma comment(lib, "ws2_32")
    #endif
    #pragma pack(1)
    #define WIN32_LEAN_AND_MEAN
    #define socklen_t int
#elif defined(USES_LINUX)
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <fcntl.h>
#else
	#error
#endif


//------------------------------------------------

static void bzeroStructure(void *lp_structure,int size)
{
#ifdef WIN32
    for (int i=0;i<size;i++)
        ((char *)lp_structure)[i] = 0;
#else
	bzero(lp_structure,size);
#endif
}

static void setNonBlockSocket(int sock)
{
#ifdef WIN32
    int var = 1;
    ioctlsocket(sock, FIONBIO, (u_long FAR*) &var);
#else
    fcntl(sock, F_SETFL, O_NONBLOCK);
#endif
}

#endif /*NETWORK_STDINCLUDES_HPP*/

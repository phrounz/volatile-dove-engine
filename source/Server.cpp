#ifndef USES_WINDOWS8_METRO
/******************************************************************
Un serveur reseau.
Done by Francois Braud
******************************************************************/

//------------------------------------------------
/**
    includes
*/

//includes standards
//#include <pthread.h>
#include <vector>

//includes locaux
#include "network_stdincludes.h"
#include "../include/Thread.h"
#include "../include/Utils.h"

#include "Server.h"

namespace
{
	void ErrorRelease(const char* str) { AssertMessage(false, str); }
}

//------------------------------------------------
/**
    Methodes et objets statiques
*/

static Server* handleSignalObject;
void* serverWorkWrapper(void *arg)
{
  void *result = handleSignalObject->work(arg);

  handleSignalObject->hasFinishedMainThread = true;

  return result;
}

void* serverResponseWrapper(void *arg)
{
    return handleSignalObject->response(arg);
}


static std::string receiveLine(int socket, bool verbose)
{
  std::string ret;

  while (1) {
    char r;

    switch(recv(socket, &r, 1, 0)) {
      case 0:return ret;
      case -1:return "";
    }

    if (r=='\0') return ret;

    ret += r;
    if (r == '\n')
    {
        if (verbose) std::cout << "readLine: " << ret.c_str();
        return ret;
    }
  }
}

static int bind2(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen)
{
    int r;
	if ((r = bind(sockfd,my_addr,addrlen)) != 0)
	{
	    std::string errMess("pb on bind for Server : ");
#ifdef WIN32
        r = WSAGetLastError();
        switch(r)
	    {
	        case WSAEADDRINUSE: errMess += "WSAEADDRINUSE";break;
	        case WSAEADDRNOTAVAIL: errMess += "WSAEADDRNOTAVAIL";break;
	        case WSAEINPROGRESS: errMess += "WSAEINPROGRESS";break;
	        case WSAEINVAL: errMess += "WSAEINVAL";break;
	    }
#else
	    switch(r)
	    {
	        case EADDRINUSE: errMess += "EADDRINUSE";break;
	        case EADDRNOTAVAIL: errMess += "EADDRNOTAVAIL";break;
	        case EBADF: errMess += "EBADF";break;
	        case EINVAL: errMess += "EINVAL";break;
	    }
#endif
        ErrorRelease(errMess.c_str());//(void *)
        return -1;
	}
	return 0;
}

//------------------------------------------------
/**
    Methodes
*/

void* Server::response(void *lpSocket)
{
    currentNbResponses++;

	int *lpsocketReponse = (int *)lpSocket;
	int sock = *lpsocketReponse;
	if (verbose)
        std::cout << "Reading client message ... " << std::endl;

/*
    char *str=new char[200];
    str[0]='p';str[1]='r';str[2]='o';str[3]='u';str[4]='t';
    str[5]='u';str[6]='t';str[7]='o';str[8]='t';str[9]='u';
    send(socketReponse, str,sizeof(char)*10,0);
*/
    //lit jusqu'a une ligne vide ou un \0
    std::string readedText;
    while (true)
    {
        std::string tmp = receiveLine(sock,verbose);
        //std::cout << "blu:"<< tmp<< ":" << std::endl;
        if (tmp.empty()) break;
        readedText += tmp.c_str();
    }

    //la fonction de reponse pour interpretation interprete le message du client
    std::string *textToSend = serverResponseFunction(readedText.c_str());

    //cree le header
    char* sizeTextToSend = Utils::itoa(textToSend->size());
    std::string header("HTTP/1.1 200 OK\r\nContent-Length: ");
    header += sizeTextToSend;
    header += "\r\n";
    header += "Connection: close\r\n";
    header += "Content-Type: text/html\r\n";
    header += "\r\n";
    delete sizeTextToSend;

    //envoie la reponse au client
    if (verbose) std::cout << "   Responding to client ... " << std::flush;
    send(sock, header.c_str(),header.size(),0);
    send(sock, textToSend->c_str(),textToSend->size()+1,0);
    if (verbose) std::cout << "done." << std::endl;

    //supprime la chaine temporaire
    delete textToSend;

    //ferme le socket
	#ifdef _MSC_VER
		_close(sock);
	#else
		close(sock);
	#endif

    //met la valeur du socket a -1 pour dire qu'il peut etre reutilise
	*lpsocketReponse = -1;

    if (verbose)
        std::cout << "done." << std::endl;

    currentNbResponses = 0;

	return NULL;
}

typedef struct struct_InfoForWork
{
    struct sockaddr_in socketAdress;
    int listenSocket;
    int sizeOfSocketAdress;
} InfoForWork;

//Thread principal du serveur, appelle par le constructeur.
void* Server::work(void *infoForWorkHidden2)
{
    InfoForWork *arg = (InfoForWork *)infoForWorkHidden2;

    // rajouts pour la gestion des threads
	int* newSocket = new int[maxClients];

	int i;
	for (i=0;i<maxClients;i++)
		newSocket[i]=-1;

    while(!hasFinished) //boucle infinie
    {
        for (i=0;i<maxClients;i++) //recherche d'une socket libre
        {
            if (newSocket[i]==-1)
                break;
        }
        if (i==maxClients) continue; //trop de sockets ouverts, on attend

        while (newSocket[i] == -1)
        {
            Utils::sleepMs(reactTime);
            newSocket[i] = accept(arg->listenSocket,(sockaddr*)&(arg->socketAdress),(socklen_t*)&(arg->sizeOfSocketAdress));
            if (hasFinished)
                return NULL;
        }

        if (verbose)
            std::cout << "Accepting client ..." << std::flush;

        AssertMessage(newSocket[i] != -1, "pb on accept");

        if (verbose)
            std::cout << "creating a new thread ("<< i << ")." << std::endl;

		Threading::Thread::createThread(serverResponseWrapper,&newSocket[i]);
    }

	delete [] newSocket;

    return NULL;
}

Server::Server(
    std::string *(*serverResponseFunction_)(const char *), int port, int reactTime_, int maxClients_, bool verbose_)
:
    hasFinished(false),
    hasFinishedMainThread(false),
    verbose(verbose_),
    reactTime(reactTime_),
    maxClients(maxClients_),
    serverResponseFunction(serverResponseFunction_),
    currentNbResponses(0)
{
    if (verbose)
        std::cout << "Creating server ..." << std::flush;

    InfoForWork *arg = new InfoForWork();
    infoForWorkHidden = (void *)arg;

    //----------------------
	// Initialise le "struct sockaddr_in"
	//----------------------

	bzeroStructure((void *)&(arg->socketAdress),sizeof(struct sockaddr_in));
//	bzero((void *)&myInAdress,sizeof(struct in_addr));

	(arg->socketAdress).sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("127.0.0.1");
	AssertMessage((arg->socketAdress).sin_addr.s_addr != INADDR_NONE,"pb sur inet_addr");

	(arg->socketAdress).sin_family = AF_INET;
	(arg->socketAdress).sin_port = htons(port);
	//socketAdress.sin_addr = myInAdress;
	//socketAdress.sin_addr = INADDR_ANY;



    arg->sizeOfSocketAdress = sizeof(arg->socketAdress);

	//----------------------
	// Create a SOCKET for listening for
	// incoming connection requests
	//----------------------
	arg->listenSocket = socket(AF_INET, SOCK_STREAM, 0);//IPPROTO_TCP
	//setsockopt(arg->listenSocket, SOL_SOCKET, SO_REUSEADDR, 0, 0);
    //definit "accept" comme non bloquant pour cette socket
    setNonBlockSocket(arg->listenSocket);

	AssertMessage(arg->listenSocket >= 0, "cannot create listen socket");

	//----------------------
	// processus de connexion
	//----------------------

    bind2(arg->listenSocket,(const sockaddr*)&(arg->socketAdress),(arg->sizeOfSocketAdress));

	if (listen(arg->listenSocket,maxClients) != 0)
        ErrorRelease("pb on listen for Server");

	//----------------------
	// Lancement du thread principal
	//----------------------

    handleSignalObject = this;
	Threading::Thread::createThread(serverWorkWrapper,(void *)arg);

    if (verbose)
        std::cout << "done." << std::endl;
}

Server::~Server()
{
    if (verbose)
        std::cout << "Closing server ..." << std::flush;

    //cast
    InfoForWork *arg = (InfoForWork *)infoForWorkHidden;

    //informe le thread serveur qu'il doit se finir
    hasFinished = true;
    //attend la fin du thread serveur
    while(!hasFinishedMainThread && currentNbResponses>0);

	// Fermeture de la socket
	#ifdef _MSC_VER
		_close(arg->listenSocket);
	#else
		close(arg->listenSocket);
	#endif

	delete arg;

	if (verbose)
        std::cout << "done." << std::endl;
}

#endif //ndef USES_WINDOWS8_METRO

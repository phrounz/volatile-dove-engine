#ifndef USES_WINDOWS8_METRO

//includes standards
#include "network_stdincludes.h"

//includes locaux
#include "../include/Network.h"
#include "../include/Utils.h"

//------------------------------------------------
/**
    Constructeur/Destructeur
*/

#ifdef WIN32
int nbNetworks = 0;
#endif

Network::Network()
{
#ifdef WIN32
    if (nbNetworks == 0)
    {
        WSADATA	wsaData ;// initialisation des variables
        int res = WSAStartup(MAKEWORD(2 ,0), &wsaData);
        Assert(res == 0);

        nbNetworks++;
    }
#endif
}

Network::~Network()
{
#ifdef WIN32
    if (nbNetworks>0)
    {
        WSACleanup();

        nbNetworks--;
    }
#endif
}

//---------------------------------------------------------------------
/**
    Renvoie l'ip locale (sous la forme de chiffres separes par des points).
*/

std::string Network::getLocalIp()
{
    char ac[80];
#ifdef WIN32
    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR)
#else
    if (gethostname(ac, sizeof(ac)) == -1)
#endif
    {
        std::string errMessage("Error ");
#ifdef WIN32
        errMessage += WSAGetLastError();
#endif
        errMessage += " when getting local host name.";

        AssertMessage(false, errMessage.c_str());
    }
    //std::cout << "Host name is " << ac << "." << std::endl;

    struct hostent *phe = gethostbyname(ac);
    if (phe == 0)
        AssertMessage(false, "Yow! Bad host lookup.");

    std::string str("");
    for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        //std::cout << "Address " << i << ": " << inet_ntoa(addr) << std::endl;
        str = inet_ntoa(addr);//break;
    }

    return str;
}

//---------------------------------------------------------------------
/**
    Renvoie directement une page a partir d'un serveur donne.
    Renvoie NULL si le serveur est inaccessible.
    \param url Url complete, exemple : "http://www.machin.com/index.php"
    \param postArguments Arguments post. Mettre NULL si passage en GET.
    \param crashIfUnreachable Affiche une erreur et quitte le programme si le serveur est inaccessible.
*/

std::string *Network::getPageFromWebServer(
    const char *url,
    const Connection::PostArgumentListType* postArguments,
	const char* bufferPutData, 
	long long int bufferPutLength, 
    bool crashIfUnreachable,
	const std::string& method, 
    bool verbose)
{
    std::string *resultString = NULL;
    int i=0;
    if (strlen(url)<7 || strncmp(url,"http://",7)!=0)
    {
        AssertMessage(false, "getPageFromWebServer: unknown protocol, url not beginning by http://");
    }
    i=7;
    while (url[i]!='/' && url[i]!='\0')
    {
        i++;
    }

    if (url[i]=='\0')
    {
        char tmp[2];tmp[0]='/';tmp[1]='\0';
        Connection* c = createConnection(&(url[7]), false, 80, crashIfUnreachable, verbose);
        if (c != NULL)
        {
            c->connectToServer();
			resultString = c->getHTTPPage(tmp, postArguments, bufferPutData, bufferPutLength, false, method);
            delete c;
        }
    }
    else
    {
        char *tmp = Utils::copyFragmentOfStringIntoNewString(url,7,i);
        Connection* c = createConnection(tmp, false, 80, crashIfUnreachable, verbose);
        if (c != NULL)
        {
            c->connectToServer();
            resultString = c->getHTTPPage(&(url[i]), postArguments, bufferPutData, bufferPutLength, false, method);
            delete c;
        }
    }

    return resultString;
}

//---------------------------------------------------------------------
/**
    cree un nouvel objet Connection. Renvoie NULL si le host est inaccessible.
    \param hostName nom de l'host, exemple: cnn.com
    \param isIpAddress est-ce que le parametre hostName fourni
        est le nom de l'host ou bien plutot l'adresse ip
    \param port_ Le port reseau. 80 pour le http
    \param crashIfUnreachable Affiche une erreur et quitte le programme si site pas accessible
*/
Connection *Network::createConnection(const char *hostName,bool isIpAddress, int port, bool crashIfUnreachable, bool verbose)
{
    std::string errorMessages("");
    Connection* connection = new Connection(hostName, isIpAddress, port, errorMessages, verbose);
    if (strcmp(errorMessages.c_str(),"") != 0)
    {
        if (crashIfUnreachable)
        {
            AssertMessage(false, errorMessages.c_str());
        }
        else
        {
            delete connection;
            return NULL;
        }
    }
    else
        return connection;
	return NULL;
}

//---------------------------------------------------------------------
/**
    cree un nouvel objet Server (serveur).
    \param port_ Le port reseau. 80 pour le http
    \param reactTime Temps maximum de reaction du serveur chaque fois
        qu'un client se connecte. Une valeur faible fera que le thread
        serveur prendra plus de CPU.
    \param Nombre de clients maximum pouvant se connecter au serveur en simultane.
*/
Server *Network::createServer(
    std::string *(*serverResponseFunction)(const char *),
    int port, int reactTime, int maxClients, bool verbose)
{
    return new Server(serverResponseFunction, port, reactTime, maxClients, verbose);
}

#endif //ndef USES_WINDOWS8_METRO

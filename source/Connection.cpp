#ifndef USES_WINDOWS8_METRO
/******************************************************************
Une connexion client
Done by Francois Braud
******************************************************************/

//------------------------------------------------
/**
    includes
*/

#include <cstdio>
#include <cstring>
#include <cerrno>

//includes standards
#include "network_stdincludes.h"

//includes locaux
#include "Connection.h"

#include "../include/Utils.h"

namespace
{
	void ErrorRelease(const char* str) { AssertMessage(false, str); }
}

//------------------------------------------------
/**
    fonctions statiques (locales a ce fichier)
*/

static void sendLine(int socket,std::string s, bool verbose)
{
    s += '\n';
    send(socket,s.c_str(),s.length(),0);
    if (verbose) std::cout << "sendLine: " << s.c_str();
}

static void sendBinaryData(int socket, const char* buf, long long int len, bool verbose)
{
    send(socket,buf,len,0);
    if (verbose) std::cout << "sendBinaryData ( " << len << " bytes)";
}

static std::string receiveLine(int socket, bool verbose) {

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
        if (verbose)
            std::cout << "readLine: " << ret.c_str();
        return ret;
    }
  }
}

void connectToServer2(
    const char *hostName, //nom de l'host, exemple: cnn.com
    int s_,//socket
    struct in_addr ipAdress, // adresse ip correspondant a l'host: exemple 1.1.1.1
    const int port)
{
    sockaddr_in addr;
    bzeroStructure((void *)&addr,sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = ipAdress;
    memset(&(addr.sin_zero), 0, 8);

    if (connect(s_, (sockaddr *) &addr, sizeof(sockaddr))) {
#ifdef WIN32
#ifdef _MSC_VER
		char err[101];
		strerror_s(err, 100, WSAGetLastError());
#else
        char* err = strerror(WSAGetLastError());
#endif
        std::string err2("error while connecting to server");
        err2 += err;
        ErrorRelease(err2.c_str());
#else
        ErrorRelease("error while connecting to server");
#endif
    }
}

//---------------------------------------------------------------------
/**
    Demande une page HTTP1.0 au serveur.
    Ne renvoie que le contenu de la page, pas l'en-tete.
    (Gère à l'arrache le Transfer-Encoding chunked, mais pas grave parce que si
    le client est HTTP1.0, le serveur ne repondra jamais en chunked.)
    \param serverPage Page requestee, exemple: /machin.html
    \param postArguments Arguments POST. Mettre NULL si passage en GET.
    \param connectionKeepAlive Faut-il envoyer un keep-alive ?
        Si non, vous ne pourrez pas demander d'autre page apres celle-la.
        Si oui, vous pourrez, pour peu que le serveur accepte les keep-alive.
*/

std::string* Connection::getHTTPPage(
    const char *serverPage,
    const PostArgumentListType* postArguments,
	const char* bufferPutData, 
	long long int bufferPutLength, 
    bool connectionKeepAlive,
	const std::string& method)
{
    if (verbose) std::cout << "Sending ..." << std::endl;

	//----------------------
	// Envoi d'une requete de page
	//----------------------

	std::string str("");
    str += method + " ";
    str += serverPage;
    str += " HTTP/1.0";
    sendLine(s_, str, verbose);

    std::string str2("Host: ");
    str2 += hostName.c_str();
    sendLine(s_, str2, verbose);

    if (connectionKeepAlive)
        sendLine(s_,"Connection: keep-alive",verbose);

    if (postArguments != NULL)
    {
        //ecrit la ligne des arguments post
        std::string str3("");
        bool firstArg = true;

        //pour chaque argument
        for (PostArgumentListType::const_iterator i=postArguments->begin();i!=postArguments->end();i++)
        {
            //ajoute un "&"
            if (firstArg)
                firstArg = false;
            else
                str3 += "&";
            //ajoute l'argument

            const PostArgumentType &arg = (*i);

            str3 += arg.first.c_str();
            str3 += "=";
            str3 += arg.second.c_str();
        }

        std::string str4("Content-Length: ");
        char* lenint = Utils::itoa(str3.length());
        str4 += lenint;
        delete [] lenint;

        sendLine(s_,str4,verbose);//Content-Length

        sendLine(s_,"Content-Type: application/x-www-form-urlencoded",verbose);
        sendLine(s_,"",verbose);//saut de ligne
        sendLine(s_,str3,verbose);//arguments
    }

	if (bufferPutData != NULL)
	{
		std::string str4("Content-Length: ");
		char* lenint = Utils::itoa(bufferPutLength);
		str4 += lenint;
		delete [] lenint;
		sendLine(s_,str4,verbose);//Content-Length

		sendLine(s_,"Content-Type: application/data",verbose);
        sendLine(s_,"",verbose);//saut de ligne
		sendBinaryData(s_, bufferPutData, bufferPutLength, verbose);
		sendLine(s_,"",verbose);//retour a la ligne
	}

    //saut de ligne
    sendLine(s_,"",verbose);

    if (verbose) std::cout << "End sending, waiting for response ..." << std::endl;

    //----------------------
	// Recuperation du resultat
	//----------------------

    std::string *result = new std::string();
    //while(strcmp(result->c_str(),"")==0)
    //{
        bool hasBegunText = false;
        bool isChunked = false;
        while (true)
        {
            std::string tmp = receiveLine(s_,verbose);
            if (tmp.empty()) break;
            if (!hasBegunText)
            {
                if (strcmp(tmp.c_str(),"Transfer-Encoding: chunked\r\n") == 0 || strcmp(tmp.c_str(),"Transfer-Encoding: chunked\n") == 0)
                {
                    isChunked = true;
                }
                else if (strcmp(tmp.c_str(),"\r\n")==0 || strcmp(tmp.c_str(),"\n")==0)
                {
                    hasBegunText = true;
                    if (isChunked)
                        receiveLine(s_,verbose);
                }
            }
            else
                *result += tmp.c_str();
        }
    //}

    if (verbose) std::cout << "End response." << std::endl;

    return result;
}

//---------------------------------------------------------------------
/**
    Envoie sendText au serveur et attend la reponse
*/
std::string *Connection::getSimplePage(const char *sendText)
{
    if (verbose) std::cout << "Sending ..." << std::endl;
    int i=0;
    while (true)
    {
        int j = i;
        while (sendText[i]!='\n' && sendText[i]!='\0') i++;
        if (i==j)
            sendLine(s_, "", verbose);
        else
        {
            char *tmp = Utils::copyFragmentOfStringIntoNewString(sendText,j,i);
            sendLine(s_, tmp, verbose);
            delete tmp;
        }
        if (sendText[i]=='\0') break;
        i++;
    }
    //octet de fin
    char endc = '\0';
    send(s_,&endc,1,0);

    if (verbose) std::cout << "End sending, waiting for response ..." << std::endl;

    std::string *result = new std::string();

    while (true)
    {
        std::string tmp = receiveLine(s_,verbose);
        //std::cout << ":"<<tmp << ":"<< std::endl;
        if (tmp.empty()) break;
        *result += tmp.c_str();
    }
	
    if (verbose) std::cout << "End response." << std::endl;

    return result;
}

//---------------------------------------------------------------------
/**
    Constructeur. Cf la methode de l'objet Network pour creer un objet.
*/

Connection::Connection(const char *hostName_, bool isIpAddress, int port_, std::string& outErrorMessage, bool verbose_)
:hostName(hostName_),port(port_),verbose(verbose_)
{
    s_ = socket(AF_INET,SOCK_STREAM,0);

    if (verbose) std::cout << "Getting ip " << hostName << "..." << std::flush;

    if (isIpAddress)
    {
        ipAddress = (void*)new struct in_addr();
        struct in_addr* inad = (struct in_addr*)ipAddress;
#ifdef WIN32
        inad->s_addr = inet_addr(hostName_);
        if (inad->s_addr == INADDR_NONE) outErrorMessage = "inet_addr == INADDR_NONE";
#else
        int res = inet_aton(hostName_, inad);
        if (res != 1) outErrorMessage = "inet_aton failed";
#endif
    }
    else
    {
        hostent *he;
        if ((he = gethostbyname(hostName_)) == 0)
        {
            std::string str("La connection a echoue. Verifiez votre connection Internet.\n");
            str += "Connection failed. Check your Internet connection.\n";
            str += "Details of the failure :\n";
            str += "(he = gethostbyname(hostName)) == 0 failed: ";
#ifdef _MSC_VER
			char err[101];
			strerror_s(err, 100, h_errno);
#else
			char* err = strerror(h_errno);
#endif
			str += err;
            str += " ; Server:";
            str += hostName_;
            outErrorMessage = str;
        }
        else
        {
            ipAddress = (void*)new in_addr();
            *((in_addr*)ipAddress) = *((in_addr *)he->h_addr);
        }
    }

    if (verbose)
    {
        if (strcmp(outErrorMessage.c_str(),"") != 0)
            std::cout << "Error in connection: " << outErrorMessage << std::endl;
        std::cout << "done." << std::endl;
    }
}

//---------------------------------------------------------------------
/**
    Se connecte au serveur.
*/

void Connection::connectToServer()
{
    if (verbose)
    {
        std::cout
            << "Connecting to " << hostName.c_str()
            << " on port " << port
            << " using socket " << s_
            //<< " with ip " << inet_ntoa(*((in_addr*)ipAddress))
            << "..." << std::flush;
    }

    connectToServer2(hostName.c_str(), s_, *((in_addr*)ipAddress), port);// *((in_addr *)he->h_addr)

    if (verbose)
        std::cout << "done." << std::endl;
}

//---------------------------------------------------------------------
/**
    Se deconnecte du serveur.
*/

void Connection::unconnect()
{
    if (verbose) std::cout << "Closing socket " << s_ << "..." << std::flush;
    //ferme la connexion
    /*if (s_ != -100)
    {
		#ifdef _MSC_VER
			_close(s_);
		#else
			close(s_);
		#endif
        s_ = -100;
    }*/
    if (verbose) std::cout << "done." << std::endl;
}

//---------------------------------------------------------------------
/**
    Destructeur. Se deconnecte s'il est connecte.
*/
Connection::~Connection()
{
    unconnect();
    delete ((struct in_addr*)ipAddress);
}

#endif //ndef USES_WINDOWS8_METRO

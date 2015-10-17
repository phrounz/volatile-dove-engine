
/******************************************************************
Gere tout ce qui est reseau/Internet.
Done by Francois Braud
******************************************************************/

#ifndef Network_h_INCLUDED
#define Network_h_INCLUDED

#include <string>

#include "../source/Connection.h"
#include "../source/Server.h"

class Network
{
public:
#ifndef USES_WINDOWS8_METRO
    //! Constructeur. Initialise le service reseau.
    Network();

    //! Destructeur. Arrete le service reseau.
    ~Network();

    //! Renvoie l'ip locale (sous la forme de chiffres separes par des points).
    std::string getLocalIp();

    //! cree un nouvel objet Connection.
    Connection *createConnection(
        const char *hostName,
        bool isIpAddress = false,
        int port = 80,
        bool crashIfUnreachable = true,
        bool verbose = false);

    //! Renvoie directement une page a partir d'un serveur.
    std::string *getPageFromWebServer(
        const char *url,
        const Connection::PostArgumentListType *postArguments = NULL,
		const char* bufferPutData = NULL, 
		long long int bufferPutLength = -1, 
        bool crashIfUnreachable = true,
		const std::string& method = "GET", 
        bool verbose = false);

    //! cree un nouvel objet Server (serveur).
    Server *createServer(
        std::string *(*serverResponseFunction)(const char *),
        int port = 80, int reactTime = 100, int maxClients = 50, bool verbose = false);
#endif
};

#endif //Network_h_INCLUDED

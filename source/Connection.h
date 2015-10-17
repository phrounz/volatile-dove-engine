#ifndef USES_WINDOWS8_METRO
/******************************************************************
Une connexion client
Done by Francois Braud
******************************************************************/

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <map>
#include <utility>

class Connection
{
    friend class Network;

public:
    //! Pour PostArgumentListType
    struct ltstrPostArgumentListType
    {
        bool operator()(std::string s1, std::string s2) const
        {return strcmp(s1.c_str(), s2.c_str()) < 0;}
    };

    typedef std::map<std::string,std::string,ltstrPostArgumentListType> PostArgumentListType;
    typedef std::pair<std::string,std::string> PostArgumentType;


    //! Se connecte au serveur.
    void connectToServer();

    //! Demande une page HTTP1.1 au serveur et recupere la reponse
    std::string* getHTTPPage(
        const char *serverPage,
        const PostArgumentListType* postArguments, // = NULL,
		const char* bufferPutData, 
		long long int bufferPutLength, 
        bool connectionKeepAlive, // = true,
		const std::string& method);// = "GET");

    //! Envoie sendText au serveur et recupere la reponse
    std::string *getSimplePage(const char *sendText);

    //! Se deconnecte du serveur.
    void unconnect();

    //! Destructeur. Se deconnecte s'il est connecte.
    ~Connection();

private:
    //! Constructeur.
    //! Pour construire un objet Connection, utiliser l'objet Network.
    Connection(const char *hostName, bool isIpAddress, int port_, std::string& outErrorMessage, bool verbose_);

    int s_;//socket
    std::string hostName;
    void* ipAddress;//struct in_addr*
    int port;
    bool verbose;
};

#endif /*CONNECTION_HPP*/

#endif //ndef USES_WINDOWS8_METRO
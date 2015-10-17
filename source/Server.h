#ifndef USES_WINDOWS8_METRO
/******************************************************************
Un serveur reseau
(fonctionne de maniere asynchrone par rapport au reste du programme).
Done by Francois Braud
******************************************************************/

#ifndef SERVER_HPP
#define SERVER_HPP

class Server
{
    friend class Network;
    friend void *serverWorkWrapper(void *arg);
    friend void *serverResponseWrapper(void *arg);

public:

    //! destructeur
    ~Server();

private:

    //! Constructeur. Lance le serveur immediatement.
    //! Pour construire un objet Server, utiliser l'objet Network.
    Server(
        std::string *(*serverResponseFunction_)(const char *),
        int port, int reactTime_, int maxClients_, bool verbose_);

    void *work(void *infoForWorkHidden2);
    void *response(void *lpSocket);

    void *infoForWorkHidden;

    bool hasFinished;
    bool hasFinishedMainThread;

    bool verbose;

    int reactTime;

    int maxClients;

    std::string *(*serverResponseFunction)(const char *);

    int currentNbResponses;
};

#endif

#endif //ndef USES_WINDOWS8_METRO
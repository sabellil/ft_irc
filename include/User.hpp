#ifndef USER_HPP
#define USER_HPP

#include <string>

class User
{
private:
    int                 _fd;//ou _clientFd ?

    std::string         _nick;
    std::string         _username;

    bool                _hasPass;
    bool                _hasNick;
    bool                _hasUser;
    bool                _registered;

    std::string         _inbuf;//s'assurer d'aller jusqu'au \r\n et d'avoir toute la ligne
    std::string         _outbuf;

public:
    User(int fd);
    ~User();

    std::string& inbuf() { return _inbuf; }//pour retourner le cumul des fragments de ligne recup par recv()
    std::string& outbuf() { return _outbuf; }//ce qu'on retourne au client
    
    int getFd() const;//return le fd du client pour idenfier sa connexion

    const std::string getNick() const;
    const std::string getUsername() const;

    void setNick(const std::string& nickname);
    void setUsername(const std::string& username);
    
    bool hasPass() const;//la commande pass a t elle ete validee?
    bool hasNick() const;//nickname valide defini?
    bool hasUser() const;//

    bool isRegistered() const;//client bien enregistre sur le serveur irc?
    
    void setHasNick(bool value);
};
#endif

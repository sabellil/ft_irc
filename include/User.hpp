#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <string>
#include <set>

class Channel;

class User
{
private:
    int                 _fd;

    std::string         _nick;
    std::string         _username;
    std::string         _realname;
    bool                _hasPass;
    bool                _hasNick;
    bool                _hasUser;
    bool                _registered;
    bool                _shouldDisconnect;

    std::string         _inbuf;
    std::string         _outbuf;

    std::set<Channel*>  _channels;

public:
    User(int fd);
    ~User();

    std::string&        inbuf() { return _inbuf; }
    std::string&        outbuf() { return _outbuf; }
    
    int                 getFd() const;
    std::string         getNick() const;
    std::string         getUsername() const;
    std::string         getRealname() const;
    
    void                setNick(const std::string& nickname);
    void                setUsername(const std::string& username);
    void                setHasNick(bool value);
    void                setRealname(const std::string& realname);
    void                setHasUser(bool value);
    void                setHasPass(bool value);

    bool                hasPass() const;
    bool                hasNick() const;
    bool                hasUser() const;

    bool                isRegistered() const;
    void                setRegistered(bool value);
    bool                shouldDisconnect() const;
    void                setShouldDisconnect(bool value);

    const std::set<Channel*>& getChannels() const;
    void                addChannel(Channel* channel);
    void                removeChannel(Channel* channel);

};
#endif

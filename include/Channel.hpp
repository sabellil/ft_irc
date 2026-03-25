#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#pragma once 

#include <string>
#include <set>

class User;

class Channel
{
    public:
        Channel();
        Channel(const std::string& name);
        ~Channel();
        std::string             _name;

        const std::string&      getName() const;
        bool                    hasUser(User* user) const;//un user est-il deja dans le channel?
        void                    addUser(User* user);
        void                    removeUser(User* user);
        const std::set<User*>&  getUsers() const;//recuperer tous les users d'un channel

        bool                    isOperator(User * user) const;
        void                    addOperator(User* user);
        void                    removeOperator(User* user);
        const std::set<User*>&  getOperators() const;

        bool                    isInviteOnly() const;
        void                    setInviteOnly(bool value);
        bool                    isInvited(User* user) const;
        void                    addInvited(User* user);
        void                    removeUserLimit(User* user);
        void                    removeInvite(User* user);

        bool                    isTopicRestricted() const;
        void                    setTopicRestricted(bool value);
        const std::string&      getTopic() const;
        void                    setTopic(const std::string& topic);



        bool                    hasKey() const;
        const std::string&      getKey() const;
        void                    setKey(const std::string& key);
        void                    removeKey();

        bool                    hasUserLimit() const;
        int                     getUserLimit() const;
        void                    setUserLimit(int limit);
        void                    removeUserLimit();

    private:
        std::set<User*>         _users;//set pour stocker mes users, eviter les doublons et verifier rqpidement si il est deja dans le channel
        std::set<User*>         _operators;
        std::set<User*>         _invitedUsers;
        std::string             _topic;

        bool                    _inviteOnly;
        bool                    _topicRestricted;

        bool                    _hasKey;
        std::string             _key;

        int                     _userLimit;
        bool                    _hasUserLimit;
};

#endif

/*
1. Completer Channel
Completer pour avoir les membres, operateurs, topic, si salon est invite only, 
si le topic est reserve aux ops, la key du salon, la limite dutilisateur, liste de sinvites pour INVITE)

2. Nommer le 1er user operateur

3. KICK

4. INVITE

5. TOPIC

6. MODE i t k o l
*/ 





#include <string>
#include <vector>

class User; 

class Message{

public:
    std::string                     _raw;
    std::string                     _command;
    std::vector<std::string>        _params;
    std::string                     _trailing;
    User*                           sender;

    private:
    /*getters de commandes, params et trailing en const pour proteger d'une modif*/


};
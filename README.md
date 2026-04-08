*This project has been created as part of the 42 curriculum by mairivie, sabellil.*

# 👾 ft\_irc : A minimal IRC server written in C++ 98 👾

### Index :

* [Description](#Description)
    Quick project description (#Description)
    Subject requirement(#Assignement)

* [Instruction](#Instruction)
    How to compile
    How use it
 
* [Ressources](#Ressources)
    What's IRC
    Biblio

## Description
This project is a practical introduction to communication protocols through the creation of a minimal IRC server.
Protocols are fundamental to networking, as they define how machines understand and respond to each other.
IRC provides a simple yet realistic foundation to explore these concepts.
Along the way, it builds solid experience with sockets and client-server design.
Lets build an IRC server from scratch !

## Assignement
This IRC server must be a multi-client TCP server capable of handling all connections simultaneously.
All I/O operations must be non-blocking and managed through a single poll() or equivalent.
Compatible with a chosen reference IRC client (IRSSI in our case), which should connect and behave similarly to a standard IRC server.

Features include : 
client authentication (PASS, NICK, USER)
channel management (JOIN, PART)
message exchange betwees clients (PRIVMSG).
Operator role management (access to commands: KICK, INVITE, TOPIC, MODE.)
Channel mode configurations: invite-only (i), topic restrictions (t), password protection (k), operator privileges (o), user limits (l).

## Context

!\[Screenshot of a bash.org IRC quote showing the message: "](image.png)

What's IRC Protocol ?



## Instructions

### What do you need ?

- Linux or any POSIX-compatible system
- C++ compiler with C++98 support (`g++` or `c++`)
- GNU `make`

### /me build the project

```bash
git clone https://github.com/sabellil/ft_irc.git
cd ft_irc
make
```

This produces the `ircserv` binary. Object files are placed in `obj/`.

```bash
make clean    # remove object files
make fclean   # remove objects and binary
make re       # full rebuild
```

### /me start the server

```bash
./ircserv <port> <password>
```

port: The port number on which your IRC server will be listening for incoming IRC connections. 
    Usual IRC port are 6665 to 6669 (mostly 6667) but we choose to be more flexible.
password: The connection password. It will be needed by any IRC client that tries to connect to your server. 
    No security requirement in the subject. We choose to only ask for at least 4 char long.

As the server starts listening, connect any IRC client to `localhost:<port>` with the given password.

### /me connect an irssi client

Through irssi
```bash
irssi
/connect 127.0.0.1 <port> <password>
```

Through nc
```bash
nc 127.0.0.1 6667
PASS motdepasse
NICK <nickname>
USER <username> 0 * :<nickname>
```

## 


What's a socket ?
init, bind, listen
With the great help of https://beej.us/guide/bgnet/html/split-wide/index.html

## Ressources
* **RTFM :** [man](link)
* **Documentations**
> https://beej.us/guide/bgnet/
* **Peer-learning** special thx to jbrousse, phwang, jewu, gcaptari for their tips


Sources:

* http://manpagesfr.free.fr/man/man2/accept.2.html
* https://man7.org/linux/man-pages/man2/poll.2.html

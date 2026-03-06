CPP       = c++
CPPFLAGS  = -Wall -Wextra -Werror -std=c++98

NAME = ircserv

SRCS = main.cpp \
	Helpers.cpp \
	Message.cpp \
	Server.cpp \
	User.cpp
	
SRC_DIR = src
SRC = $(addprefix $(SRC_DIR)/, $(SRCS))

OBJ_DIR = obj
OBJ = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
DEP = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.d))


all: $(NAME)
	@echo "-----" 
	@echo "All good ! Use ./$(NAME) <port> <password> to start the program "
	@echo "<port> : number between 1 and 65535"
	@echo "<password> : string of 4 printable char minimum (no space) "
	@echo "Quit the program with ctrl+c or ctrl+\ "
	@echo "-----"
.PHONY: all

$(NAME): $(OBJ)
	$(CPP) $(CPPFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if [ ! -d obj ]; then mkdir -p $(@D); echo "mkdir -p $(@D)" ; fi
	$(CPP) $(CPPFLAGS) -MMD -c $< -o $@

-include ${DEP}

clean:
	@if [ -d obj ]; then rm -r obj; echo "rm -r obj" ; fi
.PHONY: clean

fclean: clean
	rm -f $(NAME)
.PHONY: fclean

re: fclean all
.PHONY: re

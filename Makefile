CPP       = c++
CPPFLAGS  = -Wall -Wextra -Werror -std=c++98

NAME = irc

SRCS = main.cpp Helpers.cpp Message.cpp Server.cpp User.cpp
	
SRC_DIR = src
SRC = $(addprefix $(SRC_DIR)/, $(SRCS))

OBJ_DIR = obj
OBJ = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
DEP = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.d))


all: $(NAME)
	@echo "-----\nAll good ! Use ./$(NAME) <port> <password> to start the program\n-----"
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
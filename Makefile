CPP = c++
# CPP_FLAGS = -Wall -Werror -Wextra -std=c++98
CPP_FLAGS = -Wall -Werror -Wextra -std=c++98 -D DEBUG

PROGRAM_NAME = irc_serv

SRCDIR := ./src
INCDIR := ./includes

SRC := $(wildcard $(SRCDIR)/*.cpp) \

INCLUDES := -I$(INCDIR)

OBJDIR := objs
OBJS := $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(PROGRAM_NAME)

obj: $(OBJS)

$(PROGRAM_NAME): $(OBJS)
	$(CPP) $(CPP_FLAGS) $(INCLUDES) -o $@ $^

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CPP) $(CPP_FLAGS) $(INCLUDES) -o $@ -c $<

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(PROGRAM_NAME)

re: fclean all

.PHONY: all clean fclean re

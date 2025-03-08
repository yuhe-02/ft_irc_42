CPP = c++
# CPP_FLAGS = -Wall -Werror -Wextra -std=c++98
CPP_FLAGS = -std=c++98

PROGRAM_NAME = irc_serv

SRCDIR := .
CLASSDIR := $(SRCDIR)/class
UTILDIR := $(SRCDIR)/utils
INCDIR := ./includes

SRC += $(wildcard $(SRCDIR)/*.cpp)
SRC += $(wildcard $(CLASSDIR)/*.cpp)
SRC += $(wildcard $(UTILDIR)/*.cpp)


INCLUDES := -I$(INCDIR)

OBJDIR := objs
OBJS := $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
OBJS := $(OBJS:$(CLASSDIR)/%.cpp=$(OBJDIR)/$(CLASSDIR)/%.o)
OBJS := $(OBJS:$(UTILDIR)/%.cpp=$(OBJDIR)/$(UTILDIR)/%.o)

all: $(PROGRAM_NAME)

$(PROGRAM_NAME): $(OBJS)
	$(CPP) $(CPP_FLAGS) $(INCLUDES) -o $@ $^

$(OBJDIR):
	@mkdir -p $(OBJDIR)/$(CLASSDIR) $(OBJDIR)/$(UTILDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CPP) $(CPP_FLAGS) $(INCLUDES) -o $@ -c $<

$(OBJDIR)/$(CLASSDIR)/%.o: $(CLASSDIR)/%.cpp | $(OBJDIR)
	$(CPP) $(CPP_FLAGS) $(INCLUDES) -o $@ -c $<

$(OBJDIR)/$(UTILDIR)/%.o: $(UTILDIR)/%.cpp | $(OBJDIR)
	$(CPP) $(CPP_FLAGS) $(INCLUDES) -o $@ -c $<

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(PROGRAM_NAME)

re: fclean all

.PHONY: all clean fclean re

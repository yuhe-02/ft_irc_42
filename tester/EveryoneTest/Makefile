# GTest のパス
GTEST_DIR = /usr/local/include/gtest
GTEST_LIB = /usr/local/lib

CPP = c++
CPP_FLAGS = -Wall -Werror -Wextra -std=c++11 -I $(GTEST_DIR)

PROGRAM_NAME = irc_serv

SRCDIR := ../../src
INCDIR := ../../includes

SRC := ../../src/Channel.cpp ../../src/Everyone.cpp ../../src/RefCounted.cpp EveryoneTest.cpp EveryoneTest.cpp

INCLUDES := -I $(INCDIR) -I $(GTEST_DIR)

OBJDIR := objs
OBJS := $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(PROGRAM_NAME)

obj: $(OBJS)

$(PROGRAM_NAME): $(OBJS)
	$(CPP) $(CPP_FLAGS) $(INCLUDES) -o $@ $^ -L$(GTEST_LIB) -lgtest -lgtest_main -pthread

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

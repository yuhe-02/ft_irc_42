CPP = g++
CPP_FLAGS = -Wall -Werror -Wextra
DEBUG_FLAGS = -Wall -Werror -Wextra -D DEBUG
# Google Test のリンク用ライブラリ(スレッド等が必要になる場合も多い)
TEST_LIBS = -lgtest -lgtest_main -pthread

PROGRAM_NAME = irc_serv

SRCDIR := ../../src
INCDIR := ../../includes

# ---------- 修正1: ソースの列挙を分離 ----------
# 既存のソース(.cpp)をSRCに入れる
SRC_PROD := $(wildcard $(SRCDIR)/*.cpp)
# テスト用コード(TranslatorTest.cpp)はパターンルールとディレクトリが異なるため分離
SRC_TEST := TranslatorTest.cpp
SRC := $(SRC_PROD) $(SRC_TEST)

# インクルードディレクトリ
INCLUDES := -I$(INCDIR)

# ビルド生成物（オブジェクトファイル）を置くディレクトリ
OBJDIR := objs

# オブジェクトファイルのリスト
#   - 既存ソース(../../src/*.cpp) は $(OBJDIR)/ファイル名.o へ
#   - テストコード(TranslatorTest.cpp) は $(OBJDIR)/TranslatorTest.o へ
OBJS_PROD := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC_PROD))
OBJS_TEST := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC_TEST))
OBJS := $(OBJS_PROD) $(OBJS_TEST)

all: $(PROGRAM_NAME)

debug:
	$(CPP) $(DEBUG_FLAGS) $(INCLUDES) -o debug $(SRC) $(TEST_LIBS)


# ルール: プログラムをリンク
$(PROGRAM_NAME): $(OBJS)
	$(CPP) $(CPP_FLAGS) $(INCLUDES) -o $@ $^ $(TEST_LIBS)

# 中間ディレクトリ作成
$(OBJDIR):
	@mkdir -p $(OBJDIR)

# ---------- 修正2: パターンルールを2つ用意 ----------
# 1) 既存ソース(../../src/*.cpp)用のルール
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CPP) $(CPP_FLAGS) $(INCLUDES) -c $< -o $@

# 2) テストコード(TranslatorTest.cpp)用のルール
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CPP) $(CPP_FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(PROGRAM_NAME)

re: fclean all

.PHONY: all clean fclean re

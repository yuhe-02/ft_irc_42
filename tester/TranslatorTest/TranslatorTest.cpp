#include <gtest/gtest.h>
#include "Channel.h"
#include "Everyone.h"
#include "MessageTranslator.h"

#include <gtest/gtest.h>
#include "MessageTranslator.h"

/**
 * テスト全体で使うセットアップ/クリーンアップなどがあれば
 * テストフィクスチャを定義すると便利です。
 * 今回は簡単のため、テストごとにインスタンスをリセットする方針で書いています。
 */

/**
 * ユーザーがサーバーに接続し、PASS -> NICK -> USER と順番に登録していく流れをテスト。
 * 最終的に IsRegister() == true になっていることを確認します。
 */
TEST(MessageTranslatorTest, PassNickUserFlow) {
    // テスト用の MessageTranslator を生成。仮のサーバーパスワードを "secretpass" とする
    MessageTranslator translator("secretpass");

    // user_fd はソケットのファイルディスクリプタを想定。テストなのでダミー値とする
    int user_fd = 100;

    // 1) PASS コマンド (正しいパスワード)
    translator.Execute("PASS secretpass", user_fd);

    // PASS 後、まだ NICK と USER は設定されていないので登録状態にはなっていないはず
	auto everyone = Everyone::GetInstance();
    EXPECT_FALSE(everyone->IsRegister(user_fd))
        << "NICK, USER 未設定なので登録状態ではないはず";
    // 2) NICK コマンド
    translator.Execute("NICK testNick", user_fd);
    EXPECT_FALSE(everyone->IsRegister(user_fd))
        << "まだ USER コマンドが完了していないので登録状態ではないはず";

    // 3) USER コマンド
    translator.Execute("USER user host server RealName", user_fd);

    // ここまで完了すると登録完了 (IsRegister == true) のはず
    EXPECT_TRUE(everyone->IsRegister(user_fd))
        << "PASS, NICK, USER がすべて揃ったので登録状態になるはず";

    // ユーザー情報の確認
    const Someone& s = everyone->GetSomeone(user_fd);
    EXPECT_EQ(s.nick_name.back(), "testNick");
    EXPECT_EQ(s.user_name, "user");
    EXPECT_EQ(s.host_name, "host");
    EXPECT_EQ(s.server_name, "server");
    EXPECT_EQ(s.real_name, "RealName");
}

/**
 * JOIN でチャンネルを作成し、PART で抜ける流れをテスト。
 * 1人だけで JOIN -> PART した場合、チャンネルが存在しなくなること(自動削除)を確認します。
 */
TEST(MessageTranslatorTest, JoinAndPartRemovesChannelIfEmpty) {
    MessageTranslator translator("secretpass");
    int user_fd = 200;

    // PASS, NICK, USER で登録だけ先に済ませる
    translator.Execute("PASS secretpass", user_fd);
    translator.Execute("NICK testNick", user_fd);
    translator.Execute("USER user host server RealName", user_fd);

    auto channelMgr = Channel::GetInstance();
    std::string channelName = "#testRoom";

    // チャンネルは最初は存在しない
    EXPECT_FALSE(channelMgr->ExistChannel(channelName));

    // JOIN コマンド
    translator.Execute("JOIN #testRoom", user_fd);

    // チャンネルが作成され、ユーザーが参加している
    EXPECT_TRUE(channelMgr->ExistChannel(channelName));
    EXPECT_TRUE(channelMgr->IsJoined(user_fd, channelName))
        << "ユーザーが #testRoom に参加しているはず";

    // PART コマンド (このユーザーが唯一の参加者)
    translator.Execute("PART #testRoom", user_fd);

    // 最後のユーザーが抜けたらチャンネルは削除されているはず
    EXPECT_FALSE(channelMgr->ExistChannel(channelName))
        << "参加者がいなくなったチャンネルは自動削除されているはず";
}

/**
 * QUIT コマンドでユーザーがサーバーを離脱した場合、
 * そのユーザーが最後だったチャンネルがあれば削除されることを確認します。
 */
TEST(MessageTranslatorTest, QuitRemovesUserAndChannelIfUserWasLast) {
    MessageTranslator translator("secretpass");
    int user_fd = 300;

    // PASS, NICK, USER
    translator.Execute("PASS secretpass", user_fd);
    translator.Execute("NICK quitNick", user_fd);
    translator.Execute("USER user host server RealName", user_fd);

    auto channelMgr = Channel::GetInstance();
    auto everyone   = Everyone::GetInstance();

    // JOIN してチャンネルを作成
    std::string channelName = "#quitTest";
    translator.Execute("JOIN " + channelName, user_fd);
    EXPECT_TRUE(channelMgr->ExistChannel(channelName));
    EXPECT_TRUE(channelMgr->IsJoined(user_fd, channelName));

    // QUIT コマンド
    translator.Execute("QUIT :Goodbye", user_fd);

    // ユーザーが削除されているか確認
    // GetSomeone(int fd) は存在しないFDにアクセスすると挙動未定義になる恐れがあるので、
    // 一旦こうした判定方法しかありません(実装次第)。
    // ただし、実装によっては例外が出る場合もあるため、本来は安全に確認する仕組みが必要です。
    // ここでは「IsRegister が false に戻る」などで確認する例とします。
    EXPECT_FALSE(everyone->IsRegister(user_fd))
        << "QUIT 後はユーザーが削除され、登録状態ではなくなるはず";

    // チャンネルにも参加者がいなくなるので削除されている
    EXPECT_FALSE(channelMgr->ExistChannel(channelName))
        << "最後のユーザーがQUITしたチャンネルは削除されているはず";
}

/**
 * MODE コマンドでチャンネルのモードを変更できるかの簡単なテスト。
 * ここでは招待制(+i)だけ簡易的にテストします。
 */
TEST(MessageTranslatorTest, ModeInviteOnlyChannel) {
    MessageTranslator translator("secretpass");
    int user_fd = 400;

    // 登録
    translator.Execute("PASS secretpass", user_fd);
    translator.Execute("NICK modeNick", user_fd);
    translator.Execute("USER user host server RealName", user_fd);

    // JOIN でチャンネルに参加
    std::string channelName = "#modeTest";
    translator.Execute("JOIN " + channelName, user_fd);

    auto channelMgr = Channel::GetInstance();
    EXPECT_TRUE(channelMgr->ExistChannel(channelName));
    EXPECT_TRUE(channelMgr->IsJoined(user_fd, channelName));

    // デフォルトでは招待制ではないはず
    const ChannelInfo& infoBefore = channelMgr->GetChannelInfo(channelName);
    EXPECT_FALSE(infoBefore.is_invite);

    // MODE コマンドで +i をつける (例: "MODE #modeTest +i")
    std::string modeCmd = "MODE " + channelName + " +i";
    translator.Execute(modeCmd, user_fd);

    // チャンネル情報を再取得して、is_invite が true に変わったか確認
    const ChannelInfo& infoAfter = channelMgr->GetChannelInfo(channelName);
    EXPECT_TRUE(infoAfter.is_invite)
        << "MODE +i により、招待制フラグ is_invite が true に変わるはず";
}

/**
 * KICK コマンドでユーザーを追放するテスト。
 * - オペレータ権限の有無での分岐などは実装次第。
 * - 簡易的に同一ユーザーFDを「管理者」として扱う例とします。
 */
TEST(MessageTranslatorTest, KickUserFromChannel) {
    MessageTranslator translator("secretpass");

    // 2人のユーザー
    int op_fd   = 500;
    int user_fd = 501;

    // 両者とも PASS, NICK, USER で登録
    translator.Execute("PASS secretpass", op_fd);
    translator.Execute("NICK Operator", op_fd);
    translator.Execute("USER user1 host1 server1 RealName1", op_fd);

    translator.Execute("PASS secretpass", user_fd);
    translator.Execute("NICK NormalUser", user_fd);
    translator.Execute("USER user2 host2 server2 RealName2", user_fd);

    // Operator が JOIN
    std::string channelName = "#kickTest";
    translator.Execute("JOIN " + channelName, op_fd);
    auto channelMgr = Channel::GetInstance();
    EXPECT_TRUE(channelMgr->ExistChannel(channelName));
    EXPECT_TRUE(channelMgr->IsJoined(op_fd, channelName));

    // NormalUser も JOIN
    translator.Execute("JOIN " + channelName, user_fd);
    EXPECT_TRUE(channelMgr->IsJoined(user_fd, channelName));

    // ここで、仮に Operator がオペレータ権限を持っているとし、Kick 実行
    // 実装では is_master.insert(player_fd) 等しているため、
    // 本来ならチャンネルモードで +o をつける必要があるかもしれませんが省略。
    std::string kickCmd = "KICK " + channelName + " NormalUser :You are kicked!";
    translator.Execute(kickCmd, op_fd);

    // NormalUser がチャンネルから抜けている
    EXPECT_FALSE(channelMgr->IsJoined(user_fd, channelName));
    EXPECT_TRUE(channelMgr->IsJoined(op_fd, channelName))
        << "キック実行者はまだ残っているはず";

    // チャンネルはまだ参加者が Operator だけ残っているので削除はされていない
    EXPECT_TRUE(channelMgr->ExistChannel(channelName));
}


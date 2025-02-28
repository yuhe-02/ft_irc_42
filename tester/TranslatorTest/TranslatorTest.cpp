#include <gtest/gtest.h>
#include "Channel.h"
#include "Everyone.h"
#include "MessageTranslator.h"

#include <gtest/gtest.h>
#include "MessageTranslator.h"
#include <gtest/gtest.h>
#include "Everyone.h"
#include "Channel.h"

class EveryoneTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        // テスト開始前に全データ初期化
        Everyone::GetInstance()->Clear(0);
    }
    virtual void TearDown() {
        // テスト終了後にも念のためクリア
        Everyone::GetInstance()->Clear(0);
    }
};

TEST_F(EveryoneTest, CreateAndDeleteUser) {
    auto everyone = Everyone::GetInstance();
    // まだ誰もいない想定
    EXPECT_FALSE(everyone->IsRegister(100))
        << "未作成ユーザは存在しない";

    // CreateUser(100) -> (fd=100)
    ChannelResult r1 = everyone->CreateUser(100);
    EXPECT_EQ(r1.first, 1);  // 成功
    // NICK, USER設定前なので IsRegister(100) は false のはず
    EXPECT_FALSE(everyone->IsRegister(100));

    // DeleteUser(100)
    ChannelResult r2 = everyone->DeleteUser(100);
    EXPECT_EQ(r2.first, 1);
    // 削除されたので IsRegister(100) は false
    EXPECT_FALSE(everyone->IsRegister(100));
}

TEST_F(EveryoneTest, ClearRemovesAllUsers) {
    auto everyone = Everyone::GetInstance();
    // 2ユーザー作成
    everyone->CreateUser(101);
    everyone->CreateUser(102);

    // Clear で全ユーザー削除
    everyone->Clear(0);
    // ユーザーがいないので IsRegister は false
    EXPECT_FALSE(everyone->IsRegister(101));
    EXPECT_FALSE(everyone->IsRegister(102));
}
TEST_F(EveryoneTest, NickAndUserMakesRegisterTrue) {
    auto everyone = Everyone::GetInstance();
    // fd=200 のユーザー作成
    everyone->CreateUser(200);
    EXPECT_FALSE(everyone->IsRegister(200));

    // SetNickname
    ChannelResult rNick = everyone->SetNickname(200, "testNick");
    EXPECT_EQ(rNick.first, 1);
    // まだ USER が未設定
    EXPECT_FALSE(everyone->IsRegister(200));

    // SetUser
    ChannelResult rUser = everyone->SetUser(200, "testUser", "host", "server", "real");
    EXPECT_EQ(rUser.first, 1);

    // NICK と USER の両方が揃ったので REGISTER = true
    EXPECT_TRUE(everyone->IsRegister(200));
}
class ChannelTest : public ::testing::Test {
	protected:
		void SetUp() {
			// テスト前にまとめて初期化
			Everyone::GetInstance()->Clear(0);
			// Channel::GetInstance()->ClearChannelData();
			// 上記も Clear(0) 内で呼ばれるなら不要
		}
		void TearDown() {
			Everyone::GetInstance()->Clear(0);
		}
	};

	/**
	 * ユーザーが未登録の場合に JoinedChannel を呼んでも失敗する
	 * (あるいは FATAL 等で返す) ことを想定した例。
	 * 実装によっては「未登録でもCreateChannelされる」という仕様もあるので要確認。
	 */
	TEST_F(ChannelTest, JoinFailsIfUserNotRegistered) {
		auto channel = Channel::GetInstance();
		// user_fd=300 のユーザーをCreateするが NICK/USER未設定 -> REGISTER=false
		Everyone::GetInstance()->CreateUser(300);

		// JOIN 失敗をテスト: もし実装で "未登録なら ERR_NOTREGISTERED" などを返すならこう書く
		ChannelResult cr = channel->JoinedChannel(300, "#failRoom");
		EXPECT_EQ(cr.first, ERR_NOTREGISTERED);
		// まだチャンネルは作られていない
		EXPECT_FALSE(channel->ExistChannel("#failRoom"));
	}

	/**
	 * REGISTER済みユーザーが JOIN して、PART で抜けた後、最後の人が抜けるとチャンネル削除
	 */
	TEST_F(ChannelTest, JoinPartRemovesChannelWhenEmpty) {
		auto everyone = Everyone::GetInstance();
		auto channel = Channel::GetInstance();

		// 1) ユーザー作成+登録
		int fd = 301;
		everyone->CreateUser(fd);
		everyone->SetNickname(fd, "nick301");
		everyone->SetUser(fd, "user301", "host", "server", "real");
		EXPECT_TRUE(everyone->IsRegister(fd));

		// 2) JOIN
		ChannelResult rJoin = channel->JoinedChannel(fd, "#testRoom");
		EXPECT_EQ(rJoin.first, 332);
		EXPECT_TRUE(channel->ExistChannel("#testRoom"));
		EXPECT_TRUE(channel->IsJoined(fd, "#testRoom"));

		// 3) PART (一人しかいない→削除)
		ChannelResult rPart = channel->LeaveChannel(fd, "#testRoom");
		EXPECT_EQ(rPart.first, 1);
		EXPECT_FALSE(channel->ExistChannel("#testRoom"))
			<< "最後のユーザーが抜けたらチャンネル削除";
	}
	TEST_F(ChannelTest, TwoUsersJoinChannelAndOneLeaves) {
		auto everyone = Everyone::GetInstance();
		auto channel = Channel::GetInstance();

		// ユーザーA
		everyone->CreateUser(400);
		everyone->SetNickname(400, "Alice");
		everyone->SetUser(400, "ua", "ha", "sa", "ra");
		// ユーザーB
		everyone->CreateUser(401);
		everyone->SetNickname(401, "Bob");
		everyone->SetUser(401, "ub", "hb", "sb", "rb");

		// AとBがJOIN
		channel->JoinedChannel(400, "#multi");
		channel->JoinedChannel(401, "#multi");
		EXPECT_TRUE(channel->ExistChannel("#multi"));

		// Bが抜ける -> まだAがいるからチャンネルは残る
		channel->LeaveChannel(401, "#multi");
		EXPECT_TRUE(channel->ExistChannel("#multi"))
			<< "まだAが残っているためチャンネルは削除されない";
		EXPECT_FALSE(channel->IsJoined(401, "#multi"));
		EXPECT_TRUE(channel->IsJoined(400, "#multi"));

		// Aも抜ける -> チャンネル削除
		channel->LeaveChannel(400, "#multi");
		EXPECT_FALSE(channel->ExistChannel("#multi"));
	}
	TEST_F(ChannelTest, OperatorKickUser) {
		auto everyone = Everyone::GetInstance();
		auto channel = Channel::GetInstance();

		// オペレーター(=作成者) fd=500
		everyone->CreateUser(500);
		everyone->SetNickname(500, "Oper");
		everyone->SetUser(500, "uo", "ho", "so", "ro");
		// 作成者が CreateChannel される場合は is_master.insert(500)
		// → (実装によるがデフォルトで operator になるはず)

		// チャンネルを作って参加
		channel->CreateChannel(500, "#kickRoom");
		// 別ユーザー user_fd=501
		everyone->CreateUser(501);
		everyone->SetNickname(501, "UserB");
		everyone->SetUser(501, "ub", "hb", "sb", "rb");
		channel->JoinedChannel(501, "#kickRoom");

		// Operator(500) が UserB(501) を Kick
		ChannelResult r = channel->KickChannel(500, "UserB", "#kickRoom");
		EXPECT_EQ(r.first, 1);
		EXPECT_FALSE(channel->IsJoined(501, "#kickRoom"));
	}

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
    translator.Execute("NICK testNick11", user_fd);
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
TEST(MessageTranslatorTest, BasicFlow) {
    MessageTranslator translator("secretpass");
    int fd = 199;

    // 1) PASS
    translator.Execute("PASS secretpass", fd);
    // user_->CreateUser(fd) が呼ばれたはず
    // Everyone 側の状態を確認(登録済みだが NICK/USER がまだなので REGISTER フラグはfalse)
    auto everyone = Everyone::GetInstance();
    EXPECT_FALSE(everyone->IsRegister(fd));

    // 2) NICK
    translator.Execute("NICK testNick1", fd);
    // まだ USER 未設定なので Register は false
    EXPECT_FALSE(everyone->IsRegister(fd));

    // 3) USER
    translator.Execute("USER testUser host server RealName", fd);
    EXPECT_TRUE(everyone->IsRegister(fd)) << "NICK と USER が揃ったので登録完了";

    // 4) JOIN #room
    translator.Execute("JOIN #room", fd);
    auto channel = Channel::GetInstance();
    EXPECT_TRUE(channel->ExistChannel("#room"));
    EXPECT_TRUE(channel->IsJoined(fd, "#room"));
    // 5) PART #room
    translator.Execute("PART #room", fd);
    EXPECT_FALSE(channel->IsJoined(fd, "#room"));
    EXPECT_FALSE(channel->ExistChannel("#room")) << "最後のユーザーが抜けるとチャンネル削除";

    // 6) QUIT
    translator.Execute("QUIT :Bye", fd);
    // Everyone から削除されている
    EXPECT_FALSE(everyone->IsRegister(fd));
}
TEST(MessageTranslatorTest, ModeCommand) {
    MessageTranslator translator("secret");
    int fd = 201;
    // PASS, NICK, USER
    translator.Execute("PASS secret", fd);
    translator.Execute("NICK modeNick", fd);
    translator.Execute("USER modeUser host server Real", fd);

    // JOIN #modeRoom
    translator.Execute("JOIN #modeRoom", fd);

    // 通常: fd はオペレータとして登録されているはず (実装に依存)
    // "MODE #modeRoom +i" → invite-only フラグを有効化
    translator.Execute("MODE #modeRoom +i", fd);
    auto channel = Channel::GetInstance();
    const ChannelInfo &info = channel->GetChannelInfo("#modeRoom");
    EXPECT_TRUE(info.is_invite);

    // "MODE #modeRoom -i" → invite-only フラグを解除
    translator.Execute("MODE #modeRoom -i", fd);
    EXPECT_FALSE(channel->GetChannelInfo("#modeRoom").is_invite);
}
TEST(MessageTranslatorTest, KickCommand) {
    MessageTranslator translator("secret");

    // 2ユーザーを作る
    int op_fd = 3333;
    int user_fd = 321;
    // Operator
    translator.Execute("PASS secret", op_fd);
    translator.Execute("NICK Operatorr", op_fd);
    translator.Execute("USER oper host server RealOper", op_fd);

    // Normal user
    translator.Execute("PASS secret", user_fd);
    translator.Execute("NICK Normal", user_fd);
    translator.Execute("USER normal host server RealNorm", user_fd);

    // 両者 JOIN #kickTest
    translator.Execute("JOIN #kicTest", op_fd);
    translator.Execute("JOIN #kicTest", user_fd);
    // Kickコマンド: "KICK #kickTest Normal"
    // 実装では → channel_->KickChannel(op_fd, av[2], av[1]) → av[1]="#kickTest", av[2]="Normal"
    translator.Execute("KICK #kicTest Normal", op_fd);
    auto channel = Channel::GetInstance();
    EXPECT_FALSE(channel->IsJoined(user_fd, "#kicTest"));
    EXPECT_TRUE(channel->IsJoined(op_fd, "#kicTest"));
}
TEST(MessageTranslatorTest, UnknownCommand) {
    MessageTranslator translator("secret");
    int fd = 4444;

    // まだ PASS 未入力
    // 存在しないコマンド "FOOBAR"
    translator.Execute("FOOBAR something", fd);
    // func_["UNKNOWN"] が呼ばれて FATAL を返す実装
    // 直接 ChannelResult を受け取っていないので、状態を確認するかログで検証
    // ここでは "USER" や "NICK" が呼ばれないことを確認する程度
    EXPECT_FALSE(Everyone::GetInstance()->IsRegister(fd));
}

TEST(MessageTranslatorTest, EmptyCommandLine) {
    MessageTranslator translator("secret");
    int fd = 401;

    // 空文字列の場合、Translate すると要素0件
    translator.Execute("", fd);
    // box.size()==0 → func_.find(box[0]) の前に  out_of_range になる可能性もある
    // → 実装によってはチェックがなくてセグフォするかもしれない
    // テストを実行して挙動を確認する
    // もし安全に "UNKNOWN" とみなすように修正するなら修正後に再テスト
    // ここではセグフォしなければOKとする
    SUCCEED();
}
TEST(MessageTranslatorTest, PassCommandWithWrongPassword) {
    MessageTranslator translator("secretpass");
    int fd = 402;

    // 間違ったパスワード
    translator.Execute("PASS wrongpass", fd);
    // 返り値は ERR_NEEDMOREPARAMS になっている(現在の実装)？
    // 実際は "if (av[1] != pass_ && av[1] != operator_pass_)" の条件で
    //   → create_code_message(ERR_NEEDMOREPARAMS, "PASS") を返す
    // 矛盾する名前（NEEDMOREPARAMS）ですが、実装に合わせて確認

    // まだユーザーは作られていない
    EXPECT_FALSE(Everyone::GetInstance()->IsRegister(fd));
}

TEST(MessageTranslatorTest, PassCommandWithNoArgument) {
    MessageTranslator translator("secretpass");
    int fd = 403;

    // 引数がない
    translator.Execute("PASS", fd);
    // ERR_NEEDMOREPARAMS, "PASS" の想定
    // まだユーザーは作られていない
    EXPECT_FALSE(Everyone::GetInstance()->IsRegister(fd));
}
TEST(MessageTranslatorTest, PassOperator) {
    MessageTranslator translator("secretpass");
    // translator に operator_pass_ も設定したい場合は、実装を変更 or Setter を用意するなどする
    // ここでは仮に operator_pass_ = "operpass" として、直接代入
    // (本来の実装上どうやって operator_pass_ を設定するか不明)

	translator.SetOpePass("operpass");

    int fd = 410;
    translator.Execute("PASS operpass", fd);
    // これで "CreateUser(fd, 1)" が呼ばれ is_admin = true のUserが作られる想定

    auto s = Everyone::GetInstance()->GetSomeone(fd);
    EXPECT_TRUE(s.is_admin) << "オペレータとして作られている";
}

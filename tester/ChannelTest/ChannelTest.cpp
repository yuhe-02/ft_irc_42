// ChannelTest.cpp
#include <gtest/gtest.h>
#include "../includes/Channel.h"
#include "../includes/Everyone.h"

// テスト対象のクラスを取得
// IntrusivePtr<Channel> channel = Channel::GetInstance();
// IntrusivePtr<Everyone> everyone = Everyone::GetInstance();

// TEST(ChannelTest, CreateChannel) {
//     everyone->CreateUser("owner", "password123", "host1", "ownerNick", "real1");
//     ChannelResult res = channel->CreateChannel("testchannel", "ownerNick", 0);
//     ASSERT_EQ(res.first, 1);  // チャンネル作成成功
// }

// TEST(ChannelTest, DuplicateChannel) {
//     ChannelResult res = channel->CreateChannel("testchannel", "ownerNick", 0);
//     ASSERT_EQ(res.first, FATAL);  // 重複エラー
// }

// TEST(ChannelTest, JoinedChannel) {
//     everyone->CreateUser("user1", "password123", "host2", "userNick1", "real2");
//     ChannelResult res = channel->JoinedChannel("userNick1", "testchannel", 0);
//     ASSERT_EQ(res.first, 1);  // チャンネル参加成功
// }

// TEST(ChannelTest, LeaveChannel) {
//     ChannelResult res = channel->LeaveChannel("userNick1", "testchannel");
//     ASSERT_EQ(res.first, 1);  // チャンネル脱退成功
// }

// TEST(ChannelTest, InviteToChannel) {
//     everyone->CreateUser("user2", "password123", "host3", "userNick2", "real3");
//     ChannelResult res = channel->InviteToChannel("ownerNick", "userNick2", "testchannel");
//     ASSERT_EQ(res.first, 1);  // 招待成功
// }
class ChannelTest : public ::testing::Test {
    protected:
        void SetUp() override {
            channel = Channel::GetInstance();
            everyone = Everyone::GetInstance();

            // テストユーザーを作成
            everyone->CreateUser("UserA", "pass", "localhost", "NickA", "RealA");
            everyone->CreateUser("UserB", "pass", "localhost", "NickB", "RealB");
        }

        void TearDown() override {
        }

        IntrusivePtr<Channel> channel;
        IntrusivePtr<Everyone> everyone;
    };

// チャンネルの作成と削除のテスト
TEST_F(ChannelTest, CreateAndDeleteChannel) {
    // チャンネル作成
    ChannelResult res = channel->CreateChannel("Channel1", "NickA", 0);
    EXPECT_EQ(res.first, 1);

    // 同じ名前のチャンネルは作成失敗
    res = channel->CreateChannel("Channel1", "NickA", 0);
    EXPECT_EQ(res.first, FATAL);

    // チャンネル削除
    res = channel->DeleteChannel("Channel1");
    EXPECT_EQ(res.first, 1);

    // 削除後は存在しない
    ChannelInfo info;
    res = channel->GetChannelInfo("Channel1", info);
    EXPECT_EQ(res.first, ERR_NOSUCHCHANNEL);
}

// チャンネルの Join / Leave のテスト
TEST_F(ChannelTest, JoinAndLeaveChannel) {
    // チャンネル作成
    channel->CreateChannel("Channel1", "NickA", 0);

    // Join チャンネル
    ChannelResult res = channel->JoinedChannel("NickB", "Channel1", 1);
    EXPECT_EQ(res.first, 1);

    // チャンネル情報の確認
    ChannelInfo info;
    res = channel->GetChannelInfo("Channel1", info);
    EXPECT_EQ(res.first, 1);
    EXPECT_TRUE(info.joined_player.find("NickB") != info.joined_player.end());

    // Leave チャンネル
    res = channel->LeaveChannel("NickB", "Channel1");
    EXPECT_EQ(res.first, 1);

    // Leave 後のチャンネル確認
    res = channel->GetChannelInfo("Channel1", info);
    EXPECT_EQ(res.first, 1);
    EXPECT_TRUE(info.joined_player.find("NickB") == info.joined_player.end());
}

// チャンネルの Master 管理のテスト
TEST_F(ChannelTest, AddAndDeleteMaster) {
    channel->CreateChannel("Channel1", "NickA", 0);
    channel->JoinedChannel("NickB", "Channel1");
    // NickA が NickB を Master に追加
    ChannelResult res = channel->AddMaster("NickA", "NickB", "Channel1");
    EXPECT_EQ(res.first, 1);

    // Master に追加されたことを確認
    ChannelInfo info;
    res = channel->GetChannelInfo("Channel1", info);
    EXPECT_EQ(res.first, 1);
    EXPECT_TRUE(info.is_master.find("NickB") != info.is_master.end());

    // NickA が NickB を Master から削除
    res = channel->DeleteMaster("NickA", "NickB", "Channel1");
    EXPECT_EQ(res.first, 1);

    // 削除されたことを確認
    res = channel->GetChannelInfo("Channel1", info);
    EXPECT_EQ(res.first, 1);
    EXPECT_TRUE(info.is_master.find("NickB") == info.is_master.end());
}

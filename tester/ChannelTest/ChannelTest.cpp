// ChannelTest.cpp
#include <gtest/gtest.h>
#include "../includes/Channel.h"
#include "../includes/Everyone.h"

// テスト対象のクラスを取得
IntrusivePtr<Channel> channel = Channel::GetInstance();
IntrusivePtr<Everyone> everyone = Everyone::GetInstance();

TEST(ChannelTest, CreateChannel) {
    everyone->CreateUser("owner", "password123", "host1", "ownerNick", "real1");
    ChannelResult res = channel->CreateChannel("testchannel", "ownerNick", 0);
    ASSERT_EQ(res.first, 1);  // チャンネル作成成功
}

TEST(ChannelTest, DuplicateChannel) {
    ChannelResult res = channel->CreateChannel("testchannel", "ownerNick", 0);
    ASSERT_EQ(res.first, FATAL);  // 重複エラー
}

TEST(ChannelTest, JoinedChannel) {
    everyone->CreateUser("user1", "password123", "host2", "userNick1", "real2");
    ChannelResult res = channel->JoinedChannel("userNick1", "testchannel", 0);
    ASSERT_EQ(res.first, 1);  // チャンネル参加成功
}

TEST(ChannelTest, LeaveChannel) {
    ChannelResult res = channel->LeaveChannel("userNick1", "testchannel");
    ASSERT_EQ(res.first, 1);  // チャンネル脱退成功
}

TEST(ChannelTest, InviteToChannel) {
    everyone->CreateUser("user2", "password123", "host3", "userNick2", "real3");
    ChannelResult res = channel->InviteToChannel("ownerNick", "userNick2", "testchannel");
    ASSERT_EQ(res.first, 1);  // 招待成功
}

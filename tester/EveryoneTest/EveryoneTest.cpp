// EveryoneTest.cpp
#include <gtest/gtest.h>
#include "../includes/Everyone.h"

// テスト対象のクラスを取得
IntrusivePtr<Everyone> everyone = Everyone::GetInstance();

TEST(EveryoneTest, CreateUser) {
    ChannelResult res = everyone->CreateUser("testuser1", "password123", "host1", "nick1", "real1");
    ASSERT_EQ(res.first, 1);  // 成功
}

TEST(EveryoneTest, DuplicateNickname) {
    everyone->CreateUser("testuser2", "password123", "host2", "nick1", "real2");
    ChannelResult res = everyone->CreateUser("testuser3", "password123", "host3", "nick1", "real3");
    ASSERT_EQ(res.first, ERR_NICKNAMEINUSE);  // ニックネームの重複
}

TEST(EveryoneTest, SetNickname) {
    everyone->CreateUser("testuser4", "password123", "host4", "nick4", "real4");
    ChannelResult res = everyone->SetNickname("testuser4", "newnick");
    ASSERT_EQ(res.first, 1);  // ニックネームの変更成功
}

TEST(EveryoneTest, AddBlockUser) {
    everyone->CreateUser("blocker", "password123", "host5", "blockerNick", "real5");
    everyone->CreateUser("blocked", "password123", "host6", "blockedNick", "real6");
    ChannelResult res = everyone->AddBlockUser("blocker", "blocked");
    ASSERT_EQ(res.first, 1);  // ブロック追加成功
}

TEST(EveryoneTest, DeleteUser) {
    everyone->CreateUser("deleteuser", "password123", "host7", "deleteNick", "real7");
    ChannelResult res = everyone->DeleteUser("deleteuser");
    ASSERT_EQ(res.first, 1);  // ユーザー削除成功
}

// EveryoneTest.cpp
#include <gtest/gtest.h>
#include "../includes/Everyone.h"

// // テスト対象のクラスを取得
// IntrusivePtr<Everyone> everyone = Everyone::GetInstance();

// TEST(EveryoneTest, CreateUser) {
//     ChannelResult res = everyone->CreateUser("testuser1", "password123", "host1", "nick1", "real1");
//     ASSERT_EQ(res.first, 1);  // 成功
// }

// TEST(EveryoneTest, DuplicateNickname) {
//     everyone->CreateUser("testuser2", "password123", "host2", "nick1", "real2");
//     ChannelResult res = everyone->CreateUser("testuser3", "password123", "host3", "nick1", "real3");
//     ASSERT_EQ(res.first, ERR_NICKNAMEINUSE);  // ニックネームの重複
// }

// TEST(EveryoneTest, SetNickname) {
//     everyone->CreateUser("testuser4", "password123", "host4", "nick4", "real4");
//     ChannelResult res = everyone->SetNickname("testuser4", "newnick");
//     ASSERT_EQ(res.first, 1);  // ニックネームの変更成功
// }

// TEST(EveryoneTest, AddBlockUser) {
//     everyone->CreateUser("blocker", "password123", "host5", "blockerNick", "real5");
//     everyone->CreateUser("blocked", "password123", "host6", "blockedNick", "real6");
//     ChannelResult res = everyone->AddBlockUser("blocker", "blocked");
//     ASSERT_EQ(res.first, 1);  // ブロック追加成功
// }

// TEST(EveryoneTest, DeleteUser) {
//     everyone->CreateUser("deleteuser", "password123", "host7", "deleteNick", "real7");
//     ChannelResult res = everyone->DeleteUser("deleteuser");
//     ASSERT_EQ(res.first, 1);  // ユーザー削除成功
// }
class EveryoneTest : public ::testing::Test {
    protected:
        void SetUp() override {
            everyone = Everyone::GetInstance();
        }

        void TearDown() override {
        }

        IntrusivePtr<Everyone> everyone;
    };

// ユーザーの作成と削除のテスト
TEST_F(EveryoneTest, CreateAndDeleteUser) {
    // 正常にユーザーを作成
    ChannelResult res = everyone->CreateUser("UserA", "pass", "localhost", "NickA", "RealName");
    EXPECT_EQ(res.first, 1);

    // 同じニックネームでの作成は失敗
    res = everyone->CreateUser("UserB", "pass", "localhost", "NickA", "RealName");
    EXPECT_EQ(res.first, ERR_NICKNAMEINUSE);

    // 作成したユーザーが存在することを確認
    EXPECT_TRUE(everyone->ExistUser("NickA"));

    // ユーザー削除
    res = everyone->DeleteUser("NickA");
    EXPECT_EQ(res.first, 1);

    // 削除後にユーザーが存在しないことを確認
    EXPECT_FALSE(everyone->ExistUser("NickA"));
}

// ニックネームの重複チェック
TEST_F(EveryoneTest, DuplicateNickname) {
    everyone->CreateUser("UserA", "pass", "localhost", "Nick1", "RealName");
    ChannelResult res = everyone->CreateUser("UserB", "pass", "localhost", "Nick1", "RealName");
    EXPECT_EQ(res.first, ERR_NICKNAMEINUSE);

    res = everyone->CreateUser("UserB", "pass", "localhost", "Nick2", "RealName");
    EXPECT_EQ(res.first, 1);
}

// ユーザーのブロック・解除のテスト
TEST_F(EveryoneTest, BlockAndUnblockUser) {
    everyone->CreateUser("UserA", "pass", "localhost", "NickA", "RealA");
    everyone->CreateUser("UserB", "pass", "localhost", "NickB", "RealB");

    // NickA が NickB をブロック
    ChannelResult res = everyone->AddBlockUser("NickA", "NickB");
    EXPECT_EQ(res.first, 1);

    // NickA が NickB のブロックを解除
    res = everyone->DeleteBlockUser("NickA", "NickB");
    EXPECT_EQ(res.first, 1);
}

// ニックネーム変更のテスト
TEST_F(EveryoneTest, ChangeNickname) {
    everyone->CreateUser("UserA", "pass", "localhost", "NickA", "RealName");

    // ニックネームの変更
    ChannelResult res = everyone->SetNickname("NickA", "NewNick");
    EXPECT_EQ(res.first, 1);

    // 新しいニックネームで存在確認
    EXPECT_TRUE(everyone->ExistUser("NewNick"));
    EXPECT_FALSE(everyone->ExistUser("NickA"));
}

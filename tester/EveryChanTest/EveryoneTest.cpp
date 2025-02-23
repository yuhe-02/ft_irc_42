#include <gtest/gtest.h>
#include "Everyone.h"
#include "Channel.h"

class EveryoneTest : public ::testing::Test {
protected:
    void SetUp() override {
        everyone = Everyone::GetInstance();
        channel = Channel::GetInstance();
    }

    void TearDown() override {
    }

    IntrusivePtr<Everyone> everyone;
    IntrusivePtr<Channel> channel;
};

// 1. 複数ユーザの作成と削除の連続操作
TEST_F(EveryoneTest, CreateAndDeleteMultipleUsers) {
    for (int i = 0; i < 100; ++i) {
        std::stringstream ss;
        ss << "User" << i;
        everyone->CreateUser(ss.str(), "pass", "localhost", ss.str(), "RealName");
    }
    for (int i = 0; i < 100; i += 2) {
        std::stringstream ss;
        ss << "User" << i;
        everyone->DeleteUser(ss.str());
    }
    for (int i = 1; i < 100; i += 2) {
        std::stringstream ss;
        ss << "User" << i;
        EXPECT_TRUE(everyone->ExistUser(ss.str()));
    }
}

// 2. ニックネームの重複チェック
TEST_F(EveryoneTest, DuplicateNickname) {
    everyone->CreateUser("UserA", "pass", "localhost", "Nick1", "RealName");
    ChannelResult res = everyone->CreateUser("UserB", "pass", "localhost", "Nick1", "RealName");
    EXPECT_EQ(res.first, ERR_NICKNAMEINUSE);

    res = everyone->CreateUser("UserB", "pass", "localhost", "Nick2", "RealName");
    EXPECT_EQ(res.first, 1);
}

// 3. 複数ユーザ間のブロック操作
TEST_F(EveryoneTest, MultipleBlockUser) {
    everyone->CreateUser("UserA", "pass", "localhost", "NickA", "RealA");
    everyone->CreateUser("UserB", "pass", "localhost", "NickB", "RealB");
    everyone->CreateUser("UserC", "pass", "localhost", "Nick1", "RealC");

    everyone->AddBlockUser("NickA", "NickB");
    everyone->AddBlockUser("NickC", "NickB");

    everyone->DeleteBlockUser("NickA", "NickB");
    // UserC はまだ UserB をブロックしている
    EXPECT_TRUE(everyone->ExistUser("NickB"));
}

// 4. 複数ユーザの Join / Leave Channel 操作の検証
TEST_F(EveryoneTest, JoinLeaveMultipleChannels) {
    for (int i = 0; i < 50; ++i) {
        std::stringstream ss;
        ss << "User" << i;
        everyone->CreateUser(ss.str(), "pass", "localhost", ss.str(), "RealName");
    }
    for (int i = 0; i < 50; ++i) {
        for (int j = 0; j < 10; ++j) {
            std::stringstream user, chan;
            user << "User" << i;
            chan << "Channel" << j;
        	ChannelResult res = channel->CreateChannel(chan.str(), user.str(), 0);
			std::cout << res.first << std::endl;
            res = channel->LeaveChannel(user.str(), chan.str());
			std::cout << res.first << std::endl;
        }
    }
    for (int i = 0; i < 10; ++i) {
        std::stringstream ss;
        ss << "Channel" << i;
        ChannelInfo info;
        ChannelResult res = channel->GetChannelInfo(ss.str(), info);
        EXPECT_EQ(res.first, ERR_NOSUCHCHANNEL); // チャンネルは全て空のはず
    }
}

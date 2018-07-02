//
// Created by roman on 28.06.18.
//

#include "gtest/gtest.h"
#include "woker.h"


TEST(user_registered, add_user)//id должны быть уникальны
{
    Woker wk;
    EXPECT_EQ(true, wk.user_registered(12,"user0"));
    EXPECT_EQ(false, wk.user_registered(12,"user1"));
}

TEST(user_renamed, rename_user)//переименовать юзера можно только после добавления юзера
{
    Woker wk;
    EXPECT_EQ(false, wk.user_renamed(12,"user0"));
    wk.user_registered(12,"dsfd");
    EXPECT_EQ(true, wk.user_renamed(12,"user1"));
}

TEST(user_connected, connect_user)//сделать online юзера можно только после добавления юзера
{
    Woker wk;
    EXPECT_EQ(false, wk.user_connected(12));
    wk.user_registered(12,"dsfd");
    EXPECT_EQ(true, wk.user_connected(12));
    EXPECT_EQ(true, wk.user_connected(12));
}

TEST(user_disconnected, disconnect_user)//сделать offline юзера можно только после добавления юзера
{
    Woker wk;
    EXPECT_EQ(false, wk.user_disconnected(12));
    wk.user_registered(12,"dsfd");
    EXPECT_EQ(true, wk.user_disconnected(12));
    EXPECT_EQ(true, wk.user_disconnected(12));
}

TEST(user_deal_won, deal_won_user)
{
    Woker wk;
    EXPECT_EQ(false, wk.user_deal_won(12,time(NULL),1212));//если юзера нет в базе, то нельзя добавить ему amount
    wk.user_registered(12,"dsfd");
    EXPECT_EQ(true, wk.user_deal_won(12,time(NULL),1212)); // если юзер есть в базе, то все ОК
    EXPECT_EQ(false, wk.user_deal_won(12,time(NULL)-8*24*3600,1212)); //изменить amount для прошедшей недели нельзя
    EXPECT_EQ(true, wk.user_deal_won(12,time(NULL)-60,1212)); //изменить amount для текущей недели можно
    EXPECT_EQ(false, wk.user_deal_won(12,time(NULL)+5,1212)); //изменить amount для текущей недели можно
}

TEST(get_user_top, get_top_user)
{
    Woker wk;
    list<string> lst;
    EXPECT_EQ(false, wk.get_user_top(12,lst));//если юзера нет, в базе, то ТОПа не будет
    wk.user_registered(12,"dsfd");
    EXPECT_EQ(false, wk.get_user_top(12,lst));//если юзер не ОНЛАЙН, то ТОПа не будет
    wk.user_connected(12);
    EXPECT_EQ(true, wk.get_user_top(12,lst));//если юзер ОНЛАЙН, то все ОК
    EXPECT_EQ(false, wk.get_user_top(12,lst));//чаще раза в минуту нельзя запрашивать ТОП для юзера
    sleep(60);
    EXPECT_EQ(true, wk.get_user_top(12,lst));//если с момента запроса ТОПа для юзера прошло >60 сек, то все ОК
}

int main(int argc,char** argv) {

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

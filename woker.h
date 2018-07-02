//
// Created by roman on 27.06.18.
//

#ifndef TOPPER_WOKER_H
#define TOPPER_WOKER_H

#include "user.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ranked_index.hpp>
#include <boost/multi_index/tag.hpp>

using boost::multi_index::indexed_by;
using boost::multi_index_container;
using boost::multi_index::ranked_non_unique;
using boost::multi_index::ranked_unique;
using boost::multi_index::identity;
using boost::multi_index::member;
using boost::multi_index::tag;

using user_multiset = boost::multi_index_container<
        User,
        indexed_by<
                ranked_non_unique<
                        tag<User::ByWeekAmount>, identity<User> >,
                ranked_unique<
                        tag<User::ById>, member<User,uint64_t, &User::id> >
        >
>;

class Woker {
protected:
    mutable int global_number_week; //номер текущей недели
    mutable bool day_changed; //внутрення переменная для того, чтобы инкремент текущей недели не произошел дважды
    user_multiset users; //база

    /*получить номер текущей недели*/
    inline int get_number_current_week() const;
    /*добавлять amount можно только в текущую неделю,те если week_amount больше чем начало недели, то данные будут проигнорированны*/
    inline bool add_amount_rule(time_t week_amount) const;
    /*получить amoutn юзера за текущую неделю*/
    inline uint64_t get_amount_for_current_week(int _number_current_week, user_multiset::iterator _it) const;
    /*получить ТОП (от максимального к минимальному) заданной длины*/
    inline bool get_users_below(user_multiset::iterator _it,user_multiset::iterator _end, int _number_current_week, int _max_counter, int _start_position, list<string> &_lst) const;
    /*получить ТОП (от минимального к максимальному) заданной длины*/
    inline bool get_users_above(user_multiset::iterator _it,user_multiset::iterator _begin, int _number_current_week, int _max_counter, int _start_position, list<string> &_lst) const;
public:
    Woker() : global_number_week(0),day_changed(false) {}

    /*добавить юзера в базу*/
    bool user_registered(uint64_t _id, string _name);
    /*переименовать юзера*/
    bool user_renamed(uint64_t _id, string _name);
    /*сделать юзера онлайн*/
    bool user_connected(uint64_t _id);
    /*сделать юзера офлайн*/
    bool user_disconnected(uint64_t _id);
    /*добавить amount юзеру. */
    bool user_deal_won(uint64_t _id,time_t _time,uint64_t _amount);
    /*выдать ТОП(10 элементов сверху, позицию юзера, +/-10 элементов от юзера)*/
    bool get_user_top(uint64_t _id, list<string> &_lst) const;
};

class TestWokerMode : protected Woker {
    /*создать тестовую базу, заданного размера*/
    bool do_test_data(int size_base);
    /*тест для user_connected() и одновременно user_disconnected()*/
    bool test_change_connect(int size_base, int size_loop);
    /*тест для user_registered*/
    bool test_user_registered(int size_base, int size_loop);
    /*тест для user_renamed*/
    bool test_user_renamed(int size_base, int size_loop);
    /*тест для user_deal_won*/
    bool test_user_deal_won(int size_base, int size_loop);
    /*тест для get_user_top*/
    bool test_get_user_top(int size_base, int size_loop);

public:
    /*запустить нагрузочное тестирование, где size_base - размер создаваемой базы, size_loop - количество прогонов каждой функции*/
    bool run_tests(int size_base, int size_loop);
};

#endif //TOPPER_WOKER_H

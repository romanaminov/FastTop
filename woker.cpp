//
// Created by roman on 27.06.18.
//

#include <iostream>
#include "woker.h"
#include <chrono>

int Woker::get_number_current_week() const {
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);

    if(timeinfo->tm_wday == Monday){ // '1' is a monday
        if(day_changed == false) {
            ++global_number_week;
            day_changed = true;
        }
    }else
        day_changed = false;

    return global_number_week;

}

bool Woker::add_amount_rule(time_t week_amount) const{
    time_t curr_time = time(NULL);
    struct tm *u;
    u = localtime(&curr_time);
    int secs = 0; //seconds from start current week
    secs += u->tm_sec;
    secs += u->tm_min * 60;
    secs += u->tm_hour * 3600;
    int days = (u->tm_wday == 0/*Sunday*/) ? 6 : (u->tm_wday - Monday);
    secs += days*3600;

    time_t week_start = curr_time - secs;
    time_t week_end = week_start + 7*24*3600;
    if(week_start <= week_amount && week_amount < week_end && week_amount <= time(NULL))
        return true;
    else
        return false;
}

bool Woker::user_registered(uint64_t _id, string _name){
    auto &id_index = users.get<User::ById>();
    auto it_1 = id_index.find(_id);
    if(it_1 == id_index.end()){
        User u(_id, _name);
        u.number_week = get_number_current_week();
        auto res1 = users.insert(u).second;
        if(res1){
            return true;
        }
    }
    return false;
}

bool Woker::user_renamed(uint64_t _id, string _name){
    auto &id_index = users.get<User::ById>();
    auto it_1 = id_index.find(_id);
    if(it_1 != id_index.end()){
        it_1->ptr->name = move(_name);
        return true;
    }
    return false;
}

bool Woker::user_connected(uint64_t _id){
    auto &id_index = users.get<User::ById>();
    auto it_1 = id_index.find(_id);
    if(it_1 != id_index.end()){
        it_1->ptr->status_connect = true;
        return true;
    }
    return false;
}

bool Woker::user_disconnected(uint64_t _id) {
    auto &id_index = users.get<User::ById>();
    auto it_1 = id_index.find(_id);
    if(it_1 != id_index.end()){
        it_1->ptr->status_connect = false;
        return true;
    }
    return false;
}

bool Woker::user_deal_won(uint64_t _id, time_t _time, uint64_t _amount) {
    bool res = false;
    auto &id_index = users.get<User::ById>();
    auto it_1 = id_index.find(_id);

    if(it_1 != id_index.end() && add_amount_rule(_time)){
        User u(*it_1);
        it_1->ptr->time_last_amount = _time;
        int number_current_week = get_number_current_week();
        if( number_current_week == it_1->number_week ) {
            u.amount += _amount;
        }else{
            u.number_week = number_current_week;
            u.amount = _amount;
        }
        auto size = id_index.erase(_id);
        if( size != 0) {
            auto it_2 = users.insert(u).first;
            if( it_2 != users.end()) {
                res = true;
            }
        }
    }
    return res;
}

uint64_t Woker::get_amount_for_current_week(int _number_current_week, user_multiset::iterator _it) const{
    uint64_t the_amount = _it->amount;
    if(_number_current_week != _it->number_week)
        the_amount = 0;
    return the_amount;
}

bool Woker::get_users_below(user_multiset::iterator _it,user_multiset::iterator _end, int _number_current_week, int _max_counter, int _start_position, list<string> &_lst) const{
    int counter = 1,
            pos = _start_position;
    while(_it != _end && counter <= _max_counter){
        auto the_amount = get_amount_for_current_week(_number_current_week, _it);
        _lst.push_back("position:" + std::to_string(pos) + " id:" + std::to_string(_it->id) + \
            " amount:" + std::to_string(the_amount) + " number week:" + std::to_string(_number_current_week));

        ++counter;
        ++pos;
        ++_it;
    }
    return true;
}

bool Woker::get_users_above(user_multiset::iterator _it,user_multiset::iterator _begin, int _number_current_week, int _max_counter, int _start_position, list<string> &_lst) const{
    int counter = 1,
            pos = _start_position;
    while(_it != _begin && counter <= _max_counter){
        auto the_amount = get_amount_for_current_week(_number_current_week, _it);
        _lst.push_back("position:" + std::to_string(pos) + " id:" + std::to_string(_it->id) + \
            " amount:" + std::to_string(the_amount) + " number week:" + std::to_string(_number_current_week));

        ++counter;
        --pos;
        --_it;
        if(_it == _begin && counter <= _max_counter){
            _lst.push_back("position:" + std::to_string(pos) + " id:" + std::to_string(_it->id) + \
            " amount:" + std::to_string(the_amount) + " number week:" + std::to_string(_number_current_week));
            break;
        }
    }
    return true;
}

bool Woker::get_user_top(uint64_t _id, list<string> &_lst) const {
    if(!_lst.empty())
        _lst.clear();
    bool res = true;
    int number_current_week = get_number_current_week();
    auto &id_index = users.get<User::ById>();
    auto &week_amount_index = users.get<User::ByWeekAmount>();
    auto it_1 = id_index.find(_id);
    if(it_1 != id_index.end()){
        time_t cur_t = time(NULL);
        time_t diff_time = cur_t - it_1->ptr->time_last_get_top;
        if(it_1->ptr->status_connect && diff_time >= 60){
            int max_counter = 10 , pos = 1;
            it_1->ptr->time_last_get_top = cur_t;
            _lst.push_back("TOP10:");
            res = get_users_below(week_amount_index.begin(),week_amount_index.end(),number_current_week, max_counter, pos, _lst);
            _lst.push_back("");

            auto iit_1 = week_amount_index.find(*it_1);
            if(iit_1 != week_amount_index.end()) {
                auto iit_2 = iit_1;
                auto idx = week_amount_index.rank(iit_1);

                _lst.push_back("User position in TOP:");
                res &= get_users_below(iit_1, week_amount_index.end(), number_current_week, 1, ++idx, _lst);
                _lst.push_back("");

                _lst.push_back("Neighbors above user(10 steps):");
                res &= get_users_above(--iit_1,week_amount_index.begin(), number_current_week, max_counter, --idx,_lst);
                _lst.push_back("");

                _lst.push_back("Neighbors below User (10 steps):");
                res &= get_users_below(++iit_2, week_amount_index.end(), number_current_week, max_counter, (idx+2), _lst);
            }else
                res = false;
        }else
            res = false;
          //  cout<<"dddddddddddddddddddddddddddddddddddddddddd\n";
    }else
        res = false;
    return res;
}

bool TestWokerMode::do_test_data(int size_base){
    bool res = true;
    cout<< "Test base start building..."<<endl;
    for (int  i = 0; i < size_base; ++i) {
        user_registered(i,"user"+std::to_string(i));
    }
    cout<< "Finished."<<endl;
    cout<< "Test base include: "<<users.size() << " users\n" <<endl;
    cout<< "mcs is microseconds."<<endl;
    return res;
}

bool TestWokerMode::test_change_connect(int size_base, int size_loop) {
    bool res = true;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    int elapsed_mcseconds = 0, max = 0, medium = 0;

    for(int i = 0; i < size_loop; ++i){
        int n = rand() % size_base;
        start = std::chrono::system_clock::now();
        res &= user_connected(n);
        res &= user_disconnected(n);
        end = std::chrono::system_clock::now();
        elapsed_mcseconds = std::chrono::duration_cast<std::chrono::microseconds> (end-start).count();
        medium += elapsed_mcseconds;
        max = max > elapsed_mcseconds ? max : elapsed_mcseconds;
    }
    medium /= size_loop;
    cout << "user_connected() and user_disconnected(): max time = " << max <<"mcs, medium time = "<< medium<< "mcs" << endl;
    return res;
}

bool TestWokerMode::test_user_registered(int size_base, int size_loop) {
    bool res = true;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    int elapsed_mcseconds = 0, max = 0, medium = 0;

    for( int i = 0; i < size_loop; ++i ) {
        int n = rand() % size_base + (size_base / 2);
        start = std::chrono::system_clock::now();
        user_registered(n,"user"+std::to_string(i));
        end = std::chrono::system_clock::now();
        elapsed_mcseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        medium += elapsed_mcseconds;
        max = max > elapsed_mcseconds ? max : elapsed_mcseconds;
    }
    medium /= size_loop;
    cout << "user_registered(): max time = " << max << "mcs, medium time = " << medium << "mcs" << endl;
    return res;
}

bool TestWokerMode::test_user_renamed(int size_base, int size_loop){
    bool res = true;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    int elapsed_mcseconds = 0, max = 0, medium = 0;

    for( int i = 0; i < size_loop; ++i ) {
        int n = rand() % size_base;
        start = std::chrono::system_clock::now();
        res &= user_renamed(n,"user"+std::to_string(rand()%size_loop));
        end = std::chrono::system_clock::now();
        elapsed_mcseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        medium += elapsed_mcseconds;
        max = max > elapsed_mcseconds ? max : elapsed_mcseconds;
    }
    medium /= size_loop;
    cout << "user_renamed(): max time = " << max << "mcs, medium time = " << medium << "mcs" << endl;
    return res;
}

bool TestWokerMode::test_user_deal_won(int size_base, int size_loop){
    bool res = true;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    int elapsed_mcseconds = 0, max = 0, medium = 0;

    for( int i = 0; i < size_loop; ++i ) {
        int n = rand() % size_base + (size_base / 2);
        start = std::chrono::system_clock::now();
        user_deal_won(n,time(NULL),rand()%5+1);
        end = std::chrono::system_clock::now();
        elapsed_mcseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        medium += elapsed_mcseconds;
        max = max > elapsed_mcseconds ? max : elapsed_mcseconds;
    }
    medium /= size_loop;
    cout << "user_deal_won(): max time = " << max << "mcs, medium time = " << medium << "mcs" << endl;
    return res;
}

bool TestWokerMode::test_get_user_top(int size_base, int size_loop){
    bool res = true;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    int elapsed_mcseconds = 0, max = 0, medium = 0;

    for( int i = 0; i < size_loop; ++i ) {
        int n = rand() % size_base;
        list<string> lst;
        user_connected(n);
        start = std::chrono::system_clock::now();
        get_user_top(n, lst);
        end = std::chrono::system_clock::now();
        elapsed_mcseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        medium += elapsed_mcseconds;
        max = max > elapsed_mcseconds ? max : elapsed_mcseconds;
        /*if(lst.size()>4){
            cout << lst.size() <<endl;
            for(auto p : lst)
                cout << p << endl;
        }*/
    }
    medium /= size_loop;
    cout << "get_user_top(): max time = " << max << "mcs, medium time = " << medium << "mcs" << endl;
    return res;

}

bool TestWokerMode::run_tests(int size_base, int size_loop){
    bool res = true;
    res &= do_test_data(size_base);
    res &= test_change_connect(size_base, size_loop);
    res &= test_user_registered(size_base, size_loop);
    res &= test_user_renamed(size_base, size_loop);
    res &= test_user_deal_won(size_base, size_loop);
    res &= test_get_user_top(size_base, size_loop);

    string sres = res ? "good :)" : "bad ;(";
    cout << "\nSize loop for every function was " << size_loop << " steps";
    cout << "\nTest result is " << sres << endl << endl;
    return res;
}
//
// Created by roman on 27.06.18.
//

#ifndef TOPPER_USER_H
#define TOPPER_USER_H

#include <cstdint>
#include <string>
#include <ctime>
#include <memory>


using namespace std;

enum {
    Monday = 1 //начало недели, тк в struct tm 0 = Воскресенье
};
class PreUser{
public:
    bool status_connect;        //юзер онлайн или нет
    time_t time_last_amount;    //время последней сделки
    time_t time_last_get_top;   //время последнего запроса топа
    string name;                //имя юзера

    PreUser() = delete;
    PreUser(const string &_name) : status_connect(false), time_last_amount(0),time_last_get_top(0){
        name = move(_name);
    }
};

class User {
public:
    int number_week;            //номер недели
    uint64_t id;                //id юзера
    uint64_t amount;            //количество сделок
    shared_ptr<PreUser> ptr;
public:
    User() = delete;
    User(uint64_t _id,const string &_name) : number_week(0), id(_id), amount(0){
       ptr = make_shared<PreUser>(_name);
    }

    bool operator < (const User & u) const {
        if(number_week == u.number_week){
            if(amount == u.amount){
                return (id > u.id);
            }else
                return (amount > u.amount);
        }else
            return (number_week > u.number_week);
    }

    bool operator == (const User & u) const {
        return (number_week == u.number_week)&&(amount == u.amount)&&(id == u.id);

    }

    struct ByWeekAmount{}; //метка для сортировки по week и amount
    struct ById{};         //метка для сортировки по id
};



#endif //TOPPER_USER_H

//
// Created by yezhuoyang on 2019-04-30.
//
#ifndef SJTU_USER_HPP
#define SJTU_USER_HPP
#include "TRAIN.hpp"
#include "constant.h"
namespace sjtu {
    struct User {
        /*
         * The privilege of the user.
         * 0 unregistered
         * 1 registered user
         * 2 Administrator
         */
        int privilege;
        char name[NAMESIZE];
        char password[PASSSIZE];
        char phone[PHONESIZE];
        char email[EMAILSIZE];
        User()=default;
        User(const char* N){strcpy(name,N);privilege=1;}
        User&operator=(const User &U){
            if(&U==this) return *this;
            privilege=U.privilege;
            strcpy(name,U.name);
            strcpy(password,U.password);
            strcpy(email,U.email);
            strcpy(phone,U.phone);
            return *this;
        }
        void modify(const char*N,const char*P,const char* E,const char* Ph){
            strcpy(name,N);
            strcpy(password,P);
            strcpy(email,E);
            strcpy(phone,Ph);
        }
    };
    bool  operator==(const User&U1,const User&U2){
        return (strcmp(U1.name,U2.name)==0)&&(strcmp(U1.password,U2.password)==0)&&(strcmp(U1.phone,U2.phone)==0)&&(strcmp(U1.email,U2.email)==0);
    }
    /*
     * User buy num tickets. Return 0 if the tickets are not enough.
     * First search the ticket from all ticket that he has already bought.
     * If he hasn't bought such kind of ticket, add it to the ticket list.
     */
    std::ostream &operator<<(std::ostream &os,const User& U){
        os<<" "<<U.name<<" "<<U.password<<" "<<U.email<<" "<<U.phone;
        return os;
    }


};


#endif //SJTU_USER_HPP

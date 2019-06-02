//
// Created by yezhuoyang on 2019-04-30.
//

#ifndef SJTU_TIME_HPP
#define SJTU_TIME_HPP
#include <iostream>
#include "stdio.h"
#include "constant.h"
namespace  sjtu {
    void convert_to_char(char list[],int hour,int minute){
        list[1]='0'+hour-10*(hour/10);
        list[0]='0'+hour/10;
        list[2]=':';
        list[4]='0'+minute-10*(minute/10);
        list[3]='0'+minute/10;
        list[5]='\0';
    }
/*
 * The function convert the string of date "XXXX:XX:ab" to an integer  ab-1
 */
    int date_to_int(const char list[]){
        return 10*(list[8]-'0')+(list[9]-'1');
    }
    struct Date {
        /*
         * Store a date as an integer.
         * 0 denotes 2019-06-01
         */
        short pos;
        Date(){pos=0;}
        Date(const char*D){
            pos=date_to_int(D);
        }
        Date& operator=(const Date& rhs){
            if(&rhs==this) return *this;
            pos=rhs.pos;
            return *this;
        }
    };
    Date operator+(const Date& d,const int&k){
        Date tmp(d);
        tmp.pos+=k;
        return tmp;
    }
//时间类
    struct Time {
        short hour;
        short minute;
        Time(const char* T){
            char tmp[6];
            strcpy(tmp,T);
            if(strcmp(T,"XX:XX")==0){
                hour=minute=0;
            }
            else{
                hour=10*tmp[0]+tmp[1];
                minute=10*tmp[3]+tmp[4];
            }
        }
        Time(){
            hour=minute=-1;
        }
        Time(const Time&rhs) {
            hour = rhs.hour;
            minute = rhs.minute;
        }
        Time& operator=(Time& rhs){
            if(&rhs==this) return *this;
            hour=rhs.hour;
            minute=rhs.minute;
            return *this;
        }
        Time& operator=(const Time& rhs){
            if(&rhs==this) return *this;
            hour=rhs.hour;
            minute=rhs.minute;
            return *this;
        }
        int operator-(const Time& rhs){
            if(rhs.hour>hour){
                return (hour+24-rhs.hour)*60+minute-rhs.minute;
            }
            return (hour-rhs.hour)*60+minute-rhs.minute;
        }
    };
    std::ostream &operator<<(std::ostream& os,const Date& D){
        if(D.pos==30){
            char tmp[15]={"2019-07-01"};
            os<<tmp;
            return os;
        }
        char tmp[15]={"2019-06-00"};
        int x=(D.pos+1)/10;
        int y=D.pos+1-10*x;
        tmp[strlen(tmp)-1]='0'+y;
        tmp[strlen(tmp)-2]='0'+x;
        tmp[strlen(tmp)]=0;
        os<<tmp;
        return os;
    }
    std::istream &operator>>(std::istream& is,Date& D){
        char tmp[15]={"2019-06-00"};
        is>>tmp;
        D.pos=date_to_int(tmp);
        return is;
    }
    std::ostream& operator<<(std::ostream &os,const Time& T){
        char tmp[7]={"xx:xx"};
        if(T.hour<0){
            os<<tmp;
            return os;
        }
        int a=T.hour/10;
        int b=T.hour-10*a;
        int c=T.minute/10;
        int d=T.minute-10*c;
        tmp[0]='0'+a;
        tmp[1]='0'+b;
        tmp[strlen(tmp)-2]='0'+c;
        tmp[strlen(tmp)-1]='0'+d;
        os<<tmp;
        return os;
    }
    std::istream& operator>>(std::istream &is,Time& T){
        char tmp[6];
        is>>tmp;
        if(strcmp(tmp,"xx:xx")==0){
            T.hour=T.minute=-1;
        }
        else{
            T.hour=10*(tmp[0]-'0')+(tmp[1]-'0');
            T.minute=10*(tmp[strlen(tmp)-2]-'0')+(tmp[strlen(tmp)-1]-'0');
        }
        return is;
    }


    bool operator>(const Date&D1,const Date &D2){
        return D1.pos>D2.pos;
    }
    bool operator<(const Date&D1,const Date &D2){
        return D1.pos<D2.pos;
    }
    bool operator==(const Date&D1,const Date &D2){
        return D1.pos==D2.pos;
    }

    bool operator>(const Time &T1,const Time &T2){
        if(T1.hour>T2.hour) return true;
        if(T1.hour<T2.hour) return false;
        if(T1.minute>T2.minute) return true;
        return false;
    }
    bool operator<(const Time &T1,const Time &T2){
        if(T1.hour<T2.hour) return true;
        if(T1.hour>T2.hour) return false;
        if(T1.minute<T2.minute) return true;
        return false;
    }

    bool operator==(const Time &T1,const Time &T2){
        return (T1.hour==T2.hour&&T1.minute==T2.minute);
    }


}

#endif

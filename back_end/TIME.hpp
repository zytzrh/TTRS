//
// Created by yezhuoyang on 2019-04-30.
//

#ifndef SJTU_TIME_HPP
#define SJTU_TIME_HPP
#include <iostream>
#include "stdio.h"
#include "constant.h"



namespace  sjtu {
    /*
     * The function convert hour ab and minute cd to string "ab:cd"
     */
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
        char date[13];
        /*
         * Store a date as an integer.
         * 0 denotes 2019-06-01
         */
        int  pos;
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
        int hour;
        int minute;
        char time[6];
        Time(const char* T){
            strcpy(time,T);
            if(strcmp(T,"XX:XX")==0){
                hour=minute=0;
            }
            else{
                hour=10*time[0]+time[1];
                minute=10*time[3]+time[4];
            }
        }
        Time(){
            strcpy(time,"XX:XX");
            hour=minute=0;
        }
        Time(const Time&rhs) {
            strcpy(time, rhs.time);
            hour = rhs.hour;
            minute = rhs.minute;
        }
        Time& operator=(Time& rhs){
            if(&rhs==this) return *this;
            strcpy(time,rhs.time);
            hour=rhs.hour;
            minute=rhs.minute;
            return *this;
        }
        Time& operator=(const Time& rhs){
            if(&rhs==this) return *this;
            strcpy(time,rhs.time);
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
        os<<T.time;
        return os;
    }
    std::istream& operator>>(std::istream &is,Time& T){
        is>>T.time;
        if(strcmp(T.time,"XX:XX")==0){
            T.hour=T.minute=0;
        }
        else{
            T.hour=10*(T.time[0]-'0')+(T.time[1]-'0');
            T.minute=10*(T.time[3]-'0')+(T.time[4]-'0');
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
        return strcmp(T1.time,T2.time)>0;
    }
    bool operator<(const Time &T1,const Time &T2){
        return strcmp(T1.time,T2.time)<0;
    }
    bool operator==(const Time &T1,const Time &T2){
        return strcmp(T1.time,T2.time)==0;
    }




}

#endif 

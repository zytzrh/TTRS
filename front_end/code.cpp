//
// Created by yezhuoyang on 2019-05-15.
//


#include "../back_end/terminal.hpp"
using namespace sjtu;
int main(){
    char r_path[100]="/Users/yezhuoyang/Desktop/TTR/data/";
    char User_file[100];
    strcpy(User_file,r_path);
    char train_file[100];
    strcpy(train_file,r_path);
    char train_id[100];
    strcpy(train_id,r_path);
    char order[100];
    strcpy(order,r_path);
    char orderid[100];
    strcpy(orderid,r_path);
    char ticket[100];
    strcpy(ticket,r_path);
    char ticketid[100];
    strcpy(ticketid,r_path);
    char station[100];
    strcpy(station,r_path);
    char remain[100];
    strcpy(remain,r_path);
    strcat(User_file,"User.txt");
    strcat(train_file,"train.txt");
    strcat(order,"order.txt");
    strcat(ticket,"ticket.txt");
    strcat(station,"station.txt");
    strcat(remain,"remain.txt");
    terminal T(User_file,train_file,train_id,order,orderid,ticket,ticketid,station,remain);
    int c=1;
    while(c){
        c=T.execute();
    }
}



//
// Created by yezhuoyang on 2019-05-14.
//
#ifndef SJTU_PROCESSOR_HPP
#define SJTU_PROCESSOR_HPP
#include "TRAIN.hpp"
#include "TIME.hpp"
#include "vector.hpp"
#include "link.hpp"
#include "constant.h"
namespace sjtu{
    char MaxC[10];
    char  MinC[2];
    int current_id;
    int Register(const char *name, const char *pass, const char *email, const char *phone){
        User U;
        U.modify(name, pass, email, phone);
        //TODO How to insert User properly?
        User_Bpp[current_id]=U;
        return current_id++;
    }


    int terminal::put_on_sale(const char *tid) {
        Train T;
        Trainkey key(tid);
        int result = Train_Bpp.count(key);
        if(!result) return 0;
        T=Train_Bpp[key];
        if (T.For_sale) return 0;
        T.For_sale = true;
        Ticket tmp;
        /*
         * This step copy the information: the number of tickets and their relative name, prices.
         */
        tmp.copy(T,tid);
        Ticketkey key1(tid, T.catalog);
        key1.set_loc1(T.station[0].loc);
        tmp.start_time = T.station[0].start_time;
        tmp.copy_price(T.station[0]);
        for (int i = 1; i < T.station_num; ++i) {
            key1.set_loc2(T.station[i].loc);
            tmp.arrive_time = T.station[i].arrive_time;
            Ticket_Bpp1[key1]=tmp;
            Ticket_Bpp2[key1]=tmp;
            key1.set_loc1(T.station[i].loc);
            tmp.start_time = T.station[i].start_time;
            tmp.copy_price(T.station[i]);
        }
        return 1;
    }



    void query_transfer(const char *loc1, const char *loc2, const Date &D, const char *cat){

        sjtu::vector<Ticket> V1;
        sjtu::vector<Ticket> V2;

        sjtu::vector<Ticketkey> K1;
        sjtu::vector<Ticketkey> K2;


        Ticketkey minkey1(loc1,"\0",MinC,cat);
        Ticketkey maxkey1(loc1,"\0",MaxC,cat);
        Ticket_Bpp1.findblock(V1,K1,minkey1,maxkey1);


        Ticketkey minkey2("\0",loc2,MinC,cat);
        Ticketkey maxkey2("\0",loc2,MaxC,cat);
        Ticket_Bpp2.findblock(V2,K2,minkey2,maxkey2);

        sjtu::vector<Ticket>::iterator it1=V1.begin();
        sjtu::vector<Ticketkey>::iterator it2=K1.begin();
        std::cout<<"Start station:"<<std::endl;
        for(int i=0;i<V1.size();++i){
            std::cout<<(*it2).train_id<<" "<<(*it2).loc1<<" "<<D<<" "<<(*it1).start_time<<" "<<(*it2).loc2<<" "<<D<<" "<<(*it1).arrive_time<<" ";
            for(int j=0;j<(*it1).price_num;++j){
                std::cout<<(*it1).price_name[j]<<" "<<(*it1).remain[j][D.pos]<<" "<<(*it1).price_list[j]<<" ";
            }
            std::cout<<std::endl;
            ++it1;++it2;
        }
        it1=V2.begin();
        it2=K2.begin();
        std::cout<<"Arrive station:"<<std::endl;
        for(int i=0;i<V2.size();++i){
            std::cout<<(*it2).train_id<<" "<<(*it2).loc1<<" "<<D<<" "<<(*it1).start_time<<" "<<(*it2).loc2<<" "<<D<<" "<<(*it1).arrive_time<<" ";
            for(int j=0;j<(*it1).price_num;++j){
                std::cout<<(*it1).price_name[j]<<" "<<(*it1).remain[j][D.pos]<<" "<<(*it1).price_list[j]<<" ";
            }
            std::cout<<std::endl;
            ++it1;++it2;
        }
        std::cout<<"Comparing!"<<std::endl;
        Ticket start;
        Ticketkey startkey;
        Ticket arrive;
        Ticketkey arrivekey;
        int mintime=1e9;
        sjtu::vector<Ticket>::iterator vit2=V1.begin();
        sjtu::vector<Ticketkey>::iterator kit2=K1.begin();
        sjtu::vector<Ticket>::iterator vit1=V1.begin();
        sjtu::vector<Ticketkey>::iterator kit1=K1.begin();
        for(int i=0;i<V1.size();++i){
            vit2=V2.begin();
            kit2=K2.begin();
            for(int j=0;j<V2.size();++j){
                std::cout<<"compare"<<(*kit1).loc2<<"and"<<(*kit2).loc1<<std::endl;
                if(strcmp((*kit1).loc2,(*kit2).loc1)==0) {
                    if(!((*vit1).arrive_time>(*vit2).start_time)){
                        if(((*vit2).arrive_time)-((*vit1).start_time)<mintime){
                            mintime=(*vit2).arrive_time-(*vit1).start_time;
                            start=(*vit1);
                            arrive=(*vit2);
                            startkey=(*kit1);
                            arrivekey=(*kit2);
                        }
                    }
                }
                ++vit2;++kit2;
            }
            ++vit1;++kit1;
        }
        std::cout<<mintime<<std::endl;
        std::cout<<startkey.train_id<<" "<<startkey.loc1<<" "<<D<<" "<<start.start_time<<" "<<startkey.loc2<<" "<<D<<" "<<start.arrive_time<<" ";
        for(int j=0;j<start.price_num;++j){
            std::cout<<start.price_name[j]<<" "<<start.remain[j][D.pos]<<" "<<start.price_list[j]<<" ";
        }
        std::cout<<std::endl;
        std::cout<<arrivekey.train_id<<" "<<arrivekey.loc1<<" "<<D<<" "<<arrive.start_time<<" "<<arrivekey.loc2<<" "<<D<<" "<<arrive.arrive_time<<" ";
        for(int j=0;j<arrive.price_num;++j){
            std::cout<<arrive.price_name[j]<<" "<<arrive.remain[j][D.pos]<<" "<<arrive.price_list[j]<<" ";
        }
        std::cout<<std::endl;
    }


    int query_profile(const int &id){
        int result;
        User U;
        result=User_Bpp.count(id);
        if (!result) return 0;
        U=User_Bpp[id];
        std::cout << U.name << " " << U.email<<" "<<U.phone<< " " << U.privilege << std::endl;
        return 1;
    }


/*
 *   Query the ticket that User id has bought
 */
    int query_order(const int &id, const Date &date, const char *catalog){
        int result;
        User U;
        result=User_Bpp.count(id);
        if (!result) return -1;
        U=User_Bpp[id];
        /*
         * The num of tickets found that satisfies the requirements.
         */
        int num_found = 0;
        Ticketbought ticket_found[10];
        for (int i = 0; i < U.num_ticket; ++i) {
            if (U.ticket[i].D==date && strcmp(U.ticket[i].catalog,catalog) == 0) {
                ticket_found[num_found] = U.ticket[i];
                ++num_found;
            }
        }
        std::cout << num_found << std::endl;
        /*
         * Print all information of these tickets
         */
        for (int i = 0; i < num_found; ++i) {
            std::cout << ticket_found[i] << std::endl;
        }
        return 1;
    }




    int buy_ticket(const int &id, const int &num, const char *train_id, const char *loc1, const char *loc2, const Date &D,
                   const char *ticket_kind){
        int result;
        User U;
        Ticket T;
        result = User_Bpp.count(id);
        if (!result) return 0;
        U=User_Bpp[id];
        Ticketkey key(loc1,loc2,train_id,"\0");
        result = Ticket_Bpp1.count(key);
        if (!result) {
            return 0;
        }
        T=Ticket_Bpp1[key];
        result = U.add_ticket(T,train_id, T.catalog, loc1, loc2, D, ticket_kind, num);
        if (!result) return 0;
        User_Bpp[id]=U;
        Ticket_Bpp1[key]=T;
        Ticket_Bpp2[key]=T;

        return 1;
    }


    int refund_ticket(const int &id, const int &num, const char *train_id, const char *loc1, const char *loc2,
                      const Date &D, const char *ticket_kind) {
        int result;
        User U;
        Ticket T;
        result = User_Bpp.count(id);
        if (!result) return 0;
        U=User_Bpp[id];
        Ticketkey key(loc1, loc2, train_id,"\0");
        result = Ticket_Bpp1.count(key);
        if (!result) return 0;
        T=Ticket_Bpp1[key];
        std::cout<<"Refund!"<<std::endl;

        result = U.refund_ticket(T, num, train_id, loc1, loc2, D, ticket_kind);
        if (!result) return 0;


        User_Bpp[id]=U;
        Ticket_Bpp1[key]=T;
        Ticket_Bpp2[key]=T;
        return 1;
    }



    int add_train(const char *train_id, const char *name, const char *catalog, const int &nums, const int &nump) {
        int result;
        Train T;
        Trainkey key(train_id);
        result=Train_Bpp.count(key);
        if (result) return 0;
        strcpy(T.name, name);
        strcpy(T.catalog, catalog);
        T.station_num = nums;
        T.price_num = nump;
        for (int i = 0; i < nump; ++i){
            std::cin >> T.price_name[i];
        }
        for (int i = 0; i < nums; ++i) {
            T.station[i].type_num=nump;
            std::cin >> T.station[i];
        }
        Train_Bpp[key]=T;
        return 1;
    }


    int modify_train(const char *train_id, const char *name, const char *catalog, const int &nums, const int &nump) {
        int result;
        Train T;
        Trainkey key(train_id);
        result=Train_Bpp.count(key);
        if (!result) return 0;
        T=Train_Bpp[key];
        strcpy(T.name, name);
        strcpy(T.catalog, catalog);
        T.station_num=nums;
        T.price_num = nump;
        for (int i = 0; i < nums; ++i) {
            T.station[i].type_num = nump;
            std::cin >> T.station[i];
        }
        Train_Bpp[key]=T;
        return 1;
    }


    int delete_train(const char *tid){
        int result;
        Trainkey key(tid);
        map<Trainkey,Train,compare_train>::iterator   it=Train_Bpp.find(key);//TODO
        if(it==Train_Bpp.end()) return 0;
        Train_Bpp.erase(it);
        return 1;
    }



    int log_in(const int &id, const char *pass){
        int result;
        User U;
        result=User_Bpp.count(id);
        if(!result) return 0;
        U=User_Bpp[id];
        if (strcmp(U.password, pass) != 0) return 0;
        return 1;
    }



    int modify_profile(const int &id, const char *name, const char *password, const char *email, const char *phone){
        int result;
        User U;
        result=User_Bpp.count(id);
        if (!result) return 0;
        U=User_Bpp[id];
        U.modify(name, password, email, phone);
        User_Bpp[id]=U;
        return 1;
    }




    int modify_privilege(const int &id1, const int &id2, const int &privilege) {
        int result;
        User U1, U2;
        result = User_Bpp.count(id1);
        if (!result) return 0;
        U1=User_Bpp[id1];
        result = User_Bpp.count(id2);
        if (!result) return 0;
        U2=User_Bpp[id2];
        if (U1.privilege < 2) return 0;
        if (U2.privilege == 2&&privilege<2) return 0;
        U2.privilege = privilege;
        User_Bpp[id2]=U2;
        return 1;
    }



/*
 * Print all the information of train id.
 */
    int query_train(const char *tid){
        int result;
        Train T;
        Trainkey key(tid);
        result = Train_Bpp.count(key);
        if (!result) return 0;
        T=Train_Bpp[key];
        std::cout << tid << " " << T.name << " " << T.catalog << " " << T.station_num << " " << T.price_num << " ";
        for (int i = 0; i < T.price_num; ++i) {
            std::cout << T.price_name[i] << " ";
        }
        for (int i = 0; i < T.station_num; ++i) {
            std::cout << T.station[i] << std::endl;
        }
        std::cout << std::endl;
        return 1;
    }


/*
 * TODO query_ticket
 */

    int query_ticket(const char *lc1, const char *lc2, const Date &D, const char *cat){
        Ticketkey minkey(lc1,lc2,MinC,cat);
        Ticketkey maxkey(lc1,lc2,MaxC,cat);
        sjtu::vector<Ticket> V1;
        sjtu::vector<Ticketkey> V2;
        Ticket_Bpp1.findblock(V1,V2,minkey,maxkey);
        if(V1.empty()) return 0;
        std::cout<<V1.size()<<std::endl;
        compare_ticket1 comp;
        sjtu::vector<Ticket>::iterator it1=V1.begin();
        sjtu::vector<Ticketkey>::iterator it2=V2.begin();
        for(int i=0;i<V1.size();++i){
            std::cout<<(*it2).train_id<<" "<<(*it2).loc1<<" "<<D<<" "<<(*it1).start_time<<" "<<(*it2).loc2<<" "<<D<<" "<<(*it1).arrive_time<<" ";
            for(int j=0;j<(*it1).price_num;++j){
                std::cout<<(*it1).price_name[j]<<" "<<(*it1).remain[j][D.pos]<<" "<<(*it1).price_list[j]<<" ";
            }
            std::cout<<std::endl;
            ++it1;++it2;
        }
        return 0;
    }



}







#endif

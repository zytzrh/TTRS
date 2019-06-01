//
// Created by yezhuoyang on 2019-05-19.
//

#ifndef TRAIN_TICKET_PROJECT_MAPPROCESSOR_HPP
#define TRAIN_TICKET_PROJECT_MAPPROCESSOR_HPP
#include "../map/map.hpp"
#include "TRAIN.hpp"
#include "TIME.hpp"
#include "USER.hpp"




namespace sjtu{
    extern map<int, User> User_Bpp;
    extern map<Trainkey,Train,compare_train> Train_Bpp;
    extern map<Ticketkey,Ticket,compare_ticket> Ticket_Bpp;
/*
 * Current minimum user id that has not been registered.
 */
    extern current_id;
/*
 * Current User.
 */
    extern User current_U;

//TODO
    int Register(const char *name, const char *pass, const char *email, const char *phone) {
        User U;
        ++current_id;
        U.modify(name, pass, email, phone);
        //TODO How to insert User properly?
        User_Bpp.insert(current_id, U);
        return 1;
    }

/*TODO
 * Put the Train T on sale
 *
 *
 */
    int put_on_sale(const char *tid) {
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
        tmp.copy(T);
        Ticketkey key1(tid, T.catalog);
        key1.set_loc1(T.station[0].loc);
        tmp.start_time = T.station[0].start_time;
        for (int i = 1; i < T.station_num; ++i) {
            key1.set_loc2(T.station[i].loc);
            tmp.arrive_time = T.station[i].arrive_time;
            Ticket_Bpp.insert(key, tmp);
            key1.set_loc1(T.station[i].loc);
            tmp.start_time = T.station[i].start_time;
        }
        return 1;
    }


/*
 * TODO
 */
    void query_transfer(const char *loc1, const char *loc2, const Date &date, const char *catalog) {


    }



    int query_profile(const int &id) {
        int result;
        User U;
        int result = Train_Bpp.count(id);
        if (!result) return 0;
        U=Train_Bpp[id];
        std::cout << U.name << " " << U.email << " " << U.privilege << std::endl;
        return 1;
    }



/*
 *   Query the ticket that User id has bought
 */



    int query_order(const int &id, const Date &date, const char *catelog){
        int result;
        User U;
        result=Uesr_Bpp.count(id);
        if (!result) return -1;
        U=Uesr_Bpp.count[id];
        /*
         * The num of tickets found that satisfies the requirements.
         */
        int num_found = 0;
        Ticketbought ticket_found[10];
        for (int i = 0; i < U.num_ticket; ++i) {
            if (U.ticket[i].date == D && strcmp(U.ticket[i].catalog, catalog) == 0) {
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


/*
 * TODO
 */
    int buy_ticket(const int &id, const int &num, const char *train_id, const char *loc1, const char *loc2, const Date &D,
               const char *ticket_kind) {
        int result;
        User U;
        Ticket T;
        result = Uesr_Bpp.count(id);
        if (!result) return 0;
        U=Uesr_Bpp[id];

        Ticketkey key(loc1, loc2, train_id);
        result = Ticket_Bpp.count(key);
        if (!result) return 0;
        T=Train_Bpp[key];


        result = U.add_ticket(T, train_id, T.catalog, loc1, loc2, D, ticket_kind, num);
        if (!result) return 0;

        User_Bpp.insert(id, U);
        Ticket_Bpp.insert(key, T);
        return 1;
    }


    int refund_ticket(const int &id, const int &num, const char *train_id, const char *loc1, const char *loc2,
                      const Date &D, const char *ticket_kind) {
        int result;
        User U;
        Ticket T;
        result = Uesr_Bpp.count(id);
        if (!result) return 0;
        U=Uesr_Bpp[id];
        Ticketkey key(loc1, loc2, train_id);
        result = Ticket_Bpp.count(key);
        if (!result) return 0;
        T=Ticket_Bpp[key];
        result = U.refund_ticket(T, num, train_id, loc1, loc2, D, ticket_kind);
        if (!result) return 0;
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
        for (int i = 0; i < nump; ++i)
            std::cin >> T.price_name[i];
        for (int i = 0; i < nums; ++i) {
            std::cin >> T.station[i];
        }
        Train_Bpp.insert(key, T);
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
        return 1;
    }


    int delete_train(const char *tid){
        int result;
        Trainkey key(tid);
        map<Trainkey,Train,compare_train()>::iterator it=Train_Bpp.find(key);
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
        return 1;
    }


    int modify_privilege(const int &id1, const int &id2, const int &privilege) {
        int result;
        User U1, U2;
        result = User_Bpp.count(id1);
        if (!result) return 0;
        U1=User_Bpp[id1];
        result = User_Bpp.find(id2);
        if (!result) return 0;
        U2=User_Bpp[id2];
        if (U1.privilege < 2) return 0;
        if (U2.privilege == 2 && privilege < 2) return 0;
        U2.privilege = privilege;
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
        T=Train_Bpp.count[key];
        std::cout << tid << " " << T.name << " " << T.catalog << " " << T.station_num << " " << T.price_num << " ";
        for (int i = 0; i < T.price_num; ++i) {
            std::cout << T.price_name[i] << " ";
        }
        std::cout << endl;
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
       return 0;
    }

}



#endif //TRAIN_TICKET_PROJECT_MAPPROCESSOR_HPP

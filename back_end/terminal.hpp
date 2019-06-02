//
// This is the class that determines the input and output of terminal
//
#ifndef TRAIN_TICKET_PROJECT_TERMINAL_HPP
#define TRAIN_TICKET_PROJECT_TERMINAL_HPP
#include "TIME.hpp"
#include "TRAIN.hpp"
#include "bplus.hpp"
#include "string.h"
#include "constant.h"
#include "USER.hpp"
namespace sjtu{
    void  print(const Trainkey& Tkey,const Train& T){
        std::cout<<Tkey.train_id<<" "<<T.name<<std::endl;
    }
    void print1(const myTicketkey& Tkey,const myTicket& T){
        std::cout<<Tkey.tid<<" "<<Tkey.loc<<" "<<T.K<<" "<<" "<<T.catlog<<std::endl;
    }
    void print3(const myOrderkey& Okey,const myOrder& O){
        std::cout<<Okey.train_id<<" "<<Okey.Uid<<" "<<Okey.D<<" "<<O.catalog<<" "<<O.x<<" "<<O.y<<std::endl;
    }
    class terminal{
        list<User>  User_list;
        bptree<Trainkey,Train> Train_bpp;
        bptree<myTicketkey,myTicket> MyTicket_bpp;
        bptree<myOrderkey,myOrder> MyOrder_bpp;
        mylink<short,Station> Station_link;
        int current_id;
        char input[20];
        int  id;
        int  num;
        int id1,id2;
        char  privilege;
        char name[NAMESIZE];
        char password[PASSSIZE];
        char phone[PHONESIZE];
        char email[EMAILSIZE];
        Date date;
        char loc1[LOCSIZE],loc2[LOCSIZE];
        char ticket_kind[PRICESIZE];
        char train_id[IDSIZE];
        char catalog[CATSIZE];
        int result;
        int nums,nump;
        char  Trainfile[FILENAME];char Trainidfile[FILENAME];
        char  myOrderfile[FILENAME];char myOrderidfile[FILENAME];
        char  myTicketfile[FILENAME];char myTicketidfile[FILENAME];
        /*
         * Enumerate type for different operation
         * Reg:      register          *name* *password* *email* *phone*
         * Log:      login             *id* *password*
         * Qp:       query_profile     *id*
         * Mpro:     modify_profile    *id* *name* *password* *email* *phone*
         * Mpri:     modify_privilege  *id1* *id2* *privilege*
         * Qti:      query_ticket      *loc1* *loc2* *date* *catalog*
         * Qtrs:     query_transfer    *loc1* *loc2* *date* *catalog*
         * Bt:       buy_ticket        *id* *num* *train_id* *loc1* *loc2* *date* *ticket_kind*
         * Qo:       query_order       *id* *date* *catalog*
         * Rt:       refund_ticket     *id* *num* *train_id* *loc1* *loc2* *date* *ticket_kind*
         * At:       add_train         *train_id* *name* *catalog* *num(station)* *num(price)* *(name(price) ) xnum(price)*
                                           *[name time(arriv) time(start) time(stopover) (price) x num(price) ] x num(station)*
         * St:       sale_train        *train_id*
         * Qtra:     query_train       *train_id*
         * Dt:       delete_train      *train_id*
         * Mt:       modify_train      *train_id* *name* *catalog* *num(station)* *num(price)* *(name(price) ) xnum(price)*
                                           *[name time(arriv) time(start) time(stopover) (price) x num(price) ] x num(station)*
         * Cl:       clean
         * Ex:       exit
         */
        enum type{
            Reg,Log,Qp,Mpro,Mpri,Qti,Qtrs,Bt,Qo,Rt,At,St,Qtra,Dt,Mt,Cl,Ex
        };
        type currenttype;
        short remain_value(const sjtu::vector<short>& V,const int&d,const int &N,const int& P,const int& n,const int& x,const int &y){
            int R=d*N*(P-1)+n*(P-1);
            if(y==x+1){
                return V[R+x];
            }
            short result=2000;
            for(int i=x;i<y;++i){
                result=result<V[R+i]?result:V[R+i];
            }
            return result;
        }
        void del_remain(vector<short>& V,const int&d,const int& K,const int&k,const int& P, const int& x,const int& y,const short& num){
            int R = d*K*(P-1)+k*(P-1);
            for(int i=x;i<y;++i){
                V[R + i] -= num;
            }
        }
        int Register(const char *name, const char *pass, const char *email, const char *phone);

        int log_in(const int& id,const char* password);

        int query_profile(const int& id);

        int modify_profile(const int&id,const char* name,const char* password,const char* email,const char* phone);

        int modify_privilege(const int& id1,const int& id2,const char&privilege);

        int query_ticket(const char* loc1,const char*loc2,const Date& date,const char* catalog);

        int query_transfer(const char* loc1,const char* loc2,const Date& date,const char* catalog);

        int buy_ticket(const int&id,const int& num,const char*train_id,const char*loc1,const char*loc2,const Date& date,const char*ticket_kind);

        int query_order(const int& id,const Date& date,const char*catalog);

        int refund_ticket(const int& id,const int& num,const char* train_id,const char* loc1,const char* loc2,const Date& date,const char* ticket_kind);

        int add_train(const char* ticket_id,const char* name,const char* catalog,const int& nums,const int& nump);

        int put_on_sale(const char *tid);

        int query_train(const char *tid);

        int delete_train(const char *tid);

        int modify_train(const char*train_id,const char* name,const char* catalog,const int&nums,const int& nump);


        void print_ticket(const char* tid,const  sjtu::vector<Station>& ST,const Train& T,const sjtu::vector<short>& RM,const Date& d,const int&x,const int&y);


        /*
   * 返回x-y车站剩余车票数
   */
    public:
        terminal(const char* uf,const char* Tf,const char* Tfid,const char* of,const char* ofid,const char* tf,const char* tfid,
                 const char* stationfile):User_list(uf,UBSIZE),Train_bpp(Tf),MyOrder_bpp(of),MyTicket_bpp(tf),
                                          Station_link(stationfile,4096){
            strcpy(Trainfile,Tf);
            strcpy(Trainidfile,Tfid);
            strcpy(myOrderfile,of);
            strcpy(myOrderidfile,ofid);
            strcpy(myTicketfile,tf);
            strcpy(myTicketidfile,tfid);
            current_id=User_list.Size()+FIRSTID;
        }
        ~terminal(){
        }
        void clear(){
            current_id=FIRSTID;
            User_list.clear();
            Train_bpp.init();
            MyOrder_bpp.init();
            MyTicket_bpp.init();
            Station_link.clear();
        }
        int execute(){
            strcpy(input,"\0");
            std::cin>>input;
            if(strcmp(input,"register")==0){
                currenttype=Reg;
                std::cin>>name>>password>>email>>phone;
                result=Register(name,password,email,phone);
                if(!result)
                    std::cout<<result<<std::endl;
                else
                    std::cout<<current_id-1<<std::endl;
            }
            else if(strcmp(input,"login")==0){
                currenttype=Log;
                std::cin>>id>>password;
                result=log_in(id,password);
                std::cout<<result<<std::endl;
            }
            else if(strcmp(input,"query_profile")==0){
                currenttype=Qp;
                std::cin>>id;
                result=query_profile(id);
                if(!result) std::cout<<result<<std::endl;
            }
            else if(strcmp(input,"modify_profile")==0){
                currenttype=Mpro;
                std::cin>>id>>name>>password>>email>>phone;
                result=modify_profile(id,name,password,email,phone);
                std::cout<<result<<std::endl;
            }
            else if(strcmp(input,"modify_privilege")==0){
                currenttype=Mpri;
                std::cin>>id1>>id2>>privilege;
                result=modify_privilege(id1,id2,privilege);
                std::cout<<result<<std::endl;
            }
            else if(strcmp(input,"query_ticket")==0){
                currenttype=Qti;
                std::cin>>loc1>>loc2>>date>>catalog;
                result=query_ticket(loc1,loc2,date,catalog);
                if(!result) std::cout<<0<<std::endl;
            }
            else if(strcmp(input,"query_transfer")==0){
                currenttype=Qtrs;
                std::cin>>loc1>>loc2>>date>>catalog;
                result=query_transfer(loc1,loc2,date,catalog);
                if(!result) std::cout<<-1<<std::endl;
            }
            else if(strcmp(input,"buy_ticket")==0){
                currenttype=Bt;
                std::cin>>id>>num>>train_id>>loc1>>loc2>>date>>ticket_kind;
                result=buy_ticket(id,num,train_id,loc1,loc2,date,ticket_kind);
                std::cout<<result<<std::endl;
            }
            else if(strcmp(input,"query_order")==0){
                currenttype=Qo;
                std::cin>>id>>date>>catalog;
                result=query_order(id,date,catalog);
                if(result==0)std::cout<<result<<std::endl;
            }
            else if(strcmp(input,"refund_ticket")==0) {
                currenttype=Rt;
                std::cin>>id>>num>>train_id>>loc1>>loc2>>date>>ticket_kind;
                result=refund_ticket(id,num,train_id,loc1,loc2,date,ticket_kind);
                std::cout<<result<<std::endl;
            }
            else if(strcmp(input,"add_train")==0){
                currenttype=At;
                std::cin>>train_id>>name>>catalog>>nums>>nump;
                result=add_train(train_id,name,catalog,nums,nump);
                std::cout<<result<<std::endl;
            }
            else if(strcmp(input,"sale_train")==0) {
                currenttype = St;
                std::cin >> train_id;
                result = put_on_sale(train_id);
                std::cout << result << std::endl;
            }
            else if(strcmp(input,"query_train")==0){
                currenttype=Qtra;
                std::cin>>train_id;
                result=query_train(train_id);
                if(!result) std::cout<<result<<std::endl;
            }
            else if(strcmp(input,"delete_train")==0){
                currenttype=Dt;
                std::cin>>train_id;
                result=delete_train(train_id);
                std::cout<<result<<std::endl;
            }
            else if(strcmp(input,"modify_train")==0){
                currenttype=Mt;
                std::cin>>train_id>>name>>catalog>>nums>>nump;
                result=modify_train(train_id,name,catalog,nums,nump);
                std::cout<<result<<std::endl;
            }
            else if(strcmp(input,"clean")==0){
                currenttype=Cl;
                clear();
                std::cout<<1<<std::endl;
            }
            else if(strcmp(input,"exit")==0){
                currenttype=Ex;
                std::cout<<"BYE"<<std::endl;
            }
            else if(strcmp(input,"traverse_Train")==0){
                Train_bpp.traverse(print);
            }
            else if(strcmp(input,"T_O")==0){
                MyOrder_bpp.traverse(print3);
            }
            else if(strcmp(input,"T_Tic")==0){
                MyTicket_bpp.traverse(print1);
            }
            if(currenttype==Ex) return 0;
            else return 1;
        }
    };

    int terminal::Register(const char *name, const char *pass, const char *email, const char *phone){
        User U;
        U.modify(name, pass, email, phone);
        U.privilege = '1';
        if (current_id == FIRSTID) U.privilege = '2';
        //读取失败！
        User_list.push_back(U);
        ++current_id;
        return 1;
    }

    int terminal::put_on_sale(const char *tid){
        Trainkey K(tid);
        Train T=Train_bpp.find(K);
        if(!T.price_num||T.For_sale){
            return 0;
        }
        T.For_sale=true;
        Train_bpp.set(K,T);
        return 1;
    }

    int terminal::query_transfer(const char *loc1, const char *loc2, const Date &D, const char *cat){
        myTicketkey K1("\0",loc1);
        myTicketkey K2("\0",loc2);
        vector<pair<myTicketkey,myTicket>> V10;
        vector<pair<myTicketkey,myTicket>> V20;
        MyTicket_bpp.search(V10,K1,search_myticket());
        MyTicket_bpp.search(V20,K2,search_myticket());
        vector<pair<myTicketkey,myTicket>> V1;
        vector<pair<myTicketkey,myTicket>> V2;
        /*
         * 确定符合catalog要求的票
         */
        for(int i=0;i<V10.size();++i){
            if(strstr(cat,V10[i].second.catlog)!=NULL)
                V1.push_back(V10[i]);
        }
        for(int i=0;i<V20.size();++i){
            if(strstr(cat,V20[i].second.catlog)!=NULL)
                V2.push_back(V20[i]);
        }
        //分别去找到对应的Train
        sjtu::vector<Train> T1list;
        sjtu::vector<Train> T2list;
        for(int i=0;i<V1.size();++i){
            T1list.push_back(Train_bpp.find(Trainkey(V1[i].first.tid)));
        }
        for(int j=0;j<V2.size();++j){
            T2list.push_back(Train_bpp.find(Trainkey(V2[j].first.tid)));
        }
        int mintime=1e9;
        //存放文件读取的Station信息
        sjtu::vector<Station> ST1;
        sjtu::vector<Station> ST2;
        //存放最优的两个车票的位置
        int x=-1,y=-1;
        //存放两个最优车票的中转位置
        int k1,k2;
        for(int i=0;i<V1.size();++i){
            ST1.clear();
            Station_link.read_block(T1list[i].stblock,ST1);
            for(int j=0;j<V2.size();++j){
                /*
                 * 如果是同辆列车，就不考虑了
                 */
                if(strcmp(V2[j].first.tid,V1[i].first.tid)==0)continue;
                ST2.clear();
                Station_link.read_block(T2list[j].stblock,ST2);
                for(int k=V1[i].second.K;k<T1list[i].station_num;++k){
                    for(int q=0;q<V2[j].second.K;++q){
                        if(strcmp(ST1[k].loc,ST2[q].loc)==0&&ST1[k].arrive_time<ST2[q].start_time){
                            if((ST2[q].arrive_time-ST1[k].start_time)<mintime){
                                mintime=ST1[k].arrive_time-ST2[q].start_time;
                                x=i;y=j;k1=k;k2=q;
                            }
                        }
                    }
                }
            }
        }
        //无符合条件的车票
        if(x==-1) return 0;
        /*
         *
         * 最优车票信息位置为:
         * T1list[x]  V1[x].second.K~k1
         * T2list[y]  k2~V2[y].second.K
         *
         */
        ST1.clear();
        ST2.clear();
        /*
         * 从文件中读取剩余车票信息
         */
        sjtu::vector<short> R1;
        sjtu::vector<short> R2;
        Station_link.read_block(T1list[x].stblock,R1,ST1);
        Station_link.read_block(T2list[y].stblock,R2,ST2);
        /*
         * 计算票价信息
         */
        double P1[PRICENUM];
        double P2[PRICENUM];
        for(int i=0;i<T1list[x].price_num;++i){
            P1[i]=0;
            for(int j=V1[x].second.K+1;j<=k1;++j){
                P1[i]+=ST1[j].price[i];
            }
        }
        for(int i=0;i<T2list[y].price_num;++i){
            P2[i]=0;
            for(int j=k2+1;j<=V2[y].second.K;++j){
                P2[i]+=ST2[j].price[i];
            }
        }
        /*
         *  输出票价信息,要检查表达式是否正确
         */
        std::cout<<V1[x].first.tid<<" "<<ST1[V1[x].second.K].loc<<" "<<D+ST1[V1[x].second.K].ad<<" "<<ST1[V1[x].second.K].start_time<<" ";
        std::cout<<ST1[k1].loc<<" "<<D+ST1[k1].ad<<" "<<ST1[k1].arrive_time<<" ";
        for(int i=0;i<T1list[x].price_num;++i){
            std::cout<<T1list[x].price_name[i]<<" "<<remain_value(R1,D.pos,T1list[x].price_num,T1list[x].station_num,i,V1[x].second.K,k1)<<" "<<P1[i]<<" ";
        }
        std::cout<<std::endl;
        std::cout<<V2[y].first.tid<<" "<<ST2[k2].loc<<" "<<D+ST2[k2].ad<<" "<<ST2[k2].start_time<<" ";
        std::cout<<ST2[V2[y].second.K].loc<<" "<<D+ST2[V2[y].second.K].ad<<" "<<ST2[V2[y].second.K].arrive_time<<" ";
        for(int i=0;i<T2list[y].price_num;++i){
            std::cout<<T2list[y].price_name[i]<<" "<<remain_value(R2,D.pos,T2list[y].price_num,T2list[y].station_num,i,k2,V2[y].second.K)<<" "<<P2[i]<<" ";
        }
        std::cout<<std::endl;
        return 1;
    }
    int terminal::query_profile(const int &id){
        User U;
        if (!User_list.find(id-FIRSTID,U)) return 0;
        std::cout << U.name << " " << U.email<<" "<<U.phone<< " " << U.privilege << std::endl;
        return 1;
    }

    /*
    *   Query the ticket that User id has bought
    */
    int terminal::query_order(const int &id, const Date &date, const char *catalog){
        myOrderkey ok(id,date,"\0");
        sjtu::vector<pair<myOrderkey,myOrder>> V;
        sjtu::vector<pair<myOrderkey,myOrder>> Vfound;
        MyOrder_bpp.search(V,ok,search_myorder());
        for(int i=0;i<V.size();++i){
            if(strstr(catalog,V[i].second.catalog)!=NULL){
                Vfound.push_back(V[i]);
            }
        }
        if(Vfound.empty()) return 0;
        std::cout<<Vfound.size()<<std::endl;
        Train T;
        sjtu::vector<Station> ST;
        sjtu::vector<short> RT;
        double P[PRICENUM];
        /*
         *输出所有找到的Order信息
         */
        for(int i=0;i<Vfound.size();++i){
            ST.clear();
            RT.clear();
            T=Train_bpp.find(Trainkey(Vfound[i].first.train_id));
            Station_link.read_block(T.stblock,RT,ST);
            /*
             * 计算所有票价信息
             */
            for(int j=0;j<T.price_num;++j){
                P[j]=0;
                for(int k=Vfound[i].second.x+1;k<=Vfound[i].second.y;++k){
                    P[j]+=ST[k].price[j];
                }
            }
            /*
             *
             */
            std::cout<<Vfound[i].first.train_id<<" "<<ST[Vfound[i].second.x].loc<<" "<<date+ST[Vfound[i].second.x].ad<<" "<<ST[Vfound[i].second.x].start_time<<" ";
            std::cout<<ST[Vfound[i].second.y].loc<<" "<<date+ST[Vfound[i].second.y].ad<<" "<<ST[Vfound[i].second.y].arrive_time<<" ";
            for(int j=0;j<T.price_num;++j){
                if(j==Vfound[i].second.K){
                    std::cout<<T.price_name[j]<<" "<<Vfound[i].second.num<<" "<<P[j]<<" ";
                }
                else{
                    std::cout<<T.price_name[j]<<" "<<0<<" "<<P[j]<<" ";
                }
            }
            std::cout<<std::endl;
        }
        return 1;
    }


    int terminal::buy_ticket(const int&id,const int& num,const char*train_id,const char*loc1,const char*loc2,const Date& date,const char*ticket_kind){
        if((id-FIRSTID)>User_list.Size()){
            return 0;
        }
        Train T=Train_bpp.find(Trainkey(train_id));
        if(!T.station_num||!T.For_sale) return 0;
        //确定是哪种票
        int k=0;
        while(k<T.price_num&&strcmp(T.price_name[k],ticket_kind)!=0) ++k;
        if(k==T.price_num) return 0;
        /*
        * 读入车站的信息
        */
        sjtu::vector<Station> V;
        sjtu::vector<short> V1;
        Station_link.read_block(T.stblock,V1,V);
        /*
         * 确定票的对应车站位置
         */
        int x=-1,y=-1;
        for(int i=0;i<V.size();++i){
            if(strcmp(V[i].loc,loc1)==0) x=i;
            if(strcmp(V[i].loc,loc2)==0) y=i;
        }
        if(x==-1||y==-1) return 0;
        /*
         * 读入剩余车票的信息
         */
        if(remain_value(V1,date.pos,T.price_num,T.station_num,k,x,y)<num) return 0;
        /*
         * 减去相应的剩余车票
         */
        del_remain(V1,date.pos,T.price_num,k,T.station_num,x,y,num);
        Station_link.modify(T.stblock,V1);
        myOrderkey ok(id,date,train_id);
        myOrder O;
        O.x=x;O.y=y;
        O.num=num;
        O.K=k;
        strcpy(O.catalog,T.catalog);
        MyOrder_bpp.insert(ok, O);
        return 1;
        /*
        myOrder O=MyOrder_bpp.find(ok);
        if(O.sum==0){
            O.x=x;O.y=y;
            O.sum=num;
            O.num[k]=num;
            strcpy(O.catalog,T.catalog);
            MyOrder_bpp.insert(ok, O);
        }
        else if(O.sum>0){
            O.num[k]+=num;
            O.sum+=num;
            MyOrder_bpp.set(ok,O);
        }
        return 1;
         */
    }

    int terminal::refund_ticket(const int& id,const int& num,const char* train_id,const char* loc1,const char* loc2,const Date& date,const char* ticket_kind){
        myOrderkey okey(id,date,train_id);
        myOrder mO=MyOrder_bpp.find(okey);
        if(!mO.num) {
            return 0;
        }
        Train T=Train_bpp.find(Trainkey(train_id));
        if(!T.station_num){
            return 0;
        }
        //确定是哪种票
        int k=0;
        while(k<T.price_num&&strcmp(T.price_name[k],ticket_kind)!=0) ++k;
        if(k==T.price_num||mO.K!=k||mO.num<num) return 0;
        mO.num-=num;
        sjtu::vector<Station> V;
        sjtu::vector<short> V1;
        Station_link.read_block(T.stblock,V1,V);
        //确定票的对应车站位置
        int x=-1,y=-1;
        for(int i=0;i<V.size();++i){
            if(strcmp(V[i].loc,loc1)==0) x=i;
            if(strcmp(V[i].loc,loc2)==0) y=i;
        }
        if(x==-1||y==-1) return 0;
        del_remain(V1,date.pos,T.price_num,k,T.station_num,x,y,-num);
        Station_link.modify(T.stblock,V1);
        //如果这个order买的总票数为0了，就删掉
        if(!mO.num) MyOrder_bpp.remove(okey);
        else{
            MyOrder_bpp.set(okey,mO);
        }
        return 1;
    }

    int terminal::add_train(const char *train_id, const char *name, const char *catalog, const int &nums, const int &nump){
        Trainkey K(train_id);
        Train T=Train_bpp.find(K);
        if(T.station_num){
            return 0;
        }
        strcpy(T.name, name);
        strcpy(T.catalog, catalog);
        T.station_num=nums;
        T.price_num=nump;
        Station tmp;
        for (int i=0;i<nump;++i){
            std::cin >> T.price_name[i];
        }
        //第一班车的日期就是当前日期
        int predate=0;
        //上一班车的发车时间
        int prehour=-1;
        vector<Station> V;
        for(int i = 0; i < nums; ++i){
            tmp.type_num=nump;
            std::cin >>tmp;
            if(i>0&&(tmp.arrive_time.hour<prehour||(tmp.start_time.hour<tmp.arrive_time.hour))){
                ++predate;
            }
            prehour=tmp.start_time.hour;
            tmp.ad=predate;
            V.push_back(tmp);
        }
        vector<short> V1;
        for(int j=0;j<30;++j){
            for(int p=0;p<T.price_num;++p){
                for(int i=0;i<T.station_num-1;++i){
                    V1.push_back(2000);
                }
            }
        }
        T.stblock=Station_link.push_back(V1,V);
        myTicket mT(true,T.catalog);
        /*
         * 所有经过某站点loc的信息都要存到 MyTicket_bpp中去
         */
        for(int i=0;i<T.station_num;++i){
            /*
             * 用K标记这张票的loc对应的是该train的哪个station
             */
            mT.K=i;
            MyTicket_bpp.insert(myTicketkey(train_id,V[i].loc),mT);

            //myTicket MT=MyTicket_bpp.find(myTicketkey(train_id,V[i].loc));
            //if(!MT.exist) std::cout<<"插票失败！"<<std::endl;

        }
        Train_bpp.insert(K,T);
        return 1;
    }

    int terminal::modify_train(const char *train_id, const char *name, const char *catalog, const int &nums, const int &nump){
        Trainkey K(train_id);
        Train T=Train_bpp.find(K);
        if(!T.station_num||T.For_sale) {
            return 0;
        }
        /*
         *把需要删掉的ticket删去
         */
        vector<Station> V;
        vector<short>   R;
        Station_link.read_block(T.stblock,R,V);
        for(int i=0;i<T.station_num;++i){
            MyTicket_bpp.remove(myTicketkey(train_id,V[i].loc));
        }
        V.clear();
        strcpy(T.name, name);
        strcpy(T.catalog, catalog);
        T.station_num = nums;
        T.price_num = nump;
        Station tmp;
        for (int i = 0; i < nump; ++i){
            std::cin >> T.price_name[i];
        }
        //第一班车的日期就是当前日期
        int predate=0;
        //上一班车的发车时间
        int prehour=-1;
        for (int i = 0; i < nums; ++i) {
            tmp.type_num=nump;
            std::cin >>tmp;
            if(tmp.arrive_time.hour<prehour)++predate;
            prehour=tmp.start_time.hour;
            tmp.ad=predate;
            V.push_back(tmp);
        }
        vector<short> V1;
        for(int j=0;j<30;++j){
            for(int p=0;p<T.price_num;++p){
                for(int i=0;i<T.station_num-1;++i){
                    V1.push_back(2000);
                }
            }
        }
        T.stblock=Station_link.push_back(V1,V);
        myTicket mT(true,T.catalog);
        /*
         * 所有经过某站点loc的信息都要存到 MyTicket_bpp中去
         */
        for(int i=0;i<T.station_num;++i){
            /*
             * 用K标记这张票的loc对应的是该train的哪个station
             */
            mT.K=i;
            MyTicket_bpp.insert(myTicketkey(train_id,V[i].loc),mT);

            //myTicket MT=MyTicket_bpp.find(myTicketkey(train_id,V[i].loc));
            //if(!MT.exist) std::cout<<"插票失败！"<<std::endl;
        }
        Train_bpp.set(K,T);
        return 1;
    }
    /*
    * 删掉车次
    */
    int terminal::delete_train(const char *tid){
        Trainkey K(tid);
        Train T=Train_bpp.find(K);
        if(!T.station_num||T.For_sale) {return 0;}
        vector<Station> V;
        vector<short>     R;
        Station_link.read_block(T.stblock,R,V);

        for(int i=0;i<T.station_num;++i){
            MyTicket_bpp.remove(myTicketkey(tid,V[i].loc));
        }
        Train_bpp.remove(K);
        return 1;
    }


    int terminal::log_in(const int &id, const char *pass){
        User U;
        if (!User_list.find(id-FIRSTID,U)) return 0;
        if (strcmp(U.password, pass) != 0) return 0;
        return 1;
    }



    int terminal::modify_profile(const int &id, const char *name, const char *password, const char *email, const char *phone){
        User U;
        if(!User_list.find(id-FIRSTID,U)) return 0;
        U.modify(name, password, email, phone);
        User_list.modify(id-FIRSTID,U);
        return 1;
    }



    int terminal::modify_privilege(const int &id1, const int &id2, const char &privilege){
        User U1, U2;
        if (!User_list.find(id1-FIRSTID,U1)) return 0;
        if(!User_list.find(id2-FIRSTID,U2)) return 0;
        if (U1.privilege < '2') return 0;
        if (U2.privilege == '2'&&privilege<'2') return 0;
        U2.privilege = privilege;
        User_list.modify(id2-FIRSTID,U2);
        return 1;
    }



    int terminal::query_train(const char *tid) {
        Trainkey K(tid);
        Train T = Train_bpp.find(K);
        if (!T.station_num || !T.For_sale) return 0;
        std::cout << tid << " " << T.name << " " << T.catalog << " " << T.station_num << " " << T.price_num << " ";
        for (int i = 0; i < T.price_num; ++i) {
            std::cout << T.price_name[i] << " ";
        }
        std::cout << std::endl;
        sjtu::vector<short> V1;
        sjtu::vector<Station> V;
        Station_link.read_block(T.stblock,V1,V);
        for (int i = 0; i < V.size(); ++i) {
            std::cout << V[i] << std::endl;
        }
        return 1;
    }



    void terminal::print_ticket(const char* tid,const  sjtu::vector<Station>& ST,const Train& T,const sjtu::vector<short>& RM,const Date& d,const int&x,const int&y){
        std::cout<<tid<<" "<<ST[x].loc<<" "<<d+ST[x].ad<<" "<<ST[x].start_time<<" ";
        std::cout<<ST[y].loc<<" "<<d+ST[y].ad<<" "<<ST[y].arrive_time<<" ";
        /*
         * 计算需要输出的价格
         */
        double P[PRICENUM];
        for(int i=0;i<T.price_num;++i){
            P[i]=0;
            for(int j=x+1;j<=y;++j){
                P[i]+=ST[j].price[i];
            }
        }
        for(int i=0;i<T.price_num;++i){
            std::cout<<T.price_name[i]<<" "<<remain_value(RM,d.pos,T.price_num,T.station_num,i,x,y)<<" "<<P[i]<<" ";
        }
        std::cout<<std::endl;
    }



    int terminal::query_ticket(const char *lc1, const char *lc2, const Date &D, const char *cat){
        myTicketkey K1("\0",lc1);
        myTicketkey K2("\0",lc2);
        vector<pair<myTicketkey,myTicket>> V10;
        vector<pair<myTicketkey,myTicket>> V20;
        MyTicket_bpp.search(V10,K1,search_myticket());
        MyTicket_bpp.search(V20,K2,search_myticket());
        vector<pair<myTicketkey,myTicket>> V1;
        vector<pair<myTicketkey,myTicket>> V2;
        for(int i=0;i<V10.size();++i){
            if(strstr(cat,V10[i].second.catlog)!=NULL)
                V1.push_back(V10[i]);
        }
        for(int i=0;i<V20.size();++i){
            if(strstr(cat,V20[i].second.catlog)!=NULL)
                V2.push_back(V20[i]);
        }
        //最后找到的符合条件的Train的vector
        vector<Trainkey> F;
        /*
         * P 用来记录我确定的票是从哪一站到哪一站
         */
        vector<pair<int,int>> P;
        for(int i=0;i<V1.size();++i){
            for(int j=0;j<V2.size();++j){
                /*
                 * 注意该ticket是从loc1到loc2要求loc1对应的站次比loc2的小
                 */
                if(strcmp(V1[i].first.tid,V2[j].first.tid)==0&&(V1[i].second.K<V2[j].second.K)){
                    F.push_back(Trainkey(V1[i].first.tid));
                    P.push_back(pair<int,int>(V1[i].second.K,V2[j].second.K));
                }
            }
        }
        if(F.empty()) return 0;
        std::cout<<F.size()<<std::endl;
        sjtu::vector<Station> ST;
        sjtu::vector<short> RM;
        Train T;
        for(int i=0;i<F.size();++i){
            ST.clear();
            RM.clear();
            T=Train_bpp.find(F[i]);
            Station_link.read_block(T.stblock,RM,ST);
            print_ticket(F[i].train_id,ST,T,RM,D,P[i].first,P[i].second);
        }
        return 1;
    }


}











#endif //TRAIN_TICKET_PROJECT_TERMINAL_HPP
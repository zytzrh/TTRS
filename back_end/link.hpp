//
// Created by yezhuoyang on 2019-05-25.
//

#ifndef DEBUG_LINK_HPP
#define DEBUG_LINK_HPP
#include "../STLITE/vector.hpp"
#include "../STLITE/deque.hpp"
#include<vector>
#include "constant.h"
namespace sjtu{
    //这是一个用来存储外存某一块区域的大小、位置的结构体
    struct block{
        size_t Size;
        size_t pos;
        block(const size_t &S=0,const size_t& P=0):Size(S),pos(P){}
    };
    template<class value>
    class link{
    private:
        const size_t  blocksize;
        char  filename[FILENAME];
        value buffer[BUFFERSIZE];
        int  bufpos;
        int  bufsize;
        const int maxbuf;
        int Sz;
        FILE* F;
    public:
        link(const char* f,int MB):blocksize(sizeof(value)),maxbuf(MB){
            strcpy(filename, f);
            F = fopen(filename, "rb+");
            if (F == NULL) {
                F = fopen(filename, "wb+");
            }
            fseek(F,0,SEEK_END);
            Sz = ftell(F) / blocksize;
            bufpos = Sz;
            bufsize = 0;
        }
        ~link(){
            flushbuffer();
            fclose(F);
        }
        block push_back(vector<value>& V){
            int preSz=Sz;
            if(V.size()>=maxbuf){
                flushbuffer();
                fseek(F,0,SEEK_END);
                value buffer[V.size()];
                size_t pos=0;
                for(typename  vector<value>::iterator it=V.begin();it!=V.end();++it){
                    buffer[pos++]=(*it);
                }
                fwrite(buffer,blocksize,V.size(),F);
                fflush(F);
                Sz+=V.size();
                return block(V.size(),preSz);
            }
            if(bufsize+V.size()>=maxbuf){
                flushbuffer();
            }
            for(typename  vector<value>::iterator it=V.begin();it!=V.end();++it){
                buffer[bufsize++]=(*it);
            }
            Sz+=V.size();
            return block(V.size(),preSz);
        }
        int Size(){
            return Sz;
        }
        void clear(){
            Sz=0;
            bufpos=Sz;
            bufsize=0;
            freopen(filename,"wb+",F);
        }
        void read_block(const block&B,vector<value>& V){
            if(B.pos>=bufpos&&(B.pos+B.Size)<=(bufpos+bufsize)){
                for(int i=0;i<B.Size;++i){
                    V.push_back(buffer[i+B.pos-bufpos]);
                }
                return;
            }
            fseek(F,B.pos*blocksize,SEEK_SET);
            value buffer[B.Size];
            fread(buffer,blocksize,B.Size,F);
            for(int i=0;i<B.Size;++i){
                V.push_back(buffer[i]);
            }
        }
        void flushbuffer(){
            //std::cout<<"Flush"<<std::endl;
            fseek(F,0,SEEK_END);
            fwrite(buffer,blocksize,bufsize,F);
            fflush(F);
            bufsize=0;
            bufpos=Sz;
        }
        void modify(const block&B,vector<value> V){
            flushbuffer();
            value A[V.size()];
            for(int i=0;i<V.size();++i){
                A[i]=V[i];
            }
            fseek(F,B.pos*blocksize,SEEK_SET);
            fwrite(A,blocksize,V.size(),F);
            fflush(F);
        }
    };

    template<class value>
    class list{
    private:
        char  filename[FILENAME];
        FILE* F;
        int Sz;
        const size_t blocksize;
        value buffer[BUFFERSIZE];
        //缓存区首对应的元素位置
        int   bufpos;
        //缓存区大小
        int   bufsize;
        const int maxbuf;
    public:
        list(const char* FN,const int& MB):blocksize(sizeof(value)),maxbuf(MB){
            bufsize=0;
            strcpy(filename,FN);
            F=fopen(filename,"rb+");
            if(F==NULL){
                F=fopen(filename,"wb+");
            }
            fseek(F,0,SEEK_END);
            Sz=ftell(F)/blocksize;
            bufpos=Sz;
        }
        ~list(){
            flushbuffer();
            fclose(F);
        }

        int Size(){
            return Sz;
        }
        void clear(){
            freopen(filename,"wb+",F);
            Sz=0;
            bufsize=bufpos=0;
        }
        int push_back(const value& V){
            if(bufsize>=maxbuf){
                flushbuffer();
            }
            buffer[bufsize++]=V;
            ++Sz;
            return 1;
        }
        int modify(const int&pos,const value& V){
            if(pos>=Sz) return 0;
            if(pos>=bufpos){
                buffer[pos-bufpos]=V;
                return 1;
            }
            fseek(F,pos*blocksize,SEEK_SET);
            int cp=ftell(F);
            fwrite(&V,blocksize,1,F);
            fflush(F);
            return 1;
        }

        int find(const int&pos,value &V){
            if(pos>=Sz) return 0;
            if(pos>=bufpos){
                V=buffer[pos-bufpos];
                return 1;
            }
            fseek(F,pos*blocksize,SEEK_SET);
            fread(&V,blocksize,1,F);
            return 1;
        }
        void flushbuffer(){
            fseek(F,0,SEEK_END);
            fwrite(buffer,blocksize,bufsize,F);
            fflush(F);
            bufsize=0;
            bufpos=Sz;
        }
    };


    struct myblock{
        size_t  pos;
        size_t  S1;
        size_t  S2;
        myblock(const size_t &P=0,const size_t& s1=0,const size_t&s2=0){
            pos=P;
            S1=s1;
            S2=s2;
        }
    };





    template<class value1,class value2>
    class mylink{
    private:
        //管理缓冲区信息
        struct flushblock{
            size_t p1;
            size_t p2;
            size_t S1;
            size_t S2;
            flushblock(const size_t&P1=0,const size_t& P2=0,const size_t& s1=0,const size_t& s2=0){
                p1=P1;
                p2=P2;
                S1=s1;
                S2=s2;
            }
        };
        /*
         * 返回当前文件末尾字节数
         */
        size_t Cp;
        const size_t  bs1;
        const size_t  bs2;
        /*
         * 缓冲区的最大字节数
         */
        const size_t MB;
        /*
         * 两个类型在缓冲区的个数
         */
        size_t bufsize1;
        size_t bufsize2;
        char  filename[100];
        value1  buffer1[10000];
        value2  buffer2[10000];
        sjtu::vector<flushblock> flushlist;
        FILE* F;
    public:
        mylink(const char* f,const size_t M):bs1(sizeof(value1)),bs2(sizeof(value2)),MB(M){
            strcpy(filename,f);
            F=fopen(filename,"rb+");
            if(F==NULL){
                F=fopen(filename,"wb+");
            }
            fseek(F,0,SEEK_END);
            bufsize1=0;
            bufsize2=0;
            Cp=ftell(F);
        }
        ~mylink(){
            flushbuffer();
            fclose(F);
        }
        myblock push_back(sjtu::vector<value1>& V1,sjtu::vector<value2> V2){
            if((bufsize1+V1.size())*bs1>MB||(bufsize2+V1.size())*bs2>MB){
                flushbuffer();
            }
            for(int i=0;i<V1.size();++i){
                buffer1[bufsize1+i]=V1[i];
            }
            for(int i=0;i<V2.size();++i){
                buffer2[bufsize2+i]=V2[i];
            }
            flushlist.push_back(flushblock(bufsize1,bufsize2,V1.size(),V2.size()));
            bufsize1+=V1.size();
            bufsize2+=V2.size();
            size_t  cp=Cp;
            Cp+=(V1.size()*bs1+V2.size()*bs2);
            return myblock(cp,V1.size(),V2.size());
        }
        int Size(){
            return Cp;
        }
        void clear(){
            Cp=0;
            flushlist.clear();
            freopen(filename,"wb+",F);
        }
        void read_block(const myblock&B,sjtu::vector<value1>& V1,sjtu::vector<value2> &V2){
            flushbuffer();
            fseek(F,B.pos,SEEK_SET);
            char * tmp=new char[B.S1*bs1+B.S2*bs2+1];
            fread(tmp,1,B.S1*bs1+B.S2*bs2,F);
            memcpy(buffer1,tmp,B.S1*bs1);
            memcpy(buffer2,tmp+B.S1*bs1,B.S2*bs2);
            /*
            fread(buffer1,bs1,B.S1,F);
            fread(buffer2,bs2,B.S2,F);
            */
            for(int i=0;i<B.S1;++i){
                V1.push_back(buffer1[i]);
            }
            for(int j=0;j<B.S2;++j){
                V2.push_back(buffer2[j]);
            }

            delete[] tmp;
        }
        void read_block(const myblock&B,sjtu::vector<value1>& V1){
            flushbuffer();
            fseek(F,B.pos,SEEK_SET);
            fread(buffer1,bs1,B.S1,F);
            for(int i=0;i<B.S1;++i){
                V1.push_back(buffer1[i]);
            }
        }
        void read_block(const myblock&B,sjtu::vector<value2>& V2){
            flushbuffer();
            fseek(F,B.pos+B.S1*bs1,SEEK_SET);
            fread(buffer2,bs2,B.S2,F);
            for(int i=0;i<B.S2;++i){
                V2.push_back(buffer2[i]);
            }
        }
        void flushbuffer(){
            if(flushlist.empty()) return;
            fseek(F,0,SEEK_END);
            for(int i=0;i<flushlist.size();++i){
                fwrite(buffer1+flushlist[i].p1,bs1,flushlist[i].S1,F);
                fwrite(buffer2+flushlist[i].p2,bs2,flushlist[i].S2,F);
            }
            bufsize1=0;
            bufsize2=0;
            flushlist.clear();
            fflush(F);
        }

        /*
         * 只有可能修改第value1
         */
        void modify(const myblock&B,sjtu::vector<value1> V){
            flushbuffer();
            value1 A[V.size()];
            for(int i=0;i<V.size();++i){
                A[i]=V[i];
            }
            fseek(F,B.pos,SEEK_SET);
            fwrite(A,bs1,V.size(),F);
            fflush(F);
        }
    };















}






#endif //DEBUG_LINK_HPP

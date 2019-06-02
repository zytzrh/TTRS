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





}






#endif //DEBUG_LINK_HPP

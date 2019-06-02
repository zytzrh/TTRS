//
// Created by zjhz-zyt on 2019/5/18.
//

#ifndef BTREE_BPTREE_HPP
#define BTREE_BPTREE_HPP
#include <iostream>
#include <functional>
#include <cstring>
#include <cmath>
#include "../STLITE/exceptions.h"
#include "../STLITE/vector.hpp"
#include "../STLITE/utility.hpp"
constexpr off_t invalid_off = 0xdeadbeef;
//
template <class key_t, class value_t, size_t node_size = 4096, size_t poolsize = 577, size_t supersize = 866638860,
        class Compare = std::less<key_t>>
class bptree{
    const char tree_t = '0';
    const char block_t = '1';
    //const char blank_t = '2';
private:
    char* quickbuf;
    size_t quicksize;
    const size_t quick_max;
    bool isquick;
    off_t fa_link[100];
    size_t fa_link_pos[100];
    int pfa_link;
    size_t alsize;
    off_t blank_end;
    off_t blank_begin;
    off_t  head;
    off_t  tail;
    off_t  root;
    FILE * file;
    char * filename;

    struct tnode{
        //key_t key;
        off_t pos;   //”¶∏√ø…“‘…æ»•
        size_t sz = 0;
        tnode( off_t pos = invalid_off, size_t sz = invalid_off)
                :pos(pos),sz(sz){}
    };
    struct block{
        off_t pos;
        size_t sz;
        off_t next;
        block(off_t pos = invalid_off, size_t sz = invalid_off, off_t next = invalid_off)
                :pos(pos), sz(sz), next(next){}
    };
    enum State{
        no_used, no_changed, changed
    };
    struct Bufferpool{
        State state = no_used;
        char buffer[node_size + sizeof(key_t) + sizeof(value_t)];
    };
    Bufferpool *bufferpool;

    char *buffer;

    const size_t tnode_max;
    const size_t block_max;
    const size_t tnode_min;
    const size_t block_min;
    Compare cmp = Compare();
    bool equal(const key_t &a, const key_t &b) {
        return !cmp(a, b) && !cmp(b, a);
    }


private:
    void write_out(off_t i){
        if(*fetcht(bufferpool[i].buffer) == tree_t){
            fseek(file, load_tnode(bufferpool[i].buffer)->pos, SEEK_SET);
            fwrite(bufferpool[i].buffer, node_size, 1, file);
        }else{
            fseek(file, load_block(bufferpool[i].buffer)->pos, SEEK_SET);
            fwrite(bufferpool[i].buffer, node_size, 1, file);
        }
        fflush(file);
    }
    void read_in(off_t i){
        buffer = bufferpool[i%poolsize].buffer;
        if(fetch_pos(buffer) != i && bufferpool[i%poolsize].state == changed){
            write_out(i%poolsize);
            fseek(file, i, SEEK_SET);
            fread(buffer, 1, node_size, file);
            bufferpool[i%poolsize].state = no_changed;
        }else if(fetch_pos(buffer) != i || bufferpool[i%poolsize].state == no_used){
            fseek(file, i, SEEK_SET);
            fread(buffer, 1, node_size, file);
            bufferpool[i%poolsize].state = no_changed;
        }
    }
    size_t fetch_sz(char *tbuffer){
        if(*fetcht(tbuffer) == tree_t)
            return load_tnode(tbuffer)->sz;
        else
            return load_block(tbuffer)->sz;
    }
    off_t fetch_pos(char *tbuffer){
        if(*fetcht(tbuffer) == tree_t)
            return load_tnode(tbuffer)->pos;
        else
            return load_block(tbuffer)->pos;
    }
    tnode* load_tnode(char *tbuffer){
        return (tnode*)(tbuffer + sizeof(char));
    }
    block* load_block(char *tbuffer){
        return (block*)(tbuffer + sizeof(char));
    }
    char* fetcht(char *tbuffer){
        return tbuffer;
    }
    //0 - sz-1
    key_t * fetchk_b(size_t n, char *tbuffer) {
        return (key_t *)(tbuffer + (sizeof(key_t) + sizeof(value_t))*n + sizeof(value_t) + sizeof(char) + sizeof(block));
    }
    value_t * fetchv_b(size_t n, char *tbuffer) {
        return (value_t *)(tbuffer + (sizeof(key_t) + sizeof(value_t))*n + sizeof(char) + sizeof(block));
    }
    //0 - sz-1
    key_t * fetchk_t(size_t n, char *tbuffer) {
        return (key_t *)(tbuffer + (sizeof(off_t) + sizeof(key_t))*n + sizeof(off_t) + sizeof(char) + sizeof(tnode));
    }
    //0 - sz
    off_t * fetchv_t(size_t n, char *tbuffer) {
        return (off_t *)(tbuffer + (sizeof(off_t) + sizeof(key_t))*n + sizeof(char) + sizeof(tnode));
    }

    off_t new_node(){
        off_t p = blank_begin;
        if(blank_begin == blank_end){
            blank_end += node_size;
            blank_begin += node_size;
            return p;
        }else{
            fseek(file, blank_begin, SEEK_SET);
            fread(&blank_begin, sizeof(off_t), 1, file);
            return p;
        }
    }
    void delete_node(off_t pos){
        fseek(file, pos, SEEK_SET);
        fwrite(&blank_begin, sizeof(off_t), 1, file);
        fflush(file);
        blank_begin = pos;
    }

    //…Í«Îø’Ω⁄µ„£¨Œ¥–¥»ÎÕ‚¥Ê
    inline block new_block(off_t sz = 0, off_t pos = invalid_off, off_t next = invalid_off){
        pos = new_node();
        return block(pos, sz, next);
    };
    inline tnode new_tnode(off_t sz = 0, off_t pos = invalid_off){
        pos = new_node();
        return tnode(pos, sz);
    }

    //Ã·π©∏¯tnodeµƒsearch
    size_t  t_binary_search(const key_t &key, const size_t &sz){
        size_t l = 0, r = sz, mid;
        while(l < r){
            mid = (l + r)/2;
            if(*fetchk_t(mid, buffer) <= key)
                l = mid + 1;
            else
                r = mid;
        }
        return l;
    }
    //Ã·π©∏¯blockµƒsearch 0-sz
    size_t b_binary_search(const key_t &key, const size_t &sz){
        size_t l = 0, r = sz, mid;
        while(l < r){
            mid = (l + r)/2;
            if(*fetchk_b(mid, buffer) < key)
                l = mid + 1;
            else
                r = mid;
        }
        return r;
    }

//µ√µΩblock«“Œƒº˛∂¡»Îblock,Õ¨ ±∏¸–¬falink,∑µªÿª∫¥Ê÷–block“˝”√,Ω·Œ≤Œﬁ±£ª§
    block &find_block(const key_t &key){
        char c_type;
        tnode c_tnode;
        block c_block;
        pfa_link = 0;
        fa_link[0] = root;

        read_in(root);

        while(*fetcht(buffer) == tree_t){
            fa_link_pos[pfa_link] = t_binary_search(key, load_tnode(buffer)->sz);
            pfa_link++;
            fa_link[pfa_link] = *fetchv_t(fa_link_pos[pfa_link-1], buffer);

            read_in(fa_link[pfa_link]);
        }
        return *load_block(buffer);
    }
    //∂¡»Îbuffer,“∂ƒ⁄≤Â»Î£¨Œ¥∏ƒ±‰Õ‚¥Ê
    bool insert_in_leaf(block &c, const key_t &key, const value_t &v){
        size_t x = b_binary_search(key, c.sz);
        if(*fetchk_b(x, buffer) == key)
            return false;
        for(size_t i = c.sz; i > x; i--){
            *fetchk_b(i, buffer) = *fetchk_b(i-1, buffer);
            *fetchv_b(i, buffer) = *fetchv_b(i-1, buffer);
        }
        c.sz++;
        *fetchk_b(x, buffer) = key;
        *fetchv_b(x, buffer) = v;
        bufferpool[c.pos%poolsize].state = changed;
        return true;
    }
    void insert_in_parent(tnode &P, const key_t &key, const off_t &N2){
        for(size_t i = P.sz; i > fa_link_pos[pfa_link]; i--){
            *fetchk_t(i, buffer) = *fetchk_t(i-1, buffer);
            *fetchv_t(i+1, buffer) = *fetchv_t(i, buffer);
        }
        P.sz++;
        *fetchk_t(fa_link_pos[pfa_link], buffer) = key;
        *fetchv_t(fa_link_pos[pfa_link]+1, buffer) = N2;
        bufferpool[P.pos%poolsize].state = changed;
    }


    void insert_parent(const off_t N, const key_t key, const off_t N2){
        if(N == root){
            tnode R = new_tnode(1);
            if(bufferpool[R.pos%poolsize].state == changed)
                write_out(R.pos%poolsize);
            buffer = bufferpool[R.pos%poolsize].buffer;
            memcpy(buffer, &tree_t, sizeof(char));
            memcpy(buffer + sizeof(char), &R, sizeof(tnode));
            memcpy(buffer + sizeof(char) + sizeof(tnode), &N, sizeof(off_t));
            memcpy(buffer + sizeof(char) + sizeof(tnode) + sizeof(off_t), &key, sizeof(key));
            memcpy(buffer + sizeof(char) + sizeof(tnode) + sizeof(off_t) + sizeof(key), &N2, sizeof(off_t));
            bufferpool[R.pos%poolsize].state = changed;

            root = R.pos;
        }else{
            pfa_link--;
            read_in(fa_link[pfa_link]);
            tnode &P = *load_tnode(buffer);
            if(P.sz < tnode_max){
                insert_in_parent(P, key, N2);
            }else{
                insert_in_parent(P, key, N2);

                tnode P2 = new_tnode(P.sz/2);
                P.sz -= (P2.sz+1);

                if((P.pos % poolsize) != (P2.pos % poolsize)){
                    if(bufferpool[P2.pos%poolsize].state == changed)
                        write_out(P2.pos%poolsize);
                    memcpy(bufferpool[P2.pos%poolsize].buffer, &tree_t, sizeof(char));
                    memcpy(bufferpool[P2.pos%poolsize].buffer + sizeof(char), &P2, sizeof(tnode));
                    memcpy(bufferpool[P2.pos%poolsize].buffer + sizeof(char) + sizeof(tnode),
                           buffer + sizeof(char) +sizeof(tnode) + (sizeof(key_t) + sizeof(off_t))*(P.sz+1),
                           (sizeof(key_t) + sizeof(off_t))*P2.sz +sizeof(off_t));
                    bufferpool[P2.pos%poolsize].state = changed;
                }else{
                    fseek(file, P2.pos, SEEK_SET);
                    fwrite(&tree_t, sizeof(char), 1, file);
                    fwrite(&P2, sizeof(tnode), 1, file);
                    fwrite(buffer + sizeof(char) + sizeof(tnode) + (sizeof(key_t) + sizeof(off_t))*(P.sz+1), (sizeof(key_t) + sizeof(off_t))*P2.sz +
                                                                                                             sizeof(off_t), 1, file);
                    fflush(file);
                }

                insert_parent(P.pos, *fetchk_t(P.sz, buffer), P2.pos);
            }
        }
    }

    void remove_parent_in(tnode &c, const size_t &mid){
        for(size_t i = mid; i < c.sz-1 ; i++){
            *fetchk_t(i, buffer) = *fetchk_t(i+1, buffer);
            *fetchv_t(i+1, buffer) = *fetchv_t(i+2, buffer);
        }
        c.sz--;
        bufferpool[c.pos%poolsize].state = changed;
    }

    bool remove_leaf_in(block &c, const key_t &key){
        size_t x = b_binary_search(key, c.sz);
        if(*fetchk_b(x, buffer) != key || x == c.sz)
            return false;
        for(size_t i = x; i < c.sz-1; i++){
            *fetchk_b(i, buffer) = *fetchk_b(i+1, buffer);
            *fetchv_b(i, buffer) = *fetchv_b(i+1, buffer);
        }
        c.sz--;
        bufferpool[c.pos%poolsize].state = changed;
        return true;
    }

    inline void remove_begin_b(block &c, char *tbuffer){
        for(size_t i = 0; i < c.sz-1; i++){
            *fetchk_b(i, tbuffer) = *fetchk_b(i+1, tbuffer);
            *fetchv_b(i, tbuffer) = *fetchv_b(i+1, tbuffer);
        }
        c.sz--;
    }
    inline void remove_begin_t(tnode &c, char *tbuffer){
        for(size_t i = 0; i < c.sz-1; i++){
            *fetchk_t(i, tbuffer) = *fetchk_t(i+1, tbuffer);
            *fetchv_t(i, tbuffer) = *fetchv_t(i+1, tbuffer);
        }
        *fetchv_t(c.sz-1, tbuffer) = *fetchv_t(c.sz, tbuffer);
        c.sz--;
    }
    void remove_help_t1(char *tmp, key_t midkey, size_t mid, bool flag){
        memcpy(buffer + sizeof(char) + sizeof(tnode) + (sizeof(off_t) + sizeof(key_t))*(load_tnode(buffer)->sz) + sizeof(off_t),
               &midkey, sizeof(key_t));
        memcpy(buffer + sizeof(char) + sizeof(tnode) + (sizeof(off_t) + sizeof(key_t))*(load_tnode(buffer)->sz + 1),
               tmp + sizeof(char) + sizeof(tnode),
               (sizeof(off_t) + sizeof(key_t))*load_tnode(tmp)->sz + sizeof(off_t));
        load_tnode(buffer)->sz += (load_tnode(tmp)->sz + 1);
        bufferpool[(load_tnode(buffer)->pos)%poolsize].state = changed;
        if(!flag){
            bufferpool[(load_tnode(tmp)->pos)%poolsize].state = no_used;
        }
        delete_node(load_tnode(tmp)->pos);
        remove_parent(mid);
    }
    void remove_help_t2(char *tmp, key_t midkey, size_t mid, bool flag){
        memcpy(buffer + sizeof(char) + sizeof(tnode) + (sizeof(off_t) + sizeof(key_t))*(load_tnode(buffer)->sz) + sizeof(off_t),
               &midkey, sizeof(key_t));
        memcpy(buffer + sizeof(char) + sizeof(tnode) + (sizeof(off_t) + sizeof(key_t))*(load_tnode(buffer)->sz + 1),
               tmp + sizeof(char) + sizeof(tnode), sizeof(off_t));
        key_t  new_key = *fetchk_t(0, tmp);
        load_tnode(buffer)->sz++;
        remove_begin_t(*load_tnode(tmp), tmp);
        bufferpool[load_tnode(buffer)->pos % poolsize].state = changed;
        if(flag){
            fseek(file, load_tnode(tmp)->pos, SEEK_SET);
            fwrite(tmp, node_size, 1, file);
            fflush(file);
        }else{
            bufferpool[load_tnode(tmp)->pos % poolsize].state = changed;
        }
        read_in(fa_link[pfa_link]);
        *fetchk_t(mid, buffer) = new_key;
        bufferpool[fa_link[pfa_link] % poolsize].state = changed;
    }
    void remove_help_t3(char *tmp, key_t midkey, size_t mid, bool flag){
        tnode &P = *load_tnode(tmp);
        tnode &P2 = *load_tnode(buffer);
        for(size_t i = P.sz; i > 0; i--){
            *fetchk_t(i, tmp) = *fetchk_t(i-1, tmp);
            *fetchv_t(i+1, tmp) = *fetchv_t(i, tmp);
        }
        *fetchv_t(1, tmp) = *fetchv_t(0, tmp);

        *fetchk_t(0, tmp) = midkey;
        *fetchv_t(0, tmp) = *fetchv_t(P2.sz, buffer);
        P.sz++;
        P2.sz--;
        key_t new_key = *fetchk_t(P2.sz, buffer);

        bufferpool[load_tnode(buffer)->pos % poolsize].state = changed;
        if(flag){
            fseek(file, load_tnode(tmp)->pos, SEEK_SET);
            fwrite(tmp, node_size, 1, file);
            fflush(file);
        }else{
            bufferpool[load_tnode(tmp)->pos % poolsize].state = changed;
        }
        read_in(fa_link[pfa_link]);
        *fetchk_t(mid, buffer) = new_key;
        bufferpool[fa_link[pfa_link] % poolsize].state = changed;
    }
    void remove_parent(size_t mid){
        read_in(fa_link[pfa_link]);
        tnode &c = *load_tnode(buffer);
        remove_parent_in(c, mid);
        size_t nowsize = c.sz;
        size_t nowpos = c.pos;
        if(c.pos == root){
            if(c.sz == 1){
                delete_node(c.pos);
                root = *fetchv_t(0, buffer);
            }
            return;
        }
        if(nowsize < tnode_min){
            pfa_link--;
            read_in(fa_link[pfa_link]);
            if(fa_link_pos[pfa_link] < fetch_sz(buffer) ){
                size_t rmid = fa_link_pos[pfa_link];
                key_t rmidkey = *fetchk_t(rmid, buffer);
                off_t rbrother = *fetchv_t(fa_link_pos[pfa_link]+1, buffer);
                read_in(rbrother);
                if(load_tnode(buffer)->sz + nowsize + 1 <= tnode_max){
                    read_in(nowpos);
                    if(fetch_pos(bufferpool[rbrother%poolsize].buffer) != rbrother){
                        char tmp[node_size + sizeof(key_t) + sizeof(value_t)];
                        fseek(file, rbrother, SEEK_SET);
                        fread(tmp, 1, node_size, file);
                        fflush(file);
                        remove_help_t1(tmp, rmidkey, rmid, true);
                    }else{
                        remove_help_t1(bufferpool[rbrother%poolsize].buffer, rmidkey, rmid, false);
                    }
                }else{
                    read_in(nowpos);
                    if(fetch_pos(bufferpool[rbrother%poolsize].buffer) != rbrother){
                        char tmp[node_size + sizeof(key_t) + sizeof(value_t)];
                        fseek(file, rbrother, SEEK_SET);
                        fread(tmp, 1, node_size, file);
                        fflush(file);
                        remove_help_t2(tmp, rmidkey, rmid, true);
                    }else{
                        remove_help_t2(bufferpool[rbrother%poolsize].buffer, rmidkey, rmid, false);
                    }
                }
            }else{
                size_t lmid = fa_link_pos[pfa_link] - 1;
                key_t lmidkey = *fetchk_t(lmid, buffer);
                off_t lbrother = *fetchv_t(fa_link_pos[pfa_link]-1, buffer);
                read_in(lbrother);
                if(load_tnode(buffer)->sz + nowsize + 1 <= tnode_max){
                    if(fetch_pos(bufferpool[nowpos%poolsize].buffer) != nowpos){
                        char tmp[node_size + sizeof(key_t) + sizeof(value_t)];
                        fseek(file, nowpos, SEEK_SET);
                        fread(tmp, 1, node_size, file);
                        fflush(file);
                        remove_help_t1(tmp, lmidkey, lmid, true);
                    }else{
                        remove_help_t1(bufferpool[nowpos%poolsize].buffer, lmidkey, lmid, false);
                    }
                }else{
                    if(fetch_pos(bufferpool[nowpos%poolsize].buffer) != nowpos){
                        char tmp[node_size + sizeof(key_t) + sizeof(value_t)];
                        fseek(file, nowpos, SEEK_SET);
                        fread(tmp, 1, node_size, file);
                        fflush(file);
                        remove_help_t3(tmp, lmidkey, lmid, true);
                    }else{
                        remove_help_t3(bufferpool[nowpos%poolsize].buffer, lmidkey, lmid, false);
                    }
                }

            }
        }


    }
    //tmp≤ª–Ë“™
    void remove_help_b1(char *tmp, key_t midkey, size_t mid, bool flag){
        memcpy(buffer + sizeof(char) + sizeof(block) + (sizeof(value_t) + sizeof(key_t))*load_block(buffer)->sz,
               tmp + sizeof(char) + sizeof(block),(sizeof(value_t) + sizeof(key_t)) * load_block(tmp)->sz);
        if(load_block(tmp)->pos == tail)
            tail = load_block(buffer)->pos;
        load_block(buffer)->next = load_block(tmp)->next;
        load_block(buffer)->sz += load_block(tmp)->sz;
        bufferpool[(load_block(buffer)->pos) % poolsize].state = changed;
        if(!flag){
            bufferpool[(load_block(tmp)->pos) % poolsize].state = no_used;
        }
        delete_node(load_block(tmp)->pos);
        remove_parent(mid);
    }
    void remove_help_b2(char *tmp, key_t midkey, size_t mid, bool flag){
        memcpy(buffer + sizeof(char) + sizeof(block) + (sizeof(value_t) + sizeof(key_t))*load_block(buffer)->sz,
               tmp + sizeof(char) + sizeof(block),sizeof(value_t) + sizeof(key_t));
        load_block(buffer)->sz++;
        key_t new_key = *fetchk_b(0, tmp);
        remove_begin_b(*load_block(tmp), tmp);

        if(flag){
            fseek(file, load_block(tmp)->pos, SEEK_SET);
            fwrite(tmp, node_size, 1, file);
            fflush(file);
        }else{
            bufferpool[(load_block(tmp)->pos) % poolsize].state = changed;
        }
        bufferpool[(load_block(buffer)->pos) % poolsize].state = changed;

        read_in(fa_link[pfa_link]);
        *fetchk_t(mid, buffer) = new_key;
        bufferpool[fa_link[pfa_link]%poolsize].state = changed;
    }
    void remove_help_b3(char *tmp, key_t midkey, size_t mid, bool flag){
        block &c = *load_block(tmp);
        block &c2 = *load_block(buffer);
        for(size_t i = c.sz; i > 0; i--){
            *fetchk_b(i, tmp) = *fetchk_b(i-1, tmp);
            *fetchv_b(i, tmp) = *fetchv_b(i-1, tmp);
        }
        *fetchk_b(0, tmp) = *fetchk_b(c2.sz-1, buffer);
        *fetchv_b(0, tmp) = *fetchv_b(c2.sz-1, buffer);
        c.sz++;
        c2.sz--;
        key_t new_key = *fetchk_b(0, tmp);

        if(flag){
            fseek(file, load_block(tmp)->pos, SEEK_SET);
            fwrite(tmp, node_size, 1, file);
            fflush(file);
        }else{
            bufferpool[(load_block(tmp)->pos) % poolsize].state = changed;
        }
        bufferpool[(load_block(buffer)->pos) % poolsize].state = changed;

        read_in(fa_link[pfa_link]);
        *fetchk_t(mid, buffer) = new_key;
        bufferpool[fa_link[pfa_link]%poolsize].state = changed;
    }
    //0 - quicksize-1
    key_t* quick_key(size_t n){
        return (key_t*)(quickbuf + n * (sizeof(value_t) + sizeof(key_t)));
    }
    value_t* quick_value(size_t n){
        return (value_t*)(quickbuf + n * (sizeof(value_t) + sizeof(key_t)) + sizeof(key_t));
    }
    void QuickSort(int iBegin, int iEnd)
    {
        if (iBegin < iEnd)
        {
            int iLeft = iBegin;
            int iRight = iEnd;
            key_t iPivot_key = *quick_key(iBegin);
            value_t iPivot_v = *quick_value(iBegin);

            while (iLeft < iRight)
            {
                while (iLeft < iRight && !(*quick_key(iRight)<iPivot_key))
                {
                    iRight--;
                }
                if(iLeft < iRight){
                    *quick_key(iLeft) = *quick_key(iRight);
                    *quick_value(iLeft) = *quick_value(iRight);
                    iLeft++;
                }
                while (iLeft < iRight && *quick_key(iLeft) <= iPivot_key)
                {
                    iLeft++;
                }
                if(iLeft < iRight){
                    *quick_key(iRight) = *quick_key(iLeft);
                    *quick_value(iRight) = *quick_value(iLeft);
                    iRight--;
                }
            }
            *quick_key(iLeft) = iPivot_key;
            *quick_value(iLeft) = iPivot_v;
            QuickSort(iBegin, iLeft - 1);
            QuickSort(iRight + 1, iEnd);
        }
    }
    void quickout(){
        //≈≈–Ú
        QuickSort(0, quicksize-1);
        for(size_t i = 0; i < quicksize; i++){
            _insert(*quick_key(i), *quick_value(i));
        }
        quicksize = 0;
        isquick = false;
    }
    void save_index(){
        fseek(file, 0, SEEK_SET);
        fwrite(&alsize, sizeof(size_t), 1, file);
        fwrite(&blank_end, sizeof(off_t), 1, file);
        fwrite(&blank_begin, sizeof(off_t), 1, file);
        fwrite(&head, sizeof(off_t), 1, file);
        fwrite(&tail, sizeof(off_t), 1, file);
        fwrite(&root, sizeof(off_t), 1, file);
        fflush(file);
    }
    void load_index(){
        fseek(file, 0, SEEK_SET);
        fread(&alsize, 1, sizeof(size_t), file);
        fread(&blank_end, 1, sizeof(off_t), file);
        fread(&blank_begin, 1, sizeof(off_t), file);
        fread(&head, 1, sizeof(off_t), file);
        fread(&tail, 1, sizeof(off_t), file);
        fread(&root, 1, sizeof(off_t), file);
    }
public:
    bptree(const char * fname)
            :tnode_max((node_size - sizeof(tnode) - sizeof(char) - sizeof(off_t))/ (sizeof(off_t) + sizeof(key_t))),
             block_max((node_size - sizeof(block) - sizeof(char))/ (sizeof(value_t) + sizeof(key_t))),
             tnode_min(ceil((tnode_max+1)/2)-1), block_min(ceil(block_max/2)),
             quick_max(supersize/(sizeof(value_t) + sizeof(key_t))), quicksize(0), isquick(false){
        file = fopen(fname, "rb+");
        filename = new char[strlen(fname) + 1];
        strcpy(filename, fname);
        quickbuf = new char[supersize];
        bufferpool = new Bufferpool[poolsize];
        if(!file){
            file = fopen(fname, "wb+");
            alsize = 0;
            blank_begin = sizeof(size_t) + 5 * sizeof(off_t);
            blank_end = sizeof(size_t) + 5 * sizeof(off_t);
            head = tail = root = invalid_off;
        }else{
            load_index();
        }
        fseek(file, 0, SEEK_SET);
    }

    ~bptree(){
        if(isquick){
            quickout();
        }
        for(size_t i = 0; i < poolsize; i++){
            if(bufferpool[i].state != no_used){
                write_out(i);
            }
        }
        save_index();
        if (file) fclose(file);
        delete filename;
        delete []quickbuf;
        delete []bufferpool;
    }
    //∫Ø ˝ø™ º±£ª§£¨Ω·Œ≤Œﬁ±£ª§
    bool _insert(const key_t &key, const value_t &v){
        if(alsize == 0){
            block p = new_block(1);

            if(bufferpool[p.pos%poolsize].state == changed)
                write_out(p.pos%poolsize);
            buffer = bufferpool[p.pos%poolsize].buffer;
            memcpy(buffer, &block_t, sizeof(char));
            memcpy(buffer + sizeof(char), &p, sizeof(block));
            memcpy(buffer + sizeof(char) + sizeof(block), &v, sizeof(value_t));
            memcpy(buffer + sizeof(char) + sizeof(block) + sizeof(value_t), &key, sizeof(key_t));
            bufferpool[p.pos%poolsize].state = changed;

            alsize++;
            head = tail = root = p.pos;
            return true;
        }else{//±£÷§π˝≥Ã‘À––∆⁄º‰≤ªƒ‹∏ƒ∂Øbuffer
            block &L = find_block(key);
            if(L.sz < block_max){
                if(!insert_in_leaf(L, key, v))
                    return false;
                alsize++;
            }else{
                if(!insert_in_leaf(L, key, v))
                    return false;
                block L2 = new_block(L.sz/2);
                L.sz -= L2.sz;
                L2.next = L.next;
                L.next = L2.pos;
                if(L.pos == tail)
                    tail = L2.pos;
                alsize++;

                if((L2.pos % poolsize) != (L.pos % poolsize)){
                    if(bufferpool[L2.pos%poolsize].state == changed)
                        write_out(L2.pos%poolsize);
                    memcpy(bufferpool[L2.pos%poolsize].buffer, &block_t, sizeof(char));
                    memcpy(bufferpool[L2.pos%poolsize].buffer + sizeof(char), &L2, sizeof(block));
                    memcpy(bufferpool[L2.pos%poolsize].buffer + sizeof(char) + sizeof(block),
                           buffer + (sizeof(value_t) +sizeof(key_t)) * L.sz + sizeof(char) + sizeof(block),
                           (sizeof(value_t) + sizeof(key_t)) * L2.sz);
                    bufferpool[L2.pos%poolsize].state = changed;
                }else{
                    fseek(file, L2.pos, SEEK_SET);
                    fwrite(&block_t, sizeof(char), 1, file);
                    fwrite(&L2, sizeof(block), 1, file);
                    fwrite(buffer + (sizeof(value_t) + sizeof(key_t)) * L.sz + sizeof(char) + sizeof(block),(sizeof(value_t) + sizeof(key_t)) * L2.sz, 1, file);
                    fflush(file);
                }

                insert_parent(L.pos, *fetchk_b(L.sz, buffer), L2.pos);
                return true;
            }
        }
    }
    bool insert(const key_t &key, const value_t &v){
        if(quicksize == quick_max)
            quickout();
        memcpy(quickbuf + quicksize * (sizeof(value_t) + sizeof(key_t)), &key, sizeof(key_t));
        memcpy(quickbuf + quicksize * (sizeof(value_t) + sizeof(key_t)) + sizeof(key_t), &v, sizeof(value_t));
        quicksize++;
        isquick = true;
    }

    value_t find(const key_t &key, const value_t & no = value_t()){
        if(isquick){
            quickout();
        }
        if(root == invalid_off)
            return no;
        block &c = find_block(key);
        size_t x = b_binary_search(key, c.sz);
        if(*fetchk_b(x, buffer) != key || x == c.sz){
            return no;
        }else{
            return *fetchv_b(x, buffer);
        }
    }
    //∫Ø ˝ø™ º”–±£ª§£¨Ω· ¯Œﬁ±£ª§£¨fwriteΩ· ¯
    void set(const key_t &key, const value_t &v){
        if(isquick){
            quickout();
        }
        if(root == invalid_off)
            throw not_found();
        block &c = find_block(key);
        size_t x = b_binary_search(key, c.sz);
        if(*fetchk_b(x, buffer) != key || x == c.sz){
            throw not_found();
        }else{
            *fetchv_b(x, buffer) = v;
            bufferpool[c.pos%poolsize].state = changed;
        }
    }
    void init(){
        if(isquick){
            quickout();
        }
        freopen(filename, "wb+", file);
        alsize = 0;
        alsize = 0;
        blank_begin = sizeof(size_t) + 5 * sizeof(off_t);
        blank_end = sizeof(size_t) + 5 * sizeof(off_t);
        head = tail = root = invalid_off;
        fseek(file, 0, SEEK_SET);
        for(size_t i = 0; i < poolsize; i++){
            bufferpool[i].state = no_used;
        }
    }

    void remove(const key_t &key){
        if(isquick){
            quickout();
        }
        if(root == invalid_off)
            throw not_found();
        block &c = find_block(key);
        if(!remove_leaf_in(c, key))
            throw not_found();
        alsize--;

        size_t nowsize = c.sz;
        off_t  nowpos = c.pos;
        if(c.pos == root){
            if(c.sz == 0)
                init();
            return;
        }
        if(nowsize < block_min){
            pfa_link--;
            read_in(fa_link[pfa_link]);
            if(fa_link_pos[pfa_link] < fetch_sz(buffer)){
                size_t rmid = fa_link_pos[pfa_link];
                key_t rmidkey = *fetchk_t(rmid, buffer);
                off_t rbrother = *fetchv_t(fa_link_pos[pfa_link]+1, buffer);
                read_in(rbrother);
                if(load_block(buffer)->sz + nowsize <= block_max){
                    read_in(nowpos);
                    if(fetch_pos(bufferpool[rbrother%poolsize].buffer) != rbrother){
                        char tmp[node_size + sizeof(key_t) + sizeof(value_t)];
                        fseek(file, rbrother, SEEK_SET);
                        fread(tmp, 1, node_size, file);
                        fflush(file);
                        remove_help_b1(tmp, rmidkey, rmid, true);
                    }else{
                        remove_help_b1(bufferpool[rbrother%poolsize].buffer, rmidkey, rmid, false);
                    }
                }else{
                    read_in(nowpos);
                    if(fetch_pos(bufferpool[rbrother%poolsize].buffer) != rbrother){
                        char tmp[node_size + sizeof(key_t) + sizeof(value_t)];
                        fseek(file, rbrother, SEEK_SET);
                        fread(tmp, 1, node_size, file);
                        fflush(file);
                        remove_help_b2(tmp, rmidkey, rmid, true);
                    }else{
                        remove_help_b2(bufferpool[rbrother%poolsize].buffer, rmidkey, rmid, false);
                    }
                }
            }else{
                size_t lmid = fa_link_pos[pfa_link] - 1;
                key_t lmidkey = *fetchk_t(lmid, buffer);
                off_t lbrother = *fetchv_t(fa_link_pos[pfa_link]-1, buffer);
                read_in(lbrother);
                if(load_block(buffer)->sz + nowsize <= block_max){
                    if(fetch_pos(bufferpool[nowpos%poolsize].buffer) != nowpos){
                        char tmp[node_size + sizeof(key_t) + sizeof(value_t)];
                        fseek(file, nowpos, SEEK_SET);
                        fread(tmp, 1, node_size, file);
                        fflush(file);
                        remove_help_b1(tmp, lmidkey, lmid, true);
                    }else{
                        remove_help_b1(bufferpool[nowpos%poolsize].buffer, lmidkey, lmid, false);
                    }
                }else{
                    if(fetch_pos(bufferpool[nowpos%poolsize].buffer) != nowpos){
                        char tmp[node_size + sizeof(key_t) + sizeof(value_t)];
                        fseek(file, nowpos, SEEK_SET);
                        fread(tmp, 1, node_size, file);
                        fflush(file);
                        remove_help_b3(tmp, lmidkey, lmid, true);
                    }else{
                        remove_help_b3(bufferpool[nowpos%poolsize].buffer, lmidkey, lmid, false);
                    }
                }
            }
        }
    }
    void search(sjtu::vector<sjtu::pair<key_t, value_t>> &arr, const key_t & key,
                std::function<bool(const key_t &, const key_t &)> compar){
        if(isquick){
            quickout();
        }
        block &c = find_block(key);
        size_t x = b_binary_search(key, c.sz);
        if(x == c.sz){
            read_in(load_block(buffer)->next);
            x = 0;
        }
        while(compar(*fetchk_b(x, buffer), key)){
            arr.push_back(sjtu::pair<key_t, value_t>(*fetchk_b(x, buffer), *fetchv_b(x, buffer)));
            x++;
            if(x == load_block(buffer)->sz) {
                read_in(load_block(buffer)->next);
                x = 0;
            }
        }
    }
    void traverse(std::function<void (const key_t &, const value_t &)> compar){
        if(isquick){
            quickout();
        }
        off_t p = head;
        fflush(file);
        while(p != tail){
            read_in(p);
            block& m = *load_block(buffer);
            std::cout << "The Node" << " sz " << m.sz << " pos " << m.pos  << " next " << m.next;
            for(size_t i = 0; i < m.sz; i++){
                compar(*fetchk_b(i, buffer), *fetchv_b(i, buffer));
            }
            p = m.next;
        }
        read_in(p);
        block& m = *load_block(buffer);
        std::cout << "The Node" << " sz " << m.sz << " pos " << m.pos  << " next " << m.next;
        for(size_t i = 0; i < m.sz; i++){
            compar(*fetchk_b(i, buffer), *fetchv_b(i, buffer));
        }
    }
};
#endif //BTREE_BPTREE_HPP

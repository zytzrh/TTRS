//
// Created by yezhuoyang on 2019-05-27.
//

#ifndef DEBUG_ALLOC_HPP
#define DEBUG_ALLOC_HPP
#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../STLITE/exceptions.h"
//A very simple try
//typedef long off_t;
constexpr off_t invalid_off = 0xdeadbeef;
//typedef unsigned long size_t;
class file_alloc {
private:
    size_t node_size = 4096;
    struct node {
        off_t start, end;
        node *prev, *next;
        node(off_t start = 0, off_t end = 0, node * prev = nullptr, node *next = nullptr) :
                start(start), end(end), prev(prev), next(next) {}
        inline size_t size() {
            return end - start;
        }
        inline void print() {
            printf("[block start:%lld end:%lld]",start,end);
        }
    };
    node *head, *tail;
    //char * filename;
    //FILE * file;
    off_t file_end;

    inline void del_node(node *p);
    inline node * insert_before(node *p, off_t start, off_t end);
    inline node * insert_tail(off_t start, off_t end);
    inline void merge(node *p);
public:
    file_alloc(const char * filename="");
    ~file_alloc();

    void load(const char * filename = "");
    void dump(const char * filename = "");

    inline bool empty();
    void clear();
    off_t alloc(size_t len);
    void free(off_t pos, size_t len);
    void print() {
        node *p = head;
        for (; p; p = p->next) {
            p->print();
            printf("\n");
        }
    }
};

void file_alloc::del_node(node * p)
{
    if (p->prev) p->prev->next = p->next;
    else head = p->next;
    if (p->next) p->next->prev = p->prev;
    else tail = p->prev;
    delete p;
}

inline file_alloc::node * file_alloc::insert_before(node * p, off_t start, off_t end)
{
    node *q = new node(start, end, p->prev, p);
    if (p->prev) p->prev->next = q;
    else head = q;
    p->prev = q;
    return q;
}

inline file_alloc::node * file_alloc::insert_tail(off_t start, off_t end)
{
    if (empty()) {
        head = tail = new node(start, end);
        return head;
    }
    node *q = new node(start, end, tail, nullptr);
    tail->next = q;
    tail = q;
    return q;
}

inline void file_alloc::merge(node * p)
{
    node *q;
    if (p->next && p->next->start == p->end) {
        q = p->next;
        p->end = q->end;
        del_node(q);
    }
    if (p->prev && p->prev->end == p->start) {
        q = p->prev;
        q->end = p->end;
        del_node(p);
    }
}

file_alloc::file_alloc(const char * fname)
{
    //filename = (char *)malloc(sizeof(fname));
    //strcpy(filename, fname);
    head = tail = nullptr;
    //file = fopen(fname,"rb+");
    file_end = 0;
}

file_alloc::~file_alloc()
{
    clear();
}

void file_alloc::load(const char * filename)
{
    clear();
    FILE * fp = fopen(filename, "rb");
    if (!fp) return;
    //throw non_existent_file();
    off_t s[2];
    int read_cnt;
    fread(&file_end, sizeof(off_t), 1, fp);
    while (!feof(fp)) {
        read_cnt = fread(s, sizeof(off_t), 2, fp);
        if (read_cnt == 0) break;
        if (read_cnt == 1) throw invalid_format();
        insert_tail(s[0], s[1]);
    }
    fclose(fp);
}

void file_alloc::dump(const char * filename)
{
    FILE *fp = fopen(filename, "wb");
    node *p = head;
    off_t s[2];
    fwrite(&file_end, sizeof(off_t), 1, fp);
    for (; p; p = p->next) {
//printf("write\n");
        s[0] = p->start; s[1] = p->end;
        fwrite(s, sizeof(off_t), 2, fp);
    }
    if (fp) fclose(fp);

}

inline bool file_alloc::empty()
{
    return head == nullptr;
}

void file_alloc::clear()
{
    file_end = 0;
    node *p = head, *q;
    while (p) {
        q = p->next;
        delete p;
        p = q;
    }
    head = tail = nullptr;
}

off_t file_alloc::alloc(size_t len)
{
    node *p = head;
    off_t pos;
    for (; p != nullptr && p->size() < len; p = p->next);
    if (p) {
        pos = p->start;
        p->start += len;
        if (p->start == p->end) del_node(p);
        return pos;
    }
    else {
        pos = file_end;
        file_end += len;
        return pos;
    }
}

void file_alloc::free(off_t pos, size_t len)
{
    node * p = head, *q;
    for (; p && p->start < pos; p = p->next);

    if (p) {
        if (p->start < pos + len) {
            //printf("invalid address: %d %d %d\n", p->start, pos, len);
            throw unallocated_space();
        }
        q = insert_before(p, pos, pos + len);
    }
    else {
        q = insert_tail(pos, pos + len);
    }
    //printf("free succeed\n");
    merge(q);
    return;
}

#endif //DEBUG_ALLOC_HPP

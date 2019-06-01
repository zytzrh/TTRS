//
// Created by yezhuoyang on 2019-05-26.
//

#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP
#include "exceptions.h"
#include <cstddef>
#include <iostream>
namespace sjtu {
    inline int Rand1(){
        static int x = 0;
        static int y = 0;
        static int z = 0;
        static int w = 0;
        x = (x*3 +10457)%20011;
        y = (y*7 +5011)%20089;
        z = (z*5 +313)%20113;
        w = (w*11 +1031)%30097;
        return (x^y^z^w)&((1<<15)-1);
    }
    inline int Rand(){return (Rand1()<<14)^Rand1();}
    template <class T>
    inline T max(const T &a, const T &b){return (a < b)? b:a;}
    template <class T1, class T2>
    struct Pair{
        T1 first;T2 second;
        Pair(const T1 &v1, const T2 &v2):first(v1), second(v2){}
        Pair(){}
    };
    template <class KEY>
    class Node{
    public:
        KEY data;Node *l, *r;int size, height, randnum;
        inline int friend getsize(Node *p){
            if (p == NULL) return 0;
            else return p -> size;
        }
        inline int friend geth(Node *p){
            if (p == NULL) return 0;
            else return p -> height;
        }
        inline void friend keep(Node *p1){
            p1 -> size = getsize(p1 -> l) + getsize(p1 -> r) +1;
            p1 -> height = max( geth(p1 -> l), geth(p1 -> r) ) +1;
        }
    public:
        Node():l(NULL), r(NULL), randnum(Rand()), size(1), height(1){}
        Node(const KEY &key):l(NULL), r(NULL), randnum(Rand()),
                             size(1), height(1), data(key){}
        ~Node (){}
        friend Node* merge(Node *p1, Node *p2){
            if (p1 == NULL) return p2;
            if (p2 == NULL) return p1;
            if (p1 -> randnum >= p2 -> randnum){
                p1 -> r = merge(p1 -> r, p2);
                keep(p1);
                return p1;
            } else{
                p2 -> l = merge(p1, p2 -> l);
                keep(p2);
                return p2;
            }
        }
        friend Pair<Node* , Node*> split(Node *p, int cnt){
            Pair<Node* , Node*> ans;
            if (cnt == getsize( p ) ) return Pair<Node* , Node*>(p, NULL);
            if (cnt == 0) return Pair<Node* , Node*>(NULL, p);
            if (cnt == getsize(p -> l)){
                ans.first = p -> l ;
                ans.second = p;
                p -> l = NULL;
                keep(p);
                return ans;
            }
            if (cnt == getsize(p -> l) +1){
                ans.first = p;
                ans.second = p -> r;
                p -> r = NULL;
                keep(p);
                return ans;
            }
            if (cnt <= getsize(p -> l)){
                ans = split(p -> l, cnt);
                p -> l = ans.second;
                keep(p);
                ans.second = p;
                return ans;
            } else{
                ans = split(p -> r, cnt - getsize(p -> l) -1);
                p -> r = ans.first;
                keep(p);
                ans.first = p;
                return ans;
            }
        }
        friend void clear1(Node* p){
            if (p == NULL) return;
            clear1( p-> l);
            clear1( p-> r);
            delete p;
        }
        friend Node* copy1(Node *p){
            if (p==NULL) return NULL;
            Node *ans;
            ans = new Node (p -> data);
            ans -> size = p -> size;
            ans -> height = p -> height;
            ans -> l = copy1 (p -> l);
            ans -> r = copy1 (p -> r);
            return ans;
        }
        friend Node* find1(Node *p, int k){
            if (k==getsize(p -> l)+1) return p;
            if (k<=getsize(p -> l)) return find1(p -> l, k);
            else return find1(p -> r, k - getsize(p -> l) -1);
        }
    };
    template<class T>
    class deque {
        Node<T> *root;
        int sz;
    public:
        class const_iterator;
        class iterator {
            friend deque;
        private:
            int pos;deque * place;
        public:
            iterator ():place(NULL), pos(0){}
            iterator (deque* p, int n):place(p), pos(n){}
            iterator (const iterator &other):place(other.place), pos(other.pos){}
            iterator operator = (const iterator &other){
                pos = other.pos;
                place = other.place;
                return *this;
            }
            iterator operator+(const int &n) const {
                if (pos + n < 1 || pos + n > place -> sz +1 ) throw invalid_iterator();
                return iterator(place, pos + n);
            }
            iterator operator-(const int &n) const {
                if (pos - n < 1 || pos - n > place -> sz +1 ) throw invalid_iterator();
                return iterator(place, pos - n);
            }
            int operator-(const iterator &rhs) const {
                if (place != rhs .place) throw invalid_iterator();
                if (pos < 1 || pos > place -> sz +1 ) throw invalid_iterator();
                if (rhs. pos < 1 || rhs. pos > place -> sz +1 ) throw invalid_iterator();
                return pos - rhs. pos;
            }
            iterator operator+=(const int &n) {
                if (pos + n < 1 || pos + n > place -> sz +1 ) throw invalid_iterator();
                pos += n;
                return *this;
            }
            iterator operator-=(const int &n) {
                if (pos - n < 1 || pos - n > place -> sz +1 ) throw invalid_iterator();
                pos -= n;
                return *this;
            }
            iterator operator++(int) {
                if (pos > place -> sz) throw invalid_iterator();
                iterator ans = *this;
                ++pos;
                return ans;
            }
            iterator& operator++() {
                if (pos > place -> sz) throw invalid_iterator();
                ++pos;
                return *this;
            }
            iterator operator--(int) {
                if (pos <= 1) throw invalid_iterator();
                iterator ans = *this;
                --pos;
                return ans;
            }
            iterator& operator--() {
                if (pos <= 1) throw invalid_iterator();
                --pos;
                return *this;
            }
            T& operator*() const {
                if (pos < 1 || pos > place -> sz) throw invalid_iterator();
                return find1(place -> root, pos) -> data;
            }
            T* operator->() const noexcept {
                if (pos < 1 || pos > place -> sz) throw invalid_iterator();
                return &(find1(place -> root, pos) -> data);
            }
            bool operator==(const iterator &rhs) const {
                return pos == rhs.pos && place == rhs.place;
            }
            bool operator==(const const_iterator &rhs) const {
                return pos == rhs.pos && place == rhs.place;
            }
            bool operator!=(const iterator &rhs) const {
                return pos != rhs.pos || place != rhs.place;
            }
            bool operator!=(const const_iterator &rhs) const {
                return pos != rhs.pos || place != rhs.place;
            }
        };
        class const_iterator {
        private:
            int pos;
            const deque * place;
        public:
            const_iterator ():place(NULL), pos(0){}
            const_iterator (const deque* p, int n):place(p), pos(n){}
            const_iterator (const const_iterator &other):place(other.place), pos(other.pos){}
            const_iterator (const iterator &other):place(other.place), pos(other.pos){}
            const_iterator operator = (const iterator &other){
                pos = other.pos;
                place = other.place;
                return *this;
            }
            const_iterator operator+(const int &n) const {
                if (pos + n < 1 || pos + n > place -> sz +1 ) throw invalid_iterator();
                return const_iterator(place, pos + n);
            }
            const_iterator operator-(const int &n) const {
                if (pos - n < 1 || pos - n > place -> sz +1 ) throw invalid_iterator();
                return const_iterator(place, pos - n);
            }
            int operator-(const const_iterator &rhs) const {
                if (place != rhs .place) throw invalid_iterator();
                if (pos < 1 || pos > place -> sz +1 ) throw invalid_iterator();
                if (rhs. pos < 1 || rhs. pos > place -> sz +1 ) throw invalid_iterator();
                return pos - rhs. pos;
            }
            int operator-(const iterator &rhs) const {
                if (place != rhs .place) throw invalid_iterator();
                if (pos < 1 || pos > place -> sz +1 ) throw invalid_iterator();
                if (rhs. pos < 1 || rhs. pos > place -> sz +1 ) throw invalid_iterator();
                return pos - rhs. pos;
            }
            const_iterator operator+=(const int &n) {
                if (pos + n < 1 || pos + n > place -> sz +1 ) throw invalid_iterator();
                pos += n;
                return *this;
            }
            const_iterator operator-=(const int &n) {
                if (pos - n < 1 || pos - n > place -> sz +1 ) throw invalid_iterator();
                pos -= n;
                return *this;
            }
            const_iterator operator++(int) {
                if (pos > place -> sz) throw invalid_iterator();
                const_iterator ans = *this;
                ++pos;
                return ans;
            }
            const_iterator& operator++() {
                if (pos > place -> sz) throw invalid_iterator();
                ++pos;
                return *this;
            }
            const_iterator operator--(int) {
                if (pos <= 1) throw invalid_iterator();
                const_iterator ans = *this;
                --pos;
                return ans;
            }
            const_iterator& operator--() {
                if (pos <= 1) throw invalid_iterator();
                --pos;
                return *this;
            }
            const T& operator*() const {
                return find1(place -> root, pos) -> data;
            }
            const T* operator->() const noexcept {
                return &(find1(place -> root, pos) -> data);
            }
            bool operator==(const iterator &rhs) const {
                return pos == rhs.pos && place == rhs.place;
            }
            bool operator==(const const_iterator &rhs) const {
                return pos == rhs.pos && place == rhs.place;
            }
            bool operator!=(const iterator &rhs) const {
                return pos != rhs.pos || place != rhs.place;
            }
            bool operator!=(const const_iterator &rhs) const {
                return pos != rhs.pos || place != rhs.place;
            }
        };
        deque():root(NULL),sz(0) {}
        deque(const deque &other) {
            root = copy1(other . root);
            sz = other. sz;
        }
        ~deque() {
            clear1(root);
        }
        deque &operator=(const deque &other) {
            if (&other == this) return *this;
            clear1(root);
            root = copy1(other . root);
            sz = other. sz;
            return *this;
        }
        T & at(const size_t &pos) {
            if (pos<0||pos>=sz)throw index_out_of_bound();
            return find1(root , pos+1) -> data;
        }
        const T & at(const size_t &pos) const {
            if (pos<0||pos>=sz)throw index_out_of_bound();
            return find1(root , pos+1) -> data;
        }
        T & operator[](const size_t &pos) {
            if (pos<0||pos>=sz)throw index_out_of_bound();
            return find1(root , pos+1) -> data;
        }
        const T & operator[](const size_t &pos) const {
            if (pos<0||pos>=sz)throw index_out_of_bound();
            return find1(root , pos+1) -> data;
        }
        const T & front() const {if (sz==0)throw container_is_empty();return find1(root, 1) -> data;}
        const T & back() const {if (sz==0)throw container_is_empty();return find1(root, sz) -> data;}
        iterator begin() {
            return iterator(this, 1);
        }
        const_iterator cbegin() const {
            return const_iterator(this, 1);
        }
        iterator end() {
            return iterator(this, sz+1);
        }
        const_iterator cend() const {
            return const_iterator(this, sz+1);
        }
        bool empty() const {
            return sz==0;
        }
        size_t size() const {
            return sz;
        }
        void clear() {
            sz=0;
            clear1 (root);
            root = NULL;
        }
        iterator insert(iterator pos, const T &value) {
            if (pos.place != this || pos.pos < 1 || pos.pos > sz +1) throw invalid_iterator();
            Pair<Node <T> *, Node <T> * > ans = split(root , pos.pos-1);
            Node <T> * newp;
            newp = new Node <T> (value);
            root = merge(merge(ans.first, newp ) , ans.second);
            ++sz;return pos;
        }
        iterator erase(iterator pos) {
            if (pos.pos < 1 || pos.pos > sz) throw invalid_iterator();
            Pair<Node <T> *, Node <T> * > ans = split(root , pos.pos-1);
            Pair<Node <T> *, Node <T> * > ans2 = split(ans.second , 1);
            delete ans2.first;
            root = merge(ans.first, ans2.second);
            --sz;
            return pos;
        }
        void push_back(const T &value) {
            Node<T> *p;
            p = new Node<T> (value);
            root = merge(root, p);
            ++sz;
        }
        void pop_back() {
            if (sz==0) throw container_is_empty();
            Pair < Node <T> *, Node <T> * > ans = split(root, sz-1);
            delete ans.second;
            root = ans.first;
            --sz;
        }
        void push_front(const T &value) {
            Node<T> *p;
            p = new Node<T> (value);
            root = merge(p, root);
            ++sz;
        }
        void pop_front() {
            if (sz==0) throw container_is_empty();
            Pair < Node <T> *, Node <T> * > ans = split(root, 1);
            delete ans.first;
            root = ans.second;
            --sz;
        }
    };
}

#endif





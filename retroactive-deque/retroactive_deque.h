#ifndef RETROACTIVE_DEQUE_H_INCLUDED
#define RETROACTIVE_DEQUE_H_INCLUDED

#include <limits>
#include <map>
#include <random>
#include <set>

template<typename T>
class retroactive_deque {

private:
    struct treap {
        treap *L, *R;
        int prior;
        bool ins;
        long long tm, balance, min_pref, min_suff, max_suff;

        treap() { }

        treap(long long cur_time, bool inserted) : L(nullptr), R(nullptr),
                prior(((rand() & 0x7FFF) << 15) | (rand() & 0x7FFF)), ins(inserted),
                tm(cur_time), balance(ins ? 1 : -1), min_pref(balance), min_suff(balance), max_suff(balance) { }

        static inline long long get_balance(treap *t) { return t ? t->balance : 0; }

        static inline long long get_min_pref(treap *t) { return t ? t->min_pref : 0; }

        static inline long long get_min_suff(treap *t) { return t ? t->min_suff : 0; }

        static inline long long get_max_suff(treap *t) { return t ? t->max_suff : 0; }

        static inline void recalc(treap *t) {
            if (t) {
                t->balance = (t->ins ? 1 : -1) + treap::get_balance(t->L) + treap::get_balance(t->R);
                t->min_pref = std::min(t->L ? treap::get_min_pref(t->L) : std::numeric_limits<long long>::max(),
                              treap::get_balance(t->L) + (t->ins ? 1 : -1) + std::min(0LL, treap::get_min_pref(t->R)));
                t->min_suff = std::min(t->R ? treap::get_min_suff(t->R) : std::numeric_limits<long long>::max(),
                              treap::get_balance(t->R) + (t->ins ? 1 : -1) + std::min(0LL, treap::get_min_suff(t->L)));
                t->max_suff = std::max(t->R ? treap::get_max_suff(t->R) : std::numeric_limits<long long>::min(),
                              treap::get_balance(t->R) + (t->ins ? 1 : -1) + std::max(0LL, treap::get_max_suff(t->L)));
            }
        }

        static void merge(treap *& t, treap *l, treap *r) {
            if (!l)
                t = r;
            else if (!r)
                t = l;
            else if (l->prior > r->prior) {
                treap::merge(l->R, l->R, r);
                t = l;
            } else {
                treap::merge(r->L, l, r->L);
                t = r;
            }
            treap::recalc(t);
        }

        static void split(treap *t, treap *& l, treap *& r, long long x) { // <=x -> L,   >x -> R
            if (!t) {
                l = r = nullptr;
                return;
            }

            if (t->tm <= x) {
                treap::split(t->R, t->R, r, x);
                l = t;
            } else {
                treap::split(t->L, l, t->L, x);
                r = t;
            }
            treap::recalc(l);
            treap::recalc(r);
        }

        static void destroy(treap *t) {
            if (t) {
                treap::destroy(t->L);
                treap::destroy(t->R);
                delete t;
            }
        }

        static void copy(treap *dest, treap *src) {
            *dest = *src;
            if (src->L) {
                dest->L = new treap();
                treap::copy(dest->L, src->L);
            }
            if (src->R) {
                dest->R = new treap();
                treap::copy(dest->R, src->R);
            }
        }

        static void insert(treap *& t, long long tm, bool ins) {
            treap *t1, *t2;
            treap::split(t, t1, t2, tm);
            treap::merge(t1, t1, new treap(tm, ins));
            treap::merge(t, t1, t2);
        }

        static void erase(treap *& t, long long tm) {
            treap *t1, *t2, *t3;
            treap::split(t, t1, t3, tm);
            treap::split(t1, t1, t2, tm - 1);
            if (t2)
                delete t2;
            treap::merge(t, t1, t3);
        }

        static long long get_kth(treap *t, long long k) { // 1-indexing
            while (t) {
                if (t->R) {
                    if (k >= treap::get_min_suff(t->R) && k <= treap::get_max_suff(t->R)) {
                        t = t->R;
                        continue;
                    }
                }
                long long right_balance = treap::get_balance(t->R) + (t->ins ? 1 : -1);
                if (right_balance == k)
                    return t->tm;
                k -= right_balance;
                t = t->L;
            }
            return std::numeric_limits<long long>::max(); // epic fail
        }
    };

    std::map<long long, T> operations;
    std::set<long long> pop_operations;
    treap *ul, *ur;
    treap *balance_tree;

    inline long long get_last_time() {
        if (operations.empty() && pop_operations.empty())
            return 0;
        return std::max(operations.empty() ? std::numeric_limits<long long>::min() : operations.rbegin()->first,
                        pop_operations.empty() ? std::numeric_limits<long long>::min() : *pop_operations.rbegin()) + 1;
    }

    inline bool check_valid() {
        return treap::get_min_pref(balance_tree) >= 0;
    }

public:
    /*** Friend operators ***/
    template<class T1>
        friend bool operator==(const retroactive_deque<T1>& x, const retroactive_deque<T1>& y);
    template<class T1>
        friend bool operator!=(const retroactive_deque<T1>& x, const retroactive_deque<T1>& y);


    /*** Constructors and destructor ***/
    retroactive_deque<T>() : ul(nullptr), ur(nullptr), balance_tree(nullptr) { }

    retroactive_deque<T>(const retroactive_deque<T>& other) {
        operations = other.operations;
        pop_operations = other.pop_operations;
        ul = new treap();
        treap::copy(ul, other.ul);
        ur = new treap();
        treap::copy(ur, other.ur);
        balance_tree = new treap();
        treap::copy(balance_tree, other.balance_tree);
    }

    ~retroactive_deque<T>() {
        treap::destroy(ul);
        treap::destroy(ur);
        treap::destroy(balance_tree);
    }


    /*** Operators ***/
    retroactive_deque<T>& operator=(const retroactive_deque<T>& other) {
        operations = other.operations;
        pop_operations = other.pop_operations;
        treap::destroy(ul);
        treap::destroy(ur);
        treap::destroy(balance_tree);
        ul = new treap();
        treap::copy(ul, other.ul);
        ur = new treap();
        treap::copy(ur, other.ur);
        balance_tree = new treap();
        treap::copy(balance_tree, other.balance_tree);
        return *this;
    }


    /*** Retroactive queries ***/
    bool insert_push_operation(const T& x, long long tm, bool back_op) {
        if (operations.find(tm) != operations.end() || pop_operations.find(tm) != pop_operations.end())
            return false;

        treap::insert(balance_tree, tm, true);
        if (!check_valid()) {
            treap::erase(balance_tree, tm);
            return false;
        }

        operations[tm] = x;
        treap::insert(back_op ? ur : ul, tm, true);
        return true;
    }

    bool insert_push_back(const T& x, long long tm) {
        return insert_push_operation(x, tm, true);
    }

    bool insert_push_front(const T& x, long long tm) {
        return insert_push_operation(x, tm, false);
    }

    bool insert_pop_operation(long long tm, bool back_op) {
        if (operations.find(tm) != operations.end() || pop_operations.find(tm) != pop_operations.end())
            return false;

        treap::insert(balance_tree, tm, false);
        if (!check_valid()) {
            treap::erase(balance_tree, tm);
            return false;
        }

        pop_operations.insert(tm);
        treap::insert(back_op ? ur : ul, tm, false);
        return true;
    }

    bool insert_pop_back(long long tm) {
        return insert_pop_operation(tm, true);
    }

    bool insert_pop_front(long long tm) {
        return insert_pop_operation(tm, false);
    }

    bool delete_operation(long long tm) {
        auto op_it = operations.find(tm);
        if (op_it != operations.end()) { // it was push operation
            treap::erase(balance_tree, tm);
            if (!check_valid()) {
                treap::insert(balance_tree, tm, true);
                return false;
            }

            treap::erase(ul, tm);
            treap::erase(ur, tm);
            operations.erase(op_it);
            return true;
        }

        auto pop_op_it = pop_operations.find(tm);
        if (pop_op_it != pop_operations.end()) { // it was pop operation
            treap::erase(balance_tree, tm);
            if (!check_valid()) {
                treap::insert(balance_tree, tm, false);
                return false;
            }

            treap::erase(ul, tm);
            treap::erase(ur, tm);
            pop_operations.erase(pop_op_it);
            return true;
        }

        return false; // there wasn't any operation with that time
    }

    /// Time for the most difficult part!
    T back(long long tm = std::numeric_limits<long long>::max()) {
        T ans;

        treap *ul1, *ul2;
        treap *ur1, *ur2;
        treap::split(ul, ul1, ul2, tm);
        treap::split(ur, ur1, ur2, tm);
        if (treap::get_max_suff(ur1) > std::max(0LL, -treap::get_min_pref(ul1)))
            ans = operations[treap::get_kth(ur1, 1)];
        else {
            long long index = treap::get_balance(ul1) + treap::get_balance(ur1);
            ans = operations[treap::get_kth(ul1, index)];
        }
        treap::merge(ul, ul1, ul2);
        treap::merge(ur, ur1, ur2);

        return ans;
    }

    T front(long long tm = std::numeric_limits<long long>::max()) {
        T ans;

        treap *ul1, *ul2;
        treap *ur1, *ur2;
        treap::split(ul, ul1, ul2, tm);
        treap::split(ur, ur1, ur2, tm);
        if (treap::get_max_suff(ul1) > std::max(0LL, -treap::get_min_pref(ur1)))
            ans = operations[treap::get_kth(ul1, 1)];
        else {
            long long index = treap::get_balance(ur1) + treap::get_balance(ul1);
            ans = operations[treap::get_kth(ur1, index)];
        }
        treap::merge(ul, ul1, ul2);
        treap::merge(ur, ur1, ur2);

        return ans;
    }


    /*** Present-time queries ***/
    long long push_back(const T& x) {
        long long tm = get_last_time();
        insert_push_back(x, tm); // assuming it is always successful
        return tm;
    }

    long long push_front(const T& x) {
        long long tm = get_last_time();
        insert_push_front(x, tm); // assuming it is always successful
        return tm;
    }

    long long pop_back() {
        long long tm = get_last_time();
        insert_pop_back(tm); // calling it on an empty deque causes undefined behavior
        return tm;
    }

    long long pop_front() {
        long long tm = get_last_time();
        insert_pop_front(tm); // calling it on an empty deque causes undefined behavior
        return tm;
    }

    void clear() {
        operations.clear();
        pop_operations.clear();
        treap::destroy(ul);
        ul = nullptr;
        treap::destroy(ur);
        ur = nullptr;
        treap::destroy(balance_tree);
        balance_tree = nullptr;
    }

    inline size_t size() {
        return treap::get_balance(balance_tree);
    }

    inline bool empty() {
        return size() == 0;
    }
};


/*** Friend operators implementation ***/
template<class T>
inline bool operator==(const retroactive_deque<T>& x, const retroactive_deque<T>& y) {
    return x.operations == y.operations && x.pop_operations == y.pop_operations;
}

template<class T>
inline bool operator!=(const retroactive_deque<T>& x, const retroactive_deque<T>& y) {
    return !(x == y);
}

#endif // RETROACTIVE_DEQUE_H_INCLUDED

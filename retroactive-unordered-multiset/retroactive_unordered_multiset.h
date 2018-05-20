#ifndef RETROACTIVE_UNORDERED_MULTISET_H_INCLUDED
#define RETROACTIVE_UNORDERED_MULTISET_H_INCLUDED

#include <functional>
#include <limits>
#include <map>
#include <set>
#include <utility>
#include <vector>

template<typename T>
class retroactive_unordered_multiset {

private:
    struct treap {
        treap *L, *R;
        int prior;
        bool ins;
        long long tm, balance, min_pref, max_suff;

        treap() { }

        treap(long long cur_time, bool inserted) : L(nullptr), R(nullptr),
                prior(((rand() & 0x7FFF) << 15) | (rand() & 0x7FFF)), ins(inserted),
                tm(cur_time), balance(ins ? 1 : -1), min_pref(balance), max_suff(balance) { }

        static inline long long get_balance(treap *t) { return t ? t->balance : 0; }

        static inline long long get_min_pref(treap *t) { return t ? t->min_pref : 0; }

        static inline long long get_max_suff(treap *t) { return t ? t->max_suff : 0; }

        static inline void recalc(treap *t) {
            if (t) {
                t->balance = (t->ins ? 1 : -1) + treap::get_balance(t->L) + treap::get_balance(t->R);
                t->min_pref = std::min(t->L ? treap::get_min_pref(t->L) : std::numeric_limits<long long>::max(),
                              treap::get_balance(t->L) + (t->ins ? 1 : -1) + std::min(0LL, treap::get_min_pref(t->R)));
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

        static void fill_ins_vector(treap *t, std::vector<bool> & v) { // necessary for sequences comparisons
            if (t) {
                treap::fill_ins_vector(t->L, v);
                v.push_back(t->ins);
                treap::fill_ins_vector(t->R, v);
            }
        }
    };

    std::map<long long, T> operations;
    std::map<T, treap*> sequences;

    inline long long get_last_time() {
        return operations.empty() ? 0 : operations.rbegin()->first + 1;
    }

    inline bool check_valid(const T& x) {
        return treap::get_min_pref(sequences[x]) >= 0;
    }

public:
    /*** Friend operators ***/
    template<class T1>
        friend bool operator==(const retroactive_unordered_multiset<T1>& x, const retroactive_unordered_multiset<T1> &y);
    template<class T1>
        friend bool operator!=(const retroactive_unordered_multiset<T1>& x, const retroactive_unordered_multiset<T1> &y);


    /*** Constructors and destructor ***/
    retroactive_unordered_multiset<T>() : operations(), sequences() { }

    retroactive_unordered_multiset<T>(const retroactive_unordered_multiset<T>& other) {
        operations = other.operations;
        sequences = other.sequences;
        for (auto it = sequences.begin(); it != sequences.end(); ++it) {
            it->second = new treap();
            treap::copy(it->second, other.sequences[it->first]);
        }
    }

    ~retroactive_unordered_multiset<T>() {
        for (auto it = sequences.begin(); it != sequences.end(); ++it)
            treap::destroy(it->second);
    }


    /*** Operators ***/
    retroactive_unordered_multiset<T>& operator=(const retroactive_unordered_multiset<T>& other) {
        operations = other.operations;
        for (auto it = sequences.begin(); it != sequences.end(); ++it)
            treap::destroy(it->second);
        sequences = other.sequences;
        for (auto it = sequences.begin(); it != sequences.end(); ++it) {
            it->second = new treap();
            treap::copy(it->second, other.sequences[it->first]);
        }
        return *this;
    }


    /*** Retroactive updates and queries ***/
    bool insert(const T& x, long long tm) {
        if (operations.find(tm) != operations.end())
            return false;

        treap::insert(sequences[x], tm, true);
        operations[tm] = x;
        return true;
    }

    bool erase(const T& x, long long tm) {
        if (operations.find(tm) != operations.end())
            return false;

        treap::insert(sequences[x], tm, false);
        if (!check_valid(x)) {
            auto seq_it = sequences.find(x);
            treap::erase(seq_it->second, tm);
            if (!seq_it->second)
                sequences.erase(seq_it);
            return false;
        }
        operations[tm] = x;
        return true;
    }

    bool delete_operation(long long tm) {
        auto it = operations.find(tm);
        if (it == operations.end())
            return false;

        auto seq_it = sequences.find(it->second);
        treap::erase(seq_it->second, tm);
        if (!check_valid(seq_it->first)) {
            // It was insert operation, since erasing removal couldn't cause inconsistence
            treap::insert(seq_it->second, tm, true);
            return false;
        }
        if (!seq_it->second)
            sequences.erase(seq_it);
        operations.erase(tm);
        return true;
    }

    bool find(const T& x, long long tm = std::numeric_limits<long long>::max()) {
        auto seq_it = sequences.find(x);
        if (seq_it == sequences.end())
            return false;

        treap *s1, *s2;
        treap::split(seq_it->second, s1, s2, tm);
        bool ans = (treap::get_max_suff(s1) > 0);
        treap::merge(seq_it->second, s1, s2);
        return ans;
    }


    /*** Present-time updates ***/
    bool insert(const T& x) {
        return insert(x, get_last_time()); // we assume that the operation is always successful
    }

    bool erase(const T& x) {
        return erase(x, get_last_time()); // we assume that the operation is always successful
    }

    void clear() {
        operations.clear();
        for (auto it = sequences.begin(); it != sequences.end(); ++it)
            treap::destroy(it->second);
        sequences.clear();
    }
};


/*** Friend operators implementation ***/
template<class T>
inline bool operator==(const retroactive_unordered_multiset<T>& x, const retroactive_unordered_multiset<T> &y) {
    if (x.sequences.size() != y.sequences.size() || x.operations != y.operations)
        return false;
    for (auto it = x.sequences.begin(); it != x.sequences.end(); ++it) {
        std::vector<bool> vx, vy;
        retroactive_unordered_multiset<T>::treap::fill_ins_vector(it->second, vx);
        retroactive_unordered_multiset<T>::treap::fill_ins_vector(y.sequences[it->first], vy);
        if (vx != vy)
            return false;
    }
    return true;
}

template<class T>
inline bool operator!=(const retroactive_unordered_multiset<T>& x, const retroactive_unordered_multiset<T> &y) {
    return !(x == y);
}

#endif // RETROACTIVE_UNORDERED_MULTISET_H_INCLUDED

#ifndef RETROACTIVE_SET_H_INCLUDED
#define RETROACTIVE_SET_H_INCLUDED

#include <algorithm>
#include <map>
#include <set>
#include <vector>

template<typename T>
class retroactive_set {

private:
    struct segtree {
        segtree *L, *R;
        std::set<T> bucket;

        segtree() : L(nullptr), R(nullptr), bucket() { }

        void add(long long l, long long r, const T& x,
                 long long tl = std::numeric_limits<long long>::min(),
                 long long tr = std::numeric_limits<long long>::max()) {
            if (tl == l && tr == r)
                this->bucket.insert(x);
            else {
                long long tm = (tl >> 1) + (tr >> 1) + (tl & tr & 1LL); // overflow-safe calculation of mean value
                if (l <= tm) {
                    if (!this->L)
                        this->L = new segtree();
                    this->L->add(l, std::min(r, tm), x, tl, tm);
                }
                if (r > tm) {
                    if (!this->R)
                        this->R = new segtree();
                    this->R->add(std::max(l, tm + 1), r, x, tm + 1, tr);
                }
            }
        }

        void remove(long long l, long long r, const T& x,
                    long long tl = std::numeric_limits<long long>::min(),
                    long long tr = std::numeric_limits<long long>::max()) {
            if (tl == l && tr == r)
                this->bucket.erase(x);
            else {
                long long tm = (tl >> 1) + (tr >> 1) + (tl & tr & 1LL); // overflow-safe calculation of mean value
                if (l <= tm) // don't need nullptr checks since these node are guaranteed to exist after adding
                    this->L->remove(l, std::min(r, tm), x, tl, tm);
                if (r > tm)
                    this->R->remove(std::max(l, tm + 1), r, x, tm + 1, tr);
            }
        }

        T lower_bound(long long t, const T& x,
                      long long tl = std::numeric_limits<long long>::min(),
                      long long tr = std::numeric_limits<long long>::max()) {
            T ans = std::numeric_limits<T>::max(); // we assume for now that the type T is numeric
            segtree *tree = this;
            while (tree) {
                auto bucket_it = tree->bucket.lower_bound(x);
                if (bucket_it != tree->bucket.end())
                    ans = std::min(ans, *bucket_it);

                long long tm = (tl >> 1) + (tr >> 1) + (tl & tr & 1LL); // overflow-safe calculation of mean value
                if (t <= tm) {
                    tree = tree->L;
                    tr = tm;
                } else {
                    tree = tree->R;
                    tl = tm + 1;
                }
            }
            return ans;
        }

        T upper_bound(long long t, const T& x,
                      long long tl = std::numeric_limits<long long>::min(),
                      long long tr = std::numeric_limits<long long>::max()) {
            T ans = std::numeric_limits<T>::max(); // we assume for now that the type T is numeric
            segtree *tree = this;
            while (tree) {
                auto bucket_it = tree->bucket.upper_bound(x);
                if (bucket_it != tree->bucket.end())
                    ans = std::min(ans, *bucket_it);

                long long tm = (tl >> 1) + (tr >> 1) + (tl & tr & 1LL); // overflow-safe calculation of mean value
                if (t <= tm) {
                    tree = tree->L;
                    tr = tm;
                } else {
                    tree = tree->R;
                    tl = tm + 1;
                }
            }
            return ans;
        }

        void destroy() {
            if (this->L)
                this->L->destroy();
            if (this->R)
                this->R->destroy();
            delete this;
        }

        void copy(segtree *src) {
            this->bucket = src->bucket;
            if (src->L) {
                this->L = new segtree();
                this->L->copy(src->L);
            }
            if (src->R) {
                this->R = new segtree();
                this->R->copy(src->R);
            }
        }
    };

    std::map<long long, T> operations;
    std::map<T, std::vector<long long>> sequences;
    segtree *tree;

    inline long long get_last_time() {
        return operations.empty() ? 0 : operations.rbegin()->first + 1;
    }

public:
    /*** Friend operators ***/
    template<typename T1>
        friend bool operator==(const retroactive_set<T1>& x, const retroactive_set<T1>& y);
    template<typename T1>
        friend bool operator!=(const retroactive_set<T1>& x, const retroactive_set<T1>& y);


    /*** Constructors and destructor ***/
    retroactive_set<T>() : operations(), sequences(), tree(new segtree()) { }

    retroactive_set<T>(const retroactive_set<T>& other) {
        operations = other.operations;
        sequences = other.sequence;
        tree = new segtree();
        tree->copy(other.tree);
    }

    ~retroactive_set<T>() {
        tree->destroy();
    }


    /*** Operators ***/
    retroactive_set<T>& operator=(const retroactive_set<T>& other) {
        operations = other.operations;
        sequences = other.sequences;
        tree->destroy();
        tree = new segtree();
        tree->copy(other.tree);
        return *this;
    }


    /*** Retroactive updates and queries ***/
    bool insert(const T& x, long long tm) {
        if (operations.find(tm) != operations.end())
            return false;

        std::vector<long long>& events = sequences[x];
        if (events.size() % 2 != 0 || (!events.empty() && events.back() > tm))
            return false;

        operations[tm] = x;
        tree->add(tm, std::numeric_limits<long long>::max(), x);
        events.push_back(tm);
        return true;
    }

    bool erase(const T& x, long long tm) {
        if (operations.find(tm) != operations.end())
            return false;

        std::vector<long long>& events = sequences[x];
        if (events.size() % 2 == 0 || events.back() > tm)
            return false;

        operations[tm] = x;
        long long prev_tm = events.back();
        tree->remove(prev_tm, std::numeric_limits<long long>::max(), x);
        tree->add(prev_tm, tm - 1, x);
        events.push_back(tm);
        return true;
    }

    bool delete_operation(long long tm) {
        auto it = operations.find(tm);
        if (it == operations.end())
            return false;

        std::vector<long long>& events = sequences[it->second];
        if (events.back() != tm) // we can delete only last operation for each element
            return false;

        events.pop_back();
        if (events.size() % 2 != 0) { // delete "erase" operation
            long long prev_tm = events.back();
            tree->remove(prev_tm, tm - 1, it->second);
            tree->add(prev_tm, std::numeric_limits<long long>::max(), it->second);
        } else
            tree->remove(tm, std::numeric_limits<long long>::max(), it->second);
        operations.erase(it);
        return true;
    }

    T lower_bound(const T& x, long long tm = std::numeric_limits<long long>::max()) {
        return tree->lower_bound(tm, x);
    }

    T upper_bound(const T& x, long long tm = std::numeric_limits<long long>::max()) {
        return tree->upper_bound(tm, x);
    }

    bool find(const T& x, long long tm = std::numeric_limits<long long>::max()) {
        return lower_bound(x, tm) == x;
    }


    /*** Present-time queries ***/
    bool insert(const T& x) {
        return insert(x, get_last_time());
    }

    bool erase(const T& x) {
        return erase(x, get_last_time());
    }

    void clear() {
        operations.clear();
        sequences.clear();
        tree->destroy();
        tree = new segtree();
    }
};


/*** Friend operators implementation ***/
template<class T>
inline bool operator==(const retroactive_set<T>& x, const retroactive_set<T> &y) {
    return x.operations == y.operations;
}

template<class T>
inline bool operator!=(const retroactive_set<T>& x, const retroactive_set<T> &y) {
    return !(x == y);
}

#endif // RETROACTIVE_SET_H_INCLUDED

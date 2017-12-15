#ifndef RETROACTIVE_DICTIONARY_H_INCLUDED
#define RETROACTIVE_DICTIONARY_H_INCLUDED

#include <functional>
#include <limits>
#include <map>
#include <set>
#include <utility>

template<typename T>
class retroactive_dictionary {

private:
    std::map<long long, T> operations;
    std::map<T, std::map<long long, bool, std::greater<long long>>> sequences; // (time/is insert operation)

    inline long long get_last_time() {
        return operations.empty() ? 0 : operations.rbegin()->first + 1;
    }

public:
    /*** Friend operators ***/
    template<class T1>
        friend bool operator==(const retroactive_dictionary<T1>& x, const retroactive_dictionary<T1> &y);
    template<class T1>
        friend bool operator!=(const retroactive_dictionary<T1>& x, const retroactive_dictionary<T1> &y);


    /*** Constructors and destructor ***/
    retroactive_dictionary<T>() : operations(), sequences() { }

    retroactive_dictionary<T>(const retroactive_dictionary<T>& other) {
        operations = other.operations;
        sequences = other.sequences;
    }

    ~retroactive_dictionary<T>() { }


    /*** Operators ***/
    retroactive_dictionary<T>& operator=(const retroactive_dictionary<T>& other) {
        operations = other.operations;
        sequences = other.sequences;
        return *this;
    }


    /*** Retroactive updates and queries ***/
    bool insert(const T& x, long long tm) {
        // If the element is already in the set, this operations has no effect.
        if (operations.find(tm) != operations.end())
            return false;

        operations[tm] = x;
        sequences[x][tm] = true;
        return true;
    }

    bool erase(const T& x, long long tm) {
        // If the element has already been erased from the set, this operations has no effect.
        if (operations.find(tm) != operations.end())
            return false;

        operations[tm] = x;
        sequences[x][tm] = false;
        return true;
    }

    bool delete_operation(long long tm) {
        auto it = operations.find(tm);
        if (it == operations.end())
            return false;

        auto seq_it = sequences.find(it->second);
        if (seq_it->second.size() == 1)
            sequences.erase(seq_it);
        else
            seq_it->second.erase(tm);
        operations.erase(tm);
        return true;
    }

    bool find(const T& x, long long tm = std::numeric_limits<long long>::max()) {
        auto seq_it = sequences.find(x);
        if (seq_it == sequences.end())
            return false;

        auto it = seq_it->second.lower_bound(tm);
        return it != seq_it->second.end() && it->second;
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
        sequences.clear();
    }
};


/*** Friend operators implementation ***/
template<class T>
inline bool operator==(const retroactive_dictionary<T>& x, const retroactive_dictionary<T> &y) {
    return x.operations == y.operations;
}

template<class T>
inline bool operator!=(const retroactive_dictionary<T>& x, const retroactive_dictionary<T> &y) {
    return !(x == y);
}

#endif // RETROACTIVE_DICTIONARY_H_INCLUDED

#ifndef PARTIALLY_RETROACTIVE_SET_H_INCLUDED
#define PARTIALLY_RETROACTIVE_SET_H_INCLUDED

#include <algorithm>
#include <map>
#include <set>
#include <vector>

template<typename T>
class partially_retroactive_set {

private:
    std::map<long long, T> operations;
    std::map<T, std::vector<long long>> sequences;
    std::set<T> elements;

    inline long long get_last_time() {
        return operations.empty() ? 0 : operations.rbegin()->first + 1;
    }

public:
    /*** Friend operators ***/
    template<typename T1>
        friend bool operator==(const partially_retroactive_set<T1>& x, const partially_retroactive_set<T1>& y);
    template<typename T1>
        friend bool operator!=(const partially_retroactive_set<T1>& x, const partially_retroactive_set<T1>& y);


    /*** Constructors and destructor ***/
    partially_retroactive_set<T>() : operations(), sequences(), elements() { }

    partially_retroactive_set<T>(const partially_retroactive_set<T>& other) {
        operations = other.operations;
        sequences = other.sequence;
        elements = other.elements;
    }

    ~partially_retroactive_set<T>() { }


    /*** Operators ***/
    partially_retroactive_set<T>& operator=(const partially_retroactive_set<T>& other) {
        operations = other.operations;
        sequences = other.sequences;
        elements = other.elements;
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
        elements.insert(x);
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
        elements.erase(x);
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
        if (events.size() % 2 != 0) // delete "erase" operation
            elements.insert(it->second);
        else
            elements.erase(it->second);
        operations.erase(it);
        return true;
    }


    /*** Present-time queries ***/
    bool insert(const T& x) {
        return insert(x, get_last_time());
    }

    bool erase(const T& x) {
        return erase(x, get_last_time());
    }

    T lower_bound(const T& x) {
        return *elements.lower_bound(x);
    }

    T upper_bound(const T& x) {
        return *elements.upper_bound(x);
    }

    bool find(const T& x) {
        return elements.find(x) != elements.end();
    }

    void clear() {
        operations.clear();
        sequences.clear();
        elements.clear();
    }
};


/*** Friend operators implementation ***/
template<class T>
inline bool operator==(const partially_retroactive_set<T>& x, const partially_retroactive_set<T> &y) {
    return x.operations == y.operations;
}

template<class T>
inline bool operator!=(const partially_retroactive_set<T>& x, const partially_retroactive_set<T> &y) {
    return !(x == y);
}

#endif // PARTIALLY_RETROACTIVE_SET_H_INCLUDED

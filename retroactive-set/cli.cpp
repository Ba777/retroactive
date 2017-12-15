#include <fstream>
#include <iostream>
#include <limits>
#include <string>

#include "retroactive_set.h"

using namespace std;

void run(istream& cin, bool allow_files = false) {

    retroactive_set<int> s;

    string operation;
    int x;
    long long tm;

    while ((cin >> operation) && operation != "finish") {
        if (operation == "insert") {
            cin >> x;
            bool success = s.insert(x);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "insert_retro") {
            cin >> x >> tm;
            bool success = s.insert(x, tm);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "erase") {
            cin >> x;
            bool success = s.erase(x);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "erase_retro") {
            cin >> x >> tm;
            bool success = s.erase(x, tm);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "delete_operation") {
            cin >> tm;
            bool success = s.delete_operation(tm);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "lower_bound") {
            cin >> x;
            int answer = s.lower_bound(x);
            if (answer == numeric_limits<int>::max())
                cout << "No such element" << endl;
            else
                cout << answer << endl;

        } else if (operation == "lower_bound_retro") {
            cin >> x >> tm;
            int answer = s.lower_bound(x, tm);
            if (answer == numeric_limits<int>::max())
                cout << "No such element" << endl;
            else
                cout << answer << endl;

        } else if (operation == "upper_bound") {
            cin >> x;
            int answer = s.upper_bound(x);
            if (answer == numeric_limits<int>::max())
                cout << "No such element" << endl;
            else
                cout << answer << endl;

        } else if (operation == "upper_bound_retro") {
            cin >> x >> tm;
            int answer = s.upper_bound(x, tm);
            if (answer == numeric_limits<int>::max())
                cout << "No such element" << endl;
            else
                cout << answer << endl;

        } else if (operation == "find") {
            cin >> x;
            bool success = s.find(x);
            cout << (success ? "found" : "not found") << endl;

        } else if (operation == "find_retro") {
            cin >> x >> tm;
            bool success = s.find(x, tm);
            cout << (success ? "found" : "not found") << endl;

        } else if (operation == "run" && allow_files) {
            string filename;
            cin >> filename;
            ifstream fin(filename);
            run(fin);

        } else if (operation == "clear") {
            s.clear();

        }
    }
}

int main()
{
    run(cin, true);

    return 0;
}

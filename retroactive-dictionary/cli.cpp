#include <fstream>
#include <iostream>
#include <string>

#include "retroactive_dictionary.h"

using namespace std;

void run(istream& cin, bool allow_files = false) {
    retroactive_dictionary<string> rd;

    string operation;
    string x;
    long long tm;
    while ((cin >> operation) && operation != "finish") {
        if (operation == "insert") {
            cin >> x;
            bool success = rd.insert(x);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "insert_retro") {
            cin >> x >> tm;
            bool success = rd.insert(x, tm);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "erase") {
            cin >> x;
            bool success = rd.erase(x);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "erase_retro") {
            cin >> x >> tm;
            bool success = rd.erase(x, tm);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "delete_operation") {
            cin >> tm;
            bool success = rd.delete_operation(tm);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "find") {
            cin >> x;
            bool success = rd.find(x);
            cout << (success ? "found" : "not found") << endl;

        } else if (operation == "find_retro") {
            cin >> x >> tm;
            bool success = rd.find(x, tm);
            cout << (success ? "found" : "not found") << endl;

        } else if (operation == "run" && allow_files) {
            string filename;
            cin >> filename;
            ifstream fin(filename);
            run(fin);

        } else if (operation == "clear") {
            rd.clear();

        }
    }
}

int main()
{
    run(cin, true);

    return 0;
}

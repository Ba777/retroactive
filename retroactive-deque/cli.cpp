#include <fstream>
#include <iostream>
#include <string>

#include "retroactive_deque.h"

using namespace std;

void run(istream& cin, bool allow_files = false) {

    retroactive_deque<int> q;

    string operation;
    int x;
    long long tm;

    while ((cin >> operation) && operation != "finish") {
        if (operation == "push_back") {
            cin >> x;
            long long insert_time = q.push_back(x);
            cout << insert_time << endl;

        } else if (operation == "push_back_retro") {
            cin >> x >> tm;
            bool success = q.insert_push_back(x, tm);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "push_front") {
            cin >> x;
            long long insert_time = q.push_front(x);
            cout << insert_time << endl;

        } else if (operation == "push_front_retro") {
            cin >> x >> tm;
            bool success = q.insert_push_front(x, tm);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "pop_back") {
            if (q.empty())
                cout << "not ok" << endl;
            else {
                long long insert_time = q.pop_back();
                cout << insert_time << endl;
            }

        } else if (operation == "pop_back_retro") {
            cin >> tm;
            bool success = q.insert_pop_back(tm);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "pop_front") {
            if (q.empty())
                cout << "not ok" << endl;
            else {
                long long insert_time = q.pop_front();
                cout << insert_time << endl;
            }

        } else if (operation == "pop_front_retro") {
            cin >> tm;
            bool success = q.insert_pop_front(tm);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "delete_operation") {
            cin >> tm;
            bool success = q.delete_operation(tm);
            cout << (success ? "ok" : "not ok") << endl;

        } else if (operation == "back") {
            if (q.empty())
                cout << "not ok" << endl;
            else
                cout << q.back() << endl;

        } else if (operation == "back_retro") {
            cin >> tm;
            cout << q.back(tm) << endl;

        } else if (operation == "front") {
            if (q.empty())
                cout << "not ok" << endl;
            else
                cout << q.front() << endl;

        } else if (operation == "front_retro") {
            cin >> tm;
            cout << q.front(tm) << endl;

        } else if (operation == "size") {
            cout << q.size() << endl;

        } else if (operation == "run" && allow_files) {
            string filename;
            cin >> filename;
            ifstream fin(filename);
            run(fin);

        } else if (operation == "clear") {
            q.clear();

        }
    }
}

int main()
{
    run(cin, true);

    return 0;
}

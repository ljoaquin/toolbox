#include <iostream>
using namespace std;

struct A
{
    string s;
    A() : s("test") {cout << "default!\n";}
    A(const A& o) : s(o.s) {cout << "copy!\n";}
    A(A&& o) noexcept : s(move(o.s)) {cout << "move ok!\n";}
};

void foo(string& from, string& to)
{
    to = move(from);
}

void foo1(const string& from, string& to)
{
    to = move(from);
}

void foo2(string&& from, string& to)
{
    to = from;
}

void foo3(string&& from, string& to)
{
    to = move(from);
}

int main()
{
    A a1;
    A a2 = a1;
    A a3 = move(a2);

    cout << a3.s << endl;

    string str1 = "yo";
    string str2;

    cout << "str1:" << str1 << "," << "str1:" << str2 << endl;

    foo(str1, str2);
    // foo2(move(str1), str2);
    // foo3(move(str1), str2);

    cout << "str1:" << str1 << "," << "str1:" << str2 << endl;

    return 0;
}

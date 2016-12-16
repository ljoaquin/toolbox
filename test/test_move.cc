#include <iostream>
using namespace std;

struct A
{
    string s;
    A() : s("test") {cout << "default!\n";}
    A(const A& o) : s(o.s) {cout << "copy!\n";}
    A(A&& o) noexcept : s(move(o.s)) {cout << "move ok!\n";}
};

int main()
{
    A a1;
    A a2 = a1;
    A a3 = move(a2);

    cout << a3.s << endl;

    return 0;
}

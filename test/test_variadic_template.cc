#include <iostream>
#include <sstream>
#include <functional>

class Debug
{
public:
    static void log(const char* str);

    template<typename T, typename... Targs>
    static void log(const char* format, T arg1, Targs... args);

    static std::function<void(const char*)> m_log_func;

private:
    static void on_output();

    static std::stringstream m_output;
};

std::function<void(const char*)> Debug::m_log_func = NULL;
std::stringstream Debug::m_output;

void Debug::on_output()
{
    if(m_log_func)
    {
        m_log_func(m_output.str().c_str());
    }
    m_output.str("");
}

// no args
void Debug::log(const char* str)
{
    m_output << str;
    on_output();
}

template<typename T, typename... Targs>
void Debug::log(const char* format, T arg1, Targs... args)
{
    for(; *format != 0; format++)
    {
        if(*format == '%')
        {
            m_output << arg1;
            log(format + 1, args...);
            return;
        }
        m_output << *format;
    }
    // no '%' but some args
    on_output();
}

template<typename T>
void some_function(T arg)
{
    std::cout << "some_function:" << arg << "\n";
}

struct pass
{
    template<typename ...T>
    pass(T...){};
};

template<typename... Targs>
void expand(Targs... args)
{
    pass{(some_function(args), 1)...};
}

void test_log()
{
    Debug::log("this should not be printed");

    Debug::m_log_func = [](const char* str){std::cout << "[log]" << str << "\n";};

    Debug::log("% world% % hohoho","Hello",'!',123);
    Debug::log("i am % years old and my name is % hohoho",17,"lj");

    Debug::log("hello % ljaylin");
    Debug::log("hello ljaylin", 2);

}

void test_unpack_args()
{
    expand("lj", 17, 15.5f);
}

int main(int argc, char const *argv[])
{
    test_unpack_args();
    return 0;
}

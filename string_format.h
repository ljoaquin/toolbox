#ifndef __STRING_FORMAT_H__
#define __STRING_FORMAT_H__

#include <vector>
#include <sstream>

namespace toolbox
{
    struct pass
    {
        template<typename... Targs>
        pass(Targs...){};
    };

    class StringFormat
    {
    public:
        template<typename... Targs>
        static std::string format(std::string fmt, Targs... args);

    private:
        template<typename T>
        static void collect(T arg);

        static std::string fill(std::string fmt);

        static std::vector<std::string> s_vector_args;
    };

    template<typename... Targs>
    std::string StringFormat::format(std::string fmt, Targs... args)
    {
        s_vector_args.clear();
        pass{(collect(args), 1)...};
        return fill(fmt);
    }

    template<typename T>
    void StringFormat::collect(T arg)
    {
        std::stringstream ss;
        ss << arg;
        s_vector_args.push_back(ss.str());
    }
}

#endif // __STRING_FORMAT_H__

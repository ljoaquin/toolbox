#include <iostream>

#include "string_formater.h"

// template<typename T>
// void push_into_vector(std::vector<std::string>& vector_args, T arg)
// {
//     std::stringstream ss;
//     ss << arg;
//     vector_args.push_back(ss.str());
// }

// struct pass
// {
//     template<typename... Targs>
//     pass(Targs...){};
// };

// std::string fill(std::string fmt, const std::vector<std::string>& vector_args)
// {
//     for(int i = 0; i < vector_args.size(); i++)
//     {
//         std::stringstream ss;
//         ss << "{"} << i;
//         int index = fmt.find(ss.str());
//         if(index != -1)
//         {
//             fmt.replace(index, ss.str().length(), vector_args[i]);
//         }
//     }
//     return fmt;
// }

// template<typename... Targs>
// std::string format(std::string fmt, Targs... args)
// {
//     static std::vector<std::string> s_vector_args;
//     s_vector_args.clear();
//     pass{(push_into_vector(s_vector_args, args), 1)...};
//     return fill(fmt, s_vector_args);
// }

int main()
{
    
    std::cout << "hello\n";
    
    std::cout << "result:" << toolbox::string_formater::format("my name is lin jian, aged 25") << "\n";
    std::cout << "result:" << toolbox::string_formater::format("my name is lin jian, aged 25", "lj", 17) << "\n";
    std::cout << "result:" << toolbox::string_formater::format("my name is {0}, aged {1}") << "\n";
    std::cout << "result:" << toolbox::string_formater::format("my name is {0}, aged {1}", "lj", 17) << "\n";
    std::cout << "result:" << toolbox::string_formater::format("float {1}, char {2}, int {0}, char string {3}", 7, 3.45, 'q', "done!") << "\n";
    std::cout << "result:" << toolbox::string_formater::format("10th:{10}, 1st:{1}", "", "arg1") << "\n";
    
    return 0;
}
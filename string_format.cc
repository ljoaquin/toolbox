#include "string_format.h"

namespace toolbox
{

    std::vector<std::string> StringFormat::s_vector_args;

    std::string StringFormat::fill(std::string fmt)
    {
        for(int i = 0; i < s_vector_args.size(); i++)
        {
            std::stringstream ss;
            ss << "%" << i;
            int index = fmt.find(ss.str());
            if(index != -1)
            {
                fmt.replace(index, ss.str().length(), s_vector_args[i]);
            }
        }
        return fmt;
    }
}

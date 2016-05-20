#include <string>
#include "CLIOptions.h"

CLIOptions::CLIOptions()
: m_index(0)
{
}

CLIOptions::~CLIOptions()
{
}

void CLIOptions::AddOption(const char* name, int required, int *flag, char val)
{
   option opt = {  name, required, flag, val };
   m_options.push_back(opt);
}

void CLIOptions::SetArgs(int argc, char* argv[])
{
    m_argc = argc;
    m_argv = argv;
    m_index = 0;
}

bool CLIOptions::GetNextOption(int &val, char* &arg)
{
    std::string optString = BuildOptString();
    val = getopt_long(m_argc, m_argv, optString.c_str(), m_options.data(), &m_index);

    arg = optarg;

    return val != -1;
}

std::string CLIOptions::BuildOptString()
{
    std::string ret;
    for (size_t i=0;i<m_options.size();i++)
    {
        if (m_options[i].val != 0)
        {
            ret += (char)m_options[i].val;
            switch (m_options[i].has_arg)
            {
                case no_argument:
                    break;
                case required_argument:
                    ret += ":";
                    break;
                case optional_argument:
                    ret += "::";
                    break;
                default:
                    break;
            }
        }
    }
    return ret;
}

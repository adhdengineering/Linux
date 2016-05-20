#pragma once

#include <vector>

#include <getopt.h>

/// Yes, i know this class is total overkill, but it was more of an experiment to write
class CLIOptions
{
public:
    CLIOptions();
    virtual ~CLIOptions();

    void AddOption(const char* name, int required, int *flag, char val);
    void SetArgs(int argc, char *argv[]);
    bool GetNextOption(int &val, char* &arg);

protected:
    std::string BuildOptString();

protected:
    std::vector<option> m_options;
    int m_argc;
    char** m_argv;
    int m_index;
};

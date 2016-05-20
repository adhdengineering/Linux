#pragma once

#include <string>
#include <map>

struct ProjectReference
{
    std::string projectFile;
    std::string projectGUID;
    bool refOutputAssembly;
};

typedef std::map<std::string,std::string> Option;
typedef std::map<std::string,Option> ConfigurationOption;
typedef std::map<std::string,ConfigurationOption> FileOption;




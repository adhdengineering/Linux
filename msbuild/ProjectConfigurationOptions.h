#pragma once

#include <string>
#include "types.h"


class ProjectConfigurationOptions
{
public:
    ProjectConfigurationOptions();
    virtual ~ProjectConfigurationOptions();

    void SetGlobalOption(std::string key, std::string value);
    void SetConfigurationOption(std::string configuration, std::string key, std::string value);
    void SetFileOption(std::string filename, std::string configuration, std::string key, std::string value);
    //void SetCompileOption(std::string key, std::string value);
    void SetLinkOption(std::string configuration, std::string key, std::string value);

    Option GetCompileOptionsForConfiguration(std::string configuration);
    Option GetCompileOptionsForSourceFile(std::string configuration, std::string file);
    Option GetLinkOptionsForConfiguration(std::string configuration);

    void AddAllOptions(Option &src, Option &dst);
protected:
    Option m_globalOptions;
    ConfigurationOption m_configurationOptions;
    FileOption m_fileOptions;
    //Option m_compileOptions;
    ConfigurationOption m_linkOptions;
};


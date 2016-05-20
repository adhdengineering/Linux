#include "ProjectConfigurationOptions.h"

ProjectConfigurationOptions::ProjectConfigurationOptions()
{
}

ProjectConfigurationOptions::~ProjectConfigurationOptions()
{
}

void ProjectConfigurationOptions::SetGlobalOption(std::string key, std::string value)
{
    m_globalOptions[key] = value;
}

void ProjectConfigurationOptions::SetConfigurationOption(std::string configuration, std::string key, std::string value)
{
    if (configuration.length() == 0)
        SetGlobalOption(key,value);
    else
        m_configurationOptions[configuration][key] = value;
}

void ProjectConfigurationOptions::SetFileOption(std::string filename, std::string configuration, std::string key, std::string value)
{
    m_fileOptions[filename][configuration][key] = value;
}
/*
void ProjectConfigurationOptions::SetCompileOption(std::string key, std::string value)
{
    m_compileOptions[key] = value;
}*/

void ProjectConfigurationOptions::SetLinkOption(std::string configuration, std::string key, std::string value)
{
    m_linkOptions[configuration][key] = value;
}

Option ProjectConfigurationOptions::GetCompileOptionsForConfiguration(std::string configuration)
{
    Option ret;
    AddAllOptions(m_globalOptions, ret);
    Option configOptions = m_configurationOptions[configuration];
    AddAllOptions(configOptions, ret);

    return ret;
}

Option ProjectConfigurationOptions::GetCompileOptionsForSourceFile(std::string configuration, std::string file)
{
    Option ret;
    AddAllOptions(m_globalOptions, ret);
    Option configOptions = m_configurationOptions[configuration];
    AddAllOptions(configOptions, ret);
    Option fileOptions = m_fileOptions[file][configuration];
    AddAllOptions(fileOptions, ret);

    return ret;
}

Option ProjectConfigurationOptions::GetLinkOptionsForConfiguration(std::string configuration)
{
    return m_linkOptions[configuration];
}


void ProjectConfigurationOptions::AddAllOptions(Option &src, Option &dst)
{
    Option::iterator it;
    for(it = src.begin(); it != src.end(); it++)
    {
        dst[it->first] = it->second;
    }
}


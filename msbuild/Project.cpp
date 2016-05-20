#include "Project.h"

Project::Project()
{
    // TODO Auto-generated constructor stub

}

Project::~Project()
{
    // TODO Auto-generated destructor stub
}

void Project::SetProjectInfo(std::string dir, std::string file, std::string name)
{
    m_directory = dir;
    m_file = file;
    m_name = name;
}


bool Project::HasConfiguration(std::string configuration)
{
    for (size_t i =0;i<m_configurations.size();i++)
    {
        if (m_configurations[i] ==configuration)
            return true;
    }
    return false;
}

Option Project::GetCompileOptionsForConfiguration(std::string configuration)
{
    return m_projectConfigurationOptions.GetCompileOptionsForConfiguration(configuration);
}

Option Project::GetCompileOptionsForSourceFile(std::string configuration, std::string file)
{
    return m_projectConfigurationOptions.GetCompileOptionsForSourceFile(configuration,file);
}

Option Project::GetLinkOptionsForConfiguration(std::string configuration)
{
    return m_projectConfigurationOptions.GetLinkOptionsForConfiguration(configuration);
}

void Project::SetFileConfigurationOption(std::string file, std::string configuration, std::string key, std::string value)
{
    m_projectConfigurationOptions.SetFileOption(file, configuration, key, value);
}

void Project::SetConfigurationOption(std::string configuration, std::string key, std::string value)
{
    m_projectConfigurationOptions.SetConfigurationOption(configuration, key, value);
}

void Project::SetLinkOption(std::string configuration, std::string key, std::string value)
{
    m_projectConfigurationOptions.SetLinkOption(configuration, key, value);
}


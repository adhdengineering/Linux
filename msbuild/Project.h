#pragma once

#include <string>
#include <vector>

#include "ProjectConfigurationOptions.h"
#include "CustomBuildFile.h"
#include "types.h"

class Project
{
public:
    Project();
    virtual ~Project();

    /// used by the ProjectBuilder class
    std::vector<std::string> GetSourceFiles() { return m_sourceFiles; }
    std::vector<CustomBuildFile> GetCustomBuildFiles() { return m_customBuildFiles; }

    bool HasConfiguration(std::string configuration);
    Option GetCompileOptionsForConfiguration(std::string configuration);
    Option GetCompileOptionsForSourceFile(std::string configuration, std::string file);
    Option GetLinkOptionsForConfiguration(std::string configuration);

    void SetProjectInfo(std::string dir, std::string file, std::string name);

    void AddConfiguration(std::string configuration) { m_configurations.push_back(configuration); }
    void AddSourceFile(std::string src) { m_sourceFiles.push_back(src); }
    void AddHeaderFile(std::string header) { m_headerFiles.push_back(header); }
    void AddResourceFile(std::string resource) { m_resourceFiles.push_back(resource); }
    void AddCustomBuildFile(CustomBuildFile cbf) { m_customBuildFiles.push_back(cbf); }
    void AddReferenceProject(ProjectReference pr) { m_references.push_back(pr); }

    void SetFileConfigurationOption(std::string file, std::string configuration, std::string key, std::string value);
    void SetConfigurationOption(std::string configuration, std::string key, std::string value);
    void SetLinkOption(std::string configuration, std::string key, std::string value);

    void SetName(std::string name) { m_projectName = name; }
    void SetGUID(std::string GUID) { m_projectGUID = GUID; }
    std::string GetName() { return m_name; }
    std::string GetDirectory() { return m_directory; }
    std::string GetFile() { return m_file; }
protected:
    std::string m_name;
    std::string m_directory;
    std::string m_file;
//    std::string GUID;

    std::string m_projectName;
    std::string m_projectGUID;
    std::vector<std::string> m_configurations;

    ProjectConfigurationOptions m_projectConfigurationOptions;
    std::vector<ProjectReference> m_references;
    std::vector<std::string> m_sourceFiles;
    std::vector<std::string> m_headerFiles;
    std::vector<std::string> m_resourceFiles;
    std::vector<CustomBuildFile> m_customBuildFiles;
};


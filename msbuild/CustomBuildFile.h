#pragma once

#include <string>
#include <map>

class CustomBuildFile
{
public:
    CustomBuildFile();
    virtual ~CustomBuildFile();

    void SetFileName(std::string filename) { m_filename = filename; }
    void SetConfigurationCommandLine(std::string configuration, std::string cmdLine) { m_commandlinesMappedByConfiguration[configuration] = cmdLine; }
    void SetConfigurationOutput(std::string configuration, std::string output) { m_outputMappedByConfiguration[configuration] = output; }

    std::string GetFileName() { return m_filename; }
    std::string GetConfigurationCommandLine(std::string configuration) { return m_commandlinesMappedByConfiguration[configuration]; }
    std::string GetConfigurationOutput(std::string configuration) { return m_outputMappedByConfiguration[configuration]; }


protected:
    std::string m_filename;
    std::map<std::string, std::string> m_commandlinesMappedByConfiguration;
    std::map<std::string, std::string> m_outputMappedByConfiguration;
};


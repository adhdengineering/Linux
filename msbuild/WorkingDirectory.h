#pragma once

#include <string>

class WorkingDirectory
{
public:
    WorkingDirectory(std::string path);
    virtual ~WorkingDirectory();
    static std::string GetWorkingDirectory();
protected:
    std::string m_oldWorkingDirectory;
};


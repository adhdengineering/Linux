#include "WorkingDirectory.h"
#include <iostream>
#include <unistd.h>

WorkingDirectory::WorkingDirectory(std::string path)
{
    m_oldWorkingDirectory = GetWorkingDirectory();
    std::cout << "Changing directory to " << path << std::endl;
    chdir(path.c_str());
}

std::string WorkingDirectory::GetWorkingDirectory()
{
    char currentWD[2048] = {0};
    getcwd(currentWD, 2047);
    return std::string(currentWD);
}


WorkingDirectory::~WorkingDirectory()
{
    chdir(m_oldWorkingDirectory .c_str());
}


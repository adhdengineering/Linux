#pragma once

#include <string>
#include <vector>

class SolutionFile
{
public:
    SolutionFile();
    virtual ~SolutionFile();

    bool Parse(std::string filename);
    bool GetProject(unsigned int index, std::string &dir, std::string &file, std::string &name);
    size_t GetNumProjects() { return m_projects.size(); }

protected:

    bool IsSLNFile(std::string line);
    bool IsProject(std::string line);
    bool IsEndProject(std::string line);
    bool IsProjectSection(std::string line);
    bool IsEndProjectSection(std::string line);
    bool IsGlobal(std::string line);
    bool ReadProject(std::string line);

protected:
    struct PROJECT
    {
        std::string name;
        std::string directory;
        std::string file;
        std::string GUID;
    };

    std::vector<PROJECT> m_projects;

    enum eParseState
    {
        eInErrorCondition = -1,
        eLookForFormatVersion,
        eLookForProjectOrGlobal,
        eLookForEndProjectOrProjectSection,
        eLookForEndProjectSection,
        elookForGlobalSection,
    };
};


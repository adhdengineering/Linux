#include "SolutionFile.h"
#include <iostream>
#include <fstream>

#include <stdio.h>
#include "StringReader.h"
#include "StringHelper.h"

SolutionFile::SolutionFile()
{
}

SolutionFile::~SolutionFile()
{
}

bool SolutionFile::Parse(std::string filename)
{
    std::ifstream file;
    file.open(filename.c_str());

    if (!file.is_open())
    {
        std::cout << "Could not open solution file" << std::endl;
        return false;
    }

    unsigned char ch = 0;
    file >> ch;
    if( ch == 0xef)
    {
        // UTF8 Marker on some files.
        //should be 0xEF 0xBB 0xBF
        file >> ch;
        file >> ch;
    }
    else
    {
        file.seekg(0, file.beg);
    }
    eParseState state = eLookForFormatVersion;

    std::string line;

    while (std::getline(file, line))
    {
        StringHelper::trim(line);
        if (line.length() == 0) continue;
        if (line[0] == '#') continue;

        switch (state)
        {
            case eLookForFormatVersion:
                if (IsSLNFile(line)) state = eLookForProjectOrGlobal;
                else state = eInErrorCondition;
                break;
            case eLookForProjectOrGlobal:
                if (IsProject(line))
                {
                    if (ReadProject(line)) state = eLookForEndProjectOrProjectSection;
                    else state = eInErrorCondition;
                }
                else if (IsGlobal(line)) state = elookForGlobalSection;
                else state = eInErrorCondition;
                break;
            case eLookForEndProjectOrProjectSection:
                if (IsEndProject(line)) state = eLookForProjectOrGlobal;
                else if (IsProjectSection(line)) state = eLookForEndProjectSection;
                else state = eInErrorCondition;
                break;
            case eLookForEndProjectSection:
                if (IsEndProjectSection(line)) state = eLookForEndProjectOrProjectSection;
                break;
            case elookForGlobalSection:
                break;
            case eInErrorCondition:
                break;
        }
        if (state == eInErrorCondition) break;
    }

    file.close();

    return true;
}

bool SolutionFile::IsSLNFile(std::string line)
{
    std::string formatString = "Microsoft Visual Studio Solution File";
    return line.compare(0, formatString.length(), formatString) == 0;
}

bool SolutionFile::IsProject(std::string line)
{
    std::string projectString = "Project";
    return line.compare(0, projectString.length(), projectString) == 0;
}

bool SolutionFile::IsEndProject(std::string line)
{
    std::string endProjectString = "EndProject";
    return line.compare(0, endProjectString.length(), endProjectString) == 0;
}

bool SolutionFile::IsProjectSection(std::string line)
{
    std::string endProjectString = "ProjectSection";
    return line.compare(0, endProjectString.length(), endProjectString) == 0;
}

bool SolutionFile::IsEndProjectSection(std::string line)
{
    std::string endProjectString = "EndProjectSection";
    return line.compare(0, endProjectString.length(), endProjectString) == 0;
}

bool SolutionFile::IsGlobal(std::string line)
{
    std::string globalString = "Global";
    return line.compare(0, globalString.length(), globalString) == 0;
}

bool SolutionFile::ReadProject(std::string line)
{
    StringReader sr(line);
    std::string chuckAway;
    PROJECT p;

    chuckAway = sr.ReadUntilFindChar('\"');

    std::string slnGUID = sr.ReadUntilFindChar('\"');
    chuckAway = sr.ReadUntilFindChar('\"');

    p.name = sr.ReadUntilFindChar('\"');
    chuckAway = sr.ReadUntilFindChar('\"');

    std::string filePath = sr.ReadUntilFindChar('\"');
    StringHelper::subst(filePath,"\\","/");
    StringHelper::ExtraxtPathAndFilename(filePath, p.directory, p.file);
    chuckAway = sr.ReadUntilFindChar('\"');

    p.GUID = sr.ReadUntilFindChar('\"');

    m_projects.push_back(p);

    return true;
}

bool SolutionFile::GetProject(unsigned int index, std::string &dir, std::string &file, std::string &name)
{
    if (m_projects.size() <= index) return false;
    dir = m_projects[index].directory;
    file = m_projects[index].file;
    name = m_projects[index].name;
    return true;
}


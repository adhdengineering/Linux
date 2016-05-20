#pragma once
#include "Project.h"
#include <vector>
#include <map>
#include <string>

class Project;

class ProjectBuilder
{
public:
    ProjectBuilder(Project* project);
    virtual ~ProjectBuilder();

    void SetMacro(std::string macro, std::string value) { m_macros[macro] = value; }

    bool BuildProject(std::string configuration);

protected:
    bool CompileAllSourceFiles();

    bool CompileFile(std::string file, std::string objFile, Option options);
    bool BuildCustomFile(CustomBuildFile cbf);
    bool PerformLinkApplication();
    bool PerformLinkDynamicLibrary();
    bool PerformLinkStaticLibrary();

    std::vector<std::string> SplitOptionSringBySemiColonAndPrepend(std::string optionString,std::string toPrepend);

    std::string ExpandMacroString(std::string strWithMacros);

    bool FileIsNewer(std::string file, std::string compareTo);
    bool MakeDirectory(std::string path);

protected:
    Project *m_project;
    std::string m_configuration;
    std::vector<std::string> m_objFilesToLink;
    std::map<std::string,std::string> m_macros;


    std::string m_intDir;
    std::string m_outDir;
    bool m_forceBuild;
};


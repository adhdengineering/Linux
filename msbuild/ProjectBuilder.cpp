#include "ProjectBuilder.h"
#include "Forker.h"
#include "StringHelper.h"

#include <iostream>

#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

ProjectBuilder::ProjectBuilder(Project* project)
: m_project(project)
, m_intDir("$(ProjectDir)/$(Configuration)/")
, m_outDir("$(SolutionDir)/$(Platform/$(Configuration)/")
, m_forceBuild(false)
{
}

ProjectBuilder::~ProjectBuilder()
{
}

bool ProjectBuilder::BuildProject(std::string configuration)
{
    m_configuration = configuration;
    m_objFilesToLink.clear();

    bool bRet = true;

    Option options = m_project->GetCompileOptionsForConfiguration(m_configuration);

    std::string temp;
    temp = options["IntDir"];
    if (temp.length() > 0)
        m_intDir = temp;
    m_intDir = ExpandMacroString(m_intDir);
    DIR* dir = opendir(m_intDir.c_str());
    if (dir)
        closedir(dir);
    else
        MakeDirectory(m_intDir);

    SetMacro("IntDir", m_intDir);

    temp = options["OutDir"];
    if (temp.length() > 0)
        m_outDir = temp;
    m_outDir = ExpandMacroString(m_outDir);
    dir = opendir(m_outDir.c_str());
    if (dir)
        closedir(dir);
    else
        MakeDirectory(m_outDir);

    SetMacro("OutDir", m_outDir);

    do
    {
        bRet = CompileAllSourceFiles();
        if (bRet == false)
            break;

        std::vector<CustomBuildFile> customBuildFiles = m_project->GetCustomBuildFiles();
        for (size_t i=0;i<customBuildFiles.size();i++)
        {
            bRet = BuildCustomFile(customBuildFiles[i]);
            if (bRet == false)
                break;
        }
        if (bRet == false)
            break;

        if (options["ConfigurationType"] == std::string("Application"))
            bRet = PerformLinkApplication();
        else if (options["ConfigurationType"] == std::string("DynamicLibrary"))
            bRet = PerformLinkDynamicLibrary();
        else if (options["ConfigurationType"] == std::string("StaticLibrary"))
            bRet = PerformLinkStaticLibrary();
        else
        {
            std::cout << "Unknown output type \"" << options["ConfigurationType"] << "\" in project" << std::endl;
            bRet = false;
        }

    } while(0);
    m_configuration = "";
    return bRet;
}

bool ProjectBuilder::CompileAllSourceFiles()
{
    bool bRet = true;
    std::vector<std::string> sourceFiles = m_project->GetSourceFiles();

    for (size_t i=0;i<sourceFiles.size();i++)
    {
        std::string srcFile = sourceFiles[i];

        Option options = m_project->GetCompileOptionsForSourceFile(m_configuration, srcFile);

        if (options["ExcludedFromBuild"] == std::string("true"))
            continue;


        std::string objFilePath = srcFile;
        StringHelper::subst(objFilePath, ".cpp", ".o");
        std::string objDir,objFileName;
        StringHelper::ExtraxtPathAndFilename(objFilePath, objDir, objFileName);
        std::string objFile = m_intDir + objFileName;

        if (objFile == "Win32/Release-Linux/dispatchqueue_spool.o")
            int breakme =10;
        m_objFilesToLink.push_back(objFile);

        bool isSrcFileNewerThanObjFile =FileIsNewer( srcFile, objFile);
        bool isPrjFileNewerThanObjFile =FileIsNewer( m_project->GetFile(), objFile);
        bool compile = isSrcFileNewerThanObjFile || isPrjFileNewerThanObjFile || m_forceBuild;

        if (compile)
        {
            bRet = CompileFile(srcFile, objFile, options);
            if (bRet == false)
                break;
        }
    }
    return bRet;
}




std::string m_compiler = "g++";
std::string m_copmileSwitch = "-c";
std::string m_outputSwitch = "-o";

bool ProjectBuilder::CompileFile(std::string srcFile, std::string objFile, Option options)
{
    std::vector<std::string> args;

    args.push_back(m_compiler);

    //Get Additional include directories
    std::string includeDirs = options["AdditionalIncludeDirectories"];
    std::vector<std::string> includes = SplitOptionSringBySemiColonAndPrepend(includeDirs, "-I");//GetIncludeDirectories(options);
    if (includes.size() > 0)
    {
        args.insert( args.end(), includes.begin(), includes.end() );
    }

    //Get preprocessor defines
    std::string defines = options["PreprocessorDefinitions"];
    std::vector<std::string> preprocessorDefinitions = SplitOptionSringBySemiColonAndPrepend(defines,"-D");//GetPreprocessorDefinitions(options);
    if (preprocessorDefinitions.size() > 0)
    {
        args.insert( args.end(), preprocessorDefinitions.begin(), preprocessorDefinitions.end() );
    }

    args.push_back("-std=c++0x");
    args.push_back("-std=c++11");
    args.push_back("-fPIC");
    //args.push_back("-m32");

    args.push_back(m_copmileSwitch);
    args.push_back(srcFile);

    args.push_back(m_outputSwitch);
    args.push_back(objFile);
    Forker f;
    return f.DoFork(args);
}

bool ProjectBuilder::BuildCustomFile(CustomBuildFile cbf)
{
    bool bRet = true;
    std::string cmdLine = ExpandMacroString(cbf.GetConfigurationCommandLine(m_configuration));

    std::vector<std::string> cmds = StringHelper::split(cmdLine, "\n", false);
    for (size_t i=0;i<cmds.size();i++)
    {
        std::string cmd = StringHelper::trim(cmds[i]);
        std::vector<std::string> args = StringHelper::split(cmd, " ", false);

        Forker f;
        bRet = f.DoFork(args);
        if (bRet == false)
            break;
    }
    std::string output = ExpandMacroString(cbf.GetConfigurationOutput(m_configuration));
    if (output.length() > 0)
        m_objFilesToLink.push_back(output);
    return bRet;
}

bool ProjectBuilder::PerformLinkApplication()
{
    std::vector<std::string> args;

    args.push_back(m_compiler);

    for (size_t i=0;i<m_objFilesToLink.size();i++)
    {
        args.push_back(m_objFilesToLink[i]);
    }

    args.push_back(m_outputSwitch);
    args.push_back(m_project->GetName());
    Forker f;
    return f.DoFork(args);
}

bool ProjectBuilder::PerformLinkDynamicLibrary()
{
    std::vector<std::string> args;
    Option options = m_project->GetLinkOptionsForConfiguration(m_configuration);

    args.push_back(m_compiler);
    args.push_back("-shared");
    args.push_back("-fPIC");
    args.push_back("-Wl,--no-undefined");
    //args.push_back("-m32");

    for (size_t i=0;i<m_objFilesToLink.size();i++)
    {
        args.push_back(m_objFilesToLink[i]);
    }

    std::string additionalLibDirectories = options["AdditionalLibraryDirectories"];
    std::vector<std::string> libDirs = SplitOptionSringBySemiColonAndPrepend(additionalLibDirectories, "-L");//GetIncludeDirectories(options);
    if (libDirs.size() > 0)
    {
        args.insert( args.end(), libDirs.begin(), libDirs.end() );
    }

    std::string additionalLibs = options["AdditionalDependencies"];
    std::vector<std::string> libs = SplitOptionSringBySemiColonAndPrepend(additionalLibs, "-l");//GetIncludeDirectories(options);
    if (libs.size() > 0)
    {
        args.insert( args.end(), libs.begin(), libs.end() );
    }

    args.push_back(m_outputSwitch);
    args.push_back(m_outDir + std::string("lib") +m_project->GetName() + std::string(".so"));
    Forker f;
    return f.DoFork(args);
}

bool ProjectBuilder::PerformLinkStaticLibrary()
{
    std::vector<std::string> args;

    args.push_back("ar");
    args.push_back("rcs");
    args.push_back(m_project->GetName() + std::string(".a"));

    for (size_t i=0;i<m_objFilesToLink.size();i++)
    {
        args.push_back(m_objFilesToLink[i]);
    }

    Forker f;
    return f.DoFork(args);
}


std::vector<std::string> ProjectBuilder::SplitOptionSringBySemiColonAndPrepend(std::string optionString,std::string toPrepend)
{
    std::vector<std::string> ret;
    std::vector<std::string> options = StringHelper::split(optionString,";");
    for (size_t i=0;i<options.size();i++)
    {
        if (options[i][0] == '%') continue;
        if (options[i].length() == 0) continue;
        std::string temp = toPrepend + options[i];
        StringHelper::subst(temp,"\\","/");
        ret.push_back(ExpandMacroString(temp));
    }
    return ret;
}

std::string ProjectBuilder::ExpandMacroString(std::string strWithMacros)
{
    std::string ret;
    size_t startPos = 0;
    size_t pos = 0;
    while(1)
    {
        pos = strWithMacros.find('$',startPos);
        if (pos == std::string::npos)
        {
            ret += strWithMacros.substr(startPos, pos - startPos);
            break;
        }
        if (strWithMacros[pos+1] != '(')
        {
            std::cout << "malformed macro string \"" << strWithMacros << "\"" << std::endl;
            break;
        }
        ret += strWithMacros.substr(startPos, pos - startPos);
        startPos = pos + 2;
        pos = strWithMacros.find(')',startPos);
        if (pos == std::string::npos)
        {
            std::cout << "malformed macro string \"" << strWithMacros << "\"" << std::endl;
            break;
        }

        std::string macro = strWithMacros.substr(startPos, pos - startPos);
        if (m_macros.count( macro ))
        {
            ret += m_macros[macro];
        }
        else
        {
            std::cout << "found unknown macro string \"" << macro << "\"" << std::endl;
        }
        startPos = pos + 1;
    }

    StringHelper::subst(ret, "\\", "/" );
    StringHelper::subst(ret, "//", "/" );
    return ret;

    //m_macros.count( key )
}

bool ProjectBuilder::FileIsNewer(std::string file, std::string compareTo)
{
    struct stat fa, cta;         // create a file attribute structure

    if (stat(file.c_str(), &fa) == -1)
        return false;
    if (stat(compareTo.c_str(), &cta) == -1)
        return true;

    return fa.st_mtime > cta.st_mtime;
}

bool ProjectBuilder::MakeDirectory(std::string path)
{
    std::vector<std::string> args;

    args.push_back("mkdir");
    args.push_back("-p");
    args.push_back(path);
    Forker f;
    return f.DoFork(args);}

#include <iostream>

#include "CLIOptions.h"
#include "SolutionFile.h"
#include "ProjectFileParser.h"
#include "StringHelper.h"
#include "ProjectBuilder.h"
#include "WorkingDirectory.h"

#include "Forker.h"
#include <memory>

#define MAJOR 0
#define MINOR 1
#define REV 1

std::string g_configuration = "Release-Linux";
std::string g_platform = "Win32";

int PrintOptions();
int GetOptions( int argc, char* argv[]);

int main(int argc, char* argv[])
{
    /*std::vector<std::string> testArgs;
    testArgs.push_back("g++");
    testArgs.push_back("--help");
    Forker f;
    f.DoFork(testArgs);
    return 0;*/
    if ( argc == 1)
        return PrintOptions();
    if ( GetOptions(argc, argv) != 0 )
        return PrintOptions();

    std::string slnFilePath = argv[argc-1];

    //* slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atcore/atcore.vcxproj";
    //* slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atspooler/atspooler.vcxproj";
    //* slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atblkbx/atblkbx.vcxproj";
    //* slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atcl_bitflow/atcl_bitflow.vcxproj";
    //* slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atusb_libusb/atusb_libusb.vcxproj";
    //* slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atdevsimcam/atdevsimcam.vcxproj";
    //* slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atdevregcam/atdevregcam.vcxproj";
    //* slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atconf_regcam/atconf_regcam.vcxproj";
    //slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atmcd/atmcd.vcxproj";
    //* slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atutility/atutility.vcxproj";
    //* slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atusb_libusb10/atusb_libusb10.vcxproj";
    //slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atdevapogee/atdevapogee.vcxproj";
    //* slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atdevtemplate/atdevtemplate.vcxproj";
    //slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/../wrappergeneration/createwrappers/createwrappersSDK3.vcxproj";
    // *slnFilePath = "/home/chrisgreer/shared/Solis4.26/SDK3/vstudio/atcl_standard/atcl_standard.vcxproj";


    std::string slnDir,slnFile;
    StringHelper::ExtraxtPathAndFilename(slnFilePath, slnDir, slnFile);

    std::string ext = StringHelper::FileExtension(slnFile);
    std::vector< std::auto_ptr<Project> > projects;

    if (ext == ".sln")
    {
        SolutionFile sln;
        if (!sln.Parse(slnFilePath))
        {
            std::cout << "Could not parse solution file \"" << slnFilePath << "\"" << std::endl;
        }

        WorkingDirectory slnWorkingDirectory(slnDir);
        for(size_t i=0; i<sln.GetNumProjects(); i++)
        {
            std::string prjDir, prjFile, prjName;
            sln.GetProject(i,prjDir, prjFile, prjName);
            ProjectFileParser pfp;
            WorkingDirectory projectWorkingDirectory(prjDir);
            std::auto_ptr<Project> project = pfp.Parse(prjFile);
            if (project.get() == NULL)
            {
                std::cout << "could not parse project file \"" << prjDir << prjFile << "\"" << std::endl;
                continue;
            }
            Project *ptr = project.release();
            ptr->SetProjectInfo(prjDir, prjFile, prjName);
            projects.push_back(std::auto_ptr<Project>(ptr));
        }
    }
    else
    {
        std::string prjDir = slnDir;
        std::string prjFile = slnFile;
        ProjectFileParser pfp;
        WorkingDirectory projectWorkingDirectory(prjDir);
        std::auto_ptr<Project> project = pfp.Parse(prjFile);
        if (project.get() == NULL)
        {
            std::cout << "could not parse project file \"" << prjDir << prjFile << "\"" << std::endl;
            exit(-1);
        }
        Project *ptr = project.release();
        std::string name = StringHelper::GetFilenameWithoutExtension(prjFile);
        ptr->SetProjectInfo(prjDir, prjFile, name);
        projects.push_back(std::auto_ptr<Project>(ptr));

        slnDir = ".";
    }

    WorkingDirectory slnWorkingDirectory(slnDir);
    std::vector<std::string> succeeded;
    std::vector<std::string> failed;
    for (size_t i=0;i<projects.size();i++)
    {
        std::string configuration = g_configuration + std::string("|") + g_platform;

        Project *p = projects[i].get();
        if (!p->HasConfiguration(configuration))
        {
            std::cout << p->GetName() << " does not support configuration " << configuration << std::endl;
            continue;
        }
        ProjectBuilder builder(p);

        builder.SetMacro("SolutionDir", WorkingDirectory::GetWorkingDirectory() + std::string("/"));

        WorkingDirectory projectWorkingDirectory(p->GetDirectory());

        builder.SetMacro("ProjectDir", WorkingDirectory::GetWorkingDirectory() + std::string("/"));
        builder.SetMacro("ProjectName", p->GetName());
        builder.SetMacro("Platform", g_platform);
        builder.SetMacro("Configuration", g_configuration);

        std::cout << "Building " << p->GetFile() << std::endl;
        if (builder.BuildProject(configuration)==false)
            failed.push_back(p->GetName());
        else
            succeeded.push_back(p->GetName());
    }
    std::cout << "Succeeded " << succeeded.size() << std::endl;
    std::cout << "Failed " << failed.size() << std::endl;
    for (size_t i=0;i<failed.size();i++)
    {
        std::cout << failed[i] << std::endl;
    }
    return 0;
}



int PrintOptions()
{
	std::cout << "msbuild for linux v" << MAJOR << "." << MINOR << "." << REV << std::endl;
    std::cout << std::endl;

    std::cout << "Usage: msbuild [OPTION] SOLUTION_FILE" << std::endl;
    std::cout << std::endl;

    std::cout << "  -c, --configuration      : Sets the configuration to build" << std::endl;
    std::cout << "                               Defaults to \"" << g_configuration << "\"" << std::endl;
	std::cout << "  -p, --platform           : Sets the platform to build" << std::endl;
    std::cout << "                               Defaults to \"" << g_platform << "\"" << std::endl;
	std::cout << "  -h, --help               : Displays this message" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

	return 0;
}



int GetOptions( int argc, char* argv[])
{
    CLIOptions cliOpts;
    cliOpts.AddOption( "configuration", required_argument, 0, 'c' );
    cliOpts.AddOption( "platform",      required_argument,       0,  'p' );
    cliOpts.AddOption( "help",    no_argument, 0,  'h' );
    cliOpts.AddOption( 0, 0, 0, 0 );

    cliOpts.SetArgs(argc, argv);

    int val;
    char *option = NULL;
    while (cliOpts.GetNextOption(val,option))
    {
        switch (val)
        {
            case 'c':
                g_configuration = option;
                break;
            case 'p':
                g_platform = option;
                break;
            case 'h':
                return 1;
            default:
                std::cout << "Invalid Option " << val << std::endl << std::endl;
                return 2;
        }
    }
    return 0;
}


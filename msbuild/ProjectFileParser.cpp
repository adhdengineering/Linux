#include "ProjectFileParser.h"
#include "StringHelper.h"

#include "rapidxml/rapidxml_utils.hpp"

#include <iostream>

ProjectFileParser::ProjectFileParser()
: m_project(NULL)
{
}

ProjectFileParser::~ProjectFileParser()
{
}

std::auto_ptr<Project> ProjectFileParser::Parse(std::string file)
{
    std::auto_ptr<Project> ret;
    try
    {
        rapidxml::file<> xmlFile(file.c_str());
        rapidxml::xml_document<> doc;
        doc.parse<0>(xmlFile.data());
        rapidxml::xml_node<>* projNode = doc.first_node();
        if (projNode == NULL) return ret;
        if (std::string(projNode->name()) != "Project") return ret;

        ret.reset(new Project());
        m_project = ret.get();

        rapidxml::xml_node<>* node = projNode->first_node();
        while (node)
        {
            switch (GetNodeType(node))
            {
                case eItemGroup:
                    ReadItemGroup(node);
                    break;
                case ePropertyGroup:
                    ReadPropertyGroup(node);
                    break;
                case eImportGroup:
                    ReadImportGroup(node);
                    break;
                case eImport:
                    ReadImport(node);
                    break;
                case eItemDefinitionGroup:
                    ReadItemDefinitionGroup(node);
                    break;
                case eUnknown:
                    break;
                default:
                    // any node at this level should be handled
                    break;
            }
            node = node->next_sibling();
        }
    }
    catch (...)
    {
        ret.reset();
    }
    m_project = NULL;
    return ret;
}

bool ProjectFileParser::ReadItemGroup(rapidxml::xml_node<>* node)
{
    std::string label = GetAttributeValue(node,"Label");
    if (label == "ProjectConfigurations")
        return ReadProjectConfigurations(node);

    rapidxml::xml_node<>* child = node->first_node();
    while (child)
    {
        switch (GetNodeType(child))
        {
            case eClCompile:
                AddSourceFile(child);
                break;
            case eClInclude:
                AddIncludeFile(child);
                break;
            case eResourceCompile:
                AddResourceFile(child);
                break;
            case eCustomBuild:
                AddCustomBuildFile(child);
                break;
            case eNone:
                break;
            case eProjectReference:
                AddProjectReference(child);
                break;
            default:
                break;
        }
        child = child->next_sibling();
    }
    return true;
}

bool ProjectFileParser::ReadProjectConfigurations(rapidxml::xml_node<>* node)
{
    rapidxml::xml_node<>* child = node->first_node();
    while (child)
    {
        std::string nodeName = child->name();

        if (nodeName == "ProjectConfiguration")
            ReadProjectConfiguration(child);

        child = child->next_sibling();
    }
    return true;
}

bool ProjectFileParser::ReadProjectConfiguration(rapidxml::xml_node<>* node)
{
    std::string configName = GetAttributeValue(node,"Include");
    std::string configuration;
    std::string platform;

    rapidxml::xml_node<>* child = node->first_node();
    while (child)
    {
        std::string nodeName = child->name();

        if (nodeName == "Configuration")
            configuration = child->value();

        if (nodeName == "Platform")
            platform = child->value();

        m_project->AddConfiguration(configName);
        child = child->next_sibling();
    }
    return true;
}

void ProjectFileParser::AddSourceFile(rapidxml::xml_node<>* node)
{
    std::string src = GetAttributeValue(node,"Include");
    StringHelper::subst(src,"\\","/");
    m_project->AddSourceFile(src);
    rapidxml::xml_node<>* child = node->first_node();
    while (child)
    {
        std::string configuration = GetConfigurationFromNode(child);
        m_project->SetFileConfigurationOption(src, configuration, child->name(), child->value());
        child = child->next_sibling();
    }
}

void ProjectFileParser::AddIncludeFile(rapidxml::xml_node<>* node)
{
    std::string header = GetAttributeValue(node,"Include");
    m_project->AddHeaderFile(header);
}

void ProjectFileParser::AddResourceFile(rapidxml::xml_node<>* node)
{
    std::string resource = GetAttributeValue(node,"Include");
    m_project->AddResourceFile(resource);
}

void ProjectFileParser::AddCustomBuildFile(rapidxml::xml_node<>* node)
{
    std::string src = GetAttributeValue(node,"Include");
    StringHelper::subst(src,"\\","/");

    CustomBuildFile cbf;
    cbf.SetFileName(src);

    rapidxml::xml_node<>* child = node->first_node();
    while (child)
    {
        std::string configuration = GetConfigurationFromNode(child);
        std::string option = child->name();

        if (option == std::string("Command"))
            cbf.SetConfigurationCommandLine(configuration, child->value());

        if (option == std::string("Outputs"))
            cbf.SetConfigurationOutput(configuration, child->value());

        child = child->next_sibling();
    }
    m_project->AddCustomBuildFile(cbf);
}


void ProjectFileParser::AddProjectReference(rapidxml::xml_node<>* node)
{
    ProjectReference pr;

    pr.projectFile = GetAttributeValue(node,"Include");
    rapidxml::xml_node<>* child = node->first_node();
    while (child)
    {
        std::string nodeName = child->name();

        if (nodeName == "Project")
            pr.projectGUID = child->value();

        if (nodeName == "ReferenceOutputAssembly")
            pr.refOutputAssembly = std::string(child->value()) != "false";

        child = child->next_sibling();
    }
    m_project->AddReferenceProject(pr);
}

bool ProjectFileParser::ReadPropertyGroup(rapidxml::xml_node<>* node)
{
    std::string label = GetAttributeValue(node,"Label");

    if (label == "Globals")
        return ReadGlobalProperties(node);

    if (label == "Configuration")
        return ReadConfigurationProperties(node);

    std::string configuration = GetConfigurationFromNode(node);
    rapidxml::xml_node<>* child = node->first_node();
    while (child)
    {
        std::string nodeName = child->name();

        if (nodeName == "OutDir")
            ReadOutDir(child);
        else if (nodeName == "IntDir")
            ReadIntDir(child);
        else
            m_project->SetConfigurationOption(configuration, child->name(), child->value());

        child = child->next_sibling();
    }

    return true;
}

bool ProjectFileParser::ReadOutDir(rapidxml::xml_node<>* node)
{
    std::string configuration = GetConfigurationFromNode(node);

    m_project->SetConfigurationOption(configuration, "OutDir", node->value());

    return true;
}
bool ProjectFileParser::ReadIntDir(rapidxml::xml_node<>* node)
{
    std::string configuration = GetConfigurationFromNode(node);

    m_project->SetConfigurationOption(configuration, "IntDir", node->value());

    return true;
}

bool ProjectFileParser::ReadGlobalProperties(rapidxml::xml_node<>* node)
{
    rapidxml::xml_node<>* child = node->first_node();
    while (child)
    {
        std::string nodeName = child->name();

        if (nodeName == "ProjectName")
            m_project->SetName(child->value());

        if (nodeName == "ProjectGuid")
            m_project->SetGUID(child->value());

        child = child->next_sibling();
    }
    return true;
}

bool ProjectFileParser::ReadConfigurationProperties(rapidxml::xml_node<>* node)
{
    std::string configuration = GetConfigurationFromNode(node);

    rapidxml::xml_node<>* child = node->first_node();
    while (child)
    {
        m_project->SetConfigurationOption(configuration, child->name(), child->value());
        child = child->next_sibling();
    }

    return true;
}

bool ProjectFileParser::ReadImportGroup(rapidxml::xml_node<>* node)
{
    return true;
}

std::auto_ptr<ProjectConfigurationOptions> ProjectFileParser::ReadImport(rapidxml::xml_node<>* node)
{
    std::string project = GetAttributeValue(node,"Project");
    StringHelper::replace(project, '\\', '/');
    StringHelper::subst(project, "$(VCTargetsPath)", "." );

    return std::auto_ptr<ProjectConfigurationOptions>(NULL);
}

bool ProjectFileParser::ReadItemDefinitionGroup(rapidxml::xml_node<>* node)
{
    std::string configuration = GetConfigurationFromNode(node);
    rapidxml::xml_node<>* child = node->first_node();
    while (child)
    {
        std::string nodeName = child->name();

        if (nodeName == "ClCompile")
            ReadCompileOptions(child, configuration);
        else if (nodeName == "Link")
            ReadLinkOptions(child, configuration);

        child = child->next_sibling();
    }

    return true;
}

bool ProjectFileParser::ReadCompileOptions(rapidxml::xml_node<>* node, std::string configuration)
{
    rapidxml::xml_node<>* child = node->first_node();
    while (child)
    {
        m_project->SetConfigurationOption(configuration, child->name(), child->value());
        child = child->next_sibling();
    }
    return true;
}

bool ProjectFileParser::ReadLinkOptions(rapidxml::xml_node<>* node, std::string configuration)
{
    rapidxml::xml_node<>* child = node->first_node();
    while (child)
    {
        m_project->SetLinkOption(configuration, child->name(), child->value());
        child = child->next_sibling();
    }
    return true;
}

// this array must match the order of the node type enum or they will be heck to pay.
// gosh darn heck i say!
const std::string ProjectFileParser::nodeTypes[ProjectFileParser::eNodeTypesLast]
{
    "ItemGroup",
    "PropertyGroup",
    "ImportGroup",
    "Import",
    "ItemDefinitionGroup",
    "ProjectConfiguration",
    "ClCompile",
    "ClInclude",
    "ResourceCompile",
    "CustomBuild",
    "None",
    "ProjectReference",
};

ProjectFileParser::eNodeType ProjectFileParser::GetNodeType(rapidxml::xml_node<>* node)
{
    std::string name = node->name();

    for (int e=eNodeTypesFirst; e<eNodeTypesLast; e++)
    {
        if (nodeTypes[e] == name)
            return (eNodeType)e;
    }
    return eUnknown;
}

std::string ProjectFileParser::GetAttributeValue(rapidxml::xml_node<>* node, std::string attrName)
{
    rapidxml::xml_attribute<> *attr = node->first_attribute();
    while( attr )
    {
        std::string name = attr->name();
        if (attrName == name)
            return attr->value();
        attr = attr->next_attribute();
    }
    return std::string();
}

std::string ProjectFileParser::GetConfigurationFromNode(rapidxml::xml_node<>* node)
{
    std::string condition = GetAttributeValue(node,"Condition");
    if (condition.empty()) return std::string();

    std::vector<std::string> parts = StringHelper::split(condition,"==");
    if (parts.size() != 2) return std::string();
    std::string config = parts[1];

    StringHelper::trim(config, '\'');

    return config;
}

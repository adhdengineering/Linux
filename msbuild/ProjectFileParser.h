#pragma once

#include <string>
#include <vector>
#include <memory>
#include "rapidxml/rapidxml.hpp"

#include "Project.h"

class ProjectFileParser
{
public:
    ProjectFileParser();
    virtual ~ProjectFileParser();

    std::auto_ptr<Project> Parse(std::string file);


protected:
    enum eNodeType
    {
        eNodeTypesFirst = 0,

        // root level nodes.
        eItemGroup = eNodeTypesFirst,
        ePropertyGroup,
        eImportGroup,
        eImport,
        eItemDefinitionGroup,

        // Item group children
        eProjectConfiguration,
        eClCompile,
        eClInclude,
        eResourceCompile,
        eCustomBuild,
        eNone,
        eProjectReference,

        eUnknown,
        eNodeTypesLast = eUnknown,
    };
    static const std::string nodeTypes[eNodeTypesLast];

    bool IsProjectNode(rapidxml::xml_node<>* node);

    bool ReadItemGroup(rapidxml::xml_node<>* node);
    bool ReadPropertyGroup(rapidxml::xml_node<>* node);
    bool ReadImportGroup(rapidxml::xml_node<>* node);
    std::auto_ptr<ProjectConfigurationOptions> ReadImport(rapidxml::xml_node<>* node);
    bool ReadItemDefinitionGroup(rapidxml::xml_node<>* node);

    bool ReadProjectConfigurations(rapidxml::xml_node<>* node);
    bool ReadProjectConfiguration(rapidxml::xml_node<>* node);

    void AddSourceFile(rapidxml::xml_node<>* node);
    void AddIncludeFile(rapidxml::xml_node<>* node);
    void AddResourceFile(rapidxml::xml_node<>* node);
    void AddCustomBuildFile(rapidxml::xml_node<>* node);
    void AddProjectReference(rapidxml::xml_node<>* node);

    bool ReadOutDir(rapidxml::xml_node<>* node);
    bool ReadIntDir(rapidxml::xml_node<>* node);

    bool ReadCompileOptions(rapidxml::xml_node<>* node, std::string configuration);
    bool ReadLinkOptions(rapidxml::xml_node<>* node, std::string configuration);



    bool ReadGlobalProperties(rapidxml::xml_node<>* node);
    bool ReadConfigurationProperties(rapidxml::xml_node<>* node);

    eNodeType GetNodeType(rapidxml::xml_node<>* node);
    std::string GetAttributeValue(rapidxml::xml_node<>* node, std::string attrName);

    std::string GetConfigurationFromNode(rapidxml::xml_node<>* node);

protected:
    Project* m_project;
};


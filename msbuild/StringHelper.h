#pragma once

#include <string>
#include <vector>

class StringHelper
{
private:
    StringHelper(){}
public:
    static std::string& ltrim(std::string &s);
    static std::string& rtrim(std::string &s);
    static std::string& trim(std::string &s);
    static std::string& trim(std::string &s, char toTrim);
    static std::string& replace(std::string &s, char target, char replacement);
    static std::vector<std::string> split(const std::string& s, const std::string& delim, const bool keep_empty = true);
    static std::string& subst(std::string &s, std::string target, std::string replace);

    static void ExtraxtPathAndFilename(std::string full, std::string& path, std::string &file);
    static std::string FileExtension(std::string file);
    static std::string GetFilenameWithoutExtension(std::string file);

};



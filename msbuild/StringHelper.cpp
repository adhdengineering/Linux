#include "StringHelper.h"
#include "StringReader.h"

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

// trim from start
std::string& StringHelper::ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
std::string& StringHelper::rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
std::string& StringHelper::trim(std::string &s)
{
    return ltrim(rtrim(s));
}

// trim from both ends
std::string& StringHelper::trim(std::string &s, char toTrim)
{
    std::string::iterator it = s.begin();
    while(*it == toTrim)
    {
        s.erase(it);
        it = s.begin();
        if (it == s.end())
            break;
    }
    std::reverse_iterator<std::string::iterator> r = s.rbegin();
    while(*r == toTrim)
    {
        it = r.base();
        it--;
        s.erase(it);
        r = s.rbegin();
        if (r == s.rend())
            break;
    }

    return s;
}


std::string &StringHelper::replace(std::string &s, char target, char replacement)
{
    std::string::iterator it;
    for (it=s.begin();it != s.end(); ++it)
    {
        if (*it == target)
            *it = replacement;
    }
    return s;
}

std::vector<std::string> StringHelper::split(const std::string& s, const std::string& delim, const bool keep_empty/* = true*/)
{
    std::vector<std::string> result;
    if (delim.empty())
    {
        result.push_back(s);
        return result;
    }
    std::string::const_iterator substart = s.begin(), subend;
    while (true)
    {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        std::string temp(substart, subend);
        if (keep_empty || !temp.empty())
        {
            result.push_back(temp);
        }
        if (subend == s.end())
        {
            break;
        }
        substart = subend + delim.size();
    }
    return result;
}

std::string& StringHelper::subst(std::string &s, std::string target, std::string replace)
{
    size_t pos = s.find(target);
    while (pos != std::string::npos)
    {
        s.replace(pos,target.length(),replace);
        pos = s.find(target);
    }
    return s;
}

void StringHelper::ExtraxtPathAndFilename(std::string full, std::string& path, std::string &file)
{
    path.clear();
    file.clear();

    StringReader sr(full);
    while( !sr.IsEOS() )
    {
        // windows paths remember :)
        std::string temp = sr.ReadUntilFindChar('/',true);
        if (temp[temp.length() -1] == '/')
        {
            path += temp;
        }
        else
        {
            file = temp;
            break;
        }
    }
}

std::string StringHelper::FileExtension(std::string file)
{
    std::string ret;
    size_t pos = file.rfind('.');
    if (pos == std::string::npos)
        return ret;
    ret = file.substr(pos);
    return ret;
}

std::string StringHelper::GetFilenameWithoutExtension(std::string file)
{
    std::string ret = file;
    size_t pos = file.rfind('.');
    if (pos == std::string::npos)
        return ret;
    ret = file.substr(0, pos);
    return ret;
}



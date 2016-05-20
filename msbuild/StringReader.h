#pragma once

#include <string>

class StringReader
{
public:
    StringReader(std::string src);
    virtual ~StringReader();

    bool IsEOS() { return m_it == m_src.end(); }
    std::string ReadUntilFindChar(char ch, bool appendSearchChar=false);

protected:
    std::string m_src;
    std::string::iterator m_it;

};


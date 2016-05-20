#include "StringReader.h"

StringReader::StringReader(std::string src)
: m_src(src)
, m_it(m_src.begin())
{
}

StringReader::~StringReader()
{
}

std::string StringReader::ReadUntilFindChar(char ch, bool appendSearchChar/*=false*/)
{
    std::string ret;

    while (!IsEOS())
    {
        char temp = *m_it;
        m_it++;
        if (temp == ch)
        {
            if (appendSearchChar)
                ret += temp;
            break;
        }
        ret += temp;
    }

    return ret;
}



#pragma once

#include <vector>
#include <string>

class Forker
{
public:
    Forker();
    virtual ~Forker();

    int DoFork(std::vector<std::string> args);
};


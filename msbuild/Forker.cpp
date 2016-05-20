#include "Forker.h"

#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <memory.h>

#include <iostream>
#include <sstream>

Forker::Forker()
{
}

Forker::~Forker()
{
}

#include <unistd.h>
#include <stdio.h>


int Forker::DoFork(std::vector<std::string> args)
{
    pid_t cpid;

    int pipefd[2];
    pipe(pipefd);

    cpid = fork();
    if (cpid == -1)
    {
        return false;
    }

    bool bRet = false;
    if (cpid == 0)  // Code executed by child
    {
       while ((dup2(pipefd[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
       close(pipefd[1]);
       close(pipefd[0]);
       std::stringstream ss;
       char** argData = new char*[args.size() + 1];
       for (size_t i=0;i<args.size();i++)
       {
           argData[i] = const_cast<char*>(args[i].c_str());
           ss << args[i] << " ";
       }
       argData[args.size()] = NULL;
       ss << std::endl;
       std::cout << ss.str();
       fflush(stdout);
       execvp(argData[0], argData);
       exit(-1);
    }
    else            // parent process (msbuild)
    {
        close(pipefd[1]);
        char buffer[1024] = {0};

        while (read(pipefd[0],buffer,1023)>0)
        {
            std::cout << buffer;//<< std::endl;
            memset(buffer,0,1024);
        }
        int status;
        wait(&status);
        if (WIFEXITED(status))
        {
            bRet = WEXITSTATUS(status) == 0;
        }
    }
    return bRet;
}

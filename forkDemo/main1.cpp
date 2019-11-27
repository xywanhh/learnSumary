//
// Created by Administrator on 2019/10/9 0009.
//

// linux下
// windows下无fork函数

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
    printf("--beginning of program\n");

    int counter = 0;
    pid_t pid = fork();

    if (pid == 0)
    {
        // child process
        int i = 0;
        for (; i < 5; ++i)
        {
            printf("child process: counter=%d\n", ++counter);
        }
    }
    else if (pid > 0)
    {
        // parent process
        int j = 0;
        for(; j < 5; ++j)
        {
            printf("parent process: counter=%d\n", ++counter);
        }
    }
    else
    {
        // fork failed
        printf("fork() failed\n");
    }

    printf("--end of program--\n");

    return 0;
}


#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    
    ssize_t ret_in;
    char buffer[8192];
    for(int i=1; i<argc ; i++)
    {
       int fd=open(argv[i],O_RDONLY|O_EXCL);
        while((ret_in=read(fd,&buffer,8192))>0){
            write(STDOUT_FILENO,&buffer,(ssize_t)ret_in);    
        }    
        close(fd);
    }
     
    
        

    return 0;
}

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
    for(int i=1; i<argc ; i++)
    {
        int fd=open(argv[i],O_RDONLY);
        struct stat buf;
        fstat(fd,&buf);
        char* addr=mmap(NULL,buf.st_size,PROT_READ,MAP_PRIVATE|MAP_FILE,fd,0);
        //write(STDOUT_FILENO,addr,buf.st_size);    
        write(0,addr,buf.st_size);
        munmap(addr,buf.st_size);
        close(fd);
        
    }    

    return 0;
}

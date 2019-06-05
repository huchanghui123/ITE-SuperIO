#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<linux/ioctl.h>

#define IOC_MAGIC 'c'
#define IOC_COMMAND1 _IOW(IOC_MAGIC,0,int)
#define IOC_COMMAND2 _IOW(IOC_MAGIC,1,int)

char *command[] = {"hi","lo"};

int main(int argc, char *argv[])
{
    int cmd,fd,ret;
    
    if(argc>1)
    {
        if(strcmp(argv[1],command[0])==0)
            cmd = IOC_COMMAND1;
        else if(strcmp(argv[1],command[1])==0)
            cmd = IOC_COMMAND2;
    }
    
    fd = open("/dev/my_gpio",O_RDWR);
    if(fd < 0)
    {
        perror("open my_gpio fair!\n");
        exit(-1);
    }
    printf("open my_gpio success!\n");

    ret = ioctl(fd,cmd,0);
    if(ret == -1)
    {
        perror("ioctl error!\n");
        close(fd);
        exit(-1);
    }
    close(fd);
    return 0;
}




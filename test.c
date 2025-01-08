#include <numa.h>
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd = open("test.txt", O_RDWR);
    int res = write(fd,"new Text",8);
     if (close(fd) < 0) {
        perror("c1");
        exit(1);
    }
    printf("%d \n",res);
    return 0;
}
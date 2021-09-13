#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 512

char buffer[BUFFER_SIZE];
int buffer_size = 0;
int buffer_pos = 0;

int ReadTextLine(int fd, char str[], int max_len){
    int i = 0;
    int j = 0; 
    int ret = 0;

    // if current position is 0, reset buffer size and pos 
    if(lseek(fd, 0, SEEK_CUR) == 0)
        buffer_pos = buffer_size = 0;

    while(j < max_len - 1){ 

        if(buffer_pos == buffer_size){
            buffer[0] = 0;
            buffer_size = read(fd, buffer, BUFFER_SIZE); 
            buffer_pos = 0;
        }

        if(buffer_size == 0){ 
            if(j == 0)
                ret = EOF; 
            break;
        }
        while(j < max_len - 2 && buffer_pos < buffer_size){
            str[j++] = buffer[buffer_pos++];
            if(str[j - 1] == '\0' || str[j - 1] == 10){
                j--; 
                max_len = j; 
                break;
            }
        }
    }

    str[j] = 0; 
    
    return ret;
}

int main(){

    int fd = 0;
    int ret = 0;
    char name[64];
    int core = 0;
    long mem = 0;
    float ld_1 = 0, ld_5 = 0, ld_15 = 0;

    fd = open("/proc/cpuinfo", O_RDONLY);
    if(fd < 0){
        printf("ERROR:)\n");
        exit(0);
    }

    while(ret != EOF){
        ReadTextLine(fd, buffer, BUFFER_SIZE);

        if(strstr(buffer, "name") != NULL)
            sscanf(buffer, "model name  : %[^\n]s", name);

        if(strstr(buffer, "cores") != NULL){
            sscanf(buffer, "cpu cores   : %d", &core);
            break;
        }
    }

	printf("# of processor cores = %d\n", core);
	printf("CPU model = %s\n", name);

    close(fd);

    fd = open("/proc/meminfo", O_RDONLY);
    if(fd < 0){
        printf("ERROR:)\n");
        exit(0);
    }

    while(ret != EOF){
        ReadTextLine(fd, buffer, BUFFER_SIZE);

        if(strstr(buffer, "Total") != NULL){
            sscanf(buffer, "MemTotal:         %ld", &mem);
            break;
        }
    }

	printf("MemTotal = %ld\n", mem);

    close(fd);

    fd = open("/proc/loadavg", O_RDONLY);
    if(fd < 0){
        printf("ERROR:)\n");
        exit(0);
    }

    ReadTextLine(fd, buffer, BUFFER_SIZE);
    sscanf(buffer, "%f %f %f", &ld_1, &ld_5, &ld_15);

	printf("loadavg1 = %f, loadavg5 = %f, loadavg15 = %f\n", ld_1, ld_5, ld_15);

    close(fd);

}

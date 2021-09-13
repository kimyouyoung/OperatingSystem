#include <stdio.h>
#include <string.h>

#define MAX_CMD 2048
#define MAX_ARG 256
 
void ParseCommand(char *command, int *argc, char *argv[]){
    int b_count = 0;
    int s_count = 0;
    int p = 0;
    char temp[MAX_CMD];
	int index = 0;

    for(int i = 0; i < strlen(command); i++){
		if(command[i] == '\"'){
            b_count++;
            if(b_count == 2){
                b_count = 0;
				if(temp[p-1] != '\"'){
					index++;
                	temp[p++] = '\"';
				}
            }
        }else if(command[i] == '\''){
            s_count++;
            if(s_count == 2){
                s_count = 0;
				if(temp[p-1] != '\"'){
					index++;
                	temp[p++] = '\"';
				}
        	}
		}else if(b_count == 1 || s_count == 1){
            temp[p++] = command[i];
        }else{
            if(command[i] == ' ' || command[i] == '\t' || command[i] == 0){
                if(temp[p-1] != '\"'){
					index++;
					temp[p++] = '\"';
				}
            }else{
   				temp[p++] = command[i];
            }
        }
    }

	if(temp[p-1] != '\"'){
		index++;
		temp[p] = '\"';
	}
    
    char *ptr = strtok(temp, "\"");
    int j = 0;

    while(ptr != NULL){
		argv[j++] = ptr;
		ptr = strtok(NULL, "\"");
    }
    argv[index] = '\0';

    (*argc) = index;
 
}

int main(){
    char command[MAX_CMD];
    command[0] = command[MAX_CMD-1] = 0;
 
    int argc = 0;
    char *argv[MAX_ARG] = {NULL};
 
    while(1){
        printf("$ ");
        fgets(command, MAX_CMD - 1, stdin);
        command[strlen(command)-1] = 0;

        if(strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0)
            break;

        ParseCommand(command, &argc, argv);

        printf("argc = %d\n", argc);
        for(int i = 0; i < argc; i++)
            printf("argv[%d] = %s\n", i, argv[i]);

        printf("argv[%d] = %p\n", argc, argv[argc]);

    }

    printf("Bye!\n");

    return 0;
}
                      

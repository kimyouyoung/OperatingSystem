#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/ioctl.h>

int repeat_receiver = 1;
int terminal_rows = 0;

typedef struct {
    long type;	
	char message[512];		 
} Buffer;

Buffer global_buffer;

void *receiver(void *rcv){

    while(repeat_receiver == 1){

        // clear message buffer
        memset(global_buffer.message, 0x00, 512);

        // receive a message from rcv_queue into the message buffer
        if(msgrcv(*(int *)rcv, &global_buffer, sizeof(Buffer) - sizeof(long), 1, IPC_NOWAIT) != -1){
            // if the message buffer is not empty, print the message.
            if(global_buffer.message[0]){
                printf("%*s[incoming] \"%s\"\n", terminal_rows, " ", global_buffer.message);
                printf("[mesg] ");
                fflush(stdout);
                // break;
            }
            
        }

        usleep(1000);
    }
    pthread_exit(0);
}

void *sender(void *snd){

    int num = 0;

    while(1){

        printf("[mesg] ");
        fflush(stdout);
        
        // read a sentence from the user
        char message[512];
		message[511] = 0;

        fgets(message, 512, stdin);
		message[strlen(message) - 1] = 0;
        
        global_buffer.type = 1;
        strcpy(global_buffer.message, message);

        // send the message to snd_queue
        msgsnd(*(int *)snd, &global_buffer, sizeof(Buffer) - sizeof(long), 0);

        if(strcmp(message, "quit") == 0){
            repeat_receiver = 0;
			pthread_exit(0);
        }
        
    }

    return NULL;
    
}

int main(int argc, char *argv[])
{   
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    terminal_rows = w.ws_row;

	// parse the command line arguments to get the ids of the message queues
    if(argc < 3){
        fprintf(stderr, "Usage: a.out <snd_key> <rcv_key>\n");
        exit(0);
    }

    printf("snd_key = %d, rcv_key = %d\n", atoi(argv[1]), atoi(argv[2]));

    // create two message queues: one to send, and the other to receive messages.
    int snd_queue, rcv_queue;
    snd_queue = msgget((key_t)atoi(argv[1]), IPC_CREAT | 0666);
    rcv_queue = msgget((key_t)atoi(argv[2]), IPC_CREAT | 0666);

    pthread_t snd;
    pthread_t rcv;

    pthread_attr_t s_attr; 
    pthread_attr_t r_attr; 
    pthread_attr_init(&s_attr);
    pthread_attr_init(&r_attr);

    

    // launch a thread to run receiver() passing in &rcv_queue as argument.
    pthread_create(&rcv, &r_attr, receiver, &rcv_queue);

    // launch a thread to run sender() passing in &snd_queue as argument.
    pthread_create(&snd, &s_attr, sender, &snd_queue);


    // wait for the two child threads
    pthread_join(snd, NULL);
    pthread_join(rcv, NULL);


    // deallocate the two message queues
    msgctl(snd_queue, IPC_RMID, 0);
    msgctl(rcv_queue, IPC_RMID, 0);

	return 0;
}
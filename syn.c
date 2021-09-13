#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>


#define BUFFER_SIZE 5
#define MAX_MESSAGE 64
char buffer[BUFFER_SIZE][MAX_MESSAGE];
int in = 0, out = 0, count = 0;

pthread_mutex_t mutex;
sem_t empty, full;

int repeat = 1;

void DisplayBuffer();
void* Producer(void *param);
void* Consumer(void *param);

int main(int argc, char *argv[]){

    // Read duration from the command line arguments.
    int duration = 0;

    if(argc > 1)
		duration = atoi(argv[1]);
	if(duration == 0)				// for safety
		duration = 30;				// default value of duration

    // initialize
    srand(time(NULL));
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, BUFFER_SIZE, 0);

    // Display the initial content of buffer.
    DisplayBuffer();

    // Create threads fdor producer and consumer.
    pthread_t producer, consumer;
    pthread_create(&producer, NULL, Producer, NULL);
    pthread_create(&consumer, NULL, Consumer, NULL);

    // Wait for duration seconds.
    sleep(duration);

    // Set repeat to zero to terminate producer and Consumer.
    repeat = 0;

    // If the value of full is zero, call sem_post(&full);
    int f = 0;
    sem_getvalue(&full, &f);
    if(f == 0)
        sem_post(&full);
    
    // If the value of empty is zero, call sem_post(&empty);
    int e = 0;
    sem_getvalue(&empty, &e);
    if(e == 0)
        sem_post(&empty);

    // Wait for the producer and consumer.
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

    // Destroy mutex, full, and empty.
    pthread_mutex_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&empty);

    // Print a good-bye message.
    printf("Bye!\n");

    return 0 ;
}

void DisplayBuffer(){

    printf("Buffer contents:\n");
    printf("\tcount = %d, in = %d, out = %d\n", count, in, out);

    int p = out;
    for(int i = 0; i < count; i++){
        printf("\t%d] %s\n", p, buffer[p]);
        p = (p + 1) % BUFFER_SIZE;
    }
}

void* Producer(void *param){

    // Repeat until repeat is set to zero.
    while(repeat){

        // Randomly wait for 1~3 seconds.
        sleep(rand() % 3 + 1);

        // Generate a message by randomly choosing one of the following candidates.
        int no_messages = 10; 
        char *messages[64] = {
            "Nice to see you!",
            "Aal izz well!",
            "I love you! God loves you!",
            "God loves you and has a wonderful plan for your life.", "God bless you!",
            "You are genius!",
            "Cheer up!",
            "Everything is gonna be okay.",
            "You are so precious!",
            "Hakuna matata!"
        };
        
        int num = rand() % no_messages;

        // Print a message indicating the message was produced.
        printf("[Producer] Created a message \"%s\"\n", messages[num]);
        printf("---------------------- PRODUCER --------------------------\n");
        
        // Implement entry section using mutex lock and semaphores
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        // Print a message indicating the start of critical section.
        printf("Producer is entering critical section.\n");

        // Add the message to the buffer and increase counter.
        char nextProduced[MAX_MESSAGE];
        strcpy(nextProduced, messages[num]);
        strcpy(buffer[in], nextProduced);
        count++;
        printf("[Producer] \"%s\" ==> buffer\n", buffer[in]);
        in = (in + 1) % BUFFER_SIZE;

        // Display the content of buffer.
        DisplayBuffer();

        // Print a message indicating the end of critical section.
        printf("Producer is leaving critical section.\n");

        // Implement exit section using mutex lock and semaphores
        pthread_mutex_unlock(&mutex);
        sem_post(&full);

        printf("----------------------------------------------------------\n");
    }

    return NULL;
}

void* Consumer(void *param){
    
    
    // Repeat until repeat is set to zero.
    while(repeat){

        // Implement entry section using mutex lock and semaphores
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        // Print a message indicating the start of critical section.
        printf("----------------------- CONSUMER ------------------------- \n");
        printf("Consumer is entering critical section.\n");

        // Delete the message from the buffer and decrease counter.
        char nextConsumed[MAX_MESSAGE];
        strcpy(nextConsumed, buffer[out]);
        out = (out + 1) % BUFFER_SIZE;
        count--;

        // Print a message indicating a message was retrieved from the buffer.
        printf("[Consumer] buffer ==> \"%s\"\n", nextConsumed);

        // Display the content of buffer.
        DisplayBuffer();

        // Print a message indicating the end of critical section.
        printf("Consumer is leaving critical section.\n");

        // Implement exit section using mutex lock and semaphores
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        
        // Print a message indicating the message was consumed.
        printf("----------------------------------------------------------\n");
        printf("[Consumer] Consumed a message \"%s\"\n", nextConsumed);

        // Randomly wait for 2~4 seconds.
        sleep(rand() % 3 + 2);

    }

    return NULL;

}

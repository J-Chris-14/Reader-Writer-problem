#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

sem_t wrt;
sem_t rd;

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

int cnt = 2;
int numreader = 0;
int numwriter = 0;

void *writer(void *wno)
{   
    pthread_mutex_lock(&mutex2);
    numwriter++;
    if(numwriter == 1){
        sem_wait(&rd);      
    }
    pthread_mutex_unlock(&mutex2);
    sem_wait(&wrt); 

    cnt = cnt * 2;
    printf("Writer %d modified cnt to %d\n", *((int *)wno), cnt);

    sem_post(&wrt);
    pthread_mutex_lock(&mutex2);
    numwriter--;
    if(numwriter == 0){
        sem_post(&rd);
    }
    pthread_mutex_unlock(&mutex2); 
}

void *reader(void *rno)
{   
    sem_wait(&rd);
    pthread_mutex_lock(&mutex1);
    numreader++;
    if(numreader == 1){
        sem_wait(&wrt);
    }
    pthread_mutex_unlock(&mutex1);
    sem_post(&rd); 

    printf("Reader %d: read cnt as %d\n", *((int *)rno), cnt);

    pthread_mutex_lock(&mutex1);
    numreader--;
    if(numreader == 0){
        sem_post(&wrt);
    }
    pthread_mutex_unlock(&mutex1); 
}

int main()
{   
    pthread_t read[3], write[3];
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    sem_init(&wrt, 0, 1);
    sem_init(&rd, 0, 1);

    int a[3] = {1, 2, 3};

    for(int i = 0; i < 3; i++) {
        pthread_create(&write[i], NULL, (void *)writer, (void *)&a[i]);
    }

    for(int i = 0; i < 3; i++) {
        pthread_create(&read[i], NULL, (void *)reader, (void *)&a[i]);
    }

    for(int i = 0; i < 3; i++) {
        pthread_join(write[i], NULL);
    }

    for(int i = 0; i < 3; i++) {
        pthread_join(read[i], NULL);
    }

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    sem_destroy(&wrt);
    sem_destroy(&rd);

    return 0;
}

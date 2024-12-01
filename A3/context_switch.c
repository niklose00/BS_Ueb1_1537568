#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>

#define ITERATIONS 1000000

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

struct timespec start, end;
volatile int ready = 0; // Synchronisationsvariable

void *thread_func1(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&lock1); // Warte auf Zugriff
        pthread_mutex_unlock(&lock2); // Erlaube Thread 2, weiterzulaufen
    }
    return NULL;
}

void *thread_func2(void *arg) {
    clock_gettime(CLOCK_MONOTONIC, &start); // Zeitmessung starten
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&lock2); // Warte auf Zugriff
        pthread_mutex_unlock(&lock1); // Erlaube Thread 1, weiterzulaufen
    }
    clock_gettime(CLOCK_MONOTONIC, &end); // Zeitmessung beenden
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // Sperren initialisieren
    pthread_mutex_lock(&lock2); // Sperre für Thread 2 aktiv

    // Threads erstellen
    pthread_create(&thread1, NULL, thread_func1, NULL);
    pthread_create(&thread2, NULL, thread_func2, NULL);

    // Auf Threads warten
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Zeitdifferenz berechnen
    uint64_t total_time = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    double avg_context_switch_time = total_time / (double)(ITERATIONS * 2); // Zwei Wechsel pro Iteration

    printf("Durchschnittliche Dauer eines Kontextwechsels: %.2f ns\n", avg_context_switch_time);

    return 0;
}

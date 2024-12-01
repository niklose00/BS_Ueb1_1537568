#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>

#define ITERATIONS 1000000

// Funktion zur Berechnung der Zeitdifferenz in Nanosekunden
uint64_t diff_in_ns(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

int main() {
    struct timespec start, end;
    uint64_t min_latency = UINT64_MAX;

    // Erzeuge eine temporäre Datei für den Test
    int fd = open("/dev/null", O_RDONLY);
    if (fd == -1) {
        perror("Fehler beim Öffnen der Datei");
        return 1;
    }

    char buf[1]; // Ein Puffer mit nur 1 Byte für den Test

    for (int i = 0; i < ITERATIONS; i++) {
        // Zeit vor dem System-Call
        clock_gettime(CLOCK_MONOTONIC, &start);
        
        // System-Call: Lese 1 Byte von /dev/null
        read(fd, buf, 1);
        
        // Zeit nach dem System-Call
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        // Berechne Latenz
        uint64_t latency = diff_in_ns(start, end);

        // Aktualisiere minimale Latenz
        if (latency < min_latency) {
            min_latency = latency;
        }
    }

    // Datei schließen
    close(fd);

    printf("Minimale Latenz für den read-System-Call: %lu ns\n", min_latency);
    return 0;
}

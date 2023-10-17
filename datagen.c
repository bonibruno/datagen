// Required headers for the program
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// Constants for the program
#define NUM_THREADS 10
#define NUM_DIRS 10
#define NUM_FILES 100  
#define DEPTH 3

// File extensions for the generated files
char *extensions[] = {"txt", "csv", "json", "mp4", "doc", "pdf"};

// Mutex for thread synchronization and log file pointer
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
FILE *log_fp;

// Global variables to track directories and files
int global_dir_count = 1;
int global_file_count = 0;

// Function to generate files
void generate_files(const char *dir_path) {
    char filename[256];
    FILE *fp;

    // Loop to create files
    for (int i = 0; i < NUM_FILES; i++) {
        snprintf(filename, sizeof(filename), "%s/file%d.%s", dir_path, i, extensions[i % 6]);
        fp = fopen(filename, "w");
        fclose(fp);

        // Mutex to safely update global file counter
        pthread_mutex_lock(&mutex);
        global_file_count++;
        // Logging after each 100,000 files created, change this value based on your depth setting
        if (global_file_count % 100000 == 0) {
            fprintf(log_fp, "Files created so far: %d\n", global_file_count);
        }
        pthread_mutex_unlock(&mutex);
    }
}

// Function to generate directories and recursively call itself
void generate_directories(const char *dir_path, int depth) {
    if (depth == 0) {
        return;
    }

    char new_dir[256];

    // Loop to create directories
    for (int i = 0; i < NUM_DIRS; i++) {
        snprintf(new_dir, sizeof(new_dir), "%s/dir%d", dir_path, i);
        mkdir(new_dir, 0700);

        // Mutex to safely update global directory counter
        pthread_mutex_lock(&mutex);
        global_dir_count++;
        pthread_mutex_unlock(&mutex);

        generate_files(new_dir);
        generate_directories(new_dir, depth - 1);
    }
}

// Worker thread function, note I'm hardcoding the base directory name to manyfiles, change this accordingly.
void *worker_thread(void *arg) {
    char initial_dir[256];
    snprintf(initial_dir, sizeof(initial_dir), "manyfiles/thread%d", *(int *)arg);

    mkdir(initial_dir, 0700);

    // Mutex to safely update global directory counter
    pthread_mutex_lock(&mutex);
    global_dir_count++;
    pthread_mutex_unlock(&mutex);

    generate_directories(initial_dir, DEPTH);
    pthread_exit(NULL);
}

// Main function, note the log file gets generated in /opt/weka, change this to your log directory.
int main() {
    // Open the log file
    log_fp = fopen("/opt/weka/dirgen.log", "w");
    if (log_fp == NULL) {
        printf("Could not open log file\n");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[NUM_THREADS];
    int rc;
    int t_ids[NUM_THREADS];
    struct timeval start, end;
    long long elapsed_time;

    // Record the start time
    gettimeofday(&start, NULL);

    mkdir("manyfiles", 0700);
    generate_files("manyfiles");

    // Create worker threads
    for (int t = 0; t < NUM_THREADS; t++) {
        t_ids[t] = t;
        rc = pthread_create(&threads[t], NULL, worker_thread, (void *)&t_ids[t]);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Join worker threads
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    pthread_mutex_destroy(&mutex);

    // Record the end time
    gettimeofday(&end, NULL);
    elapsed_time = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));

    // Compute and print metrics
    double total_time = (double)elapsed_time / 1000000.0;
    double average_latency = (double)total_time / (global_dir_count + global_file_count);
    double average_throughput = (double)(global_dir_count + global_file_count) / total_time;

    printf("Total time taken: %f seconds\n", total_time);
    printf("Total directories created: %d\n", global_dir_count);
    printf("Total files created: %d\n", global_file_count);
    printf("Average latency: %f seconds\n", average_latency);
    printf("Average throughput: %f items/second\n", average_throughput);

    fclose(log_fp);

    return 0;
}

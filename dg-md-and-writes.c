// Include standard library headers for input-output, threading, etc.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// Define constants for the number of threads, directories, files, and directory depth
#define NUM_THREADS 10
#define NUM_DIRS 10
#define NUM_FILES 10  
#define DEPTH 3

// List of file extensions
char *extensions[] = {"txt", "csv", "json", "mp4", "doc", "pdf"};

// Initialize a mutex for thread-safe operations
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Global variables to keep track of the number of directories and files created
int global_dir_count = 1;  // Including the root directory
int global_file_count = 0;

// Function to generate files in a given directory
void generate_files(const char *dir_path) {
    char filename[256];
    FILE *fp;

    // Loop through and generate NUM_FILES files in the given directory
    for (int i = 0; i < NUM_FILES; i++) {
        snprintf(filename, sizeof(filename), "%s/file%d.%s", dir_path, i, extensions[i % 6]);
        fp = fopen(filename, "w");
        fprintf(fp, "This is file%d in directory %s.\n", i, dir_path);
        fclose(fp);

        // Lock mutex, update global file count, then unlock mutex
        pthread_mutex_lock(&mutex);
        global_file_count++;
        pthread_mutex_unlock(&mutex);
    }
}

// Function to generate directories and their sub-directories up to a certain depth
void generate_directories(const char *dir_path, int depth) {
    if (depth == 0) {
        return;
    }

    char new_dir[256];

    // Loop through and generate NUM_DIRS directories within the given directory
    for (int i = 0; i < NUM_DIRS; i++) {
        snprintf(new_dir, sizeof(new_dir), "%s/dir%d", dir_path, i);
        mkdir(new_dir, 0700);

        // Lock mutex, update global directory count, then unlock mutex
        pthread_mutex_lock(&mutex);
        global_dir_count++;
        pthread_mutex_unlock(&mutex);

        // Generate files and sub-directories within the new directory
        generate_files(new_dir);
        generate_directories(new_dir, depth - 1);
    }
}

// Thread function to create initial directories and generate subdirectories and files within
void *worker_thread(void *arg) {
    char initial_dir[256];
    snprintf(initial_dir, sizeof(initial_dir), "root/thread%d", *(int *)arg);

    mkdir(initial_dir, 0700);

    // Lock mutex, update global directory count, then unlock mutex
    pthread_mutex_lock(&mutex);
    global_dir_count++;
    pthread_mutex_unlock(&mutex);

    // Generate sub-directories and files within the initial directory
    generate_directories(initial_dir, DEPTH);
    pthread_exit(NULL);
}

// Main function
int main() {
    pthread_t threads[NUM_THREADS];
    int rc;
    int t_ids[NUM_THREADS];
    struct timeval start, end;
    long long elapsed_time;

    // Get the start time for performance metrics
    gettimeofday(&start, NULL);

    // Create root directory and generate files within
    mkdir("root", 0700);
    generate_files("root");

    // Create NUM_THREADS worker threads
    for (int t = 0; t < NUM_THREADS; t++) {
        t_ids[t] = t;
        rc = pthread_create(&threads[t], NULL, worker_thread, (void *)&t_ids[t]);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Wait for all threads to complete
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    // Destroy the mutex
    pthread_mutex_destroy(&mutex);

    // Calculate and display performance metrics
    gettimeofday(&end, NULL);
    elapsed_time = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    double total_time = (double)elapsed_time / 1000000.0;
    double average_latency = (double)total_time / (global_dir_count + global_file_count);
    double average_throughput = (double)(global_dir_count + global_file_count) / total_time;

    printf("Total time taken: %f seconds\n", total_time);
    printf("Total directories created: %d\n", global_dir_count);
    printf("Total files created: %d\n", global_file_count);
    printf("Average latency: %f seconds\n", average_latency);
    printf("Average throughput: %f items/second\n", average_throughput);

    return 0;
}

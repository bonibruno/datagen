DataGen: A Directory and File Generator in C

Overview

DataGen is a multi-threaded C program designed to generate a hierarchical structure of directories and files. 
It utilizes POSIX threads to concurrently build the file system structure, providing a quick way to populate a directory with sub-directories and files for testing purposes.

Features

    Multi-threaded directory and file creation
    Configurable number of threads, directories, files, and depth
    Log file generation to monitor progress
    Performance metrics output, including average client-side latency and throughput

Prerequisites

    GCC (or any C compiler)
    POSIX-compliant environment for pthreads
    Linux/Unix based OS (although it should be easily adaptable for Windows)

Installation

Clone the repository:

bash

git clone https://github.com/yourusername/datagen.git

Navigate into the directory:

bash

cd datagen

Compile the program:

bash

gcc -o datagen datagen.c -pthread

Usage

Run the compiled program:

bash

./datagen

By default, the program will generate directories and files under the "manyfiles" directory in the current working directory. 
The log file will be saved at /opt/weka/dirgen.log.

Configuration

The program contains several macros at the top of datagen.c that allow you to configure its behavior:

    NUM_THREADS: The number of worker threads
    NUM_DIRS: The number of directories in each directory
    NUM_FILES: The number of files in each directory
    DEPTH: The depth of the directory hierarchy

Code Structure

    generate_files: Function responsible for generating files in a given directory.
    generate_directories: Recursive function responsible for generating directories and delegating file creation to generate_files.
    worker_thread: Thread worker function that initializes the directory hierarchy for each thread.
    main: Entry point that sets up the environment and starts the worker threads.

For a detailed explanation of each function and code section, please refer to the comments in datagen.c.
Performance Metrics

The program outputs the following metrics:

    Total time taken for execution
    Total number of directories created
    Total number of files created
    Average latency
    Average throughput

Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
License

This project is licensed under the MIT License - see the LICENSE.md file for details.

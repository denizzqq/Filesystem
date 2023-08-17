# Filesystem

In this project, we implement a virtual file system that utilizes a data structure similar to INodes, as introduced in the lecture. This file system allows organized storage and retrieval of data, including reading and writing. All the data is stored in a common file on your computer. Note that the INodes used in this project differ in some details from the ones presented in the lecture, so carefully read the instructions provided in this assignment sheet.

## Overview

This project involves implementing a virtual file system with functionalities such as creating directories, creating files, listing contents, writing to files, reading from files, deleting files/directories, importing files, and exporting files.

## Getting Started

To run the program, you need to compile it using the provided `Makefile` and execute the resulting binary.

### Prerequisites

- GCC (GNU Compiler Collection)
- Make

### Compilation

Navigate to the project directory and execute the following commands:

```bash
make
```

### Execution

To create a new filesystem:
```bash
./build/ha2 -c <FILESYSTEM_NAME> <FS_SIZE>
```

To open an existing filesystem:

```bash
./build/ha2 -l <FILESYSTEM_NAME>
```

## Usage

Once the file system is created or loaded, you can use the provided shell to execute various commands.

Available commands:

- `mkdir <Path>`: Create a new directory.
- `mkfile <Path>`: Create a new file.
- `list <Absolute Path>`: List contents of a directory.
- `writef <Path> <Text>`: Write text to a file.
- `readf <Path>`: Read the content of a file.
- `rm <Path>`: Delete a file or directory recursively.
- `import <Internal Path> <External Path>`: Import a file into the filesystem.
- `export <Internal Path> <External Path>`: Export a file from the filesystem.
- `dump`: Save the filesystem to disk.

## Implemented Functions

- `fs_mkdir`: Create a new directory.
- `fs_mkfile`: Create a new file.
- `fs_list`: List contents of a directory.
- `fs_writef`: Write text to a file.
- `fs_readf`: Read the content of a file.
- `fs_rm`: Delete a file or directory recursively.
- `fs_import`: Import a file into the filesystem.
- `fs_export`: Export a file from the filesystem.

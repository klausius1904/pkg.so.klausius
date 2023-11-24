#include <fcntl.h>
#include "messagetype.h"
#include "systemcalls.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dynamicprog.h"
#include <sys/stat.h>


DIR *open_dir(char *directory_path) {
    DIR *to_return;

    if ((to_return = opendir(directory_path)) == NULL) {
        perror("EROARE LA DESCHIDEREA DIRECTORULUI\n");
        exit(EXIT_FAILURE);
    }
    return to_return;
}

void close_dir(DIR *directory_toclose) {
    if (closedir(directory_toclose) == -1) {
        perror("EROARE LA INCHIDEREA DIRECTORULUI\n");
        exit(EXIT_FAILURE);
    }
}

char **get_files_from_directory(DIR *working_directory, int *names_size) {
    struct dirent *directory_pointer;
    char **file_names;
    *names_size = 30;
    file_names = allocate_string_array(*names_size);
    uint8_t current_index = 0;

    while ((directory_pointer = readdir(working_directory)) != NULL) {
        if (current_index == *names_size) {
            *names_size += 30;
            file_names = reallocate_string_array(file_names, *names_size);
        }
        file_names[current_index] = allocate_string(strlen(directory_pointer->d_name));
        strcpy(file_names[current_index++], directory_pointer->d_name);
    }

    file_names[current_index] = NULL;
    file_names = reallocate_string_array(file_names, current_index);
    *names_size = current_index;

    return file_names;
}

int *generate_filedescriptors(char **file_names, int size) {

    int *filedescriptors_array = allocate_filedescriptors_array(size);
    int filedescriptors_index = 0;

    while (*file_names != NULL) {
        if ((filedescriptors_array[filedescriptors_index++] = open(*file_names, O_RDONLY)) == -1) {
            perror("EROARE LA DESCHIDEREA FISIERULUI\n");
            exit(EXIT_FAILURE);
        }
        file_names++;
    }
    return filedescriptors_array;
}

struct stat *generate_filestats(char **absolute_filepaths, int size) {

    int index = 0;
    struct stat *filestats = allocate_filestats(size);
    while (absolute_filepaths[index] != NULL) {
        if (lstat(absolute_filepaths[index], &filestats[index]) == -1) {

            perror("EROARE LA DESCHIDEREA STATISTICII");
            exit(EXIT_FAILURE);
        }
        index++;
    }

    return filestats;
}

char **process_filestats(struct stat *file_stats, char **file_names, int *file_descriptors, int size, char *absolute_path) {

    char **messages = allocate_string_array(size);

    for (int i = 0; i < size; i++) {
        messages[i] = process_filestat(file_stats[i], file_descriptors[i], file_names[i], absolute_path);
    }
    messages[size] = NULL;
    return messages;
}

char *process_filestat(struct stat file_stat, int file_descriptor, char *file_name, char *absolute_path) {
    char *message = allocate_string(1000);

    if (S_ISREG(file_stat.st_mode)) {
        if (strcmp(".bmp", file_name + (strlen(file_name) - 4)) == 0) {
            message = make_bmp_message(file_stat, file_descriptor, file_name);
        } else {
            message = make_fil_message(file_stat, file_descriptor, file_name);
        }
    } else if (S_ISDIR(file_stat.st_mode)) {
        message = make_dir_message(file_stat, file_descriptor, file_name);
    } else if (S_ISLNK(file_stat.st_mode)) {
        message = make_link_message(file_stat, file_descriptor, file_name, absolute_path);
    } else {
        /* do nothing */
    }

    return message;
}

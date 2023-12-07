//
// Created by Grozav Klaus on 07.12.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "system.h"
#include "message.h"


char *process_filestat(struct stat file_stat, char *file_name, int file_descriptor, char *out_dir) {
    //chdir(out_dir);
    char status_filename[100];
    char message[1000];

    strcpy(status_filename, file_name);
    strcat(status_filename, "_status.txt");
    if (S_ISREG(file_stat.st_mode)) {
        if (strcmp(".bmp", file_name + (strlen(file_name) - 4)) == 0) {
            strcpy(message,make_bmp_message(file_stat, file_descriptor, file_name));
        } else {
            strcpy(message,make_fil_message(file_stat, file_descriptor, file_name));
        }
    } else if (S_ISDIR(file_stat.st_mode)) {
        strcpy(message,make_dir_message(file_stat, file_descriptor, file_name));
    } else if (S_ISLNK(file_stat.st_mode)) {
        strcpy(message,make_link_message(file_stat, file_descriptor, file_name));
    } else {
        /* do nothing */
    }
    printf("%s\n", message);

    return NULL;
}
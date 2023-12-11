//
// Created by Grozav Klaus on 07.12.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "system.h"
#include "message.h"


int process_filestat(struct stat file_stat, char *file_name, int file_descriptor, char *out_dir, int *pipe1, int *pipe2) {

    char status_filename[100];
    char message[1000];
    int no_lines = 0;
    int fout;

    strcpy(status_filename, file_name);
    strcat(status_filename, "_status.txt");

    if (S_ISREG(file_stat.st_mode)) {
        if (strcmp(".bmp", file_name + (strlen(file_name) - 4)) == 0) {
            strcpy(message,make_bmp_message(file_stat, file_descriptor, file_name));
            no_lines = 10;
        } else {
            strcpy(message,make_fil_message(file_stat, file_descriptor, file_name));
            close(pipe1[0]);
            close(pipe2[1]);
            close(pipe2[0]);

            char data_file[1024];
            lseek(file_descriptor, 0, SEEK_SET);
            read(file_descriptor, data_file, sizeof(data_file));
            write(pipe1[1], data_file, strlen(data_file)+1);

            close(pipe1[1]);
            no_lines = 8;
        }
    } else if (S_ISDIR(file_stat.st_mode)) {
        strcpy(message,make_dir_message(file_stat, file_descriptor, file_name));
        no_lines = 8;
    } else if (S_ISLNK(file_stat.st_mode)) {
        strcpy(message,make_link_message(file_stat, file_descriptor, file_name));
        no_lines = 6;
    } else {
        /* do nothing */
    }
    close(file_descriptor);

    chdir(out_dir);
    if ((fout = creat(status_filename, S_IRUSR | S_IWUSR)) == -1)
    {
        perror("EROARE LA INCHIDEREA FISIERULUI\n");
        exit(EXIT_FAILURE);
    }

    write(fout, message, strlen(message));
    close(fout);

    return no_lines;
}
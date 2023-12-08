#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include "errors.h"
#include "system.h"

#define MAX_ENTRIES 100

int main(int argc, char *argv[]) {
    int index = 0;
    int file_descriptors[MAX_ENTRIES];
    int no_lines;
    int pids[MAX_ENTRIES];

    DIR *input, *output;
    struct dirent *in_dir;
    struct dirent *files_entries[MAX_ENTRIES];

    struct stat file_stat;

    verify_arguments(argc);

    if((input = opendir(argv[1])) == NULL) {
        perror("EROARE LA DESCHIDEREA FISIERULUI INTRARE");
        exit(-1);
    }

    if((output = opendir(argv[2])) == NULL) {
        perror("EROARE LA DESCHIDEREA FISIERULUI IESIRE");
        exit(-1);
    }

    chdir(argv[1]);

    while((in_dir = readdir(input)) != NULL) {
        files_entries[index++] = in_dir;
    }

    for(int i = 2 ; i < index ; i++){
        if((file_descriptors[i] = open(files_entries[i]->d_name, O_RDONLY)) == -1) {
            perror("EROARE LA DESCHIDEREA FISIERULUI ");
            exit(EXIT_FAILURE);
        }
    }



    for(int i = 2 ; i < index ; i++){


        if((pids[i] = fork()) < 0) {
            perror("Eroare la crearea procesului");
            exit(-1);
        }

        if(pids[i] == 0) {
            if ((lstat(files_entries[i]->d_name, &file_stat)) == -1) {
                perror("EROARE LA DESCHIDEREA STATISTICII");
                exit(-1);
            }
            no_lines = process_filestat(file_stat, files_entries[i]->d_name, file_descriptors[i], argv[2]);
            printf("proces %d: Numar scrieri fisier %s = %d\n\n", getpid(), files_entries[i]->d_name, no_lines);
            exit(0);
        }

    }

    for(int j = 2; j < index; j++) {
        int status;
        if ( waitpid(pids[j], &status, 0) == -1 ) {
            perror("FAIL TO WAITPID");
            return EXIT_FAILURE;
        }
        if ( WIFEXITED(status) ) {
            const int exit_status = WEXITSTATUS(status);
        }


    }

    if(closedir(input) == -1) {
        perror("EROARE LA INCHIDEREA DIRECTORULUI\n");
        exit(-1);
    }
    if(closedir(output) == -1) {
        perror("EROARE LA INCHIDEREA DIRECTORULUI\n");
        exit(-1);
    }

    return 0;
}

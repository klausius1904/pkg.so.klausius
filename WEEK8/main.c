#include <stdio.h>
#include "systemcalls.h"
#include "messagetype.h"
#include <dirent.h>
#include <stdlib.h>
#include "dynamicprog.h"
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    DIR *working_directory = NULL;
    char **file_names = NULL;
    char **absolute_filepath = NULL;
    char *absolute_outpath = NULL;
    int names_size = 0;
    int i = 0;
    int *file_descriptors = NULL;
    struct stat *file_stats = NULL;
    char **messages = NULL;

    int fout;

    check_args(argc);

    working_directory = open_dir(argv[1]);

    file_names = get_files_from_directory(working_directory, &names_size);

    absolute_filepath = get_absolute_path(file_names, argv[1], names_size);

    file_descriptors = generate_filedescriptors(absolute_filepath, names_size);

    file_stats = generate_filestats(absolute_filepath, names_size);

    //messages = process_filestats(file_stats, file_names, file_descriptors, names_size, argv[1]);

    absolute_outpath = make_absolute_path_string("statistica.txt", argv[2]);

    if ((fout = creat(absolute_outpath, S_IRUSR | S_IWUSR)) == -1)
    {
        perror("EROARE LA INCHIDEREA FISIERULUI\n");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", absolute_outpath);
    int pids[1000];
    for(i = 0; i<names_size; i++){
        if( ( pids[i]=fork() ) < 0)
        {
            perror("Eroare");
            exit(1);
        }
        printf("%d\n", pids[i]);
        if(pids[i]==0)
        {
            messages[i] = process_filestat(file_stats[i], file_descriptors[i], file_names[i], argv[1]);
            printf("%s", messages[i]);
            write(fout, messages[i], strlen(messages[i]));
            exit(0);
        }
    }
    for(i = 0; i<names_size; i++){
        int status;
        if ( waitpid(pids[i], &status, 0) == -1 ) {
            perror("waitpid failed");
            return EXIT_FAILURE;
        }
    }
//    for(int i = 2; i<names_size; i++){
//        write(fout, messages[i], strlen(messages[i]));
//    }


    printf("\n");
    free(file_descriptors);
    free(file_stats);
    free_string_array(file_names);
    free_string_array(messages);
    free_string_array(absolute_filepath);
    close_dir(working_directory);
    return 0;
}

#include "messagetype.h"
#include "dynamicprog.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

char *fil_message(const DATA_Header *header);

char *dir_message(const DATA_Header *header);

char *link_message(const DATA_Header *header, uint32_t original_size);

void check_args(int arguments_number) {
    if (arguments_number != 3) {
        perror("EROARE: NUMAR INVALID DE ARGUMENTE");
        exit(EXIT_FAILURE);
    }
}

char **get_absolute_path(char **file_names, char *absolute_path, int file_names_size) {
    char **absolute_path_filenames = allocate_string_array(file_names_size);
    int index = 0;
    int files_index = 0;

    while (file_names[files_index] != NULL) {
        absolute_path_filenames[index++] = make_absolute_path_string(file_names[files_index++], absolute_path);
    }
    absolute_path_filenames[index] = NULL;
    reallocate_string_array(absolute_path_filenames, index);
    return absolute_path_filenames;
}

char *make_absolute_path_string(char *filename, char *absolute_path) {
    char *absolute_path_string = allocate_string(100);

    strcpy(absolute_path_string, absolute_path);
    strcat(absolute_path_string, "/");
    strcat(absolute_path_string, filename);

    return reallocate_string(absolute_path_string, strlen(absolute_path_string) + 1);
}

char *bmp_message(DATA_Header *header) {
    char *bmp_string = allocate_string(1000);
    int length = sprintf(bmp_string,
                         "nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %d\nidentificatorul utilizatorului: %s\ntimpul ultimei modificari: %s\ncontorul de legaturi: %d\ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces other: %c%c%c\n",
                         header->file_name, header->height, header->width, header->size,
                         header->user_id, header->last_modify, header->link_nr,
                         header->user_acces[0], header->user_acces[1], header->user_acces[2],
                         header->group_acces[0], header->group_acces[1], header->group_acces[2],
                         header->other_acces[1], header->other_acces[2], header->other_acces[2]
    );
    return reallocate_string(bmp_string, length + 1);
}

char *fil_message(const DATA_Header *header) {
    char *fil_string = allocate_string(1000);
    int length = sprintf(fil_string,
                         "nume fisier: %s\ndimensiune: %d\nidentificatorul utilizatorului: %s\ntimpul ultimei modificari: %s\ncontorul de legaturi: %d\ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces other: %c%c%c\n",
                         header->file_name, header->size,
                         header->user_id, header->last_modify, header->link_nr,
                         header->user_acces[0], header->user_acces[1], header->user_acces[2],
                         header->group_acces[0], header->group_acces[1], header->group_acces[2],
                         header->other_acces[1], header->other_acces[2], header->other_acces[2]
    );
    return reallocate_string(fil_string, length + 1);
}

char *dir_message(const DATA_Header *header) {
    char *dir_string = allocate_string(1000);
    int length = sprintf(dir_string,
                         "nume fisier: %s\nidentificatorul utilizatorului: %s\ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces other: %c%c%c\n",
                         header->file_name,
                         header->user_id,
                         header->user_acces[0], header->user_acces[1], header->user_acces[2],
                         header->group_acces[0], header->group_acces[1], header->group_acces[2],
                         header->other_acces[1], header->other_acces[2], header->other_acces[2]
    );

    return reallocate_string(dir_string, length + 1);
}

char *link_message(const DATA_Header *header, uint32_t original_size) {
    char *link_string = allocate_string(1000);
    int length = sprintf(link_string,
                         "nume fisier: %s\ndimensiune: %d\ndimensiune fisier:%d \ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces other: %c%c%c\n",
                         header->file_name, header->size, original_size,
                         header->user_acces[0], header->user_acces[1], header->user_acces[2],
                         header->group_acces[0], header->group_acces[1], header->group_acces[2],
                         header->other_acces[1], header->other_acces[2], header->other_acces[2]
    );


    return reallocate_string(link_string, length + 1);
}

char *make_bmp_message(struct stat file_stat, int file_descriptor, char *file_name) {
    char *bmp_string = NULL;
    DATA_Header *header = malloc(sizeof(DATA_Header));
    header->file_name = malloc(strlen(file_name));

    strcpy(header->file_name, file_name);

    get_STATdata(header, file_stat);
    make_DATAHeader(header, file_descriptor);

    bmp_string = bmp_message(header);

    free(header);
    return bmp_string;
}

char *make_fil_message(struct stat file_stat, int file_descriptor, char *file_name) {
    char *fil_string = NULL;
    DATA_Header *header = malloc(sizeof(DATA_Header));
    header->file_name = malloc(strlen(file_name));

    strcpy(header->file_name, file_name);

    get_STATdata(header, file_stat);
    make_DATAHeader(header, file_descriptor);

    fil_string = fil_message(header);


    return fil_string;
}

char *make_dir_message(struct stat file_stat, int file_descriptor, char *file_name) {
    char *dir_string = NULL;
    DATA_Header *header = malloc(sizeof(DATA_Header));
    header->file_name = malloc(strlen(file_name));

    strcpy(header->file_name, file_name);
    get_STATdata(header, file_stat);
    make_DATAHeader(header, file_descriptor);
    dir_string = dir_message(header);

    free(header);
    return dir_string;
}

char *make_link_message(struct stat file_stat, int file_descriptor, char *file_name, char *absolute_path) {
    char *link_string = NULL;
    DATA_Header *header = malloc(sizeof(DATA_Header));
    header->file_name = malloc(strlen(file_name));

    strcpy(header->file_name, file_name);
    get_STATdata(header, file_stat);
    make_DATAHeader(header, file_descriptor);

    char buf[1024];
    char *absolute_link_file_path = make_absolute_path_string(file_name, absolute_path);
    char *r = realpath(absolute_link_file_path, buf);
    struct stat targetStat;
    stat(r, &targetStat);
    DATA_Header *original = malloc(sizeof(DATA_Header));
    get_STATdata(original, targetStat);

    link_string = link_message(header, original->size);

    free(header->file_name);
    free(absolute_link_file_path);
    free(header);
    free(original);
    return link_string;
}

DATE timeTConverter(time_t date) {
    struct tm *giverTime;
    giverTime = localtime(&date); /* pass a pointer */
    DATE given;
    given.day = giverTime->tm_mday;
    given.month = giverTime->tm_mon + 1; /* simply add instead of increment */
    given.year = giverTime->tm_year + 1900;

    return given;
}

char *get_date(DATE date) {
    char *date_tostring = malloc(20);

    sprintf(date_tostring, "%d.%d.%d", date.day, date.month, date.year);

    date_tostring = (char *) realloc(date_tostring, strlen(date_tostring));

    return date_tostring;
}

void get_STATdata(DATA_Header *header, struct stat file_stat) {

    header->size = file_stat.st_size;
    header->link_nr = file_stat.st_nlink;
    header->last_modify = get_date(timeTConverter(file_stat.st_mtimespec.tv_sec));
    header->user_id = malloc(20);
    header->user_id = getlogin();

    header->user_acces[0] = (file_stat.st_mode & S_IRUSR) ? 'R' : '-';
    header->user_acces[1] = (file_stat.st_mode & S_IWUSR) ? 'W' : '-';
    header->user_acces[2] = (file_stat.st_mode & S_IXUSR) ? 'X' : '-';

    header->group_acces[0] = (file_stat.st_mode & S_IRGRP) ? 'R' : '-';
    header->group_acces[1] = (file_stat.st_mode & S_IWGRP) ? 'W' : '-';
    header->group_acces[2] = (file_stat.st_mode & S_IXGRP) ? 'X' : '-';

    header->other_acces[0] = (file_stat.st_mode & S_IROTH) ? 'R' : '-';
    header->other_acces[1] = (file_stat.st_mode & S_IWOTH) ? 'W' : '-';
    header->other_acces[2] = (file_stat.st_mode & S_IXOTH) ? 'X' : '-';

}

void make_DATAHeader(DATA_Header *message, int file_descriptor) {
    uint8_t *data, *dump;


    if ((dump = (uint8_t *) malloc(14 * sizeof(uint8_t))) == NULL) {
        perror("EROARE LA ALOCAREA DINAMICA\n");
        exit(EXIT_FAILURE);
    }
    read(file_descriptor, dump, 14); // to be dumped

    if ((data = (uint8_t *) malloc(sizeof(uint8_t) * UTIL_CHUNK)) == NULL) {
        perror("EROARE LA ALOCAREA DINAMICA\n");
        exit(EXIT_FAILURE);
    }
    read(file_descriptor, data, UTIL_CHUNK);


    message->width = (data[7] << 24) + (data[6] << 16) + (data[5] << 8) + data[4];
    message->height = (data[11] << 24) + (data[10] << 16) + (data[9] << 8) + data[8];

    free(dump);
}

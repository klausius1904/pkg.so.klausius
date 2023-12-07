//
// Created by Grozav Klaus on 07.12.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include "message.h"

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
    char date_tostring[20];

    sprintf(date_tostring, "%d.%d.%d", date.day, date.month, date.year);

    return date_tostring;
}

void get_STATdata(DATA_Header *header, struct stat file_stat) {

    header->size = file_stat.st_size;
    header->link_nr = file_stat.st_nlink;
    strcpy(header->last_modify, get_date(timeTConverter(file_stat.st_mtimespec.tv_sec)));
    strcpy(header->user_id, getlogin());

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

char *bmp_message(DATA_Header *header) {
    char bmp_string[1000];
    int length = sprintf(bmp_string,
                         "nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %d\nidentificatorul utilizatorului: %s\ntimpul ultimei modificari: %s\ncontorul de legaturi: %d\ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces other: %c%c%c\n",
                         header->file_name, header->height, header->width, header->size,
                         header->user_id, header->last_modify, header->link_nr,
                         header->user_acces[0], header->user_acces[1], header->user_acces[2],
                         header->group_acces[0], header->group_acces[1], header->group_acces[2],
                         header->other_acces[1], header->other_acces[2], header->other_acces[2]
    );
    return bmp_string;
}

char *fil_message(const DATA_Header *header) {
    char fil_string[1000];
    int length = sprintf(fil_string,
                         "nume fisier: %s\ndimensiune: %d\nidentificatorul utilizatorului: %s\ntimpul ultimei modificari: %s\ncontorul de legaturi: %d\ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces other: %c%c%c\n",
                         header->file_name, header->size,
                         header->user_id, header->last_modify, header->link_nr,
                         header->user_acces[0], header->user_acces[1], header->user_acces[2],
                         header->group_acces[0], header->group_acces[1], header->group_acces[2],
                         header->other_acces[1], header->other_acces[2], header->other_acces[2]
    );
    return fil_string;
}

char *dir_message(const DATA_Header *header) {
    char dir_string[1000];
    int length = sprintf(dir_string,
                         "nume fisier: %s\nidentificatorul utilizatorului: %s\ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces other: %c%c%c\n",
                         header->file_name,
                         header->user_id,
                         header->user_acces[0], header->user_acces[1], header->user_acces[2],
                         header->group_acces[0], header->group_acces[1], header->group_acces[2],
                         header->other_acces[1], header->other_acces[2], header->other_acces[2]
    );

    return dir_string;
}

char *link_message(const DATA_Header *header, uint32_t original_size) {
    char link_string[1000];
    int length = sprintf(link_string,
                         "nume fisier: %s\ndimensiune: %d\ndimensiune fisier:%d \ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces other: %c%c%c\n",
                         header->file_name, header->size, original_size,
                         header->user_acces[0], header->user_acces[1], header->user_acces[2],
                         header->group_acces[0], header->group_acces[1], header->group_acces[2],
                         header->other_acces[1], header->other_acces[2], header->other_acces[2]
    );


    return link_string;
}

char *make_bmp_message(struct stat file_stat, int file_descriptor, char *file_name) {
    char *bmp_string = NULL;
    DATA_Header header;
    strcpy(header.file_name, file_name);

    get_STATdata(&header, file_stat);
    make_DATAHeader(&header, file_descriptor);

    bmp_string = bmp_message(&header);

    return bmp_string;
}

char *make_fil_message(struct stat file_stat, int file_descriptor, char *file_name) {
    char *fil_string = NULL;
    DATA_Header header;
    strcpy(header.file_name, file_name);

    get_STATdata(&header, file_stat);
    make_DATAHeader(&header, file_descriptor);

    fil_string = fil_message(&header);

    return fil_string;
}

char *make_dir_message(struct stat file_stat, int file_descriptor, char *file_name) {
    char *dir_string = NULL;
    DATA_Header header;
    strcpy(header.file_name, file_name);

    get_STATdata(&header, file_stat);
    make_DATAHeader(&header, file_descriptor);

    dir_string = fil_message(&header);

    return dir_string;
}

char *make_link_message(struct stat file_stat, int file_descriptor, char *file_name) {
    char *link_string = NULL;
    DATA_Header header;
    strcpy(header.file_name, file_name);

    get_STATdata(&header, file_stat);
    make_DATAHeader(&header, file_descriptor);

    char buf[1024];
    printf("am ajuns aici\n");
    char *r = realpath(file_name, buf);
    printf("%s\n", r);
    struct stat targetStat;
    stat(r, &targetStat);
    DATA_Header original;
    get_STATdata(&original, targetStat);
    printf("am ajuns aici\n");
    link_string = link_message(&header, original.size);
    printf("%s\n",link_string);
    return link_string;
}
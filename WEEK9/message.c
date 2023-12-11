//
// Created by Grozav Klaus on 07.12.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/fcntl.h>
#include "message.h"

void color_gray(char* file_name)
{
    char out_img_name[200];
    strncpy(out_img_name, file_name, strlen(file_name)-4);
    strcat(out_img_name, "_modified.bmp");

    int out_img_descriptor = open(out_img_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (out_img_descriptor == -1) {
        perror("Error opening output file");
        close(out_img_descriptor);
        exit(EXIT_FAILURE);
    }

    int input_img_descriptor = open(file_name, O_RDONLY);
    if (input_img_descriptor == -1) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }
    char header[54];
    if (read(input_img_descriptor, header, 54) != 54) {
        perror("Error reading input file header");
        close(input_img_descriptor);
        close(out_img_descriptor);
        exit(EXIT_FAILURE);
    }
    if (write(out_img_descriptor, header, 54) != 54) {
        perror("eroare la scriere in imagine");
        close(input_img_descriptor);
        close(out_img_descriptor);
        exit(EXIT_FAILURE);
    }
    char pixel[3];
    while (read(input_img_descriptor, pixel, 3) == 3) {
        unsigned char rosu = pixel[2];
        unsigned char verde = pixel[1];
        unsigned char albastru = pixel[0];
        unsigned char gri = 0.299 * rosu + 0.587 * verde + 0.114 * albastru;

        pixel[0] = gri;
        pixel[1] = gri;
        pixel[2] = gri;
        if (write(out_img_descriptor, pixel, 3) != 3) {
            perror("EROARE LA SCRIERE IN IMAGINE");
            close(input_img_descriptor);
            close(out_img_descriptor);
            exit(EXIT_FAILURE);
        }
    }

    close(input_img_descriptor);
    close(out_img_descriptor);
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
    char *date_tostring = malloc(20*sizeof(char));

    sprintf(date_tostring, "%d.%d.%d", date.day, date.month, date.year);
    return date_tostring;
}

void get_STATdata(DATA_Header *header, struct stat file_stat) {
    char *dump;
    header->size = file_stat.st_size;
    header->link_nr = file_stat.st_nlink;
    dump = get_date(timeTConverter(file_stat.st_mtimespec.tv_sec));
    strcpy(header->last_modify, dump);
    free(dump);
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
                         header->other_acces[0], header->other_acces[1], header->other_acces[2]
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
                         header->other_acces[0], header->other_acces[1], header->other_acces[2]
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
                         header->other_acces[0], header->other_acces[1], header->other_acces[2]
    );


    return link_string;
}

char *make_bmp_message(struct stat file_stat, int file_descriptor, char *file_name) {
    char *bmp_string = NULL;
    int pid;
    DATA_Header header;
    strcpy(header.file_name, file_name);

    get_STATdata(&header, file_stat);
    make_DATAHeader(&header, file_descriptor);

    bmp_string = bmp_message(&header);

    if((pid = fork()) < 0) {
        perror("Eroare la crearea procesului");
        exit(-1);
    }
    if(pid == 0) {
        color_gray(file_name);
        exit(0);
    }

    int status;
    if ( waitpid(pid, &status, 0) == -1 ) {
        perror("waitpid failed");
        exit(EXIT_FAILURE);
    }

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
    struct stat targetStat;
    DATA_Header header;
    DATA_Header original;
    strcpy(header.file_name, file_name);

    get_STATdata(&header, file_stat);
    make_DATAHeader(&header, file_descriptor);

    char buf[1024];
    char *r = realpath(file_name, buf);

    stat(r, &targetStat);

    get_STATdata(&original, targetStat);
    link_string = link_message(&header, original.size);
    return link_string;
}
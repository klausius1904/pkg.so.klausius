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

void convertToGrayscale(unsigned char *data, int width, int height, int bitsPerPixel) {
    int bytesPerPixel = bitsPerPixel / 8;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int index = i * width * bytesPerPixel + j * bytesPerPixel;

            if (bitsPerPixel == 24) {
                unsigned char r = data[index];
                unsigned char g = data[index + 1];
                unsigned char b = data[index + 2];
                unsigned char gray = (unsigned char)(0.30 * r + 0.59 * g + 0.11 * b);

                data[index] = gray;
                data[index + 1] = gray;
                data[index + 2] = gray;
            } else {
                // Handle other color depths if needed
                // You may need to adjust this part based on the actual BMP format
            }
        }
    }
}

void convert_bmp(char *file_name) {
    int fd_input;
    if((fd_input = open(file_name, O_RDWR)) == -1) {
        perror("intrarea nu s-a putut deschide!");
        exit(-1);
    }

    BMPHeader header;
    if (read(fd_input, &header, sizeof(BMPHeader)) != sizeof(BMPHeader)) {
        perror("Error reading BMP header");
        close(fd_input);
        exit(EXIT_FAILURE);
    }
    printf("%lu\n", sizeof(BMPHeader));


    printf("%d\n", header.width);
    printf("%d\n", header.height);

    int imageSize = header.imageSize != 0 ? header.imageSize : header.size - header.offset;
    unsigned char *imageData = (unsigned char *)malloc(imageSize);
    if (imageData == NULL) {
        perror("Error allocating memory for image data");
        close(fd_input);
        exit(EXIT_FAILURE);
    }

    if (read(fd_input, imageData, imageSize) != imageSize) {
        perror("Error reading image data");
        free(imageData);
        close(fd_input);
        exit(EXIT_FAILURE);
    }

    //convertToGrayscale(imageData, infoHeader.width, infoHeader.height, infoHeader.bitsPerPixel);

    // Move file pointer to the beginning of the image data
    lseek(fd_input, header.offset, SEEK_SET);

    // Write the modified image data back to the file
    if (write(fd_input, imageData, imageSize) != imageSize) {
        perror("Error writing to input file");
        free(imageData);
        close(fd_input);
        exit(EXIT_FAILURE);
    }

    free(imageData);
    close(fd_input);

    printf("Image converted to grayscale in-place successfully.\n");

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
        convert_bmp(file_name);
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
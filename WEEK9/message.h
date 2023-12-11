//
// Created by Grozav Klaus on 07.12.2023.
//

#include <stdint.h>

#ifndef WEEK8_MESSAGE_H
typedef struct {
    uint32_t size;
    uint32_t width;
    uint32_t height;
    char file_name[30];
    char user_id[30];
    char last_modify[30];
    uint8_t link_nr;
    char user_acces[3];
    char group_acces[3];
    char other_acces[3];
} DATA_Header;

typedef struct Date {
    short day, month, year;
} DATE;

char *make_bmp_message(struct stat file_stat, int file_descriptor, char *file_name);
char *make_fil_message(struct stat file_stat, int file_descriptor, char *file_name);
char *make_dir_message(struct stat file_stat, int file_descriptor, char *file_name);
char *make_link_message(struct stat file_stat, int file_descriptor, char *file_name);

#define UTIL_CHUNK 40
#define WEEK8_MESSAGE_H

#endif //WEEK8_MESSAGE_H

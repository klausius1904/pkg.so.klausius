#include <stdlib.h>
#include <sys/stat.h>
#include <stdint.h>

#ifndef __MESSAGETYPE_H_
#define MESSAGETYPE_H_
typedef struct {
    uint32_t size;
    uint32_t width;
    uint32_t height;
    char *file_name;
    char *user_id;
    char *last_modify;
    uint8_t link_nr;
    char user_acces[3];
    char group_acces[3];
    char other_acces[3];
} DATA_Header;

typedef struct Date {
    short day, month, year;
} DATE;

#define UTIL_CHUNK 40


void check_args(int arguments_number);

char **get_absolute_path(char **filenames, char *directory, int filenames_size);

void make_DATAHeader(DATA_Header *message, int file_descriptor);

void get_STATdata(DATA_Header *header, struct stat file_stat);

char *make_bmp_message(struct stat file_stat, int file_descriptor, char *file_name);

char *make_fil_message(struct stat file_stat, int file_descriptor, char *file_name);

char *make_dir_message(struct stat file_stat, int file_descriptor, char *file_name);

char *make_link_message(struct stat file_stat, int file_descriptor, char *file_name, char *absolute_path);

char *make_absolute_path_string(char *filename, char *absolute_path);

#endif

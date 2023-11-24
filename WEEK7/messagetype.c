#include "messagetype.h"
#include "dynamicprog.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

void check_args(int arguments_number){
  if(arguments_number != 2){
    perror("EROARE: NUMAR INVALID DE ARGUMENTE");
    exit(EXIT_FAILURE);
  }
}

char **get_absolute_path(char **file_names, char *absolute_path, int file_names_size){
  char **absolute_path_filenames = allocate_string_array(file_names_size);
  int index = 0;
  int files_index = 0;
  int total_length = 100;
  
  while(file_names[files_index] != NULL){
    absolute_path_filenames[index] = allocate_string(total_length);
    strcat(absolute_path_filenames[index], absolute_path);
    strcat(absolute_path_filenames[index], "/");
    strcat(absolute_path_filenames[index], file_names[files_index]);
    files_index++;
    index++;
  }
  absolute_path_filenames[index] = NULL;

  return absolute_path_filenames;
}

char *make_bmp_message(struct stat file_stat, int file_descriptor, char *file_name){
  char *bmp_message = allocate_string(1000);
  BMP_header *header = malloc(sizeof(BMP_header));
  header->file_name = malloc(strlen(file_name));
  
  strcpy(header->file_name, file_name);
  STAT_data(header, file_stat);
  BMP_data(header, file_descriptor);
  sprintf(bmp_message,"nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %d\nidentificatorul utilizatorului: %s\ntimpul ultimei modificari: %s\ncontorul de legaturi: %d\ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces other: %c%c%c\n",
            header->file_name, header->height, header->width, header->size,
            header->user_id,header->last_modify, header->link_nr,
            header->user_acces[0],header->user_acces[1], header->user_acces[2],
            header->group_acces[0], header->group_acces[1], header->group_acces[2],
            header->other_acces[1], header->other_acces[2], header->other_acces[2]
            );

  return bmp_message;
}

char *make_fil_message(struct stat file_stat, int file_descriptor, char *file_name){
  char *fil_message = allocate_string(1000);
  BMP_header *header = malloc(sizeof(BMP_header));
  header->file_name = malloc(strlen(file_name));
  
  strcpy(header->file_name, file_name);
  STAT_data(header, file_stat);
  BMP_data(header, file_descriptor);
  sprintf(fil_message,"nume fisier: %s\ndimensiune: %d\nidentificatorul utilizatorului: %s\ntimpul ultimei modificari: %s\ncontorul de legaturi: %d\ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces other: %c%c%c\n",
            header->file_name, header->size,
            header->user_id,header->last_modify, header->link_nr,
            header->user_acces[0],header->user_acces[1], header->user_acces[2],
            header->group_acces[0], header->group_acces[1], header->group_acces[2],
            header->other_acces[1], header->other_acces[2], header->other_acces[2]
            );

  return fil_message;
}

char *make_dir_message(struct stat file_stat, int file_descriptor, char *file_name){
  char *dir_message = allocate_string(1000);
  BMP_header *header = malloc(sizeof(BMP_header));
  header->file_name = malloc(strlen(file_name));
  
  strcpy(header->file_name, file_name);
  STAT_data(header, file_stat);
  BMP_data(header, file_descriptor);
  sprintf(dir_message,"nume fisier: %s\nidentificatorul utilizatorului: %s\ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces other: %c%c%c\n",
            header->file_name,
            header->user_id, 
            header->user_acces[0],header->user_acces[1], header->user_acces[2],
            header->group_acces[0], header->group_acces[1], header->group_acces[2],
            header->other_acces[1], header->other_acces[2], header->other_acces[2]
            );

  return dir_message;
}

char *make_link_message(struct stat file_stat, int file_descriptor, char *file_name){
  char *link_message = allocate_string(1000);
  BMP_header *header = malloc(sizeof(BMP_header));
  header->file_name = malloc(strlen(file_name));
  
  strcpy(header->file_name, file_name);
  STAT_data(header, file_stat);
  BMP_data(header, file_descriptor);

  // char *absolute_path = allocate_string(100);
  // char *dump = allocate_string(100);
  // int count = 0;
  // int read = 0;
  // strcpy(absolute_path,"/Users/TEST/");
  // strcat(absolute_path, file_name);
  // read = readlink(absolute_path, dump, sizeof(dump));
  //   printf("infinit\n");
  //   count+= read;
  //   dump =(char*)realloc(dump, sizeof(dump));
  // 
  // absolute_path[count] = '\0';
  // printf("DIMENSIUNEA FISIERULUI SURSA ESTE : %d\n", count);

  sprintf(link_message,"nume fisier: %s\ndimensiune: %d\ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces other: %c%c%c\n",
            header->file_name, header->size,
            header->user_acces[0],header->user_acces[1], header->user_acces[2],
            header->group_acces[0], header->group_acces[1], header->group_acces[2],
            header->other_acces[1], header->other_acces[2], header->other_acces[2]
            );



  return link_message;
}

DATE timeTConverter(time_t date) 
{
    struct tm *giverTime;
    giverTime = localtime(&date); /* pass a pointer */
    DATE given;
    given.day = giverTime->tm_mday;
    given.month = giverTime->tm_mon+1; /* simply add instead of increment */
    given.year = giverTime->tm_year+1900;

    return given;
}

char *get_date(DATE date){
  char *date_tostring =malloc(20);

  sprintf(date_tostring, "%d.%d.%d", date.day, date.month, date.year);

  date_tostring = (char *)realloc(date_tostring, strlen(date_tostring));

  return date_tostring;
}

void STAT_data(BMP_header *header, struct stat file_stat ){

  header->size = file_stat.st_size;
  header->link_nr = file_stat.st_nlink;
  header->last_modify = get_date(timeTConverter(file_stat.st_mtimespec.tv_sec));  
  header->user_id = malloc(20);
  header->user_id = getlogin();

  header->user_acces[0] = (file_stat.st_mode & S_IRUSR)?'R':'-';
  header->user_acces[1] = (file_stat.st_mode & S_IWUSR)?'W':'-';
  header->user_acces[2] = (file_stat.st_mode & S_IXUSR)?'X':'-';

  header->group_acces[0] = (file_stat.st_mode & S_IRGRP)?'R':'-';
  header->group_acces[1] = (file_stat.st_mode & S_IWGRP)?'W':'-';
  header->group_acces[2] = (file_stat.st_mode & S_IXGRP)?'X':'-';

  header->other_acces[0] = (file_stat.st_mode & S_IROTH)?'R':'-';
  header->other_acces[1] = (file_stat.st_mode & S_IWOTH)?'W':'-';
  header->other_acces[2] = (file_stat.st_mode & S_IXOTH)?'X':'-';

}

void BMP_data(BMP_header* message, int file_descriptor)
{    uint8_t *data, *dump;

    
    if ((dump = (uint8_t *)malloc(14 * sizeof(uint8_t))) == NULL)
    {
        perror("EROARE LA ALOCAREA DINAMICA\n");
        exit(EXIT_FAILURE);
    }
    read(file_descriptor, dump, 14); // to be dumped

    if ((data = (uint8_t *)malloc(sizeof(uint8_t) * UTIL_CHUNK)) == NULL)
    {
        perror("EROARE LA ALOCAREA DINAMICA\n");
        exit(EXIT_FAILURE);
    }
    read(file_descriptor, data, UTIL_CHUNK);


    message->width = (data[7] << 24) + (data[6] << 16) + (data[5] << 8) + data[4];
    message->height = (data[11] << 24) + (data[10] << 16) + (data[9] << 8) + data[8];

    free(dump);
}

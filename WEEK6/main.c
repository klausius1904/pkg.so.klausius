#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define UTIL_CHUNK 40


void close_file(int file)
{
    if (close(file) == -1)
    {
        perror("EROARE LA INCHIDEREA FISIERULUI\n");
        exit(EXIT_FAILURE);
    }
}

uint8_t *parse_file(int file)
{
    uint8_t *data, *dump;
    
    if ((dump = (uint8_t *)malloc(14 * sizeof(uint8_t))) == NULL)
    {
        perror("EROARE LA ALOCAREA DINAMICA\n");
        exit(EXIT_FAILURE);
    }
    read(file, dump, 14); // to be dumped

    if ((data = (uint8_t *)malloc(sizeof(uint8_t) * UTIL_CHUNK)) == NULL)
    {
        perror("EROARE LA ALOCAREA DINAMICA\n");
        exit(EXIT_FAILURE);
    }
    read(file, data, UTIL_CHUNK);

    free(dump);
    return data;
}

typedef struct Date {
   short day, month, year;
} DATE;

typedef struct
{
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
}BMP_header;


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

void BMP_data(uint32_t *width, uint32_t *height, uint8_t *data)
{
    *width = (data[7] << 24) + (data[6] << 16) + (data[5] << 8) + data[4];
    *height = (data[11] << 24) + (data[10] << 16) + (data[9] << 8) + data[8];
}
int main(int argc, char *argv[])
{
    int fin, fout, f_stat;
    uint8_t *data;
    char *to_print;
    struct stat file_stat;
    BMP_header header;

    if (argc != 2)
    {
        perror("NUMAR INVALID DE ARGUMENTE\n");
    }
    if ((fin = open(argv[1], O_RDONLY)) == -1)
    {
        perror("EROARE LA DESCHIDREA FISIERULUI\n");
        exit(EXIT_FAILURE);
    }

    if ((fout = creat("statistica.txt", S_IRUSR | S_IWUSR)) == -1)
    {
        perror("EROARE LA INCHIDEREA FISIERULUI\n");
        exit(EXIT_FAILURE);
    }

    if ((f_stat = fstat(fin, &file_stat)) == -1)
    {

        perror("EROARE LA DESCHIDEREA STATISTICII");
        exit(EXIT_FAILURE);
    }

    data = parse_file(fin);
    
    header.file_name= malloc(strlen(argv[1])+1);

    strcpy(header.file_name, argv[1]);
    BMP_data(&header.width, &header.height, data);
    STAT_data(&header, file_stat);
    
    to_print= malloc(sizeof(char) * 1000);
    sprintf(to_print,"nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %d\nidentificatorul utilizatorului: %s\ntimpul ultimei modificari: %s\ncontorul de legaturi: %d\ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces other: %c%c%c\n",
            header.file_name, header.height, header.width, header.size,
            header.user_id,header.last_modify, header.link_nr,
            header.user_acces[0],header.user_acces[1], header.user_acces[2],
            header.group_acces[0], header.group_acces[1], header.group_acces[2],
            header.other_acces[1], header.other_acces[2], header.other_acces[2]
            );
    write(fout, to_print, strlen(to_print));
    free(data);
    close_file(fin);
    close_file(fout);
    close_file(f_stat);
    return 0;
}

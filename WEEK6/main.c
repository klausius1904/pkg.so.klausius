#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
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

    for (int i = 0; i < sizeof(data); i++)
    {
        printf("%d ", data[i]);
    }
    printf("\n");
    free(dump);
    return data;
}

struct BMP_header
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
};

void BMP_data(uint32_t *width, uint32_t *height, uint8_t *data)
{

    *width = (data[7] << 24) + (data[6] << 16) + (data[5] << 8) + data[4];
    *height = (data[11] << 24) + (data[10] << 16) + (data[9] << 8) + data[8];
    printf("dimensiunea este: %d\n", *width);
}
int main(int argc, char *argv[])
{
    int fin, fout, f_stat;
    uint8_t *data;

    struct stat file_stat;
    struct BMP_header header;

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

    BMP_data(&header.width, &header.height, data);

    free(data);
    close_file(fin);
    close_file(fout);
    close_file(f_stat);
    return 0;
}

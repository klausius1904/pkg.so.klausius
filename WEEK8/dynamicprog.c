#include "dynamicprog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

char *allocate_string(int size) {
    char *dynamic_string = NULL;

    if ((dynamic_string = malloc(sizeof(char) * size)) == NULL) {
        perror("EROARE LA ALOCARE DINAMICA");
        exit(EXIT_FAILURE);
    }
    return dynamic_string;
}

char **allocate_string_array(int size) {
    char **dynamic_string_array = NULL;
    if ((dynamic_string_array = malloc(sizeof(char *) * size)) == NULL) {
        perror("EROARE LA ALOCARE DINAMICA");
        exit(EXIT_FAILURE);
    }
    return dynamic_string_array;
}

char *reallocate_string(char *string, int size) {

    if ((string = realloc(string, strlen(string) + 1)) == NULL) { // NOLINT(*-suspicious-realloc-usage)
        perror("EROARE LA REALOCAREA DINAMICA");
        exit(EXIT_FAILURE);
    }
    return string;
}

char **reallocate_string_array(char **to_realloc, unsigned long new_size) {
    char **dynamic_realloc_array = NULL;
    if ((dynamic_realloc_array = (char **) realloc(to_realloc, new_size * sizeof(char *))) == NULL) {
        perror("EROARE LA ALOCARE DINAMICA");
        exit(EXIT_FAILURE);
    }
    return dynamic_realloc_array;
}

void free_string_array(char **to_free) {
    char **temp = to_free;

    while (*temp != NULL) {
        free(*temp);
        temp++;
    }

}

int *allocate_int_array(int size) {
    int *filedescriptors_array = NULL;

    if ((filedescriptors_array = (int *) malloc(sizeof(int) * size)) == NULL) {
        perror("EROARE LA ALOCARE DINAMICA");
        exit(EXIT_FAILURE);
    }
    return filedescriptors_array;
}

struct stat *allocate_filestats(int size) {
    struct stat *filestat = NULL;
    if ((filestat = (struct stat *) malloc(sizeof(struct stat) * size)) == NULL) {
        perror("EROARE LA ALOCARE DINAMICA");
        exit(EXIT_FAILURE);
    }
    return filestat;
}



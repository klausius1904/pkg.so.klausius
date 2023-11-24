#ifndef DYNAMIC_PROG_
#define DYNAMIC_PROG_

char *allocate_string(int size);
char **allocate_string_array(int size);
char **reallocate_string_array(char** to_realloc, int new_size);
void free_string_array(char** to_free);
int *allocate_filedescriptors_array(int size);
struct stat*  allocate_filestats(int size);

#endif

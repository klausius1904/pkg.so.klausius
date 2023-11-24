#include <dirent.h>

#ifndef __SYSTEMCALLS_H_
#define __SYSTEMCALLS_H_

  DIR* open_dir(char*directory_path);
  void close_dir(DIR* directory_toclose);
  char **get_files_from_directory(DIR* working_directory, int* names_size);
  int *generate_filedescriptors(char **file_names, int size);
  struct stat* generate_filestats( char **absolute_filepaths, int size);
  char **process_filestats(struct stat* file_stats, char**file_names, int* file_descriptors, int size);
  char *process_filestat(struct stat file_stat,int file_descriptor, char *file_name);
#endif

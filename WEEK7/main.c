#include <stdio.h>
#include "systemcalls.h"
#include "messagetype.h"
#include <dirent.h>
#include <stdlib.h>
#include "dynamicprog.h"
#include <sys/stat.h>

int main(int argc, char *argv[]){
  DIR *working_directory = NULL;
  char **file_names = NULL;
  char **absolute_filepath = NULL;
  int names_size = 0;
  int *file_descriptors = NULL;
  struct stat* file_stats = NULL;
  char **messages = NULL;

  check_args(argc);

  working_directory = open_dir(argv[1]);

  file_names = get_files_from_directory(working_directory, &names_size);

  absolute_filepath = get_absolute_path(file_names,  argv[1], names_size); 

  file_descriptors = generate_filedescriptors(absolute_filepath, names_size);

  file_stats = generate_filestats(absolute_filepath, names_size);

  messages = process_filestats(file_stats, file_names, file_descriptors,  names_size);

  printf("\n");
  free(file_descriptors);
  free(file_stats);
  free_string_array(file_names); 
  free_string_array(absolute_filepath);
  close_dir(working_directory);
  return 0;
}

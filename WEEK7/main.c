#include <stdio.h>
#include "systemcalls.h"
#include "messagetype.h"
#include <dirent.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
  DIR *working_directory = NULL;
  char **file_names = NULL;
  char **to_free = NULL;
  int names_size = 0;
  int index = 0;
  check_args(argc);
  working_directory = open_dir(argv[1]);
  
  file_names = get_files_from_directory(working_directory, &names_size);

  while(file_names[index] != NULL){
    printf("%s\n", file_names[index++]);  
  }

  to_free = file_names;
  while(*to_free != NULL){
    free(*to_free);
    to_free++;
  } 
  
  close_dir(working_directory);
  return 0;
}

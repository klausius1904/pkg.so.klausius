#include "systemcalls.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

DIR* open_dir(char *directory_path){
  DIR* to_return; 

  if( (to_return = opendir(directory_path)) == NULL){
    perror("EROARE LA DESCHIDEREA DIRECTORULUI\n");
    exit(EXIT_FAILURE);
  }
  return to_return;
}

void close_dir(DIR *directory_toclose){
  if( closedir(directory_toclose) == -1){
    perror("EROARE LA INCHIDEREA DIRECTORULUI\n");
    exit(EXIT_FAILURE);
  }
}

char **get_files_from_directory(DIR* working_directory, int* names_size){
  struct dirent *directory_pointer;
  char **file_names;
  *names_size = 30;
  file_names = (char**)malloc(*names_size*sizeof(char*));
  uint8_t current_index = 0; 

  while( (directory_pointer = readdir(working_directory)) != NULL){
    if(current_index == *names_size){
      names_size += 30;
      file_names = (char**)realloc(file_names, *names_size*sizeof(char*));
    }
    file_names[current_index] =(char*)malloc(strlen(directory_pointer->d_name));
    strcpy(file_names[current_index++], directory_pointer->d_name);     
  }
  
  file_names[current_index] = NULL;
  file_names = realloc(file_names, current_index*sizeof(char*)); 
  *names_size = current_index;

  return file_names;
}



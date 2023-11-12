#include <dirent.h>

#ifndef __SYSTEMCALLS_H_
#define __SYSTEMCALLS_H_

  DIR* open_dir(char*directory_path);
  void close_dir(DIR* directory_toclose);
  char **get_files_from_directory(DIR* working_directory, int* names_size);

#endif

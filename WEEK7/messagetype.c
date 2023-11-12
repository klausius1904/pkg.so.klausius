#include "messagetype.h"
#include <stdlib.h>
#include <stdio.h>


void check_args(int arguments_number){
  if(arguments_number != 2){
    perror("EROARE: NUMAR INVALID DE ARGUMENTE");
    exit(EXIT_FAILURE);
  }
}


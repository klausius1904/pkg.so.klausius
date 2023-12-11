//
// Created by Grozav Klaus on 07.12.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

void verify_arguments(int arguments_no){
    if(arguments_no !=4){
        perror("EROARE LA NUMARUL DE ARGUEMENTE\n");
        exit(EXIT_FAILURE);
    }
}
/*
 * Copyright (C) 2022 Nubix Incorporated.  All rights reserved.
 */

#include <stdio.h>

// #include "bi-inc/nubix_api.h"

// Nubix container entrypoint
void on_init()
{
   printf("**** on_init() called ****\n\n");
   printf("\
        ██     ██ ███████ ██████   █████  ███████ ███████ ███████ ███    ███ ██████  ██   ██    ██ \n\
        ██     ██ ██      ██   ██ ██   ██ ██      ██      ██      ████  ████ ██   ██ ██    ██  ██  \n\
        ██  █  ██ █████   ██████  ███████ ███████ ███████ █████   ██ ████ ██ ██████  ██     ████   \n\
        ██ ███ ██ ██      ██   ██ ██   ██      ██      ██ ██      ██  ██  ██ ██   ██ ██      ██    \n\
         ███ ███  ███████ ██████  ██   ██ ███████ ███████ ███████ ██      ██ ██████  ███████ ██    \n\
                                                                                                   \n\
                                                                                 powered by Atym   \n");

   printf("\n\n Update v4\n");
}

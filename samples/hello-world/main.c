/*
 * Copyright (C) 2024 Atym Incorporated. All rights reserved.
 */

#include <stdio.h>

// Atym container entrypoint
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
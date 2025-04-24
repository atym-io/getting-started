/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */
#include <stdio.h>
#include <locale.h>

// Application entry point
int main() 
{
    // Set the locale to support Unicode characters
    setlocale(LC_ALL, "");
    
    printf("\n\
        ██     ██ ███████ ██████   █████  ███████ ███████ ███████ ███    ███ ██████  ██   ██    ██ \n\
        ██     ██ ██      ██   ██ ██   ██ ██      ██      ██      ████  ████ ██   ██ ██    ██  ██  \n\
        ██  █  ██ █████   ██████  ███████ ███████ ███████ █████   ██ ████ ██ ██████  ██     ████   \n\
        ██ ███ ██ ██      ██   ██ ██   ██      ██      ██ ██      ██  ██  ██ ██   ██ ██      ██    \n\
         ███ ███  ███████ ██████  ██   ██ ███████ ███████ ███████ ██      ██ ██████  ███████ ██    \n\
                                                                                                   \n\
                                                                                 powered by Atym   ");
    printf("\n\n Update v4\n\n");
    return 0;
}

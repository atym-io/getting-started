#ifndef OCRE_API_H
#define OCRE_API_H

#include <stdbool.h>

// TIMER API

// void ocre_timer_init();
int ocre_timer_create(int id);
int ocre_timer_delete(int id);
int ocre_timer_start(int id, int interval, int is_periodic);
int ocre_timer_stop(int id);
int ocre_timer_get_remaining(int id);
#endif // OCRE_API_H

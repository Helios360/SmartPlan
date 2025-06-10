#ifndef GUI_H
#define GUI_H
#ifndef EVENT_DATA_H
#define EVENT_DATA_H

#define MAX_EVENT 1000


void gui(int argc, char *argv[]);


typedef struct {
    int prio, year, month, day, hours, minutes, seconds, duration;
    unsigned long long id;
    char peoples[300];
    char desc[300];
} event;

extern event events[MAX_EVENT];
extern int event_count;

#endif
#endif
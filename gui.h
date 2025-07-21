#ifndef GUI_H
#define GUI_H
#ifndef EVENT_DATA_H
#define EVENT_DATA_H

#define MAX_EVENT 1000

typedef struct {
    int prio, year, month, day, hours, minutes, seconds, duration;
    unsigned long long id;
    char desc[300];
} event;

extern event events[MAX_EVENT];
extern int event_count;

void gui(int argc, char *argv[]);
void delete_event_by_id(unsigned long long id);
void create_event(event *e, int prio, int year, int month, int day,
    int hour, int minute, int second, int duration, const char *desc);
void write_all();

#endif
#endif
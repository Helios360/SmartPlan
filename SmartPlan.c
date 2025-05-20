#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX_EVENT 1000

typedef struct { //Event structure stored in Events.csv with
    int prio, id, year, month, day, ToD;
    char desc[300];
} event;

event events[MAX_EVENT];
int event_count = 0;

void build() { // Calendar display CLI
    int months, days = 1, year = 2025;
    int has_event = 0;
    for (months = 1; months <= 12; months++) {
        printf("\n|");
        if (months == 2 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))) {
            for (days; days <= 29; days++) {
                has_event = 0;
                for (int i = 0; i < event_count; i++){
                      if(events[i].month==months && events[i].day==days ){
                        has_event = 1;
                      }
                }
                if (has_event) {
                    printf("%2dE|",days);
                } else {
                    printf("%2d |",days);
                }
                if (days % 8 == 0 && days != 0) {
                    printf("\n");
                }
            }
        } else {
            for (days; days <= 31; days++) {
                has_event = 0;
                for (int i = 0; i < event_count; i++){
                      if(events[i].month==months && events[i].day==days ){
                    
                        has_event = 1;
                      }
                }
                if (has_event) {
                    printf("%2dE|",days);
                } else {
                    printf("%2d |",days);
                }
                if (days % 8 == 0 && days != 0) {
                    printf("\n|");
                }
            }
        }
        days = 1;
        printf("\n");
    }
}

void read_all() { // Gets all events from the csv file to use them in the program (display, delete, edit, etc...)
    FILE *reading = fopen("Events.csv", "r");
    if (reading == NULL) {
        printf("WHOOPS - Failed to open Events.csv | File error (absent/uncorrect path)\n");
        exit(EXIT_FAILURE);
        return;
    }

    char row[500];
    fgets(row, sizeof(row), reading); // Skip header
    while (fgets(row, sizeof(row), reading)) {
        printf("Event list\n");
        event e;
        char *token = strtok(row, ",");
        if (token != NULL) e.prio = atoi(token);
        printf("%d,",e.prio);
        token = strtok(NULL, ",");
        if (token != NULL) e.id = atoi(token);
        token = strtok(NULL, ",");
        printf("%d,",e.id);
        if (token != NULL) e.year = atoi(token);
        token = strtok(NULL, ",");
        printf("%d,",e.year);
        if (token != NULL) e.month = atoi(token);
        token = strtok(NULL, ",");
        printf("%d,",e.month);
        if (token != NULL) e.day = atoi(token);
        token = strtok(NULL, ",");
        printf("%d,",e.day);
        if (token != NULL) e.ToD = atoi(token);
        token = strtok(NULL, "\n");
        printf("%d,",e.ToD);
        if (token != NULL) strcpy(e.desc, token);
        printf("%s\n",e.desc);
        
        events[event_count++] = e;
    }
    fclose(reading);
}

void write_all() { // When, the user is done using the prog, it writes all data from scratch
    FILE *writing = fopen("Events.csv", "w");
    if (writing == NULL) {
        printf("Failed to open events.csv for writing\n");
    }

    // Write header
    fprintf(writing, "prio,id,year,month,day,desc\n");

    // Write each event
    for (int i = 0; i < event_count; i++) {
        fprintf(writing, "%d,%d,%d,%d,%d,%d,%s\n", events[i].prio, events[i].id, events[i].year, events[i].month, events[i].day, events[i].ToD, events[i].desc);
    }

    fclose(writing);
}

void delete_event_by_id(int id) {
    int i;
    for (i = 0; i < event_count; i++) {
        if (events[i].id == id) {
            // Shift all elements after the deleted one to the left
            for (int j = i; j < event_count - 1; j++) {
                events[j] = events[j + 1];
            }
            event_count--; // Decrease the count
            printf("Event with ID %d deleted.\n", id);
            return;
        }
    }
    printf("Event with ID %d not found.\n", id);
}

void update_event_by_id(int id, event updated_event) {
    for (int i = 0; i < event_count; i++) {
        if (events[i].id == id) {
            events[i] = updated_event; // Replace the old event with the new one
            printf("Event with ID %d updated.\n", id);
            return;
        }
    }
    printf("Event with ID %d not found.\n", id);
}

void create_event(event *e, int id) {
    char day_info[1];
    int hour,minute,second,ToD; // ToD == time of the day
    printf("Please enter prio, id, year, month, day, desc\n");
    printf("Prio : ");
    scanf("%d", &e->prio);
    e->id = id; // Set the id to the given one
    printf("Year : ");
    scanf("%d", &e->year);
    printf("Month : ");
    scanf("%d", &e->month);
    printf("Day : ");
    scanf("%d", &e->day);
    getchar();
    printf("Input day info ?(Y/n)");
    scanf("%c",&day_info);
    if (strcmp(day_info,"Y")==0){
        printf("hour 0-24:");
        scanf("%d",&hour);
        printf("minute 0-60:");
        scanf("%d",&minute);
        printf("second 0-60:");
        scanf("%d",&second);
        ToD = hour*3600 + minute*60 + second;
        printf("%d\n",ToD);
        e->ToD = ToD;
    } else {
        e->ToD = '\0';
    }
    getchar();
    printf("Desc : ");
    fgets(e->desc, sizeof(e->desc), stdin);
    // Remove trailing newline if present
    size_t len = strlen(e->desc);
    if (len > 0 && e->desc[len - 1] == '\n') {
        e->desc[len - 1] = '\0';
    }
}

int main() {
    read_all();
    build();
    
    printf("Enter 1 to delete, 2 to update, or 3 to add new event: ");
    int action, id_to_delete, id_to_update, new_id;
    scanf("%d", &action);

    if (action == 1) {
        printf("Enter event ID to delete: ");
        scanf("%d", &id_to_delete);
        delete_event_by_id(id_to_delete);
    } else if (action == 2) {
        printf("Enter event ID to update: ");
        scanf("%d", &id_to_update);
        event updated_event;
        create_event(&updated_event, id_to_update);
        update_event_by_id(id_to_update, updated_event);
    } else if (action == 3) {
        event new_event;
        new_id = event_count + 1;  // For simplicity, set new ID as the count of events + 1
        create_event(&new_event, new_id);
        events[event_count++] = new_event;
    }

    write_all();  // Write changes back to the file from 0
    printf("Current event ID at index 1: %d\n", events[1].id);
    return 0;
}

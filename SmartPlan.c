#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX_EVENT 1000
typedef struct {
    int prio, id, year, month, day;
    char desc[200];
} event;
event events[MAX_EVENT];
int event_count = 0;
void build() {
    int months, days = 1, year = 2025;
    for (months = 1; months <= 12; months++) {
        printf("\n|");
        if (months == 2 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))) {
            for (days; days <= 29; days++) {
                printf("%d|", days);
                if (days % 8 == 0 && days != 0) {
                    printf("\n");
                }
            }
        } else {
            for (days; days <= 31; days++) {
                printf("%d|", days);
                if (days % 8 == 0 && days != 0) {
                    printf("\n");
                }
            }
        }
        days = 1;
        printf("\n");
    }
}
void read_all() {
    FILE *reading = fopen("Events.csv", "r");
    if (reading == NULL) {
        printf("Failed to open events.csv\n");
        return;
    }
    char row[500];
    while (fgets(row, sizeof(row), reading)) {
        event e;
        char *token = strtok(row, ",");
        if (token != NULL) e.prio = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) e.id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) e.year = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) e.month = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) e.day = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) strcpy(e.desc, token);
        events[event_count++] = e;
    }
    fclose(reading);
}
void write_all() {
    FILE *writing = fopen("Events.csv", "w");
    if (writing == NULL) {
        printf("Failed to open events.csv for writing\n");
        return;
    }
    // Write header
    fprintf(writing, "prio,id,year,month,day,desc\n");
    // Write each event
    for (int i = 0; i < event_count; i++) {
        fprintf(writing, "%d,%d,%d,%d,%d,%s\n", events[i].prio, events[i].id, events[i].year, events[i].month, events[i].day, events[i].desc);
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
void input(event *e, int id) {
    printf("Please enter prio, id, year, month, day, desc\n");
    scanf("%d", &e->prio);
    e->id = id; // Set the id to the given one
    scanf("%d", &e->year);
    scanf("%d", &e->month);
    scanf("%d", &e->day);
    scanf(" %[^\n]", e->desc);  // Use this format to get a string with spaces
}
int main() {
    build();
    read_all();
    printf("Enter 1 to delete, 2 to update, or 3 to add new event: ");
    int choice;
    scanf("%d", &choice);
    if (choice == 1) {
        int id_to_delete;
        printf("Enter event ID to delete: ");
        scanf("%d", &id_to_delete);
        delete_event_by_id(id_to_delete);
    } else if (choice == 2) {
        int id_to_update;
        printf("Enter event ID to update: ");
        scanf("%d", &id_to_update);
        event updated_event;
        input(&updated_event, id_to_update);
        update_event_by_id(id_to_update, updated_event);
    } else if (choice == 3) {
        event new_event;
        int new_id = event_count + 1;  // For simplicity, set new ID as the count of events + 1
        input(&new_event, new_id);
        events[event_count++] = new_event;
    }
    write_all();  // Write changes back to the file
    printf("Current event ID at index 1: %d\n", events[1].id);
    return 0;
}

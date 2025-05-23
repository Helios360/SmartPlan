#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#define MAX_EVENT 1000

typedef struct { // Event structure stored in Events.csv with
    int prio, id, year, month, day, hours, minutes, seconds;
    char desc[300];
} event;

event events[MAX_EVENT];
int event_count = 0;
// Color coding
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"
#define BLINK   "\x1b[5m"
/*
void build(int year) { // Calendar display CLI input year to build
    int months, days = 1,pair = 31;
    int has_event = 0;
    printf(RED"\n ------- YEAR : %d ------- \n"RESET,year);
    for (months = 1; months <= 12; months++) {
        printf(MAGENTA"\n -------- MONTH : %d -------- \n"RESET,months);
        if (months == 2 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))) {
            printf(YELLOW"|MON|TUE|WED|THU|FRI|SAT|SUN|"CYAN"\n|");
            for (days; days <= 29; days++) {
                has_event = 0;
                for (int i = 0; i < event_count; i++){
                    if(events[i].year==year && events[i].month==months && events[i].day==days ){
                        has_event = 1;
                    }
                }
                if (has_event) {
                    printf(CYAN"%2dE|"RESET,days);
                } else {
                    printf(CYAN"%2d |"RESET,days);
                }
                if (days % 7 == 0 && days != 0) {
                    printf(CYAN"\n|"RESET);
                }
            }
        } else {
            if (months!=2 && months%2==0){
                pair=30;
            } else if (months == 2){ 
                pair = 28;
            } else {pair = 31;}
            printf(YELLOW"|MON|TUE|WED|THU|FRI|SAT|SUN|"CYAN"\n|");
            for (days; days <= pair; days++) {
                has_event = 0;
                for (int i = 0; i < event_count; i++){
                    if(events[i].month==months && events[i].day==days ){
                        has_event = 1;
                    }
                }
                if (has_event) {
                    printf(RED"%2dX"CYAN"|"RESET,days);
                } else {
                    printf(CYAN"%2d |",days);
                }
                if (days % 7 == 0 && days != 0) {
                    printf("\n|");
                }
            }
        }
        days = 1;
        printf("\n"RESET);
    }
}*/

void build_day_events(int year, int month, int day){
    int i;
    for (i = 0; i < event_count; i++) {
        if (events[i].year == year && events[i].month == month && events[i].day == day) {
            printf("ID %d | %04d-%02d-%02d/%02d:%02d:%02ds | Prio %d | %s\n",
                       events[i].id,
                       events[i].year, events[i].month, events[i].day,
                       events[i].hours, events[i].minutes, events[i].seconds,
                       events[i].prio, events[i].desc);
        }
    }
}

int get_days_in_month(int month, int year) {
    if (month == 2) {
        return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0) ? 29 : 28;
    } else if (month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    } else {
        return 31;
    }
}

// Display a single month
void build_month(int month, int year) {
    printf(MAGENTA"\n -------- MONTH : %d -------- \n"RESET, month);
    printf(YELLOW"|MON|TUE|WED|THU|FRI|SAT|SUN|"CYAN"\n|");

    int days_in_month = get_days_in_month(month, year);
    int has_event;

    for (int day = 1; day <= days_in_month; day++) {
        has_event = 0;
        for (int i = 0; i < event_count; i++) {
            if (events[i].year == year && events[i].month == month && events[i].day == day) {
                has_event = 1;
                break;
            }
        }
        if (has_event) {
            printf(RED"%2dX"CYAN"|"RESET,day);
        } else {
            printf(CYAN"%2d |",day);
        }
        if (day % 7 == 0 && day != 0) {
            printf("\n|");
        }
    }
    printf("\n"RESET);
}

// Display the entire year
void build_year(int year) {
    printf(RED"\n ------- YEAR : %d ------- \n"RESET, year);
    for (int month = 1; month <= 12; month++) {
        build_month(month, year);
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
        if (token != NULL) e.hours = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) e.minutes = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) e.seconds = atoi(token);
        token = strtok(NULL, "\n");
        if (token != NULL) strcpy(e.desc, token);
        
        events[event_count++] = e;
    }
    fclose(reading);
}

void write_all() { // When the user is done using the prog, it writes all data from scratch
    FILE *writing = fopen("Events.csv", "w");
    if (writing == NULL) {
        printf("Failed to open events.csv for writing\n");
    }
    // Write header
    fprintf(writing, "prio,id,year,month,day,hours,minutes,seconds,desc\n");
    // Write each event
    for (int i = 0; i < event_count; i++) {
        fprintf(writing, "%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
            events[i].prio, events[i].id,
            events[i].year, events[i].month, events[i].day,
            events[i].hours, events[i].minutes, events[i].seconds,
            events[i].desc);
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
/* to create events manually 'obsellete
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
*/
void create_event(event *e, int id, int prio, int year, int month, int day,
int use_time, int hour, int minute, int second, const char *desc) {

    e->prio = prio;
    e->id = id;
    e->year = year;
    e->month = month;
    e->day = day;
    e->hours = hour;
    e->minutes = minute;
    e->seconds = second;

    strncpy(e->desc, desc, sizeof(e->desc) - 1);
    e->desc[sizeof(e->desc) - 1] = '\0';  // Ensure null termination
}

void APIPE(){ // fusion between an api and a pipe lol
    const char *pipe_path = "/tmp/calendar_pipe";
    char buffer[512];
    char command[32];
    int cmd_id, prio, year, month, day, hour, minute, second;
    char desc[300];

    mkfifo(pipe_path, 0666);
    FILE *pipe = fopen(pipe_path, "r");
    if (!pipe){
        perror("ALERT ! Broken pipe...");
        exit(1);
    }

    while(1){
        if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            if (sscanf(buffer, "%s %d %d %d %d %d %d %d %d %[^\n]",
                command, &cmd_id, &prio, &year, &month, &day,
                &hour, &minute, &second, desc) == 8) {
            if (strcmp(command, "create") == 0) {
                event e;
                create_event(&e, cmd_id, prio, year, month, day, 1, hour, minute, second, desc);
                events[event_count++] = e;
                printf("[PIPE] Created event ID %d: %s\n", cmd_id, e.desc);
            } else if (strcmp(command, "delete") == 0) {
                delete_event_by_id(cmd_id);
            } else if (strcmp(command, "update") == 0) {
                event updated;
                create_event(&updated, cmd_id, prio, year, month, day, 1, hour, minute, second, desc);
                update_event_by_id(cmd_id, updated);
                printf("[PIPE] Updated event ID %d\n", cmd_id);
            }
            write_all();
        }
    }
    fclose(pipe);
    }
}

void command_loop() { // Cli
    char input[512];
    printf(BLUE"Welcome to the SmartPlanner CLI.\nType 'help' for commands.\n"RESET);

    while (1) {
        printf(GREEN"calendar>>"RESET);
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        input[strcspn(input, "\n")] = 0; // Remove trailing newline
        if (strcmp(input, "exit") == 0) { // Parse command
            break;
        } else if (strcmp(input, "help") == 0) {
            printf(YELLOW"Commands:\n");
            printf("    list----------------List all events\n");
            printf("    create--------------Create a new event\n");
            printf("    delete <id>---------Delete event by ID\n");
            printf("    update <id>---------Update event by ID\n");
            printf("    build <y> <m> <d>---Print year|month|day's events\n");
            printf("    exit----------------Quit the program\n"RESET);
        } else if (strcmp(input, "list") == 0) {
            for (int i = 0; i < event_count; i++) {
                printf("%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
                    events[i].prio, events[i].id,
                    events[i].year, events[i].month, events[i].day,
                    events[i].hours, events[i].minutes, events[i].seconds,
                    events[i].desc);
            }
        } else if (strncmp(input, "build ", 6) == 0) {
            int year, month, day;
            char *args = input + 6;
            int count = sscanf(args, "%d %d %d", &year, &month, &day);
            if (count == 1){
                build_year(year);
            } else if (count == 2) {
                build_month(month, year);
            } else if (count == 3) {
                build_day_events(year,month,day);
            } else { printf(RED"Usage: build <year> <?month> <?day>\n"RESET);}
            
        } else if (strncmp(input, "delete ", 7) == 0) {
            int id = atoi(input + 7);
            delete_event_by_id(id);
        } else if (strncmp(input, "update ", 7) == 0) {
            int id = atoi(input + 7);
            char new_desc[200];
            printf("Enter new description :\n");
            scanf("%s",&new_desc);
            event updated;
            create_event(&updated, id, 3, 2025, 11, 10, 1, 9, 15, 0, new_desc);
            update_event_by_id(id, updated);
        } else if (strcmp(input, "create") == 0) {
            event new_event;
            int new_id = event_count + 1;
            char day_info;
            int hour = 0, minute = 0, second = 0;

            printf("Please enter prio, year, month, day:\n");
            printf("Prio: ");
            scanf("%d", &new_event.prio);

            new_event.id = new_id;

            printf("Year: ");
            scanf("%d", &new_event.year);
            printf("Month: ");
            scanf("%d", &new_event.month);
            printf("Day: ");
            scanf("%d", &new_event.day);

            getchar(); // Clear newline after last input
            printf("Input time of day? (Y/n): ");
            scanf("%c", &day_info);

            if (day_info == 'Y' || day_info == 'y') {
                printf("Hour (0-23): ");
                scanf("%d", &hour);
                printf("Minute (0-59): ");
                scanf("%d", &minute);
                printf("Second (0-59): ");
                scanf("%d", &second);
            }

            new_event.hours = hour;
            new_event.minutes = minute;
            new_event.seconds = second;

            getchar(); // Consume leftover newline
            printf("Description: ");
            fgets(new_event.desc, sizeof(new_event.desc), stdin);
            size_t len = strlen(new_event.desc);
            if (len > 0 && new_event.desc[len - 1] == '\n') {
                new_event.desc[len - 1] = '\0';
            }

            events[event_count++] = new_event;
            printf("Created event with ID %d\n", new_id);
        } else {
            printf("Unknown command. Type 'help' to list commands.\n");
        }
        // Write to file after each command
        write_all();
    }
    printf("Exiting calendar.\n");
}

int main() {
    read_all();
    time_t now = time(NULL);
    struct tm *time_info = localtime(&now);
    int year = time_info->tm_year + 1900;
    build_year(year);
    pid_t pid = fork();
    if (pid < 0){ // Check forking
        perror("ERROR : Fork failed...");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        // Child process: run APIPE
        APIPE();
        exit(0); // Make sure the child doesn't fall through
    }
    /*
    int action, id_to_delete, id_to_update, new_id;

    printf("Enter 1 to delete, 2 to update, or 3 to add new event: ");
    scanf("%d", &action);

    if (action == 1) {
        printf("Enter event ID to delete: ");
        scanf("%d", &id_to_delete);
        delete_event_by_id(id_to_delete);
    } else if (action == 2) {
        printf("Enter event ID to update: ");
        scanf("%d", &id_to_update);
        int prio = 3, year = 2025, month = 11, day = 10;
        int use_time = 1, hour = 9, minute = 15, second = 0;
        const char *desc = "Updated Event Description";

        event updated_event;
        create_event(&updated_event, id_to_update, prio, year, month, day,
                    use_time, hour, minute, second, desc);
        update_event_by_id(id_to_update, updated_event);
    } else if (action == 3) {

        event new_event;
        new_id = event_count + 1;  // For simplicity, set new ID as the count of events + 1
        create_event(&new_event, new_id, 5, 2025, 12, 25, 1, 14, 30, 0, "Christmas Lunch");

        events[event_count++] = new_event;
    }

    write_all();  // Write changes back to the file from 0
    printf("Current event ID at index 1: %d\n", events[1].id);
    */
    command_loop();
    return 0;
}
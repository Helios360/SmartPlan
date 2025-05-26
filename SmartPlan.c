#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <curl/curl.h>
//#include <cjson/cJSON.h>
#define MAX_EVENT 1000

typedef struct { // Event structure stored in Events.csv with
    int prio, id, year, month, day, hours, minutes, seconds, duration;
    char peoples[300];
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

void build_day_events(int year, int month, int day){
    int i;
    for (i = 0; i < event_count; i++) {
        if (events[i].year == year && events[i].month == month && events[i].day == day) {
            printf("ID %d | %04d-%02d-%02d/%02d:%02d:%02ds | Prio %d | %s\n",
                       events[i].id,
                       events[i].year, events[i].month, events[i].day,
                       events[i].hours, events[i].minutes, events[i].seconds,
                       events[i].prio, events[i].duration, events[i].peoples, events[i].desc);
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
        token = strtok(NULL, ",");
        if (token != NULL) e.duration = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) strcpy(e.peoples, token);
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
        fprintf(writing, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%s,%s\n",
            events[i].prio, events[i].id,
            events[i].year, events[i].month, events[i].day,
            events[i].hours, events[i].minutes, events[i].seconds,
            events[i].duration, events[i].peoples, events[i].desc);
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

void create_event(event *e, int id, int prio, int year, int month, int day,
    int hour, int minute, int second, int duration, const char *peoples, const char *desc) {

    e->prio = prio;
    e->id = id;
    e->year = year;
    e->month = month;
    e->day = day;
    e->hours = hour;
    e->minutes = minute;
    e->seconds = second;
    e->duration = duration;

    strncpy(e->peoples, peoples, sizeof(e->peoples) - 1);
    e->peoples[sizeof(e->peoples) - 1] = '\0';
    strncpy(e->desc, desc, sizeof(e->desc) - 1);
    e->desc[sizeof(e->desc) - 1] = '\0';  // Ensure null termination
}

void optimize() {
    /* the part here is dedicated to be an implementation of AI
    to dissect the description of the event into multiple arguments
    like place (use OSM lib), peoples use(SAI DB), duration and priority 
    is already argumented I'll use libcurl to interface with the ollama 
    api for local AI (I might put this in another function tho :3)
    Exemple : Desc = "Visiting grandma(people) at her house(place)" */
    
    for (int i = 0; i < event_count; i++) {
        for (int y = 0; y < event_count; y++){
            if (events[i].year==events[y].year && i!=y){
                if (events[i].month==events[y].month && events[i].day==events[y].day) {
                    
                }
            }
        }
    }
}

void APIPE(){ // Will be useful if SmartPlan is integrated into a system
    const char *pipe_path = "/tmp/calendar_socket";
    char buffer[512];
    char command[32];
    int cmd_id, prio, year, month, day, hour, minute, second, duration;
    char peoples[300];
    char desc[300];

    mkfifo(pipe_path, 0666);
    FILE *pipe = fopen(pipe_path, "r");
    if (!pipe){
        perror("ALERT ! Broken pipe...");
        exit(1);
    }

    while(1){
        if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            if (sscanf(buffer, "%s %d %d %d %d %d %d %d %d %[^|]|%[^\n]",
                command, &cmd_id, &prio, &year, &month, &day,
                &hour, &minute, &second, &duration, peoples, desc) == 12) {
            if (strcmp(command, "create") == 0) {
                event e;
                create_event(&e, cmd_id, prio, year, month, day, hour, minute, second, duration, peoples, desc);
                events[event_count++] = e;
                printf("[PIPE] Created event ID %d: %s\n", cmd_id, e.desc);
            } else if (strcmp(command, "delete") == 0) {
                delete_event_by_id(cmd_id);
            } else if (strcmp(command, "update") == 0) {
                event updated;
                create_event(&updated, cmd_id, prio, year, month, day, hour, minute, second, duration, peoples, desc);
                update_event_by_id(cmd_id, updated);
                printf("[PIPE] Updated event ID %d\n", cmd_id);
            }
            write_all();
        }
    }
    fclose(pipe);
    }
}

event prompt_user_for_event_data(int id) {
    event e;
    char peoples[300];
    char desc[300];
    char day_info;
    int hour = 0, minute = 0, second = 0, duration = 0;

    printf("Please enter prio, year, month, day:\n");
    printf("Prio: ");
    scanf("%d", &e.prio);
    e.id = id;

    printf("Year: ");
    scanf("%d", &e.year);
    printf("Month: ");
    scanf("%d", &e.month);
    printf("Day: ");
    scanf("%d", &e.day);

    getchar(); // Clean newline
    printf("Input time of day? (y/n): ");
    scanf("%c", &day_info);

    if (day_info == 'Y' || day_info == 'y') {
        printf("Hour (0-23): ");
        scanf("%d", &hour);
        printf("Minute (0-59): ");
        scanf("%d", &minute);
        printf("Second (0-59): ");
        scanf("%d", &second);
    }

    e.hours = hour;
    e.minutes = minute;
    e.seconds = second;

    getchar(); // clear
    printf("Duration (in minutes): ");
    scanf("%d", &duration);
    e.duration = duration;

    getchar(); // clear
    printf("People involved (separate with semicolons): ");
    fgets(peoples, sizeof(peoples), stdin);
    peoples[strcspn(peoples, "\n")] = 0;

    printf("Description: ");
    fgets(desc, sizeof(desc), stdin);
    desc[strcspn(desc, "\n")] = 0;

    create_event(&e, id, e.prio, e.year, e.month, e.day,
                 e.hours, e.minutes, e.seconds, duration, peoples, desc);

    return e;
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
            printf("    optimize------------Optimize events placements with AI\n");
            printf("    exit----------------Quit the program\n"RESET);
        } else if (strcmp(input, "list") == 0) {
            for (int i = 0; i < event_count; i++) {
                printf(">%d | %04d-%02d-%02d/%02d:%02d:%02ds/t=%d | Prio %d |\nwith %s | %s\n",
                       events[i].id,
                       events[i].year, events[i].month, events[i].day,
                       events[i].hours, events[i].minutes, events[i].seconds,
                       events[i].duration, events[i].prio, events[i].peoples, events[i].desc);
                printf("-------------------------------------------------------\n");
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
            event updated_event = prompt_user_for_event_data(id);
            update_event_by_id(id, updated_event);
        } else if (strcmp(input, "create") == 0) {
            int new_id = event_count + 1;
            event new_event = prompt_user_for_event_data(new_id);
            events[event_count++] = new_event;
            printf("Created event with ID %d\n", new_id);
        } else if (strcmp(input, "optimize") == 0) {
            
        } else {
            printf("Unknown command. Type 'help' to list commands.\n");
        }
        // Write to file after each command
        write_all();
    }
    printf("Exiting calendar.\n");
}

int main(int argc, char *argv[]) {
    read_all();
    time_t now = time(NULL);
    struct tm *time_info = localtime(&now);
    int year = time_info->tm_year + 1900;
    if (argv[1]!=NULL && strcmp(argv[1],"-api")==0){
        printf("The program is running in api mode (headless) no CLI available...");
        APIPE();
    } else if (argv[1]!=NULL && strcmp(argv[1],"-help")==0) {
        printf("SmartPlan is a Calender powered by super algos and dreams ...\n");
        printf("    -api -------------- Headless, API\n");
        printf("    none -------------- CLI prompt\n");
    } else {
        build_year(year);
        command_loop();
    }
    return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <curl/curl.h>


void APIPE(){ // Will be useful if SmartPlan is integrated into a system
    const char *pipe_path = "/tmp/calendar_socket";
    char buffer[512];
    char command[32];
    int cmd_id, prio, year, month, day, hour, minute, second, duration;
    char peoples[300];
    char desc[300];

    mkfifo(pipe_path, 0666);
    FILE *pipe = fopen(pipe_path, "w");
    if (!pipe){
        perror("ALERT ! Broken pipe...");
        exit(1);
    }
    fprintf(pipe, "create 1 1 2025 10 10 10 10 20 10 elias;lucas|doingsomethingidk");
    fclose(pipe);
}

void main(){
    APIPE();
}
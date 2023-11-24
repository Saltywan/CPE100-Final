#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <strings.h>

int main()
{
    //create a file
    FILE *fp;
    char nameLog[10000];
    printf("Enter log file name: ");
    scanf(" %s", nameLog);
    //open file in log folder
    fp = fopen(nameLog, "w");

    //include date and time
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(fp, "Date: %d-%d-%d   ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    fprintf(fp, "Time: %d:%d\n", tm.tm_hour, tm.tm_min);
    
    //question until "EXIT"
    char input[10000];
    char respones[10000];
    do{
        //receive user input
        //input include " "
        scanf(" %[^\n]s", input);
        //if input is "EXIT", use strcasecmp for case insensitive strings check
        if(strcasecmp(input, "EXIT") == 0){
            break;
        }
        fprintf(fp, "User: %s\n", input);
            
        //respones include " "
        scanf(" %[^\n]s", respones);
        fprintf(fp, "Chat: %s\n", respones);
    }while(1);
    fclose(fp);

    return 0;
}
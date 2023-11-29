#include "log.h"

int checkExist(char filesList[][1000], char nameLog[], int count) {
    int alreadyExist = 0;
    for (int i = 0; i < count; i++) {
        // scanning through list of all chat file and compare with new chat name
        if (strcmp(filesList[i], nameLog) == 0) {
            alreadyExist = 1;
            break;
        }
    }
    return alreadyExist; // return 1 if the file is already exist, else 0
}

void createPath(char* nameLog) {
    // concat nameLog => log/nameLog.txt
    char path[100] = "log/";
    strcat(path, nameLog);
    strcat(path, ".txt");
    strcpy(nameLog, path);
}

void readLogFile(FILE* fp, Message *messages, int *num_messages) {
    char* role = malloc(100 * sizeof(char));
    char* content = malloc(1000 * sizeof(char));
    int index = *num_messages;
    // scanning through history of current chat
    while(fscanf(fp, "%s %[^\n]s", role, content) != EOF){
        role[strlen(role) - 1] = '\0';
        // print out the previous chat history
        printf("\n%s: %s \e[0m\n", (strcmp(role, "user") == 0) ? "\e[1;36mYou\e[0m\e[0;36m":"\e[1;35mAssistant\e[0m\e[0;35m", content);
        messages[index].role = malloc(strlen(role) + 1);
        messages[index].content = malloc(strlen(content) + 1);
        // add chat history to current chat array
        strcpy(messages[index].role, role);
        strcpy(messages[index].content, escape_json_string(content));
        index++;
    }
    *num_messages = index;
}

void writeLogFile(FILE* fp, Message *messages, int num_messages, char* user_input) {
    // save new chat to chat history
    fprintf(fp, "user, %s\n", escape_json_string(user_input));
    fprintf(fp, "assistant, %s\n", unescape_json_string(messages[num_messages].content));
}
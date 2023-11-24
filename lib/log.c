#include "log.h"

int checkExist(char filesList[][1000], char nameLog[], int count) {
    int alreadyExist = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(filesList[i], nameLog) == 0) {
            alreadyExist = 1;
            break;
        }
    }
    return alreadyExist;
}

void createPath(char* nameLog) {
    char path[100] = "log/";
    strcat(path, nameLog);
    strcat(path, ".txt");
    strcpy(nameLog, path);
}

void readLogFile(FILE* fp, Message *messages, int *num_messages) {
    char* role = malloc(100 * sizeof(char));
    char* content = malloc(1000 * sizeof(char));
    int index = *num_messages;
    while(fscanf(fp, "%s %[^\n]s", role, content) != EOF){
        role[strlen(role) - 1] = '\0';
        printf("\n%s: %s \033[0m\n", (strcmp(role, "user") == 0) ? "\033[1;36mYou\033[0m\033[0;36m":"\033[1;35mAssistant\033[0m\033[0;35m", content);
        messages[index].role = malloc(strlen(role) + 1);
        messages[index].content = malloc(strlen(content) + 1);
        strcpy(messages[index].role, role);
        strcpy(messages[index].content, content);
        //messages[index] = (Message){role, content};
        index++;
    }
    *num_messages = index;
}

void writeLogFile(FILE* fp, Message *messages, int num_messages, char* user_input) {
    fprintf(fp, "user, %s\n", escape_json_string(user_input));
    fprintf(fp, "assistant, %s\n", unescape_json_string(messages[num_messages].content));
}
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

void readLogFile(FILE* fp, Message *messages, int *num_messages, char* role, char* content) {
    while(fscanf(fp, "%s %[^\n]s", role, content) != EOF){
        role[strlen(role) - 1] = '\0';
        printf("\n%s: %s\n", (strcmp(role, "user") == 0) ? "You":"Assistant", content);
        messages[*num_messages].role = malloc(strlen(role) + 1);
        messages[*num_messages].content = malloc(strlen(content) + 1);
        strcpy(messages[*num_messages].role, role);
        strcpy(messages[*num_messages].content, content);
    }
    (*num_messages)++;
}

void writeLogFile(FILE* fp, Message *messages, int num_messages, char* user_input) {
    fprintf(fp, "user, %s\n", escape_json_string(user_input));
    fprintf(fp, "assistant, %s\n", unescape_json_string(messages[num_messages].content));
}
int checkExist(char filesList[][1000], char nameLog[], int count);
void createPath(char* nameLog);
void readLogFile(FILE* fp, Message *messages, int *num_messages);
void writeLogFile(FILE* fp, Message *messages, int num_messages, char* user_input);
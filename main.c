#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <strings.h>
#include "lib/request.c"

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
    char role[100], content[1000];
    int index = *num_messages;
    while(fscanf(fp, "%s %[^\n]s", role, content) != EOF){
        role[strlen(role) - 1] = '\0';
        printf("\n%s: %s\n", (strcmp(role, "user") == 0) ? "You":"Assistant", content);
        //strcpy(messages[index].role, role);
        //strcpy(messages[index].content, content);
        //messages[index] = (Message){role, content};
        printf("%s, %s\n", messages[index - 1].role, messages[index - 1].content);
        printf("%s, %s\n", messages[index].role, messages[index].content);
        index++;
    }
    *num_messages = index;
}

void writeLogFile(FILE* fp, Message *messages, int num_messages, char* user_input) {
    fprintf(fp, "user, %s\n", escape_json_string(user_input));
    fprintf(fp, "assistant, %s\n", unescape_json_string(messages[num_messages].content));
}

int main(void)
{
    FILE *fp, *fl;
    //open file in log folder
    char nameLog[1000], existFile[1000], fileList[100][1000];
    int count = 0, alreadyExist = 0;
    fl = fopen("filesList.txt", "a+");
    printf("\nList of previous chats:\n");
    // print exist file
    while(fscanf(fl, "%s", existFile) != EOF) {
        printf("- %s\n", existFile);
        strcpy(fileList[count], existFile);
        count++;
    }
    printf("\n");
    printf("Enter chat name: ");
    scanf("%[^\n]%*c", nameLog);
    // check if log file already exist
    if (!checkExist(fileList, nameLog, count)) {
        // if not exist save to files list
        fprintf(fl, "%s\n", nameLog);
    }
    createPath(nameLog);
    fp = fopen(nameLog, "a+");
    
    // add past conversation to array past conversation
    Message messages[100] = {
        {"system", "You are an AI assistant. You answer shortly and concisely."},
        // {"user", "Hello!"},
    };

    double temperature = 0.7;
    int num_messages = 1;

    printf("Enter a message (type \"exit\" to quit): \n");
    readLogFile(fp, messages, &num_messages);
    //char role[100], content[1000];
    // while(fscanf(fp, "%s %[^\n]s", role, content) != EOF){
    //     role[strlen(role) - 1] = '\0';
    //     printf("\n%s: %s\n", (strcmp(role, "user") == 0) ? "You":"Assistant", content);
    //     printf("KUY1\n");
    //     //strcpy(messages[*num_messages].role, role);
    //     //strcpy(messages[*num_messages].content, content);
    //     *(messages+(num_messages)) = (Message){role, content};
    //     num_messages = num_messages + 1;
    // }
    printf("%d\n", num_messages);
    for (int i = 0; i < num_messages; i++)
    {
        printf("%s %s\n", messages[i].role, messages[i].content);
    }
    while (1)
    {
        // Get the user input
        char user_input[1000];
        printf("\nYou: ");
        // scanf("%[^\n]%*c", user_input); // %[^\n] means to read until a newline is encountered, %*c means to read the newline character and discard it
        fgets(user_input, 1000, stdin);
        user_input[strlen(user_input) - 1] = '\0'; // Remove the newline character from the end of the string

        if (strcasecmp(user_input, "exit") == 0)
        {
            printf("Goodbye!\n");
            break;
        }

        // Message new_message = {"user", user_input};
        // printf("DEBUG: %s\n", escape_json_string(user_input));
        messages[num_messages] = (Message){"user", escape_json_string(user_input)};
        num_messages++;

        // Get the response data
        // printf("Thinking...\n");
        char *response = getResponseData(messages, num_messages, temperature);
        // printf("Done!\n");

        // char *response = getResponseData(messages, 2, 0.7);

        if (response != NULL)
        {
            // Extract the message from the response
            // char *message = extractMessage(response);
            // Print the response data
            // printf("Response: %s\n", response);
            // printf("\nAssistant: %s\n", unescape_json_string(message));

            messages[num_messages] = (Message){"assistant", extractMessage(response)};
            printf("\nAssistant: %s\n", unescape_json_string(messages[num_messages].content));

            // user: escape_json_string(user_input)
            // assistant: unescape_json_string(messages[num_messages].content)
            // num_messages indicates the index of message
            
            // write new chat to log file
            writeLogFile(fp, messages, num_messages, user_input);

            // Add the message to the messages array
            // messages[num_messages] = (Message){"assistant", message};
            num_messages++;

            // Cleanup the response data
            // free(message);
        }
        else
        {
            printf("Failed to get response data\n");
            // remove the last message from the messages array
            num_messages--;
        }
    }
    printf("num_messages: %d\n", num_messages - 1);
    fclose(fp);
    return 0;
}



// {
//   "id": "chatcmpl-8NLRTinqGPDMwzrYkaYDoEsYzmEcK",
//   "object": "chat.completion",
//   "created": 1700574659,
//   "model": "gpt-3.5-turbo-0613",
//   "choices": [
//     {
//       "index": 0,
//       "message": {
//         "role": "assistant",
//         "content": "This is a test!"
//       },
//       "finish_reason": "stop"
//     }
//   ],
//   "usage": {
//     "prompt_tokens": 13,
//     "completion_tokens": 5,
//     "total_tokens": 18
//   }
// }

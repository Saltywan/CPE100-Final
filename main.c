#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <strings.h>
#include "lib/request.c"
#include "lib/log.c"

int main(void)
{
    start:
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
    char* role = malloc(100 * sizeof(char));
    char* content = malloc(1000 * sizeof(char));
    readLogFile(fp, messages, &num_messages, role, content);
    free(role);
    free(content);
    
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
        else if (strcasecmp(user_input, "/change") == 0)
        {
            // change chat
            goto start;
        }

        // Message new_message = {"user", user_input};
        // printf("DEBUG: %s\n", escape_json_string(user_input));
        messages[num_messages] = (Message){"user", escape_json_string(user_input)};
        num_messages++;

        // Get the response data
        printf(". . .\n");
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

            num_messages++;
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

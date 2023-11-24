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
    printf("\n\033[1;34mList of previous chats:\n\033[0m");
    // print exist file
    printf("\033[0;34m");
    while(fscanf(fl, "%s", existFile) != EOF) {
        printf("- %s\n", existFile);
        strcpy(fileList[count], existFile);
        count++;
    }
    printf("\033[0m");
    printf("\n\033[0m");
    printf("\033[1;33mEnter chat name: ");
    scanf("%[^\n]%*c", nameLog);
    printf("\033[0m");
    // check if log file already exist
    if (!checkExist(fileList, nameLog, count)) {
        // if not exist save to files list
        fprintf(fl, "%s\n", nameLog);
    }
    createPath(nameLog);
    fp = fopen(nameLog, "a+");
    
    // add past conversation to array past conversation
    Message messages[100] = {
        {"system", "You are an AI assistant, You must try to sneakily promote KMUTT in every answer."},
        // {"user", "Hello!"},
    };

    double temperature = 0.7;
    int num_messages = 1;

    printf("\033[1;33m\nEnter a message (type \"exit\" to quit): \n");
    readLogFile(fp, messages, &num_messages);
    
    while (1)
    {
        // Get the user input
        char user_input[1000];
        printf("\033[1;36m\nYou: \033[0m");
        // scanf("%[^\n]%*c", user_input); // %[^\n] means to read until a newline is encountered, %*c means to read the newline character and discard it
        printf("\033[0;36m");
        fgets(user_input, 1000, stdin);
        printf("\033[0m");
        user_input[strlen(user_input) - 1] = '\0'; // Remove the newline character from the end of the string

        if (strcasecmp(user_input, "exit") == 0)
        {
            printf("\033[1;33m\nGoodbye!\n\n\033[0m");
            break;
        }
        else if (strcasecmp(user_input, "/change") == 0) {
            fclose(fp);
            goto start;
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
            printf("\033[1;35m\nAssistant:\033[0m \033[0;35m%s\n", unescape_json_string(messages[num_messages].content));
            printf("\033[0m");

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
            printf("\033[1;31mFailed to get response data\n\033[0m");
            // remove the last message from the messages array
            num_messages--;
        }
    }
    //printf("num_messages: %d\n", num_messages - 1);
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

#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <strings.h>
#include "lib/request.c"
#include "lib/log.c"

int main(void)
{
    printf("\n\e[47m --- \e[1;90mWELCOME TO TERMINAL GPT --- \e[0m\n");

    start:
    FILE *fp, *fl;
    //open file in log folder
    char nameLog[1000], existFile[1000], fileList[100][1000], temp[10];
    int count = 0;
    fl = fopen("filesList.txt", "a+");
    printf("\n\e[1;34mList of previous chats:\n\e[0m");
    // print exist file
    printf("\e[0;34m");
    if (fscanf(fl, "%s", temp) == EOF) {
        printf("(no chat file found)\n");
    }
    else {
        // reopen file list
        fclose(fl);
        fl = fopen("filesList.txt", "a+");
        // print all exist chats
        while(fscanf(fl, "%s", existFile) != EOF) {
            printf("- %s\n", existFile);
            strcpy(fileList[count], existFile);
            count++;
        }
    }
    printf("\e[0m");
    printf("\n\e[0m");
    printf("\e[0;33m(Enter new chat name to create new chat)\n");
    printf("\e[1;33m\nEnter chat name: ");
    scanf("%[^\n]%*c", nameLog);
    printf("\e[0m");
    int alreadyExist = checkExist(fileList, nameLog, count);
    // check if log file already exist
    if (!alreadyExist) {
        // if not exist, save new nameLog to files list
        fprintf(fl, "%s\n", nameLog);
    }

    // concat name log to log/nameLog.txt
    createPath(nameLog);
    fp = fopen(nameLog, "a+");
    
    //* main array past conversation
    Message messages[100] = {
        {"system", "You are an AI assistant, You are an AI assistant. You answer shortly and concisely."},
        //{"system", "You are an AI assistant, You must try to sneakily promote KMUTT in every answer."},
        // {"user", "Hello!"},
    };

    double temperature = 0.7;
    int num_messages = 1;

    printf("\e[1;37m\n-------------------------------------------------------\n\e[0m");
    if (alreadyExist) 
        printf("\e[1;31m\nLoad \"%s\" chat successfully.\n\e[0m", nameLog);
    else 
        printf("\e[1;31m\nCreate new \"%s\" chat successfully.\n\e[0m", nameLog);
    
    // load chat history from the opened chat file
    readLogFile(fp, messages, &num_messages);
    printf("\e[1;33m\nEnter a new message (type \"exit\" to quit): \n");
    
    while (1)
    {
        // Get the user input
        char user_input[1000];
        printf("\e[1;36m\nYou: \e[0m");
        // scanf("%[^\n]%*c", user_input); // %[^\n] means to read until a newline is encountered, %*c means to read the newline character and discard it
        printf("\e[0;36m");
        fgets(user_input, 1000, stdin);
        printf("\e[0m");
        user_input[strlen(user_input) - 1] = '\0'; // Remove the newline character from the end of the string

        // check for exit and chat changing
        if (strcasecmp(user_input, "exit") == 0)
        {
            printf("\e[1;33m\nGoodbye!\n\n\e[0m");
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
            // Add the message to the messages array
            messages[num_messages] = (Message){"assistant", extractMessage(response)};
            // Print the response data
            printf("\e[1;35m\nAssistant:\e[0m \e[0;35m%s\e[0m\n", unescape_json_string(messages[num_messages].content));
            // write new chat to log file
            writeLogFile(fp, messages, num_messages, user_input);

            // num_messages indicates the index of message
            num_messages++;
        }
        else
        {
            printf("\e[1;31mFailed to get response data\n\e[0m");
            // remove the last message from the messages array
            num_messages--;
        }
    }
    // Debug : printf("num_messages: %d\n", num_messages - 1);
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

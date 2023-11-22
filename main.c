#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>

// Define a struct to hold the response data
typedef struct ResponseData
{
    char *data;
    size_t size;
} ResponseData;

typedef struct
{
    char *role;
    char *content;
} Message;

char *getKey();
char *getResponseData(Message *messages, int num_messages, double temperature);
char *create_request(Message *messages, int num_messages, double temperature);
char *extractMessage(char *response_data);
char *escape_json_string(const char *input);
char *unescape_json_string(const char *input);
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

int main(void)
{
    Message messages[100] = {
        {"system", "You are an AI assistant. You answer shortly and concisely."},
        // {"user", "Hello!"},
    };

    double temperature = 0.7;
    int num_messages = 1;

    printf("Enter a message (type \"exit\" to quit): \n");

    while (1)
    {
        // Get the user input
        char user_input[1000];
        printf("\nYou: ");
        // scanf("%[^\n]%*c", user_input); // %[^\n] means to read until a newline is encountered, %*c means to read the newline character and discard it
        fgets(user_input, 1000, stdin);
        user_input[strlen(user_input) - 1] = '\0'; // Remove the newline character from the end of the string

        if (strcmp(user_input, "exit") == 0)
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
    return 0;
}

// Callback function to write the response data
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) // size_t is an unsigned integer type of at least 16 bit
{                                                                            // nmemb is the number of bytes to be written, size is the size of the data pointed to by ptr, userp is the pointer passed to the callback function
    size_t realsize = size * nmemb;
    ResponseData *response = (ResponseData *)userp;

    // Calculate the new total size of the response data
    size_t new_size = response->size + realsize; // -> is used to access the struct members, like a pointer, can also use (*response).size

    // Allocate memory for the new response data
    char *new_data = realloc(response->data, new_size + 1); // realloc is used to resize the memory
    if (new_data == NULL)
    {
        printf("Failed to allocate memory for response data\n");
        return 0;
    }

    // Copy the contents to the new memory location
    memcpy(new_data + response->size, contents, realsize);

    // Update the response data and size
    response->data = new_data;
    response->size = new_size;
    response->data[response->size] = '\0';

    return realsize;
}

char *getResponseData(Message *messages, int num_messages, double temperature)
{
    CURL *curl;
    CURLcode res;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if (curl)
    {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be an https:// URL if that is what should receive the
           data. */
        const char *openai_api_key = getKey();
        char auth_header[100];
        sprintf(auth_header, "Authorization: Bearer %s", openai_api_key);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, auth_header);
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        /* Now specify the POST data */
        char *post_fields = create_request(messages, num_messages, temperature);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
        // set timeout
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L); // 20 second timeout

        // Create a struct to hold the response data
        struct ResponseData response;
        response.data = malloc(1);
        response.size = 0;

        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);

        // Free the post_fields string
        free(post_fields);

        // Return the response data
        return response.data;
    }
    curl_global_cleanup();
    return NULL;
}

char *create_request(Message *messages, int num_messages, double temperature)
{
    char *request = malloc(10000); // Allocate a large enough buffer
    sprintf(request, "{\"model\": \"gpt-3.5-turbo\", \"messages\": [");

    for (int i = 0; i < num_messages; i++)
    {
        char message[10000];
        // printf("messages[%d].content: %s\n", i, messages[i].content);
        sprintf(message, "{\"role\": \"%s\", \"content\": \"%s\"}", messages[i].role, messages[i].content);
        strcat(request, message);
        if (i < num_messages - 1)
        {
            strcat(request, ", ");
        }
    }

    char temperature_str[50];
    sprintf(temperature_str, "], \"temperature\": %.1f}", temperature);
    strcat(request, temperature_str);
    // printf("request: %s\n", request);
    return request;
}

char *getKey()
{
    static char *api_key = NULL;

    if (api_key == NULL)
    {
        FILE *file = fopen("api_key.txt", "r");

        if (file != NULL)
        {
            // File exists, read API key from file
            api_key = malloc(100); // Allocate memory for API key
            fgets(api_key, 100, file);
            fclose(file);
        }
        else
        {
            // File does not exist, prompt user to create a new API key
            printf("API key not found. Please enter a new API key: ");
            api_key = malloc(100); // Allocate memory for API key
            scanf("%s", api_key);

            file = fopen("api_key.txt", "w");
            fprintf(file, "%s", api_key);
            fclose(file);
            // clear screen
            system("clear");
        }
    }

    return api_key;
}

char *extractMessage(char *response_data)
{
    // Parse the response to extract the message
    char *start = strstr(response_data, "\"content\": \"");
    // printf("start: %s\n", start);
    if (start != NULL)
    {
        start += strlen("\"content\": \"");
        // printf("start: %s\n", start);
        // printf("strlen: %d\n", strlen(start));
        char *end = start + strlen(start) - 1;
        int quote_count = 0;
        while (end >= start)
        {
            if (*end == '\"')
            {
                quote_count++;
                if (quote_count == 13)
                {
                    break;
                }
            }
            end--;
        }
        if (quote_count == 13)
        {
            size_t message_length = end - start;
            char *message = malloc(message_length + 1);
            strncpy(message, start, message_length);
            message[message_length] = '\0';
            free(response_data);
            return message;
        }
    }

    else
    {
        printf("Failed\n");
        printf("Response: %s", response_data);
        exit(1);
    }

    return NULL;
}

char *escape_json_string(const char *input)
{
    const char *p = input;
    char *output = malloc(strlen(input) * 2 + 1); // Enough space for worst case
    char *q = output;

    while (*p)
    {
        switch (*p)
        {
        case '\"':
            *q++ = '\\';
            *q++ = '\"';
            break;
        case '\\':
            *q++ = '\\';
            *q++ = '\\';
            break;
        case '\b':
            *q++ = '\\';
            *q++ = 'b';
            break;
        case '\f':
            *q++ = '\\';
            *q++ = 'f';
            break;
        case '\n':
            *q++ = '\\';
            *q++ = 'n';
            break;
        case '\r':
            *q++ = '\\';
            *q++ = 'r';
            break;
        case '\t':
            *q++ = '\\';
            *q++ = 't';
            break;
        default:
            *q++ = *p;
            break;
        }
        p++;
    }

    *q = 0;
    return output;
}

char *unescape_json_string(const char *input)
{
    const char *p = input;
    char *output = malloc(strlen(input) + 1); // Enough space for the unescaped string
    char *q = output;

    while (*p)
    {
        if (*p == '\\')
        {
            p++; // Skip the backslash
            switch (*p)
            {
            case '\"':
                *q++ = '\"';
                break;
            case '\\':
                *q++ = '\\';
                break;
            case 'b':
                *q++ = '\b';
                break;
            case 'f':
                *q++ = '\f';
                break;
            case 'n':
                *q++ = '\n';
                break;
            case 'r':
                *q++ = '\r';
                break;
            case 't':
                *q++ = '\t';
                break;
            default:
                *q++ = *p;
                break; // If it's not a special character, just copy it
            }
        }
        else
        {
            *q++ = *p;
        }
        p++;
    }

    *q = 0;
    return output;
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
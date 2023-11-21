#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>

char *getKey();

char *getResponseData();

char* extract_message(char* response_data);

// Define a struct to hold the response data
struct ResponseData {
    char *data;
    size_t size;
};

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

int main(void)
{
    char *response = getResponseData();

    if (response != NULL)
    {
        // Extract the message from the response
        char *message = extract_message(response);
        // Print the response data
        // printf("Response: %s\n", response);
        printf("Message: %s\n", message);

        // Cleanup the response data
        free(response);
        // free(message);
    }

    return 0;
}


// Callback function to write the response data
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) // size_t is an unsigned integer type of at least 16 bit
{   // nmemb is the number of bytes to be written, size is the size of the data pointed to by ptr, userp is the pointer passed to the callback function
    size_t realsize = size * nmemb;
    struct ResponseData *response = (struct ResponseData *)userp;

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

char *getResponseData()
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
        //  char *post_fields = "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"system\", \"content\": \"You are an AI assistant.\"}, {\"role\": \"user\", \"content\": \"Hello!\"}, {\"role\": \"assistant\", \"content\": \"Hi there!\"}, {\"role\": \"user\", \"content\": \"I need help with a problem.\"}], \"temperature\": 0.7}";
        char *post_fields = "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"Say this is a test!\"}], \"temperature\": 0.7}";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);

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

        // printf("Done!\n");
        // Return the response data
        return response.data;
    }
    curl_global_cleanup();
    return NULL;
}




char *getKey() {
    static char *api_key = NULL;

    if (api_key == NULL) {
        FILE *file = fopen("api_key.txt", "r");

        if (file != NULL) {
            // File exists, read API key from file
            api_key = malloc(100); // Allocate memory for API key
            fgets(api_key, 100, file);
            fclose(file);
        } else {
            // File does not exist, prompt user to create a new API key
            printf("API key not found. Please enter a new API key: ");
            api_key = malloc(100); // Allocate memory for API key
            scanf("%s", api_key);

            file = fopen("api_key.txt", "w");
            fprintf(file, "%s", api_key);
            fclose(file);
            // clear screen
            system("cls");
        }
    }

    return api_key;
}


char* extract_message(char* response_data) {
    // Parse the response to extract the message
    char *start = strstr(response_data, "\"content\": \"");
    // printf("start: %s\n", start);
    if (start != NULL) {
        start += strlen("\"content\": \"");
        // printf("start: %s\n", start);
        // printf("strlen: %d\n", strlen(start));
        char *end = start + strlen(start) - 1;
        int quote_count = 0;
        while (end >= start) {
            if (*end == '\"') {
                quote_count++;
                if (quote_count == 13) {
                    break;
                }
            }
            end--;
        }
        if (quote_count == 13) {
            size_t message_length = end - start;
            char *message = malloc(message_length + 1);
            strncpy(message, start, message_length);
            message[message_length] = '\0';
            free(response_data);
            return message;
        }
    }
    return NULL;
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
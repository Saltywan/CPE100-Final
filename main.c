#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

struct resData
{
    int index;
    struct message
    {
        char *role;
        char *content;
    } message;
} resData;

struct url_data
{
    size_t size;
    char *data;
};

char *
concat(const char *str1, const char *str2)
{
    int i = 0, j = 0;
    char *result = malloc(strlen(str1) + strlen(str2) + 1); // allow space for nul-terminator
    while (*str1)
    {
        result[i++] = *str1++; // Only increment i once and "str1" once
    }
    while (*str2)
    {
        result[i + j++] = *str2++; // Only increment j once and "str2" once
    }
    result[i + j] = '\0'; // Add required nul-terminator
    return result;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data)
{
    size_t index = data->size;
    size_t n = (size * nmemb);
    char *tmp;

    data->size += (size * nmemb);

    tmp = realloc(data->data, data->size + 1); /* +1 for '\0' */

    if (tmp)
    {
        data->data = tmp;
    }
    else
    {
        if (data->data)
        {
            free(data->data);
        }
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0;
    }

    memcpy((data->data + index), ptr, n);
    data->data[data->size] = '\0';

    return size * nmemb;
}

char *handle_url(char *user_message)
{
    CURL *curl;

    struct url_data data;
    data.size = 0;
    data.data = malloc(4096);

    if (NULL == data.data)
    {
        fprintf(stderr, "Failed to allocate memory.\n");
        return NULL;
    }

    data.data[0] = '\0';

    CURLcode res;

    curl = curl_easy_init();


    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        // curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "model=gpt-3.5-turbo&messages=[role]user&messages[content]=ILoveYourMom");
        char *post_fields = malloc(1000);
        sprintf(post_fields, "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"%s\"}], \"temperature\": 0.7}", user_message);
        // char *post_fields = "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"Say this is a test!\"}], \"temperature\": 0.7}";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
        printf("%s\n", post_fields);
        // free(post_fields);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

        struct curl_slist *headers = NULL;
        const char *openai_api_key = "";
        char auth_header[100];
        sprintf(auth_header, "Authorization: Bearer %s", openai_api_key);
        headers = curl_slist_append(headers, auth_header);
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);
        free(post_fields);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
    return data.data;
}

int main(int argc, char *argv[])
{
    char *data;

    data = handle_url("Hello World!"); // Input message here!

    if (data)
    {
        printf("%s\n", data);
        free(data);
    }

    return 0;
}
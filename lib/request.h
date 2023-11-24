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
char *getResponseData(Message* messages, int num_messages, double temperature);
char* create_request(Message* messages, int num_messages, double temperature);
char* extractMessage(char* response_data);
char* escape_json_string(const char* input);
char* unescape_json_string(const char* input);
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
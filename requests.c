#include "helpers.h"
#include "requests.h"

char *compute_delete_request(char *host, char *url, char* access_token, 
                             char **cookies, int cookies_count) 
{

    char *message = calloc(4096, sizeof(char));
    char *line = calloc(1000, sizeof(char));

    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    memset(line, 0, 1000);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (access_token != NULL) {
        memset(line, 0, 1000);
        sprintf(line, "Authorization: Bearer %s", access_token);
        compute_message(message, line);
    }

    if (cookies != NULL) {
        memset(line, 0, 1000);
        strcpy(line, "Cookie: ");
        for (int i = 0; i < cookies_count - 1; i++) {
            strcat(line, cookies[i]);
            strcat(line, ";");
        }
        strcat(line, cookies[cookies_count - 1]);
        compute_message(message, line);
    }

    compute_message(message, "");
    free(line);
    return message;
}

char *compute_get_request(char *host, char *url, char *query_params, 
                        char **cookies, char *token , int cookies_count)
{
    char *message = calloc(4096, sizeof(char));
    char *line = calloc(1000, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        memset(line, 0, 1000);
        strcat(line, "Cookie: ");

        for (int i = 0; i < cookies_count - 1; i++) {
            strcat(line, cookies[i]);
            strcat(line, ";");
        }

        strcat(line, cookies[cookies_count - 1]);
        compute_message(message, line);
    }

    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line); 
    }
    
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *body_data
                           , char *cookies, char *token)
{
    char *message = calloc(4096, sizeof(char));
    char *line = calloc(1000, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %ld", strlen(body_data));
    compute_message(message, line);

    // Step 4 (optional): add cookies
    if (cookies != NULL) {
        
        compute_message(message, line); 
    }

    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line); 
    }
    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    memset(line, 0, 1000);
    compute_message(message, body_data);

    free(line);
    return message;
}
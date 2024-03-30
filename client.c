#include "helpers.h"
#include <stdbool.h>



char *token;
char *response;
char *cookie;
/* Verific daca id ul dat e  nr */

char isNumber1(const char *text) {
    int length = strlen(text);
    for (int i = 0; i < length; i++) {
        if (!isdigit(text[i]))
            return 0;
    }
    return 1;
}



bool is_substr1(const char *str, const char *sub) {
    size_t sub_size = strlen(sub);
    
    while (*str != '\0') {
        size_t num_matches = 0;
        
        while (*sub == *str && *sub != '\0') {
            num_matches++;
            sub++;
            str++;
        }
        
        if (num_matches == sub_size) {
            return true;
        }
        
        str = str - num_matches + 1;
    }
    
    return false;
}

//Extrag cookie urile pt comanda 
void extract_cookies() {
    char *rest = response; 
    char *cookies = strtok_r(rest, "\n", &rest);

   	while (cookies != NULL)  {
    	if (is_substr1(cookies , "Set-Cookie:")) {
        	cookie = strtok(cookies, ";");
        	cookie = strtok(cookie, ":");
        	cookie = strtok(0, " ");
        	break;
		}
        cookies = strtok_r(rest, "\n", &rest);
   	}
}

//Extrag tokenul de acces
void extract_tokens() {
    char* rest = response; 
    char *tokens = strtok_r(rest, "\n", &rest);

   	while (tokens != NULL)  {
    	if (is_substr1(tokens , "token")) {
        	token = strtok(tokens, ":");
        	token = strtok(0, "\"");
        	break;
		}
        tokens = strtok_r(rest, "\n", &rest);
   	}
}


int main(int argc, char **argv)
{   
        // user info
    char username[100];
    char password[100];

        // book  info 
    

    int is_logout = 0;
    int running = 1;

    char *message;    
    int socket;
    int login_flag = 0;
    int entered_library = 0;

    while(1) {
        char command[50];
        fgets(command, 50, stdin);
        command[strlen(command) - 1] = '\0';
        
        // register
        if (strcmp(command, "register") == 0) {
            printf("username=");
            fgets(username, 4096, stdin);
            username[strlen(username) - 1] = '\0';
            DIE(strcmp(username,"\n") == 0 , "Invalid usename");

            printf("password=");
            fgets(password, 4096, stdin);
            password[strlen(password) - 1] = '\0';
            DIE(strcmp(password,"\n") == 0 , "Invalid password");
            JSON_Value *value = json_value_init_object();
            JSON_Object *object = json_value_get_object(value);

            json_object_set_string(object, "username", username);
            json_object_set_string(object, "password", password);

            char *str = NULL;

            str = json_serialize_to_string_pretty(value);
            message = compute_post_request((char*)"34.254.242.81", ROUTE_REGISTER,PAYLOAD, str, NULL, NULL);
                                
            socket = open_connection((char*)"34.254.242.81", PORT, AF_INET, SOCK_STREAM, 0);
            send_to_server(socket, message);
            response = receive_from_server(socket);
            if (strstr(response, "HTTP/1.1 201 Created") != NULL) {
                printf("Successfully registered!\n");
                close_connection(socket);
            } else {
                printf("Register error! The name is already taken!\n");
            }
        }
        
        
        // login
         else if (strcmp(command , "login") == 0) {
            if (login_flag == 1) {
    	        printf("You are already logged!\n");
                printf("Please logout if you want login with another account!\n\n");
                continue;
            }
            
            printf("username=");
            fgets(username, 4096, stdin);
            username[strlen(username) - 1] = '\0';
            DIE(strcmp(username,"\n") == 0 , "Invalid usename");

            printf("password=");
            fgets(password, 4096, stdin);
            password[strlen(password) - 1] = '\0';
            DIE(strcmp(password,"\n") == 0 , "Invalid password");

            JSON_Value *value = json_value_init_object();
            JSON_Object *object = json_value_get_object(value);

            json_object_set_string(object, "username", username);
            json_object_set_string(object, "password", password);

            char *str = NULL;

            str = json_serialize_to_string_pretty(value);
            message = compute_post_request((char*)"34.254.242.81", ROUTE_LOGIN,PAYLOAD, str, NULL, NULL);

            socket = open_connection((char*)"34.254.242.81", PORT, AF_INET, SOCK_STREAM, 0);

            send_to_server(socket, message);
            response = receive_from_server(socket);
    
            if (strstr(response, "HTTP/1.1 200 OK") != NULL) {
                printf("Successfully logged!\n");
                extract_cookies();
                close_connection(socket);
                login_flag = 1;
            } else {
                printf("Login error!\n");
            }

        } 
        

        // enter_library 
        else if (strcmp(command , "enter_library") == 0) {
            if (login_flag == 0 && entered_library == 0) {
                printf("Not logged in!\n");
                printf("Please log in first !\n");
                continue;
            }

            if (login_flag == 1 && entered_library == 1) {
                printf("You are already in library!\n");
                }
            message = compute_get_request(HOST, ENTER_LIBRARY,NULL, &cookie, NULL, 1);
            socket = open_connection((char*)"34.254.242.81", PORT, AF_INET, SOCK_STREAM, 0);
            send_to_server(socket, message);
    
            response = receive_from_server(socket);
            if (strstr(response, "HTTP/1.1 200 OK") != NULL) {
                printf("Successfully enter_library_command!\n");
                extract_tokens();
                close_connection(socket);
                entered_library = 1;
            } else {
                printf("Enter_library_command error!\n");
            }
        }
        

        // get_books
        else if (strcmp(command , "get_books") == 0) {
            if (login_flag == 0 || entered_library == 0) {
                printf("You do not have acces!\n");
                continue;
            }

            message = compute_get_request(HOST, ROUTE_BOOKS,NULL , NULL , token , 0);
            socket = open_connection((char*)"34.254.242.81", PORT, AF_INET, SOCK_STREAM, 0);
            send_to_server(socket, message);
            response = receive_from_server(socket);
            if (strstr(response, "HTTP/1.1 200 OK") != NULL) {
                printf("Successfully got books\n");
                close_connection(socket);
            } else {
            printf("Get_books error!\n");
            }
            response = strchr(response , '[');

            printf("\n%s\n", response);

        } 
        

        //add_book
        else if (strcmp(command, "add_book") == 0) {
            
            if (login_flag == 0 || entered_library == 0) {
                printf("You do not have acces!\n");
                continue;
            }

            char title[4096];
            char author[4096];
            char genre[4096];
            char publisher[4096];
            char page_count[4096];

            printf("title=");
            fgets(title, 4096, stdin);
            title[strlen(title) - 1] = '\0';

            printf("author=");
            fgets(author, 4096, stdin);
            author[strlen(author) - 1] = '\0';

            printf("publisher=");
            fgets(publisher, 4096, stdin);
            publisher[strlen(publisher) - 1] = '\0';

            printf("genre=");
            fgets(genre, 4096, stdin);
            genre[strlen(genre) - 1] = '\0';


            int not_number = 1;
            printf("page_count=");
            fgets(page_count, 4096, stdin);
            page_count[strlen(page_count) - 1] = '\0';
            
            JSON_Value *value = json_value_init_object();
            JSON_Object *object = json_value_get_object(value);

            json_object_set_string(object, "title", title);
            json_object_set_string(object, "author", author);
            json_object_set_string(object, "genre", genre);
            json_object_set_string(object, "publisher", publisher);
            json_object_set_string(object, "page_count", page_count);
            char *str = NULL;
            str = json_serialize_to_string(value);

            message = compute_post_request((char*)"34.254.242.81" , ROUTE_BOOKS , PAYLOAD,str, NULL , token);
    
            socket = open_connection((char*)"34.254.242.81", PORT, AF_INET, SOCK_STREAM, 0);
            send_to_server(socket , message);
            response = receive_from_server(socket);

            if (strstr(response, "HTTP/1.1 200 OK") != NULL) {
                printf("Successfully add_book!\n");
                close_connection(socket);
            } else {
                printf("Add_book error!\n");
            }

        }
        

        //get_book
         else if(strcmp(command, "get_book") == 0) {
            if (login_flag == 0 || entered_library == 0) {
                printf("You do not have acces!\n");
                continue;
            }
            char id[4096];
            printf("id=");
            fgets(id, 4096, stdin);
            id[strlen(id) - 1] = '\0';

            char str[100];
            strcpy(str , ROUTE_BOOKS);
            strcat(str , "/");
            strcat(str , id);

            message = compute_get_request(HOST , str , NULL , NULL , token , 0);
    
            socket = open_connection((char*)"34.254.242.81", PORT, AF_INET, SOCK_STREAM, 0);
            send_to_server(socket, message);
            response = receive_from_server(socket);
           
            if (strstr(response, "HTTP/1.1 200 OK") != NULL) {
                char *book = strchr(response , '{');
                printf("%s\n",book);
                close_connection(socket);
            } else {
                printf("Get_book error!\n");
            }

            
        }

        //delete_book
         else if (strcmp(command, "delete_book") == 0) {
            if (login_flag == 0 || entered_library == 0) {
                printf("You are not authorized!\n");
                continue;
            }
            char  id[4096];

            printf("id=");
            fgets(id, 4096, stdin);
            id[strlen(id) - 1] = '\0';

            if(isNumber1(id) == 0) {
                printf("Invalid id!\n");
                printf("id=");
                fgets(id, 4096, stdin);
                id[strlen(id) - 1] = '\0';
            }

            char str[100];
            strcpy(str , ROUTE_BOOKS);
            strcat(str , "/");
            strcat(str , id);

            message = compute_delete_request(HOST , str , token , NULL , 0);
            socket = open_connection((char*)"34.254.242.81", PORT, AF_INET, SOCK_STREAM, 0);
            send_to_server(socket, message);
            response = receive_from_server(socket);
            if (strstr(response, "HTTP/1.1 200 OK") != NULL) {
                printf("Successfully delete_book!\n");
                close_connection(socket);
            } else {
                printf("Delete_book error!\n");
            }
        } 
        
        
        //logout
        else if (strcmp(command, "logout") == 0) {
            if (is_logout == 1) {
            printf("You are logged out!\n");
                continue;            
            }
            message = compute_get_request(HOST, ROUTE_LOGOUT ,NULL, &cookie , NULL , 1);
            socket = open_connection((char*)"34.254.242.81", PORT, AF_INET, SOCK_STREAM, 0);
            send_to_server(socket, message);
            response = receive_from_server(socket);
            if (strstr(response, "HTTP/1.1 200 OK") != NULL) {
                printf("Successfully logout\n");
                cookie = NULL;
                token = NULL;
                close_connection(socket);
                login_flag = 0;
                entered_library = 0;
            } else {
                printf("Logout error!\n");
            }

        is_logout = 1;
    
        }

        //exit    
         else if (strcmp(command,"exit") == 0) {
            break;
        }
        
         else {
            printf("Invalid command! Try another one!\n\n");
        }
    }
    return 0;
}

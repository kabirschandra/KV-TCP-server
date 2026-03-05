#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../commands.h"
#include "../message.h"

#define VERSION 1
#define PORT 9000 //Common for TCP
#define MAX_MESSAGE_LENGTH 64000

/** connect_server
 * 
 * @brief Connect to an ip supplied in *request (IPV4 exclusive)
 *        Renews connection if previously connected
 * 
 * @param request :: Input request
 * 
 * @return bool :: Indication of if connection was successful
 */
bool connect_server(ClientRequest *request) {

    if(!request->field1) {
        printf("No IP address provided for connection\n");
        return false;
    }

    if(request->socketFd >= 0) { //Renew connection
        close(request->socketFd);
    }

    int socketFd = socket(AF_INET, SOCK_STREAM, 0); //IPV4, TCP
    if(socketFd < 0) {
        printf("Socket fd initialiser failed\n");
        return false;
    }
    struct sockaddr_in addr = {0}; //Zero struct
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, request->field1, &addr.sin_addr) != 1) { //IP
        printf("Bad IP '%s'\n", request->field1);
        close(socketFd);
        return false;
    }

    if(connect(socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Connection to '%s' failed\n", request->field1);
        close(socketFd);
        return false;
    }
    request->socketFd = socketFd;

    printf("Connected to '%s'\n", request->field1);

    return true;
}


/** terminate_connection
 * 
 * @brief Terminate connection to server
 * 
 * @param request :: Request
 * 
 * @return void :: None
 */
void terminate_connection(ClientRequest *request) {

    if(request->socketFd >= 0) {
        close(request->socketFd);
        request->socketFd = -1;
        printf("Connection closed\n");
    } else {
        printf("No active connection\n");
    }

    return;
}


/** send_request
 * 
 * @brief Handle insertion, deletion, rehashing and editing
 * 
 * @param request :: Request
 * 
 * @return bool :: Indication of successful insertion (true == inserted)
 */
bool send_request(ClientRequest *request) {

    if(request->socketFd == -1) {
        printf("No active connection\n");
        return false;
    }
   
    //Send sizes of keys/values then send actual data
    if(send(request->socketFd, &(request->metadata), sizeof(request->metadata), 0) != sizeof(request->metadata)) {
        printf("Failed to send data\n");
        return false;
    }
    //Send key
    if(send(request->socketFd, request->field1, request->metadata.field1Length, 0) != request->metadata.field1Length) {
        printf("Failed to send key data\n");
        return false;
    }
    //Send value
    if(send(request->socketFd, request->field2, request->metadata.field2Length, 0) != request->metadata.field2Length) {
        printf("Failed to send data\n");
        return false;
    }
    //Send size_t 
    if(send(request->socketFd, request->field3, sizeof(size_t), 0) != sizeof(size_t)) {
        printf("Failed to send numeric data\n");
        return false;
    }


    ServerResponse response;
    if(recv(request->socketFd, &response, sizeof(response), 0) != sizeof(response)) {
        printf("Invalid response recieved from server\n");
        return false;
    }
    if(response.response == SUCCESS) {
        return true;
    } else {
        printf("Server inserton failed\n");
        return false;
    }
}





/** parse_input
 * 
 * @brief Parse an input command and output a corrosponding requestOut
 * 
 * @param input :: Input command string
 * @param requestOut :: Corrosponding output request to input
 * 
 * @return bool :: Indication of if parsing occured successfully
 */
bool parse_input(char *input, ClientRequest *requestOut) {
    
    requestOut->field1 = NULL;
    requestOut->metadata.field1Length = 0;
    requestOut->field2 = NULL;
    requestOut->metadata.field2Length = 0;
    requestOut->field3 = 0;

    requestOut->metadata.version = VERSION;

    char *operation = strtok(input, " \n");
    if(!operation) {
        //No operation passed
        return false;
    }

    if(strcmp(operation, "connect") == 0) {
        requestOut->metadata.request = CONNECT;
        requestOut->field1 = strtok(NULL, " \n"); //ip

        if(!requestOut->field1) {
            return false;
        }
        requestOut->metadata.field1Length = strlen(requestOut->field1) + 1;

    } else if(strcmp(operation, "terminate") == 0) {
        requestOut->metadata.request = TERMINATE;

    } else if(strcmp(operation, "insert") == 0) {
        requestOut->metadata.request = INSERT;
        requestOut->field1 = strtok(NULL, " \n"); //key

        requestOut->field2 = strtok(NULL, " \n"); //value
        if(!requestOut->field1 || !requestOut->field2) {
            return false;
        }
        requestOut->metadata.field1Length = strlen(requestOut->field1) + 1;
        requestOut->metadata.field2Length = strlen(requestOut->field2) + 1;

    } else if(strcmp(operation, "find") == 0) {
        requestOut->metadata.request = FIND;
        requestOut->field1 = strtok(NULL, " \n"); //key

        if(!requestOut->field1) {
            return false;
        }
        requestOut->metadata.field1Length = strlen(requestOut->field1);

    } else if(strcmp(operation, "rehash") == 0) {
        requestOut->metadata.request = REHASH;
        requestOut->field1 = strtok(NULL, " \n"); //new size
        if(!requestOut->field1) {
            return false;
        }
        //Check if conversion to size_t is valid

        char *end = NULL;
        requestOut->field3 = strtoul(requestOut->field1, &end, 10);
        if(end == requestOut->field1 || *end != '\0') { //Invalid size passed
            return false;
        }
        requestOut->metadata.field1Length = strlen(requestOut->field1);

    } else if(strcmp(operation, "editv") == 0) {
        requestOut->metadata.request = EDIT_VALUE;
        requestOut->field1 = strtok(NULL, " \n"); //key

        requestOut->field2 = strtok(NULL, " \n"); //value

        if(!requestOut->field1 || !requestOut->field2) {
            return false;
        }
        requestOut->metadata.field1Length = strlen(requestOut->field1);
        requestOut->metadata.field2Length = strlen(requestOut->field2);

    } else if(strcmp(operation, "delete") == 0) {
        requestOut->metadata.request = DELETE;
        requestOut->field1 = strtok(NULL, " \n"); //key

        if(!requestOut->field1) {
            return false;
        }
        requestOut->metadata.field1Length = strlen(requestOut->field1);

    } else {
        return false;
    }
    if(requestOut->metadata.field1Length > MAX_MESSAGE_LENGTH ||
    requestOut->metadata.field2Length > MAX_MESSAGE_LENGTH) {
        printf("Exceeded send length of fields\n");
        return false;
    }


    return true;
}


int main(void) {

    char arr[] = "rehash dsa";
    ClientRequest req;

    if(!parse_input(arr, &req)) {
        return -1;
    }

    printf("%d, %s, %s, %zu\n", req.metadata.request, req.field1, req.field2, req.field3);

    return 0;
}



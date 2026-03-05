#ifndef MESSAGE_H
#define MESSAGE_H
#include "commands.h"
#include <stdint.h>

typedef struct ClientMetadata {
    REQUEST request; //Command issued (SET, GET, etc)
    uint8_t version; //Version of client
    size_t field1Length; //Length of field1
    size_t field2Length; //Length of field2
} ClientMetadata;


typedef struct ClientRequest {
    ClientMetadata metadata;
    char *field1; //field1 data (e.g key, ip)
    char *field2; //field2 data (e.g value)
    size_t field3; //size_ts
    int socketFd;
} ClientRequest;


typedef struct ServerResponse {
    char *field1; //field1 data
    size_t field1Length; //Length of field1
    size_t hash; //Hash of key (if required)
    RESPONSE response; //Success, failure, etc    
    uint8_t version; //Version of server 

} ServerResponse;


#endif

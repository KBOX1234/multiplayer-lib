#pragma once


#include <stdint.h>

#include "enet/enet.h"

//this packet must be packed to be sent
typedef struct{
    uint16_t type;

    char* data;

    size_t size;

} v_packet;


//network ready packet
typedef struct{
    char* packet_buffer;

    size_t buffer_size;
} s_packet;

typedef struct{
    uint64_t id;
}client_data;

//client manager 
typedef struct{
    
    size_t client_count;

    ENetPeer** clients;

    ENetHost *server;


 
} client_manager;

typedef enum{
    GOOD,
    BAD,
    CLIENT_NOT_FOUND,
    SERVER_NO_START,
    ALLOCATION_ERROR
} STATUS;

extern client_manager client_master;

//init
int init(const char* ip_addr, int port, int client_max);

//adds a client
int add_client(ENetEvent* event, uint64_t id);

//removes a client
int remove_client(uint64_t id);

//simple function to send a packet
int simple_send_client(uint64_t id, s_packet* packet_p);

//simple function to broadcast packet
int simple_broadcast(s_packet* packet_p);

int server_scan_event();

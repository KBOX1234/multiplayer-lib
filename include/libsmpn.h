#pragma once



#include <stdint.h>
#include <stdbool.h>
//trying to stop windows.h from poluting the namespace
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI             // Prevent inclusion of GDI (Graphics Device Interface), including Rectangle()
#define NOSYSMETRICS
//#define NOMINMAX 1
#define NOUSER
#define NOCOMM
#define NOIME
#define NOSERVICE
#define NOMCX
#endif
#include "enet/enet.h"

//this packet must be packed to be sent
typedef struct{
    uint16_t type;
    uint16_t size;
    unsigned char* data;
} v_packet;


//network ready packet
typedef struct{
    unsigned char* packet_buffer;
    
    //adds + 2 to account for meta data at the beginning
    uint16_t buffer_size;
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

//client connection to server
typedef struct{
	ENetHost* myself;

	ENetPeer* remote_server;
} server_connector;

typedef enum{
    GOOD,
    BAD,
    CLIENT_NOT_FOUND,
    SERVER_NO_START,
    ALLOCATION_ERROR
} STATUS;

typedef enum{
    TARGETED_P,
    BROADCAST_P
} sig_type_net;


typedef void (*incomming_packet_handler)(s_packet* packet_p, uint64_t client_id);

extern incomming_packet_handler ipacked_handle;

extern client_manager client_master;

extern server_connector sc_manager;

/*
 * Server Stuff
 */

#ifdef __cplusplus
extern "C" {
#endif


//init
int init_mpn_server(const char* ip_addr, int port, int client_max);

//adds a client
int add_client(ENetEvent* event, uint64_t id);

//removes a client
int remove_client(uint64_t id);

//simple function to send a packet
int simple_send_client(uint64_t id, s_packet* packet_p);

//simple function to broadcast packet
void simple_broadcast(s_packet* packet_p);

//scan for incomming packets
//cooldown_ml is the amount of time to wait in miliseconds
void server_scan_event(int cooldown_ms);

//kick a specific client
void kick_client(uint64_t client_id, bool gracefull);

//kick all clients
void kick_all_clients(bool gracefull);

/*
 *
 *Client Stuff
 *
*/

int init_mpn_client(const char* ip_addr, int port);

int simple_send_to_server(s_packet* packet_p);

//cooldown_timer_ml is the amount of miliseconds to wait for a response
void scan_for_incomming_packets(int cooldown_timer_ms);

void leave_server();

/*
 *Universal Stuff
*/

void set_incomming_packet_handler(incomming_packet_handler iph);

s_packet serialize_packet(v_packet* packet_p);

v_packet deserialize_packet(s_packet* packet_p);

void destroy_serialized_packet(s_packet* packet_p);

void destroy_deserialized_packet(v_packet* packet_p);


#ifdef __cplusplus
}
#endif

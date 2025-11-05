#pragma once


#include <stdint.h>
#include <stdbool.h>

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
typedef struct client_manager client_manager;

//client connection to server
typedef struct server_connector server_connector;

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

typedef enum{
    CONNECT_SIG,
    DISCONNECT_SIG
} status_sigs;


typedef void (*incomming_packet_handler)(s_packet* packet_p, uint64_t client_id);

typedef void (*incomming_status_sig_handler)(int type, uint64_t client_id);

extern incomming_packet_handler ipacked_handle;

extern incomming_status_sig_handler istatus_sig_handle;

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

//adds a client (internal)
int add_client(void* event, uint64_t id);

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

//gets a pointer to an array of client ids use get_client_count() - 1 to get the array size
uint64_t* get_client_ids();

//gets the amount of clients with the first client starting at 1
int get_client_count();

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

void set_status_sig_handler(incomming_status_sig_handler issh);

s_packet serialize_packet(v_packet* packet_p);

v_packet deserialize_packet(s_packet* packet_p);

void destroy_serialized_packet(s_packet* packet_p);

void destroy_deserialized_packet(v_packet* packet_p);


#ifdef __cplusplus
}
#endif

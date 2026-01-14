#pragma once


#include <stdint.h>
#include <stdbool.h>

//v_packet is a packet that is what is ment to be used to prepare data for sending.
/*! \brief 
 * The v_packet data type can take a pointer to some data.
 * You must also specify the size of the data that you are passing.
 * It is safe to pass a pointer to an stack variable, just keep the lifetime of it in mind.
 * The type can be used to specify the type of data you are sending. It is compleatly open to the dev to use and is compleatly optional.
 * I put it there to make specifying what data is being sent easyer so you don't have to do as much typecasting.
 */
typedef struct{
    //type can be anything that the dev wants.
    uint16_t type;
    //size is the length of data.
    uint16_t size;
    //raw data buffer.
    unsigned char* data;
} v_packet;


//s_packet is the packaged varient of v_packet.
/*! \brief 
 * This is the serialized varient of v_packet. You as the dev shouldnt directly manipulate this, (unless you want to).
 * This basicly is a v_packet, but all the data is smooshed and copied into a flat buffer.
 * Because it is now a flat buffer, it can be easily sent over the network.
 */
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

#define CHANNEL_COUNT 2

//Status macros for functions.
typedef enum{
    GOOD,
    BAD,
    CLIENT_NOT_FOUND,
    SERVER_NO_START,
    ALLOCATION_ERROR
} STATUS;

//Signal types.
typedef enum{
    TARGETED_P,
    BROADCAST_P
} sig_type_net;

//Signal macros.
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


//Starts the server.
/*! \brief 
 * @param ip_addr This is the ip address of the server, since this is the server, usialy just set this to "0.0.0.0".
 * @param port This is the port for the server to bind to.
 * @param client_max This is the maximum amount of clients that are allowed to connect to the server at any time.
 */
int init_mpn_server(const char* ip_addr, int port, int client_max);

//Adds a client (internal)
int add_client(void* event, uint64_t id);

//Removes a client (internal)
int remove_client(uint64_t id);

//Simple function to send a packet to a client.
/*! \brief 
 * @param id This is the client id. (The id of the client).
 * @param packet_p This is the packet you want to send.
 */
int simple_send_client(uint64_t id, s_packet* packet_p);

//Simple function to broadcast generic packets to all clients;
void simple_broadcast(s_packet* packet_p);

//Scan for incomming packets.
void server_scan_event(int cooldown_ms);

//Kick a specific client.
void kick_client(uint64_t client_id, bool gracefull);

//Kick all clients.
void kick_all_clients(bool gracefull);

//Gets a pointer to an array of connected clients.
uint64_t* get_client_ids();

//Gets the amount of clients.
int get_client_count();

/*
 *
 *Client Stuff
 *
*/

//Initialises a connects to a server as a client.
int init_mpn_client(const char* ip_addr, int port);

//Simple command to send data to a server.
int simple_send_to_server(s_packet* packet_p);

//Cooldown_timer_ml is the amount of miliseconds to wait for a response
void scan_for_incomming_packets(int cooldown_timer_ms);

//The clean way to leave a server.
void leave_server();

/*
 *Universal Stuff
*/


//Sets the function that handles packets being sent to it.
void set_incomming_packet_handler(incomming_packet_handler iph);

//Sets the function that handles different signals from client or server.
void set_status_sig_handler(incomming_status_sig_handler issh);

//Turns a v_packet into a serialised packet (s_packet) that can be sent.
s_packet serialize_packet(v_packet* packet_p);

//Turns a s_packet into a de-serialised packet (v_packet).
v_packet deserialize_packet(s_packet* packet_p);

//Frees the data inside a s_packet.
void destroy_serialized_packet(s_packet* packet_p);

//Frees the data inside a v_packet (!not recamonded!)
void destroy_deserialized_packet(v_packet* packet_p);


#ifdef __cplusplus
}
#endif

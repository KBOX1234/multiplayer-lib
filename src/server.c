#include "../include/libsmpn.h"
#include "../include/smpn_helper.h"
#include <enet/enet.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

//client manager 
typedef struct client_manager{
    
    size_t client_count;

    ENetPeer** clients;

    ENetHost *server;


 
} client_manager;

client_manager client_master;
incomming_packet_handler ipacked_handle = NULL;
incomming_status_sig_handler istatus_sig_handle = NULL;


int init_mpn_server(const char* ip_addr, int port, int client_max){
    client_master.client_count = 0;
    client_master.clients = NULL;

    ENetAddress address = {};
	  client_master.server = NULL;

    address.host = ENET_HOST_ANY;

    address.port = port;

  
    client_master.server = enet_host_create(&address /* the address to bind the server host to */,
        client_max			/* allow up to 32 clients and/or outgoing connections */,
	      2	/* 2 channels for targeted incom and brodcasting*/,
	      0			/* assume any amount of incoming bandwidth */,
	      0			/* assume any amount of outgoing bandwidth */);
    
    if(client_master.server == NULL){
        return SERVER_NO_START;
    }
    
    return GOOD;
}

int add_client(void* event_v, uint64_t id){

    ENetEvent* event = (ENetEvent*)event_v;

    if(event == NULL) return BAD;

    if(client_master.client_count == 0){
        client_master.client_count++;
        
        client_master.clients = malloc(sizeof(ENetPeer*));
        
    }

    else{

        client_master.client_count++;

        ENetPeer** tmp_ptr = (ENetPeer**)realloc(client_master.clients, sizeof(ENetPeer*) * client_master.client_count) ;
        
        if(tmp_ptr == NULL) return ALLOCATION_ERROR;

        client_master.clients = tmp_ptr;

    }
    
    client_master.clients[client_master.client_count - 1] = event->peer;

    client_data* client_d = malloc(sizeof(client_data));

    client_d->id = id;
       
    event->peer->data = client_d; 

    return GOOD;

}

int remove_client(uint64_t id){
    int index = yoink_index_enet_peer_by_id(id);
    
    if(index == BAD_INT) return CLIENT_NOT_FOUND;  

    int latter_tmp_buff_size = (client_master.client_count - index) * sizeof(ENetPeer*);

    ENetPeer** latter_tmp_buff = malloc(latter_tmp_buff_size);

    memcpy(latter_tmp_buff, &client_master.clients[index + 1], latter_tmp_buff_size- 1);

    memcpy(&client_master.clients[index], latter_tmp_buff, latter_tmp_buff_size - 1);

	client_master.client_count--;

    return GOOD;
}


int simple_send_client(uint64_t id, s_packet* packet_p){
    ENetPacket *packet_e = enet_packet_create(packet_p->packet_buffer, packet_p->buffer_size, ENET_PACKET_FLAG_RELIABLE);

    ENetPeer* client = yoink_enet_peer_by_id(id);
    
    if(client == NULL) return CLIENT_NOT_FOUND;

    enet_peer_send(client, TARGETED_P, packet_e);

    return GOOD;
}


void simple_broadcast(s_packet* packet_p){
    ENetPacket* packet_e = enet_packet_create(packet_p->packet_buffer, packet_p->buffer_size, ENET_PACKET_FLAG_RELIABLE);
    
    for(int i = 0; i < client_master.client_count; i++){
        ENetPeer* sp = client_master.clients[i];

        if(sp != NULL){
            enet_peer_send(sp, BROADCAST_P, packet_e);
        }
    }  
}

void server_scan_event(int cooldown_ms){
    ENetEvent event = {};

    while(enet_host_service(client_master.server, &event, cooldown_ms) > 0){
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:{
        
                srand(time(NULL));

                uint64_t new_id = rand();
                
                printf("(SERVER): New Client with id: %lu\n", new_id);

                add_client(&event, new_id);

                if(istatus_sig_handle != NULL){
                    istatus_sig_handle(CONNECT_SIG, new_id);
                }

                break;
            }

			      case ENET_EVENT_TYPE_RECEIVE:{
				
				        if(ipacked_handle != NULL){
					
					          s_packet* new_packet = malloc(sizeof(s_packet));

				            new_packet->packet_buffer = event.packet->data;
					          new_packet->buffer_size = event.packet->dataLength;

                    client_data* cd = (client_data*)event.peer->data;

					          ipacked_handle(new_packet, cd->id);

					          free(new_packet);
				        }

				        else{
					          printf("(SERVER): no packet handler set\n");
				        }

				        break;
			      }


			      case ENET_EVENT_TYPE_DISCONNECT:{
				        client_data* cd = (client_data*)event.peer->data;

				        uint64_t r_id = cd->id;

				        printf("(SERVER): Client with id %lu disconnected\n", r_id);

				        remove_client(r_id);

                if(istatus_sig_handle != NULL){
                    istatus_sig_handle(DISCONNECT_SIG, r_id);
                }

				        break;
			      }

			      case ENET_EVENT_TYPE_NONE:{
				        break;
			      }

        }
    }
}

void kick_client(uint64_t client_id, bool gracefull){
    ENetPeer* peer_p = yoink_enet_peer_by_id(client_id);

    if(gracefull == true) enet_peer_disconnect(peer_p, 0);
    else enet_peer_disconnect_now(peer_p, 0);
}

void kick_all_clients(bool gracefull){
    for(int i = 0; i < client_master.client_count; i++){
        ENetPeer* peer_p = client_master.clients[i];
        if(gracefull == true) enet_peer_disconnect(peer_p, 0);
        else enet_peer_disconnect_now(peer_p, 0);

    }
}

uint64_t* get_client_ids(){
    uint64_t* ids = malloc(client_master.client_count * sizeof(uint64_t));

    for(unsigned int i = 0; i < client_master.client_count; i++){
        client_data* cd = (client_data*)client_master.clients[i]->data;
        ids[i] = cd->id;
    }

    return ids;
}

int get_client_count(){
    return (int)client_master.client_count;
}

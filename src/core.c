#include "../include/libmpn.h"
#include "../include/helper.h"
#include <enet/enet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int init(const char* ip_addr, int port, int client_max){
    client_master.client_count = 0;
    client_master.clients = NULL;

    ENetAddress address = {};
	  client_master.server = NULL;

    address.host = ENET_HOST_ANY;

    address.port = port;

    client_master.server = enet_host_create(&address /* the address to bind the server host to */,
        client_max			/* allow up to 32 clients and/or outgoing connections */,
	      1	/* allow up to 2 channels to be used, 0 and 1 */,
	      0			/* assume any amount of incoming bandwidth */,
	      0			/* assume any amount of outgoing bandwidth */);
    
    if(client_master.server == NULL){
        return SERVER_NO_START;
    }
    
    return GOOD;
}

int add_client(ENetEvent* event, uint64_t id){

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

    return GOOD;
}


int simple_send_client(uint64_t id, s_packet* packet_p){
    
}


int simple_broadcast(s_packet* packet_p){
    
}

int server_scan_event(){

}

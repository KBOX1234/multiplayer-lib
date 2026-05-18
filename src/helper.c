#define LIBSMPN_PRIVATE

#include "../include/smpn_helper.h"


ENetPeer* yoink_enet_peer_by_id(uint64_t id){
    for(int i = 0; i < client_master.client_count; i++){
        client_data* cd = (client_data*)client_master.clients[i]->data;

        if(cd->id == id) return client_master.clients[i];
    }

    return NULL;
}


int yoink_index_enet_peer_by_id(uint64_t id){
    for(int i = 0; i < client_master.client_count; i++){
        client_data* cd = (client_data*)client_master.clients[i]->data;

        if(cd->id == id) return i;
    }
    return BAD;
}

void silence_logging(bool is_silent){
    be_silent = is_silent;
}

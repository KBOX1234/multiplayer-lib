#include "../include/libsmpn.h"
#include <enet/enet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//client connection to server
typedef struct server_connector{
	ENetHost* myself;

	ENetPeer* remote_server;
} server_connector;



server_connector sc_manager;

int init_mpn_client(const char *ip_addr, int port){
    sc_manager.myself = NULL;
    sc_manager.remote_server = NULL;
	
	
    sc_manager.myself = enet_host_create(NULL /* create a client host */,
        1 /* only allow 1 outgoing connection */,
        2 /* allow up 2 channels to be used, 0 and 1 */,
        0 /* assume any amount of incoming bandwidth */,
        0 /* assume any amount of outgoing bandwidth */);

    if(sc_manager.myself == NULL) return BAD;

    ENetAddress address = {};

    enet_address_set_host(&address, ip_addr);

    address.port = port;

    //argument 3 is set to 2 for a broadcast and targeted channel
    sc_manager.remote_server = enet_host_connect(sc_manager.myself, &address, 2, 0);

    if(sc_manager.remote_server == NULL) return BAD;
    printf("(CLIENT): Connected\n");
    return GOOD;
}

int simple_send_to_server(s_packet *packet_p){
    ENetPacket* e_packet = enet_packet_create(packet_p->packet_buffer, packet_p->buffer_size, ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(sc_manager.remote_server, TARGETED_P, e_packet);

    return GOOD;
}

void scan_for_incomming_packets(int cooldown_timer_ms){
    ENetEvent event = {};

    while(enet_host_service(sc_manager.myself, &event, cooldown_timer_ms) > 0){
        switch(event.type){
			
            case ENET_EVENT_TYPE_CONNECT:{break;}

            case ENET_EVENT_TYPE_RECEIVE:{
                if(ipacked_handle != NULL){

                    s_packet* packet_p = malloc(sizeof(s_packet));

                    packet_p->buffer_size = event.packet->dataLength;
                    packet_p->packet_buffer = event.packet->data;

                    ipacked_handle(packet_p, 0);

                    free(packet_p);
                }
                else{
                    printf("(CLIENT): No incomming packet handler has been set\n");
                }

                break;
            }


            case ENET_EVENT_TYPE_DISCONNECT:{

                if(istatus_sig_handle != NULL){
                    istatus_sig_handle(DISCONNECT_SIG, 0);
                }
                printf("(CLIENT): Was disconnected from server\n");

                break;
            }

            case ENET_EVENT_TYPE_NONE:{break;}
        }
    }
}

void leave_server(){
    enet_peer_disconnect(sc_manager.remote_server, 0);
}

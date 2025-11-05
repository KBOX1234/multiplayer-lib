#include <enet/enet.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SERVER

#include "../include/libsmpn.h"

void custom_iph(s_packet* packet_p, uint64_t client_id){
    v_packet new_packet = deserialize_packet(packet_p);
    
	  //there better be a null terminator
	  //this assumes its a string
    if(new_packet.type == 2){
        printf("Got message: %s\n", new_packet.data);

        v_packet new_packet;
        char* str = "hello client";

        new_packet.data = str;
        new_packet.type = 2;
        new_packet.size = strlen(str) + 1;

        s_packet send_p = serialize_packet(&new_packet);

        simple_send_client(client_id, &send_p);

        

    }
}

void custom_sig_handle(int type, uint64_t client_id){
    if(type == DISCONNECT_SIG){
        printf("Got DISCONNECT_SIG: %lu, there are new %d, clients\n", client_id, get_client_count());
    
    }
    if(type == CONNECT_SIG){
        printf("Got CONNECT_SIG: %lu, there are now %d clients\n", client_id, get_client_count());
    }
}

int main(int argc, char *argv[]){
     
    printf("Hello There...\n");

	  init_mpn_server("localhost", 8090, 32);

	  
    set_incomming_packet_handler(custom_iph);
    set_status_sig_handler(custom_sig_handle);


	  while(1){
	      server_scan_event(0);

	  }

    return 0;
    
}

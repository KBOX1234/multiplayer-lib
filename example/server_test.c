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

int main(int argc, char *argv[]){
     
    printf("Hello There...\n");

	  init_mpn_server("localhost", 8090, 32);
	  
    set_incomming_packet_handler(custom_iph);

	  while(1){
	      server_scan_event(0);


	  }

    return 0;
    
}

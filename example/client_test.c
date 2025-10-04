#include "../include/libmpn.h"
#include <string.h>
#include <stdio.h>


void handle_incomming(s_packet* packet_p, uint64_t client_id){
    v_packet new_packet = deserialize_packet(packet_p);

    if(new_packet.type == 2){
        printf("Got message: %s\n", new_packet.data);
    }
}


int main(){
	init_mpn_client("localhost", 8090);

    set_incomming_packet_handler(handle_incomming);
    
    v_packet new_packet;

    char* str = "hello server";

    new_packet.data = str;
    new_packet.size = strlen(str) + 1;
    new_packet.type = 2;

    s_packet send_p = serialize_packet(&new_packet);

    simple_send_to_server(&send_p);

	while(1){
        
        sleep(1);

        simple_send_to_server(&send_p);

	    scan_for_incomming_packets(0);
	}
}

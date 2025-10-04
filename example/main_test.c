#include <stdio.h>
#include "../include/libmpn.h"

void custom_iph(s_packet* packet_p){
	v_packet new_packet = deserialize_packet(packet_p);

	//there better be a null terminator
	//this assumes its a string
	printf("packet data: %s\n", new_packet.data);
}

int main(int argc, char *argv[]){
     
    printf("Hello There...\n");

	
	init("localhost", 8090, 32);

	set_incomming_packet_handler(custom_iph);

	while(1){
		server_scan_event(0);
	}

    return 0;
    
}

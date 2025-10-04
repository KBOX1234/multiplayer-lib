#include "../include/libmpn.h"
#include <stdint.h>
#include <string.h>

s_packet serialize_packet(v_packet *packet_p){
    s_packet new_packet;

    unsigned char* buff = malloc((sizeof(uint16_t) * 2) + packet_p->size);

    memcpy(buff, &packet_p->type, sizeof(uint16_t));

    
    memcpy(buff + 2, &packet_p->size, sizeof(uint16_t));

    memcpy(buff + 4, packet_p->data, packet_p->size);

    new_packet.buffer_size = packet_p->size + 4;

    new_packet.packet_buffer = buff;

    return new_packet;
}

v_packet deserialize_packet(s_packet *packet_p){
    v_packet new_packet;

    memcpy(&new_packet.type, packet_p->packet_buffer, sizeof(uint16_t));

    memcpy(&new_packet.size, packet_p->packet_buffer + 2, sizeof(uint16_t));
    
    new_packet.data = malloc(packet_p->buffer_size - 4);

    memcpy(new_packet.data, packet_p->packet_buffer + 4, packet_p->buffer_size - 4);

    return new_packet;
}

void destroy_serialized_packet(s_packet *packet_p){
    free(packet_p->packet_buffer);

    packet_p->packet_buffer = NULL;
}

void destroy_deserialized_packet(v_packet *packet_p){
    free(packet_p->data);

    packet_p->data = NULL;
}

void set_incomming_packet_handler(incomming_packet_handler iph){
	ipacked_handle = iph;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <enet/enet.h>
#include <stdint.h>

// -------------------- Your packet structures --------------------
typedef struct {
    uint16_t type;
    uint16_t size;
    char* data;
} v_packet;

typedef struct {
    char* packet_buffer;
    uint16_t buffer_size;
} s_packet;

// -------------------- Serialization Helpers --------------------

// Converts v_packet → s_packet (network-ready)
s_packet serialize_packet(v_packet* packet_p) {
    s_packet sp;
    sp.buffer_size = sizeof(uint16_t) * 2 + packet_p->size;
    sp.packet_buffer = malloc(sp.buffer_size);

    if (!sp.packet_buffer) {
        fprintf(stderr, "Allocation error during serialization.\n");
        exit(1);
    }

    // Copy type and size (little endian assumed)
    memcpy(sp.packet_buffer, &packet_p->type, sizeof(uint16_t));
    memcpy(sp.packet_buffer + sizeof(uint16_t), &packet_p->size, sizeof(uint16_t));

    // Copy data
    memcpy(sp.packet_buffer + sizeof(uint16_t) * 2, packet_p->data, packet_p->size);

    return sp;
}

// Clean up serialized packet
void destroy_serialized_packet(s_packet* packet_p) {
    if (packet_p && packet_p->packet_buffer) {
        free(packet_p->packet_buffer);
        packet_p->packet_buffer = NULL;
    }
}

// -------------------- Client Example --------------------
int main(int argc, char** argv) {
    if (enet_initialize() != 0) {
        fprintf(stderr, "Failed to initialize ENet.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    // Create a client host
    ENetHost* client = enet_host_create(NULL, 1, 2, 0, 0);
    if (!client) {
        fprintf(stderr, "Failed to create ENet client host.\n");
        return EXIT_FAILURE;
    }

    // Connect to server (localhost:1234 for example)
    ENetAddress address;
    enet_address_set_host(&address, "127.0.0.1");
    address.port = 8090;

    ENetPeer* peer = enet_host_connect(client, &address, 2, 0);
    if (!peer) {
        fprintf(stderr, "No available peers for initiating connection.\n");
        enet_host_destroy(client);
        return EXIT_FAILURE;
    }

    printf("Connecting to server...\n");

    ENetEvent event;
    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        printf("Connected to %s:%u\n", "127.0.0.1", 1234);
    } else {
        fprintf(stderr, "Connection failed.\n");
        enet_peer_reset(peer);
        enet_host_destroy(client);
        return EXIT_FAILURE;
    }

    // Send 3 example packets
    for (int i = 0; i < 3; ++i) {
        char msg[64];
        snprintf(msg, sizeof(msg), "Hello server! Packet #%d\0", i + 1);

        v_packet vp = {
            .type = 1,
            .size = (uint16_t)strlen(msg) + 1,
            .data = msg
        };

        s_packet sp = serialize_packet(&vp);

        ENetPacket* enet_packet = enet_packet_create(
            sp.packet_buffer,
            sp.buffer_size,
            ENET_PACKET_FLAG_RELIABLE
        );

        enet_peer_send(peer, 0, enet_packet);
        enet_host_flush(client);

        printf("Sent packet %d: '%s'\n", i + 1, msg);
        destroy_serialized_packet(&sp);
    }

    // Wait for a short time to process responses or acks
    printf("Waiting for responses...\n");
    enet_host_service(client, &event, 2000);

    // Graceful disconnect
    printf("Disconnecting...\n");
    enet_peer_disconnect(peer, 0);

    while (enet_host_service(client, &event, 3000) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            printf("Got response: %s\n", (char*)event.packet->data);
            enet_packet_destroy(event.packet);
        } else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
            printf("Disconnected from server.\n");
            break;
        }
    }

    enet_host_destroy(client);
    printf("Client terminated cleanly.\n");
    return EXIT_SUCCESS;
}


#pragma once
#include "libsmpn.h"

extern client_manager client_master;

#include <stdint.h>

#define BAD_INT -1

ENetPeer* yoink_enet_peer_by_id(uint64_t id);

int yoink_index_enet_peer_by_id(uint64_t id);

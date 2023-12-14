/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/conclave-room-lib
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef CONCLAVE_ROOM_CONNECTION_H
#define CONCLAVE_ROOM_CONNECTION_H

#include <clog/clog.h>
#include <conclave-serialize/types.h>
#include <conclave-room/connection_quality.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct ClvUserSession;
struct ImprintAllocator;
struct ClvRoom;

typedef enum ClvRoomConnectionState {
    ClvRoomConnectionStateNormal,
    ClvRoomConnectionStateWaitingForReconnect,
    ClvRoomConnectionStateDisconnected
} ClvRoomConnectionState;

typedef struct ClvRoomConnection {
    ClvSerializeRoomConnectionIndex id;
    const struct ClvUserSession* owner;
    struct ClvRoom* ownedByRoom;
    ClvRoomConnectionQuality connectionQuality;
    ClvSerializeKnowledge knowledge;
    ClvRoomConnectionState state;
    ClvSerializeConnectedToOwnerState hasConnectionToOwner;
    char logNamePrefix[20];
    Clog log;
} ClvRoomConnection;

void clvRoomConnectionInit(ClvRoomConnection* self, struct ClvRoom* ownedByRoom,
                           const struct ClvUserSession* userSession, MonotonicTimeMs now, Clog log);
void clvRoomConnectionOnPing(ClvRoomConnection* self, ClvSerializeKnowledge knowledge, MonotonicTimeMs now);
void clvRoomConnectionUpdate(ClvRoomConnection* self, MonotonicTimeMs now);
bool clvRoomConnectionShouldDisconnect(const ClvRoomConnection* self);

#endif

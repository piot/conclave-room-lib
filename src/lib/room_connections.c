/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/conclave-room-lib
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <conclave-room/room.h>
#include <conclave-room/room_connection.h>
#include <conclave-room/room_connections.h>
#include <tiny-libc/tiny_libc.h>

void clvRoomConnectionsInit(ClvRoomConnections* self, size_t maxCount, Clog log)
{
    self->log = log;
    self->connectionCount = 0;
    self->connections = tc_malloc_type_count(ClvRoomConnection, maxCount);
    self->capacityCount = maxCount;

    tc_mem_clear_type_n(self->connections, self->capacityCount);
}

void clvRoomConnectionsDestroy(ClvRoomConnections* self)
{
    tc_free(self->connections);
}

void clvRoomConnectionsDestroyConnection(ClvRoomConnections* self, ClvSerializeRoomConnectionIndex index)
{
    ClvRoomConnection* connection = &self->connections[index];
    CLOG_C_DEBUG(&self->log, "destroy connection %hhu in room %zu", connection->id, connection->ownedByRoom->id)
    connection->ownedByRoom = 0;
    connection->owner = 0;
    self->connectionCount--;
}

int clvRoomConnectionsFindConnection(ClvRoomConnections* self, const struct ClvUserSession* ownerOfConnection,
                                     ClvRoomConnection** outConnection)
{
    for (size_t i = 0; i < self->connectionCount; ++i) {
        ClvRoomConnection* roomConnection = &self->connections[i];
        if (roomConnection->owner == ownerOfConnection) {
            *outConnection = roomConnection;

            return 1;
        }
    }

    *outConnection = 0;

    return 0;
}

ClvRoomConnection* clvRoomConnectionsFindConnectionFromIndex(ClvRoomConnections* self, uint8_t connectionIndex)
{
    if (connectionIndex >= self->capacityCount) {
        return 0;
    }

    return &self->connections[connectionIndex];
}

ClvRoomConnection* clvRoomConnectionsFindConnectionWithMostKnowledge(ClvRoomConnections* self,
                                                                     const ClvRoomConnection* excludeConnection)
{
    CLOG_C_DEBUG(&self->log, "find connection with most knowledge. connection count: %zu", self->connectionCount)
    ClvRoomConnection* bestConnection = 0;

    for (size_t i = 0; i < self->capacityCount; ++i) {
        ClvRoomConnection* roomConnection = &self->connections[i];
        if (roomConnection->owner == 0) {
            continue;
        }
        if (roomConnection != excludeConnection &&
            (bestConnection == 0 || roomConnection->knowledge > bestConnection->knowledge)) {
            bestConnection = roomConnection;
        }
    }
    if (bestConnection != 0) {
        CLOG_C_DEBUG(&self->log, "best connection is %hhu", bestConnection->id)
    }
    return bestConnection;
}

bool clvRoomConnectionsHaveMostLostConnectionToOwner(const ClvRoomConnections* self)
{
    size_t votesForDisconnect = 0;
    for (size_t i = 0; i < self->capacityCount; ++i) {
        ClvRoomConnection* roomConnection = &self->connections[i];
        if (roomConnection->owner == 0) {
            continue;
        }
        if (roomConnection->hasConnectionToOwner == ClvSerializeConnectedToOwnerStateDisconnected) {
            votesForDisconnect++;
        }
    }

    return (votesForDisconnect * 2) >= self->connectionCount;
}

void clvRoomConnectionsUpdate(ClvRoomConnections* self, MonotonicTimeMs now)
{
    for (size_t i = 0; i < self->capacityCount; ++i) {
        ClvRoomConnection* roomConnection = &self->connections[i];
        if (roomConnection->owner == 0) {
            continue;
        }

        clvRoomConnectionUpdate(roomConnection, now);
    }
}

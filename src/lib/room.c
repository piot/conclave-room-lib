/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/conclave-room-lib
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <conclave-room/room.h>
#include <conclave-room/room_connection.h>
//#include <conclave-server/user_session.h>
//#include <conclave-server/user_sessions.h>

static int roomCreateConnection(ClvRoom* self, const struct ClvUserSession* ownerOfConnection, MonotonicTimeMs now,
                                ClvRoomConnection** outConnection)
{
    ClvRoomConnections* connections = &self->roomConnections;

    for (size_t i = 0; i < connections->capacityCount; ++i) {
        ClvRoomConnection* roomConnection = &connections->connections[i];
        if (roomConnection->owner == 0) {
            roomConnection->owner = ownerOfConnection;
            if (ownerOfConnection == 0) {
                CLOG_C_ERROR(&self->log, "could not create a connection in the room, owner is null")
            }
            roomConnection->id = (ClvSerializeRoomConnectionIndex) i;

            tc_snprintf(roomConnection->logNamePrefix, sizeof(roomConnection->logNamePrefix), "connections/%zu", i);
            Clog sublog;
            sublog.config = self->log.config;
            sublog.constantPrefix = roomConnection->logNamePrefix;
            clvRoomConnectionInit(roomConnection, self, ownerOfConnection, now, sublog);
            connections->connectionCount++;

            *outConnection = roomConnection;
            return 0;
        }
    }

    *outConnection = 0;

    return -1;
}

void clvRoomDestroy(ClvRoom* self)
{
    clvRoomConnectionsDestroy(&self->roomConnections);
}

void clvRoomInit(ClvRoom* self, const ClvRoomConfig* config)
{
    self->applicationId = config->applicationId;
    self->applicationVersion = config->applicationVersion;
    self->log = config->log;
    tc_strcpy(self->name, 64, config->roomName);
    self->id = config->indexInRooms;
    self->ownedByConnection = 0;
    self->createdByUserSession = config->createdByUserSession;
    self->term = 1;
    self->version = 1;
    clvRoomConnectionsInit(&self->roomConnections, config->maxMemberCount, config->log);
}

int clvRoomCreateRoomConnection(ClvRoom* self, const struct ClvUserSession* foundUserSession, MonotonicTimeMs now,
                                ClvRoomConnection** outConnection)
{
    if (foundUserSession == 0) {
        CLOG_C_ERROR(&self->log, "creator is null")
    }

    int errorCode = clvRoomConnectionsFindConnection(&self->roomConnections, foundUserSession, outConnection);
    if (errorCode < 0) {
        *outConnection = 0;
        return errorCode;
    }

    if (errorCode == 1) {
        return 0;
    }

    errorCode = roomCreateConnection(self, foundUserSession, now, outConnection);
    if (errorCode < 0) {
        *outConnection = 0;
        return errorCode;
    }
    self->version++;

    return 0;
}

void clvRoomSelectNewOwner(ClvRoom* self, const ClvRoomConnection* excludeConnection)
{
    if (self->ownedByConnection == 0) {
        if (self->roomConnections.connectionCount == 0) {
            return;
        }
        self->ownedByConnection = &self->roomConnections.connections[0];
        self->term++;
        self->version++;
        return;
    }

    self->ownedByConnection = clvRoomConnectionsFindConnectionWithMostKnowledge(&self->roomConnections,
                                                                                excludeConnection);
    self->term++;
    self->version++;
}

void clvRoomCheckForDisconnections(ClvRoom* self)
{
    for (size_t i = 0; i < self->roomConnections.capacityCount; ++i) {
        const ClvRoomConnection* roomConnection = &self->roomConnections.connections[i];
        if (roomConnection->owner == 0) {
            continue;
        }
        bool shouldDisconnect = clvRoomConnectionShouldDisconnect(roomConnection);
        if (!shouldDisconnect) {
            continue;
        }

        // Disconnect owner
        // TODO: clvUserSessionsDestroySession(sessions, roomConnection->owner->userSessionId);
        clvRoomConnectionsDestroyConnection(&self->roomConnections, roomConnection->id);
    }
}

#if defined CLOG_LOG_ENABLED
void clvRoomDebugOutput(const ClvRoom* self)
{
    CLOG_C_INFO(&self->log, "'%s' (members %u, connections %zu)", self->name, 0u, self->roomConnections.connectionCount)
}

#endif

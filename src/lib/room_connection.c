/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/conclave-room-lib
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#include <conclave-room/room_connection.h>

/**
 * Initializes a member connection.
 * @param self the member connection
 */
void clvRoomConnectionInit(ClvRoomConnection* self, struct ClvRoom* room, const struct ClvUserSession* userSession,
                           MonotonicTimeMs now, Clog log)
{
    self->log = log;
    self->owner = userSession;
    self->ownedByRoom = room;
    self->hasConnectionToOwner = false;

    clvRoomConnectionQualityInit(&self->connectionQuality, now, log);
}

void clvRoomConnectionOnPing(ClvRoomConnection* self, ClvSerializeKnowledge knowledge, MonotonicTimeMs now)
{
    clvRoomConnectionQualityOnPing(&self->connectionQuality, now);
    self->knowledge = knowledge;
}

void clvRoomConnectionUpdate(ClvRoomConnection* self, MonotonicTimeMs now)
{
    clvRoomConnectionQualityUpdate(&self->connectionQuality, now);
}

bool clvRoomConnectionShouldDisconnect(const ClvRoomConnection* self)
{
    return clvRoomConnectionQualityShouldDisconnect(&self->connectionQuality);
}

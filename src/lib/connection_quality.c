/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/conclave-room-lib
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <conclave-room/connection_quality.h>

void clvRoomConnectionQualityInit(ClvRoomConnectionQuality* self, MonotonicTimeMs now, Clog log)
{
    self->log = log;
    statsIntPerSecondInit(&self->pingsPerSecond, now, 5000);
}

int clvRoomConnectionQualityUpdate(ClvRoomConnectionQuality* self, MonotonicTimeMs now)
{
    int status = statsIntPerSecondUpdate(&self->pingsPerSecond, now);
    CLOG_C_VERBOSE(&self->log, "quality: pingPerSecond: %d isSet: %d", self->pingsPerSecond.avg,
                   self->pingsPerSecond.avgIsSet)
    return status;
}

/// checks if the connection has been decided to be disconnected
/// @param self connection quality
/// @return true if the connection should be disconnected
bool clvRoomConnectionQualityShouldDisconnect(const ClvRoomConnectionQuality* self)
{
    return self->pingsPerSecond.avgIsSet && self->pingsPerSecond.avg <= 2;
}

void clvRoomConnectionQualityOnPing(ClvRoomConnectionQuality* self, MonotonicTimeMs now)
{
    self->lastPingAt = now;
    statsIntPerSecondAdd(&self->pingsPerSecond, 100);
}

/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/conclave-room-lib
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef CONCLAVE_ROOM_CONNECTION_QUALITY_H
#define CONCLAVE_ROOM_CONNECTION_QUALITY_H

#include <stats/stats_per_second.h>
#include <clog/clog.h>

typedef struct ClvRoomConnectionQuality {
    MonotonicTimeMs lastPingAt;
    StatsIntPerSecond pingsPerSecond;
    Clog log;
} ClvRoomConnectionQuality;

void clvRoomConnectionQualityInit(ClvRoomConnectionQuality* self, MonotonicTimeMs now, Clog log);
int clvRoomConnectionQualityUpdate(ClvRoomConnectionQuality* self, MonotonicTimeMs now);
void clvRoomConnectionQualityOnPing(ClvRoomConnectionQuality* self, MonotonicTimeMs now);
bool clvRoomConnectionQualityShouldDisconnect(const ClvRoomConnectionQuality* self);

#endif

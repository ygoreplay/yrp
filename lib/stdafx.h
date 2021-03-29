#ifndef _STDAFX_H_
#define _STDAFX_H_

#include <iostream>
#include <vector>
#include <napi.h>

#include "lzma/LzmaLib.h"

#include "buffer.h"

#include "replay-parameters.h"
#include "replay-header.h"
#include "replay.h"

#define REPLAY_COMPRESSED	0x1
#define REPLAY_TAG			0x2
#define REPLAY_DECODED		0x4
#define REPLAY_SINGLE_MODE	0x8
#define REPLAY_LUA64		0x10
#define REPLAY_NEWREPLAY	0x20
#define REPLAY_HAND_TEST	0x40
#define REPLAY_DIRECT_SEED	0x80
#define REPLAY_64BIT_DUELFLAG	0x100

#define REPLAY_YRP1			0x31707279
#define REPLAY_YRPX			0x58707279

#endif 

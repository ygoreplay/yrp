#ifndef _REPLAY_HEADER_H_
#define _REPLAY_HEADER_H_

struct ReplayHeader {
	uint32_t id;
	uint32_t version;
	uint32_t flag;
	uint32_t seed;
	uint32_t datasize;
	uint32_t hash;
	uint8_t props[8];
};

#endif

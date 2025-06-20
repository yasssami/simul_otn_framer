#pragma once
#include <stdint.h>
#include <stdbool.h>

// consts
#define OTU4_Rate 112.0
#define MAX_SLOTS 80
#define FRAME_SZ 16320

typedef enum {
	CLIENT_ETH_10G,
	CLIENT_ETH_100G,
	CLIENT_OTU2,
	CLIENT_OTU4,
	CLIENT_FC_32G
} ClientType;

// frame struct
#pragma pack(push, 1)
typedef struct {
	//signal
	uint8_t sig[6];
	//mult. alignment
	uint8_t mfasig;

	//overhead structs
	struct {
		uint8_t section_monitor;
		uint8_t chan[2];
	} otu;
	
	struct {
		uint8_t path_monitor;
		uint8_t trail_id[16];
		uint8_t chan_gcc2[2];
	} odu;

	//opu payload (4 rows)
	uint8_t payload[FRAME_SZ];
} OTUFrame;
#pragma pack(pop)

typedef struct {
	ClientType client_type; //chan, WLAN, LAN...
	uint32_t bitrate; //TODO check type req for Mbps
	uint8_t *payload_pt;
	uint32_t num_pkt;
	uint32_t num_err;	
} TribSlot;

// simul state machine
typedef enum {
	FRAMER_RESET,
	FRAMER_IDLE,
	FRAMER_ACTIVE,
	FRAMER_ERR,
	FRAMER_MAINTENANCE
} FramerState;

// global framer
typedef struct {
	FramerState state;
	uint32_t num_frames;
	uint32_t bip8_err;
	uint32_t latency;
	uint32_t max_latency;
	TribSlot slots[MAX_SLOTS];
	uint32_t active_slots;
	uint8_t frame_bfr[sizeof(OTUFrame)];
} FramerContext;


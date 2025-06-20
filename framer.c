#include "otn.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

// init framer
void framer_init(FramerContext *context) {
	memset(context, 0, sizeof(FramerContext));
	context->state = FRAMER_RESET;
	context->num_frames = 0;
	context->bip8_err = 0;
	context->latency = 0;
	context->slots[0] = (TribSlot){CLIENT_ETH_100G, 103125, NULL, 0, 0};
	context->slots[1] = (TribSlot){CLIENT_OTU4, 112000, NULL, 0, 0};
	context->slots[2] = (TribSlot){CLIENT_FC_32G, 32000, NULL, 0, 0};
	context->active_slots = 3;
	
	memset(context->frame_bfr, 0, sizeof(OTUFrame));
	srand(time(NULL));
}


void gen_frame(FramerContext *context) {
	OTUFrame *frame = (OTUFrame *)context->frame_bfr;
	
	//overhead
	memcpy(frame->sig, "\xF6\xF6\xF6\xF6\xF6\xF6", 6);
	frame->mfasig = context->num_frames % 256;
	
	frame->otu.section_monitor = 0x03;
	frame->otu.chan[0] = 0xAA;
	frame->otu.chan[1] = 0x55;

	memset(frame->odu.trail_id, 'C', 16); //ciena tti
	frame->odu.chan_gcc2[0] = 0xCC;
	frame->odu.chan_gcc2[1] = 0xDD;
	
	//simul payload
	uint32_t payload_offset = 0;
	for (int i = 0; i < context->active_slots; i++) {
		const uint32_t slot_sz = context->slots[i].bitrate / (8 * 1000);
		for (uint32_t j = 0; j < slot_sz; j++) {
			frame->payload[payload_offset++] = rand() % 256;
		}
		context->slots[i].num_pkt++;
	}
	//pad if payload has space
	while (payload_offset < FRAME_SZ) {
		frame->payload[payload_offset++] = 0xFF;
	}
}


//dummy HW framer
void framer_ps(FramerContext *context) {
	uint64_t begin = __rdtsc();

	gen_frame(context);

	//simul timing
	const uint64_t cycle = (uint64_t)(2.4 * 1e9 / 1000000);
	const uint64_t jitter = rand() % (cycle /10); //simulate loss/jitter
	uint64_t end = begin + cycle + jitter;
	while (__rdtsc() < end) {} //TODO check if correct wait implementation
	
	context->latency = (__rdtsc() - begin) / 2400;
	if (context->latency > context->max_latency) {
		context->max_latency = context->latency;
	}

	context->num_frames++;
}

#include "otn.h"
#include <string.h>


uint8_t calc_bip8(const uint8_t *data, size_t len) {
	uint8_t bip8 = 0;
	for (size_t i = 0; i < len; i++) {
		bip8 ^= data[i];
		for (int j = 0; j < 8; j++) {
			if (bip8 & 0x80) {
				bip8 = (bip8 << 1) ^ 0x07;
			} else {
				bip8 <<= 1;
			}
		}
	}
	return bip8;
}

// append overhead
void append_otu_overhead(FramerContext *context) {
	OTUFrame *frame = (OTUFrame *)context->frame_bfr;

	//append bip8
	frame->otu.signal_monitor = calc_bip8(context->frame_bfr + 6, sizeof(OTUFrame) - 6);
	frame->odu.path_monitor = context->num_frames % 256;
}



// fetch client signal
void fetch_signal(FramerContext *context) {
	//simul decapsulation
	OTUFrame *frame = (OTUFrame *)context->frame_bfr;
	uint8_t *payload_pt = frame->payload;
	for (int i = 0; i < context->active_slots; i++) {
		const uint32_t slot_sz = context->slots[i].bitrate / (8 * 1000);

		//err sim
		if (context->slots[i].payload_pt && payload_pt[0] != (rand() % 256)) {
			context->slots[i].num_err++;
		}

		payload_pt += slot_sz;
	}	
}


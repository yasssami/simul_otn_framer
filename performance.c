#include "otn.h"
#include <stdio.h>
#include <windows.h>
#include <time.h>

void sim_err(FramerContext *context);

const char* get_client_name(ClientType type) {
	switch(type) {
		case CLIENT_ETH_10G: return "10G Ethernet";
		case CLIENT_ETH_100G: return "100G Ethernet";
		case CLIENT_OTU2: return "OTU2";
		case CLIENT_OTU4: return "OTU4";
		case CLIENT_FC_32G: return "Fibre Channel";
		default: return "dummy";
	}
}


void output_dashboard(FramerContext *context) {
	static uint32_t last_frame_count = 0;
    	static uint32_t last_bip8_err = 0;
    	static uint32_t last_latency = 0;
    	static uint32_t last_active_slots = 0;

	//only refresh dashboard on slot count change, should fix jittering terminal

	if (context->active_slots != last_active_slots) {
        	system("cls"); //TODO try non-windows
        	last_active_slots = context->active_slots;
    	}

    	if (context->num_frames != last_frame_count) {
        	printf("\033[H");  // ANSI escape
   	}

	if (context->num_frames != last_frame_count || 
        context->bip8_err != last_bip8_err || 
        context->latency != last_latency) {
		const float load_percent = (context->latency / 1.25f) * 100;
		const float err_rate = context->num_frames > 0 ? (float)context->bip8_err / context->num_frames * 1e6 : 0; //TODO evaluate whether per million makes sense
		
		//frame header
		printf("====== Waveserver Simul (OTN Framer) ======\n");
		printf("State: %s | Frames: %u | Errors: %u (%.2f ppm)\n",
				context->state == FRAMER_ACTIVE ? "ACTIVE" : "IDLE",
				context->num_frames,
				context->bip8_err,
				err_rate
		);

		printf("Latency: %u microseconds (Max: %u) | Load: %.1f%%\n",
				context->latency,
				context->max_latency,
				load_percent
		);
		printf("Tributary Slots [Active: %d]:\n", context->active_slots);
		printf("Slot  Type               Bitrate   Packets  Errors\n");
    		printf("----  -----------------  --------  -------  ------\n");
	}	
	for (int i = 0; i < context->active_slots; i++) {
		TribSlot *slot = &context->slots[i];
		printf("%2d	%-17s	%4d  Gbps   %7u  %6u\n",
		i,
		get_client_name(slot->client_type),
		slot->bitrate / 1000,
		slot->num_pkt,
		slot-> num_err);
	}	

	printf("\n[SPACE] Framer Toggle   |   [A] Append Slot   |   [W] Wipe Slot   |   [Q] Quit\n");
	printf("------------------------------------------------------------\n");
	fflush(stdout);
	
	last_frame_count = context->num_frames;
    	last_bip8_err = context->bip8_err;
    	last_latency = context->latency;
}
//dummy func, simulate some errors
void sim_err(FramerContext *context) {
	if ((rand() % 1000000) == 0) {
		context->bip8_err++;
		if (context->active_slots > 0) {
			int slot_idx = rand() % context->active_slots;
			context->slots[slot_idx].num_err++;
		}
	}

	//fallback err sim in case above logic is faulty
	if ((rand() % 5000000) == 0) {
		context->bip8_err += (rand() % 5) + 1;
	}
}

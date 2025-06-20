#include <stdlib.h>
#include "otn.h"
#include <conio.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>


bool read(FramerContext *context) {
    if (_kbhit()) {
        switch (_getch()) {
            case ' ':
                context->state = (context->state == FRAMER_ACTIVE) 
                    ? FRAMER_IDLE 
                    : FRAMER_ACTIVE;
                break;
                
            case 'a':
            case 'A':
                if (context->active_slots < MAX_SLOTS) {
                    // rand client type
                    ClientType types[] = {CLIENT_ETH_10G, CLIENT_ETH_100G, 
                                         CLIENT_OTU2, CLIENT_OTU4, CLIENT_FC_32G};
                    ClientType new_type = types[rand() % 5];
                    
                    context->slots[context->active_slots] = (TribSlot){
                        new_type,
                        (new_type == CLIENT_ETH_10G) ? 10312 : 
                        (new_type == CLIENT_ETH_100G) ? 103125 :
                        (new_type == CLIENT_OTU2) ? 10700 :
                        (new_type == CLIENT_OTU4) ? 112000 : 32000,
                        NULL,
                        0,
                        0
                };
                    context->active_slots++;
		    output_dashboard(context);
                }
                break;
                
            case 'w':
            case 'W':
                if (context->active_slots > 0) {
                    context->active_slots--;
                }
                break;
                
            case 'q':
            case 'Q':
                return false;
        }
    }
    return true;
}

int main() {
    FramerContext context;
    framer_init(&context);
    context.state = FRAMER_ACTIVE;
    clock_t last_refresh = clock();
    const clock_t refresh_interval = CLOCKS_PER_SEC / 10;
    bool active = true;
    while (active) {
	if (!read(&context)) {
		active = false;
		break;
	}	
        if (context.state == FRAMER_ACTIVE) {
            framer_ps(&context);
            append_otu_overhead(&context);
            fetch_signal(&context);
            sim_err(&context);
        }
	clock_t current = clock();
        if (current - last_refresh >= refresh_interval) {
            output_dashboard(&context);
            last_refresh = current;
        }        
        read(&context);
     	Sleep(context.state == FRAMER_ACTIVE ? 10 : 100);   
    }
	return 0;
}

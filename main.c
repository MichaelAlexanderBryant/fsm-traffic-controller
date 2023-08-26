#include "stm32f4xx.h"

struct State {
	uint32_t lights_output;
	uint32_t time_delay;
	uint32_t next_state[4];
};

typedef const struct State State_t;

// states
#define GO_NORTH 0
#define WAIT_NORTH 1
#define	GO_EAST 2
#define WAIT_EAST 3

// finite state machine {6-bit output, time delay, state transitions}
State_t FSM[4]={
 {0x21,3000,{GO_NORTH,WAIT_NORTH,GO_NORTH,WAIT_NORTH}},
 {0x22, 500,{GO_EAST,GO_EAST,GO_EAST,GO_EAST}},
 {0x0C,3000,{GO_EAST,GO_EAST,WAIT_EAST,WAIT_EAST}},
 {0x14, 500,{GO_NORTH,GO_NORTH,GO_NORTH,GO_NORTH}}
};

uint32_t current_state; // index to the current state
uint32_t input; // 2-bit sensor reading

int main(void) {
	// initialize phase-lock loop (PLL) at 80MHz
	// initialize SysTick
	// initialize port clocks
	// initialize inputs and outputs	
	current_state = GO_NORTH; // initial state
	while(1){
		// output LIGHT = FSM[current_state].lights_output
		// wait FSM[current_state].time_delay
		// read SENSORS Input = SENSOR
		current_state = FSM[current_state].next_state[input];
	}
}
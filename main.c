#include "stm32f4xx.h"

struct State {
	uint32_t Out;
	uint32_t Time;
	uint32_t Next[4];
};

typedef const struct State State_t;

// states
#define goN		0
#define waitN	1
#define	goE		2
#define waitE	3

// finite state machine {6-bit output, time delay, state transitions}
State_t FSM[4]={
 {0x21,3000,{goN,waitN,goN,waitN}},
 {0x22, 500,{goE,goE,goE,goE}},
 {0x0C,3000,{goE,goE,waitE,waitE}},
 {0x14, 500,{goN,goN,goN,goN}}
};

uint32_t current_state; // index to the current state
uint32_t input; // 2-bit sensor reading

int main(void) {
	// initialize phase-lock loop (PLL) at 80MHz
	// initialize SysTick
	// initialize port clocks
	// initialize inputs and outputs	
	current_state = goN; // initial state
	while(1){
		// output LIGHT = FSM[current_state].Out
		// wait FSM[current_state].Time
		// read SENSORS Input = SENSOR
		current_state = FSM[current_state].Next[input];
	}
}
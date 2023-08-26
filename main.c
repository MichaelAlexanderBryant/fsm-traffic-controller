#include "stm32f4xx.h"

struct state {
	uint32_t set_lights; // 6-bit output where left three are for North and right three are for East (green, yellow, red)
	uint32_t time_delay;
	uint32_t next_state[4];
};

typedef const struct state state_t;

// states
#define GO_NORTH 0
#define WAIT_NORTH 1
#define	GO_EAST 2
#define WAIT_EAST 3

// finite-state machine {6-bit output, time delay, state transitions}
state_t FSM[4]={
 {0x21,3000,{GO_NORTH,WAIT_NORTH,GO_NORTH,WAIT_NORTH}},
 {0x22, 500,{GO_EAST,GO_EAST,GO_EAST,GO_EAST}},
 {0x0C,3000,{GO_EAST,GO_EAST,WAIT_EAST,WAIT_EAST}},
 {0x14, 500,{GO_NORTH,GO_NORTH,GO_NORTH,GO_NORTH}}
};

uint32_t current_state; // index the current state
uint32_t input; // 2-bit sensor reading

int main(void) {
	// initialize phase-lock loop (PLL) at 80MHz
	// initialize SysTick
	RCC->AHB1ENR |= 1; // enable GPIOA clock (output)
	RCC->AHB1ENR |= 4; // enable GPIOC clock (input)

	GPIOA->MODER &= 0x00000FFF; // PA0-PA5 (output)
	GPIOA->MODER |= 0x00000555; // 010101010101
	GPIOC->MODER &= 0x0000000F; // PC0-PC1 (input)
	
	current_state = GO_NORTH; // initial state
	while(1){
		GPIOA->ODR = FSM[current_state].set_lights;
		// wait FSM[current_state].time_delay
		input = GPIOC->IDR;
		current_state = FSM[current_state].next_state[input];
	}
}
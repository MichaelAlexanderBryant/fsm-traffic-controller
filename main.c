#include "stm32f4xx.h"

// finite-state machine {6-bit output, time delay, state transitions}
struct state {
	uint32_t set_lights;
	uint32_t time_delay;
	uint32_t next_state[4];
};

typedef const struct state state_t; // store state struct in flash ROM

// states
#define GO_Y 0
#define WAIT_Y 1
#define	GO_X 2
#define WAIT_X 3

state_t FSM[4]={
 {0x21,3000,{GO_Y,WAIT_Y,GO_Y,WAIT_Y}},
 {0x22, 500,{GO_X,GO_X,GO_X,GO_X}},
 {0x0C,3000,{GO_X,GO_X,WAIT_X,WAIT_X}},
 {0x14, 500,{GO_Y,GO_Y,GO_Y,GO_Y}}
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
	
	current_state = GO_Y; // initial state
	while(1){
		GPIOA->ODR = FSM[current_state].set_lights;
		// wait FSM[current_state].time_delay
		input = GPIOC->IDR;
		current_state = FSM[current_state].next_state[input];
	}
}
#include "stm32f4xx.h"

// finite-state machine {6-bit output, time delay, state transitions}
struct state {
	uint32_t set_output;
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
 {0x1001,3000,{GO_Y,WAIT_Y,GO_Y,WAIT_Y}},
 {0x1002, 500,{GO_X,GO_X,GO_X,GO_X}},
 {0x0410,3000,{GO_X,GO_X,WAIT_X,WAIT_X}},
 {0x0810, 500,{GO_Y,GO_Y,GO_Y,GO_Y}}
};

void ppl_init(void);
void systick_delay(int n);

uint32_t current_state = GO_Y; // index the current state
uint32_t input; // 2-bit sensor reading

int main(void) {
	ppl_init(); // initialize phase-lock loop (PPL) at 80MHz
	
	RCC->AHB1ENR |= 1; // enable GPIOA clock (output)
	RCC->AHB1ENR |= 4; // enable GPIOC clock (input)

	GPIOA->MODER &= ~0x03F0030F; // clear PA0, PA1, PA4, PA10, PA11, PA12
	GPIOA->MODER |= 0x01500105; // set PA0, PA1, PA4, PA10, PA11, PA12 as output
	GPIOC->MODER &= ~0x0000000F; // clear PC0 and PC1, setting as input
	
	while(1){
		GPIOA->ODR &= 0; // turn pins off
		GPIOA->ODR |= FSM[current_state].set_output; // set current state
		systick_delay(FSM[current_state].time_delay); // delay by speciied FSM time
		input = GPIOC->IDR; // read sensors
		if ((input & 0x02)&&(input & 0x01)) {input = 3;} // if both high
		else if (input & 0x02) {input = 2;} // if sensor Y is high
		else if (input & 0x01) {input = 1;} // if sensor X is high
		else {input = 0;} // else no sensor is high
		current_state = FSM[current_state].next_state[input]; // go to next state
	}
}

void ppl_init(void){
	// PLLP output: 80MHz
	// PLLP input: 160MHz (Target*2)
	// PLLM output: 1MHz
	// PLLM input (Reference): 8MHz
	// PLLN output: 160MHz (PLLM output*PLLP input)
	// PLLM=8, PLLN=160, PLLP=2
	
	RCC->CR |= (1<<16); // enable HSE clock
	while(!(RCC->CR & (1<<17))); // wait for HSE
	RCC->CR &= ~(1<<24); // disable PLL
	
	RCC->PLLCFGR |= 1<<22; // configure PLL input source

	// configure PLLM
	RCC->PLLCFGR &= ~(0x1F);
	RCC->PLLCFGR |= 0x08; // PLLM=8
	
	// configure PLLN
	RCC->PLLCFGR &= ~(0x1FF<<6);
	RCC->PLLCFGR |= 160<<6; // PLLN=160
	
	// configure PLLP
	RCC->PLLCFGR &= ~(0x03<<16);
	RCC->PLLCFGR |= 0x01<<16;
	
	RCC->CR |= 1<<24; // enable PLL
	while(!(RCC->CR & (1<<25))); // wait for PLL
	
	// assign PLL as the system clock
	RCC->CFGR &= ~(0x03);
	RCC->CFGR |= 0x02;
	
//	// HCLK prescale=1 (AHB max=80MHz)
//	RCC->CFGR &= ~(0xF<<4);
//	
//	// APB1 prescalar=4 (APB1 max=42MHz)
//	RCC->CFGR &= ~(0x7<<10);
//	RCC->CFGR |= 0x5<<10;
//	
//	// APB2 prescaler=2 (APB2 max = 84MHz)
//	RCC->CFGR &= ~(0x7<<13);
//	RCC->CFGR |= 0x4<<13;
}

void systick_delay(int n){
	int i;
	
	SysTick->LOAD = 80000; // reload with number of clocks per millisecond (1ms * 80MHz)
	SysTick->VAL = 0; // clear current value register
	SysTick->CTRL = 0x5; // enable timer
	
	for(i=0; i<n; i++){
		while((SysTick->CTRL & 0x10000) == 0) {} // wait until the COUNTFLAG is set
	}
	SysTick->CTRL = 0; // Stop the timer
	
}
#include <stdint.h>
#include <stm32f10x.h>


#define RCC_APB2ENR   (*((volatile uint32_t*)0x40021018))
#define GPIOC_CRH     (*((volatile uint32_t*)0x40011004))
#define GPIOC_ODR     (*((volatile uint32_t*)0x4001100C))
#define GPIOC_IDR     (*((volatile uint32_t*)0x40011008))

const uint32_t delays[] = {
    256000000,  // 1/64 Гц
    128000000,  // 1/32 Гц
    64000000,   // 1/16 Гц
    32000000,   // 1/8 Гц
    16000000,   // 1/4 Гц
    8000000,    // 1/2 Гц
    4000000,    // 1 Гц
    2000000,    // 2 Гц
    1000000,    // 4 Гц
    500000,     // 8 Гц
    250000,     // 16 Гц
    125000,     // 32 Гц
    62500       // 64 Гц
};

const uint32_t delays_count = sizeof(delays);
volatile int8_t current_step = 6;

static uint8_t last_button = 1;

void simple_delay(uint32_t cycles) {
    for(volatile uint32_t i = 0; i < cycles; i++);
}

void debounce_delay(void) {
    for(volatile uint32_t i = 0; i < 50000; i++);
}

void check_buttons(void) {
    
    
    uint8_t current_button = (GPIOC_IDR & (1 << 14)) ? 1 : 0;

    if (current_button == 0 && last_button == 1) {
        debounce_delay();
        if ((GPIOC_IDR & (1 << 14)) == 0) {
        
            current_step = (current_step + 1) % delays_count;
            
            while ((GPIOC_IDR & (1 << 14)) == 0); 
            	debounce_delay();
        }
    }
    
    last_button = current_button;

}

int main(void) {
    RCC_APB2ENR |= (1 << 4);
    
    GPIOC_CRH &= ~(0xFF << 20);
    GPIOC_CRH |= (1 << 20);
    
    GPIOC_CRH &= ~(0xFF << 24);
    GPIOC_CRH |= (0x8 << 24);
    GPIOC_ODR |= (1 << 14);
    
    GPIOC_CRH &= ~(0xFF << 28);
    GPIOC_CRH |= (0x8 << 28);
    GPIOC_ODR |= (1 << 15);
    
    while(1) {
        GPIOC_ODR ^= (1 << 13);
        
        simple_delay(delays[current_step]);
        
        check_buttons();
    }
}


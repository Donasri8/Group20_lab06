#include <stdint.h>

#define SYSCTL_RCGCGPIO_R      (*((volatile uint32_t *)0x400FE608))
#define GPIO_PORTF_DATA_R      (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_DIR_R       (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_DEN_R       (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_PUR_R       (*((volatile uint32_t *)0x40025510))
#define GPIO_PORTF_LOCK_R      (*((volatile uint32_t *)0x40025520))
#define GPIO_PORTF_CR_R        (*((volatile uint32_t *)0x40025524))
#define GPIO_PORTF_IS_R        (*((volatile uint32_t *)0x40025404))
#define GPIO_PORTF_IBE_R       (*((volatile uint32_t *)0x40025408))
#define GPIO_PORTF_IEV_R       (*((volatile uint32_t *)0x4002540C))
#define GPIO_PORTF_IM_R        (*((volatile uint32_t *)0x40025410))
#define GPIO_PORTF_ICR_R       (*((volatile uint32_t *)0x4002541C))
#define GPIO_PORTF_MIS_R       (*((volatile uint32_t *)0x40025418))

#define NVIC_EN0_R             (*((volatile uint32_t *)0xE000E100))
#define NVIC_PRI7_R            (*((volatile uint32_t *)0xE000E41C))

#define NVIC_ST_CTRL_R         (*((volatile uint32_t *)0xE000E010))
#define NVIC_ST_RELOAD_R       (*((volatile uint32_t *)0xE000E014))
#define NVIC_ST_CURRENT_R      (*((volatile uint32_t *)0xE000E018))

#define LED_RED     (1U << 1)
#define LED_BLUE    (1U << 2)
#define SW2_MASK    (1U << 0)

volatile uint8_t red_led_state = 0;

void SysTick_Handler(void) {
    GPIO_PORTF_DATA_R ^= LED_BLUE;
}

void GPIOF_Handler(void) {
    if (GPIO_PORTF_MIS_R & SW2_MASK) {
        GPIO_PORTF_ICR_R = SW2_MASK;
        red_led_state ^= 1;
        if (red_led_state)
            GPIO_PORTF_DATA_R |= LED_RED;
        else
            GPIO_PORTF_DATA_R &= ~LED_RED;
    }
}

int main(void) {
    volatile uint32_t delay;
    SYSCTL_RCGCGPIO_R |= (1U << 5);
    delay = SYSCTL_RCGCGPIO_R;

    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R  |= (SW2_MASK | LED_RED | LED_BLUE);

    GPIO_PORTF_DIR_R |= (LED_RED | LED_BLUE);
    GPIO_PORTF_DEN_R |= (LED_RED | LED_BLUE);

    GPIO_PORTF_DIR_R &= ~SW2_MASK;
    GPIO_PORTF_DEN_R |= SW2_MASK;
    GPIO_PORTF_PUR_R |= SW2_MASK;

    GPIO_PORTF_IS_R  &= ~SW2_MASK;
    GPIO_PORTF_IBE_R &= ~SW2_MASK;
    GPIO_PORTF_IEV_R &= ~SW2_MASK;
    GPIO_PORTF_ICR_R  = SW2_MASK;
    GPIO_PORTF_IM_R  |= SW2_MASK;

    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | (7 << 21);
    NVIC_EN0_R  |= (1U << 30);

    NVIC_ST_RELOAD_R = 16000000 - 1;
    NVIC_ST_CURRENT_R = 0;
    NVIC_ST_CTRL_R = 0x07;

    while (1) {
    }
}

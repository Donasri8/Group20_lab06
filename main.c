#include <stdint.h>

// Base addresses
#define SYSCTL_BASE     0x400FE000
#define GPIOF_BASE      0x40025000
#define NVIC_EN0        (*(volatile uint32_t *)0xE000E100)

// Registers
#define SYSCTL_RCGCGPIO (*(volatile uint32_t *)(SYSCTL_BASE + 0x608))
#define SYSCTL_PRGPIO   (*(volatile uint32_t *)(SYSCTL_BASE + 0xA08))

#define GPIOF_DATA      (*(volatile uint32_t *)(GPIOF_BASE + 0x3FC))
#define GPIOF_DIR       (*(volatile uint32_t *)(GPIOF_BASE + 0x400))
#define GPIOF_DEN       (*(volatile uint32_t *)(GPIOF_BASE + 0x51C))
#define GPIOF_PUR       (*(volatile uint32_t *)(GPIOF_BASE + 0x510))
#define GPIOF_LOCK      (*(volatile uint32_t *)(GPIOF_BASE + 0x520))
#define GPIOF_CR        (*(volatile uint32_t *)(GPIOF_BASE + 0x524))
#define GPIOF_IS        (*(volatile uint32_t *)(GPIOF_BASE + 0x404))
#define GPIOF_IBE       (*(volatile uint32_t *)(GPIOF_BASE + 0x408))
#define GPIOF_IEV       (*(volatile uint32_t *)(GPIOF_BASE + 0x40C))
#define GPIOF_IM        (*(volatile uint32_t *)(GPIOF_BASE + 0x410))
#define GPIOF_ICR       (*(volatile uint32_t *)(GPIOF_BASE + 0x41C))
#define GPIOF_RIS       (*(volatile uint32_t *)(GPIOF_BASE + 0x418))

// Pins
#define LED_BLUE    (1 << 2)
#define SW1         (1 << 4)

// Macros
#define ENABLE_CLOCK_F       (SYSCTL_RCGCGPIO |= (1 << 5))
#define WAIT_PORTF_READY     while((SYSCTL_PRGPIO & (1 << 5)) == 0)

#define LED_OUT              (GPIOF_DIR |= LED_BLUE)
#define SW1_IN               (GPIOF_DIR &= ~SW1)

#define LED_DEN              (GPIOF_DEN |= LED_BLUE)
#define SW1_DEN              (GPIOF_DEN |= SW1)

#define SW1_PULLUP           (GPIOF_PUR |= SW1)

#define TOGGLE_LED           (GPIOF_DATA ^= LED_BLUE)
#define CLEAR_INT_SW1        (GPIOF_ICR = SW1)
#define UNMASK_INT_SW1       (GPIOF_IM |= SW1)
#define RISING_EDGE_SW1      (GPIOF_IEV |= SW1)

#define NVIC_ENABLE_PORTF    (NVIC_EN0 |= (1 << 30))

void GPIOF_Handler(void);

int main(void)
{
    ENABLE_CLOCK_F;
    WAIT_PORTF_READY;

    GPIOF_LOCK = 0x4C4F434B;
    GPIOF_CR   |= (LED_BLUE | SW1);

    LED_OUT;
    SW1_IN;

    LED_DEN;
    SW1_DEN;

    SW1_PULLUP;

    GPIOF_IS  &= ~SW1;
    GPIOF_IBE &= ~SW1;
    RISING_EDGE_SW1;
    CLEAR_INT_SW1;
    UNMASK_INT_SW1;

    NVIC_ENABLE_PORTF;

    __asm("CPSIE I");

    while(1)
    {
        // Waiting for interrupt
    }
}

void GPIOF_Handler(void)
{
    if(GPIOF_RIS & SW1)
    {
        CLEAR_INT_SW1;
        TOGGLE_LED;
    }
}

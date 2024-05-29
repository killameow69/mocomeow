#include "stm32f4xx.h"
#include <stdint.h>

#define GPIOAEN     (1U<<0)
#define PA2MOD      (2U<<4)
#define PA2AF       (7U<<8)
#define PA3MOD      (2U<<6)
#define PA3AF       (7U<<12)
#define APB1EN      (1U<<17)
#define USART2_TX   (1U<<3)
#define USART2_RX   (1U<<2)
#define USART2EN    (1U<<13)
#define SR_TXE      (1U<<7)
#define SR_RXNE     (1U<<5)

#define SYS_FREQ        16000000
#define APB1_CLK        SYS_FREQ
#define UART_BAUDRATE   115200

void uart2_init(void);
void uart2_write(int ch);
int uart2_read(void);
void delayMs(int n);

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);

int main()
{
    uart2_init();

    while(1)
    {
        // Echo received characters back
        if (USART2->SR & SR_RXNE) {
            char received_char = uart2_read();
            uart2_write(received_char);

        }

        delayMs(5); // Delay for stability
    }

    return 0;
}

void delayMs(int n)
{
    for (; n > 0; n--)
        for (int i = 0; i < 5000; i++);
}

void uart2_init(void)
{
    // Enable clock access to GPIOA
    RCC->AHB1ENR |= GPIOAEN;

    // Set PA2 (USART2_TX) and PA3 (USART2_RX) mode to alternate function mode
    GPIOA->MODER |= PA2MOD | PA3MOD;
    // Set PA2 and PA3 alternate function type to UART (AF07)
    GPIOA->AFR[0] |= PA2AF | PA3AF;

    // Enable clock access to USART2
    RCC->APB1ENR |= APB1EN;

    // Configure baudrate
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    // Configure USART2 for both transmission and reception
    USART2->CR1 = USART2_TX | USART2_RX;

    // Enable USART2 module
    USART2->CR1 |= USART2EN;
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate)
{
    USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate)
{
    return ((PeriphClk + (BaudRate / 2U)) / BaudRate);
}

void uart2_write(int ch)
{
    // Wait until transmit data register is empty
    while(!(USART2->SR & SR_TXE));

    // Send character via UART
    USART2->DR = (ch & 0xFF);
}

int uart2_read(void)
{
    // Wait until data is received
    while (!(USART2->SR & SR_RXNE));

    // Read received data
    return (USART2->DR & 0xFF);
}

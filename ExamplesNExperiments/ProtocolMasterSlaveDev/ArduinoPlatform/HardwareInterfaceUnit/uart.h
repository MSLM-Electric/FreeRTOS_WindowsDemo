#include <avr/io.h>
#include "type_def.h"

int USART_Transmit(unsigned char data);
uint8_t USART_Receive(void);
int USART_ReceiveINTEnable(void);
int USART_ReceiveINTDisable(void);
uint8_t USART_GetDataFromReceiveISR(void);
void USART_TransmitINTEnable(void);
void USART_TransmitINTDisable(void);
void USART_Flush(void);
/*
 * Simple program to flash the LEDs while sending CAN messages on port 1
 * and receiving them by polling on port 2.
 *
 * Implemented using a time-triggered scheduler.
 *
 * DK - 17-Nov 2010
 */


#include <stdbool.h>
#include <stdint.h>
#include <bsp.h>
#include <lcd.h>
#include <leds.h>
#include <can.h>
#include <scheduler.h>


uint32_t txCount;
uint32_t rxCount;


void linkLedToggleTask(void);
void connectLedToggleTask(void);
void canWriteTask(void);
void canReadTask(void);

int main () {
  
  bspInit();
  schInit();
  schAddTask(linkLedToggleTask, 0, 500);
  schAddTask(connectLedToggleTask, 100, 500);
  schAddTask(canWriteTask, 3, 250);
  schAddTask(canReadTask, 5, 100);


  schStart();
  
  while (true) {
    schDispatch();
  }
}


void linkLedToggleTask(void) {
    ledToggle(USB_LINK_LED);
}

void connectLedToggleTask(void) {
    ledToggle(USB_CONNECT_LED);
}

void canWriteTask(void) {
  
  static canMessage_t txMsg = {0x23, 4, 0, 0};
  bool txOk;
    
  // Transmit message on CAN 1
  txOk = canWrite(CAN_PORT_1, &txMsg);
  if (txOk) {
    txCount += 1;
    txMsg.dataA = txCount;
  }
}

void canReadTask(void) {

  static canMessage_t rxMsg;
  
  if (canReady(CAN_PORT_2)) {           
    canRead(CAN_PORT_2, &rxMsg);
    lcdSetTextPos(2,1);
    lcdWrite("ID    : %x", rxMsg.id); 
    lcdSetTextPos(2,2);
    lcdWrite("LEN   : %x", rxMsg.len); 
    lcdSetTextPos(2,3);
    lcdWrite("DATA_A: %x", rxMsg.dataA); 
    lcdSetTextPos(2,4);
    lcdWrite("DATA_B: %x", rxMsg.dataB); 
    rxCount += 1;
  }
}

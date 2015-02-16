/******************************************************************************************/
/*
*  Simple basic example of RAComm library use.
*  At reset it starts communication with Raspberry and send command to get timestamp:
*  command: GetRTC -s
*  Show answer on x terminal console and extract just hour.
*  Then blink n times; where n=hh.
*
*  Author Daniele Denaro
*/

#include "RAComm.h"          // Use Raspberry command library

#define CMD1 "GetRTC -s"     // Raspberry command to get timestamp

RAComm Cmd;                  // Cmd is the RAComm class

int hh=0;                    // hour value

char buff[48];               // buffer to receive answer

int led;

void setup() 
{
  pinMode(13,OUTPUT);
  Cmd.begin();               // Alert Raspberry
  delay(1000);
  boolean ok=Cmd.isReady();  // test if Raspberry is listening
  if (!ok) return;       
  Cmd.openConsole();         // open x terminal console on Raspberry just for debugging
  delay(1000);               // Note: you have to open a MobaXterm session to see Xterminal
  Cmd.sendCommand(CMD1);     // Send command
  Cmd.getAnswer(buff,48);    // Get answer (timestamp) (YYYY-MM-DD hh:mm:ss)
  Cmd.writeConsole(buff);    // Just echo on Xterminal
  readHour();                // Extract hour to hh
  delay(10000);              // Just to see the Xterminal for enough time
  Cmd.close();               // Close dialog
}

void loop() {
  blinking(hh*2,200);           // blink as many times as hh
  delay(1000);
}

void readHour()
{
  sscanf(buff,"%*d-%*d-%*d %d:%*d:%*d",&hh); // extract just hh (* skips number)
}

void blinking(int n,int blinktime)
{
  int i;
  for (i=0;i<n;i++)
  {
   digitalWrite(13,led);
   led=led^1;
   delay(blinktime);
  }
  led=1; 
}

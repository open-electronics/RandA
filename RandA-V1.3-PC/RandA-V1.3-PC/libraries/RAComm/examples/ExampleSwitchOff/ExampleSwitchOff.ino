/******************************************************************************/
/* 
*  This sketch shows how you can switch-on Raspberry if value on Arduino 
*  analogical pin is less then a threshold. 
*  Of course, in this case, Arduino must be always powered.
*  The sketch:
*  - check analogical pin; if < threshold set status = 1 else set status = 0
*  - if transition from status 0 to 1 switches Raspberry on (if it was off)
     and records a message on a file.  
*  - If Raspberry was off then switch it off again. Else Raspberry stays on. 
*
*  Led (pin 13) show operations: usualy is blinking but when operation starts 
*  it stay on.
*
*  Be careful!!! 
*  This sketch needs to close the jumper JP2 and to set SW2 in R position (i.e. 
*  Arduino always on).
*  IMPORTANT! Because Arduino gains control on Raspberry, open jump JP2 to have 
*  back Raspberry power control. 
*  
*  Author: Daniele Denaro
*/

#include "RAComm.h"       // Use Raspberry command library

#define CMD1 "GetRTC -s"  // get timestamp

#define LOGFILE  "/home/pi/wakeup.dat"  // where to write message

#define CHECKTIME 10000    // check interval 10 sec

RAComm Cmd;               // Cmd is the RAComm class

int status=0;          // status flag (when condition true, set flag 1)
                       // it doesn't restart procedure until status will be back to 0
boolean flagOn;        // flag Raspberry on or off

int sensorPin = A1;    // select the input pin for the potentiometer or photores
int ledPin = 13;       // select the pin for the LED
int sensorValue = 0;   // variable to store the value coming from the sensor
int threshold =200;    // threshold of sensorPin 
int switchPin = 4;     // pin for switching 

long ta=0;

void setup() {
  pinMode(ledPin, OUTPUT);     // led just as flag
  pinMode(switchPin, OUTPUT);  
  digitalWrite(ledPin,0);      // led off
  digitalWrite(switchPin,0);   // initialize switching pin
}

void loop() 
{
  long tb=millis();
  if (tb<ta+CHECKTIME)                     // if interval is not expired just blink
   {digitalWrite(ledPin,HIGH);delay(500);digitalWrite(ledPin,LOW);delay(500);}
  else                                     // else set led on and start procedure
  {
    digitalWrite(ledPin,HIGH);         
    sensorValue = analogRead(sensorPin);   // read the value from the sensor   
    if (status==0)
     {if(sensorValue<threshold) {switchOn();status=1;}} // if < threshold  operate
    else
     {if(sensorValue>threshold) status=0;}              // if > threshold  reset flag
    ta=tb;                                 // start new interval
  }  
}

void swOnOff()
{
  digitalWrite(switchPin,LOW);
  delay(20);
  digitalWrite(switchPin,HIGH);
  delay(20);
  digitalWrite(switchPin,LOW);
}

void switchOn()
{
  flagOn=checkOn();
  if (!flagOn)
  {
    swOnOff();                              // switch Raspberry on
    delay(60000);                           // wait 1 min for startup procedure
    if (initDialog<0) {error();return;}     // if not dialog then error   
  } 
  command();
  delay(10000);                             // just for confidence
  if (!flagOn) swOnOff();                   // switch Raspberry off again if it was off
}

int command()
{
  boolean ok;
  char rbuff[48];
  ok=Cmd.sendCommand(CMD1);                    // send command to get timestamp
   if (!ok) {error();return -1;}
  char *stime=Cmd.getAnswer(rbuff,48);         // get replay  
   if (stime==NULL) {error();return -1;}
  ok=Cmd.openFile(LOGFILE,"a","755");          // open file in append mode and linux protection code 755
   if (!ok) {error();return -1;}
  char rec[64];
  snprintf(rec,64,"Waked up at %s because value: %d",stime,sensorValue); 
  Cmd.writeRec(rec);
  Cmd.closeFile();
  Cmd.close();
}

int initDialog()
{
   boolean ok;
   Cmd.begin();                   // Raspberry command dialog initialization 
   int i;for (i=0;i<30;i++)       // retry just if Raspberry is in startup phase
    {ok=Cmd.isReady();if (ok) break;Cmd.begin();delay(1000);}
   if (!ok) {error();return -1;}  // if no dialog no action
   flagOn=true;
   return 0;
}

boolean checkOn()
{
   Cmd.begin(); 
   delay(300);
   return Cmd.isReady();   
}

void error()
{
  int i;
  for (i=0;i<10;i++)
  {digitalWrite(ledPin,HIGH);delay(250);digitalWrite(ledPin,LOW);delay(250);}
}
  

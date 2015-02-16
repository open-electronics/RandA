
/******************************************************************************/
/*  
*  This simple example show the basic use of dialog from Arduino to Raspberry.
*  This sketch reads two analogical values and two digital input every hour and 
*  saves it in a log file with a timestamp. 
*
*  At startup, the sketch just sets digital pin.
*  The real operations are performed in loop function:
*  - open dialog
*  - send command to achieve timestamp (YYYY-MM-DD hh:mm:ss)
*  - check if it is the starting of hour (5 minutes of tollerance)
*  - if yes read values and write record
*  - delay for several minutes
*  
*  This sketch doesn't switch-off the sistem, just suspend itself for several
*  minutes.
*
*  Note: flag TEST is true for default. So no check time is performed. Change to
*        true for real functionality.  
*  
*  Author: Daniele Denaro
*/

#include "RAComm.h"       // Use Raspberry command library

#define anaA 0            // analogic input pin used
#define anaB 1            // analogic input pin used
#define digA 11           // digital input pin used
#define digB 6            // digital input pin used

#define LOGFILE "/home/pi/data.dat" // log file name

//Commands:
// Raspberry command to get timestamp
#define CMD1 "GetRTC -s" 

#define TEST true       // if TEST it doesn't check time

RAComm Cmd;             // Cmd is the RAComm class

int led=1;                // led on
char rbuff[64];           // buffer for Raspberry command replay
int len=64;               // its length

void setup() 
{
  pinMode(digA,INPUT_PULLUP);  // set pin digA as input with pullup resistor
  pinMode(digB,INPUT_PULLUP);  // set pin digB as input with pullup resistor
  initLed();                   // standard Arduino led used as flag 
}

void loop() 
{
 int nexttime=checkTime();
 if (nexttime<0) nexttime=10;
 unsigned long time=nexttime*60000; 
 delay(time); 
}

int checkTime()
{
  boolean ok;
  Cmd.begin();                 // Raspberry command dialog initialization 
  delay(100);             
  for (int i=0;i<20;i++)       // delay just if Raspberry is in startup phase
  {ok=Cmd.isReady();if (ok) break;Cmd.begin();delay(100);}
   if (!ok) {noListen();return -1;}                   // if no dialog no action

  ok=Cmd.sendCommand(CMD1);                        // send command to get timestamp
   if (!ok) {error();return -1;}
  char *stime=Cmd.getAnswer(rbuff,len);            // get replay  
   if (stime==NULL) {error();return -1;}
  int hh=0; int mm=0;
  sscanf(stime,"%*d-%*d-%*d %d:%d:%*d",&hh,&mm);   // extract hh and mm from timestamp
  ok=Cmd.openFile(LOGFILE,"a","755");              // open file in append mode and linux protection code 755
   if (!ok) {error();return -1;}
  char rec[64];

  if (!TEST) 
  {
   if ((mm<55)&(mm>5)) 
   {
     snprintf(rec,64,"Start collecting next sample in %d min.",60-mm);
     Cmd.writeRec(rec);
     Cmd.closeFile();
     Cmd.close();
     return 60-mm;
   }
  }

  int vaA=analogRead(anaA);                       // read analogical value
  int vaB=analogRead(anaB);                       // read analogical value
  int vdA=digitalRead(digA);                      // read digital input
  int vdB=digitalRead(digB);                      // read digital input
  
  snprintf(rec,64,"%s %d %d %1d %1d",stime,vaA,vaB,vdA,vdB);  // make record with timestamp and value
  Cmd.writeRec(rec);                               // write record
  Cmd.closeFile();                                 // close file
  delay(10000);
  Cmd.close();                                     // close dialog 
  return 59;  
}

void initLed()
{
  pinMode(13,OUTPUT);
}

void noListen()
{
  blinking(50,100);
}

void error()
{
  Cmd.close();
  blinking(20,300);
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




/*****************************************************************************************************/
/* ATTENTION! Sketch can make the system out of control! Use JP1 to escape it, see later on! 
*  This example sketch reads an analogical value every day, saves it in a log file and sends a mail.
*  A the end it switches off itself. 
*  This example uses the Raspberry command library. 
*  This example presumes Arduino switching off together with Raspberry (no separate supply)
*
*  At startup, the scketch control time and if it is expired reads analogical value, append this value
*  with its timestamp to a log file. Aftewards, extract the last 5 records from log file and send these
*  records by e-mail.
*
*  Note: you have to fill /home/bin/mail.properties file with your data beafore
*        using this sketch.
* 
*  You, also, have to fill the addressee variable.
* 
*  If time is not expired, or after sendig mail, the sketch switches off Raspberry and itself (important!).
*  So, loop function is not used. Just to signaling error by different blinking interval.
*  
*  ATTENTION! 
*  This sketch drives the entire system! Automatic switch on and switch off !
*  When system is switched off, you can power on it by hand (push button), but, first of all, you MUST 
*  disable dialog between Arduino and Raspberry opening JP1 jumper.
*
*  Note: to test the sketch, the shutdown and check time are disabled.
*        So, every reset send an e-mail. 
*        When you are confident, change define TEST to false.
*
*  Author: Daniele Denaro
*/

#include "RAComm.h"     // Use Raspberry command library

#define analog 1        // analogic pin used
#define LOGFILE "/home/pi/values.dat" // log file name
#define TEXTMAIL "/home/pi/mailval.txt" // log file name


#define Q "\""                            // escape character for quote(")
#define SENDTO "..."                      // addressee (ex.: smith@gmai.com) 
#define SUBJECT "Value"                   // subject

//Commands:
// Raspberry command to get timestamp
#define CMD1 "GetRTC -s" 
// Command that uses "tail" linux command to extract last 5 lines from log whereby making text to send
#define CMD2 "tail -n 5 " LOGFILE " > " TEXTMAIL
// Raspberry command "SendMail" text (NB. text in quote has to be inserted using escape char) 
#define CMD3 "SendMail mailto=" SENDTO " subject=" Q SUBJECT Q " filemess=" Q TEXTMAIL Q
// Raspberry command for alarm setting (use in sprintf statment)
#define CMD4 "SetRestartAt -sd %d %d"
// Shutdown command
#define CMD5 "sudo shutdown -h now"

#define TEST true    // Test flag. When true, shutdown and checkTime are disabled
                     // Every restart send e-mail

RAComm Cmd;          // Cmd is the RAComm class

struct collecttime
{
  int hh;
  int mm;
}; 
collecttime ctime={16,0}; // 16:00 is the time when analogical value is read

int blinktime;            // blinking interval for Arduino led 
int led=1;                // led on
char rbuff[64];           // buffer for Raspberry command replay
int len=64;               // its length


void setup() 
{
  boolean ok;
  initLed();                   // standard Arduino led used as flag 
  Cmd.begin();                 // Raspberry command dialog initialization 
  delay(100);             
  for (int i=0;i<20;i++)       // delay just if Raspberry is in startup phase
     {ok=Cmd.isReady();if (ok) break;Cmd.begin();delay(100);}
   if (!ok) {noListen();return;}                   // if no dialog no action
   
  ok=Cmd.sendCommand(CMD1);                        // send command to get timestamp
   if (!ok) {error();return;}
  char *stime=Cmd.getAnswer(rbuff,len);            // get replay  
   if (stime==NULL) {error();return;}
  int hh=0; int mm=0;
  sscanf(stime,"%*d-%*d-%*d %d:%d:%*d",&hh,&mm);   // extract hh and mm from timestamp
  if (!checkTime(hh,mm)) {sleep(0);return;}        // if time is not expired yet : sleep 

  int va=analogRead(analog);                       // read value
  char rec[60];snprintf(rec,60,"%s %d",stime,va);  // make record with timestamp and value
  ok=Cmd.openFile(LOGFILE,"a","755");              // open file in append mode and linux protection code 755
   if (!ok) {error();return;}
  Cmd.writeRec(rec);                               // write record
  Cmd.closeFile();                                 // close file
  ok=Cmd.sendCommand(CMD2);                        // extract last records and make mail text
   if (!ok) {error();return;}
  ok=Cmd.sendCommand(CMD3);                        // send file as mail
  sleep(1);                                        // set next wakeup and start shutdown
  Cmd.close();                                     // close dialog 
}

void loop() 
{
  digitalWrite(13,led);
  led=led^1;
  delay(blinktime);
}

boolean checkTime(int hh,int mm)
{
  if (TEST) return true;                         // if TEST, no checkTime
  int tmin=hh*60+mm;                             // present time in minutes
  int ctmin=ctime.hh*60+ctime.mm;                // collect time in minutes
  if (abs(tmin-ctmin)>5) {return false;}         // if time is not expired yet (5 min tollerance) 
  else return true;
}

void sleep(int type)
{
  int mm;
  if (type==1) mm=ctime.mm+20;                    // to overtake the present wackeup
  else mm=ctime.mm;
  char cmd4[64];snprintf(cmd4,64,CMD4,ctime.hh,mm); 
  boolean ok=Cmd.sendCommand(cmd4);               // set restart at hh mm
  if (!ok) {error();return;}
  if (TEST) return;                               // if TEST, no shutdown 
  Cmd.sendCommand(CMD5);                          // shutdown
}

void initLed()
{
  pinMode(13,OUTPUT);
  blinktime=1000;                    // if everything is ok binking at 1 second
}

void noListen()
{
  blinktime=100;
}

void error()
{
  blinktime=300;
}



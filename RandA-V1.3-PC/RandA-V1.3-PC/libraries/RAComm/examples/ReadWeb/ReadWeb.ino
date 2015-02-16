
/*****************************************************************************************************/
/*  
*  This simple example show the basic use of dialog from Arduino to Raspberry.
*  This sketch uses linux command "curl" that sends a request to an URL on Internet.
*  The request is sent to "api.openweathermap.org" site. This site can replay with weather information 
*  for several world places.
*  After command, it reads the answer and scans it to detect temperature value. This value can be used 
*  to set heating, for instance.
*  This process occurs one time at every restart.  
*  
*  Author: Daniele Denaro
*/

#include "RAComm.h"       // Use Raspberry command library

/* Choose city uncomment or insert new town */
//#define CITY "London,uk"
//#define CITY "Paris,fr"
//#define CITY "Berlin,de"
//#define CITY "Madrid,es"
#define CITY "Roma,it"
//#define CITY "Milano,it"
//#define CITY "Napoli,it"
//#define CITY "Catania,it"


// Raspberry command to send a request to a URL
#define CMD1 "curl -i http://api.openweathermap.org/data/2.5/weather?q=" CITY 

#define DEB true        // debug flag
#define LENBUFF 64      // buffer length for answer reading

RAComm Cmd;             // Cmd is the RAComm class

char rbuff[LENBUFF];    // buffer for Raspberry command answer

int flagStatus=0;       // signaling status using standard Arduino led
int led;

/****************** struct and functions prototypes used to extract data ***************/
/*
* This is a general algorithm that can be used in other context.
* Functions used from context:
* - createPattArray() : defines the number of patterns to detect in parallel
* - setPattern() : define single pattern giving index, pattern string of starting value,
*                  character indicating the end of value and the buffer length of value
* - checkPattern() : function that detect patterns and collect values from a record;
*                    records can be several and patterns can span records
* - getPattVal() and getPattValN() : to get back values after scanning. 
*                                    In string or float format.
*/
typedef struct pattern
{
    int lenk;
    char endv;
    int lenv;
    int nkey;
    int nval;
    unsigned char stat;
    char *key;
    char *value;
} patt;

struct patta
{int npatt;patt *ptn;}pattarray;

void checkPattern(patt ptn[],char buff[]);
void setPattern(int index,char key[],char endValue,int valueLen);
void checkKeyVal(patt *p, char b);
void createPattArray(int numofpatt);
char* getPattVal(int index);
float getPattValN(int index);
/***************************************************************************************/

void setup() 
{
  initLed();                           // standard Arduino led used as flag 
                                       
  createPattArray(5);                  // define an array of patterns
  setPattern(0,"\"temp\":",',',10);    // define each pattern
  setPattern(1,"\"temp_min\":",',',10);
  setPattern(2,"\"temp_max\":",'}',10);
  setPattern(3,"\"humidity\":",',',10);
  setPattern(4,"\"pressure\":",',',10);
  
  getWeather();
  
}

void loop() // just signaling status (blinking: n times with millisec of period)
{
  switch (flagStatus) // 1: no dialog with Raspberry ; 2: error in dialog
  {
    case 1 : blinking(50,100);break;
    case 2 : blinking(10,500);break;
  }
  delay(2000); 
}

int getWeather()
{
  boolean ok;
  Cmd.begin();                 // Raspberry command dialog initialization 
  delay(500);                  // delay (just for tidiness)
  for (int i=0;i<20;i++)       // Max 20 checks. If not ready something is wrong!
     {ok=Cmd.isReady();if (ok) break;Cmd.begin();delay(100);}
  if (!ok) {noListen();return -1;}             // if no dialog no action

  Cmd.openFileWrite("/home/pi/wheather.log");  // open file to record data   
  ok=Cmd.sendCommand(CMD1);                    // send command to get wheter
  if (!ok) {error();return -1;}
  while (Cmd.getAnswer(rbuff,LENBUFF)!=NULL)   // read answer 64 bytes at time
  {
    if (DEB){Cmd.writeRec(rbuff);}             // if DEB write all answer on the file
    checkPattern(pattarray.ptn,rbuff);         // check every patterns and collect data
  } 
  // Write data on file: temperature,tem. min.,temp. max.,humidity,pressure
  char rec[30];                                // use buffer to create record for file
  float v=0;
  char sval[10];                               // read temperature as float and get new string 
                                               // because Atmel compiler doesn't use %f format
  v=getPattValN(0);v=v-273.15;dtostrf(v,5,1,sval);
  snprintf(rec,30,"Temperature: %s C",sval);Cmd.writeRec(rec);
  v=getPattValN(1);v=v-273.15;dtostrf(v,5,1,sval);
  snprintf(rec,30,"Temp-min   : %s C",sval);Cmd.writeRec(rec); 
  v=getPattValN(2);v=v-273.15;dtostrf(v,5,1,sval);
  snprintf(rec,30,"Temp-max   : %s C",sval);Cmd.writeRec(rec);  
  snprintf(rec,30,"Humidity   : %s%%",getPattVal(3));Cmd.writeRec(rec);
  snprintf(rec,30,"Pressure   : %s P",getPattVal(4));Cmd.writeRec(rec);
  Cmd.close();                                     // close dialog 
}

/* just for signaling with standard Arduino led */
void initLed()
{pinMode(13,OUTPUT);}

void noListen()
{blinking(50,100);flagStatus=1;}

void error()
{Cmd.close(); blinking(10,500);flagStatus=2;}

void blinking(int n,int blinktime)
{
  int i;
  for (i=0;i<n;i++)
  {digitalWrite(13,led); led=led^1;delay(blinktime);}
  led=1; 
}

/************************** Pattern functions ******************************/

void setPattern(int index,char key[],char endValue,int valueLen)
{
    if (index>=pattarray.npatt) return;
    int lk=strlen(key);
    patt *p=&pattarray.ptn[index];
    p->lenk=lk;
    p->endv=endValue;
    p->lenv=valueLen;
    p->nkey=0;p->nval=0;p->stat=0;
    p->key=(char*)malloc(sizeof(char)*lk);strcpy(p->key,key);
    p->value=(char*)malloc(sizeof(char)*valueLen);
    p->value[0]=0;
}

void checkKeyVal(patt *p, char b)
{
   if (p->stat==0)
   {
     if (b==p->key[p->nkey])
       {
         if (p->nkey==p->lenk-1) {p->stat=1;return;}
         else {p->nkey++;}
       }
      else p->nkey=0;
    }
    if (p->stat==1)
    {
      if (b==p->endv) {p->stat=2;p->value[p->nval]=0;}
      else
       {
         p->value[p->nval]=b;p->nval++;
         if (p->nval==p->lenv-2) {p->stat=2;p->value[p->nval]=0;}
       }
    }
}

void checkPattern(patt ptn[],char buff[])
{
    int i,k;
    int nk=pattarray.npatt;
    int len=strlen(buff);
    for (i=0;i<len;i++)
    {
      if (buff[i]<20) continue;
      for(k=0;k<nk;k++)
      {
         patt *p=&ptn[k];
         checkKeyVal(p,buff[i]);
      }
    }
}

void createPattArray(int numofpatt)
{
    pattarray.ptn=(patt*)malloc(sizeof(patt)*numofpatt);
    pattarray.npatt=numofpatt;
}

char* getPattVal(int index)
{
    return pattarray.ptn[index].value;
}

float getPattValN(int index)
{
    double v=0;
    v=atof(getPattVal(index));
    return (float)v;
}



/*
  Copyright (c) 2014 Daniele Denaro.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*/

/******************************************************************************/
/*
*                Library for RandA Commands Sending
*
*    
*    Author: Daniele Denaro
*    Version: 1.0
*/
/******************************************************************************/

#include "RAComm.h"


/********************** Main functions ****************************************/

/*
* Open serial connection to Raspberry.
* If already open serial speed is modified to 'BAUD' define
*/
void RAComm::begin()
{
         Serial.begin(BAUD);
         Serial.setTimeout(SERTIMEOUT);
         pinMode(LISTENPIN,OUTPUT);
         digitalWrite(LISTENPIN,LOW);
         delay(TWAIT);
         digitalWrite(LISTENPIN,HIGH);
         delay(TWAIT);
         digitalWrite(LISTENPIN,LOW);
         delay(TWAIT);
}

/*
* Test command server om Raspberry if active.
*/
boolean RAComm::isReady()
{
          command(PREFC);
          return getReply();           
}

/*
* Send command to Raspberry.
* If success it returns true.
* Possible answer (or error) can be rertrieved using getAnswer() function.
*/
bool RAComm::sendCommand(char cmd[])
{
          command(PREFC,cmd);
          return getReply(); 
}

/*
* Return command reply (if any).
*/
char* RAComm::getAnswer(char buff[],int len)
{
          buff[0]=0;
          int n;
          char slen[6];itoa(len,slen,10);
          command(PREFR,slen);        
          n=readSerialLn(buff,len);
          if (n<0) return NULL;
          buff[n]=0;
          return buff;
}

/*
* Close communication (and stop listen process on Raspberry) 
* 
*/
void RAComm::close()
{
          command(PREFE); 
          Serial.end();
}

/*
* Short form to open file for read access.
* Same as openFile(name,"r",NULL)
*/
bool RAComm::openFileRead(char name[])
{
          return openFile(name,"r",NULL);
}

/*
* Short form to open file for write access.
* Same as openFile(name,"w","755"). 
* Permission: owner=all; group,other=read and execute
*/
bool RAComm::openFileWrite(char name[])
{
          return openFile(name,"w","755");
}

/*
* Open file for read or write.
* Parameters:
*   name: file path (Ex.: "/home/pi/myfile.txt")
*   mode: one of c mode (r or w or a etc.) (Ex: "r")
*   prot: linux file permission in string format of numeric code (Ex.: "755")
*         or null if don't care
* Return: true if OK or false if any problem.  
*/    
bool RAComm::openFile(char name[],char mode[], char prot[])
{
          cleanSBuff();
          Serial.print(PREFOF);
          Serial.print(name);Serial.print(" ");
          if (mode!=NULL) {Serial.print(mode);Serial.print(" ");}
          if (prot!=NULL) {Serial.print(prot);Serial.print(" ");}
          Serial.print("\n");
 //         Serial.flush();
          return getReply();           
}

/*
* Read record into buffer rec of reclen length.
* When EOF is reached returns NULL else returns rec
* When EOF is reached, file is automatcally closed.
*/
char* RAComm::readRec(char rec[],int reclen)
{
          rec[0]=0;
          char slen[6];itoa(reclen,slen,10);                 
          command(PREFRF,slen);
          int n=readSerialLn(rec,reclen); 
          if (n<0) return NULL;         
          rec[n]=0;
          return rec;
}

/*
* Write record (rec parameter without linefeed).
* When you want stop write and close file, use NULL parameter
*/
bool RAComm::writeRec(char rec[])
{
          command(PREFWF,rec);
          return getReply();                     
}

/*
* Close file.
* Just if you want to close reading file before EOF.
*/    
bool RAComm::closeFile()
{
          command(PREFCF);
          return getReply();
}

/*
* Open a XConsole on Raspberry to write data or receive input or just a debug of serial communication
*/
bool RAComm::openConsole()
{
          command(PREFOW);
          return getReply();
}

/*
* Read a line from console into rec buffer. If no input it returns NULL 
*/
char* RAComm::readConsole(char rec[],int reclen)
{
          rec[0]=0;
          char slen[6];itoa(reclen,slen,10);          
          command(PREFRW,slen);
          int n=readSerialLn(rec,reclen);          
          if (n<0) return NULL;
          rec[n]=0; 
          return rec;
}

/*
* Write a line to console.
*/
bool RAComm::writeConsole(char rec[])
{
          command(PREFWW,rec);
          return getReply();                     
}

/*
* Close the XConsole
*/
bool RAComm::closeConsole()
{
          command(PREFCW);
          return getReply();
}

/*
* Set ON the serial transmission repeater to XConsole for debbugging (open a XConsole if not yet open).
*/
bool RAComm::setEcho()
{
          command(PREFEH);
          return getReply();
}

/*
* Set timeout for answer waiting. Default in RAComm.h (1 sec)
*/
void RAComm::setTimeout(int millisec)
{         
          Serial.setTimeout(millisec);
}


/*************************** END LIBRARY **************************************/

void RAComm::command(char *pre)
{ 
  command(pre,NULL);
}

void RAComm::command(char *pre,char *arg)
{
  cleanSBuff();
  Serial.print(pre);
  if (arg!=NULL) {Serial.print(arg);}
  Serial.print("\n");
 // Serial.flush();
}

int RAComm::readSerialLn(char buff[],int len)
{
  int n=Serial.readBytesUntil('\n',buff,len-1);
  if (n>0) {if (buff[0]==0x04) return -1;}
  if (n>0) {if (buff[n-1]!='\n') buff[n]='\n';}
  return n;
}

void RAComm::cleanSBuff()
{
  while (Serial.available()>0) Serial.read();
}

boolean RAComm::getReply()
{
  boolean ret=false;
  char buff[5];
  readSerialLn(buff,5);          
  if (strncmp(buff,"OK",2)==0) ret=true;
  cleanSBuff();
  return ret;
}




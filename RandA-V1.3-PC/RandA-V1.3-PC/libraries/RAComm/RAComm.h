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
/*********************** Included in library **********************************/
#ifndef RAComm_h
#define RAComm_h

#include <Arduino.h>

//#define BAUD   115200       //Serial speed
//#define BAUD   57600       //Serial speed
#define BAUD   9600       //Serial speed

#define PREFC   "$$$Ardu "    //Protocol code command
#define PREFR   "$$$Resp "    //Protocol code reply
#define PREFE   "$$$Cend "    //Protocol code end
#define PREFOF  "$$$Open "    //Protocol code open file
#define PREFRF  "$$$Read "    //Protocol code read record
#define PREFWF  "$$$Wrte "    //Protocol code write record
#define PREFCF  "$$$Clse "    //Protocol code close file
#define PREFOW  "$$$Opnw "    //Protocol code open xconsole
#define PREFCW  "$$$Clsw "    //Protocol code close xconsole
#define PREFWW  "$$$Wtow "    //Protocol code write to wconsole
#define PREFRW  "$$$Rfmw "    //Protocol code read from xconsole
#define PREFEH  "$$$Echo "    //Protocol repeat serial to xconsole

#define LISTENPIN 2

#define SERTIMEOUT 1000
#define TWAIT 20

#define DEB true

class RAComm
{

public:

/********************************** Main functions ****************************/

/*
* Open serial connection to Raspberry.
* If already open serial speed is modified to 'BAUD' define
*/
    void begin();

/*
* Test command server om Raspberry if active.
*/
    boolean isReady();

/*
* Send command to Raspberry.
* If success it returns true.
* Possible answer (or error) can be rertrieved using getAnswer() function.
*/
    bool sendCommand(char cmd[]);
    
/*
* Return command reply (if any).
*/
    char* getAnswer(char buff[],int len);
    
/*
* Close communication (and stop listen process on Raspberry) 
* 
*/
    void close();

/*
* Open file for read or write.
* Parameters:
*   name: file path (Ex.: "/home/pi/myfile.txt")
*   mode: one of c mode (r or w or a etc.) (Ex: "r")
*   prot: linux file permission in string format of numeric code (Ex.: "755")
*         or null if don't care
* Return: true if OK or false if any problem.  
*/    
    bool openFile(char name[],char mode[], char prot[]);

/*
* Short form to open file for read access.
* Same as openFile(name,"r",NULL)
*/
    bool openFileRead(char name[]);

/*
* Short form to open file for write access.
* Same as openFile(name,"w","755"). 
* Permission: owner=all; group,other=read and execute
*/
    bool openFileWrite(char name[]);

/*
* Read record into buffer rec of reclen length.
* When EOF is reached returns NULL else returns rec
* When EOF is reached, file is automatcally closed.
*/
    char* readRec(char rec[],int reclen);

/*
* Write record (rec parameter without linefeed).
* When you want stop write and close file, use NULL parameter
*/
    bool writeRec(char rec[]);
    
/*
* Close file.
* Just if you want to close reading file before EOF.
*/    
    bool closeFile();
    
/*
* Open a XConsole on Raspberry to write data or receive input or just a debug of serial communication
*/
    bool openConsole();

/*
* Read a line from console into rec buffer. If no input it returns NULL 
*/
    char* readConsole(char rec[],int reclen);

/*
* Write a line to console.
*/
    bool writeConsole(char rec[]);

/*
* Close the XConsole
*/
    bool closeConsole();

/*
* Set ON the serial transmission repeater to XConsole for debbugging (open a XConsole if not yet open).
*/
    bool setEcho();
    
/*
* Set timeout for answer waiting. Default in RAComm.h (1 sec)
*/
    void setTimeout(int millisec);
    
private:

    void command(char *pre);
    void command(char *pre,char *arg);
    int readSerialLn(char buff[],int len);        
    void cleanSBuff();
    boolean getReply();
     
};



#endif

/*
* For serial command.
* Commands always terminated by LF(\n) or CRLF
* Always answered back : a value or OK or NOK(if error) with LF
* Commands:
* Reading
*  analogical pins : RAn (where n pin number 0-5)      returns the value       -1(if error)
*  digital pins    : RDn (where n: 2-13)               returns the value (0/1) -1(if error)
* Writing
*  analogical (PWM) pins : WAn=v (where n 3,5,6,9,10,11 and v: value(0-1024)) returns OK/NOK
*  digital pins          : WDn=v (where n: 2-13 and v: value(0/1))            returns OK/NOK
* Pin setting
*  digital pin setting : SPn=INP|OUT|IPL (where n: (2-13))          returns OK/NOK
*                        (INP: INPUT, OUT: OUTPUT, IPL: INPUT_PULLUP)
* Pin status
*  read pin status : PA or PN   
*        if PN returns 3 integer with spaces (DIR register, PORT register, IN register) each bit represents value of pin (0-13)
*        if PA returns 13 couples of values (inside bracket '[' ']' ) multidim. array json format; 
*           each couple of values (comma separated) represent direction and value of pin. 
*        Ex.: 0,0 -> as input and 0 read
*             2,1 -> as pullout input and 1 read
*             1,0 -> as output and switched off
* Read all digital inp
*  read all digital input : D  return [0|1,...,0|1] (json array 13 length)
*/

long bauds=9600;  // serial speed

char EL='\n';
char buffer[16];
int DD;
int DS;
int DI;


void setup() {
  Serial.begin(bauds);
}

void loop() 
{
  int n=Serial.readBytesUntil(EL,buffer,24);
  if(n>0) {buffer[n]='\0';decode();}
}

void readPinStatus()
{
  DD=word(DDRB,DDRD);
  DS=word(PORTB,PORTD);
  DI=word(PINB,PIND);
  int i;
  for (i=0;i<16;i++){Serial.print(bitRead(DD,i));Serial.print(" ");} Serial.println();
  for (i=0;i<16;i++){Serial.print(bitRead(DS,i));Serial.print(" ");} Serial.println();
  for (i=0;i<16;i++){Serial.print(bitRead(DI,i));Serial.print(" ");} Serial.println();
  
}

void decode()
{
  char c=toupper(buffer[0]);
  if (c=='R') {cmdRead();return;}
  if (c=='W') {cmdWrite();return;}
  if (c=='S') {cmdSet();return;}
  if (c=='P') {cmdPinStatus();return;}
  if (c=='D') {cmdAllDigRead();return;}
}

void cmdRead()
{
  if (strlen(buffer)<3) return;
  char c=toupper(buffer[1]);
  if (c=='A') {anaRead();return;}
  if (c=='D') {digRead();return;}
}

void anaRead()
{
  int val=-1;
  int n=atoi(&buffer[2]);
  if ((n>=0)&(n<=5)) val=analogRead(n);
  Serial.println(val);
}

void digRead()
{
  int val=-1;
  int n=atoi(&buffer[2]);
  if ((n>=2)&(n<=13)) val=digitalRead(n);
  Serial.println(val);
}

void cmdAllDigRead()
{
  DI=word(PINB,PIND);
  char d[32];
  byte D[14];
  int i;
  for (i=0;i<14;i++) D[i]=bitRead(DI,i);
  sprintf(d,"[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]",
             D[0],D[1],D[2],D[3],D[4],D[5],D[6],D[7],D[8],D[9],D[10],D[11],D[12],D[13]);
  Serial.println(d);           
}

void cmdWrite()
{
  if (strlen(buffer)<4) return;  
  char c=toupper(buffer[1]);
  if (c=='A') {anaWrite();return;}
  if (c=='D') {digWrite();return;}  
}

void anaWrite()
{
  char *sep=strchr(&buffer[2],'=');
  if (sep==NULL) {Serial.println("NOK");return;}
  *sep='\0';
  int n=atoi(&buffer[2]);
  int val=atoi(sep+1);
  switch (n)
  {
    case 3: analogWrite(3,val);break;
    case 5: analogWrite(5,val);break;
    case 6: analogWrite(6,val);break;
    case 9: analogWrite(9,val);break;
    case 10:analogWrite(10,val);break;
    case 11:analogWrite(11,val);break;
    default: {Serial.println("NOK");return;}
  }
  Serial.println("OK");
}

void digWrite()
{
  char* sep=strchr(&buffer[2],'=');
  if (sep==NULL) {Serial.println("NOK");return;}
  *sep='\0';
  int n=atoi(&buffer[2]);
  int val=atoi(sep+1); 
  if ((n>=2)&(n<=13)) {digitalWrite(n,val); Serial.println("OK");}
  else Serial.println("NOK");
}

void cmdSet()
{
  if (strlen(buffer)<4) return;
  char c=toupper(buffer[1]);
  if (c!='P') {Serial.println("NOK");return;}
  char* sep=strchr(&buffer[2],'=');
  if (sep==NULL) {Serial.println("NOK");return;}
  *sep='\0';char* set=++sep;
  if (strlen(set)<3) {Serial.println("NOK");return;}
  int n=atoi(&buffer[2]);
  int val=-1;
  if (strncmp(set,"INP",3)==0) val=0;
  if (strncmp(set,"OUT",3)==0) val=1;
  if (strncmp(set,"IPL",3)==0) val=2;
  if (val<0) {Serial.println("NOK");return;}
  if ((n>=2)&(n<=13)) {pinMode(n,val); Serial.println("OK");}
  else Serial.println("NOK");
}

void cmdPinStatus()
{
  DD=word(DDRB,DDRD);
  DS=word(PORTB,PORTD);
  DI=word(PINB,PIND);
  
  if (strlen(buffer)<2) return;
  if (buffer[1]=='N') 
  {char sout[24]; sprintf(sout,"%d %d %d",DD,DS,DI);Serial.println(sout);}
  if (buffer[1]=='A')
  {
    char bout[6]={'[','0',',','0',']','\0'};
    int i; 
    Serial.print("[[0,0],[0,0],"); 
    for (i=2;i<14;i++)
    {
      if (bitRead(DD,i)==0)
       {
        {if (bitRead(DS,i)==0) bout[1]='0';else bout[1]='2';}
        {if (bitRead(DI,i)==0) bout[3]='0';else bout[3]='1';}
        Serial.print(bout);
       }
       else
      {
         bout[1]='1';
         {if (bitRead(DS,i)==0) bout[3]='0';else bout[3]='1';}
         Serial.print(bout);
       }
       if(i<13) Serial.print(",");
     }
     Serial.println("]");
   }
}





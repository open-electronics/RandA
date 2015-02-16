/*********************************************************************************************************/
/*
* This sketch can be used to test serial communication.
* Is a blink program where blink period can be set by serial command.
* It blinks standard Arduino led (pin 13)
* Insert time in milliseconds and send data.
*
* Author Daniele Denaro
*/

void setup() 
{
  Serial.begin(9600);
  Serial.println("Start...");
  Serial.println("To modify led period send number (millis)");
  pinMode(13,OUTPUT);
}

int led=1;
int p=500;

void loop() 
{
 if (Serial.available()>3) {p=Serial.parseInt();Serial.print("Now period: ");Serial.println(p);}
 if (p==0) p=500;
 digitalWrite(13,led);
 led=led^1;
 delay(p);
}

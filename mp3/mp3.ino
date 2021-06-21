///              MP3 PLAYER PROJECT
/// http://educ8s.tv/arduino-mp3-player/
//////////////////////////////////////////


#include "SoftwareSerial.h"
SoftwareSerial mySerial(10, 11);
#include <Wire.h>
#include <RTClib.h>

# define Start_Byte 0x7E
# define Version_Byte 0xFF
# define Command_Length 0x06
# define End_Byte 0xEF
# define Acknowledge 0x00 //Returns info with command 0x41 [0x01: info, 0x00: no info]

//pins
const int BUSY = 3;
const int PLAYorPAUSE = 2;
String  State = "play";
String Time="";
String CurrentSong="";
boolean isPlaying = false;

RTC_DS3231 rtc;

void setup () {
  Serial.begin(9600);
  mySerial.begin (9600);
 if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.lostPower()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));


 pinMode(BUSY,INPUT);
setVolume(30);
delay(1000);

}



void loop () { 

 DateTime now = rtc.now();

  //buffer can be defined using following combinations:
  //hh - the hour with a leading zero (00 to 23)
  //mm - the minute with a leading zero (00 to 59)
  //ss - the whole second with a leading zero where applicable (00 to 59)
  //YYYY - the year as four digit number
  //YY - the year as two digit number (00-99)
  //MM - the month as number with a leading zero (01-12)
  //MMM - the abbreviated English month name ('Jan' to 'Dec')
  //DD - the day as number with a leading zero (01 to 31)
  //DDD - the abbreviated English day name ('Mon' to 'Sun')

   char buf1[] = "hh:mm";
   Time= now.toString(buf1);
   Serial.println(now.toString(buf1));

   if((Time=="23:28" || Time=="12:00" || Time == "06:00" || Time == "18:00") && digitalRead(BUSY) == 1){
    Regina();
    CurrentSong="Regina";
   }

   if(digitalRead(PLAYorPAUSE)==LOW && State == "play" ){
    pause();
   }

   if(digitalRead(PLAYorPAUSE)==LOW && State == "pause"){
    play();
   }

//DateTime FiveMins = now+ TimeSpan(0,0,5,0);
//Time = FiveMins.toString(buf1);
//Serial.print("Time + 5mins:");
//Serial.println(Time);
Serial.print("busy:");
Serial.println(digitalRead(3));
   delay(1000);
  
}

void Regina()
{
  execute_CMD(0x03, 0, 7);
  delay(500);
  
}
void DivineMercy()
{
  execute_CMD(0x03, 0, 6);
  delay(500);
  
}
void Angelus()
{
  execute_CMD(0x03, 0, 5);
  delay(500);
  
}
void Glorious()
{
  execute_CMD(0x03, 0, 4);
  delay(500);

}
void Joyful() // play Joyful Mysteries
{
  execute_CMD(0x03, 0, 3);
  delay(500);
 

}
void Light()
{
  execute_CMD(0x03, 0, 2);
  delay(500);

}

void Sorrowful()
{
  execute_CMD(0x3F, 0, 0);
  delay(500);
  setVolume(30);
  
}
void pause()
{
  execute_CMD(0x0E,0,0);
  delay(500);
  State= "pause";
}

void play()
{
  execute_CMD(0x0D,0,1); 
  delay(500);
  State= "play";
}

void playNext()
{
  execute_CMD(0x01,0,1);
  delay(500);
}

void playPrevious()
{
  execute_CMD(0x02,0,1);
  delay(500);
}

void setVolume(int volume)
{
  execute_CMD(0x06, 0, volume); // Set the volume (0x00~0x30)
  delay(2000);
}

void execute_CMD(byte CMD, byte Par1, byte Par2)
// Excecute the command and parameters
{
// Calculate the checksum (2 bytes)
word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
// Build the command line
byte Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte};
//Send the command line to the module
for (byte k=0; k<10; k++)
{
mySerial.write( Command_line[k]);
}
}

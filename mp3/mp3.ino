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
const int snooze_button = 4;

// Variables
String Time = "";
String _Day = "";
String CurrentSong = "";
String Snooze_time = "";


RTC_DS3231 rtc;

// function to add 5 mins when snooze button is pressed
String Snooze(String TIME) {
  const int snooze_amount = 5;
  const int max_min = 60;
  int Hr = Time.substring(0, 2).toInt();
  int Sc = Time.substring(3, 5).toInt();
  //  Serial.print("Hr =");
  //  Serial.println(Hr);
  //  Serial.print("Sc =");
  //  Serial.println(Sc);

  // add the snooze amount to number of seconds from time 
  // deduct 60 from the sum of the seconds + snooze amont 
  // if it is <= 0 then the minute is greater than 60 the add  1 hour to Hr(Hour from time)
  // if it is =0 then the Sc + snooze_amount equals to 60, which will make a = 60-60 i.e the minute was exactly 60
  int a = max_min - (Sc + snooze_amount); 
  if ( a = 0) {
    // max minute has exceeded
    // add 1 to Hr and the reminder to Sc
    Hr += 1; Sc = a; // 
  }

  else { // else just add the equate Sc to Sc + snooze_amount
    Sc = Sc + snooze_amount; 
  }
  // add leading zero if Seconds is less than 10
  String b = String(Sc);
  if (Sc < 10) {
    b = "0" + String(Sc);
  }
  return String(Hr) + ":" + b; // returns time plus 5mins
}

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
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));


  pinMode(BUSY, INPUT);
  pinMode(PLAYorPAUSE, INPUT_PULLUP);
  pinMode(snooze_button, INPUT_PULLUP);
  setVolume(30);
  delay(1000);
  Regina();

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
  char buf2[] = "DDD";
  Time = now.toString(buf1);
  _Day = now.toString(buf2);
  Serial.println(_Day + " " + Time);


  if ((Time == "23:28" || Time == "12:00" || Time == "06:00" || Time == "18:00") && digitalRead(BUSY) == 1) {
    Regina();

  }

  if (digitalRead(PLAYorPAUSE) == LOW && digitalRead(BUSY) == 0) {
    pause();
  }

  if (digitalRead(PLAYorPAUSE) == LOW && digitalRead(BUSY) == 1) {
    play();
  }

  if (digitalRead(snooze_button) == LOW && digitalRead(BUSY) == 0) { // prayer that is currently playing is snoozed
    pause();
    Snooze_time = Snooze(Time);
    
    Serial.println(CurrentSong + " has been snoozed till " + Snooze_time);
    
  }

  if (digitalRead(snooze_button) == LOW && digitalRead(BUSY) == 1) {
    // if no song is playing and snooze button is pressed,play the rosary for the day
    if (_Day == "Mon" || _Day == "Sat") {
      Joyful();
    }
    if (_Day == "Thu") {
      Light();
    }
    if (_Day == "Tue" || _Day == "Fri") {
      Sorrowful();
    }
    if (_Day == "Wed" || _Day == "Sun") {
      Glorious();
    }
  }
  if(Time==Snooze_time && digitalRead(BUSY) == 1){
    Serial.println("Snoozy");
    // when the current time is equal to snooze time 
    // the audio that was snoozed should play but 
    // while the song is still playing the condition
    //Time==Snooze_time is still true this will cause 
    // arduino to keep sending the command to play
    // that song while the song is still playing,
    // this will cause the song to keep repeating
    //in short the song wil not play properly
    // so the command is only sent when a song is not playing 
    // snooze function is not added for the rosary because it has no reminder 
    if(CurrentSong=="Regina"){
      Regina();
    }

    if(CurrentSong=="Angelus"){
      Glorious();
    }

    if(CurrentSong=="DivineMercy"){
      DivineMercy();
    }
  }

  Serial.print("busy:");
  Serial.println(digitalRead(3));
  delay(1000);

}

void Regina()
{
  execute_CMD(0x03, 0, 7);
  delay(500);
  CurrentSong = "Regina";
  //Reset snnoze time to a 
  // string that can never
  // be a time, so it doesnt
  // ring again when it
  // equals Time again tommorow
  Snooze_time = "#";
}
void DivineMercy()
{
  execute_CMD(0x03, 0, 6);
  delay(500);
  CurrentSong = "DivineMercy";
  Snooze_time = "#";
}
void Angelus()
{
  execute_CMD(0x03, 0, 5);
  delay(500);
  CurrentSong = "Angelus";
  Snooze_time = "#";
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
  execute_CMD(0x0E, 0, 0);
  delay(500);
  Serial.println("paused");
}

void play()
{
  execute_CMD(0x0D, 0, 1);
  delay(500);
  Serial.println("playing");
  Snooze_time = "#";
}

void playNext()
{
  execute_CMD(0x01, 0, 1);
  delay(500);
}

void playPrevious()
{
  execute_CMD(0x02, 0, 1);
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
                            Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte
                          };
  //Send the command line to the module
  for (byte k = 0; k < 10; k++)
  {
    mySerial.write( Command_line[k]);
  }
}

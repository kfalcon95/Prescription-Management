/* Kathleen Falcon
 */
#include "pitches.h"
#include <Countimer.h>
#include <SoftTimer.h>
#include <Servo.h>
#include <LiquidCrystal.h>

Servo servo;
Countimer timer;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int buttonPin = 7;
const int piezoPin = 8;
const int DailyMedTimer = 5;    //5 seconds
const int QuarterSecond = 250;  //250ms
const int melody[] = { NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4 };
const int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };

bool btnFlag = true;
int btnState = 0;
int pillsLeft = 28;

void lcdTimerUpdate(Task *me)
{
  timer.run();
  if(!timer.isCounterCompleted())
      timer.start();
}

void piezoPillTimeChime()
{
    delay(50);  //for debouncing
    btnState = digitalRead(buttonPin);
    (btnState == LOW) ? btnFlag = true : btnFlag = false;
    for (int thisNote = 0; thisNote < 8; thisNote++) 
    {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(piezoPin, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(piezoPin);
    }
}
void refreshClock()
{
  lcd.home();
  lcd.print("                ");  //clear current line ONLY
  lcd.home();
  lcd.print("Time: ");
  lcd.setCursor(6,0);
  lcd.print(timer.getCurrentTime());
}

void onComplete() 
{ 
  servo.writeMicroseconds(1700);     //turn left
  delay(150);
  servo.writeMicroseconds(1500);     //stop 
  lcd.home();
  lcd.print("Time: 00:00:00");
  piezoPillTimeChime();
  pillsLeft -= 1;
  lcd.setCursor(0,1);   //col 1 row 2
  lcd.print("                ");  //clear current line ONLY
  lcd.setCursor(0,1);   //col 1 row 2
  lcd.print("Pills: ");
  lcd.setCursor(7,1);   //col 8 row 2
  (pillsLeft < 0) ? lcd.print("0") : lcd.print(pillsLeft);
  timer.start();
}

Task taskOne(QuarterSecond, lcdTimerUpdate);

void setup()
{
  //initialize button, lcd, servo
  pinMode(buttonPin, INPUT);
  lcd.begin(16, 2);
  servo.attach(9);

  //initialize tasks
  SoftTimer.add(&taskOne);

  //initialize timer
  timer.setCounter(0, 0, DailyMedTimer, timer.COUNT_DOWN, onComplete);
  timer.setInterval(refreshClock, 1000);

  //initialize timer display
  lcd.home();
  lcd.print("                ");  //clear current line ONLY
  lcd.home();
  lcd.print("Time: ");
  lcd.print(timer.getCurrentTime());

  //initialize pill count display
  lcd.setCursor(0,1);   //col 1 row 2
  lcd.print("                ");  //clear current line ONLY
  lcd.setCursor(0,1);   //col 1 row 2
  lcd.print("Pills: ");
  lcd.setCursor(7,1);   //col 8 row 2
  lcd.print(pillsLeft);
}



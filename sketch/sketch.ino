#include <Romi32U4.h>

Romi32U4LCD lcd;
Romi32U4Buzzer buzzer;
Romi32U4ButtonA buttonA;
Romi32U4ButtonB buttonB;
Romi32U4ButtonC buttonC;
Romi32U4Motors motors;
Romi32U4Encoders encoders;

extern int16_t c;
extern int16_t s;


void setup()
{
  lcd.clear();
  lcd.print("Dead");
  lcd.gotoXY(0,1);
  lcd.print("Reckon");
  buzzer.play("l32>c>f"); 
  delay(1000);
  uint16_t v = readBatteryMillivolts();
  lcd.clear();
  lcd.print("Battery:");
  lcd.gotoXY(0,1);
  lcd.print(v);
  lcd.print(" mV");
  delay(1000);

  digitalWrite(12, HIGH);
}

void loop()
{
  static uint16_t i;
  encoderService();
  followLine();

  i++;
  if(i == 100)
  {
    lcd.clear();
    lcd.gotoXY(0,0);
    lcd.print(s);
    
    lcd.gotoXY(0,1);
    lcd.print(c);
    i = 0;
  }
}

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
extern int32_t x;
extern int32_t y;
extern int32_t target_s;
uint16_t last_on_line_millis = 0;
uint16_t on_line_start_millis = 0;

void setup()
{
  lcd.clear();
  lcd.print("Dead");
  lcd.gotoXY(0,1);
  lcd.print("Reckon");
  buzzer.play("v4l32>c>f"); 
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
  static uint8_t state = 0;
  encoderService();
  followLine();

  switch(state) {
  case 0: // drive straight to line    
    readSensors();
    if(onLine())
    {
      state++;
      on_line_start_millis = millis();
    }
    motors.setSpeeds(100,100);
    break;
  case 1: // follow line
    followLine();
    if(onLine())
      last_on_line_millis = millis();
    if(millis() - on_line_start_millis > 1000 && millis() - last_on_line_millis > 1000) // change 1st # to 5000 later
    {
      transform();
      state++;
    }
    break;
  case 2: // homing
    goHome();
    if(x > -1500000)
    {
      state++;
    }
    break;
  case 3: // done
    motors.setSpeeds(0,0);
    break;
  }
  
  i++;
  if(i == 100)
  {
    lcd.clear();
    lcd.gotoXY(0,0);
    lcd.print(s/200);
    
    lcd.gotoXY(4,0);
    lcd.print(c/200);

    lcd.gotoXY(0,1);
    lcd.print(x/10000L);
    
    lcd.gotoXY(4,1);
    lcd.print(y/10000L);
    
    i = 0;
  }
}

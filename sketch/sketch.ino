#include <Romi32U4.h>

Romi32U4LCD lcd;
Romi32U4Buzzer buzzer;
Romi32U4ButtonA buttonA;
Romi32U4ButtonB buttonB;
Romi32U4ButtonC buttonC;
Romi32U4Motors motors;
Romi32U4Encoders encoders;
uint16_t sensor[4];

extern int16_t c;
extern int16_t s;

uint16_t calibratedRead(uint8_t port, uint16_t min_value, uint16_t max_value)
{
  uint16_t v = analogRead(port);
  if(v < min_value)
    return 0;
  if(v > max_value)
    return 1024;
  return (v-min_value)*1024/(max_value-min_value);
}

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
  int i=0;
  while(1)
  {
    sensor[0] = calibratedRead(4,650,850);
    sensor[1] = calibratedRead(3,580,820);
    sensor[2] = calibratedRead(2,520,750);
    sensor[3] = calibratedRead(0,760,900);
    
    int16_t pos;
    static int16_t last_pos;
    uint8_t on_line = sensor[0] + sensor[1] + sensor[2] + sensor[3] > 512 && sensor[0] + sensor[1] + sensor[2] + sensor[3] < 3000;
    if(!on_line)
    {
      if(last_pos < 0)
        pos = -2000;
      else
        pos = 2000;
    }
    else
    {
      pos = -2*sensor[0]-sensor[1]+sensor[2]+2*sensor[3];
      if(pos < -2000)
        pos = -2000;
      if(pos > 2000)
        pos = 2000;
    }

    int16_t s = pos/20 - (pos - last_pos)/4;
    last_pos = pos;

    if(on_line)
      motors.setSpeeds(100+s, 100-s);
    else
      motors.setSpeeds(0,0);
    
    i++;
    if (i >= 100)
    {
      i=0;
      lcd.clear();
      lcd.print(pos);
    }
  }
}

void loop()
{
  static uint16_t i;
  encoderService();

  i++;
  if(i == 10000)
  {
    lcd.clear();
    lcd.gotoXY(0,0);
    lcd.print(s);
    
    lcd.gotoXY(0,1);
    lcd.print(c);
    i = 0;
  }
}

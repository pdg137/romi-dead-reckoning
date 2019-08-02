#include <Romi32U4.h>
#include <EEPROM.h>

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
uint16_t start_millis = 0;
uint16_t last_on_line_millis = 0;
uint16_t on_line_start_millis = 0;
uint8_t state = 0;

void setup()
{
  digitalWrite(12, HIGH);
  if(buttonA.isPressed())
    calibrate();
  loadCalibration();
  
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
  
  sensorDebug2();
  lcd.clear();
  lcd.print("Ready...");
  buzzer.play("l32>f"); 
  delay(1000);
  buzzer.play(">>c"); 
  start_millis = millis();
}

void testCircle()
{
  int i=0;
  motors.setSpeeds(50,100);
  while(1)
  {
    encoderService();
    switch(i)
    {
    case 0:
      if(c < 0) i++;
      break;
    case 1:
      if(s < 0) i++;
      break;
    case 2:
      if(c > 0) i++;
      break;
    case 3:
      if(s > 0)
      {
        motors.setSpeeds(0,0);
        while(1);
      }
    }
    debug();
  }
}

void debug()
{
  static uint16_t i = 0;
  i++;
  if(i == 1000)
  {
    lcd.clear();
    lcd.gotoXY(0,0);
    lcd.print(s/200);
    
    lcd.gotoXY(4,0);
    lcd.print(state);

    lcd.gotoXY(0,1);
    lcd.print(x/1000000L);
    
    lcd.gotoXY(4,1);
    lcd.print(y/1000000L);
    
    i = 0;
  }
}

void loop()
{
  static uint16_t i;
  encoderService();

if(state == 2)
  ledYellow(1);
  switch(state) {
  case 0: // drive straight to line    
    readSensors();
    if(onLine())
    {
      state++;
      on_line_start_millis = millis();
    }
    motors.setSpeeds(200,190);
    break;
  case 1: // follow line
    followLine();
    if(onLine())
      last_on_line_millis = millis();
    if(millis() - on_line_start_millis > 5000 && millis() - last_on_line_millis > 500)
    {
      transform();
      state++;
    }
    break;
  case 2: // homing
    ledRed(1);
    goHome();
    if(x > -1500000L)
    {
      state++;
    }
    break;
  case 3: // done
    motors.setSpeeds(0,0);
    break;
  case 10:
    uint16_t elapsed_time = millis() - start_millis;
    uint8_t index = elapsed_time/2000;
    int8_t turns[] = {50, 50, -50, 50, 50, -50, -50, 50, 50, 50, -50, -50, 50, -50, -50};
    
    if(index > sizeof(turns))
    {
      buzzer.play("o6l32grgrgrg2");
      transform();
      state = 2;
    }
    else
    {
      motors.setSpeeds(max(100+turns[index],100),max(100-turns[index],100));
    }
    break;
  }
  
  debug();
}

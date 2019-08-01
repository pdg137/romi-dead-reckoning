uint16_t sensor[4];
uint16_t calibration_min[4];
uint16_t calibration_max[4];

bool onLine()
{
  return sensor[0] + sensor[1] + sensor[2] + sensor[3] > 1024;
}

uint16_t calibratedRead(uint8_t port, uint16_t min_value, uint16_t max_value)
{
  uint16_t v = analogRead(port);
  if(v < min_value)
    return 0;
  if(v > max_value)
    return 1024;
  return (v-min_value)*(uint32_t)1024/(max_value-min_value);
}

void loadCalibration()
{
  int i;
  for(i=0;i<4;i++)
    calibration_min[i] = EEPROM.read(2*i) + 256*EEPROM.read(2*i+1);
  for(i=0;i<4;i++)
    calibration_max[i] = EEPROM.read(8+2*i) + 256*EEPROM.read(8+2*i+1);
}

void saveCalibration()
{
  int i;
  for(i=0;i<4;i++)
  {
    EEPROM.write(2*i, calibration_min[i]%256);
    EEPROM.write(2*i+1, calibration_min[i]/256);
  }
  for(i=0;i<4;i++)
  {
    EEPROM.write(8+2*i, calibration_max[i]%256);
    EEPROM.write(8+2*i+1, calibration_max[i]/256);
  }
}

void readSensors()
{
  sensor[0] = calibratedRead(4,calibration_min[0],calibration_max[0]);
  sensor[1] = calibratedRead(3,calibration_min[1],calibration_max[1]);
  sensor[2] = calibratedRead(2,calibration_min[2],calibration_max[2]);
  sensor[3] = calibratedRead(0,calibration_min[3],calibration_max[3]);
}

void calibrateOneSensor(uint8_t index, uint8_t port)
{
  uint16_t value = analogRead(port);
  if(value < calibration_min[index])
    calibration_min[index] = value;
  if(value > calibration_max[index])
    calibration_max[index] = value;
}

void calibrateSensorsOnce()
{
  calibrateOneSensor(0,4);
  calibrateOneSensor(1,3);
  calibrateOneSensor(2,2);
  calibrateOneSensor(3,0);
}

void calibrate()
{
  lcd.clear();
  lcd.print("Cal");
  delay(1000);
  
  uint16_t i;
  for(i=0;i<4;i++)
  {
    calibration_min[i] = 1023;
    calibration_max[i] = 0;
  }
  motors.setSpeeds(-50,50);
  for(i=0; i < 2000; i++)
    calibrateSensorsOnce();
  motors.setSpeeds(50,-50);
  for(i=0; i < 4000; i++)
    calibrateSensorsOnce();
  motors.setSpeeds(-50,50);
  for(i=0; i < 2000; i++)
    calibrateSensorsOnce();
  motors.setSpeeds(0,0);
  saveCalibration();
}

void sensorDebug()
{
  lcd.clear();
  lcd.print("Raw vals");
  delay(500);
  while(1)
  {
    lcd.clear();
    lcd.print(analogRead(4));
    lcd.gotoXY(4,0);
    lcd.print(analogRead(3));
    lcd.gotoXY(0,1);
    lcd.print(analogRead(2));
    lcd.gotoXY(4,1);
    lcd.print(analogRead(0));
    delay(100);

    if(buttonB.getSingleDebouncedPress())
      return;
    if(buttonC.getSingleDebouncedPress())
      break;
  }

  sensorDebug2();
}

void sensorDebug2()
{
  lcd.clear();
  lcd.print("Cal vals");
  delay(500);
  while(1)
  {
    readSensors();
    lcd.clear();
    lcd.print(sensor[0]);
    lcd.gotoXY(4,0);
    lcd.print(sensor[1]);
    lcd.gotoXY(0,1);
    lcd.print(sensor[2]);
    lcd.gotoXY(4,1);
    lcd.print(sensor[3]);
    delay(100);

    if(buttonB.getSingleDebouncedPress())
      return;
    if(buttonC.getSingleDebouncedPress())
      break;
  }
  
  sensorDebug3();
}

void showCalibration()
{
  while(1)
  {
    lcd.clear();
    lcd.print(calibration_min[0]);
    lcd.gotoXY(4,0);
    lcd.print(calibration_min[1]);
    lcd.gotoXY(0,1);
    lcd.print(calibration_min[2]);
    lcd.gotoXY(4,1);
    lcd.print(calibration_min[3]);

    delay(1000);
    
    lcd.clear();
    lcd.print(calibration_max[0]);
    lcd.gotoXY(4,0);
    lcd.print(calibration_max[1]);
    lcd.gotoXY(0,1);
    lcd.print(calibration_max[2]);
    lcd.gotoXY(4,1);
    lcd.print(calibration_max[3]);

    delay(1000);

    if(buttonB.getSingleDebouncedPress())
      return;
    if(buttonC.getSingleDebouncedPress())
      break;
  }

  sensorDebug3();
}

void sensorDebug3()
{
  lcd.clear();
  lcd.print("Line pos");
  delay(500);
  while(1)
  {
    int16_t pos = readLine();
    lcd.clear();
    lcd.print(pos);
    delay(100);

    if(buttonB.getSingleDebouncedPress())
      return;
    if(buttonC.getSingleDebouncedPress())
      break;
  }
  
  sensorDebug();
}

int16_t readLine()
{
  readSensors();
  static int16_t pos;
  
  if(!onLine())
  {
    if(pos < 0)
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
  return pos;
}

int16_t err;
void followLine()
{
  int16_t pos = readLine();
  static int16_t last_pos;
  err = pos/20 - (pos - last_pos)/5;
  last_pos = pos;
  uint8_t speed = 100;
  err = max(min(err,speed),-speed);
  if(err > 0)
    motors.setSpeeds(speed, speed-err);
  else
    motors.setSpeeds(speed+err, speed);
}

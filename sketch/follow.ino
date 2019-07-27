uint16_t sensor[4];

bool onLine()
{
  return sensor[0] + sensor[1] + sensor[2] + sensor[3] > 512 && sensor[0] + sensor[1] + sensor[2] + sensor[3] < 3000;
}

uint16_t calibratedRead(uint8_t port, uint16_t min_value, uint16_t max_value)
{
  uint16_t v = analogRead(port);
  if(v < min_value)
    return 0;
  if(v > max_value)
    return 1024;
  return (v-min_value)*1024/(max_value-min_value);
}

void readSensors()
{
  sensor[0] = calibratedRead(4,650,850);
  sensor[1] = calibratedRead(3,580,820);
  sensor[2] = calibratedRead(2,520,750);
  sensor[3] = calibratedRead(0,760,900);
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

void followLine()
{
  int16_t pos = readLine();
  static int16_t last_pos;
  int16_t s = pos/30 - (pos - last_pos)/5;
  last_pos = pos;
  if(onLine())
    if(s > 0)
      motors.setSpeeds(100, 100-s);
    else
      motors.setSpeeds(100+s, 100);
  else
    motors.setSpeeds(0,0);
}

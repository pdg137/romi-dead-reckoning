const uint32_t RADIUS=1931*10000L;
const int32_t FOLLOW_MAX_Y=13000000L;
const int16_t FOLLOW_MAX_S=15000L;
const uint8_t MAX_SPEED=100;

int32_t target_s;

void goHome() {
  int16_t speed = MAX_SPEED;
  int32_t err;
  
  if(x > -20000000)
    speed = speed/2;
  
  if(c < 0)
  {
    // pointed backwards
    err = (s > 0 ? speed/2 : -speed/2);
  }
  else
  {    
    target_s = -max(min(y / 10000 * FOLLOW_MAX_S / (FOLLOW_MAX_Y / 10000), FOLLOW_MAX_S), -FOLLOW_MAX_S);
    err = (s - target_s)/100;
    err = max(min(err,speed),-speed);
  }
  if(err > 0)
    motors.setSpeeds(speed, speed - err);
  else
    motors.setSpeeds(speed + err, speed);
}

// direct-to the origin
void transform() {
  x -= RADIUS;
  
  double r = hypot((double)x, (double)y);
  double nx = (double)x/r;
  double ny = (double)y/r;
  int16_t new_c = -nx*c-ny*s;
  int16_t new_s = ny*c-nx*s;
  c = new_c;
  s = new_s;
  y = 0;
  x = -r;
  
  x += RADIUS;
}

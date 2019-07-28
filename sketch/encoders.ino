const int16_t ANGLE_SCALE=20000;
const int16_t STEPS_PER_RADIAN=1000;
int16_t c=ANGLE_SCALE;
int16_t s=0;
int32_t x=0, y=0;
#define sign(x) ((x)<0?-1:1)

void ticks1(int8_t n) {
  n = sign(n);
  
  int16_t dc = + divide(n*s - n*c/2/STEPS_PER_RADIAN, STEPS_PER_RADIAN);
  int16_t ds = - divide(n*c + n*s/2/STEPS_PER_RADIAN, STEPS_PER_RADIAN);
  
  c += dc;
  s += ds;
  x += n * c;
  y += n * s;
}

void ticks2(int8_t n) {
  n = sign(n);
  
  int16_t dc = - divide(n*s + n*c/2/STEPS_PER_RADIAN, STEPS_PER_RADIAN);
  int16_t ds = + divide(n*c - n*s/2/STEPS_PER_RADIAN, STEPS_PER_RADIAN);
  c += dc;
  s += ds;
  x += n * c;
  y += n * s;
}

void encoderService()
{
  int16_t t = encoders.getCountsAndResetLeft();
  while(t != 0)
  {
    ticks1(t);
    t -= sign(t);
  }

  t = encoders.getCountsAndResetRight();
  while(t != 0)
  {
    ticks2(t);
    t -= sign(t);
  }
}

#define sign(x) ((x)<0?-1:1)

int16_t divide(int16_t a, int16_t b)
{
  return (a + sign(a)*(b/2-1)) / b;
}

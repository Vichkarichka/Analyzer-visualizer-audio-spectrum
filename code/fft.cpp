#define __PROG_TYPES_COMPAT__
#include <avr/pgmspace.h>
#include "fft.h"
#include <Arduino.h>

#define N_WAVE      256   
#define LOG2_N_WAVE 8     

//дискретные значения гармонического колебания
const prog_int8_t Sinewave[N_WAVE-N_WAVE/4] PROGMEM = 
{
  0, 3, 6, 9, 12, 15, 18, 21, 
  24, 28, 31, 34, 37, 40, 43, 46, 
  48, 51, 54, 57, 60, 63, 65, 68, 
  71, 73, 76, 78, 81, 83, 85, 88, 
  90, 92, 94, 96, 98, 100, 102, 104, 
  106, 108, 109, 111, 112, 114, 115, 117, 
  118, 119, 120, 121, 122, 123, 124, 124, 
  125, 126, 126, 127, 127, 127, 127, 127, 

  127, 127, 127, 127, 127, 127, 126, 126, 
  125, 124, 124, 123, 122, 121, 120, 119, 
  118, 117, 115, 114, 112, 111, 109, 108, 
  106, 104, 102, 100, 98, 96, 94, 92, 
  90, 88, 85, 83, 81, 78, 76, 73, 
  71, 68, 65, 63, 60, 57, 54, 51, 
  48, 46, 43, 40, 37, 34, 31, 28, 
  24, 21, 18, 15, 12, 9, 6, 3, 

  0, -3, -6, -9, -12, -15, -18, -21, 
  -24, -28, -31, -34, -37, -40, -43, -46, 
  -48, -51, -54, -57, -60, -63, -65, -68, 
  -71, -73, -76, -78, -81, -83, -85, -88, 
  -90, -92, -94, -96, -98, -100, -102, -104, 
  -106, -108, -109, -111, -112, -114, -115, -117, 
  -118, -119, -120, -121, -122, -123, -124, -124, 
  -125, -126, -126, -127, -127, -127, -127, -127, 
};

inline char FIX_MPY(char a, char b)
{

  int c = ((int)a * (int)b) >> 6;
  b = c & 0x01;
  a = (c >> 1) + b;
  return a;
}
//функция быстрого преобразования преобразования фурье
int fft(char fr[], char fi[], int m)
{
  int mr, nn, i, j, l, k, istep, n, scale, shift;
  char qr, qi, tr, ti, wr, wi;

  n = 1 << m;

  if (n > N_WAVE)
    return -1;

  mr = 0;
  nn = n - 1;
  scale = 0;

  for (m = 1; m <= nn; ++m) {
    l = n;
    do {
      l >>= 1;
    } while (mr + l > nn);
    mr = (mr & (l - 1)) + l;

    if (mr <= m)
      continue;
    tr = fr[m];
    fr[m] = fr[mr];
    fr[mr] = tr;
    ti = fi[m];
    fi[m] = fi[mr];
    fi[mr] = ti;
  }

  l = 1;
  k = LOG2_N_WAVE-1;
  while (l < n) 
  {
    shift = 1;
    istep = l << 1;
    for (m = 0; m < l; ++m) 
    {
      j = m << k;
      wr =  pgm_read_word_near(Sinewave + j+N_WAVE/4);

      wi = -pgm_read_word_near(Sinewave + j);

      if (shift)
      {
        wr >>= 1;
        wi >>= 1;
      }
      for (i = m; i < n; i += istep) 
      {
        j = i + l;
        tr = FIX_MPY(wr,fr[j]) - FIX_MPY(wi,fi[j]);
        ti = FIX_MPY(wr,fi[j]) + FIX_MPY(wi,fr[j]);
        qr = fr[i];
        qi = fi[i];
        if (shift) 
        {
          qr >>= 1;
          qi >>= 1;
        }
        fr[j] = qr - tr;
        fi[j] = qi - ti;
        fr[i] = qr + tr;
        fi[i] = qi + ti;
      }
    }
    --k;
    l = istep;
    
  }
  return scale;
}



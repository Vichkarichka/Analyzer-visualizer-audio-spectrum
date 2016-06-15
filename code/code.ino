#include "fft.h"

#define COLS_NUM 32
#define ROWS_NUM 8
//Объвление пинов
const int row[8] = {3,2,0,1,4,16,14,6}; 
const int col[5] = {8,9,10,A0,A1};
const int audioIn = A2;
const int buttonOn = A3;
const int buttonType = 5;
const int buttonNum1 = 7;
const int buttonNum2 = 15;
//константы для преобразования Фурье
const int N = 128;
const int LOG_2_N = 7;
const unsigned char patterns[ROWS_NUM + 1] = 
{
  B00000000,
  B00000001,
  B00000011,
  B00000111,
  B00001111,
  B00011111,
  B00111111,
  B01111111,
  B11111111
};
char im[N], re[N];
char renderBuffer[COLS_NUM];
int maximum;
int blockSize;

void setup() 
{
  //начальное объявление пинов как вход/выход
  pinMode(buttonOn, INPUT);
  pinMode(buttonType, INPUT);
  pinMode(buttonNum1, INPUT);
  pinMode(buttonNum2, INPUT);

  for (int i = 0; i < 8; i++)
  {
    pinMode(row[i],OUTPUT);

  }
  for (int i = 0; i < 5; i++)
  {
    pinMode(col[i],OUTPUT);
  }
}
void loop() {
  //считываем значение на кнопках
  int on = digitalRead(buttonOn);
  int type = digitalRead(buttonType);
  int num1 = digitalRead(buttonNum1);
  int num2 = digitalRead(buttonNum2);
  if(on)
  {
    //считываем последовательность с аудиовхода
    for(int i = 0; i < N; i++) 
    {
      re[i] = analogRead(audioIn);
      im[i] = 0;
    }
    //выполняем быстрое преобразование Фурье
    fft(re, im, LOG_2_N, 0);
    //преобразование коэффициентов Фурье в амплитуду или фазу
    for(int i = 0; i < 64; i++) 
    {
      if(type)
      {
        re[i] = sqrt(re[i] * re[i] + im[i] * im[i]);
      }
      else
      {
        re[i] = atan2(im[i], re[i]);
      }
    } 
    int data;
    //выбор максимального значения
    if(type)
    {
      if(blockSize > 8)
      {
        maximum = 6;
      }
      else
      {
        maximum = 20;
      }
    }
    else{
      maximum = 6;
    }
    //выбор частоты дискретизации
    if(num1)
    {
      if(num2)
      {
        blockSize = 32;
      }
      else
      {
        blockSize = 16;
      }
    }
    else
    {
      if(num2)
      {
        blockSize = 8;
      }
      else
      {
        blockSize = 4;
      }
    }
    //формирование выходного массива
    for(int j = 0; j < blockSize; j++)
    {
      data = 0;
      for(int i = 0; i < COLS_NUM / blockSize; i++)
      {
        int temp = (re[j * COLS_NUM / blockSize + i * 2] +
        re[j * COLS_NUM / blockSize + i * 2 + 1]);
        if ( temp  > maximum)
        {
          temp = maximum;
        }

        data += temp;
      }
      for(int i = 0; i < COLS_NUM/blockSize; i++)
      {
        renderBuffer[j * COLS_NUM/blockSize + i] = map(data *
        blockSize / COLS_NUM, 0, maximum, 0, ROWS_NUM);
      }
    }
  }
  //цикл вывода результата
  for (int i = 0; i < 3; i++)
  {
    for(char a = 0; a < 32; a++)
    {
      //вклучение диодов используя битовую маску
      for (char b == 0; b < 8; b++ )
      {
        digitalWrite(row[b],patterns[renderBuffer[a]] &
        (1 << b));
      }
      delayMicroseconds(500);
      //выключение диодов
      for (char b  = 0; b < 8; b++ )
      {
        digitalWrite(row[b],LOW);
      }
      //выбор столбца
      if ((a+1) & B00010000)
      {
        digitalWrite(col[4],HIGH);
      }
      else
      {
        digitalWrite(col[4],LOW);
      }
      if ((a+1) & B00001000)
      {
        digitalWrite(col[3],HIGH);
      }
      else
      {
        digitalWrite(col[3],LOW);
      }
      if ((a+1) & B00000100)
      {
        digitalWrite(col[2],HIGH);
      }
      else
      {
        digitalWrite(col[2],LOW);
      }
      if ((a+1) & B00000010)
      {
        digitalWrite(col[1],HIGH);
      }
      else
      {
        digitalWrite(col[1],LOW);
      }
      if ((a+1) & B00000001)
      {
        digitalWrite(col[0],HIGH);
      }
      else
      {
        digitalWrite(col[0],LOW);
      }
    }
  }
}



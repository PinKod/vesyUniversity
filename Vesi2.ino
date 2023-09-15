#include "HX711.h"
HX711 scale;
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = A1;
const int LOADCELL_SCK_PIN = A0;

const uint8_t pin_ENA = 2;                // Вывод Arduino подключённый к входу драйвера ENA+.
const uint8_t pin_DIR = 3;                // Вывод Arduino подключённый к входу драйвера DIR+.
const uint8_t pin_PUL = 4;                // Вывод Arduino подключённый к входу драйвера PUL+.
                                          // Вывод GND Arduino соединён с входами драйвера ENA-, DIR-, PUL-.
volatile uint32_t step=0;                 // Переменная хранит количество микрошагов, которые требуется совершить.

int cycle = 1000;                         // Частота обновления датчиков
int Speed = 5;                            // задержка между шагами

int row_excel = 0; // количество строк
int test = 123; // тестовая переменная, которая будет передана excel
int test_2 = 456; // вторая тестовая переменная, которая будет передана excel
  
// кнопки
int Up=6;
int Start=7;
int Down=8;
int Stop=5;
bool flagUp = false;
bool flagDown = false;
bool flagStart = false;
bool flagStop = false;
uint32_t btnTimer = 0;                                     
bool load = false;                         // включает и выключает процесс растяжения

int mas =0;                               // переменная массы (обрабатывать и выводить ее)
                                          
void setup(){                             
     Serial.begin(9600);

     pinMode( pin_ENA, OUTPUT );          // Конфигурируем вывод Arduino как выход.
     pinMode( pin_DIR, OUTPUT );          // Конфигурируем вывод Arduino как выход.
     pinMode( pin_PUL, OUTPUT );          // Конфигурируем вывод Arduino как выход.
     funcSetTimer2( 1000 );               // Запускаем 2 таймер указав частоту следования микрошагов от 1 до 200'000 Гц.
                                          // Чем выше частота, тем выше скорость вращения вала.
     pinMode(Up, INPUT_PULLUP);
     pinMode(Down, INPUT_PULLUP);
     pinMode(Start, INPUT_PULLUP);
     pinMode(Stop, INPUT_PULLUP);
     
     Serial.println("CLEARDATA"); // очистка листа excel
     Serial.println("Строка,Строка,Время,Вес,Удлинение"); // заголовки столбцов

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();                // reset the scale to 0
     
}
                                          //
void loop(){ 
  Serial.print(flagDown);
Serial.print(flagUp);
Serial.print(flagStop);
Serial.println(flagStart);

if (flagStart==true){

  mas = (scale.get_units());
  row_excel++; // номер строки + 1
  Serial.print("DATA,TIME,"); // запись в excel текущей даты и времени
  Serial.print(mas);
  Serial.print(",");
  Serial.println(row_excel);

}
  
  // читаем инвертированное значение для удобства
  bool btnState = !digitalRead(Stop);
  if (btnState && !flagStop && millis() - btnTimer > 200) {    
    flagUp = false;				// millis - delay from start of the loop
    flagStart = false;			// function of stop from butload
    flagDown = false;
    flagStop = false;
    btnTimer = millis();
    Serial.println("Stop");
  }
  
  btnState = !digitalRead(Down);   // function of start to down
  if (btnState && !flagDown && millis() - btnTimer > 200) {
    flagUp = false;
    flagStart = false;
    flagDown = true;
    btnTimer = millis();
    Serial.println("Down");
  }

  btnState = !digitalRead(Up);
  if (btnState && !flagUp && millis() - btnTimer > 200) {
    flagDown = false;
    flagStart = false;
    flagUp = true;
    btnTimer = millis();
    Serial.println("Up");
  }

  btnState = !digitalRead(Start);
  if (btnState && !flagStart && millis() - btnTimer > 200) {
	flagUp = false;
	flagDown = false;
	flagStart = true;
	btnTimer = millis();
    Serial.println("Start");
  }
      //Serial.print(flagDown);
      //Serial.print(flagStart);
      //Serial.println(flagUp);
  
  if (flagUp==true)
  {
//   Готовимся к движению вала:           //
     digitalWrite( pin_ENA, 0 );          // Разрешаем работу двигателя.
     delayMicroseconds(5);                // Выполняем задержку t1 (см. график STEP/DIR).
     digitalWrite( pin_DIR, 1 );          // Выбираем направление вращения.
     delayMicroseconds(5);                // Выполняем задержку t2 (см. график STEP/DIR).
//   Поворачиваем вал на 2 оборота:       //
     step=1600;                           // Указываем количество микрошагов, которые требуется совершить.
     while(step){;}                       // Ждём обнуления переменной.
//   Останавливаем вал удерживая его:     // Двигатель не вращается, если на вывод PUL не поступают импульсы.                     
}  
  
  
  if (flagDown==true)
  {
//   Готовимся к движению вала:           //
     digitalWrite( pin_ENA, 0 );          // Разрешаем работу двигателя.
     delayMicroseconds(Speed);                // Выполняем задержку t1 (см. график STEP/DIR).
     digitalWrite( pin_DIR, 0 );          // Выбираем направление вращения.
     delayMicroseconds(Speed);                // Выполняем задержку t2 (см. график STEP/DIR).
//   Поворачиваем вал на 2 оборота:       //
     step=cycle;                           // Указываем количество микрошагов, которые требуется совершить.
     while(step){;}                       // Ждём обнуления переменной.
//   Останавливаем вал удерживая его:     // Двигатель не вращается, если на вывод PUL не поступают импульсы.                     
}  
  
  
  
  
  
  
  if (flagStart==true)
  {
//   Готовимся к движению вала:           //
     digitalWrite( pin_ENA, 0 );          // Разрешаем работу двигателя.
     delayMicroseconds(Speed);                // Выполняем задержку t1 (см. график STEP/DIR).
     digitalWrite( pin_DIR, 0 );          // Выбираем направление вращения.
     delayMicroseconds(Speed);                // Выполняем задержку t2 (см. график STEP/DIR).
//   Поворачиваем вал на 2 оборота:       //
     step=cycle;                           // Указываем количество микрошагов, которые требуется совершить.
     while(step){;}                       // Ждём обнуления переменной.
//   Останавливаем вал удерживая его:     // Двигатель не вращается, если на вывод PUL не поступают импульсы.                     
}

if (flagDown == false && flagUp == false && flagStart == false)
{delay (500);}                               // Ждем 0,5 секунды если двигатель выключен


}                                         //
                                          //
//   ОБРАБОТКА ПРЕРЫВАНИЙ 2 ТАЙМЕРА:      //
ISR(TIMER2_COMPA_vect){                   // Функция вызывается по совпадению регистров TCNT2 и OCR2A.
      if( step ){                         // Если требуется выполнять микрошаги, то ...
          bool p = digitalRead(pin_PUL);  // Определяем текущее состояние на выводе PUL.
          digitalWrite(pin_PUL, !p);      // Меняем состояние на выводе PUL.
          if( p ){ step--; }              // Уменьшаем количество требуемых микрошагов.
      }                                   //
}                                         //
                                          //
//    ФУНКЦИЯ НАСТРОЙКИ 2 ТАЙМЕРА:        //
void  funcSetTimer2(uint32_t f){          // Параметр: «f» - частота тактирования ШД от 1 до 200'000 Гц.
      if(f>200000){
		  f=200000;
	  }
//    Определяем значение предделителя:
      uint16_t i; 
	  uint8_t  j; 
	  f *= 2;
    if(f>(F_CPU/255/  1)){
		i=1; 
		j=1;
	}else if(f>(F_CPU/255/  8)){
		i=8; 
		j=2;
	}else if(f>(F_CPU/255/ 32)){
		i=32; 
		j=3;
	}else if(f>(F_CPU/255/ 64)){
		i=64;
		j=4;
	}else if(f>(F_CPU/255/128)){
		i=128; 
		j=5;
	}else if(f>(F_CPU/255/256)){
		i=256; 
		j=6;
	}else{
		i=1024; 
		j=7;
	}
//    Устанавливаем регистры 2 таймера:
      TCCR2A = 0<<COM2A1 | 0<<COM2A0 | 0<<COM2B1 | 0<<COM2B0 | 1<<WGM21  | 0<<WGM20;
      TCCR2B = 0<<FOC2A  | 0<<FOC2B  | 0<<WGM22  | j;
      OCR2A  = (uint8_t)(F_CPU/(i*f))-1;
      TIMSK2 = 0<<OCIE2B | 1<<OCIE2A | 0<<TOIE2;
      SREG   = 1<<7;
}

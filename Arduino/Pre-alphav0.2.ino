/*
  NeRadio Controller v.0.2
  Контроллер для микшера Behringer DX2000 USB
  Для реализации функций Fader-start, джингл-борда,
  световыми табло "Передача" и "Микрофон",
  отключением мониторной линии при включенном микрофоне

  Джингл-борд: 9 кнопок запуска и 2 кнопки перемещаения по страницам
  Track-start: по числу кнопок на микшере (линейки 3-7, 5 кнопок)
  Fader-start: по количеству фейдеров на микшере - 7

  Лампа МИКРОФОН ВКЛЮЧЕН: для обозначения, что сигнал с микрофонов идёт в эфир
  Лампа ПЕРЕДАЧА: для обозначения успешного подключения mAirList к IceCast серверу

  UPD 0.2: В данной версии лампы заменены на один светодиодный модуль на базе WS2811.

  Реле Monitor Mute: при передаче сигнала с микрофонов в линию отключается выход на мониторные динамики в студии

  UPD 0.2: В данной версии используется два реле - для L и R канала соответственно

  Для системы автоматизации радиовещания mAirList

  Данная версия предназначена для управления через COM-порт с помощью текстовых команд
  Об управлении через серийный порт:
  https://wiki.mairlist.com/config:remotecontrol:serial
  О командах:
  https://wiki.mairlist.com/reference:remote_control_commands

  Eugene Fisher, 2021
*/

#include <FastLED.h> // подключаем библиотеку для управления адресным светодиодным индикатором

#define NUM_LEDS 1 // количество контроллеров светодиодов на индикаторе (WS2811 - каждые три светодиода управляются как один)
#define PIN 46 // пин, на который подключена лента. Обязательно должен поддерживаться ШИМ, на Mega2560 это 2-13 и 44-46

CRGB leds[NUM_LEDS];
CRGB ledState; // Переменная типа CRGB для хранения цвета и корректной обработки команды включения красного "микрофонного" цвета.

//bool C1S, C2S, C3S, C4S, C5S, C6S, C7S, C8S, C9S; // Переменные для обработки джингл-борда (не используются)
//bool T3S, T4S, T5S, T6S, T7S; // Переменные для обработки кнопок TrackStart на микшере (не используются)
bool F1S, F2S, F3S, F4S, F5S, F6S, F7S; // Переменные для обработки концевых переключателей на фейдерах
bool FaderOn; // Переменная вкл-выкл обработки концевиков

// Указываем пины, на которых сидят кнопки джингл-борда (1-9) и кнопки СледСтраница ПредСтраница
int C1 = 22, C2 = 23, C3 = 24, C4 = 25, C5 = 26, C6 = 27, C7 = 28, C8 = 29, C9 = 30, CPgU = 31, CPgD = 32;

// Указываем пины, на которых сидят выводы с кнопок Track start микшера
// !!! НУМЕРАЦИЯ СООТВЕТСТВУЕТ ЛИНИЯМ НА МИКШЕРЕ !!!
// Первые два канала - микрофонные, у них кнопок нет.

int T3 = 33, T4 = 34, T5 = 35, T6 = 36, T7 = 37;

// Указываем пины, на которых сиждят концевики фейдеров

int F1 = 38, F2 = 39, F3 = 40, F4 = 41, F5 = 42, F6 = 43, F7 = 44;

// Пин переключателя активации функции Fader start

int FSw = 45;

// Пины выходов на реле (v.0.2)
/* Вариант версии 0.1:
int MicLamp = 46; // лампа МИКРОФОН ВКЛЮЧЕН
int AirLamp = 47; // лампа ПЕРЕДАЧА
int MonMute = 48; // реле отключения мониторной линии
*/

int LED_lamp = 46; // Выход должен поддерживать ШИМ. На MEGA2560 это выходы 2-13, 44-46
int L_MonMute = 52; // Выход на управление реле отключения мониторной линии левого канала
int R_MonMute = 53; // Выход на управление реле отключения мониторной линии правого канала


// При запуске объяляем пины, запускаем COM-порт. Все пины-входы с подтяжкой.

void setup() {

// Светодиодная лента
FastLED.addLeds <WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // инициализация ленты
FastLED.setBrightness(50); // установка яркости
leds[0] = CRGB::Black; // Выключение ленты после инициализации


  
// Входные пины
  pinMode(C1, INPUT_PULLUP);
  pinMode(C2, INPUT_PULLUP);
  pinMode(C3, INPUT_PULLUP);
  pinMode(C4, INPUT_PULLUP);
  pinMode(C5, INPUT_PULLUP);
  pinMode(C6, INPUT_PULLUP);
  pinMode(C7, INPUT_PULLUP);
  pinMode(C8, INPUT_PULLUP);
  pinMode(C9, INPUT_PULLUP);
  pinMode(CPgU, INPUT_PULLUP);
  pinMode(CPgD, INPUT_PULLUP);

  pinMode(T3, INPUT_PULLUP);
  pinMode(T4, INPUT);
  pinMode(T5, INPUT);
  pinMode(T6, INPUT);
  pinMode(T7, INPUT);

  pinMode(F1, INPUT_PULLUP);
  pinMode(F2, INPUT_PULLUP);
  pinMode(F3, INPUT_PULLUP);
  pinMode(F4, INPUT_PULLUP);
  pinMode(F5, INPUT_PULLUP);
  pinMode(F6, INPUT_PULLUP);
  pinMode(F7, INPUT_PULLUP);
  pinMode(FSw, INPUT_PULLUP);
  
// Выходные пины
/*  pinMode(MicLamp, OUTPUT);
  pinMode(AirLamp, OUTPUT);
  pinMode(MonMute, OUTPUT); */ //Версия 10.1

  pinMode (L_MonMute, OUTPUT); // реле левого канала
  pinMode (R_MonMute, OUTPUT); // реле правого канала

// Открываем COM порт для передачи и чтения данных
  Serial.begin(9600);
  
}

// обрабатываем нажания
void loop() {

// Обработка простых нажатий - кнопки джингл-борда и кнопки Track Start
// При нажатии клавиши в COM порт передаётся команда mAirList
  bool CB1 = digitalRead(C1);
  if (!CB1 == HIGH) {
    Serial.println("CARTWALL 1 START/FADEOUT");
    delay(100);
  }

  bool CB2 = digitalRead(C2);
  if (!CB2 == HIGH) {
    Serial.println("CARTWALL 2 START/FADEOUT");
    delay(100);
  }
  
  bool CB3 = digitalRead(C3);
  if (!CB3 == HIGH) {
    Serial.println("CARTWALL 3 START/FADEOUT");
    delay(100);
  }

  bool CB4 = digitalRead(C4);
  if (!CB4 == HIGH) {
    Serial.println("CARTWALL 4 START/FADEOUT");
    delay(100);
  }  

  bool CB5 = digitalRead(C5);
  if (!CB5 == HIGH) {
    Serial.println("CARTWALL 5 START/FADEOUT");
  }  

  bool CB6 = digitalRead(C6);
  if (!CB6 == HIGH) {
    Serial.println("CARTWALL 6 START/FADEOUT");
    delay(100);
  }  

  bool CB7 = digitalRead(C5);
  if (!CB7 == HIGH) {
    Serial.println("CARTWALL 7 START/FADEOUT");
    delay(100);
  }  

  bool CB8 = digitalRead(C8);
  if (!CB8 == HIGH) {
    Serial.println("CARTWALL 8 START/FADEOUT");
    delay(100);
  }  

  bool CB9 = digitalRead(C9);
  if (!CB9 == HIGH) {
    Serial.println("CARTWALL 9 START/FADEOUT");
    delay(100);
  }  

  bool CPgUB = digitalRead(CPgU);
  if (!CPgUB == HIGH) {
    Serial.println("CARTWALL NEXT PAGE");
    delay(100);
  } 

  bool CPgDB = digitalRead(CPgD);
  if (!CPgDB == HIGH) {
    Serial.println("CARTWALL PREVIOUS PAGE");
    delay(100);
  }

    bool TB3 = digitalRead(T3);
  if (!TB3 == HIGH) {
    Serial.println("PLAYER 1-3 START/FADEOUT ");
    delay(100);
  }

  bool TB4 = digitalRead(T4);
  if (TB4 == HIGH) {
    Serial.println("Track button 4 pressed. No action assigned.");
    delay(100);
  }  

  bool TB5 = digitalRead(T5);
  if (TB5 == HIGH) {
    Serial.println("PLAYER 1-1 START/FADEOUT");
  }  

  bool TB6 = digitalRead(T6);
  if (TB6 == HIGH) {
    Serial.println("PLAYER 1-2 START/FADEOUT ");
    delay(100);
  }  

  bool TB7 = digitalRead(T7);
  if (TB7 == HIGH) {
    Serial.println("Track button 7 pressed. No action assigned.");
    delay(100);
  }

// Обработка действий концевых переключателей фейдеров
// Обработка микрофонных фейдеров - включение токтаймера и включение лампы МИКРОФОН ВКЛЮЧЕН

  bool FB1 = digitalRead(F1);

    if (FB1 == HIGH && F1S == false) {
      Serial.println("TALKTIMER START");
      /*digitalWrite(MicLamp, HIGH);
      digitalWrite(MonMute, HIGH); */
      leds[0] = CRGB::Red;
      FastLED.show();
      digitalWrite (L_MonMute, HIGH);
      digitalWrite (R_MonMute, HIGH);
      F1S = true;
      delay(100);
    } else {
      if (FB1 == LOW && F1S == true) {
        Serial.println("TALKTIMER STOP");
          /* digitalWrite(MicLamp, LOW);
        digitalWrite(MonMute, LOW); */
        leds[0] = ledState;
        FastLED.show();
        digitalWrite (L_MonMute, LOW);
        digitalWrite (R_MonMute, LOW);
        F1S = false;
        delay(100);
      }
    }

  bool FB2 = digitalRead(F2);
    if (FB2 == HIGH && F2S == false) {
      Serial.println("TALKTIMER START");
      /*digitalWrite(MicLamp, HIGH);
      digitalWrite(MonMute, HIGH); */
      leds[0] = CRGB::Red;
      FastLED.show();
      digitalWrite (L_MonMute, HIGH);
      digitalWrite (R_MonMute, HIGH);
      F2S = true;
      delay(100);
    } else {
      if (FB2 == LOW && F2S == true) {
        Serial.println("TALKTIMER STOP");
        /* digitalWrite(MicLamp, LOW);
        digitalWrite(MonMute, LOW); */
        leds[0] = ledState;
        FastLED.show();
        digitalWrite (L_MonMute, LOW);
        digitalWrite (R_MonMute, LOW);
        F2S = false;
        delay(100);
      }
    }

// Обработка действий фейдеров источников сигнала

  bool FSwB = digitalRead(FSw);
  bool FB3 = digitalRead(F3);
    if (F3 == HIGH && F3S == false && FSwB == true) {
      Serial.println("PLAYER 1-3 START");
      F3S = true;
      delay(100);
    } else {
      if (FB3 == LOW && F3S == true && FSwB == true) {
         Serial.println("PLAYER 1-3 STOP");
         F3S = false;
         delay(100);
      }
    }
    if (F3 == HIGH && F3S == false && FSwB == false) {
      Serial.println("Fader 3 UP. Fader Start is off.");
      F3S = true;
      delay(100);
    } else {
      if (FB3 == LOW && F3S == true && FSwB == true) {
         Serial.println("Fader 3 DOWN. Fader Start is off.");
         F3S = false;
         delay(100);
      }
    }

  bool FB4 = digitalRead(F4);
    if (F4 == HIGH && F4S == false && FSwB == true) {
      Serial.println("Fader 4 UP. No action assigned.");
      F4S = true;
      delay(100);
    } else {
      if (FB4 == LOW && F4S == true && FSwB == true) {
         Serial.println("Fader 4 DOWN. No action assigned.");
         F4S = false;
         delay(100);
      }
    }
    if (F4 == HIGH && F4S == false && FSwB == false) {
      Serial.println("Fader 4 UP. Fader Start is off.");
      F4S = true;
      delay(100);
    } else {
      if (FB4 == LOW && F4S == true && FSwB == true) {
         Serial.println("Fader 4 DOWN. Fader Start is off.");
         F4S = false;
         delay(100);
      }
    }


  bool FB5 = digitalRead(F5);
    if (F5 == HIGH && F5S == false && FSwB == true) {
      Serial.println("PLAYER 1-1 START");
      F5S = true;
      delay(100);
    } else {
      if (FB5 == LOW && F5S == true && FSwB == true) {
         Serial.println("PLAYER 1-1 STOP");
         F5S = false;
         delay(100);
      }
    }
    if (F5 == HIGH && F5S == false && FSwB == false) {
      Serial.println("Fader 5 UP. Fader Start is off.");
      F5S = true;
      delay(100);
    } else {
      if (FB5 == LOW && F5S == true && FSwB == true) {
         Serial.println("Fader 5 DOWN. Fader Start is off.");
         F5S = false;
         delay(100);
      }
    }

  bool FB6 = digitalRead(F6);
    if (F6 == HIGH && F6S == false && FSwB == true) {
      Serial.println("PLAYER 1-2 START");
      F6S = true;
      delay(100);
    } else {
      if (FB6 == LOW && F6S == true && FSwB == true) {
         Serial.println("PLAYER 1-2 STOP");
         F6S = false;
         delay(100);
      }
    }
    if (F6 == HIGH && F6S == false && FSwB == false) {
      Serial.println("Fader 6 UP. Fader Start is off.");
      F6S = true;
      delay(100);
    } else {
      if (FB6 == LOW && F6S == true && FSwB == true) {
         Serial.println("Fader 6 DOWN. Fader Start is off.");
         F6S = false;
         delay(100);
      }
    }


  bool FB7 = digitalRead(F7);
    if (F7 == HIGH && F7S == false && FSwB == true) {
      Serial.println("Fader 7 UP. No action assigned.");
      F7S = true;
      delay(100);
    } else {
      if (FB7 == LOW && F7S == true && FSwB == true) {
         Serial.println("Fader 7 DOWN. No action assigned.");
         F7S = false;
         delay(100);
      }
    }
    if (F7 == HIGH && F7S == false && FSwB == false) {
      Serial.println("Fader 6 UP. Fader Start is off.");
      F7S = true;
      delay(100);
    } else {
      if (FB7 == LOW && F7S == true && FSwB == true) {
         Serial.println("Fader 6 DOWN. Fader Start is off.");
         F7S = false;
         delay(100);
      }
    }

// Работа с лампой ПЕРЕДАЧА
// mAirList при смене статуса энкодера должен передать символ в COM порт Arduino.
// V.0.1 - Если символ D - лампа включается, энкодер работате. Если F - лампа отключается.
// V.0.2 - В зависимости от символа - лампа принимает необходимое значение.



if (Serial.available() > 0) {
  int COMdata = Serial.read();

if (COMdata == 81) { //44 символ R в ASCII, проверка на то, что микрофонные фейдеры внизу
      digitalWrite(L_MonMute, HIGH);
      digitalWrite(R_MonMute, HIGH);
    }

if (COMdata == 80) { //44 символ R в ASCII, проверка на то, что микрофонные фейдеры внизу
      digitalWrite(L_MonMute, LOW);
      digitalWrite(R_MonMute, LOW);
    }
    
if (COMdata == 82 && F1S == false && F2S == false) { //44 символ R в ASCII, проверка на то, что микрофонные фейдеры внизу
      leds[0] = CRGB::Red; // задаем для первого пикселя синий цвет
      FastLED.show(); // отправляем информацию на ленту
      ledState = leds[0]; // записываем текущий цвет для обработки события микрофонного фейдера
      delay(100);
    }
    if (COMdata == 71 && F1S == false && F2S == false) { //46 символ G в ASCII
      leds[0] = CRGB::Green; // задаем для первого пикселя синий цвет
      FastLED.show(); // отправляем информацию на ленту
      ledState = leds[0];
      delay(100);
    }
    if (COMdata == 66 && F1S == false && F2S == false) { //46 символ B в ASCII
      leds[0] = CRGB::Blue; // задаем для первого пикселя синий цвет
      FastLED.show(); // отправляем информацию на ленту
      ledState = leds[0];
      delay(100);
    }
    if (COMdata == 87 && F1S == false && F2S == false) { //46 символ W в ASCII
      leds[0] = CRGB::White; // задаем для первого пикселя синий цвет
      FastLED.show(); // отправляем информацию на ленту
      ledState = leds[0];
      delay(100);
    }
    if (COMdata == 89 && F1S == false && F2S == false) { //46 символ Y в ASCII
      leds[0] = CRGB::Yellow; // задаем для первого пикселя синий цвет
      FastLED.show(); // отправляем информацию на ленту
      ledState = leds[0];
      delay(100);
    }
    if (COMdata == 67 && F1S == false && F2S == false) { //46 символ C в ASCII
      leds[0] = CRGB::Black; // задаем для первого пикселя синий цвет
      FastLED.show(); // отправляем информацию на ленту
      ledState = leds[0];
      delay(100);
    }
    if (COMdata == 69 && F1S == false && F2S == false) { //Cимвол E для EOF (End of file), моргание жёлтым

      unsigned long period = 2000;        
      unsigned long currentMillis = 0;
      unsigned long startMillis = 0;
      boolean ledOn = false;

      currentMillis = millis();

      if (currentMillis - startMillis >= period) {
        startMillis = currentMillis;
        ledOn = !ledOn;
        if(ledOn){
          leds[7]=CRGB::Yellow;
        } else {
          leds[7]=CRGB::Black;
         }
         FastLED.show();
      }
      
      /*leds[0] = CRGB::Black; // задаем для первого пикселя синий цвет
      FastLED.show(); // отправляем информацию на ленту
      ledState = leds[0];
      delay(100);*/
    }
}

/*
  bool But1 = digitalRead(6);
    if (But1 == true && But1St == false) {
      digitalWrite(5, LOW);
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      Serial.println("PLAYER 1-1 START ");
      But1St = true;
      delay(200);
    } else {
      if (But1 == false && But1St == true) {
        digitalWrite(5, HIGH);
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        Serial.println("PLAYER 1-1 FADE OUT ");
        But1St = false;
        delay(200);
      }
    }

    bool But2 = digitalRead(52);
    if (But2 == true && But2St == false) {
      Serial.println("TALKTIMER START ");
      But2St = true;
      delay(200);
    } else {
      if (But2 == false && But2St == true) {
        Serial.println("TALKTIMER STOP ");
        But2St = false;
        delay(200);
      }
    }
  
  digitalWrite(5, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(5, LOW);    // turn the LED off by making the voltage LOW
  delay(3000);                       // wait for a second */
}

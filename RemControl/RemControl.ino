#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01
#define X_AXIS A2  // Номер пина Arduino, к которому подключен джойстик
#define Y_AXIS A1 
#define PIN_CE  8  // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 10 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля
RF24 radio(PIN_CE, PIN_CSN); // Создаём объект radio с указанием выводов CE и CSN

#define Z 5 

#define pinBTN 4 //кнопка

int axes[4]; 
void setup() {
  pinMode(X_AXIS, INPUT);
  pinMode(Y_AXIS, INPUT);
  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(8); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openWritingPipe(0x7878787878LL); // Открываем трубу с уникальным ID
}

void loop() {
  axes[0] = analogRead(X_AXIS); 
  axes[1] = analogRead(Y_AXIS);
  if(digitalRead(Z)){
    while(digitalRead(Z) != 0){
      axes[2] = 1;}
  }
  axes[3] = digitalRead(pinBTN);
  delay(1);
  //Serial.println(axes[0]);
  radio.write(&axes, sizeof(axes));
  axes[2] = 0;

      
  //Serial.println(axes[2]);
  //Serial.println(axes[3]);
}
#include <GyverMotor.h>

#include <Adafruit_NeoPixel.h>

// номер пина, к которому подключена RGB-матрица
#define MATRIX_PIN    10
// количество светодиодов в матрице
#define LED_COUNT 16
Adafruit_NeoPixel matrix = Adafruit_NeoPixel(LED_COUNT, MATRIX_PIN, NEO_GRB + NEO_KHZ800);

#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01

#define PIN_CE 1  // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 2 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля
RF24 radio(PIN_CE, PIN_CSN); // Создаём объект radio с указанием выводов CE и CSN

#define PWM_X A0
#define PWM_Y A1

//Servo
#include "Servo.h"
Servo servoShock;
#define SERVOShock_PIN 13

int data[4]; 
GMotor motor_left(DRIVER2WIRE, 7, 6);
GMotor motor_right(DRIVER2WIRE, 4, 5);

int lampStat = 0;
void colorWipe(uint32_t c, uint8_t wait)
{
  for (uint16_t i = 0; i < matrix.numPixels(); i++) {
    // заполняем текущий сегмент выбранным цветом
    matrix.setPixelColor(i, c);
    matrix.show();
    // ждём
    delay(wait);
  }
}

void setup() {
  Serial.begin(9600);
  servoShock.attach(SERVOShock_PIN);
  //TCCR1A = 0b00000011;  // 10bit
  //TCCR1B = 0b00001001;  // x1 fast pwm
  //motor_right.setResolution(10);
  //motor_left.setDirection(REVERSE);
  //motor_left.setMode(AUTO);
  
  motor_right.setMinDuty(50);
  motor_left.setMinDuty(50);

  matrix.begin();

  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(8); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe (1, 0x7878787878LL); // Открываем трубу ID передатчика
  radio.startListening(); // Начинаем прослушивать открываемую трубу
}

void loop() {
   
  int X, Y;

  if(radio.available()){ // Если в буфер приёмника поступили данные
    radio.read(&data, sizeof(data));
    X = map(data[0],0,1023,-255,255); 
    Y = map(data[1],0,1023,-255,255);
  }

  if(Y < 0){
      motor_right.setMode(BACKWARD);
      motor_left.setMode(BACKWARD);
      Y = -255 - Y;
    }
    else if(Y > 0){
      motor_right.setMode(FORWARD);
      motor_left.setMode(FORWARD);
    }

    int dutyR = Y + X;
    int dutyL = Y - X;
    motor_right.setSpeed(dutyR);
    motor_left.setSpeed(dutyL);
  

  if(data[2] == 1){ 
    lampStat = abs(lampStat - 1);
  }
  if(lampStat == 1){
    colorWipe(matrix.Color(255, 255, 255), 1);
  }
  else{
    colorWipe(matrix.Color(0, 0, 0), 1);
  }
  
  //!!!!!!
  Serial.println(data[3]);
  if(data[3] == 1){ 
    servoShock.write(160);
    delay(6000);
    servoShock.write(0);
  }
  //!!!!!!
}
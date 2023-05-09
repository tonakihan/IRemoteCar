/* Шасси с дистанционным управлением - клиентская часть

 */

#include "softUART.h" //Для связи МК
#include "AFMotor.h"  // подключаем библиотеку для шилда
#include "GBUS.h"     // подключаем GBUS

softUART<2, GBUS_RX> UART(1000);  // пин 2, только прием, скорость 1000
GBUS bus(&UART, 1, 20);           // обработчик UART, адрес 1, буфер 20 байт

AF_DCMotor FrontRight(1, MOTOR12_1KHZ); // подключаем мотор к клемам M1
AF_DCMotor FrontLeft(2, MOTOR12_1KHZ); // M2
AF_DCMotor BackLeft(3); // M3
AF_DCMotor BackRight(4); // M4

int speedChassis{};

struct structForUart { //Стуктура для передачи по Uart - этим создаем протокол этой передачи
  unsigned int command{}; //Для общения с ардуиной-сервер
};

//Функции управления двигателями.
void leftBoardGo();
void rightBoardGo();
void leftBoardBack();
void rightBoardBack();
void stopChassis();
void setSpeedAll(int speedChassis = 255);


void setup() 
{
  Serial.begin(9600);
  
  setSpeedAll();
  stopChassis();
}


void loop() 
{
  bus.tick(); // в тике сидит отправка и приём
  
  
  if (bus.gotData()){  //вернёт true, если новый пакет данных принят без ошибок. Сам сбрасывается в false.
    structForUart data;
    bus.readData(data); //Запись данных в переменную
    Serial.print("Uart get data: ");
    Serial.println(data.command); //выводим что получили
  
    if (data.command == 1){      // на лево 
        rightBoardGo();
        leftBoardBack();
    }
    else if (data.command == 2){ // на право
        rightBoardBack();
        leftBoardGo();
    }
    else if (data.command == 3){ // прямо
        rightBoardGo();
        leftBoardGo();
    }
    else if (data.command == 4){ // назад
        rightBoardBack();
        leftBoardBack();
    }
    else if (data.command == 5){ //Скорость выше - vol+
        speedChassis+=50;
        setSpeedAll(speedChassis);
    }
    else if (data.command == 6){ //Скорость ниже - vol-
        speedChassis-=50;
        setSpeedAll(speedChassis);
    }
    else if (data.command == 7){ //STOP - button enter
        stopChassis();
    }
  
    if (speedChassis > 255) speedChassis = 255;
    else if (speedChassis < 0) speedChassis = 0;
  }
}
  
void leftBoardGo(){ //Левый борт вперед
  FrontLeft.run(FORWARD);
  BackLeft.run(FORWARD);  
  Serial.println("LeftBGo");
}

void rightBoardGo(){
  FrontRight.run(FORWARD);
  BackRight.run(FORWARD);
  Serial.println("RightBGo");
}

void leftBoardBack(){
  FrontRight.run(BACKWARD); 
  BackRight.run(BACKWARD);
  Serial.println("LeftBBack"); 
}

void rightBoardBack(){
  FrontRight.run(BACKWARD); 
  BackRight.run(BACKWARD); 
  Serial.println("LeftBBack");
}

void stopChassis(){
  FrontRight.run(RELEASE);  
  FrontLeft.run(RELEASE);
  BackRight.run(RELEASE); 
  BackLeft.run(RELEASE);
  Serial.println("STOP!");  
}

void setSpeedAll(int speedChassis){
  Serial.print("setSpeed: ");
  Serial.println(speedChassis);
  
  FrontRight.setSpeed(speedChassis);   
  FrontLeft.setSpeed(speedChassis); 
  BackRight.setSpeed(speedChassis);  
  BackLeft.setSpeed(speedChassis);   
}

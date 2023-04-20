/* Программа: Шасси с дистанционным управлением  - серверная часть
 * Выполнил: Гетман Марк ПКС202
 */

#include "softUART.h" // подключаем софт uart
#include "GBUS.h"
#include "IRremote.hpp" //Включает поддержку ИК порта

softUART<2, GBUS_TX> UART(1000);  // пин 2, только передача, скорость 1000
GBUS bus(&UART, 2, 20);           // обработчик UART, адрес 2, буфер 20 байт

struct structForUart {  //Стуктура для передачи по Uart - этим задаем параметры протокола.
  unsigned int command;   //Данные
};


void setup() {
  Serial.begin(9600);
  IrReceiver.begin(A0, ENABLE_LED_FEEDBACK);  // запускаем прием инфракрасного сигнала, пин A1
}


void loop() {
  bus.tick(); //в тике сидит отправка и приём
  structForUart data; //создаем экземпляр структуры
  
  if (IrReceiver.decode()){ // если кнопка нажата
    switch(IrReceiver.decodedIRData.decodedRawData){
      case 4027547392: //Налево 
        data.command  = 1;
        break;
      case 4044259072: //Направо
        data.command  = 2;
        break;
      case 4077682432: //Прямо
        data.command  = 3;
        break;
      case 4060970752: //Назад
        data.command  = 4;
        break;
      case 3827007232: //Скорость выше - vol+
        data.command  = 5;
        break;
      case 3843718912: //Скорость ниже - vol-
        data.command  = 6;
        break;
      case 4010835712: //STOP - button enter
        data.command  = 7;
        break;
      default:
        data.command = 0; //обнуление        
    }
    
    if (data.command == 0) //если данных нет
      bus.sendData(1, data);  //на адрес 1 отправляю данные
      
    IrReceiver.resume(); //принимаем следующую команду
  }
}

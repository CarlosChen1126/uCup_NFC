# uCup_NFC_QRcode讀卡機
這是一個專屬「杯特 uCup」的NFC+條碼二合一讀卡機，結合uCup的後台管理系統進行使用，方便使用者透過感應NTU學生證或uCup官方Line帳號之QRcode進行借用或歸還。



## structure
```
.
├── README.md
├── doc
│   ├── PCB.png
│   ├── Schematic.png
│   └── materials.xlsx
├── inc
├── src
│   ├── Barcode.cpp
│   ├── Barcode.h
│   ├── Button.cpp
│   ├── Button.h
│   ├── Buzzer.cpp
│   ├── Buzzer.h
│   ├── Config.h
│   ├── LED.cpp
│   ├── LED.h
│   ├── MyServer.cpp
│   ├── MyServer.h
│   ├── Oled.cpp
│   ├── Oled.h
│   ├── Rfid.cpp
│   └── Rfid.h
└── uCup_NFC.ino
```



## PIN Usage
| Usage | PIN | PIN | Usage |
| -------- | -------- | -------- |-------- |
| Power for RC522&OLED | 3.3 | GND | GND |
| RESET | EN      | GPIO23 | RC522 MOSI |
| x | GPIO36 | GPIO22  | OLED SCL |
|   x   | GPIO39  | GPIO1 / TX0  |    |
|   x   | GPIO34  | GPIO3 / RX0 |    |
|   x   | GPIO35  | GPIO21  |OLED SDA |
| x   | GPIO32  | GND     | GND|
|  x  | GPIO33  | GPIO19  |RC522 MISO |
|  x  | GPIO25  | GPIO18  |RC522 SCK |
|  x | GPIO26  | GPIO5   |RC522 SDA   |
| LED_RED | GPIO27  | GPIO17  | Button|
| Barcode | GPIO14  | GPIO16  |x |
| x | GPIO12  | GPIO4   |RC522 RST |
| GND| GND     | GPIO0   | x|
| LED_GREEN  | GPIO13  | GPIO2   | Buzzer Signal|
| x  | GPIO9   | GPIO15  | x|
| x  | GPIO10  | GPIO8   | x|
| x  | GPIO11  | GPIO7   | x|
|Power for Barcode&Buzzer| VIN 5V | GPIO6 | x|

## Element

## Schematic

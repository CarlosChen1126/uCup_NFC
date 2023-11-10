# uCup NFC/QRcode Scanner
This is an exclusive NFC and QRcode dual-function card reader designed for 'uCup' which is a product of BETTER Company Limited by Shares. It integrates with the uCup backend management system, allowing users to easily borrow or return reusable cups by scanning their NTU student ID or the official uCup Line account QR code.

![S__83001346](https://github.com/CarlosChen1126/uCup_NFC/assets/60618505/f09d9427-f281-4dd9-89bf-0fcb93a517b1)



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

## PCB
<img width="737" alt="截圖 2023-10-26 上午1 02 15" src="https://github.com/CarlosChen1126/uCup_NFC/assets/60618505/c26820db-f503-42c5-a39c-e854bb85870b">

## Schematic
![Schematic](https://github.com/CarlosChen1126/uCup_NFC/assets/60618505/4b747422-538f-4a64-b393-d3d6516ad9f5)


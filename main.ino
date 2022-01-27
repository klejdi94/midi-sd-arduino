#include <string.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <Wire.h>
#include <LiquidCrystal.h>


String menuItems[30] = {};
int readKey;
int menuPage = 0;
int cursorPosition = 0;

byte downArrow[8] = {
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};

byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};

byte menuCursor[8] = {
  B01000, //  *
  B00100, //   *
  B00010, //    *
  B00001, //     *
  B00010, //    *
  B00100, //   *
  B01000, //  *
  B00000  //
};



const int pin_RS = 8;
const int pin_EN = 9;
const int pin_d4 = 4;
const int pin_d5 = 5;
const int pin_d6 = 6;
const int pin_d7 = 7;
const int pin_BL = 10;
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);


#define chipSelect 4


SdFat card;
File dir;
File file;
File configMenuFile;

int posss = 0;


void setup() {

  Serial.begin(9600);
  if (!card.begin(chipSelect, SPI_HALF_SPEED)) card.initErrorHalt();

  if (!dir.open("/")) {
    Serial.println("dir.open failed");
  }
  showDirectory();

  Serial.println(posss);
  if (posss > 0) {
    Serial.println("Syx files found");
    for (int i = 0; i < posss; i++) {
      String mess = String(i + 1);
      mess.concat(".syx");
      menuItems[i] = mess;
    }
  } else {
    Serial.println("Syx files not found");
  }

  lcd.begin(16, 2);
  lcd.clear();

  lcd.createChar(0, menuCursor);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
}
void loop() {
  mainMenuDraw();
  drawCursor();
  operateMainMenu();
}


void showDirectory () {
  dir.rewind();
  while (file.openNext(&dir, O_RDONLY)) {
    fileNameHumanReadable(file);
    file.close();
  }
}


void fileNameHumanReadable(File activeFile) {
  char filename[10];
  activeFile.getName(filename , 10);
  String search = filename;
  if (search.indexOf(".syx") != -1) {
    posss++;
  }
}

void mainMenuDraw() {
  int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1);
  lcd.print(menuItems[menuPage + 1]);
  if (menuPage == 0) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
  } else if (menuPage > 0 and menuPage < maxMenuPages) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  } else if (menuPage == maxMenuPages) {
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  }
}


void drawCursor() {
  for (int x = 0; x < 2; x++) {   
    lcd.setCursor(0, x);
    lcd.print(" ");
  }

  
  if (menuPage % 2 == 0) {
    if (cursorPosition % 2 == 0) { 
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
  }
  if (menuPage % 2 != 0) {
    if (cursorPosition % 2 == 0) {  
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
  }
}


void operateMainMenu() {
  int activeButton = 0;
  int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 0:
        break;
      case 1: 
        button = 0;
        activeButton = 1;
        mainMenuDraw();
        drawCursor();
        break;
      case 2:
        button = 0;
        if (menuPage == 0) {
          cursorPosition = cursorPosition - 1;
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        }
        if (menuPage % 2 == 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition - 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));

        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
      case 3:
        button = 0;
        if (menuPage % 2 == 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition + 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
      case 5:
        button = 0;
        String elem = String(cursorPosition);
        elem.concat(".syx");
        Serial.println(elem);
        Serial.println("Opening file ...");
        File dataFile = card.open(elem);
        if (dataFile) {
          while (dataFile.available()) {
            char letter = dataFile.read();
            Serial.print(letter);
            
          }
          dataFile.close();
        }
        activeButton = 1;
        mainMenuDraw();
        drawCursor();
        break;
    }
  }
}

byte stringToByte(char *src, int numBytes)
{
  char charBuffer[4];
  int charToInt;
  byte intToByte;

  memcpy(charBuffer, src, numBytes);
  charToInt = atoi(charBuffer);
  intToByte = (byte)charToInt;
  return intToByte;
}

int evaluateButton(int x) {
  int result = 0;
  if (x < 59) {
    result = 1; // right
  } else if (x < 195) {
    result = 2; // up
  } else if (x < 390) {
    result = 3; // down
  } else if (x < 590) {
    result = 4; // left
  } else if (x < 800) {
    result = 5; // select
  }
  return result;
}

void drawInstructions() {
  lcd.setCursor(0, 1); 
  lcd.print("Use ");
  lcd.write(byte(1)); 
  lcd.print("/");
  lcd.write(byte(2)); 
  lcd.print(" buttons");
}

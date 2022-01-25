/*


 ** SDO - pin 11

 ** SDI - pin 12

 ** CLK - pin 13

 ** CS - pin 10


*/
#include <SD.h>
#include <MIDI.h>
#include <List.hpp>
#include <Bounce2.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

byte button_pins[] = {4, 5, 6}; // button pins, 4,5 = up/down, 6 = select
#define NUMBUTTONS sizeof(button_pins)
Bounce * buttons = new Bounce[NUMBUTTONS];


MIDI_CREATE_DEFAULT_INSTANCE();


const int chipSelect = 10;

int cursor=0;

File root;
List<char*> fileArray;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed");
    while (true);
  }
  Serial.println("initialization done.");
  root = SD.open("/");
  getElements(root, 0);

  for (int i = 0; i < NUMBUTTONS; i++) {
    buttons[i].attach( button_pins[i], INPUT_PULLUP);
    buttons[i].interval(25);
  }

  display.begin();

  showMenu();


  Serial.println("done!");
  // midi.begin();
}

void loop() {
  // midi.update()


  for (int i = 0; i < NUMBUTTONS; i++) {
    buttons[i].update();
    if ( buttons[i].fell() ) {
      if (i == 2) {
        display.setCursor(0, 7);
        display.print(">>");
        display.print(*fileArray.get(cursor));
        executeChoice(cursor);
      }
      else {
        display.setCursor(0, cursor);
        display.print(' ');
        if (i == 0) {
          cursor++;
          if (cursor > (fileArray.getSize() - 1)) cursor = 0;
        }
        else {
          cursor--;
          if (cursor < 0) cursor = (fileArray.getSize() - 1);
        }
        display.setCursor(0, cursor);
        display.print('>');
      }
    }
  }
}


void showMenu() {
  cursor = 0;
  display.clearDisplay();


  for (int i = 0; i < fileArray.getSize(); i++) {
    display.println(*fileArray.get(i));
  }
  display.setCursor(0, 0);
  display.print('>');
}

void getElements(File dir, int numTabs) {
  int arrSize = 0;
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      getElements(entry, numTabs + 1);
    } else {
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
      char * elem = entry.name();

      fileArray.add(elem);
    }
    entry.close();
  }
}


void executeChoice(int choice) {
  Serial.println(*fileArray.get(cursor));
}

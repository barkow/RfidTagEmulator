#include "Crc16.h"
#include <EEPROM.h>
#define COILPIN 9
#define BITTIME_US 256
#define EEPROM_ADDR 10

//#define ID 2050
//ID 2050 --> Checksumme im Funkframe: 0xF5D1
//#define ID 0x25d
//ID 0x25d --> Checksumme im Funkframe: 0xFE3A
//#define ID 0x1dd
//ID 0x1dd --> Checksumme im Funkframe: 0xFEAF
//#define ID 0x3df
//ID 0x3df --> Checksumme im Funkframe: 0xB091
#define ID 1234
//tagData 0xD950083080201E2D
//Chip ID 0x802
//Checksum 0xE2
/*unsigned char tagData[64] = {1,1,0,1,1,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,
                             1,0,0,0,0,0,0,0,0,0,1,0, //Chip ID
                             0,0,0,0,0,0,0,1,
                             1,1,1,0,0,0,1,0, //Checksum
                             1,1,0,1};
*/
/*unsigned char tagData[128] = { 1,1,1,1,1,1,1,1,1, //9 Start Bits
                            0,1,0,0,0,0,0,0,0,    //0x40 + jeweils 9. Bit 0
                            1,0,1,0,0,1,1,1,0,    //0xa7
                            0,0,1,1,1,1,1,1,0,    //0x3f
                            1,1,0,1,1,0,0,1,0,    //0xd9
                            0,1,0,1,0,0,0,0,0,    //0x50
                            0,0,0,0,1,0,0,0,0,    //0x08
                            0,0,1,1,i,i,i,i,0,    //0x3X
                            i,i,i,i,i,i,i,i,0,    //0xXX
                            i,i,i,i,0,0,0,0,0,    //0xX0
                            0,0,0,1,c,c,c,c,0,    //0x1X
                            c,c,c,c,1,1,0,1,0,    //0xXd
                            k,k,k,k,k,k,k,k,0,    //0xkk
                            k,k,k,k,k,k,k,k,0,    //0xkk
                            0,0                   //2 Stop Bits
  };
  i: ID
  c: checksum
  k: checksum 2*/
/*
unsigned char tagData[128] = { 1,1,1,1,1,1,1,1,1, //9 Start Bits
                            0,1,0,0,0,0,0,0,0,    //0x40 + jeweils 9. Bit 0
                            1,0,1,0,0,1,1,1,0,    //0xa7
                            0,0,1,1,1,1,1,1,0,    //0x3f
                            1,1,0,1,1,0,0,1,0,    //0xd9
                            0,1,0,1,0,0,0,0,0,    //0x50
                            0,0,0,0,1,0,0,0,0,    //0x08
                            0,0,1,1,0,0,0,0,0,    //0x30
                            0,0,1,0,0,1,0,1,0,    //0x25
                            1,1,0,1,0,0,0,0,0,    //0xd0
                            0,0,0,1,1,0,1,1,0,    //0x1b
                            0,1,1,1,1,1,0,1,0,    //0x7d
                            1,1,1,1,1,1,1,0,0,    //0xfe
                            0,0,1,1,1,0,1,0,0,    //0x3a
                            0,0                   //2 Stop Bits
  };
  /*i: 0x025d
  c: 0xb7
*/

Crc16 crc(false, false, 0x6f63, 0x0a4f, 0x0000, 0x8000, 0xffff);
String inputString = ""; 
bool stringComplete = false;

unsigned char tagData[128] = { 1,1,1,1,1,1,1,1,1, //9 Start Bits
                            0,1,0,0,0,0,0,0,0,    //0x40 + jeweils 9. Bit 0
                            1,0,1,0,0,1,1,1,0,    //0xa7
                            0,0,1,1,1,1,1,1,0,    //0x3f
                            1,1,0,1,1,0,0,1,0,    //0xd9
                            0,1,0,1,0,0,0,0,0,    //0x50
                            0,0,0,0,1,0,0,0,0,    //0x08
                            0,0,1,1,0,0,0,0,0,    //0x30
                            0,0,0,0,0,0,0,0,0,    //0x00
                            0,0,0,0,0,0,0,0,0,    //0x00
                            0,0,0,1,0,0,0,0,0,    //0x10
                            0,0,0,0,1,1,0,1,0,    //0x0d
                            0,0,0,0,0,0,0,0,0,    //0x00
                            0,0,0,0,0,0,0,0,0,    //0x00
                            0,0                   //2 Stop Bits
  };

void setupFrame(unsigned int id) {
  //ID: Bit 68-71, 73 - 80, 82 - 85
  tagData[67] = (id >> 15) & 0x01;
  tagData[68] = (id >> 14) & 0x01;
  tagData[69] = (id >> 13) & 0x01;
  tagData[70] = (id >> 12) & 0x01;
  tagData[72] = (id >> 11) & 0x01;
  tagData[73] = (id >> 10) & 0x01;
  tagData[74] = (id >> 9) & 0x01;
  tagData[75] = (id >> 8) & 0x01;
  tagData[76] = (id >> 7) & 0x01;
  tagData[77] = (id >> 6) & 0x01;
  tagData[78] = (id >> 5) & 0x01;
  tagData[79] = (id >> 4) & 0x01;
  tagData[81] = (id >> 3) & 0x01;
  tagData[82] = (id >> 2) & 0x01;
  tagData[83] = (id >> 1) & 0x01;
  tagData[84] = (id >> 0) & 0x01;
  
  //Checksum: 95 - 98, 100 - 103
  unsigned char checksum = 0x95 ^ 0x00 ^ 0x83 ^ ((id >> 8) & 0xff) ^ (id & 0xff) ^ 0x01 ^ 0xff;
  tagData[94] = (checksum >> 7) & 0x01;
  tagData[95] = (checksum >> 6) & 0x01;
  tagData[96] = (checksum >> 5) & 0x01;
  tagData[97] = (checksum >> 4) & 0x01;
  tagData[99] = (checksum >> 3) & 0x01;
  tagData[100] = (checksum >> 2) & 0x01;
  tagData[101] = (checksum >> 1) & 0x01;
  tagData[102] = (checksum >> 0) & 0x01;

  Serial.println();
  Serial.println("Tag Emulate");
  Serial.print("Use ID: ");
  Serial.println(id, HEX);
  Serial.print("Use ID-Checksum:");
  Serial.println(checksum, HEX);

  crc.clearCrc();
  unsigned char data;
  for (int i = 0; i < 11; i++){
    data = ((unsigned int) tagData[9+i*9]) << 7 | 
            ((unsigned int) tagData[9+i*9+1]) << 6 | 
            ((unsigned int) tagData[9+i*9+2]) << 5 | 
            ((unsigned int) tagData[9+i*9+3]) << 4 | 
            ((unsigned int) tagData[9+i*9+4]) << 3 | 
            ((unsigned int) tagData[9+i*9+5]) << 2 |
            ((unsigned int) tagData[9+i*9+6]) << 1 |
            ((unsigned int) tagData[9+i*9+7]) << 0;
    crc.updateCrc(data);
  }
  unsigned int frameChecksum;
  frameChecksum = crc.getCrc(); 
  Serial.print("Use Frame-Checksum:");
  Serial.println(frameChecksum, HEX);
  tagData[108] = (frameChecksum >> 15) & 0x01;
  tagData[109] = (frameChecksum >> 14) & 0x01;
  tagData[110] = (frameChecksum >> 13) & 0x01;
  tagData[111] = (frameChecksum >> 12) & 0x01;
  tagData[112] = (frameChecksum >> 11) & 0x01;
  tagData[113] = (frameChecksum >> 10) & 0x01;
  tagData[114] = (frameChecksum >> 9) & 0x01;
  tagData[115] = (frameChecksum >> 8) & 0x01;
  tagData[117] = (frameChecksum >> 7) & 0x01;
  tagData[118] = (frameChecksum >> 6) & 0x01;
  tagData[119] = (frameChecksum >> 5) & 0x01;
  tagData[120] = (frameChecksum >> 4) & 0x01;
  tagData[121] = (frameChecksum >> 3) & 0x01;
  tagData[122] = (frameChecksum >> 2) & 0x01;
  tagData[123] = (frameChecksum >> 1) & 0x01;
  tagData[124] = (frameChecksum >> 0) & 0x01;
}

void setup() {
  pinMode(COILPIN, OUTPUT);
  digitalWrite(COILPIN, LOW);

  inputString.reserve(200);
  
  Serial.begin(115200);
  unsigned int id = ((unsigned int)EEPROM.read(EEPROM_ADDR)) << 8 | EEPROM.read(EEPROM_ADDR+1);
  setupFrame(id);
  Serial.println("Enter new id (in decimal) and return to change emulated id");
  Serial.println("Tag Emulate starts");
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      unsigned int newId = inputString.toInt();
      inputString = "";
      if (!newId){
        Serial.println("Invalid ID received");
        return;
      }
      EEPROM.write(EEPROM_ADDR, newId >> 8);
      EEPROM.write(EEPROM_ADDR+1, newId);
      Serial.print("New ID written: ");
      Serial.println(newId);
      setupFrame(newId);
    }
  }
}

void loop() {  
  for (int i = 0; i < 128; i++){
    delayMicroseconds(BITTIME_US / 2);
    //writeManchester(0, tagData[i]);
    digitalWrite(COILPIN, tagData[i] ? HIGH : LOW);
    delayMicroseconds(BITTIME_US / 2);
    //writeManchester(0, tagData[i]);
    digitalWrite(COILPIN, !tagData[i] ? HIGH : LOW);
  }
}

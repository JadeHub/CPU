#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13
#define EEPROM_SIZE 8191

void setAddress(int address, bool outputEnable)
{
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}

/*
 * Read a byte from the EEPROM at the specified address.
 */
byte readEEPROM(int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}


/*
 * Write a byte to the EEPROM at the specified address.
 */
void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }

  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(50);
}

void printContents() {
  for (int base = 0; base <= 255; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}


bool isStep1Addr(int addr)
{
  return (addr >> 10) == 1;
}

bool isStep0Addr(int addr)
{
  return (addr >> 10) == 0;
}

void writeROM1() {
  char buff[80];
  Serial.println("starting");
  for(int addr=0; addr<EEPROM_SIZE; addr++) {
    if (isStep0Addr(addr)) {
      writeEEPROM(addr, 127);
      //sprintf(buff, "%d is step0", addr);
      //Serial.println(buff);
    }
      
    if (isStep1Addr(addr)) {
      //sprintf(buff, "%d is step1", addr);
      //Serial.println(buff);
      writeEEPROM(addr, 222);
    }
  }
  Serial.println("working");
  writeEEPROM(2049, 127);
  writeEEPROM(3073, 207);
  writeEEPROM(2050, 243);
  writeEEPROM(2051, 125);
  writeEEPROM(3075, 207);
  writeEEPROM(2052, 127);
  writeEEPROM(3076, 223);
  writeEEPROM(2053, 255);
  Serial.println("ROM 1 complete");
}

void writeROM2() {
  Serial.println("starting");
  for(int addr=0; addr<EEPROM_SIZE; addr++) {
    if (isStep0Addr(addr))
      writeEEPROM(addr, 152);
    if (isStep1Addr(addr))
      writeEEPROM(addr, 248);
  }
  Serial.println("working");
  writeEEPROM(2049, 152);
  writeEEPROM(3073, 232);
  writeEEPROM(2050, 168);
  writeEEPROM(2051, 184);
  writeEEPROM(3075, 168);
  writeEEPROM(2052, 152);
  writeEEPROM(3076, 40);
  writeEEPROM(2053, 32);
  Serial.println("ROM 2 complete");
}

#define CHIP1_BLANK 255
#define CHIP2_BLANK 191 //pcc low

void setup()
{ 
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  
  Serial.begin(57600);

  //setAddress(1<<10, true);
  //Serial.println("addr");
  //return;
  byte v1 = readEEPROM(2049);
  byte v2 = readEEPROM(1792);

  char buff[80];
  sprintf(buff, "%d %d", v1, v2);
  Serial.println(buff);
  //return;
  
  // Erase entire EEPROM
  /*
  Serial.print("Erasing EEPROM");
  for (int address = 0; address < EEPROM_SIZE; address += 1) {
    writeEEPROM(address, CHIP2_BLANK);

    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done");*/
  //return;*/
  writeROM2();

  printContents();
}

byte readSerialByte()
{
  byte b[1];
  Serial.readBytes(b, 1);
  return b[0];
}

int count = 0;

void loop() {
  return;
  // put your main code here, to run repeatedly:
  //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(1000);                       // wait for a second
  //digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  //delay(1000);      // wait for a second

  if (!Serial)
  {
    delay(1000);
    return;
  }
  
  if (Serial.available())
  {
    char c = Serial.read();
    if (c == 'R')
    {
    //  Serial.println("Hi");
      for (int addr=0;addr<4096;addr++)
      {
        Serial.write(readEEPROM(addr));
      }
    }
    else if (c == 'W')
    {
      byte b[2];
      Serial.readBytes(b, 2);
      unsigned int len = *(unsigned int*)&b;
      //Serial.readBytes(b, 1);
      
      char buff[80];
      sprintf(buff, "JOSH %u", len);
    //  Serial.println(buff);
     // writeEEPROM(0, val);
      
      if (len > 0 && len <= EEPROM_SIZE)
      {
        /*
        for (long addr=0;addr<len;addr++)
        {
          byte val = readSerialByte();
          writeEEPROM(addr, val);
        }*/

        byte vals[len];
        Serial.readBytes(vals, len);

        for (long addr=0;addr<len;addr++)
        {
          byte val = vals[addr];
          writeEEPROM(addr, val);
        }
        
        Serial.println(buff);
      }
    }
  }
  
}

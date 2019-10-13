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

byte readEEPROM(int address)
{
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

void writeEEPROM(int address, byte data)
{
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

uint8_t digits[] = { 63, 9, 94, 91, 105, 115, 103, 25, 127, 121 };

/*
Address format

             10 9 8 7 6 5 4 3 2 1 0
Ones value    0 0 0 <--  Value --->
Tens value    0 0 1 <--  Value --->
Hundreds value0 1 0 <--  Value --->
*/

void setup() 
{
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  
  Serial.begin(57600);
  Serial.println("starting");

  for (uint16_t addr = 0; addr <= EEPROM_SIZE; addr++)
  {
    writeEEPROM(addr, 0);
    if (addr % 16 == 0)
    {
      Serial.print(".");
    }
  }
  Serial.println("");

  for (uint16_t value = 0; value < 256; value++)
  {
    //units
    writeEEPROM(value, digits[value % 10]);
    //tens
    writeEEPROM(value + 256, digits[(value / 10) % 10]);
    //hundreds
    writeEEPROM(value + 512, digits[(value / 100)]);

    char buff[80];
    sprintf(buff, "%d %d %d", value % 10, (value/10)%10, value /100);
    Serial.println(buff);
  }

  Serial.println("finished");
  return;
}

void loop() {
  // put your main code here, to run repeatedly:

}

#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define MAW 5
#define MW 6
#define OE 7
#define CPU_CLK 13

void outputByte(byte val)
{
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, val);

  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);

  delay(1);
}

void pulseClock()
{
  digitalWrite(CPU_CLK, LOW);
  digitalWrite(CPU_CLK, HIGH);
  digitalWrite(CPU_CLK, LOW);
}

void setAddress(byte addr)
{
  outputByte(addr);
  digitalWrite(MAW, LOW);
  pulseClock();
  digitalWrite(MAW, HIGH);  
}

void write(byte addr, byte val)
{
  setAddress(addr);
  outputByte(val);
  digitalWrite(MW, LOW);
  pulseClock();
  digitalWrite(MW, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  pinMode(CPU_CLK, OUTPUT);
  pinMode(MAW, OUTPUT);
  pinMode(MW, OUTPUT);
  pinMode(OE, OUTPUT);

  digitalWrite(MAW, HIGH);
  digitalWrite(MW, HIGH);
  digitalWrite(OE, HIGH);

  digitalWrite(OE, LOW);

 // setAddress(16);

  write(0, 134);
  write(1, 5);
  write(2, 150);
  write(3, 5);
  write(4, 40);
  write(5, 214);
  write(6, 64);
  write(7, 249);
  write(64, 249);
  /*write(9, 12);
  write(10, 238);
  write(11, 4u);
  write(12, 249);
 */
  digitalWrite(OE, HIGH);
  pinMode(CPU_CLK, INPUT);
  pinMode(MAW, INPUT);
  pinMode(MW, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

}

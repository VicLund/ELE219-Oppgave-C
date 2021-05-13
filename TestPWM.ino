int PIN_0 = 0;
int PIN_1 = 1;
int PIN_2 = 2;
int PIN_3 = 3;
int PIN_4 = 4;
int PIN_5 = 5;
int PIN_6 = 6;
int PIN_7 = 7;
int PIN_8 = 8;
int PIN_10 = 10;
int PIN_12 = 12;
int PIN_18 = 18; //or A3
int PIN_19 = 19; //or A4

void setup() {
}

void loop() {
  analogWrite(0, 250);
  analogWrite(1, 30);
  analogWrite(2, 50);
  analogWrite(3, 70);
  analogWrite(4, 90);
  analogWrite(5, 110);
  analogWrite(6, 130);
  analogWrite(7, 150);
  analogWrite(8, 170);
  analogWrite(10, 190);
  analogWrite(12, 120);
  analogWrite(18, 210);
  analogWrite(19, 230);
}

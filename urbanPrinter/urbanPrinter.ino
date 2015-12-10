///////////////////////////////////////////////////
#include <avr/pgmspace.h>
#include <PS2Keyboard.h>
#include <LiquidCrystal.h>

// PS2 Keyboard
#define KCLOCK 2
#define KDATA 3
PS2Keyboard keyboard;

// PRINTER pins
#define DOT0 A0
#define DOT1 A1
#define DOT2 10
#define DOT3 11
#define DOT4 A5

// I killed A2,A3,A4 !

int PRINTPIN[] = {
  DOT0, DOT1, DOT2, DOT3, DOT4
};

#define TRIGGER 12

// LCD pins
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);
// see http://gnuéscultura.eu/ArduinoProcessing/12_pantalla_lcd.html
// and schema

///////////////////////////////////////////////////
//B00011111, B00000101, B00000101, B00000101, B00011111 };
byte a[5] = { B00011111, B00001001, B00001001, B00001001, B00011111 };
byte b[5] = { B00011111, B00010100, B00010100, B00010100, B00011100 };
byte c[5] = { B00011111, B00010001, B00010001, B00010001, B00010001 };
byte d[5] = { B00011100, B00010100, B00010100, B00010100, B00011111 };
byte e[5] = { B00011111, B00010101, B00010101, B00010101, B00010001 };
byte f[5] = { B00011111, B00000101, B00000101, B00000101, B00000001 };
byte g[5] = { B00011111, B00010001, B00010001, B00010101, B00011101 };
byte h[5] = { B00011111, B00000100, B00000100, B00000100, B00011111 };
byte i[5] = { B00010001, B00010001, B00011111, B00010001, B00010001 };
byte j[5] = { B00010000, B00010000, B00010001, B00011111, B00010001 };
byte k[5] = { B00011111, B00000100, B00001110, B00011011, B00010001 };
byte l[5] = { B00011111, B00010000, B00010000, B00010000, B00010000 };
byte m[5] = { B00011111, B00000011, B00001100, B00000011, B00011111 };
byte n[5] = { B00011111, B00000011, B00001110, B00011000, B00011111 };
byte o[5] = { B00011111, B00010001, B00010001, B00010001, B00011111 };
byte p[5] = { B00011111, B00000101, B00000101, B00000101, B00000111 };
byte q[5] = { B00000111, B00000101, B00000101, B00000101, B00011111 };
byte r[5] = { B00011111, B00000101, B00001101, B00010111, B00010000 };
byte s[5] = { B00010111, B00010101, B00010101, B00010101, B00011101 };
byte t[5] = { B00000001, B00000001, B00011111, B00000001, B00000001 };
byte u[5] = { B00011111, B00010000, B00010000, B00010000, B00011111 };
byte v[5] = { B00000011, B00011100, B00010000, B00011100, B00000011 };
byte w[5] = { B00011111, B00001000, B00001100, B00001000, B00011111 };
byte x[5] = { B00010001, B00001010, B00000100, B00001010, B00010001 };
byte y[5] = { B00000111, B00000100, B00010100, B00010100, B00011111 };
byte z[5] = { B00010001, B00001000, B00000100, B00000010, B00010001 };

const byte* alph[26] = { a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z };

byte erasor[5] = {
  B11111111, B11111111, B11111111, B11111111, B11111111
};
byte grid[5] = {
  B10101010, B01010101, B10101010, B01010101, B10101010
};

const byte* spec[2] = {
  erasor, grid
};
///////////////////////////////////////////////////
boolean config = true;
char sentenceHard[] = "ab";
char sentenceSet[99];
int charP = 0;

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  // PRINTER
  pinMode(DOT0, OUTPUT);
  pinMode(DOT1, OUTPUT);
  pinMode(DOT2, OUTPUT);
  pinMode(DOT3, OUTPUT);
  pinMode(DOT4, OUTPUT);
  digitalWrite(DOT0, LOW);
  digitalWrite(DOT1, LOW);
  digitalWrite(DOT2, LOW);
  digitalWrite(DOT3, LOW);
  digitalWrite(DOT4, LOW);

  // TRIGGER
  pinMode(TRIGGER, INPUT);

  // KEYBOARD
  keyboard.begin(KDATA, KCLOCK, PS2Keymap_French);

  // LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Against bush !");

  Serial.println("Ok. I'm ready Bush");
  delay(100);

  //startPrinting(sentence);
}

void loop() {

  delay(1);
  

  //// keyboard char receive loop
  if (keyboard.available()) {
    // read the next key
    char c = keyboard.read();
    receiveChar(c);
  }
}

boolean shouldgo() {
  return (digitalRead(TRIGGER) == LOW);
}

void addCharToMemory(char ch) {
  sentenceSet[charP] = ch;
  charP += 1;
  Serial.println(sentenceSet);
  lcd.setCursor(0, 1);
  lcd.print(sentenceSet);
}
void receiveChar(char ch) {
  //Serial.println(ch);

  int charint = (int)ch;
  if (charint >= 97 && charint <= 122) { // ALPHABET !!
    if (config) addCharToMemory(ch);
  }
  if (charint == 32) { // SPACE
    if (config) addCharToMemory(ch);
  }
  if (ch == PS2_TAB) { // TAB
    // ?
  }
  if (ch == PS2_BACKSPACE) { // ERASOR
    closeVannes();
    lcd.clear();
    config = true;
    memset(sentenceSet, 0, sizeof(sentenceSet));
    charP = 0;
  }
  if (ch == PS2_ENTER) { // NEWLINE || ch==PS2_DOWNARROW
    lcd.clear();
    Serial.println("GO PRINT !");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ETAT d'URGENCE");
    lcd.setCursor(0, 1);
    lcd.print(sentenceSet);
    config = false;
    startPrinting(sentenceSet, charP);
    charP = 0;
  }
}

void startPrinting(char* sent, int l) {

  int len = strlen(sent) - 1;
  if (l)
    len = l + 1;

  Serial.print("print: ");
  Serial.println(sent);
  Serial.print("size is: ");
  Serial.println(l);

  for (int i = 0; i < l; i = i + 1) {
    printLetter(sent[i]);
    waitLetter();
  }
  closeVannes();
  lcdPrint("Pole emploi.");
}

void closeVannes() {
  for (int dot = 0; dot < 5; dot = dot + 1) {
    digitalWrite(PRINTPIN[dot], LOW);
  }
}

void lcdPrint(String str) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str);
}
void waitLine() { // between lines
  delay(500);
}
void waitLetter() { // between letters
  delay(2000);
}
void printLetter(char c) {
  Serial.print("print letter: ");
  Serial.println(c);
  int w = (int)c - 97;
  //Serial.println(w);
  const byte *charlist = alph[w];

  // for each line
  for (int vert = 0; vert < 5; vert = vert + 1) {
    lcdPrint((String)vert);
    if (!config) {
      // for each dot
      Serial.println("vertically");
      for (int dot = 0; dot < 5; dot = dot + 1) {
        //charint is from keyboard
        //boolean on = alph[charint-97];
        boolean on = charlist[vert] & (1 << dot);
        Serial.println(on ? "X" : "O");
        if (on) {
          //if(shouldgo())
          digitalWrite(PRINTPIN[dot], HIGH);
        }
        else
          digitalWrite(PRINTPIN[dot], LOW);
      }
      waitLine();
    }
  }
  // at the end of the letter close all ? maybe not
  closeVannes();
}









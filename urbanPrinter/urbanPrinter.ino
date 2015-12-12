///////////////////////////////////////////////////
#include <avr/pgmspace.h>
#include <PS2Keyboard.h>
#include <LiquidCrystal.h>

#define WAITDOT 500
#define WAITLETTER 800
#define WAITSPACE 1200

// PS2 Keyboard
#define KCLOCK 2
#define KDATA 3
PS2Keyboard keyboard;

// PRINTER pins
#define DOT0 A0
#define DOT1 A1
#define DOT2 A2
#define DOT3 A3
#define DOT4 A4

// I killed A2,A3,A4 ... lack of light ! (on the mini board)

int PRINTPIN[] = {
  DOT0, DOT1, DOT2, DOT3, DOT4
};

#define TRIGGER 12

// LCD pins
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);
// see http://gnuéscultura.eu/ArduinoProcessing/12_pantalla_lcd.html
// and schema

///////////////////////////////////////////////////
const byte a[5] = { B00011111, B00000101, B00000101, B00000101, B00011111 };
const byte b[5] = { B00011111, B00010100, B00010100, B00010100, B00011100 };
const byte c[5] = { B00011111, B00010001, B00010001, B00010001, B00010001 };
const byte d[5] = { B00011100, B00010100, B00010100, B00010100, B00011111 };
const byte e[5] = { B00011111, B00010101, B00010101, B00010101, B00010001 };
const byte f[5] = { B00011111, B00000101, B00000101, B00000101, B00000001 };
const byte g[5] = { B00011111, B00010001, B00010001, B00010101, B00011101 };
const byte h[5] = { B00011111, B00000100, B00000100, B00000100, B00011111 };
const byte i[5] = { B00010001, B00010001, B00011111, B00010001, B00010001 };
const byte j[5] = { B00010000, B00010000, B00010001, B00011111, B00010001 };
const byte k[5] = { B00011111, B00000100, B00001110, B00011011, B00010001 };
const byte l[5] = { B00011111, B00010000, B00010000, B00010000, B00010000 };
const byte m[5] = { B00011111, B00000011, B00001100, B00000011, B00011111 };
const byte n[5] = { B00011111, B00000011, B00001110, B00011000, B00011111 };
const byte o[5] = { B00011111, B00010001, B00010001, B00010001, B00011111 };
const byte p[5] = { B00011111, B00000101, B00000101, B00000101, B00000111 };
const byte q[5] = { B00000111, B00000101, B00000101, B00000101, B00011111 };
const byte r[5] = { B00011111, B00000101, B00001101, B00010111, B00010000 };
const byte s[5] = { B00010111, B00010101, B00010101, B00010101, B00011101 };
const byte t[5] = { B00000001, B00000001, B00011111, B00000001, B00000001 };
const byte u[5] = { B00011111, B00010000, B00010000, B00010000, B00011111 };
const byte v[5] = { B00000011, B00011100, B00010000, B00011100, B00000011 };
const byte w[5] = { B00011111, B00001000, B00001100, B00001000, B00011111 };
const byte x[5] = { B00010001, B00001010, B00000100, B00001010, B00010001 };
const byte y[5] = { B00000111, B00000100, B00010100, B00010100, B00011111 };
const byte z[5] = { B00010001, B00001000, B00000100, B00000010, B00010001 };

const byte space[5] = {B00000000, B00000000, B00000000, B00000000, B00000000}; //26
const byte plain[5] = {B11111111, B11111111, B11111111, B11111111, B11111111}; //27
const byte grid[5] = {B10101010, B01010101, B10101010, B01010101, B10101010}; //28

const byte* alph[29] = { a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, space, plain, grid };

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
  
  closeVannes();
  
  // TRIGGER
  pinMode(TRIGGER, INPUT);

  // KEYBOARD
  keyboard.begin(KDATA, KCLOCK, PS2Keymap_French);

  // LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Against bush !");

  //Serial.println("Ok. I'm ready Bush");
  delay(100);
}

boolean debug = true;
int debugstyle = 0;
boolean debugstyleset = false;
void loop() {

  delay(10);

  ////////// LCD feedback !
  lcd.setCursor(13,0);
  if(debug) {
    lcd.print("DB");
  } else {
    lcd.print("GO");
  }
  if(config) lcd.print("?");

  ////////// simple motifs to test !
  if(debug) {
    if(shouldgo()) {
      //Serial.println("GOTEST");
      lcd.setCursor(0,1);
      if((debugstyle%3)==0) printVerticalPixels(B0010101);
      if((debugstyle%3)==1) printVerticalPixels(B0001010);
      if((debugstyle%3)==2) printVerticalPixels(B0011011);
      debugstyleset = false;
    } else {
      closeVannes();
      if(!debugstyleset) debugstyle = debugstyle+1;
      debugstyleset = true;
    }
    
  }
  
  ////////// keyboard char receive loop
  if (keyboard.available()) {
    // read the next key
    char c = keyboard.read();
    receiveChar(c);
  }
}

boolean shouldgo() {
  return (digitalRead(TRIGGER) == LOW);
}
boolean sleepIfNotGo() {
  while(!shouldgo()) {
    closeVannes();
    delay(10);
  }
}

void addCharToMemory(char ch) {
  sentenceSet[charP] = ch;
  charP += 1;
  //Serial.println(sentenceSet);
  lcd.setCursor(0, 1);
  lcd.print(sentenceSet);
}
void receiveChar(char ch) {

  int charint = (int)ch;
  Serial.println(charint);
  if (charint >= 97 && charint <= 122) { // ALPHABET !!
    if (config) addCharToMemory(ch);
  }
  if (charint == 32) { // SPACE
    if (config) addCharToMemory(ch);
  }
  if (ch == PS2_TAB) { // TAB
    // ?
  }
  if (charint == 61) { // "="
    debug = !debug;
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
    //Serial.println("GO PRINT !");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ETAT d'URGENCE");
    config = false;
    startPrinting(sentenceSet, charP);
    charP = 0;
  }
}

void startPrinting(char* sent, int l) {

  int len = strlen(sent) - 1;
  if (l)
    len = l + 1;

  //Serial.print("print: ");
  //Serial.println(sent);
  //Serial.print("size is: ");
  //Serial.println(l);

  for (int i = 0; i < l; i = i + 1) {
    printLetter(sent[i]);
    waitLetter();
  }
  closeVannes();
  lcdPrint("Pole emploi.");
}

void closeVannes() {
  for (int dot = 0; dot < 5; dot = dot + 1) {
    analogWrite(PRINTPIN[dot], 0);
  }
}

void lcdPrint(String str) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str);
}
void waitLine() { // between lines
  delay(WAITDOT);
}
void waitLetter() { // between letters
  delay(WAITLETTER);
}
void printLetter(char c) {
  //Serial.print("print letter: ");
  //Serial.println(c);
  int w = (int)c - 97;
  
  //Serial.println("Char index:");
  //Serial.println(w);
  
  if(c==32) w = 26; //space
  
  //Serial.println(w);
  const byte *charlist = alph[w];
  lcd.setCursor(0,1);
  lcd.print(String(c));
  
  // for each vertical line of pixels
  for (int vert = 0; vert < 5; vert = vert + 1) {

    //lcd.setCursor(15,0);
    lcd.setCursor(2,1);
    lcd.print(String(vert));
    lcd.print(" ");

    // if not triggerred, while-wait-sleep !
    sleepIfNotGo();
      
    if (!config) {
      printVerticalPixels( charlist[vert] );
      waitLine();
    }
  }
  // at the end of the letter close all ? maybe not ?
  closeVannes();
}

void printVerticalPixels(byte motif) {
  // for each dot
  for (int pix = 0; pix < 5; pix = pix + 1) {
    boolean on = motif & (1 << pix);
    //Serial.println(on ? "X" : "O");
    if(on>0) {
      analogWrite(PRINTPIN[pix], 255);
      lcd.print("X");
    } else {
      analogWrite(PRINTPIN[pix], 0);
      lcd.print("_");
    }
  }
}







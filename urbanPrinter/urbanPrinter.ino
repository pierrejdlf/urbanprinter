///////////////////////////////////////////////////
#include <avr/pgmspace.h>
#include <PS2Keyboard.h>
#include <LiquidCrystal.h>

#define WAITDOT 100 // 350 // 500
#define WAITLETTER 400
#define WAITSPACE 800

int nSentences = 19;
char* sentenceHard[19] = {

};
int choosen = 0;

// PS2 Keyboard
#define KCLOCK 2
#define KDATA 3
PS2Keyboard keyboard;

// PRINTER pins
#define DOT4 A0
#define DOT3 A1
#define DOT2 A2
#define DOT1 A3
#define DOT0 A4

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
const byte a[5] = { B00011110, B00000101, B00000101, B00000101, B00011110 };
const byte b[5] = { B00011111, B00010101, B00010101, B00010101, B00001010 };
const byte c[5] = { B00001110, B00010001, B00010001, B00010001, B00010001 };
const byte d[5] = { B00011111, B00010001, B00010001, B00010001, B00001110 };
const byte e[5] = { B00011111, B00010101, B00010101, B00010101, B00010001 };
const byte f[5] = { B00011111, B00000101, B00000101, B00000101, B00000001 };
const byte g[5] = { B00001110, B00010001, B00010001, B00010101, B00011101 };
const byte h[5] = { B00011111, B00000100, B00000100, B00000100, B00011111 };
const byte i[5] = { B00000000, B00010001, B00011111, B00010001, B00000000 };
const byte j[5] = { B00001000, B00010000, B00010001, B00001111, B00000001 };
const byte k[5] = { B00011111, B00000100, B00001010, B00011011, B00010001 };
const byte l[5] = { B00011111, B00010000, B00010000, B00010000, B00010000 };
const byte m[5] = { B00011111, B00000010, B00000100, B00000010, B00011111 };
const byte n[5] = { B00011111, B00000010, B00001100, B00010000, B00011111 };
const byte o[5] = { B00001110, B00010001, B00010001, B00010001, B00001110 };
const byte p[5] = { B00011111, B00000101, B00000101, B00000101, B00000111 };
const byte q[5] = { B00001110, B00010001, B00010101, B00011001, B00011110 };
const byte r[5] = { B00011111, B00000101, B00001101, B00010101, B00010111 };
const byte s[5] = { B00010011, B00010101, B00010101, B00010101, B00011001 };
const byte t[5] = { B00000001, B00000001, B00011111, B00000001, B00000001 };
const byte u[5] = { B00011111, B00010000, B00010000, B00010000, B00011111 };
const byte v[5] = { B00000011, B00001100, B00010000, B00001100, B00000011 };
const byte w[5] = { B00011111, B00010000, B00001100, B00010000, B00011111 };
const byte x[5] = { B00010001, B00001010, B00000100, B00001010, B00010001 };
const byte y[5] = { B00000001, B00000010, B00011100, B00000010, B00000001 };
const byte z[5] = { B00010001, B00001000, B00000100, B00000010, B00010001 };

const byte space[5] = {B00000000, B00000000, B00000000, B00000000, B00000000}; //26
const byte plain[5] = {B11111111, B11111111, B11111111, B11111111, B11111111}; //27
const byte grid[5] = {B10101010, B01010101, B10101010, B01010101, B10101010}; //28

const byte dot[5] = {B00000000, B00011000, B00011000, B0000000, B00000000}; //46 (but real dot is:)
const byte tiret[5] = {B00000000, B00000100, B00000100, B00000100, B00000000}; //45
const byte apos[5] = {B00000000, B00000100, B00000011, B00000000, B00000000}; //39

const byte* alph[32] = { a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, space, plain, grid, dot, tiret, apos };

///////////////////////////////////////////////////
#define DBG 0
#define MEM 1
#define KEY 2
char* modeStr[3] = {"DBG","MEM","KEY"};
int mode = 0;
boolean config = true;

// live typed keyboard sentence
char sentenceSet[150];
int charP = 0;

// debug pattern
byte digits = B11111111;
int debugstyle = 0;
boolean debugstyleset = false;


///////////////////////////////////////////////////
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
  lcd.print("Ag. bush.");

  //Serial.println("Ok. I'm ready Bush");
  delay(100);
}

///////////////////////////////////////////////////
void loop() {
  delay(5);

  ////////// LCD feedback !
  lcd.setCursor(13,0);
  lcd.print(modeStr[mode]);

  if(config) lcd.print("?");

  ////////// simple motifs to test !
  if(mode==DBG) {
    if(shouldgo()) {
      //Serial.println("GOTEST");
      lcd.setCursor(0,1);

      printVerticalPixels(digits);

      debugstyleset = false;
    } else {
      closeVannes();
      if(!debugstyleset) {
        debugstyle = debugstyle+1;
        // set to next pattern ?
        //setDigit()
      }
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

// Set debug pattern
void setDigit(int dig) {
  if(dig==0) 
    digits = B00000000;
  else {
    if(dig<6)
      digits = digits ^ (1 << (dig-1)); // toggle the digit // (1 << (dig-1)); 
    if(dig==6)
      digits = B00010101;
    if(dig==7)
      digits = B00000100;
    if(dig==8)
      digits = B00010001;
    if(dig==9)
      digits = B00011111;
  }
  Serial.println(digits,BIN);
};

// trigger(s) to start/pause
boolean shouldgo() {
  return (digitalRead(TRIGGER) == LOW);
}
boolean sleepIfNotGo() {
  while(!shouldgo()) {
    closeVannes();
    delay(10);
  }
}

// Keyboard setting sentence
void addCharToMemory(char ch) {
  sentenceSet[charP] = ch;
  charP += 1;
  //Serial.println(sentenceSet);
  lcd.setCursor(0, 1);
  lcd.print(sentenceSet);
}

// Global Keyboard receive
void receiveChar(char ch) {

  int charint = (int)ch;
  Serial.println(charint);
  if (charint >= 48 && charint <= 57) { // DIGITS !!
    int digit = charint - 48;
    if (mode==DBG) setDigit(digit);
  }
  if (charint >= 97 && charint <= 122) { // ALPHABET !!
    if (mode==KEY && config) addCharToMemory(ch);
  }
  if (charint==46 || charint==45 || charint==39 || charint==32) { // specials: space, tiret, apos...
    if (mode==KEY && config) addCharToMemory(ch);
  }
  if (ch == PS2_TAB) { // TAB
    // ?
  }
  if (charint == 42) { // "*" 
    mode = (mode+1) % 3;
    config = mode==KEY;
    choosen = 0;
  }
  if (ch == PS2_BACKSPACE) { // ERASOR
    choosen = 0;
    mode = KEY;
    closeVannes();
    lcd.clear();
    config = true;
    memset(sentenceSet, 0, sizeof(sentenceSet));
    charP = 0;
  }
  if (ch==43 || ch==45) { // + / -
    if(mode==MEM) {
      choosen += (ch==43) ? 1 : -1;
      choosen = choosen % nSentences;
      if(choosen<0) choosen = nSentences-1;
      Serial.print("Choosen sentence i:");
      Serial.println(choosen);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(sentenceHard[choosen]);
      Serial.println(sentenceHard[choosen]);
    }
  }
  if (ch == PS2_ENTER) { // NEWLINE || ch==PS2_DOWNARROW
    //Serial.println("GO PRINT !");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ETAT d'URGENCE");
    config = false;
    if(mode==KEY)
      startPrinting(sentenceSet, charP);
    if(mode==MEM)
      startPrinting(sentenceHard[choosen], false);
    charP = 0;
  }
}

void startPrinting(char* sent, int l) {

  int len = strlen(sent);
  if (l)
    len = l;

  //Serial.print("print: ");
  //Serial.println(sent);
  //Serial.print("size is: ");
  //Serial.println(len);

  for (int i = 0; i < len; i = i + 1) {
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
  Serial.print("print letter: ");
  Serial.println(c);

  int w = (int)c - 97;

  //Serial.println("Char index:");
  //Serial.println(w);
  
  if((int)c==32) w = 26; //space
  if((int)c==46) w = 29; //dot
  if((int)c==45) w = 30; //tiret
  if((int)c==39) w = 31; //apos
  
  
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






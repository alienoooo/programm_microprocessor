#include <LedControl.h>

#define DIN 11
#define CLK 13
#define CS  10
#define NUM_DEV 4         // <-- поставь реальное число модулей
#define BRIGHT 8          // 0..15
#define SPEED_MS 40       // скорость прокрутки
#define REVERSE_DEVICES 0 // 1 — если модули идут в обратном порядке
#define FLIP_ROWS_BITS  0 // 1 — если буквы зеркалятся по горизонтали

const char* TEXT = "  HELLO WORLD 2025  ";

LedControl lc(DIN, CLK, CS, NUM_DEV);
uint8_t buf[8][64]; // до 8 модулей (увеличь 64, если модулей >8)

void setup() {
  for (int d=0; d<NUM_DEV; d++) {
    lc.shutdown(d, false);
    lc.setScanLimit(d, 7);
    lc.setIntensity(d, BRIGHT);
    lc.clearDisplay(d);
  }
  memset(buf, 0, sizeof(buf));
}

// отрисовка буфера в модули
void flush(){
  for (int dev=0; dev<NUM_DEV; dev++){
    int outDev = REVERSE_DEVICES ? (NUM_DEV-1-dev) : dev;
    for (int row=0; row<8; row++){
      uint8_t v = 0;
      for (int b=0; b<8; b++){
        int x = dev*8 + b;
        if (buf[row][x]) v |= (1<<b);
      }
      if (FLIP_ROWS_BITS){
        uint8_t r=0; for (int i=0;i<8;i++) if (v&(1<<i)) r|=(1<<(7-i)); v=r;
      }
      lc.setRow(outDev, row, v);
    }
  }
}

inline void shiftLeft(){
  int W = NUM_DEV*8;
  for (int y=0; y<8; y++){
    for (int x=0; x<W-1; x++) buf[y][x] = buf[y][x+1];
    buf[y][W-1] = 0;
  }
}

inline void putCol(uint8_t colBits){
  int W = NUM_DEV*8;
  for (int y=0; y<8; y++){
    buf[y][W-1] = (colBits >> (7-y)) & 1; // бит7 — верхний пиксель
  }
}

// 5x7 шрифт (минимально достаточный набор)
struct Glyph { char ch; uint8_t col[5]; };
const Glyph FONT[] = {
  {' ',{0x00,0x00,0x00,0x00,0x00}},
  {'0',{0x3E,0x51,0x49,0x45,0x3E}}, {'1',{0x00,0x42,0x7F,0x40,0x00}},
  {'2',{0x42,0x61,0x51,0x49,0x46}}, {'3',{0x21,0x41,0x45,0x4B,0x31}},
  {'4',{0x18,0x14,0x12,0x7F,0x10}}, {'5',{0x27,0x45,0x45,0x45,0x39}},
  {'6',{0x3C,0x4A,0x49,0x49,0x30}}, {'7',{0x01,0x71,0x09,0x05,0x03}},
  {'8',{0x36,0x49,0x49,0x49,0x36}}, {'9',{0x06,0x49,0x49,0x29,0x1E}},
  {'A',{0x7E,0x09,0x09,0x09,0x7E}}, {'B',{0x7F,0x49,0x49,0x49,0x36}},
  {'C',{0x3E,0x41,0x41,0x41,0x22}}, {'D',{0x7F,0x41,0x41,0x22,0x1C}},
  {'E',{0x7F,0x49,0x49,0x49,0x41}}, {'F',{0x7F,0x09,0x09,0x09,0x01}},
  {'H',{0x7F,0x08,0x08,0x08,0x7F}}, {'L',{0x7F,0x40,0x40,0x40,0x40}},
  {'O',{0x3E,0x41,0x41,0x41,0x3E}}, {'R',{0x7F,0x09,0x19,0x29,0x46}},
  {'W',{0x7F,0x20,0x18,0x20,0x7F}},
};

bool getGlyph(char c, uint8_t out[5]){
  if (c>='a' && c<='z') c -= 32;
  for (unsigned i=0; i<sizeof(FONT)/sizeof(FONT[0]); ++i){
    if (FONT[i].ch == c){ memcpy(out, FONT[i].col, 5); return true; }
  }
  memset(out, 0, 5); return false;
}

void loop(){
  const char* s = TEXT;
  while (*s){
    uint8_t g[5]; getGlyph(*s, g);
    for (int k=0; k<5; k++){
      shiftLeft(); putCol(g[k]); flush(); delay(SPEED_MS);
    }
    // зазор 1 колонка
    shiftLeft(); flush(); delay(SPEED_MS);
    ++s;
  }
  // небольшой зазор в конце строки
  for (int i=0;i<8;i++){ shiftLeft(); flush(); delay(SPEED_MS); }
}
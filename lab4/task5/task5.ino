#include <LedControl.h>

// Пины: DIN=11, CLK=13, CS=10. Один модуль (индекс 0).
LedControl lc(11, 13, 10, 1);

// --------- параметры узора ---------
const uint8_t ORIGIN_X = 3;     // координата источника лучей (0..7)
const uint8_t ORIGIN_Y = 3;     // подвинь при желании (центр/угол)
const uint8_t RAY_SPEED = 80;   // мс между кадрами роста лучей
const uint8_t TWINKLE_PERIOD = 150; // как часто «подмигивают» звёзды (мс)
const uint8_t STAR_COUNT = 10;  // сколько постоянных звёзд на фоне (0..64)

// --------- внутренние буферы ---------
uint8_t buf[8];                 // экранный буфер (каждый байт — строка 8 бит)
bool stars[8][8];               // карта звёзд
unsigned long tRay=0, tTw=0;
uint8_t rayLen = 0;

// восемь направлений (dx,dy): 0°,45°,90°,...,315°
const int8_t DX[8] = { 1, 1, 0,-1,-1,-1, 0, 1 };
const int8_t DY[8] = { 0, 1, 1, 1, 0,-1,-1,-1 };

// ===== утилиты =====
inline void clearBuf(){ for (uint8_t r=0;r<8;r++) buf[r]=0; }
inline void setPixel(uint8_t x,uint8_t y,bool on){
  if (x>7||y>7) return;
  if (on) buf[y] |=  (1<<x);
  else    buf[y] &= ~(1<<x);
}
inline void flush(){ for(uint8_t r=0;r<8;r++) lc.setRow(0, r, buf[r]); }

void seedStars(){
  // инициализируем «постоянные» звёзды в случайных позициях
  for(uint8_t y=0;y<8;y++) for(uint8_t x=0;x<8;x++) stars[y][x]=false;
  randomSeed(analogRead(A0)); // если A0 свободен
  uint8_t placed=0, guard=0;
  while(placed<STAR_COUNT && guard<200){
    uint8_t x=random(0,8), y=random(0,8);
    if (!stars[y][x] && !(x==ORIGIN_X && y==ORIGIN_Y)){
      stars[y][x]=true; placed++;
    }
    guard++;
  }
}

void drawStars(){
  for(uint8_t y=0;y<8;y++)
    for(uint8_t x=0;x<8;x++)
      if (stars[y][x]) setPixel(x,y,true);
}

void twinkle(){
  // лёгкое «мигание»: изредка инвертируем 1–2 случайные звезды на кадр
  for(uint8_t k=0;k<2;k++){
    uint8_t x=random(0,8), y=random(0,8);
    if (!(x==ORIGIN_X && y==ORIGIN_Y)) {
      bool cur = (buf[y] >> x) & 1;
      setPixel(x,y,!cur);
    }
  }
}

void drawRays(uint8_t len){
  // рисуем лучи от ORIGIN длиной len во всех 8 направлениях
  setPixel(ORIGIN_X, ORIGIN_Y, true); // сам источник
  for(uint8_t dir=0; dir<8; dir++){
    int8_t x=ORIGIN_X, y=ORIGIN_Y;
    for(uint8_t s=0; s<len; s++){
      x += DX[dir]; y += DY[dir];
      if (x<0||x>7||y<0||y>7) break;
      setPixel(x,y,true);
    }
  }
}

void setup(){
  lc.shutdown(0,false);
  lc.setIntensity(0,8);   // яркость 0..15
  lc.clearDisplay(0);

  clearBuf();
  seedStars();
  tRay=millis();
  tTw =millis();
}

void loop(){
  unsigned long now = millis();

  // рост лучей (анимация «расходятся»)
  if (now - tRay >= RAY_SPEED){
    tRay = now;
    rayLen = (rayLen + 1);
    if (rayLen > 8) rayLen = 0; // перезапуск волны
  }

  // «мигание» звёзд раз в TWINKLE_PERIOD
  bool doTwinkle = (now - tTw >= TWINKLE_PERIOD);
  if (doTwinkle) tTw = now;

  // кадр
  clearBuf();
  drawStars();                  // фон
  drawRays(rayLen);             // лучи от точки
  if (doTwinkle) twinkle();     // лёгкая жизнь на фоне
  flush();
}
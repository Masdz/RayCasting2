#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#define PINY 14
#define PINX 15
#define PINZ 16

#define VISION 70
#define VISIONH 17
#define DISTANCIA 5
#define HEIGHT 56
#define WIDTH 32
#define VELOCIDAD 8
#define VELOCIDADGIRO 25

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define pugface_width 16
#define pugface_height 16
const static unsigned char pugface_bits[] PROGMEM = {
   0x1e, 0x00, 0xff, 0x07, 0xff, 0x0f, 0xff, 0x1f, 0xff, 0x3f, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0x3f,
   0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xfe, 0x1f };

#define pugface2_width 16
#define pugface2_height 16
const static unsigned char pugface2_bits[] PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0xfc, 0x03, 0xe4, 0x07, 0xca, 0x0f,
   0x42, 0x0c, 0x02, 0x1a, 0x00, 0x08, 0x00, 0x08, 0x00, 0x0c, 0x02, 0x0e,
   0x04, 0x06, 0x18, 0x03, 0xf8, 0x03, 0xf8, 0x07 };


#define rifle_width 32
#define rifle_height 32
const static unsigned char rifle_bits[] PROGMEM = {
   0x1f, 0x00, 0x00, 0x00, 0xff, 0xff, 0x1f, 0x00, 0xff, 0xff, 0x3f, 0x00,
   0xff, 0xff, 0x7f, 0x00, 0xff, 0xff, 0xff, 0x00, 0xfc, 0xff, 0xff, 0x01,
   0xc0, 0xff, 0xff, 0x07, 0x00, 0xe0, 0xff, 0x07, 0x00, 0xc0, 0xff, 0x07,
   0x00, 0x80, 0xff, 0x07, 0x00, 0x80, 0xff, 0x03, 0x00, 0x00, 0xfb, 0x03,
   0x00, 0x00, 0xff, 0x03, 0x00, 0x00, 0xfe, 0x03, 0x00, 0x00, 0xe0, 0x07,
   0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xc0, 0x0f, 0x00, 0x00, 0xc0, 0x0f,
   0x00, 0x00, 0xc0, 0x0f, 0x00, 0x00, 0x80, 0x1f, 0x00, 0x00, 0x80, 0x1f,
   0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x7f,
   0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x7e,
   0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0xe0,
   0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00 };

#define rifle2_width 32
#define rifle2_height 32
const static unsigned char rifle2_bits[] PROGMEM = {
   0xff, 0xff, 0x3f, 0x00, 0x01, 0x00, 0x60, 0x00, 0xb5, 0x6d, 0xcb, 0x00,
   0xfd, 0xff, 0x9f, 0x01, 0xf9, 0xff, 0x3f, 0x03, 0x83, 0xff, 0x7f, 0x0e,
   0x3e, 0xc0, 0xff, 0x08, 0xe0, 0x9f, 0xff, 0x09, 0x00, 0x30, 0xff, 0x0b,
   0x00, 0x60, 0xff, 0x09, 0x00, 0x40, 0xf2, 0x0d, 0x00, 0xc0, 0xf6, 0x05,
   0x00, 0x80, 0xfc, 0x05, 0x00, 0x80, 0xc1, 0x0d, 0x00, 0x00, 0xdf, 0x09,
   0x00, 0x00, 0x90, 0x1b, 0x00, 0x00, 0xb0, 0x13, 0x00, 0x00, 0xa0, 0x17,
   0x00, 0x00, 0x20, 0x37, 0x00, 0x00, 0x60, 0x27, 0x00, 0x00, 0x40, 0x6e,
   0x00, 0x00, 0x40, 0x4e, 0x00, 0x00, 0xc0, 0xde, 0x00, 0x00, 0x80, 0x9c,
   0x00, 0x00, 0x80, 0xbd, 0x00, 0x00, 0x00, 0xbd, 0x00, 0x00, 0x00, 0xa5,
   0x00, 0x00, 0x00, 0xbd, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x9f,
   0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0xf0 };

const byte mapa[] = {
  0b00000000,
  0b01010100,
  0b00000000,
  0b01010100,
  0b00000000,
  0b00111100,
  0b00000000,
  0b00000000
};

const byte enemigos[] = {
  0b00000001,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00010000,
  0b00000000,
  0b00000000,
  0b00000000
};

const int texturas[] PROGMEM = {
  0b1111011111101111,
  0b1110011111101111,
  0b1100011110000111,
  0b1101111101110111,
  0b1101111001110011,
  0b0000000001110000,
  0b0111011100000110,
  0b1110011110011111,
  0b1100011100111111,
  0b1001011101111111,
  0b0011001000011110,
  0b0111000011001000,
  0b0111000111100010,
  0b0011001111101111,
  0b1000011111001111,
  0b1011111111001111,

  0b0111101111011110,
  0b1011110110111101,
  0b1101111001111011,
  0b1100000000000011,
  0b1101111001111011,
  0b1011110110111101,
  0b0111101111011110,
  0b0000001111000000,
  0b0111101111011110,
  0b1011110110111101,
  0b1101111001111011,
  0b1100000000000011,
  0b1101111001111011,
  0b1011110110111101,
  0b0111101111011110,
  0b0000001111000000,

  0b1111011101101111,
  0b1100101011110011,
  0b1111111011111111,
  0b1000001110000011,
  0b0011000000011001,
  0b0011001110011001,
  0b1000000000000011,
  0b1100000000000110,
  0b0010011111010010,
  0b0100011111000100,
  0b0111111111111101,
  0b1000000111100111,
  0b1110000111100110,
  0b1111000011101101,
  0b1101111111111110,
  0b1110111111111101
};

struct Player {
  float posX = 1.5;
  float posY = 1.5;
  int angulo = 180;
};
struct Sprite{
  byte x = 0;
  byte y = 0;
  byte inicio = 0;
  byte final = 0;
  byte textura = 0;
  float largo = 0;
  boolean actualizado =false;
};
struct Pintable{
  byte tx = 0;
  byte x = 0;
  byte textura = 0;
  byte alto = 0;
  byte luz = 0;
  float largo = 0;
  
  void setTx(float t){
    t = (byte)(abs(t)*100)%100;
    tx = 16 * t / 100;
  }
};
struct Rayo {
  float pPosX;
  float pPosY;
  float seno;
  float coseno;
  float tangente;
  int dirX;
  int dirY;

  void setAngulo(float angulo) {
    if (angulo >= 360) {
      angulo -= 360;
    } else if (angulo < 0) {
      angulo += 360;
    }
    dirY = (angulo > 180) ? -1 : 1;
    dirX = (angulo > 90 && angulo < 270) ? -1 : 1;
    angulo = 31416 * angulo / 1800000;
    seno = sin(angulo);
    coseno = cos(angulo);
    tangente = tan(angulo);
  }

  float getYFromLargo(float largo) {
    return pPosY + seno * largo;
  }

  float getXFromLargo(float largo) {
    return pPosX + coseno * largo;
  }

  float getYFromX(float x) {
    x -= pPosX;
    return pPosY + tangente * x;
  }

  float getXFromY(float y) {
    if (tangente == 0) {
      return 255;
    }
    y -= pPosY;
    return pPosX + y / tangente;
  }

  float getLargoFromY(float y) {
    if (seno == 0) {
      return 255;
    }
    y -= pPosY;
    return y / seno;
  }

  float getLargoFromX(float x) {
    if (coseno == 0) {
      return 255;
    }
    x -= pPosX;
    return x / coseno;
  }
};

Player jugador;
int imin = 512;
int animPistola = 4;
int anim = 1;

boolean hayPared(int x, int y) {
  if (x > 7 || y > 7 || x < 0 || y < 0) {
    return true;
  }
  //byte fila = pgm_read_byte(mapa+(7-y));
  byte fila = mapa[7-y];
  return  ((fila >> (7 - x)) & 1) == 1;
}

boolean getTextura(int textura, int x, int y){
  if (x > 15 || y > 15 || x < 0 || y < 0) {
    return false;
  }
  int fila = pgm_read_word_near(texturas+textura*16+y);
  //int fila = texturas[16*textura+y];
  return  ((fila >> (15-x)) & 1) == 1;
}

Pintable shotRayX(Rayo &rayo) {
  Pintable pintable;
  float largo = 0;
  float x = (int)rayo.pPosX;
  float y = 0;
  int i = 0;
  int direccion = rayo.dirX;
  int pared = (direccion < 0) ? -1 : 0;
  x -= pared;
  do {
    if (i > DISTANCIA) {
      pintable.largo = 255;
      return pintable;
    }
    x += direccion;
    y = rayo.getYFromX(x);
    i++;
  } while (!hayPared((int)x + pared, (int)y));
  pintable.setTx(y);
  pintable.largo = abs(rayo.getLargoFromX(x));
  return pintable;
}

Pintable shotRayY(Rayo &rayo) {
  Pintable pintable;
  float largo = 0;
  float x = 0;
  float y = (int)rayo.pPosY;
  int i = 0;
  int direccion = rayo.dirY;
  int pared = (direccion < 0) ? -1 : 0;
  y -= pared;
  do {
    if (i > DISTANCIA) {
      pintable.largo = 255;
      return pintable;
    }
    y += direccion;
    x = rayo.getXFromY(y);
    i++;
  } while (!hayPared((int)x, (int)y + pared));
  pintable.setTx(x);
  pintable.largo = abs(rayo.getLargoFromY(y));
  return pintable;
}

void pintarRayo(Pintable &pintable) {
  int mitad = HEIGHT / 2;
  byte ty = 0, tx = pintable.tx, x = pintable.x;
  byte tamanioPared = pintable.alto;
  int parteAlta = mitad - (int)tamanioPared;
  if (parteAlta < 0) {
    parteAlta = 0;
  }
  for (byte i = mitad, maxi = parteAlta, ti = 0; i > maxi; i--) {
    ty = ti*8/tamanioPared;
    if (getTextura(pintable.textura,tx, 7 - ty)) {
      switch(pintable.luz){
        case 5:
        case 4:
          u8g2.drawPixel(x+3, i);
        case 3:
          u8g2.drawPixel(x+2, i);
        case 2:
          u8g2.drawPixel(x+1, i);
        case 1:
          u8g2.drawPixel(x, i);
      }
    }
    if (getTextura(pintable.textura,tx, ty+8)) {    
      switch(pintable.luz){
        case 5:
        case 4:
          u8g2.drawPixel(x+3, HEIGHT - i);
        case 3:
          u8g2.drawPixel(x+2, HEIGHT - i);
        case 2:
          u8g2.drawPixel(x+1, HEIGHT - i);
        case 1:
          u8g2.drawPixel(x, HEIGHT - i);
      }
    }
    ti++;
  }
}

void pintarRayo2(Pintable &pintable) {
  int mitad = HEIGHT / 2;
  byte x = pintable.x;
  byte tamanioPared = pintable.alto;
  int parteAlta = mitad - (int)tamanioPared;
  if (parteAlta < 0) {
    parteAlta = 0;
  }
  for (byte i = parteAlta,maxi = HEIGHT-i; i < maxi; i++) {
      u8g2.drawPixel(x, i);
  }
}

Pintable getMenor(Pintable p1, Pintable p2){
  if(p1.largo < p2.largo){
    return p1;
  }else{
    return p2;
  }
}

void render2(){
  float tamanioRayo = (VISION)/(float)(WIDTH);
  float tamanioHRayo = (HEIGHT/2) / ((float)VISIONH);
  float angulo = jugador.angulo;
  byte antTx, actTx;
  Pintable pintable;
  Rayo rayo;
  rayo.pPosX = jugador.posX;
  rayo.pPosY = jugador.posY;
  rayo.setAngulo(angulo);
  pintable = getMenor(shotRayX(rayo),shotRayY(rayo));
  antTx = pintable.tx;
  for(byte i = 1; i<WIDTH; i++){
    angulo-=tamanioRayo;
    rayo.setAngulo(angulo);
    pintable = getMenor(shotRayX(rayo),shotRayY(rayo));
    if(pintable.largo<=DISTANCIA){
      actTx = pintable.tx;
      pintable.x = i*4;
      pintable.luz = 4-((int)(pintable.largo*4)/DISTANCIA);
      pintable.alto = (int)abs(tamanioHRayo * (atan(0.5 / pintable.largo) * RAD_TO_DEG));
      for(byte j = pintable.luz; j>0; j--){
        pintarRayo(pintable);
        pintable.x--;
        pintable.tx = map(j,0,4,antTx,actTx);
      }
      antTx = actTx;
    }
  }
}

void render() {
  float tamanioRayo = (VISION)/(float)(WIDTH);
  float tamanioHRayo = (HEIGHT/2) / ((float)VISIONH);
  float angulo = jugador.angulo;
  Pintable pintable;
  Rayo rayo;
  rayo.pPosX = jugador.posX;
  rayo.pPosY = jugador.posY;
  for (byte i = 0, maxi = WIDTH; i < maxi; i++) {
    rayo.setAngulo(angulo);
    pintable = getMenor(shotRayX(rayo),shotRayY(rayo));
    if(pintable.largo<=DISTANCIA){
      pintable.x = i*4;
      pintable.luz = 5-((int)(pintable.largo*5)/DISTANCIA);
      pintable.alto = (int)abs(tamanioHRayo * (atan(0.5 / pintable.largo) * RAD_TO_DEG));
      pintarRayo(pintable);
    }
    angulo -= tamanioRayo;
  }
}

int readjoy(byte pin, int maximo) {
  int input = analogRead(pin);
  if (input > 470 && input < 520) {
    input = 512;
  }
  return map(input, 0, 1023, -maximo, maximo);
}

void mover() {
  int mx = readjoy(PINX, VELOCIDADGIRO);
  int my = readjoy(PINY, VELOCIDAD);
  byte mz = !digitalRead(PINZ);
  boolean actualizado = true;

  if (mz != 0) {

  }
  
  if (mx != 0) {
    actualizado = false;
    float angulo = jugador.angulo - mx;
    if (angulo >= 360) {
      angulo -= 360;
    } else if (angulo < 0) {
      angulo += 360;
    }
    jugador.angulo = angulo;
  }

  if (my != 0) {
    actualizado = false;
    Rayo rayo;
    rayo.pPosX = jugador.posX;
    rayo.pPosY = jugador.posY;
    rayo.setAngulo(jugador.angulo - (VISION / 2));
    jugador.posX = rayo.getXFromLargo(my * 0.05);
    jugador.posY = rayo.getYFromLargo(my * 0.05);
    if (hayPared((int)jugador.posX, (int)jugador.posY)) {
      jugador.posX = rayo.pPosX;
      jugador.posY = rayo.pPosY;
    }else{ 
      if(animPistola > 4){
        anim = -2;
      }else if(animPistola<0){
        anim = 2;
      }
      animPistola+=anim;
    }
  }

  if (!actualizado||true) {
    u8g2.clearBuffer();
    u8g2.setDrawColor(1);
    render();
    u8g2.setBitmapMode(2);
    u8g2.setDrawColor(1);
    u8g2.drawXBMP(80+animPistola,30,rifle_width,rifle_height,rifle_bits);
    u8g2.setDrawColor(0);
    u8g2.drawXBMP(80+animPistola,30,rifle2_width,rifle2_height,rifle2_bits);
    u8g2.setDrawColor(0);
    u8g2.drawXBMP(0,HEIGHT-16,pugface_width,pugface_height,pugface_bits);
    u8g2.setDrawColor(1);
    u8g2.drawXBMP(0,HEIGHT-16,pugface2_width,pugface2_height,pugface2_bits);
    u8g2.drawCircle(WIDTH/2,HEIGHT/2,4,U8G2_DRAW_ALL);
    u8g2.setDrawColor(0);
    u8g2.drawCircle(WIDTH/2,HEIGHT/2,6,U8G2_DRAW_ALL);
    u8g2.setDrawColor(1);
    u8g2.setCursor(0, 63);
    u8g2.print("X/Y: ");
    u8g2.print(jugador.posX);
    u8g2.print("/");
    u8g2.print(jugador.posY);
    u8g2.sendBuffer();
  }
}

void setup() {
  u8g2.setBusClock(800000);
  u8g2.begin();
  u8g2.setFont(u8g2_font_5x7_tr);
  pinMode(PINX, INPUT);
  pinMode(PINY, INPUT);
  pinMode(PINZ, INPUT);
}

void loop() {
  while(true){
    mover();
  }
}

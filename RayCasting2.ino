#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#define PINY 14
#define PINX 15
#define PINZ 16

#define VISION 70
#define VISIONH 70.0/4
#define DISTANCIA 5
#define HEIGHT 56
#define WIDTH 32
#define VELOCIDAD 8
#define VELOCIDADGIRO 25

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
int piedras[] = {
  0b0000100000010000,
  0b0001100000010000,
  0b0011100001111000,
  0b0010000010001000,
  0b0010000110001100,
  0b1111111110001111,
  0b1000100011111001,
  0b0001100001100000,
  0b0011100011000000,
  0b0110100010000000,
  0b1100110111100001,
  0b1000111100110111,
  0b1000111000011101,
  0b1100110000010000,
  0b0111100000110000,
  0b0100000000110000
};

int mapa2[] {0, 0, 0, 0, 0, 0, 0, 0};

byte mapa3[] = {
  0b11111111,
  0b10000001,
  0b11000111,
  0b10000111,
  0b10000001,
  0b11111001,
  0b10000001,
  0b11111111
};

byte mapa[] = {
  0b00000000,
  0b01010100,
  0b00000000,
  0b01010100,
  0b00000000,
  0b00111100,
  0b00000000,
  0b00000000
};

struct Player {
  float posX = 4;
  float posY = 4;
  int angulo = 180;
};
struct Pintable{
  byte tx = 0;
  byte x = 0;
  byte pared = 0;
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

boolean hayPared(int x, int y) {
  if (x > 7 || y > 7 || x < 0 || y < 0) {
    return true;
  }
  return  ((mapa[7 - y] >> (7 - x)) & 1) == 1;
}

boolean getPiedra(int x, int y) {
  if (x > 15 || y > 15 || x < 0 || y < 0) {
    return false;
  }
  return  ((piedras[y] >> (15 - x)) & 1) == 0;
}

int readjoy(byte pin, int maximo) {
  int input = analogRead(pin);
  if (input > 490 && input < 520) {
    input = 512;
  }
  return map(input, 0, 1023, -maximo, maximo);
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
  pintable.pared = x;
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
  pintable.pared = y;
  return pintable;
}

void pintarRayo(Pintable &pintable) {
  if(pintable.largo>DISTANCIA){
    return;
  }
  float largo = (pintable.largo == 0) ? 0.1f : pintable.largo;  
  int luz = 6-((byte)(6*largo))/DISTANCIA;
  int mitad = HEIGHT / 2;
  byte ty = 0, tx = pintable.tx, x = pintable.x;
  float tamanioRayo = (mitad) / (VISIONH);
  byte tamanioPared = (int)abs(tamanioRayo * (atan(0.5 / largo) * RAD_TO_DEG));
  int parteAlta = mitad - (int)tamanioPared;
  if (parteAlta < 0) {
    parteAlta = 0;
  }
  for (byte i = mitad, maxi = parteAlta, ti = 0; i > maxi; i--) {
    ty = ti*8/tamanioPared;
    if (i%2<luz && getPiedra(tx, 7 - ty) || i == parteAlta + 1) {
        for(int li = 0;li<luz&&li<4;li++){
          u8g2.drawPixel(x*4+li, i);  
        }
    }
    if (i%2<luz && getPiedra(tx, ty+8)) {
      for(int li = 0;li<luz&&li<4;li++){
        u8g2.drawPixel(x*4+li, HEIGHT - i);
      }
    }
    ti++;
  }
}

Pintable getMenor(Pintable p1, Pintable p2){
  if(p1.largo < p2.largo){
    return p1;
  }else{
    return p2;
  }
}

void render() {
  float tamanioRayo = VISION/(float)WIDTH;
  float angulo = jugador.angulo;
  Pintable pintable;
  Rayo rayo;
  rayo.pPosX = jugador.posX;
  rayo.pPosY = jugador.posY;
  for (byte i = 0; i < WIDTH; i++) {
    rayo.setAngulo(angulo);
    pintable = getMenor(shotRayX(rayo),shotRayY(rayo));
    pintable.x = i;
    pintarRayo(pintable);
    angulo -= tamanioRayo;
  }
}

void mover() {
  int mx = readjoy(PINX, VELOCIDADGIRO);
  int my = readjoy(PINY, VELOCIDAD);
  byte mz = !digitalRead(PINZ);
  boolean actualizado = true;

  if (mx != 0) {
    actualizado = false;
    float angulo = jugador.angulo - mx;
    if (angulo >= 360) {
      angulo -= 360;
    } else if (angulo < 0) {
      angulo += 360;
    }
    jugador.angulo = angulo;
    u8g2.setCursor(0, 63);
    u8g2.print(mx);
    u8g2.print(" - ");
    u8g2.print(analogRead(PINX));
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
    }
  }

  if (mz != 0) {
    actualizado = false;
  }

  if (!actualizado) {

  }
}

void setup() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_5x7_tr);
  pinMode(PINX, INPUT);
  pinMode(PINY, INPUT);
  pinMode(PINZ, INPUT);
}

void loop() {
  while(true){
    u8g2.clearBuffer();
    mover();
    render();
    u8g2.sendBuffer();
  }
}

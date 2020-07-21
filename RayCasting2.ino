#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#define PINY 14
#define PINX 15
#define PINZ 16

#define VISION 70.0
#define VISIONH 70.0/4
#define DISTANCIA 4
#define HEIGHT 56
#define WIDTH 32
#define VELOCIDAD 5
#define VELOCIDADGIRO 20

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
const int piedras[] PROGMEM = {
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
const int mapa[] PROGMEM = {
  0b11111111,
  0b10000001,
  0b11000111,
  0b10000111,
  0b10000001,
  0b11111001,
  0b10000001,
  0b11111111
};
struct Player {
  float posX = 4;
  float posY = 4;
  float angulo = 180;
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
    angulo = angulo * RAD_TO_DEG;
    seno = sin(angulo);
    coseno = cos(angulo);
    tangente = tan(angulo);
  }

  float getYFromLargo(float largo) {
    return pposy + seno * largo;
  }

  float getXFromLargo(float largo) {
    return pposx + coseno * largo;
  }

  float getYFromX(float x) {
    x -= pposx;
    return pposy + tangente * x;
  }

  float getXFromY(float y) {
    if (tangente == 0) {
      return 255;
    }
    y -= pposy;
    return pposx + y / tangente;
  }

  float getLargoFromY(float y) {
    if (seno == 0) {
      return 255;
    }
    y -= pposy;
    return y / seno;
  }

  float getLargoFromX(float x) {
    if (coseno == 0) {
      return 255;
    }
    x -= pposx;
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

int readjoy(byte pin, byte maximo) {
  return map(analogRead(pin), 0, 1000, -maximo, maximo);
}

float shoyRayX(rayo) {
  float largo = 0;
  float x = rayo.pPosX;
  float y = 0;
  int i = 0;
  int direccion = rayo.dirX;
  int pared = (direccion < 0) ? -1 : 0;
  x -= pared;
  do {
    if (i > DISTANCIA) {
      return 255;
    }
    x += direccion;
    y = rayo.getYFromX(x);
    i++;
  } while (!hayPared((int)x + pared, (int)y);
           largo = rayo.getLargoFromX(x);
           return abs(largo);
}

float shoyRayY(rayo) {
  float largo = 0;
  float x = 0;
  float y = rayo.pPosY;
  int i = 0;
  int direccion = rayo.dirY;
  int pared = (direccion < 0) ? -1 : 0;
  y -= pared;
  do {
    if (i > DISTANCIA) {
      return 255;
    }
    y += direccion;
    x = rayo.getXFromY(y);
    i++;
  } while (!hayPared((int)x, (int)y + pared);
           largo = rayo.getLargoFromY(y);
           return abs(largo);
}

void render() {
  float tamanioRayos = VISION / WIDTH;
  float angulo = jugador.angulo;
  float largo, largoX, largoY;
  int tx = 0;
  Rayo rayo;
  rayo.pPosX = jugador.posX;
  rayo.pPosY = jugador.posY;
  for (byte i = 0; i < WIDTH; i++) {
    rayo.setAngulo(angulo);
    largoX = shotRayX(angulo);
    largoY = shotRayY(angulo);
    if (largoX < largoY) {
      largo = largoX;
      tx = (int)(rayo.getYFromLargo(largo) * 100) % 100;
    } else {
      largo = largoY;
      tx = (int)(rayo.getXFromLargo(largo) * 100) % 100;
    }
    tx = 16 * tx / 100;
    if (largo <= distanciaDeVision) {
      largo = largo == 0 ? 0.1f : largo;
      pintarRayo(largo, i, tx);
    }
    anguloHorizontal -= tamañorayos;
  }
}

void mover() {
  byte mx = readjoy(PINX, VELOCIDADGIRO);
  byte my = readjoy(PINX, VELOCIDAD);
  byte mz = !digitalRead(PINZ);
  boolean actualizado = true;

  if (mx != 0) {
    actualizado = false;
  }

  if (my != 0) {
    actualizado = false;
  }

  if (mz != 0) {
    actualizado = false;
  }

  if (!actualizado) {

  }
}

void setup() {
  u8g2.begin();
  pinMode(PINX, INPUT);
  pinMode(PINY, INPUT);
  pinMode(PINZ, INPUT);
}

void loop() {
  u8g2.clearBuffer();
  u8g2.sendBuffer();

}

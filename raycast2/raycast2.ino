#define VISION 70
#define VISIONH 70.0/4
#define DISTANCIA 8
#define HEIGHT 56
#define WIDTH 128

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

struct Player {
  float posX = 1.5;
  float posY = 1.5;
  int angulo = 180;
};

struct Pintable{
  byte tx = 0;
  byte textura = 0;
  byte alto = 0;
  byte x = 0;
  
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

float tamanioHRayo = (HEIGHT/2) / (VISIONH/4);
Player jugador;
Pintable pintables[WIDTH];

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
      pintable.alto = 0;
      return pintable;
    }
    x += direccion;
    y = rayo.getYFromX(x);
    i++;
  } while (!hayPared((int)x + pared, (int)y));
  largo = abs(rayo.getLargoFromX(x));
  largo = largo==0?0.0001:largo;
  pintable.alto = (int)abs(tamanioHRayo * (atan(0.5 / largo) * RAD_TO_DEG));
  pintable.setTx(y);
  
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
      pintable.alto = 0;
      return pintable;
    }
    y += direccion;
    x = rayo.getXFromY(y);
    i++;
  } while (!hayPared((int)x, (int)y + pared));
  largo = abs(rayo.getLargoFromY(y));
  largo = largo==0?0.0001:largo;
  pintable.alto = (int)abs(tamanioHRayo * (atan(0.5 / largo) * RAD_TO_DEG));
  pintable.setTx(x);
  return pintable;
}

Pintable getMayor(Pintable p1, Pintable p2){
  if(p1.alto > p2.alto){
    return p1;
  }else{
    return p2;
  }
}

void cast(){
  digitalWrite(13, HIGH);
  float tamanioRayo = (VISION)/(float)(WIDTH);
  float angulo = jugador.angulo/2;
  Pintable pintable;
  Rayo rayo;
  rayo.pPosX = jugador.posX;
  rayo.pPosY = jugador.posY;
  for (byte i = 0, maxi = WIDTH; i < maxi; i++) {
    rayo.setAngulo(angulo);
    pintable = getMayor(shotRayX(rayo),shotRayY(rayo));
    pintable.x = i;
    pintables[i] = pintable;
    angulo -= tamanioRayo;
  }
  digitalWrite(13, LOW);
}

boolean hayPared(int x, int y) {
  if (x > 7 || y > 7 || x < 0 || y < 0) {
    return true;
  }
  //byte fila = pgm_read_byte(mapa+(7-y));
  byte fila = mapa[7-y];
  return  ((fila >> (7 - x)) & 1) == 1;
}

void enviarPintable(byte p){
  Pintable pintable = pintables[p];
  String send="";
  send+=pintable.tx;
  send+=" ";
  send+=pintable.textura;
  send+=" ";
  send+=pintable.alto;
  Serial.println(send);
}

void leerJugador(){
  while(Serial.available()<5){}
  jugador.posX = Serial.parseFloat();
  jugador.posY = Serial.parseFloat();
  jugador.angulo = Serial.parseInt();
}

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}


void loop() {
  while(Serial.available()<1){}
  char opc = Serial.read();
  byte x;
  switch(opc){
    case 'j':
      leerJugador();
      cast();
      break;
    case 'p':
      while(Serial.available()<2){}
      x=Serial.parseInt();
      enviarPintable(x);
      break;
    default:
    break;
  }
  
}

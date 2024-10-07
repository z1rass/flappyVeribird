#include <Wire.h>
#include "SSD1306Wire.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <cstdlib>
#include <ctime>
#include <iostream>


#define DHTPIN 14
#define DHTTYPE DHT11
#define FLASH_BUTTON_PIN 0

DHT dht(DHTPIN, DHTTYPE);
SSD1306Wire display(0x3c, 12, 14);



bool temp;

class Wall {
public:
  int height1;
  int height2;
  int x = 170;
  int sum = 49;
  int cof = 20;

  void generate(int rand) {
    this->height1 = rand;
    this->height2 = this->sum - (this->height1 - cof);
  }

  void wallStep(int bx, int by, int score) {
    display.clear();
    display.fillRect(this->x, 0, 10, height1);
    display.fillRect(this->x, height1 + cof, 10, height2);
    display.fillRect(by, bx, 5, 5);
    display.drawString(0,0, String(score));
    display.display();
    // Wall speed
    this->x -= 3;
  }
};



class Bird {
public:
  int x = 64 / 2;
  int y = 0;
  int score = 0;

  void birdMove() {
    this->x -= 5;
  }

  void physic(int wx, int height1, int height2, int score, int cof) {
    this->x += 2;
    if (wx < 7) {
      if (this->x+2 < height1 || this->x > height1 + cof-2) {
        this->lose(score);
      }
    }

    if (this->x > 64 || this->x < 2) {
      this->lose(score);
    }
  }


  void lose(int score) {
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.println("You Lose");
    delay(3000);
    display.setFont(ArialMT_Plain_16);
    display.println("Score: " + String(score));

    display.display();
    temp = true;
    this->x = 0;
    this->y = 64 / 2;  // Сброс объекта Bird, если нужно
    delay(10000);
  }

  int getX() {
    return this->x;
  }

  int getY() {
    return this->y;
  }
};

Bird b;
Wall w;


void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  Serial.println();
  Serial.println();
  display.init();
  display.flipScreenVertically();
  dht.begin();
  display.setContrast(255);
  pinMode(FLASH_BUTTON_PIN, INPUT_PULLUP);
}






void loop() {
  if(w.x < -10){
    b.score += 1;
    w.x = 170;
  }
  if (w.x == 170) {
    w.generate(random(5, 40));
  }
  if (digitalRead(FLASH_BUTTON_PIN) == LOW) {
    b.birdMove();
  }


  if (temp) {
    w.x = 170;
    temp = false;
  }
  w.wallStep(b.getX(), b.getY(), b.score);
  delay(100);
  b.physic(w.x, w.height1, w.height2, b.score, w.cof);
}

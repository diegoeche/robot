#include "gear.hpp"
#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>

Gear::Gear() {
  radius = 5;
}

Gear::Gear(const int p_radius) {
  radius = p_radius;
  x = 0;
  y = 0;
}

Gear::Gear(const int p_radius, int p_x, int p_y) {
  radius = p_radius;
  x = p_x;
  y = p_y;
}

const float PI = 3.141592;

const int DENT_SIZE = 10;
sf::Color GEAR_COLOR(100, 50, 200, 255);

void Gear::setAngle(int p_angle) {
  angle = p_angle;
}

void Gear::render(sf::RenderWindow &window) {
  sf::CircleShape circle(radius * 0.8);
  circle.setFillColor(GEAR_COLOR);
  circle.setPosition(x + radius * 0.2, y + radius * 0.2);
  window.draw(circle);

  int dents = (((2 * PI * radius) / (float) DENT_SIZE)) / 3;

  float step = 365 / dents;
  for (int i = 0; i < dents; i++) {
    float dentAngle = this->angle + i * step;
    int realDentSize = DENT_SIZE - 3;
    sf::CircleShape dent(realDentSize);
    if(i == 0) {
      dent.setFillColor(sf::Color(200,200,200));
    } else {
      dent.setFillColor(GEAR_COLOR);
    }
    dent.setPosition(x - realDentSize + radius + radius * sin(dentAngle * (PI/180)), y - realDentSize + radius + radius * cos(dentAngle * (PI/180)));
    window.draw(dent);
  }
}

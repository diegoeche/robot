#pragma once

#include <SFML/Graphics.hpp>

class Gear {
  public:
  Gear();
  Gear(int radius);
  Gear(int radius, int x, int y);
  void setAngle(int angle);
  void render(sf::RenderWindow &window);
private:
  int angle;
  int radius;
  int x;
  int y;
};

#pragma once

#include <SFML/Graphics.hpp>

class GearBox {
  public:
  GearBox();
  GearBox(int radius_one, int radius_two);
  void run(float elapsed, float speed);
  void render(sf::RenderWindow &window);
private:
  Gear gearOne;
  Gear gearTwo;

  float smallRadius;
  float bigRadius;
  float x;
  float y;
  float smallAngle;
  float bigAngle;
};

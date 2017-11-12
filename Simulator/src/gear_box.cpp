#include "gear.hpp"
#include "gear_box.hpp"

#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>


GearBox::GearBox() {
}

GearBox::GearBox(int p_radiusOne, int p_radiusTwo) {
  smallRadius = p_radiusOne;
  bigRadius = p_radiusTwo;
  x = 20; // Position.
  y = 20;
  smallAngle = 365/8;
  bigAngle = 0;

  gearTwo = Gear(bigRadius, x, y);
  gearOne = Gear(smallRadius, x + bigRadius * 2, y + (bigRadius-smallRadius));
}

void GearBox::run(float elapsed, float speed) {
  smallAngle += elapsed * speed;
  gearOne.setAngle(smallAngle);
  bigAngle -= (elapsed * speed) * (smallRadius / bigRadius);
  gearTwo.setAngle(bigAngle);
}

void GearBox::render(sf::RenderWindow &window) {
  gearOne.render(window);
  gearTwo.render(window);
}

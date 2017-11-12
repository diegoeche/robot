#include <SFML/Graphics.hpp>
#include <GLUT/glut.h>
#include "SFML/OpenGL.hpp"
// #include <thread>
#include <sstream>

#include "imgui.h"
#include "imgui-SFML.h"

#include "stats.hpp"
#include "gear.hpp"

static bool ENABLE_DEBUGGING = false;

static bool RUNNING = false;
static int SPEED = 0;

void drawMenu() {
  ImGui::Begin("Simulator");
  ImGui::SameLine();

  ImVec4 green = ImVec4(0.0f,8.0f,0.1f,1.0f);
  ImVec4 red = ImVec4(1.0f,0.1f,0.0f,1.0f);

  if(!RUNNING) {
    ImGui::TextColored(red, "STOP");
  } else {
    ImGui::TextColored(green, "RUNNING");
  }
  ImGui::InputInt("Speed", &SPEED);
  ImGui::End();
}

void die(sf::RenderWindow &window) {
  ImGui::SFML::Shutdown();
  window.close();
  exit(0);
}

void handleEvents (sf::Event event) {
  switch (event.type) {
  case sf::Event::KeyPressed:
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
      RUNNING = true;
    }
    break;
  case sf::Event::KeyReleased:
    RUNNING = false;
    break;

    // we don't process other types of events
  default:
    break;
  }
}


int main() {
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
  window.setFramerateLimit(60);
  ImGui::SFML::Init(window);
  ImGui::GetIO().FontGlobalScale = 1.0f;
  sf::Clock clock;
  float smallRadius = 20;
  float bigRadius = 60;
  float x = 20;
  float y = 20;
  float smallAngle = 365/8;
  float bigAngle = 0;

  Gear big(bigRadius, x, y);
  Gear small(smallRadius, x + bigRadius * 2, y + (bigRadius-smallRadius));

  while (window.isOpen()) {
    auto elapsed = clock.restart();
    ImGui::SFML::Update(window, elapsed);
    smallAngle += elapsed.asSeconds() * SPEED;
    small.setAngle(smallAngle);
    bigAngle -= (elapsed.asSeconds() * SPEED) * (smallRadius / bigRadius);
    big.setAngle(bigAngle);

    sf::Event event;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);
      handleEvents(event);
      if (event.type == sf::Event::Closed) die(window);
    }

    window.clear(sf::Color::Black);

    // // sf::RectangleShape rectangle(sf::Vector2f(30, 30));
    // rectangle.setPosition(0,0)// ;
    // rectangle.setOutlineThickn// ess(20);
    // rectangle.setOutlineColor(// sf::Color(0, 0, 0));
    // rectangle.setFillColor(sf:// :Color(100, 100, 100, 255));
    // window.draw(rectangle);

    big.render(window);
    small.render(window);

    drawMenu();
    drawFrameDrawingStats(elapsed.asSeconds());
    ImGui::Render();
    window.display();
  }
  ImGui::SFML::Shutdown();
  return 0;
}

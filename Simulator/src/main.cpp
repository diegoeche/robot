#include <SFML/Graphics.hpp>
#include <GLUT/glut.h>
#include "SFML/OpenGL.hpp"
// #include <thread>
#include <sstream>

#include "imgui.h"
#include "imgui-SFML.h"

#include "stats.hpp"
#include "gear.hpp"
#include "gear_box.hpp"

static bool ENABLE_DEBUGGING = false;

static bool RUNNING = false;
static int SPEED = 0;

void drawMenu() {
  ImGui::Begin("Simulator");
  ImGui::SameLine();

  ImVec4 green = ImVec4(0.0f,8.0f,0.1f,1.0f);
  ImVec4 red = ImVec4(1.0f,0.1f,0.0f,1.0f);
  RUNNING = SPEED != 0;
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
    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
    //   RUNNING = true;
    // }
    break;
  case sf::Event::KeyReleased:
    // RUNNING = false;
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
  GearBox gb(20, 60);

  while (window.isOpen()) {
    auto elapsed = clock.restart();
    ImGui::SFML::Update(window, elapsed);
    gb.run(elapsed.asSeconds(), SPEED);

    sf::Event event;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);
      handleEvents(event);
      if (event.type == sf::Event::Closed) die(window);
    }

    window.clear(sf::Color::Black);

    gb.render(window);

    drawMenu();
    drawFrameDrawingStats(elapsed.asSeconds());
    ImGui::Render();
    window.display();
  }
  ImGui::SFML::Shutdown();
  return 0;
}

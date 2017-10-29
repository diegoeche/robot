#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

static bool ENABLE_DEBUGGING = false;

void drawMenu() {
  ImGui::Begin("Tank Viewer");
  ImGui::Text("Example Test");
  ImGui::End();

  // ImGui::BeginMenu("My Tank");
  // ImGui::MenuItem("Debug ", NULL, &ENABLE_DEBUGGING);
  // ImGui::EndMenu();
  // ImGui::End();
}

int main() {
  // create the window
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
  window.setFramerateLimit(60);
  ImGui::SFML::Init(window);
  ImGui::GetIO().FontGlobalScale = 1.0f;
  sf::Clock clock;

  while (window.isOpen()) {
    auto elapsed = clock.restart();
    ImGui::SFML::Update(window, elapsed);

    sf::Event event;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);

      if (event.type == sf::Event::Closed) window.close();
    }

    window.clear(sf::Color::Black);

    sf::RectangleShape rectangle(sf::Vector2f(30, 30));

    rectangle.setPosition(0,0);
    rectangle.setOutlineThickness(20);
    rectangle.setOutlineColor(sf::Color(0, 0, 0));
    rectangle.setFillColor(sf::Color(100, 100, 100, 255));
    window.draw(rectangle);

    // window.draw(
    drawMenu();

    ImGui::Render();
    window.display();
  }
  ImGui::SFML::Shutdown();
  return 0;
}

#include <SFML/Graphics.hpp>
#include <GLUT/glut.h>
#include "SFML/OpenGL.hpp"
#include <thread>
#include <sstream>

#include "imgui.h"
#include "imgui-SFML.h"
#include "network.hpp"

static bool ENABLE_DEBUGGING = false;

static char UP[]    = "UP\n";
static char DOWN[]  = "DOWN\n";
static char LEFT[]  = "LEFT\n";
static char RIGHT[] = "RIGHT\n";
static char STOP[]  = "STOP\n";

static char *CURRENT_COMMAND = STOP;
static char *LAST_COMMAND = STOP;

void drawMenu() {
  ImGui::Begin("Tank Viewer");
  ImGui::Text("Current Command:");
  ImGui::SameLine();

  ImVec4 green = ImVec4(0.0f,8.0f,0.1f,1.0f);
  ImVec4 red = ImVec4(1.0f,0.1f,0.0f,1.0f);

  if(CURRENT_COMMAND == STOP) {
    ImGui::TextColored(red, CURRENT_COMMAND);
  } else {
    ImGui::TextColored(green, CURRENT_COMMAND);
  }
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
      CURRENT_COMMAND = LEFT;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
      CURRENT_COMMAND = RIGHT;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
      CURRENT_COMMAND = UP;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
      CURRENT_COMMAND = DOWN;
    }
    break;
  case sf::Event::KeyReleased:
    CURRENT_COMMAND = STOP;
    break;

    // we don't process other types of events
  default:
    break;
  }
}


#define TIMES_BUFFER_SIZE 100
static float TIMES_BUFFER[TIMES_BUFFER_SIZE];

void drawFrameDrawingStats(float elapsed) {
  std::ostringstream os;
  os << "T: " << elapsed;
  std::string str = os.str();
  char * cstr = new char [str.length()+1];
  std::strcpy (cstr, str.c_str());

  ImGui::Begin("Stats");

  for(int i = 0; i < TIMES_BUFFER_SIZE - 1; i++) {
    TIMES_BUFFER[i] = TIMES_BUFFER[i+1];
  }
  TIMES_BUFFER[TIMES_BUFFER_SIZE-1] = elapsed;

  // Frame Rate
  ImGui::PlotHistogram(cstr, TIMES_BUFFER, TIMES_BUFFER_SIZE, 0, NULL, 0, 0.1f);
  ImGui::End();
}


int main() {
  // create the window
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
  window.setFramerateLimit(60);
  ImGui::SFML::Init(window);
  ImGui::GetIO().FontGlobalScale = 1.0f;
  sf::Clock clock;
  networkInitialize();
  // StatsServer();

  sf::Clock lastMessage;

  while (window.isOpen()) {
    auto elapsed = clock.restart();
    ImGui::SFML::Update(window, elapsed);

    sf::Event event;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);
      handleEvents(event);
      if (event.type == sf::Event::Closed) die(window);
    }

    window.clear(sf::Color::Black);

    sf::RectangleShape rectangle(sf::Vector2f(30, 30));

    rectangle.setPosition(0,0);
    rectangle.setOutlineThickness(20);
    rectangle.setOutlineColor(sf::Color(0, 0, 0));
    rectangle.setFillColor(sf::Color(100, 100, 100, 255));
    window.draw(rectangle);

    sf::Time sinceLastMessage = lastMessage.getElapsedTime();
    if (LAST_COMMAND != CURRENT_COMMAND) {
      for(int i= 0; i < 4; i++) {
	sendCommand(CURRENT_COMMAND);
      }
      LAST_COMMAND = CURRENT_COMMAND;
      lastMessage.restart();
    }

    drawMenu();
    drawFrameDrawingStats(elapsed.asSeconds());
    ImGui::Render();
    window.display();
  }
  ImGui::SFML::Shutdown();
  return 0;
}
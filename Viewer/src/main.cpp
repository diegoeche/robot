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
static char NW[]  = "NW\n";
static char NE[]  = "NE\n";
static char RIGHT[] = "RIGHT\n";
static char STOP[]  = "STOP\n";

static char *CURRENT_COMMAND = STOP;
static char *LAST_COMMAND = STOP;

static float x;
static float y;

void drawMenu() {
  ImGui::Begin("Tank Viewer");
  float c_x = x;
  float c_y = y;
  ImGui::SliderFloat("Camera X", &x, -1.5, 1.5, "%.2f");
  ImGui::SliderFloat("Camera Y", &y, -1.5, 1.5, "%.2f");
  if(c_x != x || c_y != y) {
    std::ostringstream os;
    os << "SET_CAMMERA" << std::endl << x << " " << y << std::endl;
    std::string str = os.str();
    char * cstr = new char [str.length()+1];
    std::strcpy (cstr, str.c_str());
    sendCommand(cstr);
  }

  ImGui::Text("Current Command:");

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

bool anyKeyPressed() {
  return sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
}

void handleEvents (sf::Event event) {
  switch (event.type) {
  case sf::Event::KeyPressed:
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) CURRENT_COMMAND = NW;
      else CURRENT_COMMAND = LEFT;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) CURRENT_COMMAND = NE;
      else CURRENT_COMMAND = RIGHT;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
      CURRENT_COMMAND = UP;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
      CURRENT_COMMAND = DOWN;
    }
    break;
  case sf::Event::KeyReleased:
    if (!anyKeyPressed()) CURRENT_COMMAND = STOP;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) CURRENT_COMMAND = UP;
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


int main(int args, char **parameters) {
  // create the window
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
  window.setFramerateLimit(60);
  ImGui::SFML::Init(window);
  ImGui::GetIO().FontGlobalScale = 1.0f;
  sf::Clock clock;
  if(args > 1) {
    printf("IP: %s\n", parameters[1]);
    networkInitialize(parameters[1]);
  } else {
    networkInitialize("192.168.1.3");
  }

  // statsserver();

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

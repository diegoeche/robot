#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
// #include <SFML/OpenGL.hpp>
#include <GLUT/glut.h>

#include <thread>
#include <string>
#include <sstream>
#include <iostream>

#include "imgui.h"
#include "imgui-SFML.h"
#include "network.hpp"

// static bool ENABLE_DEBUGGING = false;

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

//////////////// Histogram Buffers ////////////////////

#define BUFFER_SIZE 300
#define TIMES_BUFFER_SIZE BUFFER_SIZE
static float TIMES_BUFFER[TIMES_BUFFER_SIZE];

#define TEMP_BUFFER_SIZE BUFFER_SIZE
static float TEMP_BUFFER[TEMP_BUFFER_SIZE];

#define MAG_BUFFER_SIZE BUFFER_SIZE
static float MAG_BUFFER[3][MAG_BUFFER_SIZE];

#define GYRO_BUFFER_SIZE BUFFER_SIZE
static float GYRO_BUFFER[3][GYRO_BUFFER_SIZE];

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

  sf::Vector2f size(250, 80);

  ImGui::PlotHistogram("Temperature", TEMP_BUFFER, TEMP_BUFFER_SIZE, 0, NULL, 0, 100.0f, size);

  ImGui::Columns(2);
  ImGui::PlotHistogram("MagX",  MAG_BUFFER[0],  MAG_BUFFER_SIZE,  0, NULL, -1000, 1000.0f, size);
  ImGui::PlotHistogram("MagY",  MAG_BUFFER[1],  MAG_BUFFER_SIZE,  0, NULL, -4000, 500.0f, size);
  ImGui::PlotHistogram("MagZ",  MAG_BUFFER[2],  MAG_BUFFER_SIZE,  0, NULL, 0, 360.0f, size);

  ImGui::NextColumn();

  ImGui::PlotHistogram("GyroX", GYRO_BUFFER[0], GYRO_BUFFER_SIZE, 0, NULL, -360, 360.0f, size);
  ImGui::PlotHistogram("GyroY", GYRO_BUFFER[1], GYRO_BUFFER_SIZE, 0, NULL, -360, 360.0f, size);
  ImGui::PlotHistogram("GyroZ", GYRO_BUFFER[2], GYRO_BUFFER_SIZE, 0, NULL, -360, 360.0f, size);

  ImGui::End();
}

void drawRectangle(sf::RenderWindow &window) {
  sf::RectangleShape rectangle(sf::Vector2f(30, 30));
  rectangle.setPosition(0,0);
  rectangle.setOutlineThickness(20);
  rectangle.setOutlineColor(sf::Color(0, 0, 0));
  rectangle.setFillColor(sf::Color(100, 100, 100, 255));
  window.draw(rectangle);
}

void processEvents(sf::RenderWindow &window) {
  sf::Event event;
  while (window.pollEvent(event)) {
    ImGui::SFML::ProcessEvent(event);
    handleEvents(event);
    if (event.type == sf::Event::Closed) die(window);
  }

  // Send network commands:
  if (LAST_COMMAND != CURRENT_COMMAND) {
    for(int i= 0; i < 4; i++) {
      sendCommand(CURRENT_COMMAND);
    }
    LAST_COMMAND = CURRENT_COMMAND;
  }
}

void initWindow(sf::RenderWindow &window) {
  window.setFramerateLimit(60);
  ImGui::SFML::Init(window);
  ImGui::GetIO().FontGlobalScale = 1.0f;
}

void drawUI(sf::RenderWindow &window, sf::Time elapsed) {
  drawRectangle(window);
  drawMenu();
  drawFrameDrawingStats(elapsed.asSeconds());
}

void processNetwork(sf::UdpSocket &socket) {
  char data[1024];
  std::size_t received;
  sf::IpAddress sender;
  unsigned short port;

  auto status = socket.receive(data, 1024, received, sender, port);
  if (status != sf::Socket::Done) {
    if(status == sf::Socket::NotReady) {
      // This is Ok
    } else {
      printf("Error receiving data\n");
    }
  } else {
    std::stringstream ss;
    ss << data;

    // TEMP: 43.6 MAG:  655.7 -3970.3    nan GYRO:    0.0    0.0   -0.0

    float temp;
    float mag[3];
    float gyro[3];

    std::string command;
    ss >> command;

    ss >> temp;
    printf("%f\n", temp);

    // MAG:
    ss >> command;
    ss >> mag[0] >> mag[1] >> mag[2];
    printf("%f, %f, %f\n", mag[0], mag[1], mag[2]);

    // GYRO:
    ss >> command;
    ss >> gyro[0] >> gyro[1] >> gyro[2];
    printf("%f, %f, %f\n", gyro[0], gyro[1], gyro[2]);

    for(int i = 0; i < TEMP_BUFFER_SIZE - 1; i++) {
      TEMP_BUFFER[i] = TEMP_BUFFER[i+1];
    }
    TEMP_BUFFER[TEMP_BUFFER_SIZE-1] = temp;


    for(int i = 0; i < MAG_BUFFER_SIZE - 1; i++) {
      MAG_BUFFER[0][i] = MAG_BUFFER[0][i+1];
    }
    MAG_BUFFER[0][MAG_BUFFER_SIZE-1] = mag[0];

    for(int i = 0; i < MAG_BUFFER_SIZE - 1; i++) {
      MAG_BUFFER[1][i] = MAG_BUFFER[1][i+1];
    }
    MAG_BUFFER[1][MAG_BUFFER_SIZE-1] = mag[1];

    for(int i = 0; i < MAG_BUFFER_SIZE - 1; i++) {
      MAG_BUFFER[2][i] = MAG_BUFFER[2][i+1];
    }
    MAG_BUFFER[2][MAG_BUFFER_SIZE-1] = mag[2];

    for(int i = 0; i < GYRO_BUFFER_SIZE - 1; i++) {
      GYRO_BUFFER[0][i] = GYRO_BUFFER[0][i+1];
    }
    GYRO_BUFFER[0][GYRO_BUFFER_SIZE-1] = gyro[0];

    for(int i = 0; i < GYRO_BUFFER_SIZE - 1; i++) {
      GYRO_BUFFER[1][i] = GYRO_BUFFER[1][i+1];
    }
    GYRO_BUFFER[1][GYRO_BUFFER_SIZE-1] = gyro[1];

    for(int i = 0; i < GYRO_BUFFER_SIZE - 1; i++) {
      GYRO_BUFFER[2][i] = GYRO_BUFFER[2][i+1];
    }
    GYRO_BUFFER[2][GYRO_BUFFER_SIZE-1] = gyro[2];
    // printf("%s", data);
  }
}

int main(int args, char **parameters) {
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
  initWindow(window);

  sf::Clock clock;

  if(args > 1) {
    printf("IP: %s\n", parameters[1]);
    networkInitialize(parameters[1]);
  } else {
    networkInitialize("192.168.1.3");
  }

  sf::UdpSocket socket;
  socket.setBlocking(false);

  if (socket.bind(8090) != sf::Socket::Done) {
    printf("Error binding it to port 8090\n");
  }

  while (window.isOpen()) {
    auto elapsed = clock.restart();
    ImGui::SFML::Update(window, elapsed);

    processEvents(window);
    processNetwork(socket);

    window.clear(sf::Color::Black);

    drawUI(window, elapsed);

    ImGui::Render();
    window.display();
  }

  ImGui::SFML::Shutdown();
  return 0;
}

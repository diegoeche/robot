#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <GLUT/glut.h>
#include "SFML/OpenGL.hpp"
#include <thread>
#include <string>

#include <sstream>
#include <iostream>

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

#define TCP_SIZE (1024 * 2)

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
  int lastValidPos = 0;

  // sf::TcpSocket socket;
  // // socket.setBlocking(false);
  // char data[TCP_SIZE] =
  //   "GET /?action=stream HTTP/1.1\n"
  //   "Host: 192.168.1.6:8090\n"
  //   "Connection: keep-alive\n"
  //   "Cache-Control: max-age=0\n"
  //   "User-Agent: Dr Faldin Robotin\n"
  //   "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\n"
  //   "Accept-Encoding: gzip, deflate\n"
  //   "\n";

  // sf::Socket::Status connect = socket.connect("192.168.1.6", 8090);

  // if(connect == sf::Socket::Error) {
  //   std::cout << "Error Connecting" << std::endl;
  // }

  // // TCP socket:
  // sf::Socket::Status request = socket.send(data, TCP_SIZE);
  // std::cout << data << std::endl;
  // if (request != sf::Socket::Done){
  //   if(request == sf::Socket::Error) {
  //     std::cout << "Error Connecting" << std::endl;
  //   }
  // }

  // socket.setBlocking(false);
  std::stringstream readStream;

  int frames = 0;

  while (window.isOpen()) {
    frames++;
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

    // char readData[TCP_SIZE];
    // std::size_t received;

    // TCP socket:
    // sf::Socket::Status status = socket.receive(readData, TCP_SIZE, received);
    // if (status == sf::Socket::Error){
    //   std::cout << "Error:" << status << std::endl;
    // } else if (status == sf::Socket::Disconnected) {
    //   std::cout << "Disconnected" << std::endl;
    // } else if (status == sf::Socket::Done) {

    //   if(readStream.tellp()) {
    // 	readStream.clear();
    // 	readStream.seekg(readStream.beg);
    // 	// std::cout << "Reseting to:" << readStream.tellp() << std::endl;
    //   }

    //   socket.receive(readData, TCP_SIZE, received);
    //   // std::stringstream partialRead;
    //   // partialRead << readData;
    //   readStream << readData;


    //   std::string firstLine;
    //   std::string type ("--boundarydonotcross\r");
    //   // std::string type ("Content-Type: image/jpeg\r");
    //   int i = 0;
    //   int images = 0;

    //   while(std::getline(readStream, firstLine)) {
    // 	// std::cout << firstLine << std::endl;
    // 	if(firstLine.compare(type) == 0) {
    // 	  images ++;
    // 	  // std::cout << firstLine << std::endl;
    // 	  // std::cout << readStream.tellp() << std::endl;
    // 	}
    // 	i++;
    //   }
    //   if((frames % 60) == 0) {
    // 	std::cout << "Images:" << images << std::endl;
    // 	std::cout << "Lines read: " << i << std::endl;
    //   }
    //   // std::getline(readStream, firstLine);
    //   // std::cout << firstLine << std::endl;
    //   // if(firstLine == "--boundarydonotcross") {
    //   // 	std::cout << "Starting Data:" << std::endl;
    //   // 	std::cout << firstLine << std::endl;
    //   // }
    // }

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

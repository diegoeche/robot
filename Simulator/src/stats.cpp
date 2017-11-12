#pragma once

#include "imgui.h"
#include "imgui-SFML.h"
#include <sstream>

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

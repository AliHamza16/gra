#pragma once
#include <vector>
#include <cstdint>
#include "constants.h"

struct Rule {
  size_t d;
  std::vector<uint8_t> state;
  std::vector<uint8_t> division;

  Rule(size_t d, int rule);
};

struct Graph {
  size_t d;
  std::vector<int> edges;
  std::vector<uint8_t> state;
  std::vector<uint8_t> division;

  Graph() = default;
  Graph(std::string_view path);
  Graph(size_t d, bool save = false, std::string_view outputDir = Config::OUTPUT_DIR);
};

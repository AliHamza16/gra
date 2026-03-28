#pragma once
#include <vector>
#include <cstdint>

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
};

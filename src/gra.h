#pragma once
#include <vector>
#include <cstdint>
#include <filesystem>
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

  Graph() = default;
  Graph(const std::filesystem::path& path);
  Graph(size_t d);
};

void saveGraphToFile(const Graph& graph, const std::filesystem::path& path);
void exportToGraphviz(const Graph& graph, const std::filesystem::path& path);
void evolveGraph(Graph& graph, const Rule& rule);

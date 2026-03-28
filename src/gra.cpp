#include "gra.h"
#include <filesystem>
#include <format>
#include <fstream>
#include <print>

static void saveGraphToFile(const Graph& graph, const std::filesystem::path& path);
static Graph loadGraphFromFile(std::string_view path);
static Graph findMinimalGraph(size_t d);

Rule::Rule (size_t d, int rule) : d(d) {
  state.resize(2*(d+1));
  division.resize(2*(d+1));
  
  int offset = 0;

  for (size_t i = 0; i < 2*(d+1); ++i, ++offset)
    state[i] = (rule >> offset) & 1;

  for (size_t i = 0; i < 2*(d+1); ++i, ++offset)
    division[i] = (rule >> offset) & 1;
}

Graph::Graph(std::string_view path) {
  Graph g = loadGraphFromFile(path);
  d = g.d;
  state = g.state;
  division = g.division;
}

Graph::Graph(size_t d, bool save, std::string_view outputDir) : d(d) {
  Graph minimalGraph = findMinimalGraph(d);
  state = minimalGraph.state;
  division = minimalGraph.division;
  
  if (save) {
    std::filesystem::path path{outputDir};
    path /= std::format("minimal{}.graph", d);
    if (!std::filesystem::is_regular_file(path)) {
      saveGraphToFile(minimalGraph, path);
    }
  }
}

static void saveGraphToFile(const Graph& graph, const std::filesystem::path& path) {
  std::println("Saving graph to file: {}", path.string());
}

static Graph loadGraphFromFile(std::string_view path) {
  std::println("Loading graph from file: {}", path); 
  Graph g;
  return g;
}

static Graph findMinimalGraph(size_t d) {
  Graph g;
  return g; 
}

#include "gra.h"
#include <format>
#include <fstream>
#include <print>
#include <stdexcept>

namespace fs = std::filesystem;

static Graph loadGraphFromFile(const fs::path& path);
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

Graph::Graph(const fs::path& path) {
  *this = loadGraphFromFile(path);
}

Graph::Graph(size_t d) {
  *this = findMinimalGraph(d);
}

void saveGraphToFile(const Graph& graph, const fs::path& path) {
  std::println("Saving graph to file: {}", path.string());
  std::ofstream file{path};
  
  if (!file) {
    std::println("An error occured during opening the file: {}", path.string()); 
  } else {
    file << graph.d << " " << graph.state.size() << "\n";

    for (int e : graph.edges) file << e << " ";
    file << "\n";

    for (uint8_t s : graph.state) file << (int)s << " ";
    file << "\n";
  }
}

static Graph loadGraphFromFile(const fs::path& path) {
  std::println("Loading graph from file: {}", path.string()); 
  std::ifstream file{path};
  
  if (!file) {
    std::println("An error occured during opening the file: {}", path.string());  
    throw std::runtime_error("Error when opening file");
  } else {
    Graph g;
    size_t n;

    file >> g.d >> n;
    g.state.resize(n);
    g.edges.resize(g.d*n);

    for (int& e: g.edges) file >> e;
    for (uint8_t& s: g.state) { int val; file >> val; s = (uint8_t)val; }

    return g;
  }
}

static Graph findMinimalGraph(size_t d) {
  // TODO: implement search algorithm for minimal graphs
  throw new std::runtime_error("Not implemented");
  Graph g;
  return g; 
}

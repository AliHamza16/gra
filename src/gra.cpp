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

void evolveGraph(Graph& graph, const Rule& rule) {
  size_t n = graph.state.size();
  size_t d = graph.d;
  size_t initialSize = n;

  // There are 2(d+1) possible local configurations. I dont see any issue with
  // assuming this value is less than 256. So we can use uint8_t for configurations.
  // In that case, d must be less than 127, and this is a reasonable limit in practice.
  std::vector<uint8_t> config{graph.state};
  std::vector<uint8_t> division(n);

  for (uint8_t& c: config) c *= d+1;
  for (size_t i = 0; i < d*n; ++i) config[i/d] += graph.state[graph.edges[i]];

  for (size_t i = 0; i < n; ++i) graph.state[i] = rule.state[config[i]]; // Update state
  for (size_t i = 0; i < n; ++i) division[i] = rule.division[config[i]]; // Update division

  for (size_t i = 0; i < initialSize; ++i) {
    if (division[i]) {
      // IDs of new nodes are i, n, n+1, ..., n+d-2.
      for (size_t j = 1; j < d; ++j) graph.state.push_back(graph.state[i]); // Grow state
      for (size_t j = 1; j < d; ++j) division.push_back(0); // Grow division

      // Connect node n+j-1 with jth connection of old node i, and connect it to
      // nodes i, n, n+1, n+2, ..., n+d-2 except itself.
      for (size_t j = 1; j < d; ++j) {
        for (size_t k = 0; k < d; ++k) { 
          if (k == 0) graph.edges.push_back(i);
          else if (k == j) graph.edges.push_back(graph.edges[i*d + j]);
          else graph.edges.push_back(n-1 + k);
        }
      }
      // Keep the first connection old node i, and connect it to
      // nodes n, n+1, ..., n+d-2.
      for (size_t j = 1; j < d; ++j) graph.edges[i*d + j] = n-1 + j;

      n += d-1;
    }
  }
}

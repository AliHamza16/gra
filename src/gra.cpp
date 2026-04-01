#include "gra.h"
#include <format>
#include <fstream>
#include <iostream>
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
  std::cout << std::format("Saving graph to file: {}\n", path.string());
  std::ofstream file{path};
  
  if (!file) {
    std::cout << std::format("An error occured during opening the file: {}\n", path.string());
  } else {
    file << graph.d << " " << graph.state.size() << "\n";

    for (int e : graph.edges) file << e << " ";
    file << "\n";

    for (uint8_t s : graph.state) file << (int)s << " ";
    file << "\n";
  }
}

static Graph loadGraphFromFile(const fs::path& path) {
  std::cout << std::format("Loading graph from file: {}\n", path.string());
  std::ifstream file{path};
  
  if (!file) {
    std::cout << std::format("An error occured during opening the file: {}\n", path.string());
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
  // assuming this value is no greater than 256. So we can use uint8_t for configurations.
  // In that case, d must be less than or equal 127, and this is a reasonable limit in practice.
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

      graph.edges.resize((n+d-1)*d);
      
      // Update the connections of the nodes that we were previously connected to
      for (size_t j = 1; j < d; ++j) {
        int c = graph.edges[i*d + j];
        for (size_t k = 0; k < d; ++k) {
          if (graph.edges[c*d + k] == i) {
            graph.edges[c*d + k] = (n+j-1);
            graph.edges[(n+j-1)*d + 1] = c;
          }
        }
      }

      // Connect nodes n, n+1, ..., n+d-2 to node i
      for (size_t j = 1; j < d; ++j) {
        graph.edges[(n+j-1)*d + 0] = i;
      }

      // (n+j-1) goes from n to n+d-2
      // (n+j-1)*d + k goes from (n+j-1)*d + 2 to (n+j-1)*d + d-1
      // m = (k-2) or (k-2)+1
      // for n=0, m= _, 1, 2, ..., d-2
      // for n=1, m= 0, _, 2, ..., d-2
      // for n=2, m= 0, 1, _, ..., d-2
      for (size_t j = 1; j < d; ++j) {
        for (size_t k = 2; k < d; ++k) {
          graph.edges[(n+j-1)*d + k] = (k-2) < (j-1) ? (n + (k-2)) : (n + ((k-2)+1));
        }
      }

      // connect node i to nodes n, n+1, ..., n+d-2
      for (size_t j = 1; j < d; ++j) {
        graph.edges[i*d + j] = (n+j-1);
      }
      
      n += d-1;
    }
  }
}

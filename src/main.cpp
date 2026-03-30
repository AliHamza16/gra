#include <print>
#include <cmath>
#include <utility>
#include <format>
#include "constants.h"
#include "gra.h"

namespace fs = std::filesystem;

static void printUsage(std::string_view path) {
  std::print(
    "Usage: {} [options]\n\n"
    "Options:\n"
    "  -r, --rule <N>       Rule number [ 0, 16^(d+1) )                      (Required)\n"
    "  -i, --iterations <N> Number of iterations to evolve the graph         (Required)\n"
    "  -d, --degree <N>     Degree of the regular graph (d-regular)          (Required if no initial graph)\n"
    "  -g, --initial <file> Path to the initial graph file                   (Optional)\n"
    "  -o, --output <dir>   Output directory for generated .dot files        (default: data)\n"
    "  -h, --help           Show this help message\n\n"
    "Example:\n"
    "  {} -r 2236 -i 30 -d 3 -o data\n\n",
    path, path);
}

int main(int argc, char* argv[]) {

  std::pair<int, bool> ruleNumber{0, false};
  std::pair<int, bool> iterations{0, false};
  std::pair<size_t, bool> d{0, false};
  std::pair<fs::path, bool> outputDir{fs::path{Config::OUTPUT_DIR}, false};
  std::pair<fs::path, bool> initialGraphPath{fs::path{}, false};

  if (argc == 1) {
    printUsage(argv[0]);
    return 0;
  }

  for (int i = 1; i < argc; ++i) {
    std::string_view arg{argv[i]};
    if (arg == "--help") {
      printUsage(argv[0]);
      return 0;
    } else if ((arg == "--rule" || arg == "-r") && i+1 < argc) {
        ruleNumber.first = std::stoi(argv[++i]);
        ruleNumber.second = true;
    } else if ((arg == "--iterations" || arg == "-i") && i+1 < argc) {
        iterations.first = std::stoi(argv[++i]);
        iterations.second = true;
    } else if ((arg == "--degree" || arg == "-d") && i+1 < argc) {
        d.first = std::stoi(argv[++i]);
        d.second = true;
    } else if ((arg == "--initial" || arg == "-g") && i+1 < argc) {
        initialGraphPath.first = fs::path{argv[++i]};
        initialGraphPath.second = true; 
    } else if ((arg == "--output" || arg == "-o") && i+1 < argc) {
        outputDir.first = fs::path{argv[++i]};
        outputDir.second = true;
    } else {
      std::println("Unknown argument: {}", arg);
      std::print("\n");
      printUsage(argv[0]);
      return 1;
    }
  }

  if (!initialGraphPath.second && !d.second) {
    std::println("You must set the degree if no initial graph path provided\n");
    printUsage(argv[0]);
    return 1;
  }

  if (!ruleNumber.second) {
    std::println("You must set the rule number\n");
    printUsage(argv[0]);
    return 1;
  } 
  
  if (!iterations.second) {
    std::println("You must set the iterations\n");
    printUsage(argv[0]);
    return 1;
  }

  if (d.second && initialGraphPath.second) { 
    std::println("[INFO] Degree parameter will be overrided by loaded graph");
  }

  std::println("[INFO] Output files will be saved to the {}/", outputDir.first.string());

  Rule rule{d.first, ruleNumber.first};
  fs::create_directories(outputDir.first);

  Graph g;

  if (!initialGraphPath.second) {
    g = Graph{d.first}; // search for minimal graph
    fs::path filepath = outputDir.first / std::format("minimal{}.graph", g.d);
    saveGraphToFile(g, filepath);
  } else {
    g = Graph{initialGraphPath.first};
  }
  
  if (g.d >= 127) {
    std::println("Degree is too large to compute. Use a degree less than 127.");
    return 1; 
  } 
  
  if (ruleNumber.first < 0 || ruleNumber.first >= std::pow(16, g.d+1)) {
    std::println("Rule number must be between [0, {})", std::pow(16, g.d+1));
    return 1;
  }

  evolveGraph(g, rule);
  fs::path filepath = outputDir.first / "test.graph";
  saveGraphToFile(g, filepath);

  return 0;
}

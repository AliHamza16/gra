#include <print>
#include <filesystem>
#include <cmath>
#include "constants.h" 
#include "gra.h" 

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

  int ruleNumber;
  int iterations;
  size_t d;

  bool ruleNumberInitialized = false;
  bool iterationsInitialized = false;
  bool dInitialized = false;

  std::string outputDir{Config::OUTPUT_DIR};
  std::string initialGraphPath = "";

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
        ruleNumber = std::stoi(argv[++i]);
        ruleNumberInitialized = true;
    } else if ((arg == "--iterations" || arg == "-i") && i+1 < argc) {
        iterations = std::stoi(argv[++i]);
        iterationsInitialized = true;
    } else if ((arg == "--degree" || arg == "-d") && i+1 < argc) {
        d = std::stoi(argv[++i]);
        dInitialized = true;
    } else if ((arg == "--initial" || arg == "-g") && i+1 < argc) {
        initialGraphPath = argv[++i];
    } else if ((arg == "--output" || arg == "-o") && i+1 < argc) {
        outputDir = argv[++i];
    } else {
      std::println("Unknown argument: {}", arg);
      std::print("\n");
      printUsage(argv[0]);
      return 1;
    }
  }

  if (initialGraphPath == "" && !dInitialized) {
    std::println("You must set the degree if no initial graph path provided\n");
    printUsage(argv[0]);
    return 1;
  }

  if (!ruleNumberInitialized) {
    std::println("You must set the rule number\n");
    printUsage(argv[0]);
    return 1;
  }
  
  if (ruleNumber < 0 || ruleNumber >= std::pow(16, d+1)) {
    std::println("Rule number must be between [0, {})\n", std::pow(16, d+1));
    return 1;
  }
  
  if (!iterationsInitialized) {
    std::println("You must set the iterations\n");
    printUsage(argv[0]);
    return 1;
  }

  if (dInitialized && initialGraphPath != "") { 
    std::println("[INFO] Degree parameter will be overrided by loaded graph\n");
  }

  Rule rule{d, ruleNumber};
  std::filesystem::create_directories(outputDir);

  Graph g;

  if (initialGraphPath == "") {
    g = Graph{d, true /*save to file*/, outputDir}; // search for minimal regular graph
  } else {
    g = Graph{initialGraphPath};
  }

  return 0;
}

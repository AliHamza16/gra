# Graph-Rewriting Automata
This repository contains the unofficial C++ implementation of [GRA](https://paulcousin.net/graph-rewriting-automata/index.html)

## Compiling 
```
g++ -std=c++23 -o gra src/main.cpp
```

## Usage
```
./gra [RULE NUMBER] [ITERATIONS]
```

**RULE NUMBER** is an integer between [0-65536) that its binary form displays the behavior of the rule.
**ITERATIONS** determine how many times the graph will be evolved according to the rule.

More information about rules is available [here](https://paulcousin.net/graph-rewriting-automata/rules.html#rules).
Initial graph is a [minimal regular graph](https://paulcousin.net/graph-rewriting-automata/rules.html#minimal-graphs).


### Example
```
./gra 2236 30
```

### Visualization
__gra__ generates graph files in graphviz dot format for each step.
These graphs can be visualized using any layouting engine comes with graphviz.

```
neato -Tpng -O rule-2236/*.dot
feh -B white rule-2236/*.png
```

This command creates PNG files for each graph using neato and opens them in feh.

## References
- Cousin, P., & Maignan, A. (2022). Organic Structures Emerging from Bio-Inspired Graph-Rewriting Automata. 2022 24th International Symposium on Symbolic and Numeric Algorithms for Scientific Computing (SYNASC), 293–296. https://doi.org/10.1109/synasc57785.2022.00053

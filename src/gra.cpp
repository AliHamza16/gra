#include "gra.h"

Rule::Rule (size_t d, int rule) : d(d) {
  state.resize(2*(d+1));
  division.resize(2*(d+1));
  
  int offset = 0;

  for (size_t i = 0; i < 2*(d+1); ++i, ++offset)
    state[i] = (rule >> offset) & 1;

  for (size_t i = 0; i < 2*(d+1); ++i, ++offset)
    division[i] = (rule >> offset) & 1;
}

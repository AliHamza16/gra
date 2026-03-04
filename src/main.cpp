#include <iostream>
#include <vector>
#include <format>
#include <sstream>
#include <fstream>
#include <filesystem>

struct Vector {
  size_t size;
  std::vector<float> elements;

  float& operator [] (int i) {
     return elements[i];
  }

  const float& operator [] (int i) const {
     return elements[i];
  }

  Vector operator + (const Vector&) const;
  Vector mult(float) const;
};

struct Matrix {
  size_t rows;
  size_t cols;
  std::vector<float> elements;

  float& operator [] (int i, int j) {
     return elements[i*cols + j];
  }

  const float& operator [] (int i, int j) const {
     return elements[i*cols + j];
  }

  Vector mult(const Vector&) const;
};

void matrix_init(Matrix& m, size_t rows, size_t cols) {
    m.rows = rows;
    m.cols = cols;
    m.elements.clear();
    m.elements.resize(rows * cols, 0.0);
}

void matrix_init_sq(Matrix& m, size_t dim) {
  m.rows = dim;
  m.cols = dim;
  m.elements.clear();
  m.elements.resize(dim * dim, 0.0);
}

void matrix_init(Matrix& m, std::vector<std::vector<float>> init) {
  m.rows = init.size();
  m.cols = init[0].size();
  m.elements.resize(m.rows * m.cols);
  
  for (size_t i = 0; i < m.rows; ++i)
    for (size_t j = 0; j < m.cols; ++j)
      m[i, j] = init[i][j];
};


void matrix_dump(const Matrix& m) {
  for(size_t i = 0; i < m.rows; ++i) {
    for(size_t j = 0; j < m.cols; ++j) {
      std::cout << std::format("{:4.2f} ", m[i, j]);
    }
    std::cout << "\n";
  }
}

void vector_init(Vector& v, size_t size) {
  v.size = size;
  v.elements.resize(size, 0.0);
}

void vector_init(Vector& v, std::vector<float> init) {
  v.size = init.size();
  v.elements.resize(v.size);
  for (size_t i = 0; i < v.size; ++i) v[i] = init[i];
}

void vector_dump(const Vector& v) {
  for(size_t i = 0; i < v.size; ++i)
    std::cout << std::format("{:.2f}\n", v[i]);
}

// Matrix * Vector
Vector Matrix::mult (const Vector& u) const {
  Vector v;
  vector_init(v, this->rows);

  for (size_t i = 0; i < this->rows; ++i)
    for (size_t j = 0; j < this->cols; ++j)
      v[i] += (*this)[i, j] * u[j];
  
  return v;
}

// Vector * scalar
Vector Vector::mult(float s) const {
  Vector v;
  vector_init(v, this->size);

  for (size_t i = 0; i < v.size; ++i)
    v[i] = s * (*this)[i];

  return v;
}

// Vector + Vector
Vector Vector::operator+(const Vector& other) const {
  Vector v;
  vector_init(v, other.size);

  for (size_t i = 0; i < v.size; ++i)
    v[i] = (*this)[i] + other[i];

  return v;
}

// basic d-regular graph
void matrix_init_d_regular (Matrix& m, size_t d) {
  matrix_init_sq(m, d+1);
  for(size_t i = 0; i < m.rows; ++i) {
    for(size_t j = 0; j < m.cols; ++j) {
      if (i == j) { m[i, j] = 0.0; }
      else        { m[i, j] = 1.0; }
    }
  }
}

// d-regular rule
struct Rule {
  size_t d;
  std::vector<float> states;
  std::vector<float> divisions;
};

void rule_init(Rule& rule, size_t d, int binary) {
  rule.d = d;
  rule.states.resize(2 * (d + 1));
  rule.divisions.resize(2 * (d + 1));

  size_t index = 0;
  for (float& stt: rule.states) {
    bool is_set = (binary & (1 << index)); // it doesnt work if (binary & (1 << index)) not defined seperately. i dont know why.
                                           // TODO: check the implicit casting if there is a problem.
    if (is_set) { stt = 1.0; }
    else        { stt = 0.0; }
    ++index;
  }
  
  for (float& div: rule.divisions) {
    bool is_set = (binary & (1 << index));
    if (is_set) { div = 1.0; }
    else        { div = 0.0; }
    ++index;
  }
}

void update_state(Vector& state, const Vector& config, const Rule& rule) {
  vector_init(state, config.size);
  for (size_t i = 0; i < state.size; ++i)
      state[i] = rule.states[(size_t)config[i]];
}

void update_division(Vector& division, const Vector& config, const Rule& rule) {
  vector_init(division, config.size);
  for (size_t i = 0; i < division.size; ++i)
      division[i] = rule.divisions[(size_t)config[i]];
}

void grow_state_vector(Vector& state, size_t index, size_t d) {
  auto& s = state.elements;
  state.size += d - 1;

  for (size_t n = 1; n < d; ++n) {
    s.insert(s.begin() + index + n, s[index]);
  }
}

void grow_division_vector(Vector& division, size_t index, size_t d) {
  auto& v = division.elements;
  division.size += d - 1;

  v[index] = 0.0;
  for (size_t n = 1; n < d; ++n) {
    v.insert(v.begin() + index + n, 0.0);
  }
}

void grow_adjacency_matrix(Matrix& adj, size_t index, size_t d) {
  Matrix old = adj;
  matrix_init_sq(adj, old.rows + d-1);

  // replace divided cell with d-regular cluster
  for (size_t i = 0; i < d; ++i) 
    for (size_t j = 0; j < d; ++j)
      adj[i+index, j+index] = (i==j) ? 0.0 : 1.0;

  // place the old nodes and connections that not effected by division, that comes before the divided cell
  for (size_t i = 0; i < index; ++i) {
    for (size_t j = 0; j < index; ++j) {
      adj[i, j] = old[i, j];
    }
    for (size_t j = index+1; j < old.cols; ++j) {
      adj[i, j+d-1] = old[i, j];
    }
  }
   
  // place the old nodes and connections that not effected by division, that comes after the divided cell
  for (size_t i = index+1; i < old.rows; ++i) {
    for (size_t j = 0; j < index; ++j) {
      adj[i+d-1, j] = old[i, j];
    }
    for (size_t j = index+1; j < old.cols; ++j) {
      adj[i+d-1, j+d-1] = old[i, j];
    }
  }

  // connect the new d-regular cluster to the divided cells previous connections
  size_t counter = 0;
  for (size_t i = 0; i < old.rows; ++i) {
    if (old[i, index] > 0.0) {
      if (i < index)
        adj[i, index+counter] = 1.0;
      else 
        adj[i+d-1, index+counter] = 1.0;
      
      adj[index+counter, i] = adj[i, index+counter];
      adj[index+counter, i+d-1] = adj[i+d-1, index+counter];
      ++counter;
    }
  }
};

void handle_division (Matrix& adj, Vector& state, Vector& division, size_t d) {
  Vector division_new = division;
  
  size_t index = 0;
  for (float div: division.elements) {
    if (div > 0.0) {
      grow_state_vector(state, index, d);
      grow_division_vector(division_new, index, d);
      grow_adjacency_matrix(adj, index, d);
      index += d;
    } else { 
      ++index; 
    }
  }

  division = division_new;
}

void graphviz_dump(Matrix& adj, Vector& state, std::string file_path) {
  std::stringstream ss;

  ss << "graph G {\n";
  ss << "  bgcolor=\"transparent\"\n";
  ss << "  node [shape=circle, label=\"\", style=filled];\n";

  for (size_t i = 0; i < state.size; ++i)
    ss << std::format("  {} [fillcolor=\"{}\"]\n", i, state[i] > 0.0 ? "purple" : "orange");

  for (size_t i = 0; i < adj.rows; ++i)
    for (size_t j = 0; j < i; ++j)
      if (adj[i, j] > 0.0) ss << std::format("  {} -- {}\n", i, j);
    
  ss << "}\n";

  std::ofstream file(file_path);
  file << ss.str();
  file.close();

  std::cout << std::format("graphviz dot file saved to {}\n", file_path);
};

int main(int argc, char* argv[]) {
  size_t o = 4;     // order of the graph
  size_t d = o - 1; // d-regular value

  Matrix A; // adjacency matrix
  Vector S; // state vector
  Vector D; // division vector
  Vector C; // configuration vector
  Rule   R; // d-regular rule
 
  std::vector<std::vector<float>> init_matrix = {
      {0, 1, 0, 0, 0, 0, 0, 0, 1, 1},
      {1, 0, 1, 1, 0, 0, 0, 0, 0, 0},
      {0, 1, 0, 1, 1, 0, 0, 0, 0, 0},
      {0, 1, 1, 0, 0, 1, 0, 0, 0, 0},
      {0, 0, 1, 0, 0, 1, 1, 0, 0, 0},
      {0, 0, 0, 1, 1, 0, 0, 1, 0, 0},
      {0, 0, 0, 0, 1, 0, 0, 1, 1, 0},
      {0, 0, 0, 0, 0, 1, 1, 0, 0, 1},
      {1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
      {1, 0, 0, 0, 0, 0, 0, 1, 1, 0}
  };

  matrix_init(A, init_matrix);

  vector_init(S, {
      1,
      0,
      0,
      0,
      0,
      1,
      1,
      0,
      1,
      1
  });


  if (argc != 3) {
    std::cout << "USAGE\n" << "gra [RULE NUMBER] [ITERATIONS]\n\n";
    std::cout << "EXAMPLE\n" << "gra 2236 30\n";
    return -1;
  }

  unsigned int RULE_NUMBER = std::stoi(argv[1]);
  size_t ITERATIONS = std::stoi(argv[2]);
  
  if (RULE_NUMBER >= 65536) {
    std::cerr << "Rule number must be between [0-65536)\n";
    return -1;
  }
  
  rule_init(R, d, RULE_NUMBER);
  
  std::filesystem::create_directories(std::format("./rule-{:05}", RULE_NUMBER)); 
  graphviz_dump(A, S, std::format("rule-{:05}/{:04}.dot", RULE_NUMBER, 0));
  
  for (size_t t = 1; t <= ITERATIONS; ++t) {
    C = S.mult(d+1) + A.mult(S);
    update_state(S, C, R); 
    update_division(D, C, R);
    handle_division(A, S, D, d);
    graphviz_dump(A, S, std::format("rule-{:05}/{:04}.dot", RULE_NUMBER, t));
  }

  return 0;
}

#include "cyk_algo.h"
#include <iostream>

int main() {
  Grammar g;
  size_t n, s, p;
  std::cin >> n >> s >> p;
  std::string n_term;
  std::cin >> n_term;
  for (char sym : n_term) {
    g.AddNonTerminal(sym);
  }
  std::string term;
  std::cin >> term;
  for (char sym : term) {
    g.AddTerminal(sym);
  }
  for (size_t i = 0; i < p; ++i) {
    std::string rule;
    std::cin >> rule;
    g.AddRule(rule);
  }
  char start;
  std::cin >> start;
  g.SetStart(start);
  CYK_Algo algo;
  algo.fit(g);
  size_t m;
  std::cin >> m;
  for (size_t i = 0; i < m; ++i) {
    std::string word;
    std::cin >> word;
    if (algo.predict(word)) {
      std::cout << "Yes\n";
    } else {
      std::cout << "No\n";
    }
  }
  return 0;
}
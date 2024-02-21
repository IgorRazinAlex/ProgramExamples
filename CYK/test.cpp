#include "main.h"
#include <cassert>

void test_union() {
  NKA nka1('a');
  NKA nka2('b');
  nka1.unite(nka2);
  assert(nka1.size() == 5);
  assert(nka1.get_states()[0].finish == false);
  assert(nka1.get_states()[0].transitions['a'] == std::set<size_t>({1}));
  assert(nka1.get_states()[1].finish);
  assert(!nka1.get_states()[2].finish);
  assert(nka1.get_states()[2].transitions['b'] == std::set<size_t>({3}));
  assert(nka1.get_states()[3].finish);
  assert(!nka1.get_states()[4].finish);
  assert(nka1.get_states()[4].transitions['1'] == std::set<size_t>({0, 2}));
  assert(nka1.get_start() == 4);
  assert(nka1.get_ends() == std::set<size_t>({1, 3}));

  NKA nka_empty;
  nka1.unite(nka_empty);
  assert(nka1.size() == 5);
  assert(nka1.get_states()[0].finish == false);
  assert(nka1.get_states()[0].transitions['a'] == std::set<size_t>({1}));
  assert(nka1.get_states()[1].finish);
  assert(!nka1.get_states()[2].finish);
  assert(nka1.get_states()[2].transitions['b'] == std::set<size_t>({3}));
  assert(nka1.get_states()[3].finish);
  assert(!nka1.get_states()[4].finish);
  assert(nka1.get_states()[4].transitions['1'] == std::set<size_t>({0, 2}));
  assert(nka1.get_start() == 4);
  assert(nka1.get_ends() == std::set<size_t>({1, 3}));

  std::cout << "Union test passed\n";
}

void test_concatenation() {
  NKA nka1('a');
  NKA nka2('b');
  nka1.concatenate(nka2);
  assert(nka1.size() == 4);
  assert(nka1.get_states()[0].finish == false);
  assert(nka1.get_states()[0].transitions['a'] == std::set<size_t>({1}));
  assert(nka1.get_states()[1].finish == false);
  assert(nka1.get_states()[1].transitions['1'] == std::set<size_t>({2}));
  assert(nka1.get_states()[2].finish == false);
  assert(nka1.get_states()[2].transitions['b'] == std::set<size_t>({3}));
  assert(nka1.get_states()[3].finish == true);
  assert(nka1.get_start() == 0);
  assert(nka1.get_ends() == std::set<size_t>({3}));

  NKA nka_empty;
  nka1.concatenate(nka_empty);
  assert(nka1.size() == 4);
  assert(nka1.get_states()[0].finish == false);
  assert(nka1.get_states()[0].transitions['a'] == std::set<size_t>({1}));
  assert(nka1.get_states()[1].finish == false);
  assert(nka1.get_states()[1].transitions['1'] == std::set<size_t>({2}));
  assert(nka1.get_states()[2].finish == false);
  assert(nka1.get_states()[2].transitions['b'] == std::set<size_t>({3}));
  assert(nka1.get_states()[3].finish == true);
  assert(nka1.get_start() == 0);
  assert(nka1.get_ends() == std::set<size_t>({3}));

  std::cout << "Concatenation test passed\n";
}

void test_iteration() {
  NKA nka('a');
  nka.iterate();
  assert(nka.size() == 3);
  assert(nka.get_states()[0].finish == false);
  assert(nka.get_states()[0].transitions['a'] == std::set<size_t>({1}));
  assert(nka.get_states()[1].finish == false);
  assert(nka.get_states()[1].transitions['1'] == std::set<size_t>({2}));
  assert(nka.get_states()[2].finish == true);
  assert(nka.get_states()[2].transitions['1'] == std::set<size_t>({0}));
  assert(nka.get_start() == 2);
  assert(nka.get_ends() == std::set<size_t>({2}));

  std::cout << "Iteration test passed\n";
}

void test_normalization() {
  NKA nka('a');
  nka.iterate();
  nka.normalize();
  assert(nka.size() == 3);
  assert(nka.get_states()[0].finish == false);
  assert(nka.get_states()[1].finish == true);
  assert(nka.get_states()[1].transitions['a'] == std::set<size_t>({1}));
  assert(nka.get_states()[2].finish == true);
  assert(nka.get_states()[2].transitions['a'] == std::set<size_t>({1}));
  assert(nka.get_start() == 2);
  assert(nka.get_ends() == std::set<size_t>({1, 2}));
  
  std::cout << "Normalization test passed\n";
}

void test_sum_parser() {
  Parser parser;
  parser.set_sentence("a+");
  try {
    parser.parse();
    assert(false);
  } catch (std::runtime_error) {
    // ok
  }
  parser.set_sentence("ab+");
  try {
    parser.parse();
  } catch (std::runtime_error) {
    throw;
  }
  std::cout << "Parser sum test passed\n";
}

void test_concatenation_parser() {
  Parser parser;
  parser.set_sentence("a.");
  try {
    parser.parse();
    assert(false);
  } catch (std::runtime_error) {
    // ok
  }
  parser.set_sentence("ab.");
  try {
    parser.parse();
  } catch (std::runtime_error) {
    throw;
  }
  std::cout << "Parser concatenation test passed\n";
}

void test_iteration_parser() {
  Parser parser;
  parser.set_sentence("*");
  try {
    parser.parse();
    assert(false);
  } catch (std::runtime_error) {
    // ok
  }
  parser.set_sentence("a*");
  try {
    parser.parse();
  } catch (std::runtime_error) {
    throw;
  }
  parser.set_sentence("ab*");
  try {
    parser.parse();
    assert(false);
  } catch (std::runtime_error) {
    // ok
  }
  std::cout << "Parser iteration test passed\n";
}

void test_parser() {
  Parser parser;
  parser.set_sentence("d*");
  try {
    parser.parse();
    assert(false);
  } catch (std::runtime_error) {
    // ok
  }
  parser.set_sentence("ab&");
  try {
    parser.parse();
    assert(false);
  } catch (std::runtime_error) {
    // ok
  }
  parser.set_sentence("abc+");
  try {
    parser.parse();
    assert(false);
  } catch (std::runtime_error) {
    // ok
  }
  parser.set_sentence("ab+");
  try {
    parser.parse();
  } catch (std::runtime_error) {
    throw;
  }
}

void test_answer() {
  Parser parser;
  LargestEndingFinder finder;

  parser.set_sentence("a*");
  parser.parse();
  parser.normalize();
  finder.set_letter('d');
  try {
    finder.read_NKA(parser.get_NKA());
    assert(false);
  } catch (std::runtime_error) {
    // ok
  }
  finder.set_letter('a');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == -1);
  finder.set_letter('b');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 0);

  parser.set_sentence("ab+");
  parser.parse();
  parser.normalize();
  finder.set_letter('a');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 1);
  finder.set_letter('b');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 1);
  finder.set_letter('c');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 0);

  parser.set_sentence("b*a.");
  parser.parse();
  parser.normalize();
  finder.set_letter('a');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 1);
  finder.set_letter('b');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 0);
  finder.set_letter('c');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 0);

  parser.set_sentence("ab*.");
  parser.parse();
  parser.normalize();
  finder.set_letter('a');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 1);
  finder.set_letter('b');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == -1);
  finder.set_letter('c');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 0);

  parser.set_sentence("abc++");
  parser.parse();
  parser.normalize();
  finder.set_letter('a');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 1);
  finder.set_letter('b');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 1);
  finder.set_letter('c');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 1);

  parser.set_sentence("ab+c.aba.*.bac.+.+*");
  parser.parse();
  parser.normalize();
  finder.set_letter('b');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 1);

  parser.set_sentence("acb..bab.c.*.ab.ba.+.+*a.");
  parser.parse();
  parser.normalize();
  finder.set_letter('b');
  finder.read_NKA(parser.get_NKA());
  assert(finder.get_answer(parser.get_NKA()) == 0);

  std::cout << "Answer correctness test passed\n";
}

int main() {
  test_union();
  test_concatenation();
  test_iteration();
  test_normalization();
  test_sum_parser();
  test_concatenation_parser();
  test_iteration_parser();
  test_parser();
  test_answer();
  return 0;
}
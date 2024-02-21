#include "cyk_algo.h"
#include <cassert>

void TestAlphabet() {
  Grammar g1;
  try {
    g1.AddTerminal('A');
    throw;
  } catch (...) {
    //  ok
  }
  try {
    g1.AddNonTerminal('a');
    throw;
  } catch (...) {
    //  ok
  }
  Grammar g2;
  g2.AddTerminal('a');
  g2.AddNonTerminal('A');
}

void TestRuleAddition() {
  Grammar g1;
  g1.AddTerminal('a');
  g1.AddNonTerminal('A');
  g1.AddRule("A->a");
  try {
    g1.AddRule("Aab");
    throw;
  } catch (...) {
    //  ok;
  }
  try {
    g1.AddRule("aAa->A");
    throw;
  } catch (...) {
    //  ok
  }
  g1.AddRule("A->aaaa");
  try {
    g1.AddRule("a->aaaa");
    throw;
  } catch (...) {
    //  ok
  }
  try {
    g1.AddRule("A->aab");
    throw;
  } catch (...) {
    //  ok
  }
}

void TestCustomStart() {
  Grammar g1;
  try {
    g1.SetStart('C');
    throw;
  } catch (...) {
    //  ok
  }
  g1.AddNonTerminal('A');
  g1.SetStart('A');
}

void TestGrammarCreation() {
  TestAlphabet();
  TestRuleAddition();
  TestCustomStart();
}

void TestCYKAlgo() {
  Grammar g1;
  g1.AddNonTerminal('S');
  g1.AddTerminal('a');
  g1.AddTerminal('b');
  g1.AddRule("S->aSbS");
  g1.AddRule("S->");
  g1.SetStart('S');
  CYK_Algo algo1;
  algo1.fit(g1);
  assert(algo1.predict("aababb") == true);
  assert(algo1.predict("aabbba") == false);
  assert(algo1.predict("") == true);
  Grammar g2;
  g2.AddNonTerminal('S');
  g2.AddNonTerminal('A');
  g2.AddNonTerminal('B');
  g2.AddTerminal('a');
  g2.AddTerminal('b');
  g2.AddRule("S->ASB");
  g2.AddRule("A->a");
  g2.AddRule("B->b");
  g2.AddRule("S->");
  CYK_Algo algo2;
  algo2.fit(g2);
  assert(algo2.predict("aaabbb") == true);
  assert(algo2.predict("aaabb") == false);
  assert(algo2.predict("") == true);
}

int main() {
  TestGrammarCreation();
  TestCYKAlgo();
  return 0;
}
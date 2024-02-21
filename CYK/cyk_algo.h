#include <queue>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace Alphabet {
  std::string non_terminal = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  std::string terminal = "abcdefghijklmnopqrstuvwxyz1234567890()+-=*/%";

  void CheckTerminal(char sym) {
    if (terminal.find(sym) == std::string::npos) {
      throw("Terminal symbol " + std::string(1, sym) + " is not allowed");
    }
  }

  void CheckNonTerminal(char sym) {
    if (non_terminal.find(sym) == std::string::npos) {
      throw("Non terminal symbol " + std::to_string(sym) + " is not allowed");
    }
  }

  bool IsTerminal(char sym) {
    for (auto& term : terminal) {
      if (term == sym) {
        return true;
      }
    }
    return false;
  }

  bool IsNonTerminal(char sym) {
    for (auto& n_term : non_terminal) {
      if (n_term == sym) {
        return true;
      }
    }
    return false;
  }
};

//  Т.к. работаем только с КС грамматиками в данной задаче, то left = char
struct Rule {
  char left;
  std::string right;

  Rule(const char left, const std::string& right)
      : left(left), right(right) {}

  bool operator<(const Rule& other) const {
    if (left < other.left) {
      return true;
    }
    return right < other.right;
  }

  bool operator==(const Rule& other) const {
    return (left == other.left) && (right == other.right);
  }
};

struct Grammar {
  std::set<char> N;  // non-term
  std::set<char> S;  // term
  std::set<Rule> rules;
  char start = 'S'; // стартовый нетерминал

  void AddTerminal(char sym) {
    Alphabet::CheckTerminal(sym);
    S.insert(sym);
  }

  void AddNonTerminal(char sym) {
    Alphabet::CheckNonTerminal(sym);
    N.insert(sym);
  }

  void AddRule(const char left, const std::string& right) {
    if (N.find(left) == N.end()) {
      throw("Symbol " + std::string(1, left) + " is not non-term, but appeaars in left side of rule");
    }
    for (char sym : right) {
      if ((N.find(sym) == N.end()) && (S.find(sym) == S.end())) {
        throw("Symbol " + std::string(1, sym) + " is not in alphabet");
      }
    }
    rules.insert(Rule(left, right));
  }

  void AddRule(const std::string& rule) {
    if ((rule[1] != '-') && (rule[2] != '>')) {
      throw("No '->' in rule!");
    }
    if (rule.size() > 3) {
      AddRule(rule[0], rule.substr(3, rule.size()));
    } else {
      AddRule(rule[0], "");
    }
  }

  void SetStart(const char new_start) {
    if (N.find(new_start) == N.end()) {
      throw("Symbol " + std::string(1, new_start) + " can`t be a start because it`s not in the grammar non-termainals");
    }
    start = new_start;
  }
};

//  Функции для приведения грамматики к НФ Хомского
namespace NFFunc {
  //  Удаляет из грамматики все нетерминалы, заданные в сете, и содержащие их правила
  Grammar DeleteCriteriaNTerm(const std::set<char>& criteria,
                              const Grammar& grammar) {
    //  Если пытаемся удалить стартовый символ, то ошибка
    if (criteria.find(grammar.start) != criteria.end()) {
      throw("NF Error: Trying to delete starting symbol of grammar");
    }
    //  Создаём новую пустую грамматику, заполняем терминалы и нетерминалы
    Grammar result;
    for (char sym : grammar.S) {
      result.AddTerminal(sym);
    }
    for (char sym : grammar.N) {
      if (criteria.find(sym) == criteria.end()) {
        result.AddNonTerminal(sym);
      }
    }
    //  Здесь нужна гарантия, что мы не удаляем стартовый символ
    result.start = grammar.start;
    //  Копируем нужные правила
    for (auto& rule : grammar.rules) {
      if (criteria.find(rule.left) != criteria.end()) {
        continue;
      }
      bool to_copy = true;
      for (auto& sym : rule.right) {
        if (criteria.find(sym) != criteria.end()) {
          to_copy = false;
          break;
        }
      }
      if (to_copy) {
        result.AddRule(rule.left, rule.right);
      }
    }
    return result;
  }

  //  Удаление непорождающих символов
  Grammar DeleteUnbirth(const Grammar& grammar) {
    Grammar result = grammar;
    std::set<char> unbirth = result.N;
    std::map<Rule, std::set<char>> right_unterm;
    //  Находим для каждого правило мн-во нетерминалов правой части
    for (const auto& rule : result.rules) {
      right_unterm[rule] = {};
      for (const auto& sym : rule.right) {
        if (Alphabet::IsNonTerminal(sym)) {
          right_unterm[rule].insert(sym);
        }
      }
    }
    //  Находим все порождающие "сразу" символы
    std::queue<char> cur_birth;
    for (const auto& rule : result.rules) {
      if (right_unterm[rule].empty()) {
        cur_birth.push(rule.left);
      }
    }
    //  Основной цикл получения всех порождающих символов
    while (!cur_birth.empty()) {
      //  Берём произвольный порождающий символ из очереди
      char sym = cur_birth.front();
      cur_birth.pop();
      //  Удаляем его из мн-ва непорождающих
      unbirth.erase(sym);
      //  Удаляем его из правых частей
      for (const auto& rule : result.rules) {
        right_unterm[rule].erase(sym);
        //  Если пусто, то символ - порождающий. Добавляем в очередь (если не обрабатывали символ до этого, т.е. онлежит в unbirth)
        if (right_unterm[rule].empty()) {
          if (unbirth.find(sym) != unbirth.end()) {
            cur_birth.push(rule.left);
          }
        }
      }
    }
    result = DeleteCriteriaNTerm(unbirth, result);
    return result;
  }

  //  DFS для удаления недостижимых символов
  void DeleteUnreachDFS(std::map<char, bool>& reachable,
                        std::map<char, std::set<char>>& edges,
                        const char n_term) {
    reachable[n_term] = true;
    for (const auto& sym : edges[n_term]) {
      if (reachable[sym]) {
        continue;
      }
      DeleteUnreachDFS(reachable, edges, sym);
    }
  };

  //  Удаление недостижимых символов
  Grammar DeleteUnreach(const Grammar& grammar) {
    Grammar result = grammar;
    std::map<char, bool> reachable;
    for (const auto& n_term : result.N) {
      reachable[n_term] = false;
    }
    // Всевозможные переходы из нетерминала в другие (т.е. сущ-ет правило, где другой - в правой части)
    std::map<char, std::set<char>> edges;
    for (const auto& rule : result.rules) {
      for (const auto& sym : rule.right) {
        if (Alphabet::IsNonTerminal(sym)) {
          edges[rule.left].insert(sym);
        }
      }
    }
    //  DFS - ищем все достижимые из start символы
    DeleteUnreachDFS(reachable, edges, result.start);
    std::set<char> unreach;
    for (char n_term : result.N) {
      if (!reachable[n_term]) {
        unreach.insert(n_term);
      }
    }
    result = DeleteCriteriaNTerm(unreach, result);
    return result;
  }

  void FixComplexRule(Grammar& grammar, const Rule& rule) {
    //  Замена всех терминалов из правила в незанятые нетерминалы. Если не хватает символов - проблема задачи :)
    //  P.S. можно тогда расширить алфавит нетерминалов, чтобы хватало, но я пишу решение в рамках данной задачи
    std::map<char, char> translate;
    //  Проиницилизируем ' '
    for (char sym : rule.right) {
      if (Alphabet::IsTerminal(sym)) {
        translate[sym] = ' ';
      }
    }
    //  Переводим все терминалы в нетерминалы
    for (const auto& tranlation : translate) {
      for (auto& n_term : Alphabet::non_terminal) {
        if (grammar.N.find(n_term) == grammar.N.end()) {
          translate[tranlation.first] = n_term;
          grammar.N.insert(n_term);
          break;
        }
      }
    }
    //  В каждом правиле меняем терминалы на соотв. нетерминалы
    std::set<Rule> new_rules;
    for (auto& rule : grammar.rules) {
      Rule new_rule(rule.left, rule.right);
      for (size_t i = 0; i < new_rule.right.size(); ++i) {
        for (auto& translation : translate) {
          if (new_rule.right[i] == translation.first) {
            new_rule.right[i] = translation.second;
          }
        }
      }
      new_rules.insert(new_rule);
    }
    grammar.rules = new_rules;
    //  Добавляем правила
    for (const auto& translation : translate) {
      grammar.rules.insert(Rule(translation.second, std::string(1, translation.first)));
    }
  }

  //  Удаление смешанных правил
  Grammar DeleteComplex(const Grammar& grammar) {
    Grammar result = grammar;
    for (const auto& rule : grammar.rules) {
      bool has_term = false;
      bool has_nterm = false;
      for (char sym : rule.right) {
        if (Alphabet::IsTerminal(sym)) {
          has_term = true;
        } else {
          has_nterm = true;
        }
      }
      if (has_term && has_nterm) {
        FixComplexRule(result, rule);
      }
    }
    return result;
  }

  //  Удаление одного длинного правила
  void FixLongRule(Grammar& grammar, const Rule& rule) {
    //  Наберём новые символы (опять же, если не хватит - проблема условия)
    std::vector<char> new_nterm;
    for (size_t i = 0; i < rule.right.size() - 2; ++i) {
      for (const auto& sym : Alphabet::non_terminal) {
        if (grammar.N.find(sym) == grammar.N.end()) {
          new_nterm.push_back(sym);
          grammar.N.insert(sym);
          break;
        }
      }
    }
    //  Удаляем старое правило и добавляем новые
    grammar.rules.erase(rule);
    grammar.AddRule(rule.left, rule.right[0] + std::string(1, new_nterm[0]));
    for (size_t i = 1; i < new_nterm.size(); ++i) {
      grammar.AddRule(new_nterm[i - 1], rule.right[i] + std::string(1, new_nterm[i]));
    }
    grammar.AddRule(new_nterm[new_nterm.size() - 1], rule.right.substr(rule.right.size() - 2, rule.right.size()));
  }

  //  Удаление длинных правил
  Grammar DeleteLong(const Grammar& grammar) {
    Grammar result = grammar;
    for (const auto& rule : grammar.rules) {
      if (rule.right.size() > 2) {
        FixLongRule(result, rule);
      }
    }
    return result;
  }

  //  Обработка пустого слова
  void FactorEps(Grammar& grammar) {
    //  Нужно сделать новый стартовый символ и переходы
    char new_s = ' ';
    for (auto sym : Alphabet::non_terminal) {
      if (grammar.N.find(sym) == grammar.N.end()) {
        new_s = sym;
        break;
      }
    }
    grammar.rules.erase(Rule(grammar.start, ""));
    grammar.N.insert(new_s);
    grammar.AddRule(new_s, std::string(1, grammar.start));
    grammar.AddRule(new_s, "");
    grammar.start = new_s;
  }

  //  Удаление eps-порождающих символов
  //  Алгоритм не как с лекции, потому что у меня возникали ошибки, которые я никак не мог исправить,
  //  хотя логически всё было верно
  Grammar DeleteEpsBirth(const Grammar& grammar) {
    Grammar result = grammar;
    std::set<char> eps_birth = {};
    std::queue<char> cur_eps_birth;
    //  Создаём очередь "сразу" eps-порождающих
    for (const auto& rule : result.rules) {
      if (rule.right == "") {
        cur_eps_birth.push(rule.left);
      }
    }
    //  Обработка очередного члена очереди
    while (!cur_eps_birth.empty()) {
      char eps_b_nterm = cur_eps_birth.front();
      cur_eps_birth.pop();
      //  Если уже обрабатывали то пропускаем
      if (eps_birth.find(eps_b_nterm) != eps_birth.end()) {
        continue;
      }
      eps_birth.insert(eps_b_nterm);
      //  Обработка правил
      for (const auto& rule : result.rules) {
        //  Если в павиле левый нетерминал уже был обработан, то пропускаем
        if (eps_birth.find(rule.left) != eps_birth.end()) {
          continue;
        }
        //  Проверяем, все ли символы в правой части - eps-порождающие нетерминалы
        bool is_eps_b = true;
        for (const auto& sym : rule.right) {
          if (eps_birth.find(sym) == eps_birth.end()) {
            is_eps_b = false;
            break;
          }
        }
        //  Если да, то добавляем в очередь
        if (is_eps_b) {
          cur_eps_birth.push(rule.left);
        }
      }
    }
    for (const auto& rule : grammar.rules) {
      if (rule.right.size() != 2) {
        continue;
      }
      if (eps_birth.find(rule.right[0]) != eps_birth.end()) {
        result.AddRule(rule.left, std::string(1, rule.right[1]));
      }
      if (eps_birth.find(rule.right[1]) != eps_birth.end()) {
        result.AddRule(rule.left, std::string(1, rule.right[0]));
      }
    }
    auto end_rules = result.rules.end();
    for (auto iter = result.rules.begin(); iter != end_rules; ++iter) {
      if ((*iter).right.empty()) {
        --end_rules;
        result.rules.erase(iter);
      }
      if (iter == end_rules) {
        break;
      }
    }
    //  Обработаем, если в языке выводимо пустое слово (добавим новое начало)
    if (eps_birth.find(result.start) != eps_birth.end()) {
      FactorEps(result);
    }
    return result;
  }

  void DeleteSingularDFS(const Grammar& grammar, std::set<std::string>& right_parts,
                         std::map<char, bool>& visited, char sym) {
    visited[sym] = true;
    for (auto& rule : grammar.rules) {
      if (rule.left != sym) {
        continue;
      }
      if (rule.right.size() != 1) {
        right_parts.insert(rule.right);
        continue;
      }
      if (Alphabet::IsTerminal(rule.right[0])) {
        right_parts.insert(rule.right);
        continue;
      }
      if (visited[rule.right[0]]) {
        continue;
      }
      DeleteSingularDFS(grammar, right_parts, visited, rule.right[0]);
    }
  }

  //  Удаление одиночных правил
  Grammar DeleteSingular(const Grammar& grammar) {
    Grammar result = grammar;
    result.rules.clear();
    for (auto& rule : grammar.rules) {
      if ((rule.right.size() == 1) && (Alphabet::IsNonTerminal(rule.right[0]))) {
        //  Нужно сделать транзитивное замыкание для данного правила
        //  Правые части, с которыми нужно сделать замыкание
        std::set<std::string> right_parts;
        //  Посещённые в процессе DFS нетерминалы
        std::map<char, bool> visited;
        for (auto& n_term : grammar.N) {
          visited[n_term] = false;
        }
        //  Находим все правые части для замыкания
        DeleteSingularDFS(grammar, right_parts, visited, rule.left);
        //  Добавляем все замыкания
        for (auto& right : right_parts) {
          result.AddRule(rule.left, right);
        }
      } else {
        result.AddRule(rule.left, rule.right);
      }
    }
    return result;
  }

  //  Общая функция приведения к НФ
  Grammar NF(const Grammar& grammar) {
    Grammar result = DeleteUnbirth(grammar);
    result = DeleteUnreach(result);
    result = DeleteComplex(result);
    result = DeleteLong(result);
    result = DeleteEpsBirth(result);
    result = DeleteSingular(result);
    return result;
  }
};

struct CYK_Algo {
 private:
  Grammar grammar;
  std::map<char, std::map<size_t, std::map<size_t, bool>>> dp;

  void InitOneLetters(const std::string& word) {
    for (size_t i = 0; i < word.size(); ++i) {
      char sym = word[i];
      for (const auto& rule : grammar.rules) {
        if (rule.right == std::string(1, sym)) {
          dp[rule.left][i][i + 1] = true;
        }
      }
    }
  }

  void ProcessWords(size_t word_len, const std::string& word) {
    for (size_t start = 0; start <= word.size() - word_len; ++start) {
      size_t end = start + word_len;
      for (auto& rule : grammar.rules) {
        if (rule.right.size() != 2) {
          continue;
        }
        for (size_t mid = start + 1; mid < end; ++mid) {
          dp[rule.left][start][end] |= (dp[rule.right[0]][start][mid] & dp[rule.right[1]][mid][end]);
        }
      }
    }
  }

 public:
  void fit(const Grammar& g) {
    grammar = NFFunc::NF(g);
  }

  bool predict(const std::string& word) {
    //  Для пустого слова ответить можно сразу
    if (word == "") {
      return grammar.rules.find(Rule(grammar.start, "")) != grammar.rules.end();
    }
    //  Проиницализируем массив dp пустым
    for (char n_term : grammar.N) {
      for (size_t i = 0; i < word.size(); ++i) {
        for (size_t j = i + 1; j <= word.size(); ++j) {
          dp[n_term][i][j] = false;
        }
      }
    }
    //  Проинициализируем однобуквенные выводы
    InitOneLetters(word);
    //  Шаг индукции
    for (size_t word_len = 2; word_len <= word.size(); ++word_len) {
      ProcessWords(word_len, word);
    }
    return dp[grammar.start][0][word.size()];
  }
};
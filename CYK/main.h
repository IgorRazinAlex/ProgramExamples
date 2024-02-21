#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <set>
#include <string>
#include <vector>

struct State {
  bool finish;
  std::map<char, std::set<size_t>> transitions;

  State() : finish(false), transitions() {}
};

struct NKA {
 private:
  std::set<char> alphabet;
  std::vector<State> states;
  size_t start;
  std::set<size_t> ends;

 public:
  NKA()
    : alphabet({'a', 'b', 'c'}) {}

  NKA(char symbol)
    : alphabet({'a', 'b', 'c'}),
      states(2),
      start(0),
      ends({1}) {
    states[0].transitions[symbol] = {1};
    states[1].finish = true;
  }

  // Объединяет данный НКА с полученным
  NKA& unite(const NKA& other) {
    // Добавление тривиального автомата не меняет исходного
    if (other.states.size() == 0) {
      return *this;
    }
    // Запоминаем на сколько нужно поменять индексы у other.states
    size_t delta = states.size();
    // Меняем массив состояний (последнее состояние зарезервировано под новый start)
    states.resize(states.size() + other.states.size() + 1);
    for (size_t i = 0; i < other.states.size(); ++i) {
      State state = other.states[i];
      // Делаем реиндексцию в новые states
      for (auto& transition : state.transitions) {
        char symbol = transition.first;
        for (auto& vertice : transition.second) {
          states[delta + i].transitions[symbol].insert(vertice + delta);
        }
      }
      states[delta + i].finish = state.finish;
    }
    // Меняем начало
    states[states.size() - 1].transitions['1'] = {start, other.start + delta};
    start = states.size() - 1;
    // Объединяем конечные состояния
    for (auto& end : other.ends) {
      ends.insert(end + delta);
    }
    return *this;
  }

  // Конкатенирует к данному НКА справа полученный
  NKA& concatenate(const NKA& other) {
    // Добавление тривиального автомата не меняет исходного
    if (other.states.size() == 0) {
      return *this;
    }
    // Запоминаем на сколько нужно поменять индексы у other.states
    size_t delta = states.size();
    // Меняем массив состояний
    states.resize(states.size() + other.states.size());
    for (size_t i = 0; i < other.states.size(); ++i) {
      State state = other.states[i];
      // Делаем реиндексцию в новые states
      for (auto& transition : state.transitions) {
        char symbol = transition.first;
        for (auto& vertice : transition.second) {
          states[delta + i].transitions[symbol].insert(vertice + delta);
        }
      }
      states[delta + i].finish = state.finish;
    }
    // Добавляем пустые переходы из концов автомата в начало other
    for (auto& end: ends) {
      states[end].transitions['1'].insert(other.start + delta);
      states[end].finish = false;
    }
    // Меняем множество конечных состояний
    ends.clear();
    for (auto& end : other.ends) {
      ends.insert(end + delta);
    }
    return *this;
  }

  // Итерирует звездой Клини данный НКА
  void iterate() {
    // Добавляем новое начальное состояние
    states.resize(states.size() + 1);
    // Из всех концов делаем в него пустые переходы
    for (auto& end : ends) {
      states[end].transitions['1'].insert(states.size() - 1);
      states[end].finish = false;
    }
    // Меняем множество конечных состояний
    ends.clear();
    ends.insert(states.size() - 1);
    // Добавляем из нового начального состояния пустой перехд в бывшее
    states[states.size() - 1].transitions['1'].insert(start);
    // Меняем начальное состояние
    start = states.size() - 1;
    states[start].finish = true;
  }

  // Добавляет переходы e...ey в преобразовании автомата без пустых переходов
  void add_transitions(size_t current, size_t origin) {
    if (current != origin) {
      for (char symbol : alphabet) {
        for (auto& vertice : states[current].transitions[symbol]) {
          states[origin].transitions[symbol].insert(vertice);
        }
      }
    }
    for (auto& vertice : states[current].transitions['1']) {
      add_transitions(vertice, origin);
    }
  }

  // Делает origin конечной, если из неё достижима какая-то конечная по пустым 
  // переходам (возможно, новая добавленная конечная, но это не мешает, т.к. можно
  // получить путь до изначальной конечной склейкой двух путей)
  void make_ending(size_t current, size_t origin) {
    for (auto& vertice : states[current].transitions['1']) {
      if (states[origin].finish) { return; }
      if (states[vertice].finish) {
        states[origin].finish = true;
        ends.insert(origin);
        return;
      }
      make_ending(vertice, origin);
    }
  }

  // Удаляет пустые переходы из данной вершины
  void delete_empty_transitions(size_t vertice) {
    states[vertice].transitions['1'].clear();
  }

  // Прогоняем DFS чтобы найти множество достижимых
  void visit(size_t vertice, std::vector<bool>& can_visit) {
    can_visit[vertice] = true;
    for (char symbol : alphabet) {
      for (auto& to : states[vertice].transitions[symbol]) {
        if (can_visit[to]) { continue; }
        visit(to, can_visit);
      }
    }
  }

  // Преобразует НКА в эквивалентный без пустых переходов
  void normalize() {
    // Добавим новые переходы
    for (size_t i = 0; i < states.size(); ++i) {
      add_transitions(i, i);
    }
    // Добавим новые конечные вершины
    for (size_t i = 0; i < states.size(); ++i) {
      make_ending(i, i);
    }
    // Удаляем пустые переходы
    for (size_t i = 0; i < states.size(); ++i) {
      delete_empty_transitions(i);
    }
    // Удаляем вершины, в которые нельзя добраться из start (не участвуют в обработке слов)
    std::vector<bool> can_visit(states.size(), false);
    visit(start, can_visit);
    for (size_t i = 0; i < states.size(); ++i) {
      if (!can_visit[i]) {
        ends.erase(i);
        states[i] = State();
      }
    }
    // Получаем НКА без пустых переходов, но в нём могут быть "отрезанные" вершины степени 0 - они не помешают обработке,
    // т.к. в них ничего не входит и из них ничего не исходит, они не являются конечными
  }

  size_t size() {
    return states.size();
  }

  std::vector<State>& get_states() {
    return states;
  }

  std::set<char>& get_alphabet() {
    return alphabet;
  }

  std::set<size_t> get_ends() {
    return ends;
  }

  size_t get_start() {
    return start;
  }
};

// Обрабатыввет выражение в обратной польской записи в НКА с однобуквенными переходами
struct Parser {
 private:
  std::string sentence;
  std::stack<NKA> stack;
 
 public:
  // Функция чтения ввода
  void read() {
    std::cin >> sentence;
  }

  // Задать новое выражение в parser (при этом очищается вся работа с предыдущим выражением)
  void set_sentence(const std::string& string) {
    sentence = string;
    stack = std::stack<NKA>();
  }

  // Создаёт НКА, распознающий слово 'symbol'
  void create_letter_NKA(char symbol) {
    NKA nka(symbol);
    stack.push(nka);
  }

  // Объединяет два лежащих на верху стека НКА
  void sum_NKA() {
    if (stack.size() < 2) {
      throw std::runtime_error("Bad expression: trying to summarize 1 or less expressions");
    }
    NKA nka = stack.top();
    stack.pop();
    stack.top().unite(nka);
  }

  // Конкатенирует верхний НКА на стеке справа к предпоследнему на стеке
  void concatenate_NKA() {
    if (stack.size() < 2) {
      throw std::runtime_error("Bad expression: trying to concatenate 1 or less expressions");
    }
    NKA nka = stack.top();
    stack.pop();
    stack.top().concatenate(nka);
  }

  // Итерирует верхний на стеке НКА
  void iterate_NKA() {
    if (stack.size() < 1) {
      throw std::runtime_error("Bad expression: no expression to iterate");
    }
    stack.top().iterate();
  }

  // Функция разбора полученной строки в НКА
  void parse() {
    for (size_t i = 0; i < sentence.size(); ++i) {
      char symbol = sentence[i];
      if ((symbol == 'a') || (symbol == 'b') || (symbol == 'c') || (symbol == '1')) {
        create_letter_NKA(symbol);
      } else if (symbol == '+') {
        sum_NKA();
      } else if (symbol == '.') {
        concatenate_NKA();
      } else if (symbol == '*') {
        iterate_NKA();
      } else {
        // Полученный символ не является разрешённым в задаче
        throw std::runtime_error("Bad expression: symbol " +
                                 std::to_string(symbol) +
                                 " on position " +
                                 std::to_string(i) +
                                 " is not allowed operand/letter\n");
      }
    }
    // Если выражение не преобразовалось в один НКА, то кидаем ошибку
    if (stack.size() > 1) {
      throw std::runtime_error("Bad expression: input is not a single regular expression");
    }
  }

  // Преобразует stack.top() в эквивалентный НКА без пустых рёбер с однобуквенными переходами
  void normalize() {
    stack.top().normalize();
  }

  NKA& get_NKA() {
    return stack.top();
  }
};

struct LargestEndingFinder {
 private:
  char x;
  std::vector<std::vector<size_t>> edges;

 public:
  void read_letter() {
    std::cin >> x;
  }

  // Изменяет искомую букву (притом удаляет весь граф, т.к. он не актуален)
  void set_letter(char symbol) {
    x = symbol;
    edges.clear();
  }

  void read_NKA(NKA& nka) {
    if (nka.get_alphabet().find(x) == nka.get_alphabet().end()) {
      throw std::runtime_error("Bad character: not in allowed alphabet. Answer is always 0");
    }
    edges.resize(nka.size());
    for (size_t i = 0; i < nka.size(); ++i) {
      State state = nka.get_states()[i];
      for (auto& vertice : state.transitions[x]) {
        edges[vertice].push_back(i);
      }
    }
  }

  void FindCycle(int vertice, std::vector<char>& colour, bool& cycle) {
    if (cycle) { return; }
    colour[vertice] = 'g';
    for (size_t to : edges[vertice]) {
      if (colour[to] == 'b') {
        continue;
      } else if (colour[to] == 'g') {
        cycle = true;
        return;
      }
      FindCycle(to, colour, cycle);
    }
  }

  int FindLargestPath(NKA& nka) {
    int k = 0;
    for (auto& end : nka.get_ends()) {
      std::vector<int> distances(nka.size(), 0);
      std::queue<size_t> queue;
      queue.push(end);
      while (!queue.empty()) {
        size_t vertice = queue.front();
        queue.pop();
        for (size_t to : edges[vertice]) {
          distances[to] = std::max(distances[to], distances[vertice] + 1);
          k = std::max(k, distances[to]);
          queue.push(to);
        }
      }
    }
    return k;
  }

  // Поиск самого длинного пути. -1 <=> нашли цикл
  int get_answer(NKA& nka) {
    // попробкем найти цикл. Перебираем все концы. Вершины красятся в цвета: b - уже разобрана dfs,
    // g - рассматривается в данной итерации, w - не рассмотрена
    std::vector<char> colour(nka.size(), 'w');
    for (auto& end : nka.get_ends()) {
      if (colour[end] == 'b') { continue; }
      bool has_cycle = false;
      FindCycle(end, colour, has_cycle);
      if (has_cycle) {
        // Нашли цикл
        return -1;
      }
    }
    // Если не нашли цикл, то существует наибольшее расстояние - ищем его bfs
    return FindLargestPath(nka);
  }
};

void print_answer(int k) {
  if (k == -1) {
    std::cout << "INF\n";
  } else {
    std::cout << k << '\n';
  }
}
#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <set>
#include <string>
#include <vector>
#include <memory>

struct Vertice {
  bool finish;
  std::map<char, std::set<std::shared_ptr<Vertice>>> edges;
};

struct NKA {
 private:
  std::set<char> alphabet;
  std::shared_ptr<Vertice> start;
  std::shared_ptr<Vertice> end;
  size_t size_ = 0;

 public:
  NKA()
    : alphabet({'a', 'b', 'c'}),
      size_(0) {}

  NKA(char symbol)
    : alphabet({'a', 'b', 'c'}),
      size_(2) {
    start = std::make_shared<Vertice>();
    end = std::make_shared<Vertice>();
    end.get()->finish = true;
    start.get()->edges[symbol].insert(end);
  }

  size_t size() {
    return size_;
  }
  
  NKA& unite(const NKA& other) {
    if (other.size_ == 0) { return *this; }
    size_ += other.size_;

    std::shared_ptr<Vertice> new_start = std::make_shared<Vertice>();
    new_start.get()->edges[' '].insert(start);
    new_start.get()->edges[' '].insert(other.start);
    ++size_;

    std::shared_ptr<Vertice> new_end = std::make_shared<Vertice>();
    end.get()->edges[' '].insert(new_end);
    other.end.get()->edges[' '].insert(new_end);
    new_end.get()->finish = true;
    end.get()->finish = false;
    other.end.get()->finish = false;
    ++size_;

    start = new_start;
    end = new_end;

    return *this;
  }

  NKA& concatenate(const NKA& other) {
    if (other.size_ == 0) { return *this; }
    size_ += other.size_;

    end.get()->edges[' '].insert(other.start);
    end.get()->finish = false;
    end = other.end;

    return *this;
  }

  NKA& iterate() {
    if (size_ == 0) { return *this; }
    ++size_;

    std::shared_ptr<Vertice> mid = std::make_shared<Vertice>();
    mid.get()->finish = true;
    mid.get()->edges[' '].insert(start);
    end.get()->edges[' '].insert(mid);
    end.get()->finish = false;
    start = mid;
    end = mid;

    return *this;
  }

  std::set<char>& get_alphabet() {
    return alphabet;
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

  NKA& get_NKA() {
    return stack.top();
  }
};

struct LargestEndingFinder {
 private:
  char x;
  NKA nka_;
  std::map<std::shared_ptr<Vertice>, size_t> dist;

 public:
  void read_letter() {
    std::cin >> x;
  }

  // Изменяет искомую букву (притом удаляет весь граф, т.к. он не актуален)
  void set_letter(char symbol) {
    x = symbol;
    dist.clear();
  }

  void dp(NKA& nka, std::shared_ptr<Vertice> cur_vertice) {

  }

  int get_answer(NKA& nka) {
    
  }
};

int main() {
  Parser parser;
  parser.read();
  parser.parse();

  return 0;
}
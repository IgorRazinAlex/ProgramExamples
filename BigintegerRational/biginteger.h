#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>

class BigInteger {
 private:
  static const int BASE = 10000000;
  static const int BASE_STEP = 7;
  enum class Sign {
    NEG = -1,
    ZER = 0,
    POS = 1
  };
  Sign sign_ = Sign::ZER; 
  std::vector<long long> digits_;
  void nullify_();
  bool lessAbs_(const BigInteger& other) const;
  void reverseSign_();
  void shift_();
  size_t size_() const;
  bool isPositive_() const;
  bool isZero_() const;
  bool isNegative_() const;
  void removeLeadingZeros_();
  static void add(std::vector<long long>& digits1, const std::vector<long long>& digits2);
  static std::vector<long long> substract(std::vector<long long> digits1, 
                                          const std::vector<long long>& digits2);
  static void multiply(std::vector<long long>& digits1, const std::vector<long long>& digits2);

 public:
  BigInteger(long long num);
  BigInteger(const std::string& num);
  BigInteger();
  BigInteger(const BigInteger& other);
  BigInteger& operator=(const BigInteger &other);
  BigInteger& operator+=(const BigInteger &other);
  BigInteger& operator-=(const BigInteger &other);
  BigInteger& operator*=(const BigInteger &other);
  BigInteger& operator/=(const BigInteger &other);
  BigInteger operator/(const BigInteger &other) const;
  BigInteger& operator%=(const BigInteger &other);
  bool operator==(const BigInteger& other) const;
  bool operator!=(const BigInteger& other) const;
  bool operator<(const BigInteger& other) const;
  bool operator>=(const BigInteger& other) const;
  bool operator>(const BigInteger& other) const;
  bool operator<=(const BigInteger& other) const;
  BigInteger operator-() const;
  BigInteger& operator++();
  BigInteger operator++(int);
  BigInteger& operator--();
  BigInteger operator--(int);
  std::string toString() const;
  explicit operator bool() const;
  explicit operator int() const;
};

class Rational {
 private:
  static const size_t DOUBLE_DIGITS = 20;
  BigInteger numerator_ = 0;
  BigInteger denomerator_ = 1;
  void normalize_();
  static BigInteger gcd_(BigInteger b_int1, BigInteger b_int2);

 public:
  Rational();
  Rational(const BigInteger& b_int);
  Rational(long long num);
  Rational& operator+=(const Rational& other);
  Rational& operator-=(const Rational& other);
  Rational& operator*=(const Rational& other);
  Rational& operator/=(const Rational& other);
  Rational operator-() const;
  bool operator==(const Rational& other) const;
  bool operator!=(const Rational& other) const;
  bool operator<(const Rational& other) const;
  bool operator>=(const Rational& other) const;
  bool operator>(const Rational& other) const;
  bool operator<=(const Rational& other) const;
  std::string toString() const;
  std::string asDecimal(size_t precision) const;
  explicit operator double() const;
};

BigInteger::BigInteger(long long num) {
  if (num > 0) {
    sign_ = Sign::POS;
  } else if (num == 0) {
    nullify_();
    return;
  } else {
    num *= -1;
    sign_ = Sign::NEG;
  }
  while (num > 0) {
    digits_.push_back(num % BASE);
    num /= BASE;
  }
}

BigInteger::BigInteger(const std::string& num) {
  if (num == "0" || num == "-0") {
    nullify_();
    return;
  }
  long long cell = 0;
  int cnt = 0;
  for (size_t i = static_cast<int>(num.size()) - 1; i > 0; --i) {
    cell = (num[i] - '0') * static_cast<long long>(pow(10, cnt)) + cell;
    ++cnt;
    if (cnt == BASE_STEP) {
      digits_.push_back(cell);
      cell = 0;
      cnt = 0;
    }
  }
  if (num[0] == '-') {
    sign_ = Sign::NEG;
  } else {
    sign_ = Sign::POS;
    cell = (num[0] - '0') * static_cast<long long>(pow(10, cnt)) + cell;
    ++cnt;
  }
  if (cnt != 0) {
    digits_.push_back(cell);
  }
}

BigInteger::BigInteger() : sign_(Sign::ZER), digits_(1, 0){};

void BigInteger::shift_() {
  if (digits_.empty()) {
    digits_.push_back(0);
    return;
  }
  digits_.push_back(digits_[size_() - 1]);
  for (size_t i = size_() - 2; i > 0; --i) {
    digits_[i] = digits_[i - 1];
  }
  digits_[0] = 0;
}

BigInteger::BigInteger(const BigInteger& other) = default;

BigInteger& BigInteger::operator=(const BigInteger& other) = default;

size_t BigInteger::size_() const {
  return digits_.size();
}

bool BigInteger::isPositive_() const {
  return sign_ == Sign::POS;
}

bool BigInteger::isZero_() const {
  return sign_ == Sign::ZER;
}

bool BigInteger::isNegative_() const {
  return sign_ == Sign::NEG;
}

bool BigInteger::operator==(const BigInteger& other) const {
  if (sign_ != other.sign_) {
    return false;
  }
  if (size_() != other.size_()) {
    return false;
  }
  for (int i = size_() - 1; i >= 0; --i) {
    if (digits_[i] != other.digits_[i]) {
      return false;
    }
  }
  return true;
}

bool BigInteger::operator!=(const BigInteger& other) const {
  return !(*this == other);
}

bool BigInteger::lessAbs_(const BigInteger& other) const {
  if (size_() < other.size_()) {
    return true;
  }
  else if (size_() > other.size_()) {
    return false;
  }
  for (int i = static_cast<int>(size_()) - 1; i >= 0; --i) {
    if (digits_[i] < other.digits_[i]) {
      return true;
    }
    else if (digits_[i] > other.digits_[i]) {
      return false;
    }
  }
  return false;
}

bool BigInteger::operator<(const BigInteger& other) const {
  if (static_cast<int>(sign_) < static_cast<int>(other.sign_)) {
    return true;
  }
  if (static_cast<int>(sign_) > static_cast<int>(other.sign_)) {
    return false;
  }
  bool res = lessAbs_(other) || digits_ == other.digits_;
  if (isNegative_()) {
    return !res;
  }
  if (isPositive_()) {
    return res && (*this != other);
  }
  return false;
}

bool BigInteger::operator>=(const BigInteger& other) const {
  return !(*this < other);
}

bool BigInteger::operator>(const BigInteger& other) const {
  return other < *this;
}

bool BigInteger::operator<=(const BigInteger& other) const {
  return !(*this > other);
}

BigInteger BigInteger::operator-() const {
  BigInteger res = *this;
  res.reverseSign_();
  return res;
}

void BigInteger::removeLeadingZeros_() {
  for (int i = static_cast<int>(digits_.size()) - 1; i >= 0; --i) {
    if (digits_[i] == 0) {
      digits_.pop_back();
    } else {
      break;
    }
  }
  if (sign_ == Sign::ZER) {
    nullify_();
  }
}

void BigInteger::add(std::vector<long long>& digits1, const std::vector<long long>& digits2) {
  digits1.resize(std::max(digits1.size(), digits2.size()) + 1);
  for (size_t i = 0; i < digits2.size(); ++i) {
    digits1[i] += digits2[i];
  }
  for (size_t i = 0; i < digits2.size(); ++i) {
    digits1[i + 1] += digits1[i] / BASE;
    digits1[i] %= BASE;
  }
  size_t i = digits2.size();
  while (digits1[i] > BASE) {
    digits1[i + 1] += digits1[i] / BASE;
    digits1[i] %= BASE;
    ++i;
  }
}

std::vector<long long> BigInteger::substract(std::vector<long long> digits1, 
                                             const std::vector<long long>& digits2) {
  digits1.resize(std::max(digits1.size(), digits2.size()) + 1);
  for (size_t i = 0; i < digits2.size(); ++i) {
    digits1[i] -= digits2[i];
  }
  for (size_t i = 0; i < digits2.size(); ++i) {
    if (digits1[i] < 0) {
      --digits1[i + 1];
      digits1[i] += BASE;
    }
  }
  size_t i = digits2.size();
  while (digits1[i] < 0) {
    --digits1[i + 1];
    digits1[i] += BASE;
    ++i;
  }
  return digits1;
}

void BigInteger::multiply(std::vector<long long>& digits1, const std::vector<long long>& digits2) {
  std::vector<long long> res;
  res.resize(digits1.size() + digits2.size() + 1);
  for (size_t i = 0; i < digits1.size(); ++i) {
    for (size_t j = 0; j < digits2.size(); ++j) {
      res[i + j] += digits1[i] * digits2[j];
    }
  }
  for (size_t i = 0; i < res.size(); ++i) {
    res[i + 1] += res[i] / BASE;
    res[i] %= BASE;
  }
  digits1 = res;
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
  if (other.isZero_()) {
    return *this;
  } else if (isZero_()) {
    digits_ = other.digits_;
    sign_ = other.sign_;
  } else if (sign_ == other.sign_) {
    add(digits_, other.digits_);
  } else if (digits_ == other.digits_) {
    nullify_();
  } else {
    if (lessAbs_(other)) {
      digits_ = substract(other.digits_, digits_);
      reverseSign_();
    } else {
      digits_ = substract(digits_, other.digits_);
    }
  }
  removeLeadingZeros_();
  return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& other) {
  if (other.isZero_()) {
    return *this;
  } else if (isZero_()) {
    digits_ = other.digits_;
    sign_ = (other.isPositive_() ? Sign::NEG : Sign::POS);
  } else if (sign_ != other.sign_) {
    add(digits_, other.digits_);
  } else if (digits_ == other.digits_) {
    nullify_();
  } else {
    if (lessAbs_(other)) {
      digits_ = substract(other.digits_, digits_);
      reverseSign_();
    } else {
      digits_ = substract(digits_, other.digits_);
    }
  }
  removeLeadingZeros_();
  return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
  int this_sign = static_cast<int>(sign_);
  int other_sign = static_cast<int>(other.sign_);
  if (sign_ == Sign::ZER) {
    return *this;
  } else if (other.sign_ == Sign::ZER) {
    nullify_();
  } else {
    multiply(digits_, other.digits_);
    if (this_sign == other_sign) {
      sign_ = Sign::POS;
    } else {
      sign_ = Sign::NEG;
    }
  }
  removeLeadingZeros_();
  return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& other) {
  *this = (*this) / other;
  return *this;
}

BigInteger operator+(const BigInteger& b_int1, const BigInteger& b_int2) {
  BigInteger tmp = b_int1;
  return tmp += b_int2;
}

BigInteger operator-(const BigInteger& b_int1, const BigInteger& b_int2) {
  BigInteger tmp = b_int1;
  return tmp += -b_int2;
}

BigInteger operator*(const BigInteger& b_int1, const BigInteger& b_int2) {
  BigInteger tmp = b_int1;
  return tmp *= b_int2;
}

BigInteger BigInteger::operator/(const BigInteger& b_int) const {
  if (b_int.size_() > size_() || (lessAbs_(b_int))) {
    return 0;
  }
  if (b_int == *this) {
    return 1;
  }
  BigInteger result, tmp, copy = b_int;
  copy.sign_ = Sign::POS;
  result.digits_.resize(size_());
  for (int i = static_cast<int>(size_()) - 1; i >= 0; --i) {
    tmp.shift_();
    tmp.digits_[0] = digits_[i];
    if (tmp.digits_[0] != 0) {
      tmp.sign_ = Sign::POS;
    }
    tmp.removeLeadingZeros_();
    int digit = 0, left = 0, right = BASE;
    while (left <= right) {
      int middle = (left+ right) / 2;
      BigInteger t = copy * middle;
      if (t <= tmp) {
        digit = middle;
        left = middle + 1;
      } else {
        right = middle - 1;
      }
    }
    result.digits_[i] = digit;
    tmp = tmp - copy * digit;
  }
  result.sign_ = (sign_ == b_int.sign_ ? Sign::POS : Sign::NEG);
  result.removeLeadingZeros_();
  return result;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
  BigInteger tmp = other;
  return *this -= other * (*this / tmp);
}

BigInteger operator%(const BigInteger& b_int1, const BigInteger& b_int2) {
  BigInteger tmp = b_int1;
  return tmp %= b_int2; 
}

BigInteger& BigInteger::operator++() {
  *this += 1;
  return *this;
}

BigInteger BigInteger::operator++(int) {
  ++*this;
  return *this - 1;
}

BigInteger& BigInteger::operator--() {
  *this -= 1;
  return *this;
}

BigInteger BigInteger::operator--(int) {
  --*this;
  return *this + 1;
}

BigInteger::operator bool() const {
  return sign_ != Sign::ZER;
}

BigInteger::operator int() const {
  int res = 0;
  int power = 1;
  for (size_t i = 0; i < size_(); ++i) {
    res += digits_[i] * power;
    power *= BASE;
  }
  return res;
}

void BigInteger::nullify_() {
  sign_ = Sign::ZER;
  digits_.clear();
  digits_.push_back(0);
}

void BigInteger::reverseSign_() {
  if (sign_ == Sign::NEG) {
    sign_ = Sign::POS;
  } else if (sign_ == Sign::POS) {
    sign_ = Sign::NEG;
  }
}

std::ostream& operator<<(std::ostream& out, const BigInteger& b_int) {
  std::string num = b_int.toString();
  out << num;
  return out;
}

std::istream& operator>>(std::istream& in, BigInteger& b_int) {
  std::string res;
  in >> res;
  b_int = BigInteger(res);
  return in;
}

std::string BigInteger::toString() const {
  std::string res;
  if (sign_ == Sign::ZER) {
    res += '0';
    return res;
  }
  for (size_t i = 0; i < size_() - 1; ++i) {
    std::string digit = std::to_string(digits_[i]);
    while (digit.size() < BASE_STEP) {
      digit = '0' + digit;
    }
    res = digit + res;
  }
  res = std::to_string(digits_[size_() - 1]) + res;
  if (sign_ == Sign::NEG) {
    res = '-' + res;
  }
  return res;
}

BigInteger operator""_bi(unsigned long long num) {
  BigInteger res(num);
  return res;
}

BigInteger operator""_bi(const char* num, size_t) {
  BigInteger res(num);
  return res;
}

BigInteger Rational::gcd_(BigInteger b_int1, BigInteger b_int2) {
  if (b_int1 < 0) {
    b_int1 *= -1;
  }
  if (b_int2 < 0) {
    b_int2 *= -1;
  }
  if (b_int1 == 0 || b_int2 == 0) {
    return 0;
  } else {
    if (b_int1 < b_int2) {
      std::swap(b_int1, b_int2);
    }
    while (b_int1 % b_int2 != 0) {
      b_int1 %= b_int2;
      std::swap(b_int1, b_int2);
    }
  }
  return b_int2;
}

Rational::Rational() = default;

Rational::Rational(const BigInteger &b_int) : numerator_(b_int), denomerator_(1) {}

Rational::Rational(long long num) : numerator_(num), denomerator_(1) {}

void Rational::normalize_() {
  if (denomerator_ < 0) {
    denomerator_ *= -1;
    numerator_ *= -1;
  }
  if (denomerator_ == 1) {
    return;
  }
  if (numerator_ == 0) {
    denomerator_ = 1;
    return;
  }
  if (numerator_ == 1) {
    return;
  }
  if (numerator_ == denomerator_) {
    numerator_ = 1;
    denomerator_ = 1;
    return;
  }
  if (denomerator_ > numerator_ && denomerator_ % numerator_ == 0) {
    denomerator_ /= numerator_;
    numerator_ = 1;
    return;
  }
  if (numerator_ > denomerator_ && numerator_ % denomerator_ == 0) {
    numerator_ /= denomerator_;
    denomerator_ = 1;
    return;
  }
  BigInteger common = gcd_(numerator_, denomerator_);
  if (common > 1) {
    numerator_ /= common;
    denomerator_ /= common;
  }
}

Rational& Rational::operator+=(const Rational& other) {
  if (denomerator_ == other.denomerator_) {
    numerator_ += other.numerator_;
  } else {
    numerator_ *= other.denomerator_;
    numerator_ += other.numerator_ * denomerator_;
    denomerator_ *= other.denomerator_;
  }
  normalize_();
  return *this;
}

Rational& Rational::operator-=(const Rational& other) {
  *this += -other;
  return *this;
}

Rational& Rational::operator*=(const Rational& other) {
  numerator_ *= other.numerator_;
  denomerator_ *= other.denomerator_;
  normalize_();
  return *this;
}

Rational& Rational::operator/=(const Rational& other) {
  if (numerator_ == 0) {
    return *this;
  }
  numerator_ *= other.denomerator_;
  denomerator_ *= other.numerator_;
  normalize_();
  return *this;
}

Rational operator+(const Rational& ratio1, const Rational& ratio2) {
  Rational tmp = ratio1;
  tmp += ratio2;
  return tmp;
}

Rational operator-(const Rational &ratio1, const Rational &ratio2) {
  Rational tmp = ratio1;
  tmp -= ratio2;
  return tmp;
}

Rational operator*(const Rational &ratio1, const Rational &ratio2) {
  Rational tmp = ratio1;
  tmp *= ratio2;
  return tmp;
}

Rational operator/(const Rational &ratio1, const Rational &ratio2) {
  Rational tmp = ratio1;
  tmp /= ratio2;
  return tmp;
}

Rational Rational::operator-() const {
  Rational tmp = *this;
  tmp.numerator_ *= -1;
  return tmp;
}

bool Rational::operator==(const Rational& other) const {
  return (numerator_ == other.numerator_) &&
         (denomerator_ == other.denomerator_);
}

bool Rational::operator!=(const Rational& other) const {
  return !(*this == other);
}

bool Rational::operator<(const Rational& other) const {
  return (other - *this).numerator_ > 0;
}

bool Rational::operator>=(const Rational& other) const {
  return !(*this < other);
}

bool Rational::operator>(const Rational& other) const {
  return other < *this;
}

bool Rational::operator<=(const Rational& other) const {
  return !(*this > other);
}

std::string Rational::toString() const {
  std::string res = numerator_.toString();
  if (denomerator_ != 1) {
    res += '/' + denomerator_.toString();
  }
  return res;
}

std::string Rational::asDecimal(size_t precision) const {
  BigInteger copy1 = numerator_, copy2 = denomerator_;
  std::string res = (copy1 / copy2).toString();
  if ((numerator_ < 0) && res[0] != '-') {
    res = '-' + res;
  }
  if (precision != 0) {
    res += '.';
    Rational tmp = *this;
    if (tmp.numerator_ < 0) {
      tmp.numerator_ *= -1;
    }
    tmp.numerator_ %= denomerator_;
    for (size_t i = 0; i < precision; ++i) {
      tmp.numerator_ *= 10;
      res += (tmp.numerator_ / tmp.denomerator_).toString();
      tmp.numerator_ %= denomerator_;
    }
  }
  return res;
}

Rational::operator double() const {
  return std::stod(asDecimal(DOUBLE_DIGITS));
}
#pragma once
#define _USE_MATH_DEFINES
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

const double ACCURACY = pow(10, -6);
const double WHOLE_ANGLE = 180.0;

class Line;

namespace Essentials {
  bool are_equal_numbers(double a, double b) {
    return fabs(a - b) < ACCURACY;
  }
};

struct Point {
 public:
  double x = 0;
  double y = 0;
  Point() = default;
  Point(double x, double y) : x(x), y(y) {}
  Point(const Point& point) : x(point.x), y(point.y) {};
  Point& operator=(const Point& point);
  void rotate(const Point& center, double angle);
  void reflect(const Point& center);
  void reflect(const Line& axis);
  void scale(const Point& center, double coefficient);
};

Point& Point::operator=(const Point& point) {
  x = point.x;
  y = point.y;
  return *this;
}

void Point::rotate(const Point& center, double angle) {
  double dx = x - center.x;
  double dy = y - center.y;
  double radian = M_PI * angle / WHOLE_ANGLE;
  x = center.x + dx * cos(radian) - dy * sin(radian);
  y = center.y + dx * sin(radian) + dy * cos(radian);
}

void Point::reflect(const Point& center) {
  x = 2 * center.x - x;
  y = 2 * center.y - y;
}

void Point::scale(const Point& center, double coefficient) {
  x = center.x + (x - center.x) * coefficient;
  y = center.y + (y - center.y) * coefficient;
}

double distancePoints(const Point& first_point, const Point& second_point) {
  return sqrt(pow(first_point.x - second_point.x, 2.0) + pow(first_point.y - second_point.y, 2.0));
}

bool operator==(const Point& first_point, const Point& second_point) {
  return distancePoints(first_point, second_point) < ACCURACY;
}

bool operator!=(const Point& first_point, const Point& second_point) {
  return !(first_point == second_point);
}

struct Vector {
 public:
  double x = 0;
  double y = 0;
  Vector() = default;
  Vector(double x, double y) : x(x), y(y) {};
  Vector(const Point& first_point, const Point& second_point)
      : x(second_point.x - first_point.x),
        y(second_point.y - first_point.y) {}
  Vector(const Vector& other);
  ~Vector() = default;
  Vector& operator=(const Vector& other);
  double length();
};

Vector::Vector(const Vector& other) {
  x = other.x;
  y = other.y;
}

Vector& Vector::operator=(const Vector& other) {
  x = other.x;
  y = other.y;
  return *this;
}

double Vector::length() {
  return (sqrt(pow(x, 2.0) + pow(y, 2.0)));
}

bool operator==(const Vector& first_vector, const Vector& second_vector) {
  return Essentials::are_equal_numbers(first_vector.x, second_vector.x) &&
         Essentials::are_equal_numbers(first_vector.y, second_vector.y);
}

bool operator!=(const Vector& first_vector, const Vector& second_vector) {
  return !(first_vector == second_vector);
}

double crossProduct(const Vector& first_vector, const Vector& second_vector) {
  return second_vector.y * first_vector.x - second_vector.x * first_vector.y;
}

class Line {
 public:
  double a, b, c;
  Line(const Point& first_point, const Point& second_point);
  Line(double angle_coefficient, double shift);
  Line(const Point& point, double angle_coefficient);
  Line(const Line& other);
  ~Line() = default;
  Line& operator=(const Line& other);
};

Line::Line(const Point& first_point, const Point& second_point)
    : a(first_point.y - second_point.y),
      b(second_point.x - first_point.x),
      c(-a * first_point.x - b * first_point.y) {}

Line::Line(double angle_coefficient, double shift)
    : a(-angle_coefficient), b(1), c(-shift) {}

Line::Line(const Point& point, double angle_coefficient)
    : a(-angle_coefficient), b(1), c(angle_coefficient * point.x - point.y) {}

Line::Line(const Line& other) {
  a = other.a;
  b = other.b;
  c = other.c;
}

Line& Line::operator=(const Line& other) {
  a = other.a;
  b = other.b;
  c = other.c;
  return *this;
}

bool operator==(const Line& first_line, const Line& second_line) {
  double check_ab = second_line.a * first_line.b - second_line.b * first_line.a;
  double check_bc = second_line.b * first_line.c - second_line.c * first_line.b;
  double check_ca = second_line.c * first_line.a - second_line.a * first_line.c;
  return Essentials::are_equal_numbers(check_ab, 0) &&
         Essentials::are_equal_numbers(check_bc, 0) &&
         Essentials::are_equal_numbers(check_ca, 0);
}

bool operator!=(const Line& first_line, const Line& second_line) {
  return !(first_line == second_line);
}

Line perpendicular(const Point& point, const Line& axis) {
  Point on_perpendicular(point.x + axis.a, point.y + axis.b);
  return Line(point, on_perpendicular);
}

Point intersection(const Line& first_line, const Line& second_line) {
  double x = -first_line.c * second_line.b + second_line.c * first_line.b;
  double y = -first_line.a * second_line.c + second_line.a * first_line.c;
  double normalise_factor = first_line.a * second_line.b - second_line.a * first_line.b;
  return Point(x / normalise_factor, y / normalise_factor);
}

void Point::reflect(const Line& axis) {
  Line perpendicular_line = perpendicular(*this, axis);
  Point center = intersection(axis, perpendicular_line);
  reflect(center);
}

class Shape {
 private:
 public:
  virtual ~Shape() = default;
  virtual double area() const = 0;
  virtual double perimeter() const = 0;
  virtual bool equalShape(const Shape& other) const = 0;
  virtual bool isCongruentTo(const Shape& other) const = 0;
  virtual bool isSimilarTo(const Shape& other) const = 0;
  virtual bool containsPoint(const Point& point) const = 0;
  virtual void rotate(const Point& center, double angle) = 0;
  virtual void reflect(const Point& center) = 0;
  virtual void reflect(const Line& axis) = 0;
  virtual void scale(const Point& center, double coefficient) = 0;
};

bool operator==(const Shape& first_shape, const Shape& second_shape) {
  return first_shape.equalShape(second_shape);
}

bool operator!=(const Shape& first_shape, const Shape& second_shape) {
  return !(first_shape == second_shape);
}

class Polygon: public Shape {
 private:
 protected:
  std::vector<Point> vertices_;
  Point nextVertice_(size_t index) const;
  Point prevVertice_(size_t index) const;
  Point center_() const;
  bool checkEquality_(const std::vector<Point>& other_vertices, size_t start, bool direction) const;
  bool checkTriangleCongruency_(const std::vector<Point>& vertices, const std::vector<Point>& other_vertices,
                                size_t start, size_t end) const;
  bool checkCongruencyFromStart_(const std::vector<Point>& vertices, const std::vector<Point>& other_vertices,
                                 size_t start) const;
  bool checkCongruency_(const std::vector<Point>& vertices, const std::vector<Point>& other_vertices) const;
  bool checkTriangleSimilarity(const std::vector<Point>& vertices, const std::vector<Point>& other_vertices,
                               size_t start, size_t end) const;
  bool checkSimilarityFromStart_(const std::vector<Point>& vertices, const std::vector<Point>& other_vertices,
                                 size_t start) const;
  bool checkSimilarity_(const std::vector<Point>& vertices, const std::vector<Point>& other_vertices) const;
 public:
  Polygon() = default;
  Polygon(std::vector<Point> vertices);
  template <typename ... Args>
  Polygon(Args ... point);
  virtual ~Polygon() = default;
  size_t verticesCount() const;
  std::vector<Point> getVertices() const;
  bool isConvex() const;
  double area() const override;
  double perimeter() const override;
  bool equalShape(const Shape& other) const override;
  bool isCongruentTo(const Shape& other) const override;
  bool isSimilarTo(const Shape& other) const override;
  bool containsPoint(const Point& point) const override;
  void rotate(const Point& center, double angle) override;
  void reflect(const Point& center) override;
  void reflect(const Line& axis) override;
  void scale(const Point& center, double coefficient) override;
};

Point Polygon::nextVertice_(size_t index) const {
  return vertices_[(index + 1) % vertices_.size()];
}

Point Polygon::prevVertice_(size_t index) const {
  return vertices_[(index + vertices_.size() - 1) % vertices_.size()];
}

Point Polygon::center_() const {
  double x = 0, y = 0;
  for (size_t i = 0; i < vertices_.size(); ++i) {
    x += vertices_[i].x;
    y += vertices_[i].y;
  }
  return Point(x / vertices_.size(), y / vertices_.size());
}

Polygon::Polygon(std::vector<Point> vertices) : vertices_(vertices) {}

template <typename ... Args>
Polygon::Polygon(Args ... point) {
  (vertices_.push_back(point), ...);
}

size_t Polygon::verticesCount() const {
  return vertices_.size();
}

std::vector<Point> Polygon::getVertices() const {
  return vertices_;
}

bool Polygon::isConvex() const {
  double convex_characteristic = 0;
  for (size_t i = 0; i < vertices_.size(); ++i) {
    double new_convex_characteristic = crossProduct(Vector(vertices_[i], nextVertice_(i)),
                                                    Vector(vertices_[i], prevVertice_(i)));
    if (convex_characteristic * new_convex_characteristic < 0) {
      return false;
    }
    convex_characteristic = new_convex_characteristic;
  }
  return true;
}

double Polygon::area() const {
  double result = 0;
  for (size_t i = 0; i < vertices_.size(); ++i) {
    result += vertices_[i].x * nextVertice_(i).y;
    result -= vertices_[i].y * nextVertice_(i).x;
  }
  return fabs(result) / 2;
}

double Polygon::perimeter() const {
  double result = 0;
  for (size_t i = 0; i < vertices_.size(); ++i) {
    result += distancePoints(vertices_[i], nextVertice_(i));
  }
  return result;
}

bool Polygon::checkEquality_(const std::vector<Point>& other_vertices, size_t start, bool direction) const {
  for (size_t i = 0; i < verticesCount(); ++i) {
    if ((vertices_[i] != other_vertices[(start + i) % verticesCount()]) && direction) {
      return false;
    }
    if ((vertices_[i] != other_vertices[(start + verticesCount() - i) % verticesCount()]) && !direction) {
      return false;
    }
  }
  return true;
}

bool Polygon::equalShape(const Shape& other) const {
  const Polygon* other_polygon = dynamic_cast<const Polygon*>(&other);
  if (other_polygon == nullptr) {
    return false;
  }
  if (verticesCount() != other_polygon->verticesCount()) {
    return false;
  }
  size_t start = verticesCount();
  for (size_t i = 0; i < verticesCount(); ++i) {
    if (vertices_[0] == other_polygon->vertices_[i]) {
      start = i;
      break;
    }
  }
  if (start == verticesCount()) {
    return false;
  }
  return checkEquality_(other_polygon->vertices_, start, 1) ||
         checkEquality_(other_polygon->vertices_, start, 0);
}

bool Polygon::checkTriangleCongruency_(const std::vector<Point>& vertices,
                                       const std::vector<Point>& other_vertices,
                                       size_t start, size_t end) const {
  size_t size = verticesCount();
  Vector side(vertices[(start + end) % size], vertices[(start + end + 1) % size]);
  Vector other_side(other_vertices[end], other_vertices[(end + 1) % size]);
  if (!Essentials::are_equal_numbers(side.length(), other_side.length())) {
    return false;
  }
  double area = crossProduct(side, Vector(vertices[(start + end) % size],
                                          vertices[(start + end + size - 1) % size]));
  double other_area = crossProduct(other_side, Vector(other_vertices[end],
                                                      other_vertices[(end + size - 1) % size]));
  if (!Essentials::are_equal_numbers(fabs(area), fabs(other_area))) {
       return false;
  }
  return true;
}

bool Polygon::checkCongruencyFromStart_(const std::vector<Point>& vertices,
                                        const std::vector<Point>& other_vertices,
                                        size_t start) const {
  for (size_t end = 0; end < verticesCount(); ++end) {
    if (!checkTriangleCongruency_(vertices, other_vertices, start, end)) {
      return false;
    }
  }
  return true;
}

bool Polygon::checkCongruency_(const std::vector<Point>& vertices,
                               const std::vector<Point>& other_vertices) const {
  size_t size = verticesCount();
  for (size_t start = 0; start < size; ++start) {
    if (checkCongruencyFromStart_(vertices, other_vertices, start)) {
      return true;
    }
  }
  return false;
}

bool Polygon::checkTriangleSimilarity(const std::vector<Point>& vertices,
                                      const std::vector<Point>& other_vertices,
                                      size_t start, size_t end) const {
  size_t size = verticesCount();
  Vector side(vertices[(start + end) % size], vertices[(start + end + 1) % size]);
  Vector other_side(other_vertices[end], other_vertices[(end + 1) % size]);
  if (Essentials::are_equal_numbers(side.length(), 0) ^
      Essentials::are_equal_numbers(other_side.length(), 0)) {
    return false;
  }
  double coefficient = side.length() / other_side.length();
  if (!Essentials::are_equal_numbers(coefficient, (side.length() / other_side.length()))) {
    return false;
  }
  double area = crossProduct(side, Vector(vertices[(start + end) % size],
                                          vertices[(start + end + size - 1) % size])) / 2.0;
  double other_area = crossProduct(other_side, Vector(other_vertices[end],
                                                      other_vertices[(end + size - 1) % size])) / 2.0;
  if (!Essentials::are_equal_numbers(fabs(area), pow(coefficient, 2.0) * fabs(other_area))) {
    return false;
  }
  return true;
}

bool Polygon::checkSimilarityFromStart_(const std::vector<Point>& vertices,
                                        const std::vector<Point>& other_vertices,
                                        size_t start) const {
  for (size_t end = 0; end < verticesCount(); ++end) {
    if (!checkTriangleSimilarity(vertices, other_vertices, start, end)) {
      return false;
    }
  }
  return true;
}

bool Polygon::checkSimilarity_(const std::vector<Point>& vertices,
                               const std::vector<Point>& other_vertices) const {
  size_t size = verticesCount();
  for (size_t start = 0; start < size; ++start) {
    if (checkSimilarityFromStart_(vertices, other_vertices, start)) {
      return true;
    }
  }
  return false;
}

bool Polygon::isCongruentTo(const Shape& other) const {
  const Polygon* other_polygon = dynamic_cast<const Polygon*>(&other);
  if (other_polygon == nullptr) {
    return false;
  }
  if (verticesCount() != other_polygon->verticesCount()) {
    return false;
  }
  if (!Essentials::are_equal_numbers(perimeter(), other_polygon->perimeter()) ||
      !Essentials::are_equal_numbers(area(), other_polygon->area())) {
    return false;
  }
  std::vector<Point> other_vertices = other_polygon->getVertices();
  if (checkCongruency_(vertices_, other_vertices)) {
    return true;
  }
  std::vector<Point> reversed_vertices(vertices_.rbegin(), vertices_.rend());
  return checkCongruency_(reversed_vertices, other_vertices);
}

bool Polygon::isSimilarTo(const Shape& other) const {
  const Polygon* other_polygon = dynamic_cast<const Polygon*>(&other);
  if (other_polygon == nullptr) {
    return false;
  }
  if (verticesCount() != other_polygon->verticesCount()) {
    return false;
  }
  std::vector<Point> other_vertices = other_polygon->getVertices();
  if (checkSimilarity_(vertices_, other_vertices)) {
    return true;
  }
  std::vector<Point> reversed_vertices(vertices_.rbegin(), vertices_.rend());
  return checkSimilarity_(reversed_vertices, other_vertices);
}

bool Polygon::containsPoint(const Point& point) const {
  bool contains = false;
  size_t j = vertices_.size() - 1;
  for (size_t i = 0; i < vertices_.size(); ++i) {
    if (((vertices_[i].y < point.y && vertices_[j].y >= point.y)
        || (vertices_[j].y < point.y && vertices_[i].y >= point.y))
        && (vertices_[i].x + (point.y - vertices_[i].y) / (vertices_[j].y - vertices_[i].y)
        * (vertices_[j].x - vertices_[i].x) < point.x + ACCURACY)) {
      contains = !contains;
    }
    j = i;
  }
  return contains;
}

void Polygon::rotate(const Point& center, double angle) {
  for (size_t i = 0; i < vertices_.size(); ++i) {
    vertices_[i].rotate(center, angle);
  }
}

void Polygon::reflect(const Point& center) {
  for (size_t i = 0; i < vertices_.size(); ++i) {
    vertices_[i].reflect(center);
  }
}

void Polygon::reflect(const Line& axis) {
  for (size_t i = 0; i < vertices_.size(); ++i) {
    vertices_[i].reflect(axis);
  }
}

void Polygon::scale(const Point& center, double coefficient) {
  for (size_t i = 0; i < vertices_.size(); ++i) {
    vertices_[i].scale(center, coefficient);
  }
}

class Ellipse: public Shape {
 private:
 protected:
  Point first_focus;
  Point second_focus;
  double diameter;
 public:
  Ellipse() = default;
  Ellipse(Point first_focus, Point second_focus, double diameter);
  virtual ~Ellipse() = default;
  std::pair<Point, Point> focuses() const;
  std::pair<Line, Line> directrises() const;
  double eccentricity() const;
  Point center() const;
  double area() const override;
  double perimeter() const override;
  bool equalShape(const Shape& other) const override;
  bool isCongruentTo(const Shape& other) const override;
  bool isSimilarTo(const Shape& other) const override;
  bool containsPoint(const Point& point) const override;
  void rotate(const Point& center, double angle) override;
  void reflect(const Point& center) override;
  void reflect(const Line& axis) override;
  void scale(const Point& center, double coefficient) override;
};

Ellipse::Ellipse(Point first_focus, Point second_focus, double diameter)
    : first_focus(first_focus), second_focus(second_focus), diameter(diameter) {}

std::pair<Point, Point> Ellipse::focuses() const {
  return {first_focus, second_focus};
}

std::pair<Line, Line> Ellipse::directrises() const {
  double focus_distance = distancePoints(first_focus, second_focus);
  double normalise_factor = diameter / (2 * eccentricity());
  Vector central(first_focus, second_focus);
  Point p1(first_focus.x + central.x * (normalise_factor + focus_distance / 2) / focus_distance,
           first_focus.y + central.y * (normalise_factor + focus_distance / 2) / focus_distance);
  Point p2(second_focus.x - central.x * (normalise_factor + focus_distance / 2) / focus_distance,
           second_focus.y - central.y * (normalise_factor + focus_distance / 2) / focus_distance);
  Point p3(p1.x - central.y, p1.y + central.x);
  Point p4(p2.x - central.y, p2.y + central.x);
  return {Line(p1, p3), Line(p2, p4)};
}

double Ellipse::eccentricity() const {
  return distancePoints(first_focus, second_focus) / diameter;
}

Point Ellipse::center() const {
  return Point((first_focus.x + second_focus.x) / 2, (first_focus.y + second_focus.y) / 2);
}

double Ellipse::area() const {
  return M_PI * diameter * sqrt(pow(diameter, 2.0) -
         pow(distancePoints(first_focus, second_focus), 2.0)) / 4;
}

double Ellipse::perimeter() const {
  return 2 * diameter * std::comp_ellint_2(eccentricity());
}

bool Ellipse::equalShape(const Shape& other) const {
  if (dynamic_cast<const Shape*>(&other) == nullptr) {
    return false;
  }
  const Ellipse& other_ellipse = dynamic_cast<const Ellipse&>(other);
  return (((first_focus == other_ellipse.first_focus && second_focus == other_ellipse.second_focus)
          || (second_focus == other_ellipse.first_focus && first_focus == other_ellipse.second_focus))
          && Essentials::are_equal_numbers(diameter, other_ellipse.diameter));
}

bool Ellipse::isCongruentTo(const Shape& other) const {
  if (dynamic_cast<const Ellipse*>(&other) == nullptr) {
    return false;
  }
  const Ellipse& other_ellipse = dynamic_cast<const Ellipse&>(other);
  if (distancePoints(first_focus, second_focus) == distancePoints(other_ellipse.first_focus,
                                                                  other_ellipse.second_focus)
      && Essentials::are_equal_numbers(diameter, other_ellipse.diameter)) {
    return true;
  }
  return false;
}

bool Ellipse::isSimilarTo(const Shape& other) const {
  if (dynamic_cast<const Ellipse*>(&other) == nullptr) {
    return false;
  }
  const Ellipse& other_ellipse = dynamic_cast<const Ellipse&>(other);
  if (first_focus == second_focus) {
    return true;
  }
  if (Essentials::are_equal_numbers(distancePoints(first_focus, second_focus) * other_ellipse.diameter,
            distancePoints(other_ellipse.first_focus, other_ellipse.second_focus) * diameter)) {
    return true;
  }
  return false;
}

bool Ellipse::containsPoint(const Point& point) const {
  if (distancePoints(point, first_focus) + distancePoints(point, second_focus) <= diameter + ACCURACY) {
    return true;
  }
  return false;
}

void Ellipse::rotate(const Point& center, double angle) {
  first_focus.rotate(center, angle);
  second_focus.rotate(center, angle);
}

void Ellipse::reflect(const Point& center) {
  first_focus.reflect(center);
  second_focus.reflect(center);
}

void Ellipse::reflect(const Line& axis) {
  first_focus.reflect(axis);
  second_focus.reflect(axis);
}

void Ellipse::scale(const Point& center, double coefficient) {
  first_focus.scale(center, coefficient);
  second_focus.scale(center, coefficient);
  diameter *= coefficient;
}

class Circle: public Ellipse {
 public:
  Circle(Point center, double radius);
  double radius() const;
  Point center() const;
};

Circle::Circle(Point center, double radius): Ellipse(center, center, 2 * radius) {}

double Circle::radius() const {
  return diameter / 2;
}

Point Circle::center() const {
  return first_focus;
}

class Rectangle: public Polygon {
 public:
  Rectangle() = default;
  Rectangle(const Point& first_point, const Point& second_point, double coefficient);
  Point center() const;
  std::pair<Line, Line> diagonals() const;
};

Rectangle::Rectangle(const Point& first_point, const Point& second_point, double coefficient) {
  coefficient = (coefficient > 1 ? pow(coefficient, -1.0) : coefficient);
  vertices_.resize(4);
  vertices_[0] = first_point;
  vertices_[2] = second_point;
  double cos_angle = (1 - pow(coefficient, 2.0)) / (1 + pow(coefficient, 2.0));
  double sin_angle = (2 * coefficient / (1 + pow(coefficient, 2.0)));
  Vector first_vector(Point((first_point.x + second_point.x) / 2,
                            (first_point.y + second_point.y) / 2), first_point);
  double x1 = (first_point.x + second_point.x) / 2 +
              first_vector.x * cos_angle + first_vector.y * sin_angle;
  double y1 = (first_point.y + second_point.y) / 2 +
              first_vector.y * cos_angle - first_vector.x * sin_angle;
  vertices_[1] = Point(x1, y1);
  Vector second_vector(Point((first_point.x + second_point.x) / 2,
                             (first_point.y + second_point.y) / 2), second_point);
  double x2 = (first_point.x + second_point.x) / 2 +
              second_vector.x * cos_angle + second_vector.y * sin_angle;
  double y2 = (first_point.y + second_point.y) / 2 +
              second_vector.y * cos_angle - second_vector.x * sin_angle;
  vertices_[3] = Point(x2, y2);
}

Point Rectangle::center() const {
  Point first_point = vertices_[0], second_point = vertices_[2];
  return Point((first_point.x + second_point.x) / 2, (first_point.y + second_point.y) / 2);
}

std::pair<Line, Line> Rectangle::diagonals() const {
  return {Line(vertices_[0], vertices_[2]),
          Line(vertices_[1], vertices_[3])};
}

class Square: public Rectangle {
 private:
  double side_() const;
 public:
  Square(Point first_point, Point second_point);
  Circle circumscribedCircle() const;
  Circle inscribedCircle() const;
};

Square::Square(Point first_point, Point second_point) {
  vertices_.resize(4);
  vertices_[0] = first_point;
  vertices_[2] = second_point;
  vertices_[1] = Point((first_point.x + second_point.x + second_point.y - first_point.y) / 2,
                       (first_point.y + second_point.y + first_point.x - second_point.x) / 2);
  vertices_[3] = Point((first_point.x + second_point.x + first_point.y - second_point.y) / 2,
                       (first_point.y + second_point.y + second_point.x - first_point.x) / 2);
}

double Square::side_() const {
  return Vector(vertices_[0], vertices_[1]).length();
}

Circle Square::circumscribedCircle() const {
  return Circle(center(), side_() * sqrt(2) / 2);
}

Circle Square::inscribedCircle() const {
  return Circle(center(), side_() / 2);
}

class Triangle: public Polygon {
 private:
  Point circumcenter_() const;
  Point incenter_() const;
 public:
  Triangle(std::vector<Point>& vertices);
  Triangle(Point first_vertice, Point second_vertice, Point third_vertice);
  Circle circumscribedCircle() const;
  Circle inscribedCircle() const;
  Point centroid() const;
  Point orthocenter() const;
  Line EulerLine() const;
  Circle ninePointsCircle() const;
};

Triangle::Triangle(std::vector<Point>& vertices) {
  assert(vertices.size() == 3);
  vertices_ = vertices;
}

Triangle::Triangle(Point first_vertice, Point second_vertice, Point third_vertice) {
  vertices_.push_back(first_vertice);
  vertices_.push_back(second_vertice);
  vertices_.push_back(third_vertice);
}

Point Triangle::circumcenter_() const {
  Point A = vertices_[0], B = vertices_[1], C = vertices_[2];
  Vector a(B, C), b(C, A), c(A, B);
  std::vector<double> baricentric_coords {
    pow(a.length(), 2.0) * (pow(b.length(), 2.0) + pow(c.length(), 2.0) - pow(a.length(), 2.0)),
    pow(b.length(), 2.0) * (pow(c.length(), 2.0) + pow(a.length(), 2.0) - pow(b.length(), 2.0)),
    pow(c.length(), 2.0) * (pow(a.length(), 2.0) + pow(b.length(), 2.0) - pow(c.length(), 2.0))
  };
  double norming_factor = baricentric_coords[0] + baricentric_coords[1] + baricentric_coords[2];
  for (size_t i = 0; i < 3; ++i) {
    baricentric_coords[i] /= norming_factor;
  }
  double center_x = baricentric_coords[0] * A.x +
                    baricentric_coords[1] * B.x +
                    baricentric_coords[2] * C.x;
  double center_y = baricentric_coords[0] * A.y +
                    baricentric_coords[1] * B.y +
                    baricentric_coords[2] * C.y;
  return Point(center_x, center_y);
}

Circle Triangle::circumscribedCircle() const {
  Point center = circumcenter_();
  Point incenter = incenter_();
  double inradius = inscribedCircle().radius();
  double radius = inradius + sqrt(pow(inradius, 2.0) + pow(distancePoints(center, incenter), 2.0));
  return Circle(center, radius);
}

Point Triangle::incenter_() const {
  Point A = vertices_[0], B = vertices_[1], C = vertices_[2];
  Vector a(B, C);
  Vector b(C, A);
  Vector c(A, B);
  std::vector<double> baricentric_coords {a.length(), b.length(), c.length()};
  double norming_factor = baricentric_coords[0] + baricentric_coords[1] + baricentric_coords[2];
  for (size_t i = 0; i < 3; ++i) {
    baricentric_coords[i] /= norming_factor;
  }
  double center_x = baricentric_coords[0] * A.x +
                    baricentric_coords[1] * B.x +
                    baricentric_coords[2] * C.x;
  double center_y = baricentric_coords[0] * A.y +
                    baricentric_coords[1] * B.y +
                    baricentric_coords[2] * C.y;
  return Point(center_x, center_y);
}

Circle Triangle::inscribedCircle() const {
  Point center = incenter_();
  double radius = 2.0 * area() / perimeter();
  return Circle(center, radius);
}

Point Triangle::centroid() const {
  double x = 0, y = 0;
  for (size_t i = 0; i < 3; ++i) {
    x += vertices_[i].x / 3;
    y += vertices_[i].y / 3;
  }
  return Point(x, y);
}

Point Triangle::orthocenter() const {
  Point A = vertices_[0], B = vertices_[1], C = vertices_[2];
  Line perpenicular_A = perpendicular(A, Line(B, C));
  Line perpenicular_B = perpendicular(B, Line(C, A));
  return intersection(perpenicular_A, perpenicular_B);
}

Line Triangle::EulerLine() const {
  return Line(orthocenter(), centroid());
}

Circle Triangle::ninePointsCircle() const {
  Point orthocenter_point = orthocenter();
  Point circumcenter_point = circumcenter_();
  Point point1((orthocenter_point.x + circumcenter_point.x) / 2, (orthocenter_point.y + circumcenter_point.y) / 2);
  Point point2((vertices_[0].x + vertices_[1].x) / 2,
               (vertices_[0].y + vertices_[1].y) / 2);
  return Circle(point1, distancePoints(point1, point2));
}
// Copyright (c) 2024 Michael Thoma
// This program and the accompanying materials are made available under the
// terms of the MIT License, which is available in the project root.
#include "point.hpp"

#include <cmath>

double Distance(const Point& point1, const Point& point2) {
  return std::sqrt((point1.x - point2.x) * (point1.x - point2.x) +
                   (point1.y - point2.y) * (point1.y - point2.y));
}

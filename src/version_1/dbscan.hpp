// Copyright (c) 2024 Michael Thoma
// This program and the accompanying materials are made available under the
// terms of the MIT License, which is available in the project root.
#pragma once

#include <string>
#include <vector>

#include "point.hpp"

enum class Type {
  kNotVisited,
  kCore,
  kReachable,
  kNoise,
};

std::string ToString(Type type);

constexpr size_t kNoCluster = 0;

struct DbscanResult {
  size_t cluster{kNoCluster};
  Type type{Type::kNotVisited};
};

std::vector<DbscanResult> Dbscan(const std::vector<Point>& points, double eps,
                                 size_t min_points);

// Copyright (c) 2024 Michael Thoma
// This program and the accompanying materials are made available under the
// terms of the MIT License, which is available in the project root.
#pragma once

#include <vector>

#include "point.hpp"

constexpr size_t kNoCluster = 0;

std::vector<size_t> Dbscan(const std::vector<Point>& points, double eps,
                           size_t min_points);

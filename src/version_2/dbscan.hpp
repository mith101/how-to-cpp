// Copyright (c) 2024 Michael Thoma
// This program and the accompanying materials are made available under the
// terms of the MIT License, which is available in the project root.
#pragma once

#include <list>
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

template <typename T>
concept Measurable = requires(const T& x, const T& y) {
  { Distance(x, y) } -> std::same_as<double>;
};

template <Measurable T>
std::vector<size_t> RegionQuery(const std::vector<T>& points,
                                const T& point_under_test, double eps) {
  std::vector<size_t> neighbours;
  size_t index = 0;
  for (const auto& point : points) {
    if (Distance(point_under_test, point) < eps) {
      neighbours.emplace_back(index);
    }
    ++index;
  }
  return neighbours;
}

template <Measurable T>
void ExpandCluster(const std::vector<T>& points, size_t index_under_test,
                   size_t current_cluster,
                   const std::vector<size_t>& neighbours,
                   std::vector<DbscanResult>& result, double eps,
                   size_t min_points) {
  result.at(index_under_test).cluster = current_cluster;
  std::list<std::vector<size_t>> additional_neighbours;
  std::list<std::reference_wrapper<const std::vector<size_t>>> all_neighbours;
  all_neighbours.emplace_back(std::cref(neighbours));
  for (const auto& neighbours : all_neighbours) {
    for (auto index_prime : neighbours.get()) {
      if (result.at(index_prime).type == Type::kNotVisited) {
        result.at(index_prime).type = Type::kReachable;
        const auto neighbours_prime =
            RegionQuery(points, points.at(index_prime), eps);
        if (min_points <= neighbours_prime.size()) {
          result.at(index_prime).type = Type::kCore;
          additional_neighbours.emplace_back(std::move(neighbours_prime));
          all_neighbours.emplace_back(std::cref(additional_neighbours.back()));
        }
      }
      if (result.at(index_prime).cluster == kNoCluster) {
        result.at(index_prime).cluster = current_cluster;
      }
    }
  }
}

template <Measurable T>
std::vector<DbscanResult> Dbscan(const std::vector<T>& points, double eps,
                                 size_t min_points) {
  std::vector<DbscanResult> result(points.size());
  size_t current_cluster = kNoCluster;
  size_t index = 0;
  for (const auto& point : points) {
    if (result.at(index).type == Type::kNotVisited) {
      result.at(index).type = Type::kNoise;
      const auto neighbours = RegionQuery(points, point, eps);
      if (min_points <= neighbours.size()) {
        result.at(index).type = Type::kCore;
        ++current_cluster;
        ExpandCluster(points, index, current_cluster, neighbours, result, eps,
                      min_points);
      }
    }
    ++index;
  }
  return result;
}

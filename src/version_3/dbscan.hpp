// Copyright (c) 2024 Michael Thoma
// This program and the accompanying materials are made available under the
// terms of the MIT License, which is available in the project root.
#pragma once

#include <functional>
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

template <typename F, typename T>
concept NeighbourhoodFunction =
    requires(F func, const std::vector<T>& data, const T& point) {
      { func(data, point) } -> std::same_as<std::vector<size_t>>;
    };

template <typename F, typename T>
concept IsCoreFunction = std::predicate<F, const T&, const std::vector<size_t>&,
                                        const std::vector<T>&>;

template <typename T, NeighbourhoodFunction<T> GetNeighbours,
          IsCoreFunction<T> IsCore>
void ExpandCluster(const std::vector<T>& points, size_t index_under_test,
                   size_t current_cluster,
                   const std::vector<size_t>& neighbours,
                   std::vector<DbscanResult>& result,
                   GetNeighbours get_neighbours, IsCore is_core) {
  result.at(index_under_test).cluster = current_cluster;
  std::list<std::vector<size_t>> additional_neighbours;
  std::list<std::reference_wrapper<const std::vector<size_t>>> all_neighbours;
  all_neighbours.emplace_back(std::cref(neighbours));
  for (const auto& neighbours : all_neighbours) {
    for (auto index_prime : neighbours.get()) {
      if (result.at(index_prime).type == Type::kNotVisited) {
        result.at(index_prime).type = Type::kReachable;
        const auto neighbours_prime =
            std::invoke(get_neighbours, points, points.at(index_prime));
        if (std::invoke(is_core, points, points.at(index_prime),
                        neighbours_prime)) {
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

template <typename T, NeighbourhoodFunction<T> GetNeighbours,
          IsCoreFunction<T> IsCore>
std::vector<DbscanResult> Dbscan(const std::vector<T>& points,
                                 GetNeighbours get_neighbours, IsCore is_core) {
  std::vector<DbscanResult> result(points.size());
  size_t current_cluster = kNoCluster;
  size_t index = 0;
  for (const auto& point : points) {
    if (result.at(index).type == Type::kNotVisited) {
      result.at(index).type = Type::kNoise;
      const auto neighbours = std::invoke(get_neighbours, points, point);
      if (std::invoke(is_core, points, point, neighbours)) {
        result.at(index).type = Type::kCore;
        ++current_cluster;
        ExpandCluster(points, index, current_cluster, neighbours, result,
                      get_neighbours, is_core);
      }
    }
    ++index;
  }
  return result;
}

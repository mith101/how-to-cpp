// Copyright (c) 2024 Michael Thoma
// This program and the accompanying materials are made available under the
// terms of the MIT License, which is available in the project root.
#include "dbscan.hpp"

#include <functional>
#include <list>
#include <vector>

std::vector<size_t> RegionQuery(const std::vector<Point>& points,
                                const Point& point_under_test, double eps) {
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

enum class Type {
  kNotVisited,
  kCore,
  kReachable,
  kNoise,
};

void ExpandCluster(const std::vector<Point>& points, size_t index_under_test,
                   size_t current_cluster,
                   const std::vector<size_t>& neighbours,
                   std::vector<size_t>& clusters, std::vector<Type>& types,
                   double eps, size_t min_points) {
  clusters.at(index_under_test) = current_cluster;
  std::list<std::vector<size_t>> additional_neighbours;
  std::list<std::reference_wrapper<const std::vector<size_t>>> all_neighbours;
  all_neighbours.emplace_back(std::cref(neighbours));
  for (const auto& neighbours : all_neighbours) {
    for (auto index_prime : neighbours.get()) {
      if (types.at(index_prime) == Type::kNotVisited) {
        types.at(index_prime) = Type::kReachable;
        const auto neighbours_prime =
            RegionQuery(points, points.at(index_prime), eps);
        if (min_points <= neighbours_prime.size()) {
          types.at(index_prime) = Type::kCore;
          additional_neighbours.emplace_back(std::move(neighbours_prime));
          all_neighbours.emplace_back(std::cref(additional_neighbours.back()));
        }
      }
      if (clusters.at(index_prime) == kNoCluster) {
        clusters.at(index_prime) = current_cluster;
      }
    }
  }
}

std::vector<size_t> Dbscan(const std::vector<Point>& points, double eps,
                           size_t min_points) {
  std::vector<Type> types(points.size(), Type::kNotVisited);
  std::vector<size_t> clusters(points.size(), kNoCluster);
  size_t current_cluster = kNoCluster;
  size_t index = 0;
  for (const auto& point : points) {
    if (types.at(index) == Type::kNotVisited) {
      types.at(index) = Type::kNoise;
      const auto neighbours = RegionQuery(points, point, eps);
      if (min_points <= neighbours.size()) {
        types.at(index) = Type::kCore;
        ++current_cluster;
        ExpandCluster(points, index, current_cluster, neighbours, clusters,
                      types, eps, min_points);
      }
    }
    ++index;
  }
  return clusters;
}

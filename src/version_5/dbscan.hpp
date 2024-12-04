// Copyright (c) 2024 Michael Thoma
// This program and the accompanying materials are made available under the
// terms of the MIT License, which is available in the project root.
#pragma once

#include <functional>
#include <iterator>
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

template <std::input_or_output_iterator Iterator>
struct DbscanResult {
  Iterator iter{};
  size_t cluster{kNoCluster};
  Type type{Type::kNotVisited};
};

template <typename F, typename I>
concept NeighbourhoodFunction =
    requires(F func, I& data, const std::ranges::const_iterator_t<I>& point) {
      {
        func(data, point)
      } -> std::same_as<std::vector<decltype(std::cbegin(data))>>;
    };

template <typename F, typename I>
concept IsCoreFunction =
    std::predicate<F, I&, std::ranges::const_iterator_t<I>,
                   const std::vector<std::ranges::const_iterator_t<I>>&>;

template <typename C>
concept InputContainer =
    std::ranges::constant_range<C> && std::ranges::forward_range<C>;

template <typename C, typename T>
concept OutputContainer = std::ranges::forward_range<C> && requires(C& c) {
  { std::back_inserter(c) } -> std::output_iterator<T>;
  { c.back() } -> std::same_as<T&>;
};

template <typename Range, typename Iter>
bool Contains(const Range& range, Iter iter) {
  return std::ranges::find_if(range, [iter](auto x) {
           return x.iter == iter;
         }) != std::end(range);
}

void ExpandCluster(auto& points, size_t current_cluster, auto& neighbours,
                   auto& result, auto get_neighbours, auto is_core) {
  using N_ = std::remove_cvref_t<decltype(neighbours)>;
  auto back_inserter = std::back_inserter(result);
  result.back().cluster = current_cluster;
  std::list<N_> additional_neighbours;
  std::list<std::reference_wrapper<const N_>> all_neighbours;
  all_neighbours.emplace_back(std::cref(neighbours));
  for (const auto& neighbours : all_neighbours) {
    for (auto iter_prime : neighbours.get()) {
      if (!Contains(result, iter_prime)) {
        back_inserter = DbscanResult{iter_prime, kNoCluster, Type::kReachable};
        const auto neighbours_prime =
            std::invoke(get_neighbours, points, iter_prime);
        if (std::invoke(is_core, points, iter_prime, neighbours_prime)) {
          result.back().type = Type::kCore;
          additional_neighbours.emplace_back(std::move(neighbours_prime));
          all_neighbours.emplace_back(std::cref(additional_neighbours.back()));
        }
      }
      if (result.back().cluster == kNoCluster) {
        result.back().cluster = current_cluster;
      }
    }
  }
}

template <typename Input, typename Output, typename GetNeighbours,
          typename IsCore, typename I_ = std::ranges::const_iterator_t<Input>>
requires(InputContainer<Input> && OutputContainer<Output, DbscanResult<I_>> &&
         NeighbourhoodFunction<GetNeighbours, Input> &&
         IsCoreFunction<IsCore, Input>)
void Dbscan(Input& points, Output& result, GetNeighbours get_neighbours,
            IsCore is_core) {
  auto back_inserter = std::back_inserter(result);
  size_t current_cluster = kNoCluster;
  for (auto iter = std::cbegin(points); iter != std::cend(points); ++iter) {
    if (!Contains(result, iter)) {
      back_inserter = DbscanResult{iter, kNoCluster, Type::kNoise};
      const auto neighbours = std::invoke(get_neighbours, points, iter);
      if (std::invoke(is_core, points, iter, neighbours)) {
        result.back().type = Type::kCore;
        ++current_cluster;
        ExpandCluster(points, current_cluster, neighbours, result,
                      get_neighbours, is_core);
      }
    }
  }
}

template <template <typename> typename Container, typename Input,
          typename I_ = std::ranges::const_iterator_t<Input>,
          typename R_ = DbscanResult<I_>, typename... Args>
requires(InputContainer<Input> && OutputContainer<Container<R_>, R_>)
auto DbscanOutputContainer([[maybe_unused]] Input& points, Args... args) {
  return Container<R_>(std::forward(args)...);
}

// Copyright (c) 2024 Michael Thoma
// This program and the accompanying materials are made available under the
// terms of the MIT License, which is available in the project root.
#include "dbscan.hpp"

std::string ToString(Type type) {
  switch (type) {
    case Type::kNotVisited:
      return "NotVisited";
    case Type::kCore:
      return "Core";
    case Type::kReachable:
      return "Reachable";
    case Type::kNoise:
      return "Noise";
  }
  return "[ERROR]";
};

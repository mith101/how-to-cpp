// Copyright (c) 2024 Michael Thoma
// This program and the accompanying materials are made available under the
// terms of the MIT License, which is available in the project root.
#pragma once

struct Point2d {
  double x;
  double y;
};

double Distance(const Point2d& point1, const Point2d& point2);

struct Point3d {
  double x;
  double y;
  double z;
};

double Distance(const Point3d& point1, const Point3d& point2);

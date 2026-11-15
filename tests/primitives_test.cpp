#include "visual_integrator/visualization/primitives.hh"

#include <gtest/gtest.h>

#include <glm/geometric.hpp>
#include <stdexcept>

namespace visual_integrator
{
TEST(TriangelMesh, AddsTriangleWithOwnedVerticesInOrder)
{
  TriangelMesh mesh({}, {});
  mesh.add_triangle({1, 0, 0}, {0, 1, 0}, {0, 0, 1});
  ASSERT_EQ(mesh.triangles().size(), 1);
  const auto& indices = mesh.triangles().front().vertics;
  EXPECT_EQ(mesh.vertices().at(indices[0]), glm::vec3(1, 0, 0));
  EXPECT_EQ(mesh.vertices().at(indices[1]), glm::vec3(0, 1, 0));
  EXPECT_EQ(mesh.vertices().at(indices[2]), glm::vec3(0, 0, 1));
}
}  // namespace visual_integrator

#include "visual_integrator/visualization/marching_cubes.hh"

namespace visual_integrator
{
TEST(AreaCuting, CutsCubeIntoIndependentClosedSlices)
{
  std::vector<glm::vec3> v{{0, 0, 0}, {2, 0, 0}, {2, 2, 0}, {0, 2, 0},
                           {0, 0, 2}, {2, 0, 2}, {2, 2, 2}, {0, 2, 2}};
  std::vector<Triangel> t{{{0, 2, 1}}, {{0, 3, 2}}, {{4, 5, 6}}, {{4, 6, 7}},
                          {{0, 1, 5}}, {{0, 5, 4}}, {{3, 7, 6}}, {{3, 6, 2}},
                          {{0, 4, 7}}, {{0, 7, 3}}, {{1, 2, 6}}, {{1, 6, 5}}};
  TriangelMesh cube(v, t);
  ParserFunc parser;
  parser.parserF.SetExpr("1");
  Grid grid({1, 1, 1}, {0, 0, 0}, {2, 2, 2}, parser);
  const auto slices = AreaCuting(cube, grid, k_x);
  ASSERT_EQ(slices.size(), 2);
  Grid partial_grid({1, 1, 1}, {0.5, 0, 0}, {1, 2, 2}, parser);
  const auto partial = AreaCuting(cube, partial_grid, k_x);
  ASSERT_EQ(partial.size(), 1);
  for (const auto& point : partial[0].vertices()) {
    EXPECT_GE(point.x, 0.5f);
    EXPECT_LE(point.x, 1.5f);
  }
  for (std::size_t i = 0; i < slices.size(); ++i)
  {
    EXPECT_FALSE(slices[i].triangles().empty());
    bool cap = false;
    for (const auto& tri : slices[i].triangles())
    {
      const auto& p = slices[i].vertices();
      for (auto n : tri.vertics)
      {
        EXPECT_GE(p.at(n).x, static_cast<float>(i));
        EXPECT_LE(p.at(n).x, static_cast<float>(i + 1));
      }
      cap |= p[tri.vertics[0]].x == 1 && p[tri.vertics[1]].x == 1 && p[tri.vertics[2]].x == 1;
    }
    EXPECT_TRUE(cap);
    const auto recut = AreaCuting(slices[i], grid, k_x);
    EXPECT_EQ(recut.size(), 1);
  }
}
}  // namespace visual_integrator

namespace visual_integrator
{
TEST(AreaCuting, RejectsInvalidAxis)
{
  TriangelMesh mesh({}, {});
  ParserFunc parser;
  parser.parserF.SetExpr("1");
  Grid grid({1, 1, 1}, {0, 0, 0}, {2, 2, 2}, parser);
  EXPECT_THROW(AreaCuting(mesh, grid, static_cast<Axis>(3)), std::invalid_argument);
}
TEST(AreaCuting, CapsHollowBoxForEveryAxis)
{
  std::vector<glm::vec3> v;
  std::vector<Triangel> t;
  auto box = [&](float low, float high, bool reverse)
  {
    std::size_t n = v.size();
    v.insert(v.end(), {{low, low, low},
                       {high, low, low},
                       {high, high, low},
                       {low, high, low},
                       {low, low, high},
                       {high, low, high},
                       {high, high, high},
                       {low, high, high}});
    std::array<std::array<std::size_t, 3>, 12> faces{{{{0, 2, 1}},
                                                      {{0, 3, 2}},
                                                      {{4, 5, 6}},
                                                      {{4, 6, 7}},
                                                      {{0, 1, 5}},
                                                      {{0, 5, 4}},
                                                      {{3, 7, 6}},
                                                      {{3, 6, 2}},
                                                      {{0, 4, 7}},
                                                      {{0, 7, 3}},
                                                      {{1, 2, 6}},
                                                      {{1, 6, 5}}}};
    for (auto f : faces)
    {
      if (reverse) std::swap(f[1], f[2]);
      t.push_back({{n + f[0], n + f[1], n + f[2]}});
    }
  };
  box(0, 4, false);
  box(1, 3, true);
  TriangelMesh mesh(v, t);
  ParserFunc parser;
  parser.parserF.SetExpr("1");
  Grid grid({2, 2, 2}, {0, 0, 0}, {4, 4, 4}, parser);
  for (auto axis : {k_x, k_y, k_z})
  {
    auto slices = AreaCuting(mesh, grid, axis);
    ASSERT_EQ(slices.size(), 2);
    for (const auto& slice : slices)
    {
      double cap_area = 0;
      for (const auto& tri : slice.triangles())
      {
        auto a = slice.vertices()[tri.vertics[0]];
        auto b = slice.vertices()[tri.vertics[1]];
        auto c = slice.vertices()[tri.vertics[2]];
        if (a[axis] == 2 && b[axis] == 2 && c[axis] == 2)
          cap_area += glm::length(glm::cross(b - a, c - a)) / 2;
      }
      EXPECT_NEAR(cap_area, 12, 1e-5);
      EXPECT_EQ(AreaCuting(slice, grid, axis).size(), 1);
    }
  }
}
}  // namespace visual_integrator

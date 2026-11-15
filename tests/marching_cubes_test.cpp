#include "visual_integrator/visualization/marching_cubes.hh"

#include <gtest/gtest.h>

#include <glm/geometric.hpp>
#include <limits>
#include <stdexcept>

namespace visual_integrator
{
namespace
{
struct Field : ParserFunc
{
  explicit Field(const char* expression) { parserF.SetExpr(expression); }
};
Field field(const char* expression) { return Field(expression); }
}  // namespace
TEST(Grid, SamplesRectangularAreaIncludingLastWholeStep)
{
  auto parser = field("x + 10*y + 100*z");
  Grid grid({2, 3, 4}, {10, 20, 30}, {5, 3, 0}, parser);
  EXPECT_EQ(grid.dimensions, glm::uvec3(3, 2, 1));
  EXPECT_EQ(grid.valuse, (std::vector<double>{3210, 3240, 3212, 3242, 3214, 3244}));
  EXPECT_EQ(grid.locations.size(), 6);
}
TEST(MarchingCubes, UniformSamplesProduceNoTriangles)
{
  for (double value : {-1.0, 1.0})
  {
    auto parser = field(value < 0 ? "-1" : "1");
    Grid grid({1, 1, 1}, {0, 0, 0}, {1, 1, 1}, parser);
    auto mesh = polygonize(grid);
    EXPECT_TRUE(mesh.vertices().empty());
    EXPECT_TRUE(mesh.triangles().empty());
  }
}
TEST(MarchingCubes, InterpolatesSingleCornerInTableOrder)
{
  auto parser = field("(x==10 && y==20 && z==30) ? -1 : 3");
  Grid grid({2, 4, 6}, {10, 20, 30}, {2, 4, 6}, parser);
  auto mesh = polygonize(grid);
  ASSERT_EQ(mesh.triangles().size(), 1);
  const auto& triangle = mesh.triangles()[0];
  EXPECT_EQ(mesh.vertices().at(triangle.vertics[0]), glm::vec3(10.5, 20, 30));
  EXPECT_EQ(mesh.vertices().at(triangle.vertics[1]), glm::vec3(10, 21, 30));
  EXPECT_EQ(mesh.vertices().at(triangle.vertics[2]), glm::vec3(10, 20, 31.5));
}
TEST(Grid, RejectsInvalidGeometryAndSamples)
{
  auto parser = field("1");
  EXPECT_THROW(Grid(glm::dvec3(-1), glm::dvec3(0), glm::dvec3(1), parser), std::invalid_argument);
  EXPECT_THROW(Grid(glm::dvec3(1), glm::dvec3(0), glm::dvec3(-1), parser), std::invalid_argument);
  auto nonfinite = field("1e308*1e308");
  EXPECT_THROW(Grid(glm::dvec3(1), glm::dvec3(0), glm::dvec3(1), nonfinite), std::invalid_argument);
}
TEST(MarchingCubes, RejectsInvalidIsolevelAndModifiedSampleStorage)
{
  auto parser = field("1");
  Grid grid({1, 1, 1}, {0, 0, 0}, {1, 1, 1}, parser);
  EXPECT_THROW(polygonize(grid, std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
  grid.valuse.pop_back();
  EXPECT_THROW(polygonize(grid), std::invalid_argument);
}
TEST(MarchingCubes, PreservesOppositeWindingForComplementaryCorner)
{
  auto parser = field("(x==0 && y==0 && z==0) ? 1 : -1");
  Grid grid({1, 1, 1}, {0, 0, 0}, {1, 1, 1}, parser);
  const auto mesh = polygonize(grid);
  ASSERT_EQ(mesh.triangles().size(), 1);
  const auto& t = mesh.triangles()[0].vertics;
  EXPECT_EQ(mesh.vertices().at(t[0]), glm::vec3(0.5, 0, 0));
  EXPECT_EQ(mesh.vertices().at(t[1]), glm::vec3(0, 0, 0.5));
  EXPECT_EQ(mesh.vertices().at(t[2]), glm::vec3(0, 0.5, 0));
}
TEST(MarchingCubes, ExtractsPlaneAcrossRectangularGridWithLargeVertexIndices)
{
  auto parser = field("x");
  Grid grid({2, 3, 4}, {10, 20, 30}, {2, 30, 80}, parser);
  const auto mesh = polygonize(grid, 10.5);
  ASSERT_EQ(mesh.triangles().size(), 400);
  ASSERT_GT(mesh.vertices().size(), 256);
  bool uses_large_index = false;
  for (const auto& triangle : mesh.triangles())
  {
    for (auto index : triangle.vertics)
    {
      ASSERT_LT(index, mesh.vertices().size());
      const auto p = mesh.vertices()[index];
      EXPECT_FLOAT_EQ(p.x, 10.5f);
      EXPECT_GE(p.y, 20);
      EXPECT_LE(p.y, 50);
      EXPECT_GE(p.z, 30);
      EXPECT_LE(p.z, 110);
      uses_large_index |= index > 255;
    }
    const auto& t = triangle.vertics;
    const auto normal = glm::cross(mesh.vertices()[t[1]] - mesh.vertices()[t[0]],
                                   mesh.vertices()[t[2]] - mesh.vertices()[t[0]]);
    EXPECT_GT(normal.x, 0);
    EXPECT_FLOAT_EQ(normal.y, 0);
    EXPECT_FLOAT_EQ(normal.z, 0);
  }
  EXPECT_TRUE(uses_large_index);
}
TEST(MarchingCubes, SurfaceOnGridBoundaryUsesExactSamplePositions)
{
  auto parser = field("x-1");
  Grid grid({1, 1, 1}, {0, 0, 0}, {1, 1, 1}, parser);
  const auto mesh = polygonize(grid);
  ASSERT_EQ(mesh.triangles().size(), 2);
  for (const auto& p : mesh.vertices()) EXPECT_FLOAT_EQ(p.x, 1);
}
TEST(MarchingCubes, GridWithOnlyOneSampleAlongAnAxisHasNoCubes)
{
  auto parser = field("y-0.5");
  Grid grid({1, 1, 1}, {0, 0, 0}, {0, 2, 3}, parser);
  EXPECT_TRUE(polygonize(grid).triangles().empty());
}
TEST(MarchingCubes, InterpolatesLargeFiniteSamplesWithoutOverflow)
{
  auto parser = field("x==0 ? -1e308 : 1e308");
  Grid grid({1, 1, 1}, {0, 0, 0}, {1, 1, 1}, parser);
  for (const auto& sample :
       std::array<std::pair<double, float>, 3>{{{0, 0.5f}, {5e307, 0.75f}, {-5e307, 0.25f}}})
  {
    const auto mesh = polygonize(grid, sample.first);
    ASSERT_EQ(mesh.triangles().size(), 2);
    ASSERT_FALSE(mesh.vertices().empty());
    for (const auto& p : mesh.vertices()) EXPECT_FLOAT_EQ(p.x, sample.second);
  }
}
}  // namespace visual_integrator

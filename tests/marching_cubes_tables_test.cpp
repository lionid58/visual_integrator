#include "visual_integrator/visualization/marching_cubes_tables.hh"

#include <gtest/gtest.h>

namespace visual_integrator::marching_cubes {
TEST(MarchingCubesTables, UsesTressFxCornerAndEdgeNumbering) {
  constexpr std::array<std::array<std::uint8_t, 3>, 8> expected_corners{
      {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}, {0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1}}};
  constexpr std::array<std::array<std::uint8_t, 2>, 12> expected_edges{{{0, 1},
                                                                        {1, 2},
                                                                        {2, 3},
                                                                        {3, 0},
                                                                        {4, 5},
                                                                        {5, 6},
                                                                        {6, 7},
                                                                        {7, 4},
                                                                        {0, 4},
                                                                        {1, 5},
                                                                        {2, 6},
                                                                        {3, 7}}};
  EXPECT_EQ(CORNER_OFFSETS, expected_corners);
  EXPECT_EQ(EDGE_CORNERS, expected_edges);
}
TEST(MarchingCubesTables, EdgeMasksSelectExactlyTheEdgesBetweenDifferentOccupancies) {
  ASSERT_EQ(EDGE_MASKS.size(), 256u);
  for (unsigned configuration = 0; configuration < 256; ++configuration) {
    SCOPED_TRACE(configuration);
    EXPECT_EQ(EDGE_MASKS[configuration] & 0xf000, 0);
    for (unsigned edge = 0; edge < 12; ++edge) {
      const auto endpoints = EDGE_CORNERS[edge];
      const bool first_filled = (configuration & (1u << endpoints[0])) != 0;
      const bool second_filled = (configuration & (1u << endpoints[1])) != 0;
      EXPECT_EQ((EDGE_MASKS[configuration] & (1u << edge)) != 0, first_filled != second_filled);
    }
  }
}
TEST(MarchingCubesTables, TriangulationsUseCrossingEdgesAndTerminateAfterCompleteTriangles) {
  ASSERT_EQ(TRIANGLE_EDGES.size(), 256u);
  for (unsigned configuration = 0; configuration < 256; ++configuration) {
    SCOPED_TRACE(configuration);
    const auto& row = TRIANGLE_EDGES[configuration];
    ASSERT_EQ(row.size(), 16u);
    unsigned count = 0;
    unsigned used_edges = 0;
    bool terminated = false;
    for (const auto edge : row) {
      if (edge == -1) {
        terminated = true;
        continue;
      }
      ASSERT_FALSE(terminated);
      ASSERT_GE(edge, 0);
      ASSERT_LT(edge, 12);
      used_edges |= 1u << edge;
      ++count;
    }
    EXPECT_TRUE(terminated);
    EXPECT_LE(count, 15u);
    EXPECT_EQ(count % 3, 0u);
    EXPECT_EQ(used_edges, EDGE_MASKS[configuration]);
    for (unsigned i = 0; i < count; i += 3) {
      EXPECT_NE(row[i], row[i + 1]);
      EXPECT_NE(row[i], row[i + 2]);
      EXPECT_NE(row[i + 1], row[i + 2]);
    }
  }
  // Literal reference cases from TressFX; preserve the original winding.
  const std::array<std::int8_t, 16> empty{-1, -1, -1, -1, -1, -1, -1, -1,
                                          -1, -1, -1, -1, -1, -1, -1, -1};
  const std::array<std::int8_t, 16> one{0,  8,  3,  -1, -1, -1, -1, -1,
                                        -1, -1, -1, -1, -1, -1, -1, -1};
  const std::array<std::int8_t, 16> opposite{0,  3,  8,  -1, -1, -1, -1, -1,
                                             -1, -1, -1, -1, -1, -1, -1, -1};
  const std::array<std::int8_t, 16> five{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1};
  EXPECT_EQ(TRIANGLE_EDGES[0], empty);
  EXPECT_EQ(TRIANGLE_EDGES[255], empty);
  EXPECT_EQ(TRIANGLE_EDGES[1], one);
  EXPECT_EQ(TRIANGLE_EDGES[254], opposite);
  EXPECT_EQ(TRIANGLE_EDGES[61], five);
}
}  // namespace visual_integrator::marching_cubes

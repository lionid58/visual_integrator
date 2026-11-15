#include "visual_integrator/visualization/area_pipeline.h"

#include <gtest/gtest.h>

namespace visual_integrator
{
TEST(AreaPipeline, FilledBoxProducesClosedSlices)
{
  AreaRequest request{"-1", {-1, -1, -1}, {1, 1, 1}, 4, k_x};
  const auto result = build_area(request);
  ASSERT_EQ(result.slices.size(), 4);
  for (const auto& slice : result.slices) EXPECT_FALSE(slice.triangles().empty());
}
TEST(AreaPipeline, SphereAndClippedRegionProduceSlices)
{
  for (const auto* expression : {"x*x+y*y+z*z-0.5", "x-0.2"})
  {
    AreaRequest request{expression, {-1, -1, -1}, {1, 1, 1}, 12, k_x};
    const auto result = build_area(request);
    EXPECT_FALSE(result.slices.empty()) << expression;
  }
}
TEST(AreaPipeline, EmptyAndInvalidInputAreReported)
{
  AreaRequest request{"1", {-1, -1, -1}, {1, 1, 1}, 4, k_z};
  EXPECT_TRUE(build_area(request).slices.empty());
  request.min.x = 1;
  EXPECT_THROW(build_area(request), std::invalid_argument);
  request.min.x = -1;
  request.expression = "bad_variable";
  EXPECT_ANY_THROW(build_area(request));
}
}  // namespace visual_integrator

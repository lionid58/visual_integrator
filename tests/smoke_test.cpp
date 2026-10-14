#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <gtest/gtest.h>
#include <imgui.h>
#include <muParser.h>

#include <Eigen/Core>
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

TEST(Dependencies, EigenPerformsMatrixMultiplication) {
  const Eigen::Matrix2d matrix{{1.0, 2.0}, {3.0, 4.0}};
  const Eigen::Vector2d vector{2.0, 1.0};

  EXPECT_TRUE((matrix * vector).isApprox(Eigen::Vector2d{4.0, 10.0}));
}

TEST(Dependencies, GladLoaderCanBeCalled) {
  const int loaded = gladLoadGLLoader([](const char*) -> void* { return nullptr; });

  EXPECT_EQ(loaded, 0);
}

TEST(Dependencies, GlfwReportsItsVersion) {
  int major = 0;
  int minor = 0;
  int revision = 0;

  glfwGetVersion(&major, &minor, &revision);

  EXPECT_GT(major, 0);
  EXPECT_GE(minor, 0);
  EXPECT_GE(revision, 0);
}

TEST(Dependencies, GlmCalculatesVectorLength) {
  const glm::dvec3 vector{3.0, 4.0, 0.0};

  EXPECT_DOUBLE_EQ(glm::length(vector), 5.0);
}

TEST(Dependencies, ImGuiCreatesContext) {
  ImGuiContext* context = ImGui::CreateContext();

  ASSERT_NE(context, nullptr);
  EXPECT_EQ(ImGui::GetCurrentContext(), context);

  ImGui::DestroyContext(context);
}

TEST(Dependencies, MuParserEvaluatesExpression) {
  mu::Parser parser;
  parser.SetExpr("2 + 3 * 4");

  EXPECT_DOUBLE_EQ(parser.Eval(), 14.0);
}

TEST(TestInfrastructure, GoogleTestIsAvailable) { SUCCEED(); }

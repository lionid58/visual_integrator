#include "visual_integrator/visualization/primitives.hh"

#include <algorithm>
#include <cmath>
#include <glm/geometric.hpp>
#include <limits>
#include <map>
#include <mapbox/earcut.hpp>
#include <ranges>
#include <stdexcept>
#include <utility>

#include "visual_integrator/visualization/marching_cubes.hh"

namespace visual_integrator {
TriangelMesh::TriangelMesh(std::vector<glm::vec3> vertics_,
                           std::vector<Triangel> triangels_)
    : vertics(std::move(vertics_)), triangels(std::move(triangels_)) {}

void TriangelMesh::add_triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
  const auto start = vertics.size();
  vertics.insert(vertics.end(), {a, b, c});
  triangels.push_back({{start, start + 1, start + 2}});
}

namespace {
using Point = glm::dvec3;
using Key = std::array<long long, 3>;
using Edge = std::pair<Key, Key>;
using Ring = std::vector<std::array<double, 2>>;

Key key(Point p, double eps) {
  return {std::llround(p.x / eps), std::llround(p.y / eps),
          std::llround(p.z / eps)};
}
Edge edge(Key a, Key b) { return a < b ? Edge{a, b} : Edge{b, a}; }

std::vector<Point> clip(const std::vector<Point>& input, int axis, double bound,
                        bool lower) {
  std::vector<Point> out;
  if (input.empty()) return out;
  auto inside = [=](Point p) {
    return lower ? p[axis] >= bound : p[axis] <= bound;
  };
  Point a = input.back();
  for (Point b : input) {
    bool ai = inside(a), bi = inside(b);
    if (ai != bi) {
      Point p = a + (b - a) * ((bound - a[axis]) / (b[axis] - a[axis]));
      p[axis] = bound;
      out.push_back(p);
    }
    if (bi) out.push_back(b);
    a = b;
  }
  return out;
}

bool contains(const Ring& ring, const std::array<double, 2>& p) {
  bool result = false;
  for (size_t i = 0, j = ring.size() - 1; i < ring.size(); j = i++) {
    auto a = ring[i], b = ring[j];
    if ((a[1] > p[1]) != (b[1] > p[1]) &&
        p[0] < (b[0] - a[0]) * (p[1] - a[1]) / (b[1] - a[1]) + a[0])
      result = !result;
  }
  return result;
}

double area(const Ring& r) {
  double a = 0;
  for (size_t i = 0; i < r.size(); ++i) {
    const auto& p = r[i];
    const auto& q = r[(i + 1) % r.size()];
    a += p[0] * q[1] - q[0] * p[1];
  }
  return a / 2;
}

void cap(TriangelMesh& mesh, const std::map<Edge, size_t>& edges,
         const std::map<Key, Point>& points, int axis, double plane, bool upper,
         double eps) {
  std::map<Key, std::vector<Key>> adjacent;
  for (const auto& [e, count] : edges) {
    if (count != 1) continue;
    const auto& a = points.at(e.first);
    const auto& b = points.at(e.second);
    if (std::abs(a[axis] - plane) > eps || std::abs(b[axis] - plane) > eps)
      continue;
    adjacent[e.first].push_back(e.second);
    adjacent[e.second].push_back(e.first);
  }
  for (const auto& [k, neighbors] : adjacent) {
    (void)k;
    if (neighbors.size() != 2)
      throw std::invalid_argument("Non-manifold slice contour");
  }
  std::vector<Ring> rings;
  int u = (axis + 1) % 3, v = (axis + 2) % 3;
  while (!adjacent.empty()) {
    Key start = adjacent.begin()->first, current = start, previous{};
    bool first = true;
    Ring ring;
    do {
      Point p = points.at(current);
      ring.push_back({p[u], p[v]});
      const auto neighbors = adjacent.at(current);
      Key next =
          first || neighbors[0] != previous ? neighbors[0] : neighbors[1];
      first = false;
      adjacent.erase(current);
      previous = current;
      current = next;
    } while (current != start);
    if (ring.size() < 3 || std::abs(area(ring)) <= eps * eps)
      throw std::invalid_argument("Degenerate slice contour");
    rings.push_back(std::move(ring));
  }
  std::vector<int> parent(rings.size(), -1), depth(rings.size());
  for (size_t i = 0; i < rings.size(); ++i) {
    double best = std::numeric_limits<double>::infinity();
    for (size_t j = 0; j < rings.size(); ++j) {
      double a = std::abs(area(rings[j]));
      if (i != j && a > std::abs(area(rings[i])) && a < best &&
          contains(rings[j], rings[i][0])) {
        parent[i] = static_cast<int>(j);
        best = a;
      }
    }
  }
  for (size_t i = 0; i < rings.size(); ++i)
    for (int p = parent[i]; p != -1; p = parent[p]) ++depth[i];
  for (size_t i = 0; i < rings.size(); ++i) {
    if (depth[i] % 2) continue;
    std::vector<Ring> polygon{rings[i]};
    for (size_t j = 0; j < rings.size(); ++j)
      if (parent[j] == static_cast<int>(i)) polygon.push_back(rings[j]);
    std::vector<std::array<double, 2>> flat;
    for (const auto& ring : polygon)
      flat.insert(flat.end(), ring.begin(), ring.end());
    auto indices = mapbox::earcut<std::uint32_t>(polygon);
    for (size_t t = 0; t < indices.size(); t += 3) {
      Point p[3];
      for (int j = 0; j < 3; ++j) {
        auto coord = flat.at(indices[t + j]);
        p[j][axis] = plane;
        p[j][u] = coord[0];
        p[j][v] = coord[1];
      }
      if ((glm::cross(p[1] - p[0], p[2] - p[0])[axis] > 0) != upper)
        std::swap(p[1], p[2]);
      Point center = (p[0] + p[1] + p[2]) / 3.0;
      for (int e = 0; e < 3; ++e) {
        Point start = p[e], end = p[(e + 1) % 3], d = end - start;
        double length2 = glm::dot(d, d);
        std::vector<std::pair<double, Point>> cuts{{0, start}, {1, end}};
        for (const auto& coord : flat) {
          Point q;
          q[axis] = plane;
          q[u] = coord[0];
          q[v] = coord[1];
          double t = glm::dot(q - start, d) / length2;
          if (t > 1e-9 && t < 1 - 1e-9 &&
              glm::length(q - (start + t * d)) <= eps)
            cuts.emplace_back(t, q);
        }
        std::sort(cuts.begin(), cuts.end(), [](const auto& x, const auto& y) {
          return x.first < y.first;
        });
        for (size_t k = 1; k < cuts.size(); ++k)
          if (glm::length(cuts[k].second - cuts[k - 1].second) > eps)
            mesh.add_triangle(center, cuts[k - 1].second, cuts[k].second);
      }
    }
  }
}
}  // namespace

std::vector<TriangelMesh> AreaCuting(const TriangelMesh& base, const Grid& grid,
                                     Axis axis_value) {
  int axis = static_cast<int>(axis_value);
  if (axis < 0 || axis > 2) throw std::invalid_argument("Invalid cutting axis");
  double lo = grid.origin[axis];
  double scale = 1.0;
  for (int j = 0; j < 3; ++j) {
    const double end =
        grid.origin[j] +
        (grid.dimensions[j] ? grid.dimensions[j] - 1 : 0) * grid.step[j];
    scale = std::max({scale, std::abs(grid.origin[j]), std::abs(end)});
  }
  double eps = scale * 1e-7;
  std::vector<TriangelMesh> result;
  for (size_t i = 0; i + 1 < grid.dimensions[axis]; ++i) {
    double a = lo + i * grid.step[axis], b = lo + (i + 1) * grid.step[axis];
    TriangelMesh slice({}, {});
    std::map<Edge, size_t> edges;
    std::map<Key, Point> points;
    const auto intersects_slice = [&](const Triangel& t) {
      const auto& vertices = base.vertices();
      const auto [min_coord, max_coord] = std::minmax(
          {vertices[t.vertics[0]][axis], vertices[t.vertics[1]][axis],
           vertices[t.vertics[2]][axis]});
      return max_coord >= a && min_coord <= b;
    };
    for (const auto& t :
         base.triangles() | std::views::filter(intersects_slice)) {
      std::vector<Point> polygon;
      for (auto index : t.vertics) polygon.emplace_back(base.vertices()[index]);
      const auto normal =
          glm::cross(polygon[1] - polygon[0], polygon[2] - polygon[0]);
      const bool on_lower = std::all_of(polygon.begin(), polygon.end(),
                                        [&](Point p) { return p[axis] == a; });
      const bool on_upper = std::all_of(polygon.begin(), polygon.end(),
                                        [&](Point p) { return p[axis] == b; });
      if ((on_lower && normal[axis] > 0) || (on_upper && normal[axis] < 0))
        continue;
      if (!std::ranges::all_of(
              polygon, [&](Point p) { return p[axis] >= a && p[axis] <= b; })) {
        polygon = clip(clip(polygon, axis, a, true), axis, b, false);
      }
      if (polygon.size() < 3) continue;
      for (size_t j = 1; j + 1 < polygon.size(); ++j) {
        const std::array<Point, 3> triangle{polygon[0], polygon[j],
                                            polygon[j + 1]};
        if (glm::length(glm::cross(triangle[1] - triangle[0],
                                   triangle[2] - triangle[0])) <= eps * eps)
          continue;
        slice.add_triangle(triangle[0], triangle[1], triangle[2]);
        for (size_t k = 0; k < triangle.size(); ++k) {
          const auto& start = triangle[k];
          const auto& end = triangle[(k + 1) % triangle.size()];
          Key x = key(start, eps), y = key(end, eps);
          points[x] = start;
          points[y] = end;
          ++edges[edge(x, y)];
        }
      }
    }
    if (slice.triangles().empty()) continue;
    cap(slice, edges, points, axis, a, false, eps);
    cap(slice, edges, points, axis, b, true, eps);
    result.push_back(std::move(slice));
  }
  return result;
}
}  // namespace visual_integrator

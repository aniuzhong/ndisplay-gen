#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace ndisplay {
namespace test {

struct Diff {
    std::string path;   // JSON pointer-like path
    std::string detail; // human-readable description
};

// Structured JSON comparison with float tolerance.
// All numeric values are compared as double, regardless of JSON integer/float type.
// Special: if expected ≈ 0 (|expected| < tolerance*10), generated must also be
//   near 0 (|generated| < tolerance).
// Otherwise: |generated - expected| < max(tolerance, tolerance * max(|a|, |b|)).
std::vector<Diff> Compare(const nlohmann::json& generated,
                          const nlohmann::json& expected,
                          double tolerance = 1e-4);

}  // namespace test
}  // namespace ndisplay

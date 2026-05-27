#include "test_json_compare.h"

namespace ndisplay {
namespace test {

namespace {

bool IsNumber(const nlohmann::json& j) {
    return j.is_number();
}

double AsDouble(const nlohmann::json& j) {
    return j.get<double>();
}

bool NumbersEqual(double a, double b, double tol) {
    double abs_a = std::abs(a);
    double abs_b = std::abs(b);
    double scale = std::max(tol, tol * std::max(abs_a, abs_b));

    // Special: if expected ≈ 0
    if (abs_b < tol * 10.0 && abs_a < tol) {
        return true;
    }

    return std::abs(a - b) <= scale;
}

void CompareImpl(const nlohmann::json& gen, const nlohmann::json& exp,
                 const std::string& path, std::vector<Diff>& diffs,
                 double tol) {
    // Both numbers: compare as double (regardless of int/float/unsigned subtype)
    if (IsNumber(gen) && IsNumber(exp)) {
        double g = AsDouble(gen);
        double e = AsDouble(exp);
        if (!NumbersEqual(g, e, tol)) {
            diffs.push_back({path, "value mismatch: generated=" +
                             std::to_string(g) + " expected=" + std::to_string(e)});
        }
        return;
    }

    // Only one side is a number
    if (IsNumber(gen) != IsNumber(exp)) {
        diffs.push_back({path, "type mismatch: generated=" +
                         std::string(gen.type_name()) +
                         " expected=" + std::string(exp.type_name())});
        return;
    }

    // Non-numeric types must match exactly
    if (gen.type() != exp.type()) {
        diffs.push_back({path, "type mismatch: generated=" +
                         std::string(gen.type_name()) +
                         " expected=" + std::string(exp.type_name())});
        return;
    }

    // Objects
    if (gen.is_object()) {
        for (const auto& [key, eval] : exp.items()) {
            std::string child_path = path.empty() ? key : path + "." + key;
            if (!gen.contains(key)) {
                diffs.push_back({child_path, "missing key in generated"});
                continue;
            }
            CompareImpl(gen[key], eval, child_path, diffs, tol);
        }
        for (const auto& [key, gval] : gen.items()) {
            std::string child_path = path.empty() ? key : path + "." + key;
            if (!exp.contains(key)) {
                diffs.push_back({child_path, "unexpected key in generated"});
            }
        }
        return;
    }

    // Arrays
    if (gen.is_array()) {
        if (gen.size() != exp.size()) {
            diffs.push_back({path, "array size mismatch: generated=" +
                             std::to_string(gen.size()) + " expected=" +
                             std::to_string(exp.size())});
            return;
        }
        for (std::size_t i = 0; i < gen.size(); ++i) {
            std::string child_path = path + "[" + std::to_string(i) + "]";
            CompareImpl(gen[i], exp[i], child_path, diffs, tol);
        }
        return;
    }

    // Strings
    if (gen.is_string()) {
        if (gen.get<std::string>() != exp.get<std::string>()) {
            diffs.push_back({path, "string mismatch: generated=\"" +
                             gen.get<std::string>() + "\" expected=\"" +
                             exp.get<std::string>() + "\""});
        }
        return;
    }

    // Booleans and null: direct comparison
    if (gen != exp) {
        diffs.push_back({path, "value mismatch: generated=" + gen.dump() +
                         " expected=" + exp.dump()});
    }
}

}  // namespace

std::vector<Diff> Compare(const nlohmann::json& generated,
                          const nlohmann::json& expected,
                          double tolerance) {
    std::vector<Diff> diffs;
    CompareImpl(generated, expected, "", diffs, tolerance);
    return diffs;
}

}  // namespace test
}  // namespace ndisplay

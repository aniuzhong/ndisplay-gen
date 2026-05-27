#pragma once

#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

#include "model.h"

namespace ndisplay {

Configuration FromJson(const nlohmann::json& j);
Configuration FromFile(const std::string& path);

}  // namespace ndisplay

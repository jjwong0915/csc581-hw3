#ifndef SFML_ADDON_VECTOR2F
#define SFML_ADDON_VECTOR2F

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace sf {
void to_json(json& j, const Vector2f& v);
void from_json(const json& j, Vector2f& v);

void to_json(json& j, const Color& c);
void from_json(const json& j, Color& c);
};  // namespace sf

#endif

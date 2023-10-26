#include "static_platform/static_platform.hpp"

#include <iostream>

#include "sfml_addon/json/json.hpp"

static_platform::static_platform() : size{0, 0}, position{0, 0}, color{0} {};

static_platform::static_platform(sf::Vector2f siz, sf::Vector2f pos,
                                 sf::Color col)
    : size{siz}, position{pos}, color{col} {};

sf::RectangleShape static_platform::get_shape() {
  sf::RectangleShape rectangle{size};
  rectangle.setPosition(position);
  rectangle.setFillColor(color);
  return rectangle;
}

void to_json(json& j, const static_platform& s) {
  j = json{
      {"type", "static_platform"},
      {"size", s.size},
      {"position", s.position},
      {"color", s.color},
  };
}

void from_json(const json& j, static_platform& s) {
  j.at("size").get_to(s.size);
  j.at("position").get_to(s.position);
  j.at("color").get_to(s.color);
}

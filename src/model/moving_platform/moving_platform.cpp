#include "moving_platform/moving_platform.hpp"

#include "sfml_addon/json/json.hpp"

moving_platform::moving_platform()
    : position{0, 0}, velocity{0, 0}, color{0} {};

moving_platform::moving_platform(sf::Vector2f pos, sf::Vector2f vel,
                                 sf::Color col)
    : position{pos}, velocity{vel}, color{col} {};

void moving_platform::move() {
  float next_x = position.x + velocity.x;
  float next_y = position.y + velocity.y;
  if (next_x < 50 || next_x > 650) {
    velocity.x = -velocity.x;
  }
  if (next_y < 100 || next_y > 500) {
    velocity.y = -velocity.y;
  }
  position.x += velocity.x;
  position.y += velocity.y;
}

sf::RectangleShape moving_platform::get_shape() {
  sf::RectangleShape rectangle{{100, 20}};
  rectangle.setPosition(position);
  rectangle.setFillColor(color);
  return rectangle;
}

void to_json(json& j, const moving_platform& m) {
  j = json{
      {"type", "moving_platform"},
      {"position", m.position},
      {"velocity", m.velocity},
      {"color", m.color},
  };
}

void from_json(const json& j, moving_platform& m) {
  j.at("position").get_to(m.position);
  j.at("velocity").get_to(m.velocity);
  j.at("color").get_to(m.color);
}

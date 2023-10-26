#ifndef STATIC_PLATFORM_H
#define STATIC_PLATFORM_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class static_platform {
  sf::Vector2f size;
  sf::Vector2f position;
  sf::Color color;

 public:
  static_platform();
  static_platform(sf::Vector2f siz, sf::Vector2f pos, sf::Color col);
  sf::RectangleShape get_shape();
  friend void to_json(json& j, const static_platform& s);
  friend void from_json(const json& j, static_platform& s);
};

#endif

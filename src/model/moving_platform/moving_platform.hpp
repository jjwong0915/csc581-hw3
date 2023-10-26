#ifndef MOVING_PLATFORM_H
#define MOVING_PLATFORM_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class moving_platform {
  sf::Vector2f position;
  sf::Vector2f velocity;
  sf::Color color;

 public:
  moving_platform();
  moving_platform(sf::Vector2f pos, sf::Vector2f vel, sf::Color col);
  void move();
  sf::RectangleShape get_shape();
  friend void to_json(json& j, const moving_platform& m);
  friend void from_json(const json& j, moving_platform& m);
};

#endif

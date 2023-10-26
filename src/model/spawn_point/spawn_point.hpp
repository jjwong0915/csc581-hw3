#ifndef SPAWN_POINT_H
#define SPAWN_POINT_H

#include <SFML/System.hpp>

class spawn_point {
  sf::Vector2f position;

 public:
  spawn_point();
  spawn_point(sf::Vector2f pos);
  sf::Vector2f get_position();
};

#endif

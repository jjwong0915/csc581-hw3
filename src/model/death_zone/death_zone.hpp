#ifndef DEATH_ZONE_H
#define DEATH_ZONE_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class death_zone {
  sf::Vector2f position;
  sf::Vector2f size;

 public:
  death_zone();
  death_zone(sf::Vector2f pos, sf::Vector2f siz);
  sf::FloatRect get_bound();
};

#endif

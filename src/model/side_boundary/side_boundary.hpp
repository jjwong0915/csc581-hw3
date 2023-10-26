#ifndef SIDE_BOUNDARY_H
#define SIDE_BOUNDARY_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class side_boundary {
  sf::Vector2f position;
  sf::Vector2f size;

 public:
  side_boundary();
  side_boundary(sf::Vector2f pos, sf::Vector2f siz);
  sf::FloatRect get_bound();
};

#endif

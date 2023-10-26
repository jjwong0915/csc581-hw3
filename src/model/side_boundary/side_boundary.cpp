#include <side_boundary/side_boundary.hpp>

side_boundary::side_boundary() : position{0, 0}, size{0, 0} {}

side_boundary::side_boundary(sf::Vector2f pos, sf::Vector2f siz)
    : position{pos}, size{siz} {}

sf::FloatRect side_boundary::get_bound() {
  return sf::FloatRect{position, size};
}

#include "spawn_point/spawn_point.hpp"

spawn_point::spawn_point() : position{0, 0} {}

spawn_point::spawn_point(sf::Vector2f pos) : position{pos} {}

sf::Vector2f spawn_point::get_position() { return position; }

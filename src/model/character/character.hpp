#ifndef CHARACTER_H
#define CHARACTER_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <deque>
#include <nlohmann/json.hpp>

#include "spawn_point/spawn_point.hpp"

using json = nlohmann::json;

class character {
  unsigned client_id;
  sf::Vector2f position;
  sf::Vector2f velocity;
  sf::Color color;
  bool supported;
  bool respawned;

 public:
  character();
  character(unsigned cid, sf::Vector2f pos, sf::Vector2f vel, sf::Color col);
  void go_left();
  void go_right();
  void jump();
  void move();
  sf::RectangleShape get_shape();
  void apply_gravity();
  void reset_collision();
  void apply_collision(const sf::FloatRect& bound);
  void reset_death();
  void apply_death(const sf::FloatRect& bound,
                   std::deque<spawn_point>& spawn_points);
  bool is_respawned();
  unsigned get_clinet_id();
  sf::Vector2f get_position();
  void apply_scrolling(sf::RenderWindow& window, const sf::FloatRect& bound);
  void friend to_json(json& j, const character& c);
  friend void from_json(const json& j, character& c);
};

#endif

#include <SFML/Graphics.hpp>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <vector>
#include <zmq.hpp>

#include "character/character.hpp"
#include "death_zone/death_zone.hpp"
#include "moving_platform/moving_platform.hpp"
#include "sfml_addon/json/json.hpp"
#include "spawn_point/spawn_point.hpp"
#include "static_platform/static_platform.hpp"

using json = nlohmann::json;

void handle_client(std::map<unsigned, character>& client_list,
                   std::mutex& client_list_mtx) {
  zmq::context_t context;
  zmq::socket_t sub_socket(context, ZMQ_SUB);
  sub_socket.bind("tcp://127.0.0.1:5556");
  sub_socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
  //
  while (true) {
    zmq::message_t message;
    zmq::recv_result_t result = sub_socket.recv(message);
    //
    json message_json = json::parse(message.to_string());
    std::string message_type = message_json.at("type");
    std::lock_guard<std::mutex> client_list_lck{client_list_mtx};
    unsigned client_id = message_json.at("client_id");
    if (message_type == "join") {
      client_list[client_id] =
          character{client_id, {80, 400}, {0, 0}, sf::Color::White};
    } else if (message_type == "leave") {
      client_list.erase(client_id);
    } else if (message_type == "go_left") {
      client_list[client_id].go_left();
    } else if (message_type == "go_right") {
      client_list[client_id].go_right();
    } else if (message_type == "jump") {
      client_list[client_id].jump();
    }
  }
}

int main() {
  zmq::context_t context;
  zmq::socket_t pub_socket(context, ZMQ_PUB);
  pub_socket.bind("tcp://127.0.0.1:5555");
  // static platforms
  static_platform platform_left{{200, 30}, {0, 500}, sf::Color::Blue};
  static_platform platform_right{{200, 30}, {600, 500}, sf::Color::Blue};
  static_platform platform_up{{200, 30}, {900, 100}, sf::Color::Red};
  // moving platforms
  moving_platform platform_vertical({800, 500}, {0, -1}, sf::Color::Yellow);
  moving_platform platform_horizontal({400, 400}, {2, 0}, sf::Color::Cyan);
  // death zone
  death_zone bottom_zone{{-1000, 600}, {1900, 50}};
  // spawn points
  std::deque<spawn_point> spawn_points;
  spawn_points.push_back(sf::Vector2f{60, 400});
  spawn_points.push_back(sf::Vector2f{80, 400});
  spawn_points.push_back(sf::Vector2f{100, 400});
  // client handler
  std::cout << "starting client handler" << std::endl;
  std::map<unsigned, character> client_list;
  std::mutex client_list_mtx;
  std::thread client_handler(handle_client, std::ref(client_list),
                             std::ref(client_list_mtx));
  // server loop
  std::cout << "starting server loop" << std::endl;
  while (true) {
    // locking client list
    {
      std::lock_guard<std::mutex> client_list_lck{client_list_mtx};
      // apply gravitational accelration
      for (auto& id_char : client_list) {
        id_char.second.apply_gravity();
      }
      // apply object collision
      for (auto& id_char : client_list) {
        character& character_ = id_char.second;
        character_.reset_collision();
        character_.apply_collision(platform_left.get_shape().getGlobalBounds());
        character_.apply_collision(
            platform_right.get_shape().getGlobalBounds());
        character_.apply_collision(platform_up.get_shape().getGlobalBounds());
        character_.apply_collision(
            platform_vertical.get_shape().getGlobalBounds());
        character_.apply_collision(
            platform_horizontal.get_shape().getGlobalBounds());
      }
      // execute object movement
      for (auto& id_char : client_list) {
        id_char.second.move();
      }
      platform_vertical.move();
      platform_horizontal.move();
      // apply character death
      for (auto& id_char : client_list) {
        id_char.second.reset_death();
        id_char.second.apply_death(bottom_zone.get_bound(), spawn_points);
      }
      // send object list
      json obj_json = {platform_left, platform_right, platform_up,
                       platform_vertical, platform_horizontal};
      for (auto& id_char : client_list) {
        obj_json.push_back(id_char.second);
      }
      std::string obj_str = obj_json.dump();
      pub_socket.send(zmq::message_t{obj_str.begin(), obj_str.end()},
                      zmq::send_flags::none);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds{10});
  }
}

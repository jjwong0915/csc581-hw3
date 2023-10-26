#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <vector>
#include <zmq.hpp>

#include "character/character.hpp"
#include "moving_platform/moving_platform.hpp"
#include "side_boundary/side_boundary.hpp"
#include "static_platform/static_platform.hpp"

using json = nlohmann::json;

int main() {
  unsigned client_id = std::random_device{}();
  std::cout << client_id << std::endl;
  // setup sockets
  zmq::context_t context;
  zmq::socket_t pub_socket(context, ZMQ_PUB);
  pub_socket.connect("tcp://127.0.0.1:5556");
  zmq::socket_t sub_socket(context, ZMQ_SUB);
  sub_socket.connect("tcp://127.0.0.1:5555");
  sub_socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
  // send join message
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  std::string msg_str = json{{"type", "join"}, {"client_id", client_id}}.dump();
  pub_socket.send(zmq::message_t{msg_str.begin(), msg_str.end()},
                  zmq::send_flags::none);
  // side boundaries
  side_boundary boundary_left{{0, 0}, {100, 600}};
  side_boundary boundary_right{{700, 0}, {100, 600}};
  // pressed keys
  std::set<sf::Keyboard::Key> pressed_keys;
  // event and display loop
  sf::RenderWindow window(sf::VideoMode{800, 600}, "Homework 3",
                          sf::Style::Close);
  while (window.isOpen()) {
    // handle event and keyboard
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        std::string msg_str =
            json{{"type", "leave"}, {"client_id", client_id}}.dump();
        pub_socket.send(zmq::message_t{msg_str.begin(), msg_str.end()},
                        zmq::send_flags::none);
        window.close();
      } else if (event.type == sf::Event::KeyPressed) {
        pressed_keys.insert(event.key.code);
        if (event.key.code == sf::Keyboard::Space) {
          std::string msg_str =
              json{{"type", "jump"}, {"client_id", client_id}}.dump();
          pub_socket.send(zmq::message_t{msg_str.begin(), msg_str.end()},
                          zmq::send_flags::none);
        }
      } else if (event.type == sf::Event::KeyReleased) {
        pressed_keys.erase(event.key.code);
      }
    }
    if (pressed_keys.count(sf::Keyboard::Left) > 0) {
      std::string msg_str =
          json{{"type", "go_left"}, {"client_id", client_id}}.dump();
      pub_socket.send(zmq::message_t{msg_str.begin(), msg_str.end()},
                      zmq::send_flags::none);
    }
    if (pressed_keys.count(sf::Keyboard::Right) > 0) {
      std::string msg_str =
          json{{"type", "go_right"}, {"client_id", client_id}}.dump();
      pub_socket.send(zmq::message_t{msg_str.begin(), msg_str.end()},
                      zmq::send_flags::none);
    }
    // draw objects
    window.clear();
    zmq::message_t message;
    zmq::recv_result_t result = sub_socket.recv(message);
    for (json& obj : json::parse(message.to_string())) {
      if (obj.at("type") == "moving_platform") {
        moving_platform platform{obj};
        window.draw(platform.get_shape());
      }
      if (obj.at("type") == "static_platform") {
        static_platform platform{obj};
        window.draw(platform.get_shape());
      }
      if (obj.at("type") == "character") {
        character character_{obj};
        window.draw(character_.get_shape());
        if (character_.get_clinet_id() == client_id) {
          if (character_.is_respawned()) {
            sf::View view = window.getDefaultView();
            view.setCenter(character_.get_position().x + 300,
                           view.getCenter().y);
            window.setView(view);
          }
          character_.apply_scrolling(window, boundary_left.get_bound());
          character_.apply_scrolling(window, boundary_right.get_bound());
        }
      }
    }
    //
    window.display();
  }
}
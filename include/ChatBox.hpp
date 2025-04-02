#ifndef CHATBOX_HPP
#define CHATBOX_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <vector>

struct ChatMessage {
  std::string text;
  SDL_Color color;

  ChatMessage(const std::string &msg, SDL_Color clr) : text(msg), color(clr) {}
};

class ChatBox {
 public:
  ChatBox(int x, int y, int w, int h);
  ~ChatBox();
  void setFont(TTF_Font *font);
  void addMessage(const std::string &message,
                  SDL_Color color = {255, 255, 255, 255});
  void render(SDL_Renderer *renderer);

  // Add method to update the position and size
  void setRect(const SDL_Rect &newRect);

 private:
  SDL_Rect rect;                      // Position and size of the chat box
  std::vector<ChatMessage> messages;  // Text messages to display
  TTF_Font *font;                     // Font used for rendering text
};

#endif

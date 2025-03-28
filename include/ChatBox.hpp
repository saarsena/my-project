#include "ChatLine.hpp"
#include <vector>

class ChatBox {
public:
  ChatBox(int max_lines);   
  void render(SDL_Renderer *renderer);
  void addLine(const std::string &text, SDL_Color color);
  void scroll(int offset);
  void handleInput(SDL_Event &e);
  void clear();

  std::vector<ChatLine> chat_history;
  std::string input_buffer;
  int scroll_offset = 0;
  int max_lines;

private:
};
#include "ChatBox.hpp"

#include <SDL2/SDL_ttf.h>

#include <algorithm>
#include <iostream>
#include <string>

#include "SDL_render.h"

ChatBox::ChatBox(int x, int y, int w, int h)
    : rect{x, y, w, h}, messages(), font(nullptr) {
  std::cout << "ChatBox created at (" << x << "," << y << ") with dimensions "
            << w << "x" << h << std::endl;
}

ChatBox::~ChatBox() {
  std::cout << "ChatBox destroyed, font pointer: " << font << std::endl;
}

void ChatBox::setFont(TTF_Font *f) {
  std::cout << "ChatBox::setFont - Setting font to: " << f << std::endl;
  font = f;
  if (font) {
    if (TTF_FontHeight(font) <= 0) {
      std::cerr << "Warning: Font has invalid height: " << TTF_FontHeight(font)
                << std::endl;
    } else {
      std::cout << "Font set successfully, height: " << TTF_FontHeight(font)
                << std::endl;
    }
  } else {
    std::cerr << "Warning: Setting null font pointer" << std::endl;
  }
}

void ChatBox::addMessage(const std::string &message, SDL_Color color) {
  std::cout << "ChatBox::addMessage - Adding message: " << message << std::endl;
  messages.emplace_back(message, color);
  // Keep only the last 15 messages to allow for more scrolling
  if (messages.size() > 15) {
    messages.erase(messages.begin());
  }
  std::cout << "ChatBox now has " << messages.size() << " messages"
            << std::endl;
}

void ChatBox::render(SDL_Renderer *renderer) {
  // Skip rendering if font is not set
  if (!font) {
    std::cerr << "ChatBox::render - Font not set (font pointer: " << font << ")"
              << std::endl;
    return;
  }

  std::cout << "ChatBox::render - Starting render with " << messages.size()
            << " messages, font height: " << TTF_FontHeight(font) << std::endl;

  // Draw the chat box background with transparency
  SDL_SetRenderDrawColor(renderer, 0, 43, 54,
                         160);  // Reduced alpha for transparency
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_RenderFillRect(renderer, &rect);

  // Draw the chat box border
  SDL_SetRenderDrawColor(renderer, 253, 246, 227,
                         200);  // Semi-transparent white border
  SDL_RenderDrawRect(renderer, &rect);

  // Render chat messages
  int fontHeight = TTF_FontHeight(font);
  int margin = 10;      // Margin from the edges
  int lineSpacing = 2;  // Space between lines

  // Calculate how many messages can fit in the viewport
  int maxVisibleMessages = (rect.h - 2 * margin) / (fontHeight + lineSpacing);

  // Calculate the starting Y position for the last (newest) message
  int lastMessageY = rect.y + rect.h - margin - fontHeight;

  // Determine how many messages to show
  size_t messagesToShow = std::min(messages.size(), (size_t)maxVisibleMessages);

  // Calculate the start index (oldest message that will be shown)
  size_t startIdx =
      messages.size() > messagesToShow ? messages.size() - messagesToShow : 0;

  // Render messages from oldest to newest (bottom to top)
  for (size_t i = 0; i < messagesToShow; i++) {
    const auto &message = messages[startIdx + i];

    // Calculate Y position for this message
    // The newest message (last in vector) should be at the bottom
    // Earlier messages appear above it
    int messageY =
        lastMessageY - (messagesToShow - 1 - i) * (fontHeight + lineSpacing);

    if (message.text.empty()) {
      std::cout << "Skipping empty message" << std::endl;
      continue;
    }

    std::cout << "Rendering message: " << message.text << std::endl;

    // Create a surface from the message with the specified color
    SDL_Surface *textSurface =
        TTF_RenderText_Solid(font, message.text.c_str(), message.color);
    if (!textSurface) {
      std::cerr << "Failed to render text: " << TTF_GetError() << std::endl;
      continue;
    }

    // Create a texture from the surface
    SDL_Texture *textTexture =
        SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
      std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
      SDL_FreeSurface(textSurface);
      continue;
    }

    // Set the render position
    SDL_Rect renderQuad = {rect.x + margin, messageY, textSurface->w,
                           textSurface->h};

    // Render the texture
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);

    // Free resources
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
  }

  // Reset blend mode
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void ChatBox::setRect(const SDL_Rect &newRect) {
  rect = newRect;
  std::cout << "ChatBox::setRect - Updated position to (" << rect.x << ","
            << rect.y << ") with dimensions " << rect.w << "x" << rect.h
            << std::endl;
}

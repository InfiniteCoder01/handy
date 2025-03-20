#include "flap.h"
#include "hardware/bluetooth.h"
#include "hardware/display.h"
#include "hardware/input.h"
#include "hardware/wifi.h"
#include "ui/ui.h"
#include "utils.h"
#include <OreonMath.hpp>

using namespace VectorMath;

static vec2f cmul(vec2f a, vec2f b) {
  return vec2f(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
}

namespace flap {
void play() {
  while (true) {
    const int16_t offset = ui::screen.width() / 4;
    const float velocityX = 50.0;
    vec2f position = vec2f(0.0, ui::screen.height() / 2);
    vec2f velocity = vec2f(velocityX, 0);

    const int16_t pipeWidth = 20;
    struct Pipe {
      float x;
      int16_t gapY, gapSize;
    };
    std::vector<Pipe> pipes;
    pipes.reserve(2);

    uint32_t lastTime = millis();
    bool alive = true;
    while (position.y < ui::screen.height()) {
      input.update();
      wifi::tick();
      bluetooth::silence();

      float deltaTime;
      {
        uint32_t t = millis();
        deltaTime = (t - lastTime) / 1000.0;
        lastTime = t;
      }

      if (alive) {
        if (pipes.empty() ||
            (pipes.back().x - position.x < ui::screen.width() / 2 &&
             pipes.size() < 2 && rand() % 100 < 2)) {
          int16_t gapSize = rand() % 10 + 30;
          pipes.push_back(Pipe{
              .x = position.x + (float)ui::screen.width(),
              .gapY =
                  (int16_t)(rand() % (ui::screen.height() - gapSize - 6) + 3),
              .gapSize = gapSize,
          });
        }
      }

      velocity.y += 60.0 * deltaTime;

      if (alive) {
        if (velocity.y > 0.0 && input.down.pressing()) {
          velocity.y *= 0.95;
          velocity.x += (velocityX * 1.5 - velocity.x) * 0.05;
        } else {
          velocity.x = velocityX;
          if (input.up.press())
            velocity.y = -70.0;
          else if (input.up.release() && velocity.y < 0)
            velocity.y *= 0.5;
        }
      } else {
        velocity.x = 0;
      }

      position += velocity * deltaTime;

      if (alive) {
        for (ssize_t i = pipes.size() - 1; i >= 0; i--) {
          if (pipes[i].x - position.x + pipeWidth < 0)
            pipes.erase(pipes.begin() + i);
          else if (pipes[i].x - position.x + 3 < offset &&
                   pipes[i].x - position.x + pipeWidth - 3 > offset) {
            if (position.y < pipes[i].gapY ||
                position.y > pipes[i].gapY + pipes[i].gapSize) {
              alive = false;
            }
          }
        }
      }

      if (position.y < 0 || position.y >= ui::screen.height())
        alive = false;

      ui::screen.fillScreen(BLACK);

      for (const auto &pipe : pipes) {
        ui::screen.fillRect(pipe.x - position.x,
                            pipe.gapY - ui::screen.height(), pipeWidth,
                            ui::screen.height(), WHITE);
        ui::screen.fillRect(pipe.x - position.x, pipe.gapY + pipe.gapSize,
                            pipeWidth, ui::screen.height(), WHITE);
      }

      {
        const vec2f dir = velocity.normalized();
        const vec2f p1 = cmul(vec2f(0, -3), dir) + vec2f(offset, position.y);
        const vec2f p2 = cmul(vec2f(5, 0), dir) + vec2f(offset, position.y);
        const vec2f p3 = cmul(vec2f(0, 3), dir) + vec2f(offset, position.y);
        ui::screen.drawLine(p1.x, p1.y, p2.x, p2.y, WHITE);
        ui::screen.drawLine(p2.x, p2.y, p3.x, p3.y, WHITE);
      }

      ui::show();
    }

    bool respawn = false, exit = false;
    ui::Container gameOverScreen;
    {
      gameOverScreen.size = ui::screenSize();
      gameOverScreen.justifyContent = 0.5;

      using namespace ui::shortcuts;
      gameOverScreen << vcenter(
          {
              label("Game Over!"),
              label(format("Your score is %d", 0)),
              inl(button(label("Respawn"), [&]() { respawn = true; }),
                  button(label("Exit"), [&]() { exit = true; })),
          },
          vec2u(0, ui::screen.height()));
    }
    while (!respawn) {
      input.update();
      wifi::tick();
      bluetooth::silence();

      ui::screen.fillScreen(BLACK);
      ui::serve(gameOverScreen);
      ui::show();

      if (exit)
        return;
    }
  }
}
} // namespace flap
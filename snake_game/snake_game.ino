#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 2
#define DATA_IN_1 11
#define DATA_IN_2 12
#define CLK 13
#define CS 10
#define UP_BUTTON_PIN 1
#define DOWN_BUTTON_PIN 2
#define LEFT_BUTTON_PIN 4
#define RIGHT_BUTTON_PIN 5
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define STOP 4

MD_MAX72XX mx_1 = MD_MAX72XX(HARDWARE_TYPE, DATA_IN_1, CLK, CS, MAX_DEVICES);
MD_MAX72XX mx_2 = MD_MAX72XX(HARDWARE_TYPE, DATA_IN_2, CLK, CS, MAX_DEVICES);

class SnakeGame {
private:
  int snakeX[256];
  int snakeY[256];
  int snakeLength;
  int seedX, seedY;
  int x, y;
  int prevX, prevY, prev2X, prev2Y;
  int direction;

public:
  SnakeGame() {
    // Initialize game state
    snakeLength = 1;
    x = 5;
    y = 5;
    seedX = random(15) + 1;
    seedY = random(15) + 1;
    direction = STOP;
  }

  void updateDirection(int newDirection) {
    if ((newDirection + 2) % 4 != direction) {
      // The new direction is valid (not opposite of the current direction)
      direction = newDirection;
    }
  }

  void drawGameOver(MD_MAX72XX &mx_1, MD_MAX72XX &mx_2) {
    // Turn on all LEDs to indicate game over
    mx_1.clear();
    mx_2.clear();
    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
        mx_1.setPoint(i, j, 1);
        mx_2.setPoint(i, j, 1);
      }
    }
    mx_1.update();
    mx_2.update();
  }

  bool checkGameOver() {
    // Check if the snake has collided with itself
    for (int i = 1; i < snakeLength; i++) {
      if (snakeX[i] == snakeX[0] && snakeY[i] == snakeY[0]) {
        return true; // Game over
      }
    }

    // Check if the snake has collided with the boundaries of the game board
    if (snakeX[0] < 0 || snakeX[0] >= 16 || snakeY[0] < 0 || snakeY[0] >= 16) {
      return true; // Game over
    }

    return false; // Game is not over
  }

  void update() {
    prevX = snakeX[0];
    prevY = snakeY[0];
    snakeX[0] = x;
    snakeY[0] = y;
    // Update the snake's position based on the current direction
    for (int i = snakeLength - 1; i > 0; i--) {
      prev2X = snakeX[i];
      prev2Y = snakeY[i];
      snakeX[i] = prevX;
      snakeY[i] = prevY;
      prevX = prev2X;
      prevY = prev2Y;
    }

    switch (direction) {
      case UP:
        y--;
        break;
      case RIGHT:
        x++;
        break;
      case DOWN:
        y++;
        break;
      case LEFT:
        x--;
        break;
      default:
        break;
    }

    // Check if the snake has eaten the seed
    if (snakeX[0] == seedX && snakeY[0] == seedY) {
      // Increase the length of the snake and generate a new seed
      snakeLength++;
      seedX = random(15) + 1;
      seedY = random(15) + 1;
    }
  }

  void draw(MD_MAX72XX &mx_1, MD_MAX72XX &mx_2) {
    mx_1.clear();
    mx_2.clear();

    // Draw the snake
    for (int i = 0; i < snakeLength; i++) {
      if (snakeX[i] < 8) {
        mx_1.setPoint(snakeX[i], snakeY[i], 1);
      }
      else {
        mx_2.setPoint(snakeX[i] - 8, snakeY[i], 1);
      }
    }

    // Draw the seed
    if (seedX < 8) {
      mx_1.setPoint(seedX, seedY, 1);
    }
    else {
      mx_2.setPoint(seedX - 8, seedY, 1);
    }

    mx_1.update();
    mx_2.update();
  }
};

SnakeGame game;

void setup() {
  mx_1.begin();
  mx_2.begin();
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  if (game.checkGameOver()) {
    game.drawGameOver(mx_1, mx_2); // Turn on all LEDs to indicate game over
  } else {
    // Check for button presses and update the snake's direction
    if (digitalRead(UP_BUTTON_PIN) == LOW) {
      game.updateDirection(UP);
    } else if (digitalRead(RIGHT_BUTTON_PIN) == LOW) {
      game.updateDirection(RIGHT);
    } else if (digitalRead(DOWN_BUTTON_PIN) == LOW) {
      game.updateDirection(DOWN);
    } else if (digitalRead(LEFT_BUTTON_PIN) == LOW) {
      game.updateDirection(LEFT);
    }
    game.update();
    game.draw(mx_1, mx_2);
  }
  delay(80);
}

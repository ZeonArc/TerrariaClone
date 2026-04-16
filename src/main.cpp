#include "Game.h"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        Game game(1280, 720, "Terraria Clone");
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "[FATAL ERROR] " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
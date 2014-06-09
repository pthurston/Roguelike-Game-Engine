#include "main.hpp"

Engine engine(80, 50);

int main() {
    //Will either start a new game or load one from a save file based on menu input
    engine.load();

    //Main game loop
    while (!TCODConsole::isWindowClosed()) {
        engine.update();
        engine.render();
        TCODConsole::flush();
    }

    //Save the map before closing
    engine.save();
    return 0;
}

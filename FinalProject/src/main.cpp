#include "Game.h"
#include <iostream>
using namespace std;

int main() {
    cout << "Starting game..." << endl;
    try {
        Game game;
        cout << "Game initialized." << endl;
        game.run();
    } catch (const std::exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown exception occurred." << endl;
        return 1;
    }
    cout << "Game finished." << endl;

    return 0;
}
#include <iostream>
#include <chrono>
#include <thread>
#include <future>

// Funktion für den Timer
void timerFunction(int seconds) {
    for (int i = seconds; i > 0; --i) {
        std::cout << "Noch " << i << " Sekunden übrig...\r" << std::flush;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "\nTimer abgelaufen!\n";
}

int main() {
    // Timer asynchron starten
    auto timerFuture = std::async(std::launch::async, timerFunction, 60);

    // Hauptthread: Andere Aufgaben
    for (int i = 0; i < 10; ++i) {
        std::cout << "Hauptthread arbeitet... Schritt " << i + 1 << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Keine Notwendigkeit für `join`, da `std::async` automatisch wartet
    std::cout << "Programm beendet.\n";
    return 0;
}

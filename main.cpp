#include <iostream>
#include <limits>
#include <string>
#include <utility>

#include "include/UniquePtr.h"
#include "include/SharedPtr.h"

int read_int(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }

        std::cout << "Invalid integer. Try again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Адрес показывает, один объект у двух указателей или разные
void print_unique(const char* name, const UniquePtr<int>& pointer) {
    std::cout << "  " << name << ": ";
    if (pointer.get() == nullptr) {
        std::cout << "empty\n";
    } else {
        std::cout << "value=" << *pointer << " address=" << pointer.get() << '\n';
    }
}

void print_shared(const char* name, const SharedPtr<int>& pointer) {
    std::cout << "  " << name << ": ";
    if (pointer.get() == nullptr) {
        std::cout << "empty\n";
    } else {
        std::cout << "value=" << *pointer << " address=" << pointer.get();
        std::cout << " use_count=" << pointer.use_count() << '\n';
    }
}

void print_state(const UniquePtr<int>& a, const UniquePtr<int>& b, const SharedPtr<int>& x, const SharedPtr<int>& y) {
    std::cout << "\nUniquePtr:\n";
    print_unique("a", a);
    print_unique("b", b);
    std::cout << "SharedPtr:\n";
    print_shared("x", x);
    print_shared("y", y);
}

void print_menu() {
    std::cout << "\nMenu:\n";
    std::cout << "1. a = new value\n";
    std::cout << "2. b = move(a)\n";
    std::cout << "3. a.reset()\n";
    std::cout << "4. swap a and b\n";
    std::cout << "5. x = new value\n";
    std::cout << "6. y = x (copy)\n";
    std::cout << "7. y = move(x)\n";
    std::cout << "8. x.reset()\n";
    std::cout << "9. swap x and y\n";
    std::cout << "0. Exit\n";
}

// Обмен через перемещение: временная переменная забирает объект, потом он переходит дальше
template <typename Pointer>
void swap_pointers(Pointer& first, Pointer& second) {
    Pointer temp(std::move(first));
    first = std::move(second);
    second = std::move(temp);
}

void execute_command(int command, UniquePtr<int>& a, UniquePtr<int>& b, SharedPtr<int>& x, SharedPtr<int>& y,
                     bool& running) {
    switch (command) {
        case 0:
            running = false;
            return;
        case 1:
            a.reset(new int(read_int("Value: ")));
            return;
        case 2:
            b = std::move(a);
            return;
        case 3:
            a.reset();
            return;
        case 4:
            swap_pointers(a, b);
            return;
        case 5:
            x.reset(new int(read_int("Value: ")));
            return;
        case 6:
            y = x;
            return;
        case 7:
            y = std::move(x);
            return;
        case 8:
            x.reset();
            return;
        case 9:
            swap_pointers(x, y);
            return;
        default:
            std::cout << "Unknown command\n";
            return;
    }
}

int main() {
    UniquePtr<int> a;
    UniquePtr<int> b;
    SharedPtr<int> x;
    SharedPtr<int> y;

    std::cout << "Smart pointer console UI\n";
    bool running = true;
    while (running) {
        print_state(a, b, x, y);
        print_menu();
        int command = read_int("Command: ");
        execute_command(command, a, b, x, y, running);
    }

    std::cout << "Bye\n";
    return 0;
}

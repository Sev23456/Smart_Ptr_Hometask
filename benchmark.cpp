#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>

#include "include/UniquePtr.h"
#include "include/SharedPtr.h"

// Подменяем глобальные new и delete, чтобы считать, сколько памяти и сколько раз запрашивает программа.
// Служебные байты самого malloc (заголовки блоков) сюда не попадают.
std::uint64_t allocated_bytes = 0;
std::uint64_t allocation_count = 0;

void* allocate(std::size_t size) {
    allocated_bytes += size;
    allocation_count++;
    if (size == 0) {
        size = 1;
    }
    void* memory = std::malloc(size);
    if (memory == nullptr) {
        throw std::bad_alloc();
    }
    return memory;
}

void* operator new(std::size_t size) {
    return allocate(size);
}

void* operator new[](std::size_t size) {
    return allocate(size);
}

void operator delete(void* memory) noexcept {
    std::free(memory);
}

void operator delete[](void* memory) noexcept {
    std::free(memory);
}

void operator delete(void* memory, std::size_t) noexcept {
    std::free(memory);
}

void operator delete[](void* memory, std::size_t) noexcept {
    std::free(memory);
}

const int series_count = 5;
const char* series_names[] = {"raw", "UniquePtr", "SharedPtr", "std::unique_ptr", "std::shared_ptr"};
const char* series_csv_names[] = {"raw", "unique", "shared", "std_unique", "std_shared"};
const char* series_colors[] = {"#2563eb", "#16a34a", "#ea580c", "#7c3aed", "#db2777"};

struct Measurement {
    double ms;
    std::uint64_t bytes;
    std::uint64_t allocations;
};

struct Result {
    std::size_t count;
    Measurement series[series_count];
};

double elapsed_ms(std::chrono::steady_clock::time_point started) {
    std::chrono::steady_clock::time_point finished = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(finished - started).count();
}

void start_counting() {
    allocated_bytes = 0;
    allocation_count = 0;
}

// Вызывается, когда все count объектов созданы и ещё живы
void save_memory(Measurement& result) {
    result.bytes = allocated_bytes;
    result.allocations = allocation_count;
}

// Каждый замер: создать count объектов, запомнить память, затем удалить их все
Measurement benchmark_raw(std::size_t count) {
    Measurement result;
    start_counting();
    std::chrono::steady_clock::time_point started = std::chrono::steady_clock::now();

    int** items = new int*[count];
    for (std::size_t index = 0; index < count; index++) {
        items[index] = new int(index);
    }
    save_memory(result);
    for (std::size_t index = 0; index < count; index++) {
        delete items[index];
    }
    delete[] items;

    result.ms = elapsed_ms(started);
    return result;
}

Measurement benchmark_unique(std::size_t count) {
    Measurement result;
    start_counting();
    std::chrono::steady_clock::time_point started = std::chrono::steady_clock::now();

    UniquePtr<int>* items = new UniquePtr<int>[count];
    for (std::size_t index = 0; index < count; index++) {
        items[index].reset(new int(index));
    }
    save_memory(result);
    delete[] items;

    result.ms = elapsed_ms(started);
    return result;
}

Measurement benchmark_shared(std::size_t count) {
    Measurement result;
    start_counting();
    std::chrono::steady_clock::time_point started = std::chrono::steady_clock::now();

    SharedPtr<int>* items = new SharedPtr<int>[count];
    for (std::size_t index = 0; index < count; index++) {
        items[index].reset(new int(index));
    }
    save_memory(result);
    delete[] items;

    result.ms = elapsed_ms(started);
    return result;
}

Measurement benchmark_std_unique(std::size_t count) {
    Measurement result;
    start_counting();
    std::chrono::steady_clock::time_point started = std::chrono::steady_clock::now();

    std::unique_ptr<int>* items = new std::unique_ptr<int>[count];
    for (std::size_t index = 0; index < count; index++) {
        items[index].reset(new int(index));
    }
    save_memory(result);
    delete[] items;

    result.ms = elapsed_ms(started);
    return result;
}

// reset(new int), а не make_shared, чтобы сравнение с SharedPtr было честным: объект и счётчик выделяются отдельно
Measurement benchmark_std_shared(std::size_t count) {
    Measurement result;
    start_counting();
    std::chrono::steady_clock::time_point started = std::chrono::steady_clock::now();

    std::shared_ptr<int>* items = new std::shared_ptr<int>[count];
    for (std::size_t index = 0; index < count; index++) {
        items[index].reset(new int(index));
    }
    save_memory(result);
    delete[] items;

    result.ms = elapsed_ms(started);
    return result;
}

Result measure(std::size_t count) {
    Result result;
    result.count = count;
    result.series[0] = benchmark_raw(count);
    result.series[1] = benchmark_unique(count);
    result.series[2] = benchmark_shared(count);
    result.series[3] = benchmark_std_unique(count);
    result.series[4] = benchmark_std_shared(count);
    return result;
}

void write_csv(const Result* results, std::size_t size) {
    std::ofstream file("benchmark_results.csv");
    if (!file) {
        throw std::runtime_error("Cannot create benchmark_results.csv");
    }

    file << "count";
    for (int series = 0; series < series_count; series++) {
        const char* name = series_csv_names[series];
        file << ',' << name << "_ms," << name << "_bytes," << name << "_allocations";
    }
    file << '\n';

    file << std::fixed << std::setprecision(3);
    for (std::size_t index = 0; index < size; index++) {
        file << results[index].count;
        for (int series = 0; series < series_count; series++) {
            const Measurement& item = results[index].series[series];
            file << ',' << item.ms << ',' << item.bytes << ',' << item.allocations;
        }
        file << '\n';
    }
}

double chart_value(const Result& row, int series, bool memory) {
    if (memory) {
        return row.series[series].bytes / (1024.0 * 1024.0);
    }
    return row.series[series].ms;
}

// x-координата точки с номером index на оси графика
double chart_x(std::size_t index, std::size_t size, int left, int width) {
    if (size == 1) {
        return left + width / 2.0;
    }
    return left + (double)index * width / (size - 1);
}

void draw_chart(std::ofstream& file, const Result* results, std::size_t size, int top, bool memory) {
    int left = 85;
    int width = 760;
    int height = 190;

    double maximum = 0.0;
    for (std::size_t index = 0; index < size; index++) {
        for (int series = 0; series < series_count; series++) {
            maximum = std::max(maximum, chart_value(results[index], series, memory));
        }
    }
    if (maximum == 0.0) {
        maximum = 1.0;
    }

    file << "<text x='85' y='" << top - 20 << "' font-size='18' font-weight='bold'>";
    if (memory) {
        file << "Memory requested from new for N live objects (MiB)";
    } else {
        file << "Elapsed time: create and delete N objects (ms)";
    }
    file << "</text>\n";

    file << "<path d='M" << left << ' ' << top << " V" << top + height << " H" << left + width;
    file << "' fill='none' stroke='#444'/>\n";
    file << std::fixed << std::setprecision(2);
    file << "<text x='8' y='" << top + 5 << "' font-size='12'>" << maximum << "</text>\n";
    file << "<text x='38' y='" << top + height + 4 << "' font-size='12'>0</text>\n";

    for (int series = 0; series < series_count; series++) {
        file << "<polyline fill='none' stroke='" << series_colors[series] << "' stroke-width='3' points='";
        for (std::size_t index = 0; index < size; index++) {
            double x = chart_x(index, size, left, width);
            double y = top + height * (1.0 - chart_value(results[index], series, memory) / maximum);
            file << x << ',' << y << ' ';
        }
        file << "'/>\n";
    }

    for (std::size_t index = 0; index < size; index++) {
        double x = chart_x(index, size, left, width);
        file << "<text x='" << x << "' y='" << top + height + 19 << "' font-size='11' text-anchor='middle'>";
        file << results[index].count << "</text>\n";
    }
}

void write_svg(const Result* results, std::size_t size) {
    std::ofstream file("benchmark_results.svg");
    if (!file) {
        throw std::runtime_error("Cannot create benchmark_results.svg");
    }

    file << "<svg xmlns='http://www.w3.org/2000/svg' width='900' height='620' viewBox='0 0 900 620'>\n";
    file << "<rect width='900' height='620' fill='white'/>\n";
    file << "<text x='85' y='28' font-size='21' font-weight='bold'>Smart pointer comparison</text>\n";

    // Легенда: название каждой линии её цветом
    for (int series = 0; series < series_count; series++) {
        file << "<text x='" << 85 + series * 150 << "' y='51' font-size='13' font-weight='bold' fill='";
        file << series_colors[series] << "'>" << series_names[series] << "</text>\n";
    }

    draw_chart(file, results, size, 95, false);
    draw_chart(file, results, size, 375, true);
    file << "<text x='85' y='603' font-size='12'>";
    file << "Memory: bytes requested from operator new, without malloc block headers.</text>\n";
    file << "</svg>\n";
}

std::size_t parse_max_count(int argc, char** argv) {
    if (argc == 1) {
        return 1000000;
    }
    if (argc != 2) {
        throw std::invalid_argument("Usage: benchmark [max_count]");
    }

    std::string text(argv[1]);
    std::size_t parsed = 0;
    unsigned long long value = 0;
    // stoull бросает исключение с неинформативным текстом "stoull", если в начале строки не число
    try {
        value = std::stoull(text, &parsed);
    } catch (const std::exception&) {
        parsed = 0;
    }
    if (parsed != text.size() || value == 0 || value > 100000000) {
        throw std::invalid_argument("max_count must be between 1 and 100000000");
    }
    return value;
}

void print_table_header(const char* title) {
    std::cout << '\n' << title << '\n';
    std::cout << std::setw(12) << "N";
    for (int series = 0; series < series_count; series++) {
        std::cout << std::setw(17) << series_names[series];
    }
    std::cout << '\n';
}

// what: 0 — время, 1 — байты, 2 — число выделений
void print_table(const Result* results, std::size_t size, const char* title, int what) {
    print_table_header(title);
    std::cout << std::fixed << std::setprecision(2);
    for (std::size_t index = 0; index < size; index++) {
        std::cout << std::setw(12) << results[index].count;
        for (int series = 0; series < series_count; series++) {
            const Measurement& item = results[index].series[series];
            if (what == 0) {
                std::cout << std::setw(17) << item.ms;
            } else if (what == 1) {
                std::cout << std::setw(17) << item.bytes;
            } else {
                std::cout << std::setw(17) << item.allocations;
            }
        }
        std::cout << '\n';
    }
}

int main(int argc, char** argv) {
    try {
        std::size_t maximum = parse_max_count(argc, argv);

        std::size_t standard_counts[] = {10, 1000, 100000, 1000000, 10000000, 100000000};
        Result results[7];
        std::size_t result_count = 0;
        for (std::size_t count : standard_counts) {
            if (count <= maximum) {
                results[result_count] = measure(count);
                result_count++;
            }
        }
        // Если maximum не из стандартного списка, замеряем и его
        if (result_count == 0 || results[result_count - 1].count != maximum) {
            results[result_count] = measure(maximum);
            result_count++;
        }

        print_table(results, result_count, "Time: create and delete N objects (ms)", 0);
        print_table(results, result_count, "Memory requested from new while N objects are alive (bytes)", 1);
        print_table(results, result_count, "Number of new calls", 2);
        std::cout << "\nMemory excludes malloc block headers (usually 16+ bytes per allocation).\n";

        write_csv(results, result_count);
        write_svg(results, result_count);
        std::cout << "Saved benchmark_results.csv and benchmark_results.svg\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}

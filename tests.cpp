#include <iostream>
#include <type_traits>
#include <utility>

#include "include/UniquePtr.h"
#include "include/SharedPtr.h"
#include "Лаба_2/include/ArraySequence.h"

int passed = 0;
int failed = 0;

void check(bool condition, const char* name) {
    if (condition) {
        passed++;
        std::cout << "[PASS] " << name << '\n';
    } else {
        failed++;
        std::cout << "[FAIL] " << name << '\n';
    }
}

// Считает, сколько объектов сейчас живо, чтобы находить утечки
struct Tracked {
    static int alive;
    int value;

    Tracked(int value = 0) : value(value) {
        alive++;
    }

    Tracked(const Tracked& other) : value(other.value) {
        alive++;
    }

    ~Tracked() {
        alive--;
    }
};

int Tracked::alive = 0;

// Base и Derived для проверки подтипизации; Derived::destroyed показывает, что вызвался деструктор наследника
struct Base {
    static int alive;

    Base() {
        alive++;
    }

    virtual ~Base() {
        alive--;
    }

    virtual int value() const {
        return 0;
    }
};

int Base::alive = 0;

struct Derived : Base {
    static int destroyed;
    int number;

    Derived(int number) : number(number) {}

    ~Derived() override {
        destroyed++;
    }

    int value() const override {
        return number;
    }
};

int Derived::destroyed = 0;

// Копирование UniquePtr запрещено: такой код не должен компилироваться
static_assert(!std::is_copy_constructible_v<UniquePtr<Tracked>>);
static_assert(!std::is_copy_assignable_v<UniquePtr<Tracked>>);
static_assert(!std::is_copy_constructible_v<UniquePtr<Tracked[]>>);
// UniquePtr<Derived> переходит в UniquePtr<Base> только перемещением
static_assert(!std::is_constructible_v<UniquePtr<Base>, UniquePtr<Derived>&>);

// Для неродственных классов приведение не компилируется (static_assert внутри конструктора).
// Чтобы проверить, раскомментируйте строку:
// SharedPtr<Base> wrong(SharedPtr<Tracked>(new Tracked));

void test_unique_move() {
    {
        UniquePtr<Tracked> first(new Tracked(5));
        UniquePtr<Tracked> moved(std::move(first));
        check(first.get() == nullptr, "UniquePtr move constructor empties the source");
        check(moved->value == 5, "UniquePtr move constructor takes the object");

        first = std::move(moved);
        check(moved.get() == nullptr, "UniquePtr move assignment empties the source");
        check(first->value == 5, "UniquePtr move assignment takes the object");

        // Через ссылку, чтобы компилятор не предупреждал о самоперемещении
        UniquePtr<Tracked>& same = first;
        first = std::move(same);
        check(first->value == 5, "UniquePtr self move assignment keeps the object");
    }
    check(Tracked::alive == 0, "UniquePtr moves leave no live objects");
}

void test_unique_release_and_reset() {
    UniquePtr<Tracked> pointer(new Tracked(3));
    Tracked* released = pointer.release();
    check(pointer.get() == nullptr, "UniquePtr release empties the pointer");
    check(released->value == 3, "UniquePtr release returns the object");

    pointer.reset(released);
    pointer.reset(pointer.get());
    check(pointer->value == 3 && Tracked::alive == 1, "UniquePtr reset with the same pointer keeps the object");

    pointer.reset();
    check(pointer.get() == nullptr, "UniquePtr reset empties the pointer");
    check(Tracked::alive == 0, "UniquePtr reset deletes the object");

    pointer.reset();
    check(pointer.get() == nullptr, "UniquePtr reset of an empty pointer does nothing");
    check(pointer.release() == nullptr, "UniquePtr release of an empty pointer returns nullptr");
}

void test_unique_array() {
    {
        UniquePtr<Tracked[]> array(new Tracked[3]);
        check(Tracked::alive == 3, "UniquePtr array owns all elements");

        array[1].value = 42;
        UniquePtr<Tracked[]> moved(std::move(array));
        check(array.get() == nullptr, "UniquePtr array move empties the source");
        check(moved[1].value == 42, "UniquePtr array supports indexing");

        moved.reset(moved.get());
        check(Tracked::alive == 3, "UniquePtr array reset with the same pointer keeps the array");

        moved.reset(new Tracked[2]);
        check(Tracked::alive == 2, "UniquePtr array reset deletes the old array");

        UniquePtr<Tracked[]> empty;
        empty.reset();
        check(empty.get() == nullptr, "UniquePtr array reset of an empty pointer does nothing");
    }
    check(Tracked::alive == 0, "UniquePtr array deletes every element");
}

void test_unique_subtyping() {
    Derived::destroyed = 0;
    {
        UniquePtr<Derived> derived(new Derived(7));
        UniquePtr<Base> base(std::move(derived));
        check(derived.get() == nullptr, "UniquePtr Derived to Base move empties the source");
        check(base->value() == 7, "UniquePtr Base calls Derived methods");

        UniquePtr<Derived> other(new Derived(9));
        base = std::move(other);
        check(other.get() == nullptr, "UniquePtr Derived to Base move assignment empties the source");
        check(base->value() == 9, "UniquePtr Derived to Base move assignment takes the object");
        check(Derived::destroyed == 1, "UniquePtr Derived to Base move assignment deletes the old object");

        UniquePtr<Derived> empty;
        base = std::move(empty);
        check(base.get() == nullptr, "UniquePtr empty Derived to Base move empties the target");
    }
    check(Derived::destroyed == 2 && Base::alive == 0, "UniquePtr Base deletes Derived objects through virtual destructor");
}

void test_shared_copy() {
    {
        SharedPtr<Tracked> first(new Tracked(5));
        SharedPtr<Tracked> copy(first);
        check(copy.get() == first.get(), "SharedPtr copy points to the same object");
        check(Tracked::alive == 1, "SharedPtr copy does not create a new object");
        check(copy.use_count() == 2, "SharedPtr copy increases use_count");

        first.reset();
        check(first.use_count() == 0, "SharedPtr use_count of an empty pointer is 0");
        check(copy.use_count() == 1, "SharedPtr reset decreases use_count");
        check(copy->value == 5, "SharedPtr keeps the object while it has an owner");

        SharedPtr<Tracked> other(new Tracked(8));
        other = copy;
        check(other.get() == copy.get(), "SharedPtr copy assignment shares the object");
        check(Tracked::alive == 1, "SharedPtr copy assignment deletes the old object");

        other = other;
        check(other->value == 5 && other.use_count() == 2, "SharedPtr self-assignment keeps the object");
    }
    check(Tracked::alive == 0, "SharedPtr deletes the object after the last owner");
}

void test_shared_empty() {
    SharedPtr<Tracked> empty;
    SharedPtr<Tracked> copy(empty);
    check(copy.get() == nullptr && copy.use_count() == 0, "SharedPtr copy of an empty pointer is empty");

    SharedPtr<Tracked> target(new Tracked(1));
    target = empty;
    check(target.get() == nullptr && Tracked::alive == 0, "SharedPtr assignment of an empty pointer deletes the old object");

    empty.reset();
    check(empty.use_count() == 0, "SharedPtr reset of an empty pointer does nothing");

    SharedPtr<Tracked> null_pointer(nullptr);
    check(null_pointer.use_count() == 0, "SharedPtr made from nullptr has no counter");
}

void test_shared_move() {
    {
        SharedPtr<Tracked> first(new Tracked(7));
        SharedPtr<Tracked> moved(std::move(first));
        check(first.get() == nullptr, "SharedPtr move constructor empties the source");
        check(moved->value == 7, "SharedPtr move constructor takes the object");
        check(first.use_count() == 0 && moved.use_count() == 1, "SharedPtr move constructor does not change use_count");

        first = std::move(moved);
        check(moved.get() == nullptr, "SharedPtr move assignment empties the source");
        check(first->value == 7, "SharedPtr move assignment takes the object");

        SharedPtr<Tracked>& same = first;
        first = std::move(same);
        check(first->value == 7 && first.use_count() == 1, "SharedPtr self move assignment keeps the object");

        // first и second владеют одним объектом: после перемещения владелец остаётся один
        SharedPtr<Tracked> second(first);
        first = std::move(second);
        check(second.get() == nullptr && first.use_count() == 1, "SharedPtr move inside one ownership group");
        check(Tracked::alive == 1, "SharedPtr move inside one ownership group keeps the object");
    }
    check(Tracked::alive == 0, "SharedPtr moves leave no live objects");
}

void test_shared_reset() {
    {
        SharedPtr<Tracked> first(new Tracked(1));
        first.reset(first.get());
        check(first->value == 1 && first.use_count() == 1, "SharedPtr reset with the same pointer keeps the object");

        SharedPtr<Tracked> second(first);
        first.reset(new Tracked(2));
        check(first->value == 2 && first.use_count() == 1, "SharedPtr reset takes a new object with its own counter");
        check(second->value == 1 && second.use_count() == 1, "SharedPtr reset keeps the old object for other owners");
    }
    check(Tracked::alive == 0, "SharedPtr reset leaves no live objects");
}

void test_shared_subtyping() {
    Derived::destroyed = 0;
    {
        SharedPtr<Derived> derived(new Derived(11));
        SharedPtr<Base> base(derived);
        check(base.get() == derived.get() && base->value() == 11, "SharedPtr Derived to Base copy shares the object");
        check(base.use_count() == 2, "SharedPtr Derived to Base copy increases use_count");

        derived.reset();
        check(base->value() == 11 && Derived::destroyed == 0, "SharedPtr Base keeps Derived object alive");

        SharedPtr<Derived> fourth(new Derived(14));
        SharedPtr<Base> assigned;
        assigned = fourth;
        check(assigned->value() == 14 && assigned.use_count() == 2, "SharedPtr Derived to Base copy assignment shares the object");

        SharedPtr<Derived> fifth(new Derived(15));
        SharedPtr<Base> moved(std::move(fifth));
        check(fifth.get() == nullptr && moved.use_count() == 1, "SharedPtr Derived to Base move constructor transfers ownership");

        SharedPtr<Derived> sixth(new Derived(16));
        assigned = std::move(sixth);
        check(sixth.get() == nullptr && assigned->value() == 16, "SharedPtr Derived to Base move assignment transfers ownership");
        check(fourth.use_count() == 1 && Derived::destroyed == 0, "SharedPtr Derived to Base move assignment releases the old object");

        // Объект общий у seventh и same_group: перемещение не должно его удалить
        SharedPtr<Derived> seventh(new Derived(17));
        SharedPtr<Base> same_group(seventh);
        same_group = std::move(seventh);
        check(seventh.get() == nullptr && same_group.use_count() == 1, "SharedPtr Derived to Base move inside one ownership group");
    }
    check(Derived::destroyed == 5 && Base::alive == 0, "SharedPtr Base deletes Derived objects through virtual destructor");
}

void test_array_sequence() {
    {
        SharedPtr<Tracked> input[] = {SharedPtr<Tracked>(new Tracked(1)), SharedPtr<Tracked>(new Tracked(2))};
        MutableArraySequence<SharedPtr<Tracked>> sequence(input, 2);
        check(sequence.get(0).get() == input[0].get(), "ArraySequence stores SharedPtr elements");

        sequence.append(SharedPtr<Tracked>(new Tracked(3)));
        check(sequence.get(2)->value == 3, "ArraySequence append adds an element");

        UniquePtr<Sequence<SharedPtr<Tracked>>> part(sequence.get_subsequence(1, 2));
        check(part->get(1).get() == sequence.get(2).get(), "ArraySequence subsequence shares objects");

        input[0].reset();
        check(sequence.get(0)->value == 1, "ArraySequence keeps objects after external reset");
    }
    check(Tracked::alive == 0, "ArraySequence deletes all stored objects");
}

int main() {
    test_unique_move();
    test_unique_release_and_reset();
    test_unique_array();
    test_unique_subtyping();
    test_shared_copy();
    test_shared_empty();
    test_shared_move();
    test_shared_reset();
    test_shared_subtyping();
    test_array_sequence();

    std::cout << "\nPassed: " << passed << "\nFailed: " << failed << '\n';
    if (failed != 0) {
        return 1;
    }
    return 0;
}

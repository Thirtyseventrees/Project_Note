#include <iostream>

#include "shared_ptr.hpp"

struct Test {
    int id;
    static int instance_count;

    Test(int i) : id(i) {
        std::cout << "Construct Test(" << id << ")\n";
        ++instance_count;
    }
    ~Test() {
        std::cout << "Destruct Test(" << id << ")\n";
        --instance_count;
    }
};

int Test::instance_count = 0;

int main() {
    {
        std::cout << "\n-- Create shared_ptr sp1 --\n";
        shared_ptr<Test> sp1(new Test(1));
        std::cout << "Use count: " << sp1.use_count() << "\n";

        std::cout << "\n-- Copy to sp2 --\n";
        shared_ptr<Test> sp2 = sp1;
        std::cout << "Use count after copy: " << sp1.use_count() << "\n";

        std::cout << "\n-- Move to sp3 --\n";
        shared_ptr<Test> sp3 = std::move(sp2);
        std::cout << "Use count after move: " << sp1.use_count() << "\n";

        std::cout << "\n-- Reset sp3 --\n";
        sp3.reset();
        std::cout << "sp3 reset. sp1 use_count: " << sp1.use_count() << "\n";

        std::cout << "\n-- Self assignment sp1 = sp1 --\n";
        sp1 = sp1;
        std::cout << "After self assignment: " << sp1.use_count() << "\n";

        std::cout << "\n-- Construct weak point wp1 = sp1 --\n";
        weak_ptr<Test> wp1 = sp1;
        std::cout << "After self assignment, use count: " << sp1.use_count() << "\n";
        std::cout << "After self assignment, weak count " << sp1.weak_count() << "\n";

        std::cout << "\n-- Construct weak point wp2(wp1) --\n";
        weak_ptr<Test> wp2(wp1);
        std::cout << "After self assignment, use count: " << sp1.use_count() << "\n";
        std::cout << "After self assignment, weak count " << sp1.weak_count() << "\n";

        std::cout << "\n-- Construct weak point wp3(std::move(wp1)) --\n";
        weak_ptr<Test> wp3 = std::move(wp1);
        std::cout << "After self assignment, use count: " << sp1.use_count() << "\n";
        std::cout << "After self assignment, weak count " << sp1.weak_count() << "\n";

        std::cout << "\n-- wp3.lock() --\n";
        shared_ptr<Test> sp4 = wp3.lock();
        wp3.lock();
        std::cout << "After self assignment, use count: " << sp1.use_count() << "\n";
        std::cout << "After self assignment, weak count " << sp1.weak_count() << "\n";
    }

    std::cout << "\n-- All shared_ptr out of scope --\n";
    std::cout << "Active Test instances: " << Test::instance_count << "\n";

    return 0;
}
// C++ program to demonstrate errors of using recursive_mutex
#include <iostream>
#include <thread>
#include <mutex>

// A recursive mutex object
std::recursive_mutex rm;

// A function that calls itself recursively
void f(int n)
{
    // Lock the recursive mutex
    rm.lock();
    std::cout << "Thread " << std::this_thread::get_id() << " entered f(" << n << ")\n";

    // Simulate some work
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Call itself with a smaller argument
    if (n > 0)
        f(n - 1);

    // Unlock the recursive mutex
    rm.unlock();
}

int main()
{
    // Create two threads that call the f function
    std::thread t1(f, 3);
    std::thread t2(f, 2);

    // Wait for both threads to finish
    t1.join();
    t2.join();

    return 0;
}

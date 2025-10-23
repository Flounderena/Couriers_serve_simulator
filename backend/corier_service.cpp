#include <iostream>
#include "Dispatcher.h"
#include <random>
#include <chrono>
#include <cstdlib> 

int main()
{
    unsigned seed = (unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::srand(seed);

    int num_of_offices, num_of_couriers, step;
    std::cin >> num_of_offices >> num_of_couriers >> step;

    Dispatcher d(num_of_offices, num_of_couriers, step);
    d.programm_work();

    return 0;
}

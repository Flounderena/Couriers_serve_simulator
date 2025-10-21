#include <iostream>
#include "Dispatcher.h"
#include <random>
#include <chrono>


int main()
{
    int num_of_offices, num_of_couriers, step; std::cin >> num_of_offices >> num_of_couriers >> step;

    Dispatcher d(num_of_offices, num_of_couriers, step);

    d.programm_work();

    return 0;

}





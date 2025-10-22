#pragma once
#include "Courier.h"
#include "Office.h"
#include <iostream>
#include <algorithm>


class Dispatcher
{
public:
    Dispatcher (int off, int cour, int s); // конструктор по 3 аргументам: кол-во филлиалов, кол-во курьеров, величина шага
    // в конструкторе создаётся массив филлиалов с координтатами (у меня координаты не больше 30,т.к. время перемещения = расстянию между филлиалами)
    // надо поменять 30 на размеры окна и в матрице путей делить расстояние на скорость курьера (желательно, чтобы в матрице ways не было чисел больше 20)
    ~Dispatcher() {}

    std::queue<Letter> get_delivered_letters(); // возвращает очередь доставленных писем

    std::queue<Letter> get_ordered_letters(); // возвращает очередь закаазанных, но не доставленных писем

    std::queue<Letter> get_waiting_letters(); // возвращает очередь писем, ждущих, пока за ними приедет курьер из другого филлиала (тоже недоставленные)

    int get_distance(int a, int b); // возвращает расстояние между филлиалами а и b 

    Office get_ofice(int n); // возвращает n тый филлиал 

    Courier get_courier(int n); // возвращает n того курьера

    int get_num_free_rides(); // возвращает количество холостых поездок

    int get_sum_of_deviations(); // возвращает суммарное время опозданий

    int get_free_rides_time(); // возвращает общее время холостых поездок

    void lett_deliver(Courier& cour, int t); // отмечает доставленные этим курьером к моменту времени t письма

    void equal_begin_time(Courier& cour);  // пересчитывает время доставки для каждого письма

    void programm_work(); // весь функционал программы

private:
	std::queue<Letter> letters;
	std::queue<Letter> letters_delivered;
	std::queue<Letter> wait_for_take;
	std::vector<Courier> couriers;
	std::vector<Office> offices;
	std::vector<std::vector<int>> ways;
	int num_free_rides, sum_of_deviations, letters_cnt, free_rides_time;
	int num_of_offices, num_of_couriers, step;
};

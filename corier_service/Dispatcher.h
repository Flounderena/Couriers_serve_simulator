#pragma once
#include "Courier.h"
#include "Office.h"
#include <iostream>
#include <algorithm>


class Dispatcher
{
public:
    Dispatcher (int off, int cour, int s); // ����������� �� 3 ����������: ���-�� ���������, ���-�� ��������, �������� ����
    // � ������������ �������� ������ ��������� � ������������� (� ���� ���������� �� ������ 30,�.�. ����� ����������� = ��������� ����� ����������)
    // ���� �������� 30 �� ������� ���� � � ������� ����� ������ ���������� �� �������� ������� (����������, ����� � ������� ways �� ���� ����� ������ 20)
    ~Dispatcher() {}

    std::queue<Letter> get_delivered_letters(); // ���������� ������� ������������ �����

    std::queue<Letter> get_ordered_letters(); // ���������� ������� �����������, �� �� ������������ �����

    std::queue<Letter> get_waiting_letters(); // ���������� ������� �����, ������, ���� �� ���� ������� ������ �� ������� �������� (���� ��������������)

    int get_distance(int a, int b); // ���������� ���������� ����� ���������� � � b 

    Office get_ofice(int n); // ���������� n ��� ������� 

    Courier get_courier(int n); // ���������� n ���� �������

    int get_num_free_rides(); // ���������� ���������� �������� �������

    int get_sum_of_deviations(); // ���������� ��������� ����� ���������

    int get_free_rides_time(); // ���������� ����� ����� �������� �������

    void lett_deliver(Courier& cour, int t); // �������� ������������ ���� �������� � ������� ������� t ������

    void equal_begin_time(Courier& cour);  // ������������� ����� �������� ��� ������� ������

    void programm_work(); // ���� ���������� ���������

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

#pragma once
#include <vector>
#include <queue>
#include <deque>
#include "Letter.h"


class Courier
{
public:

	Courier(int p); // ����������� �� 1 ���������: �������, � ������� ����������� ������ � ������� 1 ���
	~Courier() {}

	void change_status(bool f); // ������ ������ ������� �� f

	bool is_free(); // ����������, �������� �� ������

	void set_pos(int p); // �������������, � ����� �������� ������ 

	void take_let(Letter l); // ���������� ������ � ���������

	Letter first_let(); // ���������� 1 ������ � ���������

	Letter last_let(); // ���������� ��������� ������ � ���������

	void deliver_let(); // ������� ������ �� ��������� � ������� ������������ �����

	int get_pos(); // ����������, � ����� �������� ������ ������

	int get_num_of_let();  // ���������� ���������� ����� � ���������

	void move_to_end(); // ���������� ������ � ��������� �� ������ � ����� �������

	int get_mid_travels(); // ���������� ������� ����� �������

	std::pair<int, int> get_coordinates(std::pair<int, int> from, std::pair<int, int> to, int cur_time); // ���������� ���������� �������


private:
	bool free;
	int place, rides_time, rides_num;
	std::deque<Letter> backpack;
	std::vector<Letter> history;
};


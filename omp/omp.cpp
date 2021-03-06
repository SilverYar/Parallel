#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <thread>
#include <windows.h>
#include <omp.h>
using namespace std;
// Число потоков PROCCOUNT=2
#define PROCCOUNT 3

int part_text[PROCCOUNT];
int part_text_size[PROCCOUNT];
std::string text;
map <string, int> MapOfFreqs[PROCCOUNT];
void my_thread_proc(int procnum)
{
	int idx = part_text[procnum];
	int sz = part_text_size[procnum];
	for (int b = idx; b < idx + sz - 2; b++)
	{
		string str = text.substr(b, 3);
		int prob = 0;
		for (int i = 1; i < 4 && prob == 0; i++)
		{
			if (str[i - 1] == ' ')
				prob = i;
		}
		if (prob == 0)
		{
			map<string, int>::iterator it
				= MapOfFreqs[procnum].find(str);
			if (it != MapOfFreqs[procnum].end())
			{
				it->second++;
			}
			else
				MapOfFreqs[procnum].insert(pair<string, int>(str, 1));
		}
		else
			b += prob;
	}
}
int main() {
	ifstream t("textfile.txt");
	if (!t.is_open()) {
		cout << "Error\n";
		return 0;
	}
	setlocale(LC_ALL, "Russian");
	SetConsoleOutputCP(866);
	t.seekg(0, std::ios::end);
	size_t size = t.tellg();
	text.assign(size, ' ');
	t.seekg(0);
	t.read(&text[0], size);
	//выкидываем знаки препинания, подправляем регистр
	for (int i = 0; i < text.size(); i++)
	{
		if (text[i] >= 'А' && text[i] <= 'П')
		{
			text[i] = (text[i] - 'А' + 'а');
		}
		else if (text[i] >= 'Р' && text[i] <= 'Я')
		{
			text[i] = (text[i] - 'Р' + 'р');
		}
		else if (text[i] >= 'а' && text[i] <= 'п' || text[i] >= 'р' && text[i] <= 'я')
		{
		}
		else
		{
			text[i] = ' ';
		}
	}
	int help = text.size() / PROCCOUNT;

	for (int ii = 0; ii < PROCCOUNT; ii++) {
		part_text[ii] = help * ii;
		part_text_size[ii] = help;
	}

	//разделяем вычисления директивой
	#pragma omp parallel for num_threads(PROCCOUNT)
	for (int a = 0; a < PROCCOUNT; a++)
	{
		my_thread_proc(a);
	}
	/*//приступим к подсчёту частоты встреч
	std::vector<std::thread> threads;
	for (int a = 0; a < PROCCOUNT; a++)
	{
		std::thread thr(my_thread_proc, a);
		threads.emplace_back(std::move(thr));
	}
	for (auto& thr : threads) {
		thr.join();
	}*/
	//Cводим полученные ответы в один словарь
	map<string, int> MapOfFreq = MapOfFreqs[0];
	for (int i = 1; i < PROCCOUNT; i++)
	{
		for (map<string, int>::iterator it = MapOfFreqs[i].begin();
			it != MapOfFreqs[i].end(); ++it)
		{
			map<string, int>::iterator it2 = MapOfFreq.find(it->first);
			if (it2 != MapOfFreq.end())
			{
				it2->second += it->second;
			}
			else
				MapOfFreqs[i].insert(pair<string, int>(it->first, it->second));
		}
	}

	/*
	map <string, int> MapOfFreq;
	for (int a = 0; a < 4; a++)
		for (int b = delim[a]; b < delim[a + 1] - 2; b++)
		{
			string str = text.substr(b, 3);
			int prob = 0;
			for (int i = 1; i < 4 && prob == 0; i++)
			{
				if (str[i - 1] == ' ')
					prob = i;
			}
			if (prob == 0)
			{
				map<string, int>::iterator it = MapOfFreq.find(str);
				if (it != MapOfFreq.end())
				{
					it->second++;
				}
				else
					MapOfFreq.insert(pair<string, int>(str, 1));
			}
			else
				b += prob;
		}*/
		//выводим результат
	printf("Топ-20 результатов вероятностей в процентах:\n");
	int summ = 0;
	for (map<string, int>::iterator it = MapOfFreq.begin(); it != MapOfFreq.end();
		++it)
	{
		if (it->second > 2)
			summ += it->second;
	}
	//берём топ 20
	std::vector<std::pair<std::string, int>> top_ten(20);
	std::partial_sort_copy(MapOfFreq.begin(),
		MapOfFreq.end(),
		top_ten.begin(),
		top_ten.end(),
		[](std::pair<const std::string, int> const& l,
			std::pair<const std::string, int> const& r)
	{
		return l.second > r.second;
	});
	for (vector<pair<string, int>>::iterator it = top_ten.begin(); it != top_ten.end();
		it++)
		cout << it->first << " - " << (float)it->second / summ * 100 << endl;
}
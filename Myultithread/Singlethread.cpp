#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <windows.h>
using namespace std;

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
	std::string text(size, ' ');
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
	int help = text.size() / 4;
	int delim[5] = { 0, help,2 * help,3 * help,4 * help - 1 };
	//приступим к подсчёту частоты встреч
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
		}
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
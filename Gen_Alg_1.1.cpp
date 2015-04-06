#define _CRTDBG_MAP_ALLOC
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstddef>

using namespace std;

int _outputf(ofstream &f, float gene_x, float gene_y, int i)//Функция для записи

{
	if (i) f.open("D:\\plot.txt", 7);//Открываем файл
	if (!i) f.open("D:\\death.txt", 7);
	//Проверка успешности открытия файла:
	if (f.fail()) {
		cout << "\n Error of open file"; //Ошибка открытия файла
		return 1;
	}
	f << gene_x << '\t' << gene_y << "\n\n"; //записываем переменную в файл
	f.close();//Закрываем файл
	return 0;
}

struct animal
{
	float gene_x, gene_y;
	float survive = 1;

	animal& operator=(const animal& zebra)
	{
		survive = zebra.survive;
		gene_x = zebra.gene_x;
		gene_y = zebra.gene_y;
		return *this;
	}
};

class Genalg
{
private:
	float Xc, Yc, accuracy;
	int R;
	vector<animal> pop;
	animal solution;
	int iterations;
	int sz; // number of animals;
	int add = 0;
	int mutati = 0;
	int flag = 1;
public:
	Genalg(float Xc_ = 0, float Yc_ = 0, int R_ = 10, float accuracy_ = 1)
	{
		R_ = abs(R_) % 100;
		if (R_ == 0)
		{
			cout << "Minimum point is (0, 0);\n";
		}
		else
		{
			Xc = Xc_; Yc = Yc_; R = R_; accuracy = accuracy_;
			cout << "How much animals do you want?\n";
			cin >> sz;
			pop.reserve(sz);
			srand(time(NULL));

			for (int i = 0; i < sz; i++)
			{
				animal zebra;
				zebra.gene_x = rand() % (2 * R) - R;
				zebra.gene_y = rand() % (2 * R) - R;
				pop.push_back(zebra);
			}
		}
	}
	// R или вокруг Xc, Yc  или вокруг нуля! (сейчас ноль)
	~Genalg()
	{
		pop.clear();
		pop.shrink_to_fit();
	}

	float f(float x, float y)
	{
		return ((x - Xc)*(x - Xc) + (y - Yc)*(y - Yc)) + 2;
	}

	// not clever choise
	void GetIndex(vector<int>& num)
	{
		srand(time(NULL));

		for (int i = 0; i < sz; i++) num[i] = i;
		for (int i = sz - 1; i > 0; i--)
		{
			int cur = rand() % (i);
			int remember = num[cur];
			num[cur] = num[i];
			num[i] = remember;
		}
	}

	//find the best child
	void Breed(const animal& parent1, const animal& parent2, animal& child)
	{
		float progress = 1.5;
		float better_x;
		float better_y;
		float the_best;
		if (parent1.survive > parent2.survive)
		{
			the_best = parent2.survive;
			better_x = (abs(parent2.gene_x * progress) < R) ? parent2.gene_x * progress : ((parent2.gene_x > 0) ? R : (-1) * R);
			better_y = (abs(parent2.gene_y * progress) < R) ? parent2.gene_y * progress : ((parent2.gene_y > 0) ? R : (-1) * R);
		}
		else
		{
			the_best = parent1.survive;
			better_x = (abs(parent1.gene_x * progress) < R) ? parent1.gene_x * progress : ((parent1.gene_x > 0) ? R : (-1) * R);
			better_y = (abs(parent1.gene_y * progress) < R) ? parent1.gene_y * progress : ((parent1.gene_y > 0) ? R : (-1) * R);
		}
		// grad(f)x
		if (f(better_x, better_y) < f(better_x / progress, better_y))
		{
			child.gene_x = better_x;
		}
		else
		{
			child.gene_x = better_x + (parent1.gene_x + parent2.gene_x - 2 * better_x) * (the_best / (parent1.survive + parent2.survive)) ;
		}
		//grad(f)y
		if (f(better_x, better_y) < f(better_x, better_y / progress))
		{
			child.gene_y = better_y;
		}
		else
		{
			child.gene_y = better_y + (parent1.gene_y + parent2.gene_y - 2 * better_y) * (the_best / (parent1.survive + parent2.survive));
		}

		//свиней обрезать
		if (abs(child.gene_x) > R)
		{
			child.gene_x = ((child.gene_x > 0) ? R : (-1) * R);
		}
		if (abs(child.gene_y) > R)
		{
			child.gene_y = ((child.gene_y > 0) ? R : (-1) * R);
		}
	}

	// create children
	void Crossing()
	{
		vector<int> num_parent(sz);
		GetIndex(num_parent); // ок, сделать разделение на хороших и плохих

		for (int j = 0; j < sz; j++)
		{
			animal child;
			Breed(pop[num_parent[j]], pop[j], child);
			pop.push_back(child);
		}
		add = pop.size() - sz; //кол-во детей
		sz = pop.size();
		num_parent.clear();
		num_parent.shrink_to_fit();
	}

	// z = (x-Xc)^2 + (y-Yc)^2; unsigned variant
	void Life(ofstream& fout)
	{
		for (int i = 0; i < sz; i++)
		{
			pop[i].survive = f(pop[i].gene_x, pop[i].gene_y);
			_outputf(fout, pop[i].gene_x, pop[i].gene_y, 1);
			cout << pop[i].survive << endl;
		}
	}

	// kill bad animal
	void Death(ofstream& fout)
	{
		int index = 1; // bubble sort
		for (int i = 0; i < sz; i++)
		{
			if (!index) { break; }
			index = 0;
			for (int j = 0; j < sz - 1; j++)
			{
				if (pop[j].survive > pop[j + 1].survive)
				{
					index++;
					animal remember = pop[j + 1];
					pop[j + 1] = pop[j];
					pop[j] = remember;
				}
			}
		}

		for (int death = sz - add; death < sz; death++)
		{
			_outputf(fout, pop[death].gene_x, pop[death].gene_y, 0);
		}
		pop.erase(pop.begin() + sz - add, pop.end());
		pop.shrink_to_fit();
		sz = pop.size();
	}

	void Mutation()
	{
		srand(time(NULL));
		for (int i = 0; i < sz; i++)
		{
			float brain = (Winner(100 * accuracy)) ? 0.1 : 1;
			float medicine = (Winner(10 * accuracy)) ? accuracy : accuracy * 5; // more accuracy
			if (rand() % 2) // 50:50
			{
				mutati++;
				pop[i].gene_x += brain * medicine * ((rand() % (int)(abs(pop[i].gene_x) + 1)) - pop[i].gene_x / 2);
				pop[i].gene_y += brain * medicine * ((rand() % (int)(abs(pop[i].gene_y) + 1)) - pop[i].gene_y / 2);
			}
			
			// переселить часть животных на ту сторону "обрыва" (чтобы не подходили к мин с одной стороны)
			if (Winner(10 * accuracy) && flag)
			{
				for (int i = 0; i < sz / 2; i++)
				{
					float x1 = pop[i].gene_x, x2 = pop[sz - i - 1].gene_x, y1 = pop[i].gene_y, y2 = pop[sz - i - 1].gene_y;

					pop[sz - i - 1].gene_x = x1 + 10 * accuracy * ((x1 > x2) ? 1 : (-1));
					pop[sz - i - 1].gene_y = (y1 - y2) * (x1 + 10 * accuracy * ((x1 > x2) ? 1 : (-1)) - x2) / (x1 - x2) + y2;
				}
				flag = 0;
			}
		}
	}

	int Winner(float r)
	{
		float delta_x, delta_y;
		for (int i = 1; i < sz; i++)
		{
			delta_x = (pop[0].gene_x - pop[i].gene_x) * (pop[0].gene_x - pop[i].gene_x);
			delta_y = (pop[0].gene_y - pop[i].gene_y) * (pop[0].gene_y - pop[i].gene_y);
			if ((delta_x + delta_y) > r) { return 0; }
		}
		return 1;
	}
	// good?

	int Again()
	{
		iterations++;
		if (iterations == 1000) { return 0; }
		if (solution.survive == 0) { return 0; }
		if (Winner(accuracy))
		{
			solution = pop[0];
			return 0;
		}
		else
		{
			return 1;
		}
	}

	int GetIterations()
	{
		return iterations;
	}

	animal& GetSolution()
	{
		return solution;
	}
};

animal& Solution(Genalg& example, ofstream& fout)
{
	do 
	{
		example.Death(fout); // kill some animals
		example.Crossing(); //create kid
		example.Mutation();
		example.Life(fout);  // z = (x-Xc)^2 + (y-Yc)^2; unsigned variant
	} while (example.Again());
	return example.GetSolution();
}

//Attantion!
//Надо переписать Live (под митоз и мейоз), Death используя add.
//расширить "покрытие", чтобы не кидалась на первый же минимум
void main()
{
	ofstream fout;
	//clear files
	{
		fout.open("D:\\death.txt");
		fout << "#xi\t#yi\t#xi\t#yi\n\n\n";
		fout.close();
		fout.open("D:\\plot.txt");
		fout << "#xi\t#yi\t#xi\t#yi\n\n\n";
		fout.close();
		fout.open("D:\\solution.txt");
		fout << "#xi\t#yi\t#xi\t#yi\n\n\n";
		fout.close();
	}
	float Xc, Yc, accuracy;
	int R;
	cout << "Enter R, A, B and accuracy\nfor  f(x, y) = (x-A)^2 + (y-B)^2\naccuracy often 0.1\n";
	cin >> R >> Xc >> Yc >> accuracy;
	Genalg example(Xc, Yc, R, accuracy);
	animal great = Solution(example, fout);
	cout << "Iterations:" << example.GetIterations() << endl;
	cout << "Minimum point is (" << great.gene_x << " +/- " << accuracy << ", " << great.gene_y << " +/- " << accuracy << ");\n";
	fout.open("D:\\solution.txt", 7);
	fout << great.gene_x << '\t' << great.gene_y << endl;
	fout.close();
	_CrtDumpMemoryLeaks();
}


//Нужно управлять точностью. Чтобы Accuracy был справедлив и указывал верный диапазон (а реально ли???)

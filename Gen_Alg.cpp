#define _CRTDBG_MAP_ALLOC //для поиска утечек памяти
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h> //
#include <crtdbg.h> // для утечек
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
	f << gene_x << '\t' << gene_y << "\n\n";//записываем переменную в файл
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
	int crap = 0;
	int add = 0;
	int mutati = 0;
	int flag = 0;
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
		if (parent1.survive > parent2.survive)
		{
			better_x = (abs(parent2.gene_x * progress) < R) ? parent2.gene_x * progress : ((parent2.gene_x > 0) ? R : (-1) * R);
			better_y = (abs(parent2.gene_y * progress) < R) ? parent2.gene_y * progress : ((parent2.gene_y > 0) ? R : (-1) * R);
		}
		else
		{
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
			child.gene_x = (parent1.gene_x + parent2.gene_x) / 2;
		}
		//grad(f)y
		if (f(better_x, better_y) < f(better_x, better_y / progress))
		{
			child.gene_y = better_y;
		}
		else
		{
			child.gene_y = (parent1.gene_y + parent2.gene_y) / 2;
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
		if (crap) { return; }
		vector<int> num_parent(sz);
		GetIndex(num_parent); // ок, сделаем разделение на хороших и плохих

		//if (iterations % 100 == 0)
		//{
		//	int i;
		//}

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
		if (crap) { return; }
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
		if (crap) { return; }

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
		if (crap) { return; }
		srand(time(NULL));
		for (int i = 0; i < sz; i++)
		{
			float brain = (iterations > 50) ? 0.01 : 1;
			float medicine = (iterations > 100) ? 0.001 : 0.01; // more accuracy
			if (rand() % 2) // 50:50
			{
				mutati++;
				pop[i].gene_x += brain * medicine * ((rand() % (int)(abs(pop[i].gene_x) + 1)) - pop[i].gene_x / 2);
				pop[i].gene_y += brain * medicine * ((rand() % (int)(abs(pop[i].gene_y) + 1)) - pop[i].gene_y / 2);
			}
			//if (rand() % 50 == 7)
			//{
			//	pop[i].gene_x = rand() % (2 * R) - R;
			//	pop[i].gene_y = rand() % (2 * R) - R;
			//}
		}
	}

	int Winner()
	{
		//return (pop[sz - 1].survive > 0.99) ? 1 : 0; bad
		float delta_x, delta_y;
		for (int i = 1; i < sz; i++)
		{
			delta_x = (pop[0].gene_x - pop[i].gene_x) * (pop[0].gene_x - pop[i].gene_x);
			delta_y = (pop[0].gene_y - pop[i].gene_y) * (pop[0].gene_y - pop[i].gene_y);
			if ((delta_x + delta_y) > accuracy) { return 0; }
		}
		return 1;
	}
	// good?

	int Again()
	{
		if (crap)
		{
			cout << "It's a crap\n";
			return 0;
		}
		if (iterations % 100 == 0)
		{
			int i = 0;
		}
		iterations++;
		//if (iterations == 300) 
		//{ 
		//	solution = pop[0];
		//	return 0; 
		//}
		if (solution.survive == 0) { return 0; }
		if (Winner())
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

	void GetAnswer()
	{
		cout << "Answer is (" << Xc << ", " << Yc << ");\n";
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
	
	float Xc, Yc, accuracy;
	int R;
	fout.open("D:\\death.txt");
	fout << "#xi\t#yi\t#xi\t#yi\n\n\n";
	fout.close();
	fout.open("D:\\plot.txt");
	fout << "#xi\t#yi\t#xi\t#yi\n\n\n";
	fout.close();
	fout.open("D:\\solution.txt");
	fout << "#xi\t#yi\t#xi\t#yi\n\n\n";
	fout.close();
	cout << "Enter R, Xc, Yc and accuracy\n";
	cin >> R >> Xc >> Yc >> accuracy;
	Genalg example(Xc, Yc, R, accuracy);
	animal great = Solution(example, fout);
	cout << "Iterations:" << example.GetIterations() << endl;
	cout << "Minimum point is (" << great.gene_x << " +/- " << accuracy << ", " << great.gene_y << " +/- " << accuracy << ");\n";
	example.GetAnswer();
	fout.open("D:\\solution.txt", 7);
	fout << great.gene_x << '\t' << great.gene_y << endl;
	fout.close();
	_CrtDumpMemoryLeaks();
}

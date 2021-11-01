#include <iostream>
#include <random>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <fstream>
#include <utility>
using namespace std;

struct podproblem {
	vector<int> ciag;
	int liczba;
	int wartosc;
};


void obliczDwumian(vector<int> arr, int index, vector<int> subarr, vector<vector<int>>& podzbiory) {

	if (index == arr.size()) {
		int l = subarr.size();
		if (l != 0) {
			podzbiory.push_back(subarr);
		}
	}
	else
	{
		obliczDwumian(arr, index + 1, subarr, podzbiory);
		subarr.push_back(arr[index]);
		obliczDwumian(arr, index + 1, subarr, podzbiory);
	}
}



void generacjaGrafu(int rozmiar, vector<vector<int>>& graf) {
	vector<int> wierzcholki;
	wierzcholki.resize(rozmiar);
	graf.clear();
	graf.resize(rozmiar, vector<int>(rozmiar, INT_MAX));
	for (int i = 0; i < rozmiar; i++) {
		for (int j = 0; j < rozmiar; j++) {
			if (i == j) {
				graf[i][j] = 0;
			}
			else {
				graf[i][j] = rand() % 150 + 1;
			}
		}
		wierzcholki[i] = i;
	}

	int wierzcholekA = 0, wierzcholekB = 0;
	wierzcholki.erase(find(wierzcholki.begin(), wierzcholki.end(), wierzcholekA));
	for (int i = 0; i < rozmiar - 1; i++) {
		wierzcholekB = wierzcholki[rand() % wierzcholki.size()];
		wierzcholki.erase(find(wierzcholki.begin(), wierzcholki.end(), wierzcholekB));
		graf[wierzcholekA][wierzcholekB] = 0;
		wierzcholekA = wierzcholekB;
	}
	graf[wierzcholekB][0] = 0;
}

void wyswietl(vector<vector<int>> graf) {
	for (int i = 0; i < graf.size(); i++) {//wywietlenie wierszy
		for (int j = 0; j < (int)graf.size(); j++) {//wyswtielenie kolumn
			cout << graf[i][j];
			if (graf[i][j] > 9) {
				if (graf[i][j] < 100) {
					cout << "  ";
				}
				else {
					cout << " ";
				}
			}
			else {
				cout << "   ";
			}
		}
		cout << endl;
	}
}

int wczytaj(vector<vector<int>>& graf, string nazwa) {
	int wartoscOptymalna;
	int rozmiar;
	fstream dane;
	string sciezka = "dane\\" + nazwa + ".tsp";
	dane.open(sciezka, ios::in);
	dane >> sciezka;
	dane >> rozmiar;
	cout << sciezka << endl;
	graf.resize(rozmiar, vector<int>(rozmiar, INT_MAX));
	for (int i = 0; i < rozmiar; i++) {
		for (int ii = 0; ii < rozmiar; ii++) {
			dane >> graf[i][ii];
		}
	}
	wyswietl(graf);

	dane >> wartoscOptymalna;
	return wartoscOptymalna;
}

std::pair < int, vector<int>> bruteForce(vector<vector<int>> graf) {
	int i, minDroga = INT_MAX, droga;
	vector<int> wierzcholki, najlepszaDroga;
	wierzcholki.resize(graf.size());
	for (i = 0; i < graf.size(); i++) {
		wierzcholki[i] = i;
	}
	i = 0;
	do {
		droga = 0;
		for (i = 0; i < graf.size() - 1; i++) {//obliczenie drogi
			droga += graf[wierzcholki[i]][wierzcholki[i + 1]];
		}
		droga += graf[wierzcholki[i]][wierzcholki[0]];
		if (droga < minDroga) {//porownianie wynikow
			minDroga = droga;
			najlepszaDroga = wierzcholki;
		}

	} while (next_permutation(wierzcholki.begin(), wierzcholki.end()));

	return std::make_pair(minDroga, najlepszaDroga);
}

int dynamicProgramming(vector<vector<int>> graf) {
	int minDroga = INT_MAX, droga;
	vector<int> najlepszaDroga;
	vector<podproblem> podproblemy;
	vector<int> ciag;
	vector<int> tablica;

	for (int i = 1; i < graf.size(); i++) {
		podproblem temp;
		temp.liczba = i;
		temp.ciag.push_back(i);
		temp.wartosc = graf[0][i];
		podproblemy.push_back(temp);
		tablica.push_back(i);
	}

	for (int i = 1; i < graf.size() - 1; i++) {
		vector<vector<int>> podzbior;
		vector<int> dummy;
		obliczDwumian(tablica, i + 1, dummy, podzbior);
		for (int ii = 0; ii < podzbior.size(); ii++) {
			for (int iii = 0; iii < i; iii++) {
				podproblem temp;
				temp.ciag = podzbior[ii];
				temp.liczba = iii;
				temp.wartosc = graf[iii][ii];
			}
		}
	}

	return minDroga;
}



int main()
{
	srand(time(NULL));
	string nazwa;
	time_t start, koniec;
	vector<vector<int>> graf;//graf na którym będziemy pracować
	long long czas;
	int wartoscOptymalna;
	fstream wynik;

	cin >> nazwa;
	wynik.open("wynik.txt", ios::app);//plik do zapisu

	//generacjaGrafu(rozmiar, graf);//generacja danych
	wartoscOptymalna = wczytaj(graf, nazwa);//wczytanie danych z pliku

	start = clock();
	auto wynik = bruteForce(graf);//wykonanie algorytmu
	koniec = clock();


	auto p = std::make_pair(1, 3.14);



	czas = koniec - start;//obliczenie czasu
	wynik << nazwa << " " << czas;//zapis
	cout << "czas = " << czas / CLOCKS_PER_SEC << "s,\n";
	cout << "wykryta droga = " << std::get<0>(p);
	cout << "\noptymalna droga = " << wartoscOptymalna;
	cout << "\nblad = " << (1.0 * wynik.get() / wartoscOptymalna) - 1;
	return 0;
}

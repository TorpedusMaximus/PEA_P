#include <iostream>
#include <random>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <fstream>
using namespace std;



void wypelnienieGrafu(int rozmiar, vector<vector<int>>& graf) {
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

void bruteForce(vector<vector<int>> graf) {
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


}

void tester(int rozmiar) {
	fstream wynik;
	wynik.open("wynik.txt", ios::app);//plik do zapisu
	time_t start, koniec;
	vector<vector<int>> graf;//graf na którym będziemy pracować
	long long czas;

	wypelnienieGrafu(rozmiar, graf);//generacja danych
	start = clock();
	bruteForce(graf);//wykonanie algorytmu
	koniec = clock();
	czas = koniec - start;//obliczenie czasu
	wynik << endl << rozmiar << " " << czas;//zapis
}

int main()
{
	srand(time(NULL));
	int rozmiar;
	cin >> rozmiar;
	tester(rozmiar);
	return 0;
}

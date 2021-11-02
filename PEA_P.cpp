#include <iostream>
#include <random>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <fstream>
#include <string>
#include <utility>
#include <unordered_map>
using namespace std;


struct podproblem {
	int liczba;
	int wartosc;
};

struct wynikAlgorytmu {
	vector<int> ciag;
	int wartosc;
};

void obliczDwumian(vector<int> graf, int index, vector<int> podzbior, vector<vector<int>>& podzbiory, int stopien) {
	if (index == graf.size()) {
		int l = podzbior.size();
		if (l != 0) {
			if (podzbior.size() == stopien) {
				podzbiory.push_back(podzbior);
			}
		}
	}
	else {
		obliczDwumian(graf, index + 1, podzbior, podzbiory, stopien);
		podzbior.push_back(graf[index]);
		obliczDwumian(graf, index + 1, podzbior, podzbiory, stopien);
	}
}

void generacjaGrafu(int rozmiar, vector<vector<int>>& graf) {
	vector<int> wierzcholki;
	wierzcholki.resize(rozmiar);
	graf.clear();
	graf.resize(rozmiar, vector<int>(rozmiar, INT_MAX));
	for (int i = 0; i < rozmiar; i++) {
		for (int ii = 0; ii <= i; ii++) {
			if (i == ii) {
				graf[i][ii] = 0;
			}
			else {
				graf[i][ii] = rand() % 150 + 1;
				graf[ii][i] = graf[i][ii];
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

void wyswietlWiersz(vector<int> graf) {
	for (int j = 0; j < (int)graf.size(); j++) {//wyswtielenie wierszy
		cout << graf[j];
		if (graf[j] > 9) {
			if (graf[j] < 100) {
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

void wyswietl(vector<vector<int>> graf) {
	for (int i = 0; i < graf.size(); i++) {//wywietlenie kolumn
		wyswietlWiersz(graf[i]);
	}
}

int wczytaj(vector<vector<int>>& graf, string nazwa) {
	int wartoscOptymalna;
	int rozmiar;
	fstream dane;
	string sciezka = "dane\\" + nazwa + ".tsp";
	dane.open(sciezka, ios::in);
	if (!dane) {
		generacjaGrafu(stoi(nazwa), graf);
		rozmiar = stoi(nazwa);
		wartoscOptymalna = 0;
		cout << nazwa << endl;
	}
	else {
		dane >> sciezka;
		dane >> rozmiar;
		cout << sciezka << endl;
		graf.resize(rozmiar, vector<int>(rozmiar, INT_MAX));
		for (int i = 0; i < rozmiar; i++) {
			for (int ii = 0; ii < rozmiar; ii++) {
				dane >> graf[i][ii];
			}
		}
		dane >> wartoscOptymalna;
	}
	return wartoscOptymalna;
}

wynikAlgorytmu bruteForce(vector<vector<int>> graf) {
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

	wynikAlgorytmu wynik;
	wynik.ciag = najlepszaDroga;
	wynik.wartosc = minDroga;

	return wynik;
}

wynikAlgorytmu dynamicProgramming(vector<vector<int>> graf) {
	unordered_map<string, podproblem> podproblemy;
	vector<int> tablica;
	podproblem najlepszaWartosc, wartosc;
	unsigned long bits, prev;

	for (int i = 1; i < graf.size(); i++) {
		wartosc.liczba = i;
		wartosc.wartosc = graf[0][i];
		podproblemy[to_string(1 << i) + to_string(i)] = wartosc;
		tablica.push_back(i);
	}

	for (int i = 2; i < graf.size(); i++) {
		vector<vector<int>> podzbiory;
		vector<int> dummy;
		obliczDwumian(tablica, 0, dummy, podzbiory, i);

		for (auto podzbior : podzbiory) {
			bits = 0;
			for (auto bit : podzbior) {
				bits |= 1 << bit;
			}

			for (auto k : podzbior) {
				prev = bits & ~(1 << k);
				najlepszaWartosc.liczba = INT_MAX;
				najlepszaWartosc.wartosc = INT_MAX;

				for (auto m : podzbior) {
					if (m == k) {
						continue;
					}
					wartosc.wartosc = podproblemy[to_string(prev) + to_string(m)].wartosc + graf[m][k]; //new value
					wartosc.liczba = m;
					najlepszaWartosc = (najlepszaWartosc.wartosc < wartosc.wartosc) ? najlepszaWartosc : wartosc; // if best < value than best else value
				}
				podproblemy[to_string(bits) + to_string(k)] = najlepszaWartosc;
			}
		}
	}

	bits = (1 << graf.size()) - 2;
	najlepszaWartosc.liczba = INT_MAX;
	najlepszaWartosc.wartosc = INT_MAX;

	for (int i = 1; i < graf.size() - 1; i++) {

		wartosc.wartosc = podproblemy[to_string(bits) + to_string(i)].wartosc + graf[i][0];
		najlepszaWartosc = (najlepszaWartosc.wartosc < wartosc.wartosc) ? najlepszaWartosc : wartosc;
	}

	wartosc = najlepszaWartosc;
	tablica.clear();
	tablica.resize(graf.size() + 1, 0);

	for (int i = 1; i < graf.size(); i++) {
		tablica[i] = wartosc.liczba;
		wartosc = podproblemy[to_string(bits) + to_string(tablica[i])];
		bits = bits & ~(1 << tablica[i]);
	}
	wynikAlgorytmu wynik;
	wynik.ciag = tablica;
	wynik.wartosc = najlepszaWartosc.wartosc;

	return wynik;
}

void test(string nazwa) {
	srand(time(NULL));
	time_t start, koniec;
	vector<vector<int>> graf;//graf na którym będziemy pracować
	long long czas;
	int wartoscOptymalna = -1;
	fstream wynik;


	wynik.open("wynik.txt", ios::app);//plik do zapisu

	wartoscOptymalna = wczytaj(graf, nazwa);//wczytanie danych z pliku

	wyswietl(graf);
	start = clock();
	wynikAlgorytmu wyniki = dynamicProgramming(graf);//wykonanie algorytmu
	koniec = clock();

	czas = koniec - start;//obliczenie czasu
	wynik << nazwa << ";" << czas / CLOCKS_PER_SEC << endl;//zapis
	cout << "czas = " << czas / CLOCKS_PER_SEC << "s" << endl;
	cout << "wykryta droga = ";
	wyswietlWiersz(wyniki.ciag);
	cout << "dlugosc drogi = " << wyniki.wartosc << endl;
	cout << "optymalna droga = " << wartoscOptymalna << endl;
	if (wartoscOptymalna == 0) {
		cout << "blad = " << (1.0 * wyniki.wartosc / 1) * 100 << "%";
	}
	else {
		cout << "blad = " << ((1.0 * wyniki.wartosc / wartoscOptymalna) - 1) * 100 << "%";
	}

}


int main()
{
	test("20");

	return 0;
}

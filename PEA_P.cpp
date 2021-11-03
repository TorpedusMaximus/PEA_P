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


struct podproblem {//struktura ułatwia pracę funkcji dynamicProgramming 
	int liczba;
	int wartosc;
};

struct wynikAlgorytmu {//struktura zwrotna dla funkcji obliczających problem
	vector<int> ciag;
	int wartosc;
};

void obliczDwumian(vector<int> graf, int index, vector<int> podzbior, vector<vector<int>>& podzbiory, int stopien) {
	//funkcja oblicza kolejne podzbiory wierzchołków o podanej wielkości
	if (index == graf.size()) {
		int l = podzbior.size();
		if (l != 0) {
			if (podzbior.size() == stopien) {//sprawdzenie długosci zbioru
				podzbiory.push_back(podzbior);//zapis podzbioru
			}
		}
	}
	else {
		obliczDwumian(graf, index + 1, podzbior, podzbiory, stopien);//rekurencja
		podzbior.push_back(graf[index]);
		obliczDwumian(graf, index + 1, podzbior, podzbiory, stopien);
	}
}

void generacjaGrafu(int rozmiar, vector<vector<int>>& graf) {//funkcja generująca graf
	vector<int> wierzcholki;
	wierzcholki.resize(rozmiar);
	graf.clear();
	graf.resize(rozmiar, vector<int>(rozmiar, INT_MAX));//inicjacja macierzy wag
	for (int i = 0; i < rozmiar; i++) {
		for (int ii = 0; ii <= i; ii++) {
			if (i == ii) {
				graf[i][ii] = 0;//przekątna macierzy
			}
			else {
				graf[i][ii] = rand() % 150 + 1;//losowanie wag
				graf[ii][i] = graf[i][ii];
			}
		}
		wierzcholki[i] = i;
	}

	int wierzcholekA = 0, wierzcholekB = 0;//utworzenie cyklu hamiltona o wartosci 0 dla pewności działania algorytmu
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
				cout << "  ";//formatowanie układu wag
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

int wczytaj(vector<vector<int>>& graf, string& nazwa, string& sciezka) {
	int wartoscOptymalna;
	int rozmiar;
	fstream dane;
	sciezka = "dane\\" + nazwa + ".tsp";//określenie scieżki do pliku
	dane.open(sciezka, ios::in);
	if (!dane) {//przy braku pliku generuj własny graf
		generacjaGrafu(stoi(nazwa), graf);
		rozmiar = stoi(nazwa);
		wartoscOptymalna = 0;
		sciezka = nazwa;
	}
	else {
		dane >> sciezka;
		dane >> rozmiar;
		graf.resize(rozmiar, vector<int>(rozmiar, INT_MAX));
		for (int i = 0; i < rozmiar; i++) {//wczytywanie z pliku
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
	unordered_map<string, podproblem> podproblemy;//zbior wszystkich podproblemów
	vector<int> tablica;
	podproblem najlepszaWartosc, wartosc;//zmienne 
	unsigned long ciag, poprzedni;

	for (int i = 1; i < graf.size(); i++) {//wypełnienie wartości najktrótszych podproblemów
		wartosc.liczba = i;
		wartosc.wartosc = graf[0][i];
		podproblemy[to_string(1 << i) + to_string(i)] = wartosc;
		tablica.push_back(i);//zmienna pomocnicza do innego miejsca w kodzie
	}

	for (int i = 2; i < graf.size(); i++) {//obliczenie wartości wszytskich podzbiorów
		vector<vector<int>> podzbiory;//podzbiory danego poziomu 
		vector<int> dummy;
		obliczDwumian(tablica, 0, dummy, podzbiory, i);//obliczenie podzbiorów

		for (auto podzbior : podzbiory) {
			ciag = 0;
			for (auto bit : podzbior) {
				ciag |= 1 << bit;
			}

			for (auto k : podzbior) {
				poprzedni = ciag & ~(1 << k);
				najlepszaWartosc.liczba = INT_MAX;//zmienna przechowująca minimum
				najlepszaWartosc.wartosc = INT_MAX;

				for (auto m : podzbior) {//szukanie podzbioru o minimalnej wartości
					if (m == k) {
						continue;
					}
					wartosc.wartosc = podproblemy[to_string(poprzedni) + to_string(m)].wartosc + graf[m][k]; //new value
					wartosc.liczba = m;
					najlepszaWartosc = (najlepszaWartosc.wartosc < wartosc.wartosc) ? najlepszaWartosc : wartosc; // if best < value than best else value
				}
				podproblemy[to_string(ciag) + to_string(k)] = najlepszaWartosc;//dodanie optymalnego podzbioru
			}
		}
	}

	ciag = (1 << graf.size()) - 2;
	najlepszaWartosc.liczba = INT_MAX;
	najlepszaWartosc.wartosc = INT_MAX;

	for (int i = 1; i < graf.size() - 1; i++) {//obliczenie optymalnego cyklu
		wartosc.wartosc = podproblemy[to_string(ciag) + to_string(i)].wartosc + graf[i][0];
		najlepszaWartosc = (najlepszaWartosc.wartosc < wartosc.wartosc) ? najlepszaWartosc : wartosc;
	}

	tablica.clear();
	tablica.resize(graf.size() + 1, 0);
	wartosc = najlepszaWartosc;

	for (int i = 1; i < graf.size(); i++) {//obliczenie scieżki optymlanego cyklu
		tablica[i] = wartosc.liczba;
		wartosc = podproblemy[to_string(ciag) + to_string(tablica[i])];
		ciag = ciag & ~(1 << tablica[i]);
	}
	wynikAlgorytmu wynik;//zwracanie wyników algorytmu
	wynik.ciag = tablica;
	wynik.wartosc = najlepszaWartosc.wartosc;

	return wynik;
}

void test(string rozmiar, int powtorzenia, bool wyswietlenie) {
	srand(time(NULL));
	time_t start, koniec;
	vector<vector<int>> graf;//graf na którym będziemy pracować
	int wartoscOptymalna;
	fstream wynik;
	wynikAlgorytmu wyniki;
	string sciezka;

	wynik.open("wynik.csv", ios::app);//plik do zapisu
	wartoscOptymalna = wczytaj(graf, rozmiar, sciezka);//wczytanie danych z pliku

	start = clock();
	for (int i = 0; i < powtorzenia; i++) {
		wyniki = dynamicProgramming(graf);//wykonanie algorytmu
	}
	koniec = clock();

	wynik << rozmiar << ";" << powtorzenia << ";" << (koniec - start) / CLOCKS_PER_SEC << endl;//zapis

	if (wyswietlenie == true) {//wyswietlanie wyników na ekranie
		cout << sciezka << endl;
		wyswietl(graf);
		cout << "czas = " << (koniec - start) / CLOCKS_PER_SEC << "s" << endl;
		cout << "wykryta droga = ";
		wyswietlWiersz(wyniki.ciag);
		cout << "dlugosc drogi = " << wyniki.wartosc << endl;
		cout << "optymalna droga = " << wartoscOptymalna << endl;
		if (wartoscOptymalna == 0) {//przy generacji grafu tworzony jes tcykl 0 i niszczy to obliczenia
			cout << "blad = " << (1.0 * wyniki.wartosc / 1) * 100 << "%" << endl << endl;
		}
		else {
			cout << "blad = " << ((1.0 * wyniki.wartosc / wartoscOptymalna) - 1) * 100 << "%" << endl << endl;
		}
	}
}

void inicjalizacja() {
	fstream konfiguracja;
	konfiguracja.open("start.ini", ios::in);//wczytanie konfiguracji
	int rozmiar, powtorzenia;
	bool wyswietlenie;

	if (konfiguracja) {
		while (!konfiguracja.eof()) {//testy dla wczytanych wartości
			konfiguracja >> rozmiar;
			konfiguracja >> powtorzenia;
			konfiguracja >> wyswietlenie;
			test(to_string(rozmiar), powtorzenia, wyswietlenie);
		}
	}
}

int main() {
	inicjalizacja();
	return 0;
}
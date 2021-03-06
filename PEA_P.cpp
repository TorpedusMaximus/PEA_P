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


/////////////////////////////////////////////////////OBSŁUGA GRAFU//////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct podproblem {//struktura ułatwia pracę funkcji dynamicProgramming 
	int liczba;
	int wartosc;
};

struct tabu {
	int index1;
	int index2;
	int kadecja;
};

struct wynikAlgorytmu {//struktura zwrotna dla funkcji obliczających problem
	vector<int> ciag;
	int wartosc;
};

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

int dlugoscSciezki(vector<vector<int>> graf, vector<int> sciezka) {
	int wynik = 0;
	for (int i = 0; i < sciezka.size() - 1; i++) {
		wynik += graf[sciezka[i]][sciezka[i + 1]];
	}
	wynik += graf[sciezka[sciezka.size() - 1]][sciezka[0]];
	return wynik;
}

vector<int> losujSciezke(vector<vector<int>> graf) {
	vector<int> sciezka, najlepszaSciezka;

	for (int i = 0; i < graf.size(); i++) {
		sciezka.push_back(i);
	}
	najlepszaSciezka = sciezka;
	for (int i = 0; i < 10000; i++) {
		random_shuffle(sciezka.begin(), sciezka.end());
		if (dlugoscSciezki(graf, sciezka) < dlugoscSciezki(graf, najlepszaSciezka)) {
			najlepszaSciezka = sciezka;
		}
	}
	return najlepszaSciezka;
}


/////////////////////////////////////////////////////ALGORYTMY/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//BF

wynikAlgorytmu bruteForce(vector<vector<int>> graf) {
	int i, minDroga = INT_MAX, droga;
	vector<int> wierzcholki, najlepszaDroga;
	wierzcholki.resize(graf.size());
	for (i = 1; i < graf.size(); i++) {
		wierzcholki[i] = i;
	}
	i = 0;
	do {
		droga = 0;
		for (i = 0; i < graf.size() - 1; i++) {//obliczenie drogi
			droga += graf[wierzcholki[i]][wierzcholki[i + 1]];
			if (droga > minDroga) {
				break;
			}
		}
		droga += graf[0][wierzcholki[0]];
		droga += graf[wierzcholki[wierzcholki.size() - 1]][0];
		if (droga < minDroga) {//porownianie wynikow
			minDroga = droga;
			najlepszaDroga = wierzcholki;
		}

	} while (next_permutation(wierzcholki.begin(), wierzcholki.end()));

	wynikAlgorytmu wynik;
	wynik.ciag = najlepszaDroga;
	wynik.wartosc = minDroga;
	wynik.ciag.push_back(0);

	return wynik;
}

//DP

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
					if (najlepszaWartosc.wartosc > wartosc.wartosc) {
						najlepszaWartosc = wartosc;
					}
				}
				podproblemy[to_string(ciag) + to_string(k)] = najlepszaWartosc;//dodanie optymalnego podzbioru
			}
		}
	}

	ciag = (1 << graf.size()) - 2;
	najlepszaWartosc.liczba = 0;
	najlepszaWartosc.wartosc = INT_MAX;

	for (int i = 1; i < graf.size() - 1; i++) {//obliczenie optymalnego cyklu
		wartosc.wartosc = podproblemy[to_string(ciag) + to_string(i)].wartosc + graf[i][0];
		wartosc.liczba = i;
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

//BnB

void znajdzLepszyZbior(vector<vector<int>> graf, vector<vector<int>>& lepszeZbiory) {
	lepszeZbiory.resize(graf.size());

	for (int i = 0; i < graf.size(); i++) {
		sort(graf[i].begin(), graf[i].end());
		graf[i].erase(graf[i].begin());
		lepszeZbiory[i].assign(graf[i].begin(), graf[i].begin() + 2);
	}
}

void BnBrekurencja(vector<vector<int>> graf, int aktualnyZbior, int aktualnaSciezka, int poziom, vector<int> sciezka, int& najlepszeRozwiazanie, vector<int>& najlepszaSciezka, vector<bool> odwiedzone, vector<vector<int>> lepszeZbiory) {
	if (poziom == graf.size()) {
		int aktualneRozwiazanie = aktualnaSciezka + graf[sciezka[poziom - 1]][0];
		if (aktualneRozwiazanie < najlepszeRozwiazanie) {
			najlepszeRozwiazanie = aktualneRozwiazanie;
			najlepszaSciezka = sciezka;
		}
		return;
	}

	for (int i = 0; i < graf.size(); i++) {
		if (sciezka[poziom - 1] != i && odwiedzone[i] == false) {
			int wartosc = 0;
			aktualnaSciezka += graf[sciezka[poziom - 1]][i];
			int temp = aktualnyZbior;
			if (poziom == 1) {
				aktualnyZbior -= (lepszeZbiory[sciezka[poziom - 1]][0] + lepszeZbiory[i][0]) / 2;
			}
			else {
				aktualnyZbior -= (lepszeZbiory[sciezka[poziom - 1]][1] + lepszeZbiory[i][0]) / 2;
			}

			if (aktualnyZbior + aktualnaSciezka < najlepszeRozwiazanie) {
				sciezka[poziom] = i;
				odwiedzone[i] = true;
				BnBrekurencja(graf, aktualnyZbior, aktualnaSciezka, poziom + 1, sciezka, najlepszeRozwiazanie, najlepszaSciezka, odwiedzone, lepszeZbiory);
			}

			aktualnaSciezka -= graf[sciezka[poziom - 1]][i];
			aktualnyZbior = temp;

			for (int ii = 0; ii < odwiedzone.size(); ii++) {
				odwiedzone[ii] = false;
			}
			for (int ii = 0; ii < poziom; ii++) {
				odwiedzone[sciezka[ii]] = true;
			}

		}
	}
}

wynikAlgorytmu BnBstart(vector<vector<int>> graf) {
	vector<vector<int>> lepszeZbiory;
	znajdzLepszyZbior(graf, lepszeZbiory);
	vector<int> sciezka;
	vector<bool> odwiedzone;
	sciezka.resize(graf.size() + 1, 0);
	odwiedzone.resize(graf.size() + 1, 0);
	int aktualnyZbior = 0;

	for (int i = 0; i < graf.size(); i++) {
		aktualnyZbior += lepszeZbiory[i][0];
		aktualnyZbior += lepszeZbiory[i][1];
	}

	if (aktualnyZbior % 2 == 0) {
		aktualnyZbior = aktualnyZbior / 2;
	}
	else {
		aktualnyZbior = aktualnyZbior / 2 + 1;
	}
	odwiedzone[0] = true;
	sciezka[0] = 0;

	wynikAlgorytmu wyniki;
	wyniki.wartosc = INT_MAX;
	wyniki.ciag;
	BnBrekurencja(graf, aktualnyZbior, 0, 1, sciezka, wyniki.wartosc, wyniki.ciag, odwiedzone, lepszeZbiory);
	return wyniki;
}

//SA

double prawdopodobienstwo(vector<vector<int>> graf, vector<int> sciezka, vector<int> sasiad, double temperatura) {
	double energia = dlugoscSciezki(graf, sciezka);
	double energiaDelta = dlugoscSciezki(graf, sasiad);
	if (energia > energiaDelta) {
		return 1;
	}
	else {
		return exp(-(energiaDelta - energia) / temperatura);
	}
}

wynikAlgorytmu symulowaneWyzarzanie(vector<vector<int>> graf) {
	vector<int> sciezka, najlepszaSciezka;
	double temperatura = 1000.0;
	wynikAlgorytmu wyniki;
	int w1, w2;

	sciezka = losujSciezke(graf);
	najlepszaSciezka = sciezka;

	temperatura = 0.5 * dlugoscSciezki(graf, najlepszaSciezka);
	while (temperatura > 0.0000001) {
		for (int i = 0; i < 60; i++) {
			vector<int> sasiad = sciezka;

			w1 = rand() % sciezka.size();
			w2 = (w1 + rand() % sciezka.size()) % sciezka.size();
			swap(sasiad[w1], sasiad[w2]);

			if (prawdopodobienstwo(graf, sciezka, sasiad, temperatura) > (double)rand() / RAND_MAX) {
				sciezka = sasiad;
			}

			if (dlugoscSciezki(graf, sciezka) < dlugoscSciezki(graf, najlepszaSciezka)) {
				najlepszaSciezka = sciezka;
			}
		}
		temperatura *= 0.999;
	}

	wyniki.wartosc = dlugoscSciezki(graf, najlepszaSciezka);
	wyniki.ciag = najlepszaSciezka;

	return wyniki;
}

//TS

vector<int> znajdzSasiada(vector<int> sciezka, vector<vector<int>> graf, vector<tabu> &listaTabu, int najlepszaWartosc) {
	vector<int> sasiad = sciezka, najlepszaSciezka = sciezka;
	tabu elementListy;

	for (int i = 1; i < graf.size(); ++i) {
		for (int j = i + 1; j < graf.size(); ++j) {
			swap(sciezka[i], sciezka[j]);
			int tabu = 0;
			if (dlugoscSciezki(graf, sciezka) < dlugoscSciezki(graf, najlepszaSciezka)) {
				for (auto element : listaTabu) {
					if ((element.index1 == i && element.index2 == j) || (element.index1 == j && element.index2 == i)) {
						tabu = 1;

						if (dlugoscSciezki(graf, sciezka) < najlepszaWartosc) {
							najlepszaSciezka = sciezka;
							najlepszaWartosc = dlugoscSciezki(graf, sasiad);
							elementListy.index1 = i;
							elementListy.index2 = j;
							elementListy.kadecja = graf.size() * 0.4;
							break;
						}
						break;
					}
				}
				if (!tabu) {
					najlepszaSciezka = sciezka;
					elementListy.index1 = i;
					elementListy.index2 = j;
					elementListy.kadecja = graf.size() * 0.4;
				}
			}

		}
	}

	for (auto element : listaTabu) {
		element.kadecja--;
		if (element.kadecja <= 0) {
			element.index1 = 0;
			element.index2 = 0;
			element.kadecja = -1;
		}
	}

	if (najlepszaSciezka != sciezka) {
		rotate(listaTabu.begin(), listaTabu.begin() + 1, listaTabu.end());
		listaTabu[0] = elementListy;
	}

	return najlepszaSciezka;
}

wynikAlgorytmu tabuSearch(vector<vector<int>> graf) {
	wynikAlgorytmu wyniki;
	vector<int> sciezka, najlepszaSciezka;
	vector<tabu> listaTabu;
	int licznik = 0.2 * graf.size();

	sciezka = losujSciezke(graf);
	najlepszaSciezka = sciezka;
	for (int i = 0; i < graf.size() * 0.4; i++) {
		tabu element;
		element.index1 = 0;
		element.index2 = 0;
		element.kadecja = -1;
		listaTabu.push_back(element);
	}

	int i = 0, ii = 0;
	while (i < 500) {
		vector<int> sasiad;
		sasiad = znajdzSasiada(sciezka, graf, listaTabu, dlugoscSciezki(graf, najlepszaSciezka));

		if (dlugoscSciezki(graf, sasiad) < dlugoscSciezki(graf, najlepszaSciezka)) {
			najlepszaSciezka = sasiad;
		}

		if (licznik > 0 && ii > 1000) {
			licznik--;
			sasiad = losujSciezke(graf);
			i = 0;
		}

		if (dlugoscSciezki(graf, sasiad) * 0.02 <= dlugoscSciezki(graf, sciezka)) {
			i++;
		}
		else {
			i = 0;
		}
		ii++;
		sciezka = sasiad;
	}

	wyniki.ciag = najlepszaSciezka;
	wyniki.wartosc = dlugoscSciezki(graf, najlepszaSciezka);

	return wyniki;
}


////////////////////////////////////////////////////APLIKACJA///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void algorytm(int zadanie, int powtorzenia, vector<vector<int>> graf, wynikAlgorytmu& wyniki) {
	switch (zadanie) {
	case 1:
		for (int i = 0; i < powtorzenia; i++) {
			wyniki = bruteForce(graf);//wykonanie algorytmu
		}
		break;
	case 2:
		for (int i = 0; i < powtorzenia; i++) {
			wyniki = dynamicProgramming(graf);//wykonanie algorytmu
		}
		break;
	case 3:
		for (int i = 0; i < powtorzenia; i++) {
			wyniki = BnBstart(graf);//wykonanie algorytmu
		}
		break;
	case 4:
		for (int i = 0; i < powtorzenia; i++) {
			wyniki = symulowaneWyzarzanie(graf);//wykonanie algorytmu
		}
		break;
	case 5:
		for (int i = 0; i < powtorzenia; i++) {
			wyniki = tabuSearch(graf);//wykonanie algorytmu
		}
		break;

	}
}

void test(string rozmiar, int powtorzenia, bool wyswietlenie, int zadanie) {
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
	algorytm(zadanie, powtorzenia, graf, wyniki);
	koniec = clock();

	wynik << rozmiar << ";" << powtorzenia << ";" << (koniec - start) / CLOCKS_PER_SEC << ";" << ((1.0 * wyniki.wartosc / wartoscOptymalna) - 1) * 100 << endl;//zapis

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
	int rozmiar, powtorzenia, zadanie;
	bool wyswietlenie;

	if (konfiguracja) {
		do {//testy dla wczytanych wartości
			konfiguracja >> rozmiar;
			konfiguracja >> powtorzenia;
			konfiguracja >> wyswietlenie;
			konfiguracja >> zadanie;
			test(to_string(rozmiar), powtorzenia, wyswietlenie, zadanie);
			cout << rozmiar<<" gotowe\n";
		} while (!konfiguracja.eof());
	}
}

int main() {
	inicjalizacja();
	/*for (int i = 0; i < 100; i++) {
		cout << ((double)rand() / RAND_MAX) << endl;
	}*/
	return 0;
}
﻿#include <iostream>
#include <random>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <fstream>
#include <string>
#include <utility>
#include <unordered_map>
#include <set>
using namespace std;


/////////////////////////////////////////////////////OBSŁUGA GRAFU//////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct podproblem {//struktura ułatwia pracę funkcji dynamicProgramming 
	int liczba;
	int wartosc;
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

bool mniejszeWieksze(wynikAlgorytmu a, wynikAlgorytmu b) {
	if (a.wartosc < b.wartosc)
		return true;
	else
		return false;
}


/////////////////////////////////////////////////////ALGORYTMY/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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

	for (int i = 0; i < graf.size(); i++) {
		sciezka.push_back(i);
	}
	najlepszaSciezka = sciezka;
	for (int i = 0; i < 1000; i++) {
		random_shuffle(sciezka.begin(), sciezka.end());
		if (dlugoscSciezki(graf, sciezka) < dlugoscSciezki(graf, najlepszaSciezka)) {
			najlepszaSciezka = sciezka;
		}
	}
	sciezka = najlepszaSciezka;

	temperatura = 0.5 * dlugoscSciezki(graf, najlepszaSciezka);
	if (temperatura > 1000) {
		temperatura = 1000;
	}
	while (temperatura > 0.000001) {
		for (int i = 0; i < 100; i++) {
			vector<int> sasiad = sciezka;

			w1 = rand() % sciezka.size();
			w2 = (w1 + rand() % sciezka.size()) % sciezka.size();
			swap(sasiad[w1], sasiad[w2]);

			/*if (w1 == sasiad.size() - 1) {
				swap(sasiad[w1], sasiad[0]);
			}
			else {
				swap(sasiad[w1], sasiad[w1+1]);
			}*/

			/*if (w1 < w2) {
				random_shuffle(sciezka.begin() + w1, sciezka.begin() + w2);
			}
			else {
				random_shuffle(sciezka.begin() + w2, sciezka.begin() + w1);
			}*/


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


wynikAlgorytmu selekcjaRuletka(vector<wynikAlgorytmu> populacja) {
	vector<double> wagi;
	int wartoscPopulacji = 0;
	double wagaPopulacji = 0;
	double licz = 0;

	for (int i = 0; i < populacja.size(); i++) {
		wartoscPopulacji += populacja[i].wartosc;
	}

	for (int i = 0; i < populacja.size(); i++) {
		double waga = 1.0 * populacja[i].wartosc / wartoscPopulacji;
		wagi.push_back(waga);
	}
	reverse(wagi.begin(), wagi.end());

	for (int i = 0; i < populacja.size(); i++) {
		double waga = wagi[i] + wagaPopulacji;
		wagi[i] = waga;
		wagaPopulacji = waga;
	}

	wynikAlgorytmu gracz1;
	wynikAlgorytmu gracz2;

	double szansa = (1.0 * rand() / RAND_MAX) / 2;
	for (int i = 0; i < populacja.size(); i++) {
		licz += wagi[i];
		if (licz > szansa) {
			if (i == 0) {
				gracz1 = populacja[i];
			}
			else {
				gracz1 = populacja[i - 1];
			}
		}
	}
	szansa = (1.0 * rand() / RAND_MAX) / 2;
	for (int i = 0; i < populacja.size(); i++) {
		licz += wagi[i];
		if (licz > szansa) {
			if (i == 0) {
				gracz2 = populacja[i];
			}
			else {
				gracz2 = populacja[i - 1];
			}
		}
	}

	return mniejszeWieksze(gracz1, gracz2) ? gracz1 : gracz2;
}

wynikAlgorytmu selekcjaTurniej(vector<wynikAlgorytmu> populacja) {
	wynikAlgorytmu pretendent1 = populacja[rand() % populacja.size()];
	wynikAlgorytmu pretendent2 = populacja[rand() % populacja.size()];

	return mniejszeWieksze(pretendent1, pretendent2) ? pretendent1 : pretendent2;
}

wynikAlgorytmu krzyzowanieCX(wynikAlgorytmu rodzic1, wynikAlgorytmu rodzic2) {
	vector<int> tablica(rodzic1.ciag.size(), -1);
	set<int> odwiedzone;
	int pozycja = rand() % rodzic1.ciag.size();
	int poczatekCyklu = rodzic1.ciag[pozycja];

	while (true)
	{
		tablica[pozycja] = rodzic1.ciag[pozycja];
		odwiedzone.insert(rodzic1.ciag[pozycja]);

		if (poczatekCyklu == rodzic2.ciag[pozycja]) {
			break;
		}
		else {
			auto iterator = find(rodzic1.ciag.begin(), rodzic1.ciag.end(), rodzic2.ciag[pozycja]);
			pozycja = iterator - rodzic1.ciag.begin();
		}
	}

	for (int i = 0; i < rodzic1.ciag.size(); i++) {
		if (tablica[i] == -1) {
			tablica[i] = rodzic2.ciag[i];
		}
	}

	wynikAlgorytmu dziecko;
	dziecko.ciag = tablica;

	return dziecko;

}

void mutowanieSwap(wynikAlgorytmu& dziecko) {
	int x = rand() % dziecko.ciag.size();
	int y = (x + rand() % dziecko.ciag.size()) % dziecko.ciag.size();

	swap(dziecko.ciag[x], dziecko.ciag[y]);
}

void mutowanieScramble(wynikAlgorytmu& dziecko) {
	int x = rand() % dziecko.ciag.size();
	int y = (x + rand() % dziecko.ciag.size()) % dziecko.ciag.size();

	if (x > y) {
		swap(x, y);
	}

	random_shuffle(dziecko.ciag.begin() + x, dziecko.ciag.begin() + y);
}

void mutowanieReverse(wynikAlgorytmu& dziecko) {
	int x = rand() % dziecko.ciag.size();
	int y = (x + rand() % dziecko.ciag.size()) % dziecko.ciag.size();

	if (x > y) {
		swap(x, y);
	}

	reverse(dziecko.ciag.begin() + x, dziecko.ciag.begin() + y);
}

wynikAlgorytmu algorytmGenetyczny(vector<vector<int>> graf) {
	//burma14.tsp, gr17.tsp, gr21.tsp, gr24.tsp, ftv33.atsp,
	//ftv44.atsp, ft53.atsp,  ch150.tsp, ftv170.atsp, gr202.tsp,
	//rbg323.atsp, pcb442.tsp, rgb443.atsp
	int wielkoscPopulacjiPoczatkowej = 40;
	double wspolczynnikMutacji = 0.4;
	double wspolczynnikKrzyzowania = 0.9;


	vector<wynikAlgorytmu> populacja;
	wynikAlgorytmu najlepszyZNajlepszych;
	najlepszyZNajlepszych.wartosc = INT_MAX;


	for (int i = 0; i < wielkoscPopulacjiPoczatkowej; i++) {
		wynikAlgorytmu osobnik;
		for (int j = 0; j < graf.size(); j++) {
			osobnik.ciag.push_back(j);
		}

		random_shuffle(osobnik.ciag.begin(), osobnik.ciag.end());
		osobnik.wartosc = dlugoscSciezki(graf, osobnik.ciag);

		if (mniejszeWieksze(osobnik, najlepszyZNajlepszych)) {
			najlepszyZNajlepszych = osobnik;
		}

		populacja.push_back(osobnik);
	}

	time_t start = clock();
	int iteracjeBezPoprawy = 0;
	int iterator = 0;
	wynikAlgorytmu poprzedniNajlepszyZNajlepszych = najlepszyZNajlepszych;
	while (true) {
		iteracjeBezPoprawy++;
		if (iteracjeBezPoprawy > 50000) {
			break;

		}

		time_t teraz = clock();
		int minelo = (teraz - start) / CLOCKS_PER_SEC;
		if (minelo > 600) {
			break;
		}

		vector<wynikAlgorytmu> nastepnaPopulacja;
		sort(populacja.begin(), populacja.end(), mniejszeWieksze);

		nastepnaPopulacja.assign(populacja.begin(), populacja.begin() + wielkoscPopulacjiPoczatkowej / 8);
		wynikAlgorytmu osobnikLosowy = populacja[wielkoscPopulacjiPoczatkowej - 1];
		random_shuffle(osobnikLosowy.ciag.begin(), osobnikLosowy.ciag.end());
		nastepnaPopulacja.push_back(osobnikLosowy);

		while (nastepnaPopulacja.size() < wielkoscPopulacjiPoczatkowej) {
			wynikAlgorytmu rodzic1 = selekcjaTurniej(populacja);
			wynikAlgorytmu rodzic2 = selekcjaTurniej(populacja);
			wynikAlgorytmu dziecko1 = rodzic1;
			wynikAlgorytmu dziecko2 = rodzic2;
			double szansa;

			szansa = 1.0 * rand() / RAND_MAX;
			if (szansa < wspolczynnikKrzyzowania) {
				dziecko1 = krzyzowanieCX(rodzic1, rodzic2);
				dziecko2 = krzyzowanieCX(rodzic2, rodzic1);
			}

			szansa = 1.0 * rand() / RAND_MAX;
			if (szansa < wspolczynnikMutacji) {
				mutowanieSwap(dziecko2);
			}
			szansa = 1.0 * rand() / RAND_MAX;
			if (szansa < wspolczynnikMutacji) {
				mutowanieScramble(dziecko2);
				mutowanieReverse(dziecko2);
			}

			dziecko1.wartosc = dlugoscSciezki(graf, dziecko1.ciag);
			dziecko2.wartosc = dlugoscSciezki(graf, dziecko2.ciag);

			nastepnaPopulacja.push_back(dziecko1);
			nastepnaPopulacja.push_back(dziecko2);

			if (mniejszeWieksze(dziecko1, najlepszyZNajlepszych)) {
				poprzedniNajlepszyZNajlepszych = najlepszyZNajlepszych;
				najlepszyZNajlepszych = dziecko1;
				iteracjeBezPoprawy = 0;
			}
			if (mniejszeWieksze(dziecko2, najlepszyZNajlepszych)) {
				poprzedniNajlepszyZNajlepszych = najlepszyZNajlepszych;
				najlepszyZNajlepszych = dziecko2;
				iteracjeBezPoprawy = 0;
			}
		}

		populacja = nastepnaPopulacja;
		iterator++;
	}
	cout << iterator << endl;
	cout << iteracjeBezPoprawy << endl;
	return  najlepszyZNajlepszych;
}


//wynikAlgorytmu koloniaMrowek(vector<vector<int>> graf) {
//
//}


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
	case 6:
		for (int i = 0; i < powtorzenia; i++) {
			wyniki = algorytmGenetyczny(graf);//wykonanie algorytmu
		}
		break;
	case 7:
		for (int i = 0; i < powtorzenia; i++) {
			//wyniki = koloniaMrowek(graf);//wykonanie algorytmu
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
		} while (!konfiguracja.eof());
	}
}

int main() {
	inicjalizacja();

	/*int rozmiar;
	cout << "Podaj rozmiar instancji. Instancje gotowe znjaduja sie w folerze dane. W przyadku innego rozmiaru program wygeneruje wlasny graf.\n";
	cin >> rozmiar;
	cout << "Wyniki zostana wyswietlone na ekranie oraz zapisane do pliku. W pliku bedzie format instancja;czas;blad" << endl;
	test(to_string(rozmiar), 1, 1, 2);
	Sleep(5000);*/

	return 0;
}
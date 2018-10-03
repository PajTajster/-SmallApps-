#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include <bitset>

using namespace std;

void generate(int n) {
	srand((unsigned)time(NULL));
	int tmp = 0;
	string binary;
	
	
	ofstream in;
	in.open("in.txt");
	in << n << endl;
	long int x;
	for (int i = 0; i < n; ++i) {
		tmp = rand() % 33554432;
		binary = bitset<32>(tmp).to_string();
		auto dig = binary.find('1');
		if (dig != string::npos)
			binary = binary.substr(dig);
		//string tmp1 = to_string(tmp);
		//x = strtol(tmp1.c_str(), nullptr, 2);
		in << binary << endl;
	}
	in.close();
}

void naive() {
	int licznik = 0;
	fstream in, out;
	in.open("in.txt", ios::in);
	if (!in.good())
		return;
	int n = 0;
	in >> n;

	string *lineArr = new string[n];
	for (int i = 0; i < n; ++i)
		in >> lineArr[i];
	in.close();

	out.open("outNaive.txt", ios::out);
	if (!out.good()) {
		delete[] lineArr;
		return;
	}

	int crntCmp = 1, cmp = 0;
	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; ++j) {
			if (lineArr[i] == lineArr[j])
				++crntCmp;
			++licznik;
		}
		if (crntCmp > cmp)
			cmp = crntCmp;
		crntCmp = 1;
	}

	out << "Liczba powtorzen: " << cmp << "\n\n\n" << "Liczba operacji: " << licznik;
	out.close();

	delete[] lineArr;
	return;
}

void fast() {
	int licznik = 0;
	fstream in, out;
	in.open("in.txt", ios::in);
	if (!in.good())
		return;
	int n = 0;
	in >> n;

	string *lineArr = new string[n];
	for (int i = 0; i < n; ++i)
		in >> lineArr[i];
	in.close();

	int *intArr = new int[33554432];
	memset(intArr, 0, 33554432);
	int tmp = 0, cmp = 0;
	for (int i = 0; i < n; ++i) {
		tmp = strtol(lineArr[i].c_str(), nullptr, 2);
		++intArr[tmp];
		++licznik;
	}
	for (int i = 0; i < n; ++i) {
		tmp = strtol(lineArr[i].c_str(), nullptr, 2);
		if (intArr[tmp] > cmp)
			cmp = intArr[tmp];
		++licznik;
	}

	out.open("outFast.txt", ios::out);
	if (!out.good()) {
		delete[] lineArr;
		return;
	}

	out << "Liczba powtorzen: " << cmp << "\n\n\n" << "Liczba operacji: " << licznik;
	out.close();

	delete[] lineArr;
	delete[] intArr;
	return;
}

int main() {
	clock_t start = clock();
	double durationNaive, durationFast;

	//generate(10000);
	naive();
	durationNaive = ((clock() - start) / (double)CLOCKS_PER_SEC) * 1000;

	fast();
	durationFast = ((clock() - durationNaive) / (double)CLOCKS_PER_SEC) * 1000;

	cout << "Czas wykonania algorytmu:\nNaiwnego: " << durationNaive << " ms, Zlozonosc czasowa: O(n^2)\nSzybkiego: " <<
		durationFast << " ms, Zlozonosc czasowa: O(n)\nSzybszym algorytmem jest algorytm o zlozonosci O(n)." << endl;
	
	return 0;
}
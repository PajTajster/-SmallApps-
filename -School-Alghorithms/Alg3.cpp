#include <fstream>
#include <cmath>

using namespace std;

int MOD = (int)pow(10, 9);
static int COUNTER = 0;

bool isSameColour(char **tab, int n, int m) {
	int allWhite = 0, allBlack = 0;
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			if (tab[i][j] == 'b')
				++allWhite;
			else if (tab[i][j] == 'c')
				++allBlack;
		}
	}
	return (allWhite == n*m || allBlack == n*m);
}

bool canDivideHorizontally(char **tab, int n, int m, int index, int checkFromPos) {
	bool upDivisions = false;
	bool downDivisions = false;

	if (m - checkFromPos <= 1)
		return false;
	if (checkFromPos == 0) {
		char tmp = tab[index][0];
		for (int i = 0; i < index; ++i) {
			for (int j = 0; j < n; ++j) {
				if (tab[i][j] != tmp) {
					upDivisions = true;
					break;
				}
				++COUNTER;
			}
		}

		tmp = tab[index + 1][0];
		for (int i = 0; i < m; ++i) {
			for (int j = 0; j < n; ++j) {
				if (tab[i][j] != tmp) {
					downDivisions = true;
					break;
				}
				++COUNTER;
			}
		}


	}
	else {
		char tmp = tab[index + checkFromPos][0];
		for (int i = checkFromPos; i <= index + checkFromPos; ++i) {
			for (int j = 0; j < n; ++j) {
				if (tab[i][j] != tmp) {
					upDivisions = true;
					break;
				}
			}
		}

		tmp = tab[index + checkFromPos + 1][0];
		for (int i = index + checkFromPos + 1; i < m + checkFromPos; ++i) {
			for (int j = 0; j < n; ++j) {
				if (tab[i][j] != tmp) {
					downDivisions = true;
					break;
				}
			}
		}

	}

	return (upDivisions && downDivisions);
}

bool canDivideVertically(char **tab, int n, int m, int index, int checkFromPos) {
	bool leftDivision = false;
	bool rightDivision = false;
	if (n - checkFromPos <= 1)
		return false;
	if (checkFromPos == 0) {
		char tmp = tab[0][index];
		for (int i = 0; i < m; ++i) {
			for (int j = 0; j <= index; ++j) {
				if (tab[i][j] != tmp) {
					leftDivision = true;
					break;
				}
			}
		}

		tmp = tab[0][index + 1];
		for (int i = 0; i < m; ++i) {
			for (int j = index + 1; j < n; ++j) {
				if (tab[i][j] != tmp) {
					rightDivision = true;
					break;
				}
			}
		}
	}
	else {
		char tmp = tab[0][index + checkFromPos];
		for (int i = 0; i < m; ++i) {
			for (int j = checkFromPos; j <= index + checkFromPos; ++j) {
				if (tab[i][j] != tmp) {
					leftDivision = true;
					break;
				}
			}
		}

		tmp = tab[0][index + checkFromPos + 1];
		for (int i = 0; i < m; ++i) {
			for (int j = index + checkFromPos + 1; j < checkFromPos + n; ++j) {
				if (tab[i][j] != tmp) {
					rightDivision = true;
					break;
				}
			}
		}


	}

	return (leftDivision && rightDivision);
}

int divideChocolate(char **tab, int n, int m) {
	if ((n <= 3 && m <= 1) || (n <= 1 && m <= 3))
		return 1;

	/*char** dividedTab = new char*[m];
	for (int i = 0; i < m; ++i) {
		dividedTab[i] = new char[n];
		for (int j = 0; j < n; ++j)
			dividedTab[i][j] = tab[i][j];
	}*/

	if (isSameColour(tab, n, m))
		return 1;

	int leftDivisions = 0, rightDivisions = 0, upDivisions = 0, downDivisions = 0;
	int allVerticalDivisions = 0, allHorizontalDivisions = 0;

	for (int i = 0; i < m - 1; ++i) {
		if (canDivideHorizontally(tab, n, m, i, i)) {
		char** dividedTabUP = new char*[m-i];
		for (int k = 0; k < m-i; ++k) {
			dividedTabUP[k] = new char[n];
			for (int j = 0; j < n; ++j)
				dividedTabUP[k][j] = tab[k][j];
		}
		char** dividedTabDOWN = new char*[m];
		for (int k = i; k < m; ++k) {
			dividedTabDOWN[k] = new char[n - i];
			for (int j = 0; j < n; ++j)
				dividedTabDOWN[k][j] = tab[k][j];
		}
			upDivisions = divideChocolate(dividedTabUP, n, i + 1);
			downDivisions = divideChocolate(dividedTabDOWN, n, m - (i + 1));
			allHorizontalDivisions += (upDivisions*downDivisions) % MOD;
		}
		++COUNTER;
	}
	
	for (int i = 0; i < n - 1; ++i) {
		if (canDivideVertically(tab, n, m, i, i)) {
			char** dividedTabLEFT = new char*[m];
			for (int k = 0; k < m; ++k) {
				dividedTabLEFT[k] = new char[n - i];
				for (int j = 0; j < n - i; ++j)
					dividedTabLEFT[k][j] = tab[k][j];
			}
			char** dividedTabRIGHT = new char*[m];
			for (int k = 0; k < m; ++k) {
				dividedTabRIGHT[k] = new char[n];
				for (int j = i; j < n; ++j)
					dividedTabRIGHT[k][j] = tab[k][j];
			}
			leftDivisions = divideChocolate(dividedTabLEFT, i + 1, m);
			rightDivisions = divideChocolate(dividedTabRIGHT, n - (i + 1), m);
			allVerticalDivisions += (leftDivisions * rightDivisions) % MOD;
		}
		++COUNTER;
	}



	//for (int i = 0; i < m; ++i)
		//delete[] dividedTab[i];
	//delete[] dividedTab;
	return allVerticalDivisions + allHorizontalDivisions;
}


void countDivisions() {
	ifstream in;
	in.open("in.txt");
	if (!in.good())
		return;
	int n, m;


	in >> n >> m;
	char** tab = new char*[m];
	for (int i = 0; i < m; ++i) {
		tab[i] = new char[n];
	}
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			in >> tab[i][j];
		}
	}

	in.close();

	int total_divisions = divideChocolate(tab, n, m);

	ofstream out;
	out.open("out.txt");
	if (!out.good()) {
		for (int i = 0; i < m; ++i)
			delete[] tab[i];
		delete[] tab;
		return;
	}
	out << total_divisions << endl;

	out.close();

	for (int i = 0; i < m; ++i)
		delete[] tab[i];
	delete[] tab;
	return;
}

int main() {
	countDivisions();
	return 0;
}

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <fstream>

using namespace std;

typedef struct TElement {
	string word;
	struct TElement* l_son;
	struct TElement* r_son;
	int height;
	struct TElement(string name) : word(name), l_son(nullptr), r_son(nullptr), height(1) {};
}TreeElement;

int max(int a, int b){
	return (a > b) ? a : b;
}
int GetHeight(TreeElement* node){
	if (!node)
		return 0;
	return node->height;
}


int GetWeight(TreeElement* node){
	if (!node)
		return 0;
	return GetHeight(node->l_son) - GetHeight(node->r_son);
}

TreeElement* RRotate(TreeElement*& oldFather){
	TreeElement* newFather = oldFather->l_son;
	TreeElement* rest = newFather->r_son; // no  reszta

	
	newFather->r_son = oldFather;
	oldFather->l_son = rest;

	
	oldFather->height = max(GetHeight(oldFather->l_son), GetHeight(oldFather->r_son)) + 1;
	newFather->height = max(GetHeight(newFather->l_son), GetHeight(newFather->r_son)) + 1;

	return newFather;
}
TreeElement* LRotate(TreeElement*& oldFather) {
	TreeElement* newFather = oldFather->r_son;
	TreeElement* rest = newFather->l_son; // no  reszta


	newFather->l_son = oldFather;
	oldFather->r_son = rest;


	oldFather->height = max(GetHeight(oldFather->l_son), GetHeight(oldFather->r_son)) + 1;
	newFather->height = max(GetHeight(newFather->l_son), GetHeight(newFather->r_son)) + 1;

	return newFather;
}
void PrintTree(TreeElement* some_node, int depth) {
	if (!some_node)
		return;
	for (int i = 0; i<depth; ++i) {
		cout << "\t";
	}
	cout << some_node->word << "\n";
	++depth;  
	if (some_node->r_son)
		PrintTree(some_node->r_son, depth);
	else {
		for (int i = 0; i < depth; ++i) {
			cout << "\t";
		}
		cout << "-\n";
	}
	if (some_node->l_son)
		PrintTree(some_node->l_son, depth);
	else {
		for (int i = 0; i < depth; ++i) {
			cout << "\t";
		}
		cout << "-\n";
	}
}

TreeElement* SmallestElement(TreeElement* node)
{
	TreeElement* smlstElement = node;

	while (smlstElement->l_son)
		smlstElement = smlstElement->l_son; //przemieszcza sie w lewo szukajac najmniejszego elementu

	return smlstElement;
}

TreeElement* InsertElement(TreeElement *&root, string name)
{

	
	if (!root) {
		root = new TreeElement(name);
	}

	if (name < root->word)
		root->l_son = InsertElement(root->l_son, name);
	else if (name > root->word)
		root->r_son = InsertElement(root->r_son, name);
	else 
		return root; //identyczny wyraz

	// zaktualizowanie wysokosci elementu drzewa 
	root->height = 1 + max(GetHeight(root->l_son),
		GetHeight(root->r_son));

	// sprawdzamy wage elementu, aby sprawdzic czy konieczna jest rotacja 
	int weight = GetWeight(root);


	// przypadek RR
	if (weight > 1 && GetWeight(root->l_son) >= 0)
		return RRotate(root);

	// Przypadek LL
	if (weight < -1 && GetWeight(root->r_son) <= 0)
		return LRotate(root);

	// Przypadek LR
	if (weight > 1 && GetWeight(root->l_son) < 0)
	{
		root->l_son = LRotate(root->l_son);
		return RRotate(root);
	}

	// Przypadek RL
	if (weight < -1 && GetWeight(root->r_son) > 0)
	{
		root->r_son = RRotate(root->r_son);
		return LRotate(root);
	}

	/* zwraca niezmieniony wskaznik w przypadku braku rotacji */
	return root;
}
TreeElement* DeleteNode(TreeElement*& root, string _word) {

	if (!root) //trafi na nulla
		return root;
	if (_word < root->word) //szukany wyraz jest "mniejszy" od obecnie rozpatrywanego
		root->l_son = DeleteNode(root->l_son, _word);
	else if (_word > root->word) //szukany wyraz jest większy od obecnie rozpatrywanego
		root->r_son = DeleteNode(root->r_son, _word); 
	else {//znaleziono element do usuniecia
		if ((root->l_son == nullptr) || (root->r_son == nullptr)) { //element ma 1 lub 0 synów

			TreeElement *tmp;
			if (root->l_son)
				tmp = root->l_son;
			else
				tmp = root->r_son;
			if (!tmp) {
				tmp = root;
				root = nullptr; //jesli nie ma synow, jego wskaznik jest ustawiany na nulla
			}
			else
				*root = *tmp; //jesli ma syna, przepisywane sa jego dane

			delete tmp;
		}
		else {

			TreeElement* tmp = SmallestElement(root->r_son); //szukany jest nastepnik usuwanego elementu
			root->word = tmp->word; //przepisywany jest wyraz z nastepnika
			root->r_son = DeleteNode(root->r_son, tmp->word); //nastepnik jest usuwany
		}
	}

	if (!root)
		return root; // jeszcze nie wiem czemu

	root->height = 1 + max(GetHeight(root->l_son), GetHeight(root->r_son)); // aktualizowanie wysokosci
	int weight = GetWeight(root); //sprawdzanie wagi
	if ((weight > 1) && (GetWeight(root->l_son) >= 0)) //rotacja RR
		return RRotate(root);

	if ((weight < -1) && GetWeight(root->r_son) <= 0) //rotacja LL
		return LRotate(root);

	if ((weight > 1) && (GetWeight(root->l_son) < 0)) {// rotacja LR
		root->l_son = LRotate(root->l_son);
		return RRotate(root);
	}
	if ((weight < -1) && GetWeight(root->r_son) > 0) { //rotacja RL
		root->r_son = RRotate(root->r_son);
		return LRotate(root);
	}
	return root; //brak rotacji
}
bool Find(TreeElement* root, string searchedWord) {
	while (root) {
		if (root->word == searchedWord)
			return true;
		if (searchedWord > root->word)
			root = root->r_son;
		else
			root = root->l_son;
	}
	return false;
}

int CountPrefixAppearance(TreeElement *tmp, string prefix )
{
	int number = 0;
	if (tmp)
	{
		if (prefix == (tmp->word).substr(0, prefix.size())) //znaleziono prefix
		{
			number++;
			number+=CountPrefixAppearance(tmp->l_son, prefix);
			number+=CountPrefixAppearance(tmp->r_son, prefix);
		}

		if (prefix<(tmp->word).substr(0, prefix.size())) //prefix jest "mniejszy"
		{
			number+=CountPrefixAppearance(tmp->l_son, prefix);
		}

		if (prefix>(tmp->word).substr(0, prefix.size())) //prefix jest "większy"
		{
			number+=CountPrefixAppearance(tmp->r_son, prefix);
		}
		
	}
	return number;
}

		
	
	


int main() {
	char choice;
	TreeElement* root = nullptr;
		
	while (true) {
		cout << "\nWybierz opcje\n"
			<< "1. Wstawienie nowego slowa\n"
			<< "2. Usuniecie danego slowa\n"
			<< "3. Wyszukanie w slowniku zadanego slowa\n"
			<< "4. Obliczenie liczby slow o danym prefiksie\n"
			<< "5. Wyswietlenie struktury drzewa wraz z elementami\n"
			<< "6. Wykonanie skryptu polecen (z pliku in.txt):\n"
			<< "1) W x = wstaw x\n"
			<< "2) U x = usun x\n"
			<< "3) S x = szukaj x(odpowiedz: TAK / NIE)\n"
			<< "4) L x = wypisac, ile slow zaczyna sie prefiksem x\n"
			<< "7. Wylacz program\n\n";
		cin >> choice;
		switch (choice) {
			case '1': {
				cout << "Podaj wyraz do dodania:\n";
				string name;
				cin >> name;
				root = InsertElement(root,name);
				
				break;
			}
			case '2': {
				cout << "Podaj wyraz do usuniecia:\n";
				string name;
				cin >> name;
				root = DeleteNode(root, name);
				break;
			}
			case '3': {
				cout << "Podaj wyraz do wyszukania:\n";
				string name;
				cin >> name;
				if (Find(root, name))
					cout << "Podany wyraz istnieje\n";
				else
					cout << "Podany wyraz nie istnieje\n";
				break;
			}
			case '4': {
				cout << "Podaj prefix:\n";
				string prefix;
				cin >> prefix;
				cout << CountPrefixAppearance(root, prefix) <<  "\n";
				break;
			}
			case '5': {
				PrintTree(root, 0);
				break;
			}
			case '6': {
				int n;
				string command,x;
				fstream file;

				file.open("in.txt", ios::in);

				if (file.good()) {
					file >> n;

					for (int i = 0; i < n; ++i) {
						file >> command>>x;
						if (command == "W") {
							root = InsertElement(root, x);
						}
						if(command == "U")
							root = DeleteNode(root, x);
						if(command == "S")
							if (Find(root, x))
								cout << "TAK\n";
							else
								cout << "NIE\n";
						if (command == "L") {
							cout << CountPrefixAppearance(root, x) << "\n";
						}
					}
					file.close();
				}
				break;
			}
			case '7': {
				return 0;
			}
			default: {
				cout << "Niepoprawna opcja\n";
				break;
			}
		}
	}
}

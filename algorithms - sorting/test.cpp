#include <iostream>
#include <string>

using namespace std;

int f(string s)
{
	return s[0]-'a';
}

int main()
{
	string word;
	cin >> word;
	cout << word[0] << " "  << f(word) << endl;
	return 0;
}

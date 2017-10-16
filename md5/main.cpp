#include"md5.hpp"

int main() {
	Md5 m;
	m.update("lzl");
	cout << m.hexdigest() << endl;
	m.update("hello world!");
	cout << m.hexdigest() << endl;
	m.update("lll");
	cout << m.hexdigest() << endl;

	return 0;
}
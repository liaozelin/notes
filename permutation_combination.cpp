#include<iostream>
#include<string>
using namespace std;

/*
 * 给定字符串的全排列
 */
void swap_char(string& s, int a, int b) {
    char t = s[a];
    s[a] = s[b];
    s[b] = t;
}
void perm_driver(string& s, int cur, int len) {
    if (cur == len) {
        cout << s << endl;
        return;
    }
    for (int i = cur; i < len; ++i) {
        swap_char(s, cur, i);
        perm_driver(s, cur + 1, len);
        swap_char(s, cur, i);
    }
}
void perm(string s) {
    int n = s.size();
    perm_driver(s, 0, n);
}

/*
 * 给定字符串，求所有可能的组合
 */
char target[100];
int tar_count = 0;
void print() {
    for (int i = 0; i < tar_count; ++i) {
        cout << target[i];
    }
    cout << endl;
}
void comb_driver(char* str, int cur, int target_len, int len) {
    if (target_len == 0) {
        print();
        return;
    }
    int max = len - target_len + 1;
    for (int i = cur; i < max; ++i) {
        target[tar_count++] = str[i];
        comb_driver(str, i + 1, target_len - 1, len);
        tar_count--;
    }
}
void comb(string s) {
    int n = s.size();
    char str[50];
    strncpy(str, s.c_str(), n);
    // 长度从1到n
    for (int i = 1; i <= n; ++i) {
        comb_driver(str, 0, i, n);
    }
}

int main() {
    cout << "permutation:\n";
    perm("abc");
    cout << "\ncombination:\n";
    comb("abc");

    return 0;
}


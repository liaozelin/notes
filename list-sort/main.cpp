/*
 * 使用快速排序对单链表进行排序
 * 也可用归并排序：快慢两个指针将链表一切为二，递归再合并
 * LeetCode题目：
 *   https://leetcode.com/problems/insertion-sort-list/description/
 *   https://leetcode.com/problems/sort-list/description/
 * */

#include<iostream>
#include<vector>
using namespace std;

class List {
    private:
        struct Node {
            int val;
            Node* next;
            Node(int v = 0, Node* n = NULL) : val(v), next(n) {}
        };
        Node* head;
        int size;
    public:
        List() {
            head = new Node();
            size = 0;
        }
        ~List() {
            Node* temp;
            while (head != NULL) {
                temp = head;
                head = head->next;
                delete temp;
            }
        }
 
        void create(vector<int> arr) {
            size = arr.size();
            Node* temp = head;
            for (int i = 0; i < size; ++i) {
                temp->next = new Node(arr[i], NULL);
                temp = temp->next;
            }
        }

        void print() {
            Node* temp = head->next;
            while (temp != NULL) {
                cout << temp->val << ' ';
                temp = temp->next;
            }
            cout << endl;
        }

        void sort() {
            if (head->next == NULL) return;
            Node* high = head->next;
            while (high->next != NULL) high = high->next;
            Node* tail = new Node();
            high->next = tail;
            _sort(head->next, tail);
            high->next = NULL;
            delete tail;
        }

    private:
        // 由于是链表，不能随机访问，使用[low, high]会造成麻烦
        void _sort(Node* low, Node* high) { // [low, high)
            if (low == high || low->next == high) return;
            Node* mid = partition(low, high);
            _sort(low, mid);
            _sort(mid->next, high);
        }
        Node* partition(Node* low, Node* high) {
            int meta = low->val;
            Node* i = low;
            Node* j = low->next;
            while (j != high) {
                if (j->val < meta) {
                    i = i->next;
                    swap(i, j);
                }
                j = j->next;
            }
            swap(low, i);
            return i;
        }
        void swap(Node* a, Node* b) {
            if (a == NULL || b == NULL) return;
            int t = a->val;
            a->val = b->val;
            b->val = t;
        }
};

int main() {
    vector<int> a;
    int input;
    while (cin >> input && input != -1) {
        a.push_back(input);
    }
    List al;
    al.create(a);
    al.sort();
    al.print();

    return 0;
}

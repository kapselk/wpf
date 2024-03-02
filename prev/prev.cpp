/*
 * Laboratorium 11 - Poprzednie w zakresie
 * Autor: Kacper Omieliańczyk
 */

#include "prev.h"
#include <algorithm>
#include <memory>
#include <climits>
#include <iostream>

const int ID = 0;

using namespace std;

typedef long long ll;

class Node {
	public:
		shared_ptr<Node> lchild = nullptr, rchild = nullptr;
		ll l, r;
		int val = ID;
		
		// konstruktor Node, dla danego przedzialu [lb, rb] i wartosci w wierzcholku v
		Node(ll lb, ll rb, int v) {
			l = lb;
			r = rb;
			val = v;
		}
		
		// konstruktor Node, dla danych dzieci lx, rx
		Node(shared_ptr<Node> lx, shared_ptr<Node> rx) {
			l = lx->l;
			r = rx->r;
			lchild = lx;
			rchild = rx;
			val = max(lx->val, rx->val);
		}
		
		// metoda rozszerzajaca Node -> o ile nie istnieja tworzy dzieci i inicjuje je
		void extend() {
			if (lchild == nullptr && l < r) {
				ll m = (l + r) >> 1LL;
				lchild = make_shared<Node>(l, m, ID);
				rchild = make_shared<Node>(m+1, r, ID);
			}
		}
		
		// metoda zwracajaca max na przedziale [a, b] w obszarze obejmowanym przez Node
		int get(ll a, ll b) {
			if (a <= l && b >= r) return val;
			if (b < l || a > r) return ID;
			extend();
			return max(lchild->get(a, b), rchild->get(a, b));
		}
};

typedef shared_ptr<Node> pNode;

// modyfikacja drzewa zaczepionego w v, zwraca nowy korzen do drzewa, ktore jest kopia tego zaczepionego w v z zmodyfikowana odpowiednia sciezka od korzenia do wierzcholka odpowiadajacego polu k
pNode update(pNode v, ll k, int x) {
	ll l = v->l, r = v->r;
	if (l == r) return make_shared<Node>(l, r, x);
	v->extend();
	ll m = (l + r) >> 1LL;
	if (k <= m) return make_shared<Node>(update(v->lchild, k, x), v->rchild);
	else return make_shared<Node>(v->lchild, update(v->rchild, k, x));
}

const ll M = abs((ll) INT_MIN);
const ll MX = M + (ll) INT_MAX + 69;
vector<pNode> roots;

// inicjuje drzewo na podstawie tablicy x
void init(const vector<int> &x) {
	int n = (int) x.size();
	roots.resize(n + 1);
	roots[0] = make_shared<Node>(0LL, MX, ID);
	for (int i = 0; i < n; i++) {
		roots[i + 1] = update(roots[i], ((ll) x[i]) + M, i + 1);
	}
}

// zwraca wynik - max na przedziale czasu [1, .. i] z wartosci [lo, hi]
int prevInRange(int i, int lo, int hi) {
	ll a = (ll) lo + M;
	ll b = (ll) hi + M;
	return roots[i + 1]->get(a, b) - 1;
}

// dodaje nowe pole w tablicy
void pushBack(int v) {
	int id = (int) roots.size();
	ll val = (ll) v + M;
	roots.push_back(update(roots.back(), val, id));
}

/*
 * czysci drzewo, wywolanie metody clear() na wektorze korzeni wywoluje rekurencyjnie
 * destruktory wierzcholkow, wskazniki z shared_ptr posiadaja destruktor domyslny wiec zwalniaja heap
 */
void done() {
	roots.clear();
}

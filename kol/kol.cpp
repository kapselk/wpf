/*
 * Laboratorium 9 - Kolejki
 * Autor: Kacper Omieliańczyk
 */
#include "kol.h"

#include <cstdlib>
#include <cassert>

// poczatek implementacji kolejki dwukierunkowej

struct Lista {
	interesant *head, *tail;
};

// konstruktor listy
Lista makeList() {
	Lista ret;
	ret.head = (interesant*) malloc(sizeof(interesant));
	ret.tail = (interesant*) malloc(sizeof(interesant));
	ret.head->id = -1;
	ret.tail->id = -1;
	ret.head->l1 = NULL;
	ret.head->l2 = ret.tail;
	ret.tail->l1 = NULL;
	ret.tail->l2 = ret.head;
	return ret;
}

// czy l jest pusta
bool isempty(const Lista l) {
	return l.head->l2 == l.tail;
}

// zwraca pierwszy element kolejki
interesant* front(const Lista l) {
	return l.head->l2;
}

// zlepia el z nw w wyniku usuniecia old
void link(interesant* el, interesant* old, interesant* nw) {
	if (el) {
		if (el->l1 == old) {
			el->l1 = nw;
		} else {
			el->l2 = nw;
		}
	}
}

// wrzuca na koniec kolejki l element el
void push_back(Lista &l, interesant* el) {
	el->l1 = l.tail;
	el->l2 = l.tail->l2;
	link(l.tail->l2, l.tail, el);
	l.tail->l2 = el;
}

// usuwa element el
void remove_interesant(interesant* el) {
	link(el->l1, el, el->l2);
	link(el->l2, el, el->l1);
}

// usuwa ostatni element kolejki l
void pop_front(Lista &l) {
	if (!isempty(l)) {
		remove_interesant(l.head->l2);
	}
}

// odwraca kolejke
void reverse(Lista &l) {
	interesant* tmp = l.head;
	l.head = l.tail;
	l.tail = tmp;
}

// dolacza kolejke l2 na koniec kolejki l1
void append(Lista &l1, Lista &l2) {
	interesant *b = l1.tail;
	interesant *f = l2.head;
	link(b->l2, b, f->l2);
	link(f->l2, f, b->l2);
	b->l2 = f;
	f->l2 = b;
	l1.tail = l2.tail;
	l2.head = f;
	l2.tail = b;
}

// koniec implementacji kolejki dwukierunkowej

// vector przechowujacy kolejki
std::vector<Lista> kol;
// licznik numerkow
int T;

// funkcja do otwierania urzedu - inicjalizuje kolejki
void otwarcie_urzedu(int m) {
	kol.resize(m);
	T = 0;
	for (int i = 0; i < m; i++) {
		kol[i] = makeList();
	}
}

// zwraca nowego interesanta umieszczonego na koncu kolejki k
interesant* nowy_interesant(int k) {
	interesant* ret = (interesant*) malloc(sizeof(interesant));
	ret->id = T++;
	push_back(kol[k], ret);
	return ret;
}

// zwraca numer interesanta i
int numerek(interesant* i) {
	return i->id;
}

// zrzuca pierwszy element kolejki k
interesant* obsluz(int k) {
	if (isempty(kol[k])) return NULL;
	else {
		interesant* ret = front(kol[k]);
		pop_front(kol[k]);
		return ret;
	}
}

// usuwa interesanta i z kolejki obecnej i dorzuca go na koniec kolejki k
void zmiana_okienka(interesant *i, int k) {
	remove_interesant(i);
	push_back(kol[k], i);
}

// przelacza kolejke k1 na koniec kolejki k2
void zamkniecie_okienka(int k1, int k2) {
	append(kol[k2], kol[k1]);
}

/*
 * przeglada z i1 w obu kierunkach dopoki nie napotka i2
 * usuwa i zwraca liste elementow na poprawnej drodze
 * zlozonosc O(|i2-i1|) poniewaz w kazdym kroku przesuwa sie o jedna pozycje oraz konczy najpozniej jak napotka i2 idac z i1
 */
std::vector<interesant*> fast_track(interesant *i1, interesant *i2) {
	if (i1 == i2) {
		remove_interesant(i1);
		return std::vector<interesant*>{i1};
	}
	interesant *pa = i1->l1, *pb = i1->l2;
	interesant *la = i1, *lb = i1;
	if (pa == i2 || pb == i2) {
		remove_interesant(i1);
		remove_interesant(i2);
		return std::vector<interesant*>{i1, i2};
	}
	std::vector<interesant*> q1, q2;
	q1.push_back(i1); q2.push_back(i1);
	q1.push_back(pa); q2.push_back(pb);
	while ((pa != NULL || pb != NULL) && (pa->id != -1 || pb->id != -1)) {
		interesant *ta = pa, *tb = pb;
		if (pa != NULL && pa->id != -1) {
			pa = (pa->l1 == la ? pa->l2 : pa->l1);
			q1.push_back(pa);
			la = ta;
		}
		if (pb != NULL && pb->id != -1) {
			pb = (pb->l1 == lb ? pb->l2 : pb->l1);
			q2.push_back(pb);
			lb = tb;
		}
		if (pa == i2) {
			for (interesant* i : q1) {
				remove_interesant(i);
			}
			return q1;
		}
		if (pb == i2) {
			for (interesant* i : q2) {
				remove_interesant(i);
			}
			return q2;
		}
	}
	assert(true);
	return std::vector<interesant*>();
}

// odwraca kolejke k
void naczelnik(int k) {
	reverse(kol[k]);
}

// zamyka urzad - dokonuje destrukcji kolejek w tym zwalnia atrapy
std::vector<interesant*> zamkniecie_urzedu() {
	int m = (int) kol.size();
	std::vector<interesant*> ret;
	for (int i = 0; i < m; i++) {
		while (!isempty(kol[i])) {
			ret.push_back(front(kol[i]));
			pop_front(kol[i]);
		}
		free(kol[i].head);
		free(kol[i].tail);
	}
	kol.clear();
	return ret;
}

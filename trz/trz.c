/*
 * Zadanie Trzy różne
 * Autor: Kacper Omieliańczyk
 * T: O(n)
 * M: O(n)
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Tstr {
	int* id;
	int sz;
};

typedef struct Tstr* T;

int inf;
int* type;
int* cord;

// konstuktor obiektu T
T createT() {
	T ret = (T) malloc(sizeof(struct Tstr));
	ret->id = (int*) malloc(3 * sizeof(int));
	//memset(ret->id, -1, 3 * sizeof(int));
	for (int i = 0; i < 3; i++) ret->id[i] = -1;
	ret->sz = 0;
	return ret;
}


// destruktor obiektu T
void destroy(T t) {
	if (t->id != NULL) free(t->id);
	free(t);
}

// move - wyzeruj pozycje t[suf] i przesun sufiks t[suf+1 .. size-1] na pozycje t[suf .. size-2]
void move(int* tab, int size, int suf) {
	// suf >= 0 && suf < size
	if (suf < 0 || suf >= size) return;
	for (int i = suf + 1; i < size; i++) {
		tab[i - 1] = tab[i];
	}
	tab[size - 1] = -1; // ostatni element wolny
}

// dodawanie nowego elementu do strukta T
void push(T t, int vid) {
	for (int i = 0; i < 3; i++) {
		if (t->id[i] != -1 && type[t->id[i]] == type[vid]) {
			move(t->id, 3, i);
			t->sz--;
		}
	}
	if (t->sz == 3) {
		move(t->id, 3, 0);
		t->sz--;
	}
	t->id[t->sz++] = vid;
}

// copy b into a
void copy(T a, T b) {
	a->sz = b->sz;
	for (int i = 0; i < 3; i++) {
		a->id[i] = b->id[i];
	}
}

// zwraca minimum z wartosci a, b
int min(int a, int b) {
	if (a <= b) return a;
	else return b;
}

// zwraca maksimum z wartosci a, b
int max(int a, int b) {
	if (a >= b) return a;
	else return b;
}

// zwraca wartosc bezwzgledna z a
int abs(int a) {
	if (a < 0) return -a;
	else return a;
}

// wyliczanie wyniku dla srodka typu t i koordynatach c, gdzie zbior lewych kandydatow to left a prawych to right ; minimum = czy liczymy minimum
int calc(T left, T right, int t, int c, bool minimum) {
	// t, c - typ i kordy środka
	int ret = (minimum ? inf : 0);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (left->id[i] == -1 || right->id[j] == -1) continue;
			int ta = type[left->id[i]], tb = type[right->id[j]];
			int ca = cord[left->id[i]], cb = cord[right->id[j]];
			// ta, tb - typy kandydatow odpowiednio na lewo i prawo od srodka
			// ca, cb - koordynaty kandydatow odpowiednio na lewo i prawo od srodka
			if (ta != tb && ta != t && tb != t && ca <= c && c <= cb) {
				if (minimum) ret = min(ret, max(abs(ca - c), abs(cb - c)));
				else ret = max(ret, min(abs(ca - c), abs(cb - c)));
			}
		}
	}
	return ret;
}

int main() {
	int n;
	scanf("%d", &n);
	if (n <= 0) {
		// szczegolny przypadek - odpowiedz = 0
		printf("0 0\n");
		return 0;
	}
	type = (int*) malloc((unsigned int) n * sizeof(int));
	cord = (int*) malloc((unsigned int) n * sizeof(int));
	if (type == NULL || cord == NULL) {
		printf("straszny blad pamieci - nie udalo sie zaalokowac pamieci!\n");
		return 1;
	}
	for (int i = 0; i < n; i++) {
		scanf("%d %d", &type[i], &cord[i]);
	}
	inf = cord[n - 1] + 1;
	T* pref = (T*) malloc((unsigned int) n * sizeof(T));
	T* suf = (T*) malloc((unsigned int) n * sizeof(T));
	if (pref == NULL || suf == NULL) {
		printf("straszny blad pamieci - nie udalo sie zaalokowac pamieci!\n");
		return 1;
	}
	for (int i = 0; i < n; i++) {
		pref[i] = createT();
		if (pref[i] == NULL || pref[i]->id == NULL) {
			printf("straszny blad pamieci - nie udalo sie zaalokowac pamieci!\n");
			return 1;
		}
		if (i > 0) copy(pref[i], pref[i-1]);
		push(pref[i], i);
	}
	for (int i = (n - 1); i >= 0; i--) {
		suf[i] = createT();
		if (suf[i] == NULL || suf[i]->id == NULL) {
			printf("straszny blad pamieci - nie udalo sie zaalokowac pamieci!\n");
			return 1;
		}
		if (i < (n - 1)) copy(suf[i], suf[i+1]);
		push(suf[i], i);
	}
	int ansMin = inf, ansMax = 0;
	for (int i = 0; i < n; i++) {
		// i - srodkowy hotel
		ansMin = min(ansMin, calc(pref[i], suf[i], type[i], cord[i], true));
	}
	T lf, rf;
	lf = suf[0];
	rf = pref[n - 1];
	// dla przypadku maksimum lf i rf to odpowiednio lewy i prawy kandydat dla wszystkich srodkow
	for (int i = 0; i < n; i++) {
		// i - srodkowy hotel
		ansMax = max(ansMax, calc(lf, rf, type[i], cord[i], false));
	}
	if (ansMin == inf) {
		printf("0 0\n");
	} else {
		printf("%d %d\n", ansMin, ansMax);
	}
	// dealokacja pamieci
	for (int i = 0; i < n; i++) {
		destroy(pref[i]);
		destroy(suf[i]);
	}
	// lf i rf zwolnione - bo lf = suf[0] i rf = pref[n-1]
	free(type);
	free(cord);
}

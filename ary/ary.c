/*
 * Laboratorium 3 - Arytmetyka
 * Autor: Kacper Omieliańczyk
 */
#include "ary.h"

const double LIBeps = 1e-12;

bool iszero(double x) {
	return fabs(x) < LIBeps;
}

bool isequal(double x, double y) {
	return iszero(x - y);
}
 
double fix_neg_zero(double x) {
	if (iszero(x)) {
		return 0.0;
	} else {
		return x;
	}
}

bool leq(double a, double b) {
	return a <= b || isequal(a, b) || a < b;
}

bool geq(double a, double b) {
	return a >= b || isequal(a, b) || a > b;
}

// KONSTRUKTORY

wartosc wartosc_od_do(double x, double y) {
	wartosc ret;
	ret.left = x;
	ret.right = y;
	return ret;
}

wartosc wartosc_dokladna(double x) {
	return wartosc_od_do(x, x);
}

wartosc wartosc_dokladnosc(double x, double p) {
	double diff = fabs(x) * (p / 100.0);
	return wartosc_od_do(x - diff, x + diff);	
}

// SELEKTORY

bool in_wartosc(wartosc w, double x) {
	if (leq(w.left, w.right)) {	// w = przedzial [a, b]
		return (leq(w.left, x) && leq(x, w.right));
	} else {	// w = przedzial (-inf, b] U [a, +inf)
		return (leq(w.left, x) || leq(x, w.right));
	}
}

double min_wartosc(wartosc w) {
	if (isnan(w.left) || isnan(w.right)) {
		return NAN;
	}
	if (leq(w.left, w.right)) {	// w = przedzial [a, b]
		return w.left;
	} else {	// w = przedzial (-inf, b] U [a, +inf) - odpowiedz -inf
		return -HUGE_VAL;
	}
}

double max_wartosc(wartosc w) {
	if (isnan(w.left) || isnan(w.right)) {
		return NAN;
	}
	if (leq(w.left, w.right)) {	// w = przedzial [a, b]
		return w.right;
	} else {	// w = przedzial (-inf, b] U [a, +inf) - odpowiedz inf
		return HUGE_VAL;
	}
}

/*
 * Średnia wartość (min, max):
 * (-inf, +inf) -> NaN
 * (-inf, max) -> -inf
 * (min, inf) -> inf
 * (min, max) -> (min + max) / 2
 */

double sr_wartosc(wartosc w) {
	return (min_wartosc(w) + max_wartosc(w)) / 2.0;
}

// MODYFIKATORY

/*
 * plus(a, b):
 * a = (-inf, y1] U [x1, inf) ; b = (-inf, y2] U [x2, inf) - ans = (-inf, inf)
 * BSO a = (-inf, y1] U [x1, inf) ; b = [x2, y2] oraz x1 + x2 >= y1 + y2 - ans = (-inf, inf)
 * pozostałe - ans = [x1 + y1 ; x2 + y2]
 */

wartosc plus(wartosc a, wartosc b) {
	if (a.left > a.right && b.left > b.right) {
		return wartosc_od_do(-HUGE_VAL, HUGE_VAL);
	} else if ((a.left > a.right || b.left > b.right) && leq((a.left + b.left), (a.right + b.right))) {
		return wartosc_od_do(-HUGE_VAL, HUGE_VAL);
	} else {
		return wartosc_od_do(a.left + b.left, a.right + b.right);
	}
}

/*
 * minus(a, b) = plus(a, przeciwna(b))
 * 
 * przeciwna(x) = {-x.right, -x.left}
 * 
 * fix_neg_zero(x) naprawia -0:
 * 	- 0 dla -0.0
 * 	- x dla pozostalych
 * 
 */

wartosc przeciwna(wartosc x) {
	return wartosc_od_do(fix_neg_zero(-x.right), fix_neg_zero(-x.left));
}

wartosc minus(wartosc a, wartosc b) {
	return plus(a, przeciwna(b));
}

// mnozenie(double x, double y) - zwraca x * y z uwzględnieniem wyjątku 0*(inf) = 0

double mnozenie(double x, double y) {
	if ((iszero(x) && isinf(fabs(y))) || (iszero(y) && isinf(fabs(x)))) return 0.0; // fabs(y) >= HUGE_VAL  --  fabs(x) >= HUGE_VAL
	else return x * y;
}

double min(double a, double b) {
	if (leq(a, b)) {
		return a;
	} else {
		return b;
	}
}

double max(double a, double b) {
	if (leq(b, a)) {
		return a;
	} else {
		return b;
	}
}

/*
 * merge(a, b) - zwraca sume zbiorów a, b
 * dla a i b spojnych lub:
 * b - niespojne, ale a ma na jednym krańcu +-inf
 */
 
wartosc merge(wartosc a, wartosc b) {
	if (isnan(a.left) || isnan(a.right)) return b;
	if (isnan(b.left) || isnan(b.right)) return a;
	double a1 = min_wartosc(a);
	double a2 = max_wartosc(a);
	double b1 = min_wartosc(b);
	double b2 = max_wartosc(b);
	if (leq(b.left, b.right)) {
		if (a2 < b1) {
			return wartosc_od_do(b1, a2);
		} else if (b2 < a1) {
			return wartosc_od_do(a1, b2);
		} else {
			return wartosc_od_do(min(a1, b1), max(a2, b2));
		}
	} else {
		// b = (-inf, b2] U [b1, +inf)
		// a = (-inf, a2] lub [a1, +inf)
		if (leq(a.left, b.right)) {
			if (leq(b.left, a.right)) {
				return wartosc_od_do(-HUGE_VAL, HUGE_VAL);
			} else {
				return wartosc_od_do(b.left, max(a.right, b.right));
			}
		} else {
			return wartosc_od_do(min(a.left, b.left), b.right);
		}
	}
}

/*
 * razy_spojne(a, b) - zwraca wynik mnozenia dla spojnych przedziałów a, b
 */
 
wartosc razy_spojne(wartosc a, wartosc b) {
	double tmp1 = mnozenie(min_wartosc(a), min_wartosc(b));
	double tmp2 = mnozenie(min_wartosc(a), max_wartosc(b));
	double tmp3 = mnozenie(max_wartosc(a), min_wartosc(b));
	double tmp4 = mnozenie(max_wartosc(a), max_wartosc(b));
	return wartosc_od_do(min(tmp1, min(tmp2, min(tmp3, tmp4))), max(tmp1, max(tmp2, max(tmp3, tmp4))));
}

/*
 * razy(a, b):
 * - a i b spojne - ans: razy_spojne(a, b)
 * - dokladnie jedno z a, b spojne: BSO niech a spojne, b = b1 U b2:
 * ans: razy_spojne(a, b1) U razy_spojne(a, b2)
 * - a i b nie sa spojne, niech a = a1 U a2 oraz b = b1 U b2
 * ans: razy_spojne(a1, b1) U razy_spojne(a1, b2) U razy_spojne(a2, b1) U razy_spojne(a2, b2)
 */

wartosc razy(wartosc a, wartosc b) {
	if (isnan(a.left) || isnan(a.right) || isnan(b.left) || isnan(b.right)) return wartosc_od_do(NAN, NAN);
	if (leq(a.left, a.right) && leq(b.left, b.right)) {
		return razy_spojne(a, b);
	} else if (leq(a.left, a.right)) {
		wartosc b1 = wartosc_od_do(-HUGE_VAL, b.right);
		wartosc b2 = wartosc_od_do(b.left, HUGE_VAL);
		return merge(razy_spojne(a, b1), razy_spojne(a, b2));
	} else if (leq(b.left, b.right)) {
		wartosc a1 = wartosc_od_do(-HUGE_VAL, a.right);
		wartosc a2 = wartosc_od_do(a.left, HUGE_VAL);
		return merge(razy_spojne(b, a1), razy_spojne(b, a2));
	} else {
		wartosc a1 = wartosc_od_do(-HUGE_VAL, a.right);
		wartosc a2 = wartosc_od_do(a.left, HUGE_VAL);
		wartosc b1 = wartosc_od_do(-HUGE_VAL, b.right);
		wartosc b2 = wartosc_od_do(b.left, HUGE_VAL);
		wartosc tmp1 = razy_spojne(a1, b1);
		wartosc tmp2 = razy_spojne(a1, b2);
		wartosc tmp3 = razy_spojne(a2, b1);
		wartosc tmp4 = razy_spojne(a2, b2);
		return merge(tmp1, merge(tmp2, merge(tmp3, tmp4)));
	}
}

/*
 * podzielic(a, b):
 * - a = 0 i b != 0 ; ans: 0
 * - b = 0 lub b = nan lub a = nan ; ans = [nan, nan]
 * - a != [0] i b = (-inf, inf) ; ans = (-inf, inf)
 * - gdy b = [b1, 0] ; ans = razy(a, [-inf, 1/b1])
 * - gdy b = [x, y] ; ans = razy(a, [1/y, 1/x])
 */
 
wartosc podzielic(wartosc a, wartosc b) {
	if ((isnan(a.left) || isnan(a.right)) || (isnan(b.left) || isnan(b.right)) || (iszero(b.left) && iszero(b.right))) return wartosc_dokladna(NAN);
	if (iszero(a.left) && iszero(a.right)) return wartosc_dokladna(0);
	if (isinf(b.left) && isinf(b.right)) { // b.left <= -HUGE_VAL && b.right >= HUGE_VAL
		return wartosc_od_do(-HUGE_VAL, HUGE_VAL);
	}
	if (iszero(b.left)) {
		return razy(a, wartosc_od_do(fix_neg_zero((1.0 / b.right)), HUGE_VAL));
	} else if (iszero(b.right)) {
		return razy(a, wartosc_od_do(-HUGE_VAL, fix_neg_zero((1.0 / b.left))));
	} else {
		return razy(a, wartosc_od_do(fix_neg_zero((1.0 / b.right)), fix_neg_zero((1.0 / b.left))));
	}
}

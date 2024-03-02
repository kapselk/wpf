/*
 * Laboratorium 7 - Origami
 * Autor: Kacper Omieliańczyk (459458)
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

const double eps = 1e-12;

bool isZero(double x) {
	return fabs(x) < eps;
}

bool isEqual(double x, double y) {
	return isZero(x - y);
}

bool leq(double x, double y) {
	return isEqual(x, y) || (x < y) || (x <= y);
}

typedef enum{Rectangle, Circle, Folding} Type;

typedef struct {
	double x, y;
} Point;

typedef struct {
	Point p1, p2;
} Rect;

typedef struct {
	Point O;
	double radius;
} Circ;

struct PaperT;
typedef struct PaperT* Paper;

typedef struct {
	Paper folded;
	Point p1, p2;
} Fold;

typedef union {
	Rect rect;
	Circ circ;
	Fold fold;
} PaperUnion;

struct PaperT {
	PaperUnion p;
	Type type;
};

// konstruktor punktu
Point makePoint(double x, double y) {
	Point ret;
	ret.x = x;
	ret.y = y;
	return ret;
}

// konstruktor kartki papieru
Paper makePaper(Type type, PaperUnion p) {
	Paper ret = (Paper) malloc(sizeof(struct PaperT));
	ret->p = p;
	ret->type = type;
	return ret;
}

// konstruktor kartki papieru typu prostokat
Paper makeRect(Point p1, Point p2) {
	Rect ret;
	ret.p1 = p1;
	ret.p2 = p2;
	PaperUnion pu;
	pu.rect = ret;
	return makePaper(Rectangle, pu);
}

// konstruktor kartki papieru typu kolko
Paper makeCirc(Point O, double radius) {
	Circ ret;
	ret.O = O;
	ret.radius = radius;
	PaperUnion pu;
	pu.circ = ret;
	return makePaper(Circle, pu);
}

// konstruktor kartki papieru typu zgiecie
Paper makeFold(Paper folded, Point p1, Point p2) {
	Fold ret;
	ret.folded = folded;
	ret.p1 = p1;
	ret.p2 = p2;
	PaperUnion pu;
	pu.fold = ret;
	return makePaper(Folding, pu);
}

/*
 * iloczyn wektorowy(p, q, r)
 * zwraca:
 * 0 - Q lezy na prostej PR
 * 1 - po prawej
 * -1 - po lewej
 * 
 */
int crossProduct(Point p, Point q, Point r) {
	double res = (r.x - p.x) * (q.y - p.y) - (r.y - p.y) * (q.x - p.x);
	if (isZero(res)) return 0;
	else if (res > 0) return 1;
	else return -1;
}

/*
 * prosta(a, b)
 * zwraca w Point (bo para elementow - wsp. ax + by) wspolczynniki prostej przechodzacej
 * przez punkty a, b
 * 
 */
Point line(Point a, Point b) {
	double ar = (a.y - b.y) / (a.x - b.x);
	double br = a.y - (a.x * ar);
	return makePoint(ar, br);
}

/*
 * odbicie punkt r wzgledem prostej przechodzacej przez p i q
 * 
 */
Point mirror(Point p, Point q, Point r) {
	if (isEqual(p.x, q.x)) {
		return makePoint((p.x + p.x - r.x), r.y);
	} else if (isEqual(p.y, q.y)) {
		return makePoint(r.x, (p.y + p.y - r.y));
	} else {
		Point ln = line(p, q);
		double a = ln.x, b = ln.y;
		double xs = (a * (r.y - b) + r.x) / (a * a + 1.0);
		double ys = a * xs + b;
		double xr = xs * 2.0 - r.x;
		double yr = ys * 2.0 - r.y;
		return makePoint(xr, yr);
	}
}

/*
 * przebicie kartki ,paper' w punkcie ,pt'
 * zwraca wynik - liczbe przebic
 * 
 */
int puncture(Paper paper, Point pt) {
	if (paper->type == Rectangle) {
		Rect r = paper->p.rect;
		Point p1 = r.p1, p2 = r.p2;
		int res = ((leq(p1.x, pt.x) && leq(pt.x, p2.x) && leq(p1.y, pt.y) && leq(pt.y, p2.y)) ? 1 : 0);
		return res;
	} else if (paper->type == Circle) {
		Circ c = paper->p.circ;
		Point O = c.O;
		double radius = c.radius;
		int res = (leq(((pt.x - O.x) * (pt.x - O.x) + (pt.y - O.y) * (pt.y - O.y)), (radius * radius)) ? 1 : 0);
		return res;
	} else {
		Fold f = paper->p.fold;
		Paper folded = f.folded;
		Point p1 = f.p1, p2 = f.p2;
		int product = crossProduct(p1, p2, pt);
		if (product == 0) {
			return puncture(folded, pt);
		} else if (product == 1) {
			return 0;
		} else {
			Point mr = mirror(p1, p2, pt);
			return puncture(folded, pt) + puncture(folded, mr);
		}
	}
}

int main() {
	int n, q;
	scanf("%d %d", &n, &q);
	// papers - tablica na kartki
	Paper* papers = (Paper*) malloc((unsigned int) n * sizeof(Paper));
	for (int i = 0; i < n; i++) {
		char type;
		scanf(" %c", &type);
		if (type == 'P') {
			double x1, y1, x2, y2;
			scanf("%lf %lf %lf %lf", &x1, &y1, &x2, &y2);
			papers[i] = makeRect(makePoint(x1, y1), makePoint(x2, y2));
		} else if (type == 'K') {
			double x, y, radius;
			scanf("%lf %lf %lf", &x, &y, &radius);
			papers[i] = makeCirc(makePoint(x, y), radius);
		} else {
			int k;
			double x1, y1, x2, y2;
			scanf("%d %lf %lf %lf %lf", &k, &x1, &y1, &x2, &y2);
			papers[i] = makeFold(papers[k - 1], makePoint(x1, y1), makePoint(x2, y2));
		}
	}
	while (q--) {
		int k;
		double x, y;
		scanf("%d %lf %lf", &k, &x, &y);
		Point pt = makePoint(x, y);
		printf("%d\n", puncture(papers[k - 1], pt));
	}
	// dealokacja pamieci
	for (int i = 0; i < n; i++) {
		free(papers[i]);
	}
	free(papers);
}

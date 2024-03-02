/*
 * Laboratorium 13 - Wyszukiwanie
 * Autor: Kacper Omieliańczyk
 */
#include "wys.h"
#include <vector>
#include <map>
#include <utility>
#include <unordered_map>

using namespace std;

typedef pair<int, int> pii;
typedef long long ll;

struct chash {
	size_t operator()(vector<int> const& v) const {
		ll ret = 0;
		for (int i = 0; i < (int) v.size(); i++) {
			ret *= (ll) 10;
			ret += (ll) v[i];
		}
		return hash<ll>{}(ret);
	}
};

const int INF = 1e9;
int n, k, minAns = INF;
unordered_map<vector<int>, pair<int, int>, chash> M;
//map<vector<int>, pii> M;

// ustawianie w hashmapie wartosci v i optymalnego kierunku dir
void set(const vector<int> &state, int v, int dir) {
	M[state] = {v, dir};
}

// zwraca pare {wartosc, optymalny kierunek} z hashmapy dla state
pii find(const vector<int> &state) {
	auto it = M.find(state);
	if (it != M.end()) return it->second;
	else return {-1, -1};
}

// zwraca status gry po wykonaniu ruchu m zakladajac odpowiedz answer ze statusu state
vector<int> makeMove(const vector<int> &state, int m, bool answer) {
	// state, smaller than m, answer
	vector<int> ret(state);
	if (answer) {
		for (int i = m; i <= n; i++) {
			if (ret[i] <= k) ret[i]++;
		}
	} else {
		for (int i = 1; i < m; i++) {
			if (ret[i] <= k) ret[i]++;
		}
	}
	return ret;
}

// zwraca true iff status jest konczacy gre
bool isEnding(const vector<int> &state) {
	int candidates = 0;
	for (int i = 1; i <= n; i++) {
		if (state[i] <= k) candidates++;
	}
	return candidates == 1;
}

// zwraca minimalna liczbe zapytan dla statusu state
int calculateAnswer(const vector<int> &state, int depth = 0) {
	int ans = find(state).first;
	if (ans != -1) return ans;
	else ans = INF;
	if (isEnding(state)) return 0;
	if (depth > minAns) return INF;
	int direction = -1;
	for (int i = 1; i <= n; i++) {
		vector<int> no = makeMove(state, i, 0), yes = makeMove(state, i, 1);
		if (state == no || state == yes) continue;
		int cost = max(calculateAnswer(no), calculateAnswer(yes));
		if (cost < ans) {
			ans = cost;
			direction = i;
		}
	}
	minAns = min(minAns, depth + ans + 1);
	set(state, ans + 1, direction);
	return ans + 1;
}

// zbudowanie drzewa gry
void build() {
	vector<int> initialState(n + 1, 0);
	calculateAnswer(initialState);
}

int main() {
	int g;
	dajParametry(n, k, g);
	build();
	while (g--) {
		vector<int> state(n + 1, 0);
		while (!isEnding(state)) {
			int m = find(state).second;
			bool ret = mniejszaNiz(m);
			state = makeMove(state, m, ret);
		}
		int ans = -1;
		for (int i = 1; i <= n; i++) {
			if (state[i] <= k) ans = i;
		}
		if (ans != -1) odpowiedz(ans);
		else odpowiedz(1);
	}
}

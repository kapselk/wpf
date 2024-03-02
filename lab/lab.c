// Autor: Kacper Omieliańczyk
#include <stdio.h>
#include <stdlib.h>

int max(int a, int b) { return (a >= b) ? a : b; }
int check(int a, int b) { return (a == 0 ? 0 : (b % a == 0 ? 1 : 0)); }

int main() {
	int n;
	scanf("%d", &n);
	int* tab = (int*) malloc((unsigned int) n * sizeof(int));
	for (int i = 0; i < n; i++) scanf("%d", &tab[i]);
	int ans = 1, lid = n - 1;
	while (lid > 0) {
		int fid = lid - 1, good = 1;
		while (fid >= 0 && good) {
			if (!check(tab[fid], tab[lid])) {
			//	printf("[%d, %d] = [%d, %d]\n", fid, lid, tab[fid], tab[lid]);
				good = 0;
				lid = fid;
			} else {
				ans = max(ans, (lid - fid + 1));
				fid--;
			}
		}
		if (good) lid = 0;
	}
	printf("%d", ans);
}

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <ctime>

clock_t st, et[10];
extern void init(int N, int mSubscriber[]);
extern int subscribe(int mId, int mNum);
extern int unsubscribe(int mId, int mNum);
extern int count(int sId, int eId);
extern int calculate(int sId, int eId);

/////////////////////////////////////////////////////////////////////////

#define MAX_N 200000
#define CMD_INIT 100
#define CMD_SUBSCRIBE 200
#define CMD_UNSUBSCRIBE 300
#define CMD_COUNT 400
#define CMD_CALCULATE 500

static int mSubscriber[MAX_N];

static bool run() {
	int q;
	scanf("%d", &q);

	int n, mId, mNum, sId, eId;
	int cmd, ans, ret = 0;
	bool okay = false;

	for (int i = 0; i < q; ++i) {
		st = clock();
		scanf("%d", &cmd);
		switch (cmd) {
		case CMD_INIT:
			okay = true;
			scanf("%d", &n);
			for (int j = 0; j < n; ++j) {
				scanf("%d", &mSubscriber[j]);
			}
			init(n, mSubscriber);
			et[0] += clock() -st;
			break;
		case CMD_SUBSCRIBE:
			scanf("%d %d %d", &mId, &mNum, &ans);
			ret = subscribe(mId, mNum);
			if (ans != ret) {
				okay = false;
				printf("--------------------->subscribe in count: expected %d, got %d\n", ans, ret);
			}
			et[1] += clock() - st;
			break;
		case CMD_UNSUBSCRIBE:
			scanf("%d %d %d", &mId, &mNum, &ans);
			ret = unsubscribe(mId, mNum);
			if (ans != ret) {
				okay = false;
				printf("--------------------->CMD_UNSUBSCRIBE in count: expected %d, got %d\n", ans, ret);
			}
			et[2] += clock() - st;
			break;
		case CMD_COUNT:
			scanf("%d %d %d", &sId, &eId, &ans);
			ret = count(sId, eId);
			if (ans != ret) {
				okay = false;
				printf("--------------------->CMD_COUNT in count: expected %d, got %d\n", ans, ret);
			}
			et[3] += clock() - st;
			break;
		case CMD_CALCULATE:
			scanf("%d %d %d", &sId, &eId, &ans);
			ret = calculate(sId, eId);
			if(ans != ret) {
				okay = false;
				printf("--------------------->CMD_CALCULATE in count: expected %d, got %d\n", ans, ret);
			}
			et[4] += clock() - st;
			break;
		default:
			okay = false;
			break;
		}
		et[5] += clock() - st;
	}
	return okay;
}

int main() {
	setbuf(stdout, NULL);
	 freopen("sample_input.txt", "r", stdin);

	int T, MARK;
	scanf("%d %d", &T, &MARK);

	for (int tc = 1; tc <= T; tc++) {
		int score = run() ? MARK : 0;
		printf("#%d %d\n", tc, score);
	}
	for(int i = 0 ; i < 6; ++i) { 
		printf("%d %.3f\n", i, (double)et[i] * 1000. / CLOCKS_PER_SEC);
	}
	
	return 0;
}
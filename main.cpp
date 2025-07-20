#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>

#define MAX_M 50
#define MAX_K 1000

#define CMD_INIT        100
#define CMD_ADD			200
#define CMD_REMOVE    	300
#define CMD_TRANSPORT   400

extern void init(int N, int M, int K, int mID[], int aStorage[], int bStorage[], char mAttr[][MAX_M]);
extern void add(int mID, int aStorage, int bStorage, char mAttr[]);
extern void remove(int mID);
extern int transport(int sStorage, int eStorage, char mAttr[]);

static bool run()
{
	int Q, ret, ans;
	int N, M, K, mID, aStorage, bStorage, sStorage, eStorage;
	int mIDArr[MAX_K], aStorageArr[MAX_K], bStorageArr[MAX_K];
	char buf[MAX_M + 1], mAttr[MAX_M], mAttrArr[MAX_K][MAX_M];

	scanf("%d", &Q);
	bool okay = false;

	for (int q = 0; q < Q; ++q)
	{
		int cmd;
		scanf("%d", &cmd);
		switch (cmd) {
		case CMD_INIT:
			scanf("%d %d %d", &N, &M, &K);
			for (int i = 0; i < K; ++i) {
				scanf("%d %d %d %s", &mIDArr[i], &aStorageArr[i], &bStorageArr[i], buf);
				for (int j = 0; j < M; ++j) mAttrArr[i][j] = buf[j];
			}
			init(N, M, K, mIDArr, aStorageArr, bStorageArr, mAttrArr);
			okay = true;
			break;
		case CMD_ADD:
			scanf("%d %d %d %s", &mID, &aStorage, &bStorage, buf);
			for (int i = 0; i < M; ++i) mAttr[i] = buf[i];
			add(mID, aStorage, bStorage, mAttr);
			break;
		case CMD_REMOVE:
			scanf("%d", &mID);
			remove(mID);
			break;
		case CMD_TRANSPORT:
			scanf("%d %d %d %s", &sStorage, &eStorage, &ans, buf);
			for (int i = 0; i < M; ++i) mAttr[i] = buf[i];
			ret = transport(sStorage, eStorage, mAttr);
			if (ans != ret)
				okay = false;
			break;
		default:
			okay = false;
			break;
		}
	}

	return okay;
}

int main()
{
	setbuf(stdout, NULL);
	 freopen("sample_input.txt", "r", stdin);

	int TC, MARK;

	scanf("%d %d", &TC, &MARK);
	for (int tc = 1; tc <= TC; ++tc)
	{
		int score = run() ? MARK : 0;
		printf("#%d %d\n", tc, score);
	}

	return 0;
}
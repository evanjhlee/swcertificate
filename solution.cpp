sdfsadf
sdfsdf
sdf


#include<string>
#include<vector>
#include<iostream>
#include<unordered_map>
#include<queue>
#include<set>
#include<list>
using namespace std; 
#define log(...) //do { 	fprintf(stderr, __VA_ARGS__); } while (0)
#define ff(i, a, b) for (int i = (a); i < (b); ++i)
#define pii pair<int, int>

#if 1
/*
2. 각 테스트 케이스에서 addRect() 로부터 전달되는 mID 값은 1부터 시작하며 1씩 증가한다.
3. 각 테스트 케이스에서 addRect() 의 호출 횟수는 최대 10,000 회 이다.
4. 각 테스트 케이스에서 selectAndMove() 의 호출 횟수는 최대 1,000 회 이다.
5. 각 테스트 케이스에서 moveFront() 의 호출 횟수는 최대 10,000 회 이다.
6. 각 테스트 케이스에서 selectAndErase() 의 호출 횟수는 최대 1,000 회 이다.
7. 각 테스트 케이스에서 check() 의 호출 횟수는 최대 100,000 회 이다.

 */
const int NM = 10'000 + 5;
const int BS = 50;
const int BM = 200;
struct Node { 	int id, y, x, h, w;  } node[NM]; int ndx;
unordered_map<int, Node*> id2node; // mID -> idx
list<Node*> l[BM][BM];
unordered_map<Node*, list<Node*>::iterator> id2iter; // mID -> iterator
int N;
void dbg() {
	ff(i, 0, N/BS) ff(j, 0, N / BS)
	for(auto &n: l[i][j]) {
		log("%-8s - idx=%d, mID=%d, (%d, %d) ~ (%d, %d)\n", __func__, idx, n->id, n->y, n->x, n->y + n->h, n->x+n->w);
	}
	log("--------------------------------------------------\n");
}
void init(int N)
{
	::N = N;
	ndx = 0;
	id2node.clear();
	ff(i,0, 200) ff(j,0, 200) l[i][j].clear();
	id2iter.clear();
}
void addRect(int mID, int mY, int mX, int mHeight, int mWidth)
{
	dbg();
	node[ndx] = { mID, mY, mX, mHeight, mWidth };
	Node* np = &node[ndx];
	id2node[mID] = np;
	id2iter[np] = l[mY/BS][mX/BS].insert(l[mY / BS][mX / BS].begin(), np); // 새로 추가된 노드의 인덱스를 리스트에 추가
	ndx++;
	log("%-8s()\t\t\n", __func__);
}
Node* select(int y1, int x1)
{
	ff(i, max(0, y1 / BS - 1), min(BM, y1 / BS + 2)) {
		ff(j, max(0, x1 / BS - 1), min(BM, x1 / BS + 2)) {
			if (l[i][j].empty()) continue; // 해당 구역에 노드가 없음
			for(auto &n: l[i][j]) {
				if ( n->y <= y1 && n->y + n->h > y1 && n->x <= x1 && n->x + n->w > x1) {
					return n; // 해당 좌표에 있는 노드 반환
				}
			}
		}
	} 

	return nullptr; // 해당 좌표에 있는 노드가 없음
}
void selectAndMove(int y1, int x1, int y2, int x2)
{
	dbg();
	Node*np = select(y1, x1);
	if (np == nullptr) {
		return ; // 해당 좌표에 있는 노드가 없음
	}

	// 이동
	np->y = y2;
	np->x = x2;

	if ( np->y / BS == y1 / BS && np->x / BS == x1 / BS) {
		// 같은 구역에 있는 경우
		log("%-8s(%d %d %d %d)\t\t\n", __func__, y1, x1, y2, x2);
		return; // 이동할 필요 없음
	}
	else {
		// 다른 구역으로 이동
		l[y1 / BS][x1 / BS].erase(id2iter[np]); // 현재 위치에서 제거
		id2iter[np] = l[y2 / BS][x2 / BS].insert(l[y2 / BS][x2 / BS].begin(), np); // 새 위치에 추가
	}


	log("%-8s(%d %d %d %d)\t\t\n", __func__, y1, x1, y2, x2);
}
bool isOverlap(Node* a, Node *b) 
{
	// a와 b가 겹치는지 확인
	return !(a->x + a->w <= b->x || a->x >= b->x + b->w || a->y + a->h <= b->y || a->y >= b->y + b->h);
}
int moveFront(int mID)
{
	dbg();
	int ret = 0;
	if (id2node.find(mID) == id2node.end()) {
		ret = 0;
		log("%-8s(%d)\t\tret=%d\n", __func__, mID, ret);
		return 0; // mID가 존재하지 않음
	}

	Node*np = id2node[mID];

	if (id2iter.find(np) == id2iter.end()) {
		ret = 0;
		log("%-8s(%d)\t\tret=%d\n", __func__, mID, ret);
		return 0; // mID가 존재하지 않음
	}

	l[np->y /BS][np->x / BS].erase(id2iter[np]); // 현재 위치에서 제거
	id2iter[np] = l[np->y / BS][np->x / BS].insert(l[np->y / BS][np->x / BS].begin(), np); // 맨 앞에 추가

	ff(i,max(0, np->y / BS - 1), min(BM, np->y / BS + 2)) {
		ff(j, max(0, np->x / BS - 1), min(BM, np->x / BS + 2)) {
			if (l[i][j].empty()) continue; // 해당 구역에 노드가 없음
			for(auto &n: l[i][j]) {
				if (n->id == mID) continue; // 자기 자신은 제외
				if (isOverlap(n, np)) {
					// 겹치는 경우
					ret = n->id;
					log("%-8s(%d)\t\tret=%d\n", __func__, mID, ret);
					return ret;
				}
			}
		}
	} 


	log("%-8s(%d)\t\tret=%d\n", __func__, mID, ret);
	return ret;
}
int selectAndErase(int mY, int mX)
{
	dbg();
	int ret = 0;
	Node*np = select(mY, mX);
	if (np == nullptr) {
		ret = 0;
		log("%-8s(%d, %d)\t\tret=%d\n", __func__, mY, mX, ret);
		return ret; // mID가 존재하지 않음
	}

	l[mY/BS][mX /BS].erase(id2iter[np]); // 리스트에서 제거
	id2node.erase(np->id); // 맵에서 제거
	id2iter.erase(np); // 맵에서도 제거

	ret = np->id;
	log("%-8s(%d, %d)\tret=%d\n", __func__, mY, mX, ret);
	return ret; // mID가 존재하지 않음
}
int check(int mY, int mX)
{
	dbg();
	int ret = 0;
	Node* np = select(mY, mX);
	if (np ==nullptr) {		
		ret = 0;
		log("%-8s(%d, %d)\t\tret=%d\n", __func__, mY, mX, ret);
		return ret; // mID가 존재하지 않음

	}
	ret = np->id;  
	log("%-8s(%d, %d)\t\tret=%d\n", __func__, mY, mX, ret);
	return ret; // mID가 존재하지 않음
}


#elif 0

/*
  N : 구역의 개수 (2 ≤ N ≤ 26)
  M : 각 구역에 있는 슬롯의 개수 (2 ≤ M ≤ 1,000)
  L : 차량을 주차장에 주차할 수 있는 최대 기간 (500 ≤ L ≤ 100,000)
3. 각 테스트 케이스에서 enter() 함수의 호출 횟수는 70,000 이하이다.
4. 각 테스트 케이스에서 pullout() 함수의 호출 횟수는 40,000 이하이다.
5. 각 테스트 케이스에서 search() 함수의 호출 횟수는 50,000 이하이다.
6. 각 함수를 호출할 때 전달되는 mTime 값은 호출될 때마다 증가한다.
*/

const int AM = 26; // 알파벳 소문자 개수
const int NM = 100001; // 최대 전화번호 개수

struct RESULT_E
{
	int success;
	char locname[5];
};

struct RESULT_S
{
	int cnt;
	char carlist[5][8];
};
const int NM = 70000 + 5;
const int MM = 1000 + 5;
struct Node { 
	char carName[11];
	int expiredTime, lot, slot, parked, towed;
	Node() {}
	Node(char _carName[11], int _expiredTime, int _lot, int _slot, int _parked, int _towed) {
		strcpy(this->carName, _carName);
		this->expiredTime = _expiredTime;
		this->parked = _parked;
		this->towed = _towed;
		this->lot = _lot;
		this->slot = _slot;
	}
} node[NM]; int ndx;
struct cmp{
	bool operator()(const int & a, const int& b) const {
		return 
			node[a].parked != node[b].parked ?
			node[a].parked > node[b].parked :
			node[a].expiredTime != node[b].expiredTime ?
			node[a].expiredTime < node[b].expiredTime :
			strcmp(node[a].carName, node[b].carName) < 0;
	}
};
//int freeCnt[26]; 					// 빈공간 
priority_queue<pii, vector<pii>, greater<pii>> freeSpace;		// freeCnt, lot

int parking[26][MM]; 						// [lot][slot]    <->  id (car 정보) 
unordered_map<string, int> id2idx;	

unordered_map<string, set<int, cmp> > plate4;
int N, M, L;
void init(int N, int M, int L)
{
	::N = N, ::M = M, ::L = L;
	memset(parking, -1, sizeof(parking));
	ff(i, 0, N) {
		freeSpace.push({ M, i });
	}

	ndx = 0; id2idx.clear();
}
/*
bool parking(int idx){
	auto it = freeSpace.begin();
	if ( it != freeSpace.end() ) {
		auto [ freecnt, lot ] = *it;

		int slot = 0 ;
		ff(i, 0, M){
			if (  parked[lot][i] == -1 ) {
				parked[lot][i] = idx;
				slot = i;
			}
		}
		if ( slot == 0 )
			return 0;



		//
		return value;
	}
	return 0;
}
*/
RESULT_E enter(int mTime, char mCarNo[])
{
	RESULT_E res_e;
	res_e.success = -1;
	
	int slot, lot;
    if (!freeSpace.empty()) {  
		auto topElement = freeSpace.top(); freeSpace.pop();
        int freecnt = topElement.first;  
        lot = topElement.second;  


		ff(i, 0, M) {
			if ( parking[lot][i] == -1) { // 빈 공간 찾기
				slot = i;
				break;
			}	
		}

		node[ndx] = Node(mCarNo, mTime + L, lot, slot, 1, 0);
		parking[lot][slot] = ndx; // 주차 공간에 차량 정보 저장
		id2idx[mCarNo] = ndx; // 차량 번호와 인덱스 매핑
		res_e.success = 1; // 성공적으로 주차됨
		res_e.locname[0] = 'A' + lot; // 구역 이름 설정
		
		/*
		int i = 0;
		while (slot) {
			res_e.locname[i++] = slot % 10 + '0'; // 슬롯 번호 설정
			slot /= 10;
		}
		*/
		sprintf(res_e.locname, "%c%03d", 'A' + lot, slot); // 구역 이름과 슬롯 번호 설정 

		char str[11];
		strncpy(str, mCarNo + 3 , 4);
		str[4] = '\0'; // 차량 번호의 마지막 3자리만 사용
		plate4[str].insert(ndx); // 차량 번호와 인덱스 매핑

    } 	

	return res_e;

	/*
	// 나중에 pq로도 바꿔바라.
	int idx = ndx;
	int lot = -1, maxv = -1;
	ff(i, 0, 26) {
		if (maxv < freeCnt[i]) {
			maxv = freeCnt[i], lot = i;
		}
	}
	if (lot == -1)
		return 0;
	freeCnt[lot]--;

	int slot = -1;

	ff(i, 0, M) {
		if (parked[lot][j] == -1) {
			slot = i;
			parked[lot][j] == idx;
		}
	}

	// sprintf(res_e, "%d", slot);
	int i = 0;
	while (slot) {
		res_e[i++] = slot % 10 + '0';
		slot /= 10;
	}
	res_e[i++] = '\0';
	*/
}
int pullout(int mTime, char mCarNo[])
{
	int ret = -1;
	if( id2idx.count(mCarNo) == 0 ) {
		log("pullout: %s not found\n", mCarNo);
		return -1; // 차량이 존재하지 않음
	}
	else {

		int idx = id2idx[mCarNo];

		if ( node[idx].parked == 0) {
			log("pullout: %s already pulled out\n", mCarNo);
			return -1; // 차량이 이미 출차됨
		}
		else {
			Node* np = &node[idx];

			if (np->expiredTime < mTime) {
				// (주차된 기간 + 견인된 기간 * 5) * (-1)
				ret = L + ( mTime - np->expiredTime) * -1;
				np->towed = 1; // 차량이 견인됨
			}
			else {
				// 주차된 기간만큼 주차 요금 계산
				ret = mTime - (np->expiredTime - L);
			}			
		}

		log("%s(%d, %s)\t\tret=%d\n", mTime, mCarNo, ret);
		return ret; // 차량 출차 성공
	}
	return -1;
}
RESULT_S search(int mTime, char mStr[])
{
	RESULT_S res_s;
	res_s.cnt = -1;

	int cnt = 0;
	for (auto& i : plate4[mStr]) {

		int idx = i;
		Node* np = &node[idx];

		if (np->parked == 0) {
			continue; // 차량이 이미 출차됨
		}		

		strcpy(res_s.carlist[cnt++], np->carName); // 차량 번호 설정				

		if (res_s.cnt >= 5) {
			res_s.cnt = cnt;
			log("search: %s too many results\n", mStr);
			return res_s; // 최대 5개까지만 반환
		}		
	}

	return res_s;
}


#elif 0
#include<string>
#include<vector>
#include<iostream>
#include<unordered_map>
#define log(...) do { \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
} while (0)
using namespace std;

const int AM = 26; // 알파벳 소문자 개수
const int NM = 100001; // 최대 전화번호 개수

struct Node {
	Node* children[AM];
	bool isEndOfWord;
	Node() {
		isEndOfWord = false;
		for (int i = 0; i < AM; ++i) {
			children[i] = nullptr; // 자식 노드를 nullptr로 초기화
		}
	}
}node[NM]; int ndx = 0;

Node* getNode() {
	if (ndx < NM) {
		return &node[ndx++];
	}
	log("%10s,%d", __func__, __LINE__);
	return nullptr; // 노드가 부족한 경우
}

struct Trie {
	Node* root;
	Trie() {
		root = getNode();
		if (!root) {
			log("Trie 초기화 실패");
			exit(1);
		}
	}
	void init(){
		ndx = 0;
		root = getNode();
		if (!root) {
			log("Trie 초기화 실패");
			exit(1);
		}
	}
	void insert(char* word) {
		Node* current = root;
		for (int i = 0; word[i] != '\0'; ++i) {
			int index = word[i] - 'a'; // 문자 'a'를 0으로 변환
			if (current->children[index] == nullptr) {
				current->children[index] = getNode();
				if (!current->children[index]) {
					log("노드 할당 실패");
					exit(1);
				}
			}
			current = current->children[index];
		}
		current->isEndOfWord = true; // 단어의 끝 표시
	}
	void remove(char* word) {
		Node* current = root;
		for (int i = 0; word[i] != '\0'; ++i) {
			int index = word[i] - 'a';
			if (current->children[index] == nullptr) {
				return; // 단어가 존재하지 않음
			}
			current = current->children[index];
		}
		current->isEndOfWord = false; // 단어의 끝 표시 제거
	}

};





#elif 0
class TrieNode {
public:
	unordered_map<char, TrieNode*> children;
	bool isEndOfPhoneNumber;

	TrieNode() : isEndOfPhoneNumber(false) {}
};
class Trie {
private:
	TrieNode * root;
	void findAllPhoneNumbers(TrieNode *node, string prefix, vector<string> results) {

	}
	void deleteNode(TrieNode *node) {

	}
public:
	Trie() {
		root = new TrieNode();
	}
	~Trie() {
		deleteNode(root); 
	}
	void insert(const string& phonenumber) {		
		TrieNode *current = root;
		for (char c : phonenumber) {
			if (current->children.find(c) == current->children.end()) {
				current->children[c] = new TrieNode();
			}
			current = current->children[c];
		}
		current->isEndOfPhoneNumber = true;
	}
	vector<string> seachByPrefix(const string& prefix) {
		TrieNode *current = root;
		for (char c : prefix) {
			if (current->children.find(c) == current->children.end()) {
				return {};
			}
			current = current->children[c];
		}
		vector<string> results;
		findAllPhoneNumbers(current, prefix, results);
		return results;
	}
};
vector<string> foo() {
	return {};	
}
int main() {
	Trie phoneBook;
	phoneBook.insert("1234567890");
	phoneBook.insert("1234561234");
	phoneBook.insert("9876543210");
	phoneBook.insert("1231231234");
	string prefix = "123";
	vector<string> a;
	string b = "evan";

	char cc = '1';
	cout << b + cc << endl;

	printf("%s%c\n", b.c_str(), cc );
	exit(1);

	a.push_back(prefix);
	a.push_back(prefix);
	a.push_back(prefix);
	a.push_back(prefix);

	char ch = 't';

	a.push_back(prefix + ch);

	cout << prefix + ch << endl;

	for (auto &it : a) {
		cout << it << endl;
	}

	exit(1);
	vector<string> results = phoneBook.seachByPrefix(prefix); 

	for (auto &number : results) {
		cout << number << endl;
	}
	return 0;
}

#elif 0

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

class TrieNode {
public:
	unordered_map<char, TrieNode*> children;
	bool isEndOfPhoneNumber;

	TrieNode() : isEndOfPhoneNumber(false) {}
};

class Trie {
private:
	TrieNode* root;

	void findAllPhoneNumbers(TrieNode* node, string prefix, vector<string>& results) {
		if (node->isEndOfPhoneNumber) {
			results.push_back(prefix);
		}
		for (const auto& pair : node->children) {
			findAllPhoneNumbers(pair.second, prefix + pair.first, results);
		}
	}

	void deleteNode(TrieNode* node) {
		for (auto& pair : node->children) {
			deleteNode(pair.second);
		}
		delete node;
	}

public:
	Trie() {
		root = new TrieNode();
	}

	~Trie() {
		deleteNode(root);
	}

	void insert(const string& phoneNumber) {
		TrieNode* current = root;
		for (char c : phoneNumber) {
			if (current->children.find(c) == current->children.end()) {
				current->children[c] = new TrieNode();
			}
			current = current->children[c];
		}
		current->isEndOfPhoneNumber = true;
	}

	vector<string> searchByPrefix(const string& prefix) {
		TrieNode* current = root;
		for (char c : prefix) {
			if (current->children.find(c) == current->children.end()) {
				return {}; // Prefix not found
			}
			current = current->children[c];
		}
		vector<string> results;
		findAllPhoneNumbers(current, prefix, results);
		return results;
	}
};

int main() {
	Trie phoneBook;

	// 전화번호 추가
	phoneBook.insert("1234567890");
	phoneBook.insert("1234561234");
	phoneBook.insert("9876543210");
	phoneBook.insert("1231231234");

	// 접두사 검색
	string prefix = "123";
	vector<string> results = phoneBook.searchByPrefix(prefix);

	cout << "전화번호 '" << prefix << "'로 시작하는 번호들:\n";
	for (const string& number : results) {
		cout << number << "\n";
	}

	return 0;
}


#elif 0
#include<iostream>
using namespace std;
#define SM		(26)
#define LOG(...) do { \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
} while (0)
    

struct Trie {
	bool is_end;
	Trie *ch[SM];
	Trie(): is_end(false){
		//this->is_end = false;
		for (int i = 0; i < SM; ++i) {
			ch[i] = nullptr;
		}
	}
	~Trie() {
		for (int i = 0; i < SM; ++i) {
			if (ch[i])
				delete ch[i];
		}
	}
	void insert(const char *s) {
		if (!*s) {
			this->is_end = true;
			return;
		}
		int c = *s - 'a';
		if ( !ch[c] ){
			ch[c] = new Trie;
			// 다음 스트링처리를 위해 insert()를 재호출한다
			ch[c]->insert(s + 1);
		}
	}
	bool search(const char *s) {
		// string의 끝(NULL)이면 찾은 것임.
		if (!*s) {
			if (this->is_end)
				return true;
			else
				return false;
		}		
		int c = *s - 'a';
		if (!ch[c]) 
			return false;
		else 
			ch[c]->search(s + 1);		
	}
};

Trie *r = new Trie;
int main() {
	
	
	

	r->insert("abcdef");

	int ret;
	ret = r->search("abcdef"); 
	LOG("%d", ret);
	ret = r->search("abcde"); 
	LOG("%d", ret);
	ret = r->search("abcdefg"); 
	LOG("%d", ret);
	ret = r->search("abcdef "); 
	LOG("%d", ret);
	ret = r->search("abcdefghi"); 
	LOG("%d", ret);
	ret = r->search("abcdef");
	LOG("%d", ret);
	delete r;
	

	return 0;
}


#elif 0

물론, C와 C++로 각각 트라이(Trie) 자료구조를 구현해보겠습니다!트라이는 문자열을 효율적으로 저장하고 탐색하는 자료구조입니다.먼저 C 언어로 구현한 예제를 보여드리겠습니다.

### C 언어로 트라이 구현하기

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_SIZE 26

struct TrieNode {
	int isLeaf;
	struct TrieNode* children[CHAR_SIZE];
};

struct TrieNode* createNode() {
	struct TrieNode* newNode = (struct TrieNode*)malloc(sizeof(struct TrieNode));
	newNode->isLeaf = 0;
	for (int i = 0; i < CHAR_SIZE; ++i) {
		newNode->children[i] = NULL;
	}
	return newNode;
}

void insert(struct TrieNode* root, const char* key) {
	struct TrieNode* curr = root;
	while (*key) {
		int index = *key - 'a';
		if (!curr->children[index]) {
			curr->children[index] = createNode();
		}
		curr = curr->children[index];
		++key;
	}
	curr->isLeaf = 1;
}

int search(struct TrieNode* root, const char* key) {
	struct TrieNode* curr = root;
	while (*key) {
		int index = *key - 'a';
		if (!curr->children[index]) {
			return 0; // Not found
		}
		curr = curr->children[index];
		++key;
	}
	return curr->isLeaf;
}

int main() {
	struct TrieNode* root = createNode();

	insert(root, "apple");
	insert(root, "app");

	printf("Search 'apple': %d\n", search(root, "apple")); // 1 (found)
	printf("Search 'app': %d\n", search(root, "app"));     // 1 (found)
	printf("Search 'apples': %d\n", search(root, "apples")); // 0 (not found)

	// Clean up (free memory)
	// ...

	return 0;
}
```

이 코드는 C 언어로 트라이를 구현한 간단한 예제입니다.이제 C++ 언어로도 구현해보겠습니다.

### C++ 언어로 트라이 구현하기

```cpp
#include <iostream>
#include <unordered_map>
using namespace std;

class TrieNode {
public:
	bool isLeaf;
	unordered_map<char, TrieNode*> children;

	TrieNode() : isLeaf(false) {}
};

class Trie {
private:
	TrieNode* root;

public:
	Trie() {
		root = new TrieNode();
	}

	void insert(const string& word) {
		TrieNode* curr = root;
		for (char c : word) {
			if (!curr->children.count(c)) {
				curr->children[c] = new TrieNode();
			}
			curr = curr->children[c];
		}
		curr->isLeaf = true;
	}

	bool search(const string& word) {
		TrieNode* curr = root;
		for (char c : word) {
			if (!curr->children.count(c)) {
				return false; // Not found
			}
			curr = curr->children[c];
		}
		return curr->isLeaf;
	}
};

int main() {
	Trie trie;
	trie.insert("apple");
	trie.insert("app");

	cout << "Search 'apple': " << trie.search("apple") << endl; // 1 (found)
	cout << "Search 'app': " << trie.search("app") << endl;     // 1 (found)
	cout << "Search 'apples': " << trie.search("apples") << endl; // 0 (not found)

	// Clean up (delete nodes)
	// ...

	return 0;
}
```

C++ 버전은 클래스를 사용하여 트라이를 구현했습니다. `unordered_map`을 활용하여 자식 노드를 관리하고 있습니다.이 코드도 간단한 예제로, 실전에서는 메모리 관리와 더 많은 기능을 추가해야 합니다.

두 언어로 구현한 트라이는 기본적인 동작은 동일하며, C++ 버전은 더 객체지향적인 구

원본 : Copilot과의 대화, 2024. 7. 20.



	(5) 트라이(Trie) 자료구조 - Crocus.https ://www.crocus.co.kr/1053.
	(6) C언어와 C++ 차이점 비교(객체지향과 하향식) - 코딩 기록.https ://codingcoding.tistory.com/287.
	(7)[c++] 트라이 구조체와 알고리즘.https ://bing.com/search?q=C%2b%2b+%ec%96%b8%ec%96%b4+%ed%8a%b8%eb%9d%bc%ec%9d%b4+%ec%95%8c%ea%b3%a0%eb%a6%ac%ec%a6%98+%ec%98%88%ec%a0%9c.
	(8)[자료구조] 트라이(Trie) 자료구조 - Rebro의 코딩 일기장.https ://rebro.kr/86.
	(9) undefined.https ://bing.com/search?q=.
	(10) github.com.https ://github.com/Sahil12S/LeetCode/tree/5a9916827e969cd4adbc394779e1bb3ba576f6e2/Cplusplus%2FreplaceWords.cpp.
#elif 0
#include<iostream>
#include<cstring>

using namespace std;

void foo(char *str) {


	char dest[10];
	//strcpy(dest, ptr);
	strcpy(dest, str);

	printf("%s\n", dest);

}

int main() {
	char a[] = "abcd";

	char *ptr = a;

	//foo(ptr);
	foo(a);

	return 0;
}

#elif 0
#include<vector>
#include<set>
using namespace std;
#define NM 10'001

struct Schedule { int s, e; };
struct C {
	int id, rc, rp;
	vector<Schedule> schedule;
}c[NM];

struct cmp {
	bool operator()(C *lhs, C *rhs) {
		return lhs->rp != rhs->rp ? lhs->id < rhs->id : lhs->rp < rhs->rp;
	}
};
set<C*, cmp> cars[11][12][5][5];
vector<C*> company[NM];
void init(int N) {

}

void add(int mCarID, int mCompanyID, int mCarInfo[]) {
	C *node = &c[mCarID - 1];
	*node = { mCarID, mCarInfo[4], mCarInfo[5] };
	node->schedule.clear();	

	cars[mCarInfo[0]][mCarInfo[1]][mCarInfo[2]][mCarInfo[3]].insert( node );
	company[mCompanyID].push_back( node );
}

int rent(int mCondition[]) {
	int s = mCondition[0]; 
	int e = mCondition[1]; 
	// 값 추출

	/*
	set<C*,  cmp> node2 = cars[mCondition[2]][mCondition[3]][mCondition[4]][mCondition[5]];
	//auto node2 = cars[mCondition[2]][mCondition[3]][mCondition[4]][mCondition[5]];

	for (auto it = node2.begin(); it != node2.end(); it++) {
		printf("%d \n", (*it));
	}
	*/
	


	// 원하는 조건 차량 순환
	for (auto node : cars[mCondition[2]][mCondition[3]][mCondition[4]][mCondition[5]]) {
		printf("[%d] \n", node->id);

		// 예약 스케쥴 확인
		int flag = 0;
		
		for (auto v : node->schedule) {			
			// 
			int scheduled_start = v.s;
			int scheduled_end = v.e;
			if (node->rc)
				node->rc = node->rc - 1;
			else
				break;
#if 0
			if (e <= scheduled_start || scheduled_end <= s) {
				printf("s=%d, e=%d\n", v.s, v.e);
				continue;
			}
			else {
				flag = 1;
			}
#else
			if (e > scheduled_start && scheduled_end > s) {
				flag = 1;  
			}
#endif

			printf("s=%d, e=%d\n", v.s, v.e);
		}

		if (flag == 0) {
			node->schedule.push_back({ s, e });
		}

	}
	printf("\n");



	
	// 조건 반복
	{
		//렌트불가? 
		{
			//continue;
		}

		//렌트가능?
		{

		}
	}
	return 0;
}

int promote(int mCompanyID, int mDiscount) {
	return 0;
}


#elif 0
#include<iostream>
using namespace std;





int main() {


	int N;
	char s[2000];
	cin >> N;

	char id[200], time[200], loc[9][200], people[9][200];
	int k = 0, c = 0;

	for (int i = 0; i < N; i++) {

		cin >> s;
		char *m = s;		
		
		while (*m++ != '[');
		while (*m != ']'){
			id[k++] = *m++;
		} id[k++] = '\0';
		k = 0; c = 0;
		cout << id << " ";
		

		while (*m++ != '[');
		while (*m != ']') {
			time[k++] = *m++;
		}time[k++] = '\0';
		k = 0; c = 0;
		cout << time << " ";


		while (*m++ != '[');
		while (*m != ']') {
			if (*m == ',') {
				loc[c][k++] = '\0';
				c++; k = 0; *m++;
			}
			loc[c][k++] = *m++;
		}loc[c][k++] = '\0';		

		for (int i = 0; i <= c; i++) {
			cout << loc[i] << " " ;
		}

		k = 0; c = 0;
		



		while (*m++ != '[');
		while (*m != ']') {
			if (*m == ',') {
				people[c][k++] = '\0';
				c++; k = 0; *m++;
			}
			people[c][k++] = *m++;
		}people[c][k++] = '\0';

		for (int i = 0; i <= c; i++) {
			cout << people[i] << " ";
		}

		k = 0; c = 0;
		cout << endl;


	}

	return 0;
}
#elif 0
#include<vector>
#include<deque>
#include<iostream>
#include<algorithm>
#include <chrono>
#include<queue>	
using namespace std;
#define NM 5'001
#define TM 11'000
#define rint register int
int m[NM][NM], cmd, N;
/*
3. 각 테스트 케이스에서 색깔 하나의 타워는 최대 11, 000 개 이다.
4. 각 테스트 케이스에서 buildTower() 함수의 호출 횟수는 최대 50, 000 이다.
5. 각 테스트 케이스에서 removeTower() 함수의 호출 횟수는 최대 1, 000 이다.
6. 각 테스트 케이스에서 countTower() 함수의 호출 횟수는 최대 10, 000 이다.
7. 각 테스트 케이스에서 getClosest() 함수의 호출 횟수는 최대 5, 000 이다.
*/
struct P {
	int r, c;
};
queue<P> q;
void init(int N)
{
	::N = N;
	memset(m, 0, sizeof(m));
}

void buildTower(int mRow, int mCol, int mColor)
{
	m[mRow][mCol] = mColor;
}
void removeTower(int mRow, int mCol)
{
	m[mRow][mCol] = -1;
}
bool iVal(int r, int c) {
	if (r < 0 || r > N || c < 0 || c > N)
		return 0;
	else
		return 1;
}

int dr[] = { -1, 0, 1, 0 };int dc[] = { 0, 1, 0, -1 };
int v[NM][NM], tc;
int countTower(int mRow, int mCol, int mColor, int mDis)
{
	int ret = -1;
	int sum = 0;
	int l = -1;
	q = {};
	q.push({ mRow, mCol });
	v[mRow][mCol] = ++tc;

	while (!q.empty()) {
		int cr = q.front().r;
		int cc = q.front().c;
		q.pop();		

		if (cr < mRow - mDis || cr > mRow + mDis || cc < mCol - mDis || cc > mCol + mDis)
			continue;

		if (!iVal(cr, cc))
			continue;

		if (m[cr][cc]) {
			if (mColor == 0 || m[cr][cc] == mColor) {
				if (cmd == 1) {
					return l = abs(cr - mRow) + abs(cc - mCol);
				}
				sum++;
			}
		}

		for (rint i = 0; i < 4; i++) {
			int nr = cr + dr[i];
			int nc = cc + dc[i];
			if (!iVal(nr, nc)) continue;
			if (v[nr][nc] != tc) {
				v[nr][nc] = tc;
				q.push({ nr, nc });
			}
		}
	}	

	if (cmd == 0)
		ret = sum == 0 ? 0 : sum;
	else
		ret = l;

	printf("(%s) %d \n", __FUNCTION__, ret);
	return ret;
}
int getClosest(int mRow, int mCol, int mColor)
{
	int ret = -1;
	cmd = 1;
	ret = countTower(mRow, mCol, mColor, NM*3);
	cmd = 0;

	printf("(%s) %d \n", __FUNCTION__, ret);
	
	if (ret == 0)
		return -1;
	else
		return ret; 
}

#elif 0
#include<vector>
#include<deque>
#include<iostream>
#include<algorithm>
using namespace std;
struct H {
	int r, c, v, l;
	bool operator<(const H &ref)const {

		return l != ref.l ? l < ref.l : 
			   r != ref.r ? r < ref.r : 
			                c < ref.c;
	}

} h[6];
struct P {
	int f, s;
	bool operator<(const P &ref)const {
		return f < ref.f;
	}
}p[3];
int main() {

	h[0] = { 2,2,3,4 };
	h[1] = { 1,8,3,7 };
	h[2] = { 1,3,3,4 };
	h[3] = { 1,3,3,6 };
	h[4] = { 3,2,4,4 };
	h[5] = { 1,2,4,7 };


	sort(h, h + 6);

	for (auto &it : h) {
		cout << it.c << " " << it.r<< " " << it.v << " " << it.l << " " << endl;
	}

	return 0;


	if (h[0] < h[1]) {
		cout << h[1].r << " " << h[1].c << " " << h[1].v << " " << h[1].l << " " << endl;
	}
	else {
		cout << h[1].r << " " << h[1].c << " " << h[1].v << " " << h[1].l << " " << endl;
	}

	return 0;

	p[0] = { 1,2 };
	p[1] = { 2,4 };

	if (p[0] < p[1]) {
		cout << p[1].f << endl;
	}
	




	return 0;
}


#elif 0
#include<vector>
#include<deque>
using namespace std;
#define rint register int
#define NM 3'001
#define HM 30'001
#define INF 987654321
//mK : 구슬 치기의 반복 횟수(1 ≤ mK ≤ 20)
//일직선으로 연결된 길 내에는 20개 이하의 홀이 존재함을 보장한다.
//3. 각 테스트 케이스에서 addDiagonal() 함수의 호출 횟수는 최대 3, 000 이다.
//4. 각 테스트 케이스에서 addHole() 함수의 호출 횟수는 최대 30, 000 이다.
//5. 각 테스트 케이스에서 eraseHole() 함수의 호출 횟수는 최대 3, 000 이다.
//6. 각 테스트 케이스에서 hitMarble() 함수의 호출 횟수는 최대 10, 000 이다.
vector<int> R[NM];
vector<int> C[NM];
vector<int> U[NM * 2];
vector<int> D[NM * 2];
struct Y { int sr, sc, er, ec; } UL[NM * 2], DL[NM * 2];

struct Q {	int id, v ; } m[NM][NM]; 
struct H {	int r, c, v, l; 
	bool operator<(const H &ref)const {		
		return l != ref.l ? l < ref.l : 
			   r != ref.r ? r < ref.r : 
			                c < ref.c;
	}
} h[HM]; 

int N, tc;
void init(int N)
{
	::N = N;
	for (rint i = 1; i <= N; i++) {
		R[i].clear();
		C[i].clear();
	}
	for (rint i = 1; i <= 2*N; i++) {
		U[i].clear();
		D[i].clear();
	}
	memset(m, 0, sizeof(m));
	memset(h, 0, sizeof(h));
}
void addDiagonal(int mARow, int mACol, int mBRow, int mBCol)
{
	if (mACol > mBCol) {
		swap(mARow, mBRow);
		swap(mACol, mBCol);
	}
	if (mARow < mBRow) {
		UL[mARow + mACol] = { mARow, mACol, mBRow, mBCol };
	}
	else {
		DL[mARow - mACol + N] = { mARow, mACol, mBRow, mBCol };
	}


	/*
	if (mARow > mBRow) {
		swap(mARow, mBRow);
		swap(mACol, mBCol);
	}
	if (mACol < mBCol) {
		for (auto &it : U[mARow + mACol]) {
			if (h[it].r < mARow || mBRow < h[it].r) {
				it = -1;
			}
		}
	}
	else {
		for (auto &it : D[mARow + mACol]) {
			if (h[it].r < mARow || mBRow < h[it].r) {
				it = -1;
			}
		}
	}
	*/
}
void addHole(int mRow, int mCol, int mID)
{
	R[mRow].push_back(mID);
	C[mCol].push_back(mID);
	U[mRow + mCol].push_back({ mID });
	D[mRow - mCol + N].push_back({ mID });
	h[mID] = { mRow, mCol, true, 0 };
	m[mRow][mCol] = { mID, true };
}
void eraseHole(int mRow, int mCol)
{
	h[m[mRow][mCol].id].v = false;
	m[mRow][mCol] = { false, false };
}
bool isVal(int r, int c, int idx) {	
	int sr = UL[idx].sr;
	int sc = UL[idx].sc;
	int er = UL[idx].er;
	int ec = UL[idx].ec;

	if (sc > ec) {
		swap(sr, er);
		swap(sc, ec);
	}

	// U
	if (sr > er) {
		if (r > sr || r < er || c < sc || c > ec)
			return -1;
	}
	else {
		if (r < sr || r > er || c < sc || c > ec)
			return -1;
	}
	return 1;
}
int hitMarble(int mRow, int mCol, int mK)
{
	int ret = -1;
	
	while (mK--) {
		H mv = { INF, INF, INF, INF };
		for (auto & it : R[mRow]) {
			if (it == -1) continue;
			if (h[it].v == -1) continue;
			h[it].l = abs(h[it].r - mRow) + abs(h[it].c - mCol);			
			if ( h[it] < mv) {
				mv = h[it];
			}
		}
		for (auto & it : C[mCol]) {
			if (it == -1) continue;
			if (h[it].v == -1) continue;
			h[it].l = abs(h[it].r - mRow) + abs(h[it].c - mCol);
			if (h[it] < mv) {
				mv = h[it];
			}
		}
		for (auto & it : U[mRow + mCol]) {
			if (it == -1) continue;
			if (h[it].v == -1) continue;

			if (!isVal(h[it].r, h[it].c, mRow + mCol)) continue;

			h[it].l = abs(h[it].r - mRow) + abs(h[it].c - mCol);
			if (h[it] < mv) {
				mv = h[it];
			}			
		}
		for (auto & it : D[mRow - mCol + N]) {
			if (it == -1) continue;
			if (h[it].v == -1) continue;
			h[it].l = abs(h[it].r - mRow) + abs(h[it].c - mCol);
			if (h[it] < mv) {
				mv = h[it];
			}
		}
		if (mv.l == INF) {
			break;
		}
		else {
			ret = m[mv.r][mv.c].id;
			h[ret].v = -1;
			mRow = mv.r;
			mCol = mv.c;
			printf("%d ", ret);
		}			
	}
	
	printf("<===%d \n", ret);
	return ret;
}

#elif 0
/*
* @file: [H2407] [Pro] 상징물 설치
* @brief: 모범 답안
* @copyright: All rights reserved (c) 2024 Samsung Electronics, Inc.
*/

#include <unordered_map>
#include <vector>
#include <array>
#include <algorithm>
using namespace std;
const int INF = 1'000'000'000;
int num_beam;
int beams[201];

unordered_multimap<int, int> sols;

void init()
{
	num_beam = 1;    // 1부터 시작한다. 0인 경우 막대가 없는 경우이다.
	beams[0] = 0;

	sols.clear();
}


void addBeam(int mLength)
{
	for (int i = 0; i < num_beam; ++i)
		sols.insert({ mLength + beams[i], i * 1000 + num_beam });
	beams[num_beam++] = mLength;


}


int requireSingle(int mHeight)
{
	int ret = INF;
	for (int i = 0; i < num_beam; ++i)
	{
		auto range = sols.equal_range(mHeight - beams[i]);
		for (auto it = range.first; it != range.second; ++it)
		{
			int b1 = it->second % 1000;
			int b2 = it->second / 1000;

			if (i > 0 && (i == b1 || i == b2)) // 중복되면 안된다.
				continue;


			ret = min(ret, max(beams[i], max(beams[b1], beams[b2])));
		}
	}


	return ret != INF ? ret : -1;
}


int get_max(array<int, 3> &arr)
{
	int ret = 0;
	for (auto idx : arr)
		ret = max(ret, beams[idx]);
	return ret;
}


vector<array<int, 3>> ways;


int requireTwin(int mHeight)
{
	ways.clear();


	for (int i = 0; i < num_beam; ++i)
	{
		auto range = sols.equal_range(mHeight - beams[i]);
		for (auto it = range.first; it != range.second; ++it)
		{
			int b1 = it->second % 1000;
			int b2 = it->second / 1000;

			if (i > 0 && (i == b1 || i == b2)) // 중복되면 안된다.
				continue;


			ways.push_back({ b1, b2, i });
		}
	}


	int n = ways.size();

	int ret = INF;
	for (register int i = 0; i < n; ++i)
		for (register int j = i + 1; j < n; ++j)
		{
			bool okay = true;
			for (register int k = 0; okay && k < 3; ++k)
				for (register int l = 0; okay && l < 3; ++l)
					if (ways[i][k] > 0 && ways[i][k] == ways[j][l]) // 두 경우에 중복이 있는지 확인한다.
						okay = false;
			if (okay)
				ret = min(ret, max(get_max(ways[i]), get_max(ways[j])));
		}


	return ret != INF ? ret : -1;
}

#elif 0
#include <iostream>
#include <unordered_map>
#include <string>
using namespace std;
int main() {
	// unordered_multimap 생성
	std::unordered_multimap<int, std::string> umultimap;

	// 요소 추가
	umultimap.insert(std::make_pair(1, "apple"));
	umultimap.insert(std::make_pair(2, "banana"));
	umultimap.insert(std::make_pair(1, "orange"));  // 동일한 키를 가짐
	umultimap.insert(std::make_pair(3, "grape"));
	umultimap.insert(std::make_pair(1, "kiwi"));    // 동일한 키를 가짐
	
	// 요소 접근
	//std::cout << "umultimap contains:\n";
	//for (const auto& pair : umultimap) {
	//	std::cout << pair.first << ": " << pair.second << std::endl;
	//}

	auto range = umultimap.equal_range(1);
	//int n = umultimap.count(1);

	for (auto it = range.first; it != range.second; ++it){
		cout << it->first << " " << it->second << endl;
	}
	return 0;
}

#elif 0
#define rint register int
#define NM 200
using namespace std;
int a[NM], cnt;

	

void init()
{
}
void addBeam(int mLength)
{
}
int requireSingle(int mHeight) {
	int ret = -1;
	return ret;
}
int requireTwin(int mHeight)
{
	int ret = -1;
	return ret;
}

#elif 0

#include<iostream>
#define rint register int
using namespace std;

int a[] = { 1,2,3,4,5,6,7 };
int o[100], v[100], t[100][4];
int n = 3, r = 2;

int idx;
void perm(int d) {

	if (d == r) {
		for (rint i = 0; i < r; ++i) {
			t[idx][i] = o[i];
		}
		idx++;
	}
	else {
		for (int i = 0; i < n; ++i) {
			if (!v[i]) {
				o[d] = a[i];

				v[i] = 1;
				perm(d + 1);
				v[i] = 0;
			}
		}
	}
}
void combi(int d, int s) {

	if (d == r) {
		for (rint i = 0; i < r; ++i) {
			t[idx][i] = o[i];
		}
		idx++;
	}
	else {
		for (int i = s; i < n; ++i) {
			if (!v[i]) {
				o[d] = a[i];

				v[i] = 1;
				combi(d + 1, s + 1);
				v[i] = 0;
			}
		}
	}
}

int main() {

	//perm(0);
	combi(0, 0);
	for (rint i = 0; i < idx; ++i) {
		for (rint j = 0; j < r; ++j) {
			printf("%d ", t[i][j]);
		}printf("\n");
	}
}
#elif 0
#include<set>
#include<algorithm>
#include<vector>
#define rint register int
#define NM 200
using namespace std;
//set<int,  greater<int>> s;
int a[NM], cnt;
void init()
{
	cnt = 0;
	//s.clear();
}

void addBeam(int mLength)
{
	//s.insert(mLength);
	a[cnt++] = mLength;
}
int o[10], odx;
int requireSingle(int mHeight) {
	for (rint i = 0; i < 6; ++i) o[i] = 0; odx = 0;
	sort(a, a + cnt, greater<int>());
	
	int l = 0, ml = 0;
	for (rint i = 0; i < cnt; ++i) {

		if (a[i] == mHeight ){
			o[odx++] = a[i];
			goto exit;
		}
		else if (a[i] > mHeight) {
			continue;
		}

		ml = a[i], l = a[i];
		for (rint j = i + 1; j < cnt; ++j) {

			if (l + a[j] == mHeight) {
				o[odx++] = a[j];
				goto exit;
			}
			else if (l + a[j] > mHeight) {
				continue;
			}

			for (rint k = j + 1; k < cnt; ++k) {

				if (l + a[k] == mHeight) {
					o[odx++] = a[k];
					goto exit;
				}
				else if (l + a[k] > mHeight) {
					continue;
				}
				else if (l + a[k] < mHeight) {
					goto exit;
				}
			}
		}

	}

	
	for (rint i = 0; i < cnt; ++i) {
		printf("%d \n", a[i]);
	}
	printf("\n");

	exit: 
	return ml; 
}
int requireTwin(int mHeight)
{
	for (rint i = 0; i < cnt; ++i) {
		printf("%d \n", a[i]);
	}
	printf("\n");
	return -2;
}


#elif 0
#include<vector>
#include<queue>
using namespace std;
#define rint register int
#define NM 505
#define INF 987654321
int N, K, tdx, o[100], v[100], tc, d[NM], t[5][130][5], START, END;
int a[5] = { 0,1,2,3,4 };
struct P {
	int s, l; 
	bool operator<(const P &r) const {
		return l > r.l;
	}
}; 
priority_queue<P> pq;
vector<P> adj[NM];
void addRoad(int mRoadA, int mRoadB, int mLen)
{
	adj[mRoadA].push_back({ mRoadB, mLen });
	adj[mRoadB].push_back({ mRoadA, mLen });
}
void perm_table(int n, int r, int d) {

	if (d == r) {
		for (rint i = 0; i < r; ++i) {
			t[n-1][tdx][i] = o[i];
			//printf("%d ", o[i]);
		}
		//printf("\n");
		tdx++;
	}
	else {
		for (rint i = 0; i < n; ++i) {
			if (v[i] != tc) {
				o[d] = a[i];
				v[i] = tc;
				perm_table(n, r, d + 1);
				v[i] = 0;
			}
		}
	}
}
void print_table() {
	for (rint i = 0; i < 5; ++i) {
		for (rint j = 0; j < 120; ++j) {
			for (rint k = 0; k < 5; ++k) {
				printf("%d ", t[i][j][k]);
			}printf("\n"); 
		}printf("\n");
	}
}
void init(int N, int K, int mRoadAs[], int mRoadBs[], int mLens[])
{
	::N = N;
	for (rint i = 0; i <= N; ++i) {
		adj[i].clear();
	}
	for (rint i = 0; i < K; ++i) {
		addRoad(mRoadAs[i], mRoadBs[i], mLens[i]);
	}
	for (rint i = 1; i <= 5; ++i) {
		tdx = 0; tc++;
		perm_table(i, i, 0);
	}
	//print_table();
}

int dijkstra(int s, int e) {
	int ret = -1;
	for (rint i = 0; i <= N; ++i) { d[i] = INF; }
	pq.push({ s, 0 });
	d[s] = 0;
	while (!pq.empty()) {
		auto curr = pq.top(); pq.pop();		

		//if (curr.s == e) {
		//	return curr.l;  // <=========================================================
		//}

		if (d[curr.s] < curr.l) 
			continue;

		if (s != START) {
			if (curr.s == START || curr.s == END) {
				continue;
			}
		}

		for (auto &next : adj[curr.s]) {
			if (d[next.s] > d[curr.s] + next.l) {
				d[next.s] = d[curr.s] + next.l;
				pq.push({ next.s, d[next.s] });
			}
		}
	}
	ret = d[e];// >= INF ? -1 : d[e];
	return ret;
}
int p[] = { 0, 1, 2, 6, 24, 120 };
int findPath(int mStart, int mEnd, int M, int mStops[])
{	

	int m[7];
	START = m[0] = mStart;
	END = m[M + 1] = mEnd;

	int ret = INF;
	for (rint i = 0; i < p[M]; i++) {
		
		for (rint j = 0; j < M; j++) {
			m[j + 1] = mStops[t[M - 1][i][j]];
		}
		
		int sum = 0;
		for (rint k = 0; k < M + 1; k++) {
			int t = dijkstra(m[k], m[k + 1]);

			sum += t;
		}
		ret = min(sum, ret);
	}	
	//printf("%d \n", ret);
	ret = ret >= INF ? -1 : ret;
	return ret;
}

#elif 0
#include<iostream>
#define rint register int
using namespace std;

int a[] = { 1,2,3,4,5,6,7 };
int o[100], v[100], t[100][4];
int n=3, r=3;

int idx;
void perm(int d) {

	if (d == r) {
		for (rint i = 0; i < r; ++i) {
			t[idx][i] = o[i];
		}
		idx++;
	}
	else {
		for (int i = 0; i < n; ++i) {
			if (!v[i]) {
				o[d] = a[i];

				v[i] = 1;
				perm(d + 1);
				v[i] = 0;
			}
		}
	}
}


int main() {

	perm(0);
	for (rint i = 0; i < idx; ++i) {
		for (rint j = 0; j < r; ++j) {
			printf("%d ", t[i][j]);
		}printf("\n");
	}
}

#elif 0
#include<iostream>
#define rint register int
using namespace std;

int a[] = { 1,2,3,4,5,6,7 };
int o[100], v[100]; 
void perm(int n, int r, int d) {

	if (d == r) {
		for (rint i = 0; i < r; i++) {
			printf("%d ", o[i]);
		}printf("\n");
	}
	else {
		for (int i = 0; i < n; ++i) {			
			if (!v[i]) {
				o[d] = a[i];

				v[i] = 1;
				perm(n, r, d + 1);
				v[i] = 0;
			}
		}
	}
}


int main() {

	perm(3, 3, 0);

}

#elif 0
#include<queue>
using namespace std;
#define MAP_SIZE_MAX	350
#define NM	200
#define rint register int
#define INF 987654321
int visited[MAP_SIZE_MAX][MAP_SIZE_MAX], (*m)[MAP_SIZE_MAX], S, N, tc, rd, wr; 
int d[NM];
struct Q { int r, c, s, l; } q[350 * 350];
struct P { int e, l; 
	bool operator<(const P &ref) const{
		return l > ref.l;
	}
};
struct G { int r, c, valid; } g[NM];
vector<P> adj[NM];
priority_queue<P> pq;
void init(int N, int mMaxStamina, int mMap[MAP_SIZE_MAX][MAP_SIZE_MAX]){
	::N = N, ::S = mMaxStamina;
	memset(g, 0, sizeof(g));	
	for (rint i = 0; i < NM; i++)
		adj[i].clear();

	m = mMap;
}
int dr[] = { -1, 0, 1, 0 };int dc[] = { 0, -1, 0, 1 };
bool rcheck(int r, int c) {
	if (r >= N || r < 0 || c >= N || c < 0 || m[r][c] ==1 ) 
		return false;
	else 
		return true;
}
void addGate(int mGateID, int mRow, int mCol){
	g[mGateID] = { mRow, mCol, 1 };	
	rd = wr = 0;
	m[mRow][mCol] = -mGateID;
	visited[mRow][mCol] = ++tc;
	q[wr++] = { mRow, mCol, S, 0 };
	while (wr != rd) {
		auto t = q[rd++];
		if (m[t.r][t.c] < 0 && m[t.r][t.c] != -mGateID) {
			adj[mGateID].push_back({ -m[t.r][t.c] , t.l });
			adj[-m[t.r][t.c]].push_back({ mGateID  , t.l });
		}
		if (t.s == 0) continue;
		for (rint i = 0; i < 4; ++i) {
			int nr = t.r + dr[i];
			int nc = t.c + dc[i];
			if (rcheck(nr, nc) && visited[nr][nc] != tc) {
				visited[nr][nc] = tc;
				q[wr++] = { nr, nc, t.s - 1, t.l + 1 };
			}
		}
	}

}
void removeGate(int mGateID){
	g[mGateID].valid = 0;
	m[g[mGateID].r][g[mGateID].c] = 0;
	return;
}
int getMinTime(int mStartGateID, int mEndGateID){

	int ret = -1;

	for (rint i = 0; i < NM; ++i) {
		d[i] = INF;
	}
	pq = {};
	pq.push({ mStartGateID, 0 });
	d[mStartGateID] = 0;
	while (!pq.empty()) {
		auto curr = pq.top(); pq.pop();

		if (d[curr.e] < curr.l) {
			continue;
		}		

		if (curr.e == mEndGateID) {
			return curr.l;
		}
		
		for (auto &next : adj[curr.e]) {
			if (!g[curr.e].valid)
				continue;

			if (d[next.e] > d[curr.e] + next.l){
				d[next.e] = d[curr.e] + next.l;
				pq.push({ next.e, d[next.e] });
			}
		}
	}	
		
	if (d[mEndGateID] != INF)		
		ret = d[mEndGateID];

	//printf("%d \n", ret);
	return ret;
}
#elif 0
#include <iostream>
#define MAP_SIZE_MAX    350
#define rint register int
#define pii pair<int, int>
using namespace std;
int N, maxS, ts;
int(*maps)[MAP_SIZE_MAX];
int visit[MAP_SIZE_MAX][MAP_SIZE_MAX];
struct Q {
	int x, y, h, c;
}gpos[201], que[350 * 350];
int vlen[201];
pii v[201][201];
int dx[] = { 0, 1, 0, -1 };
int dy[] = { 1, 0, -1, 0 };
void init(int N, int mMaxStamina, int mMap[MAP_SIZE_MAX][MAP_SIZE_MAX])
{
	::N = N;
	maxS = mMaxStamina;
	maps = mMap;
	for (rint i = 1; i < 201; i++) vlen[i] = 0, gpos[i] = {};
}

void addGate(int mGateID, int mRow, int mCol)
{
	gpos[mGateID] = { mRow, mCol };
	maps[mRow][mCol] = -mGateID;
	visit[mRow][mCol] = ++ts;
	rint wr = 0, rd = 0;
	que[wr++] = { mRow, mCol, maxS };
	while (wr != rd) {
		auto t = que[rd++];
		//if (maps[t.x][t.y] < 0 && mGateID != -maps[t.x][t.y]) {
		if (maps[t.x][t.y] < 0 ){
			// && mGateID != -maps[t.x][t.y]) {
			int ey = -maps[t.x][t.y];
			v[mGateID][vlen[mGateID]++] = { ey, t.c };
			v[ey][vlen[ey]++] = { mGateID, t.c };
		}
		if (t.h == 0) continue;
		for (rint i = 0; i < 4; i++) {
			rint x = t.x + dx[i], y = t.y + dy[i];
			if (x < 0 || y < 0 || x == N || y == N || maps[x][y] == 1) continue;
			if (visit[x][y] != ts) {
				visit[x][y] = ts;
				que[wr++] = { x, y, t.h - 1, t.c + 1 };
			}
		}
	}
}
void removeGate(int mGateID)
{
	gpos[mGateID].h = 1;
	maps[gpos[mGateID].x][gpos[mGateID].y] = 0;
}

int getMinTime(int mStartGateID, int mEndGateID)
{
	rint wr = 0, rd = 0, ans = 99999999, check[201] = {};
	for (rint i = 1; i < 201; i++) check[i] = 99999999;
	que[wr++] = { mStartGateID };
	check[mStartGateID] = 0;
	while (wr != rd) {
		auto t = que[rd++];
		if (t.y >= ans) continue;
		if (t.x == mEndGateID) {
			ans = ans > t.y ? t.y : ans;
			continue;
		}
		for (rint i = 0; i < vlen[t.x]; i++) {
			auto e = v[t.x][i];
			if (gpos[e.first].h) continue;
			
			if (check[e.first] > t.y + e.second) 
				que[wr++] = { e.first, check[e.first] = t.y + e.second };
		}
	}
	return  ans == 99999999 ? -1 : ans;
}




#elif 0
#include <unordered_map>
#include <vector>
#define rint register int
#define MAX_N 999
#define MAX_H 1048576
using namespace std;
int (*m)[MAX_N], N;
vector<pair<int, int>> h[MAX_H];

struct Tile {
	int r, c, del;
	void add(int _r, int _c) {
		r = _r, c = _c;
		del = 0;
	}
}T[20010];

int dr[5] = { 0, 0, 1, 2, 2 };
int dc[5] = { 0, 2, 1, 0, 2 };

int get_key(int r, int c, int pos = 5) {
	int k = 0;
	for (int i = 0; i < 5; ++i) {
		k <<= 4;
		k += m[dr[i] + r][dc[i] + c];
	}
	return k;
}
void init(int N, int mInfo[MAX_N][MAX_N]){
	m = mInfo;
	for (int i = 0; i < MAX_H; ++i) h[i].clear();

	for (int i = 0; i < N; ++i) {
		for (int j = (i % 2); j < N; j += 2) {
			int k = get_key(i, j);
			h[k].push_back({ i,j });
		}
	}
}
bool iAB(int tile[3][3], int &pos) {
	for (int i = 0; i < 5; ++i) {
		if (tile[dr[i]][dc[i]] > 10) {
			pos = i;
			return false;
		}
	}
	return true;
}
int get_rkey(int tile[3][3]) {
	int k = 0;
	for (int i = 0; i < 5; ++i) {
		k <<= 4;		
		k += tile[dr[i]][dc[i]] > 10 ? tile[dr[i]][dc[i]] - 10 : tile[dr[i]][dc[i]] + 10;
	}
	return k;
}
void put_tile(int r, int c, int tid) {
	for (rint i = 0; i < 5; ++i) {
		m[dr[i] + r][dc[i] + c] = -tid;
	}
}
bool is_ok_put_tile(int r, int c) {
	for (rint i = 0; i < 5; ++i) {
		if (m[dr[i] + r][dc[i] + c] < 0)
			return false;
	}
	return true;
}
int addRectTile(int mID, int mTile[3][3]){
	rint pos;
	rint minr = 987654321, minc = 987654321;
	rint k = get_rkey(mTile);
	for (auto &it : h[k]) {
		if (is_ok_put_tile(it.first, it.second)) {
			if (it.first < minr || (it.first == minr && it.second < minc)) {
				minr = it.first, minc = it.second;
			}
		}
	}

	if (!iAB(mTile, pos)) {
		for (rint b = 1; b <= 5; ++b) {
			mTile[dr[pos]][dc[pos]] = b;
			k = get_rkey(mTile);
			for (auto &it : h[k]) {
				if (is_ok_put_tile(it.first, it.second)) {
					if (it.first < minr || (it.first == minr && it.second < minc)) {
						minr = it.first, minc = it.second;
					}
				}
			}
		}
	}

	
	if (minr == 987654321) {
		T[mID].del = 1;
		//printf("-1\n"); 
		return -1;
	}
	put_tile(minr, minc, mID);
	T[mID].add(minr, minc);

	//printf("%d \n", minr * 10000 + minc);

	return minr * 10000 + minc;
}
void removeRectTile(int mID){
	if (T[mID].del) return;
	T[mID].del = 1;
	for (rint i = 0; i < 5; ++i) {
		m[T[mID].r + dr[i]][T[mID].c + dc[i]] = 0;
	}
}

#elif 0
#define MD 30000
#define MX 3005
#define f(i,x,n) for(register int i=x;i<n;i++)

struct City {
	int id, pr, dis[7], hid[7];
	int l, L[20];
	bool prior(City& c, int h) {
		return pr == c.pr ? dis[h] == c.dis[h] ? id < c.id : dis[h] < c.dis[h] : pr < c.pr;
	}
}C[MX];

struct Heap {
	int h, N, H[MX];
	void up(int id) {
		int tmp = H[id];
		while (id) {
			int t = (id - 1) / 2;
			if (C[H[t]].prior(C[tmp], h)) break;
			H[id] = H[t];
			C[H[id]].hid[h] = id;
			id = t;
		}
		H[id] = tmp;
		C[tmp].hid[h] = id;
	}
	void add(int Id) {
		H[N] = Id;
		up(N++);
	}
	void down(int id) {
		int tmp = H[id];
		int t = 2 * id + 1;
		while (t < N) {
			if (t + 1 < N&&C[H[t + 1]].prior(C[H[t]], h)) t++;
			if (C[tmp].prior(C[H[t]], h)) break;
			H[id] = H[t];
			C[H[id]].hid[h] = id;
			id = t;
			t = 2 * id + 1;
		}
		H[id] = tmp;
		C[tmp].hid[h] = id;
	}
	void mod(int id) {
		up(id);
		down(id);
	}
}Hp[7];

int D[MX][MX], H[MX], B[3], G, lD;
bool L[MX][MX];
int A[25] = { 0,0,0,1,2,0,3,0,4,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,6 };

void init(int N, int Town[3]) {
	f(i, 0, 7) Hp[i].h = i, Hp[i].N = lD = 0;
	G = N + 1;
	f(i, 1, G) {
		C[i].id = i;
		C[i].pr = C[i].l = 0;
		f(j, 1, G) L[i][j] = D[i][j] = MD;
		D[i][i] = H[i] = 0;
	}
	f(i, 0, 3) {
		B[i] = Town[i];
		H[B[i]] = i + 2;
	}
}

void newLine(int M, int city[MX], int dis[MX]) {
	f(i, 0, M - 1) {
		if (L[city[i]][city[i + 1]]) {
			C[city[i]].L[C[city[i]].l++] = city[i + 1];
			C[city[i + 1]].L[C[city[i + 1]].l++] = city[i];
			L[city[i]][city[i + 1]] = L[city[i + 1]][city[i]] = 0;
		}
		if (D[city[i]][city[i + 1]] > dis[i])
			D[city[i]][city[i + 1]] = D[city[i + 1]][city[i]] = dis[i];
	}
}

int Q[MX * 2], R[MX], r = 0;
void bfs(int x) {
	r++;
	int h = 0, t = 1;
	Q[0] = x;
	R[x] = r;
	while (h < t) {
		int id = Q[h++];
		f(i, 0, C[id].l) {
			if (D[x][id] + D[id][C[id].L[i]] < D[x][C[id].L[i]]) {
				D[x][C[id].L[i]] = D[C[id].L[i]][x] = D[x][id] + D[id][C[id].L[i]];
				R[C[id].L[i]] = r;
				Q[t++] = C[id].L[i];
			}
			if (R[C[id].L[i]] < r) {
				R[C[id].L[i]] = r;
				Q[t++] = C[id].L[i];
			}
		}
	}
}

void changeLimitDistance(int dis) {
	lD = dis;
	f(i, 0, 7) Hp[i].N = 0;
	f(i, 0, 3) bfs(B[i]);
	f(i, 1, G) {
		if (H[i]) continue;
		f(x, 0, 3) {
			if (D[i][B[x]] <= dis) {
				C[i].dis[x] = D[i][B[x]];
				Hp[x].add(i);
			}
			f(y, x + 1, 3) {
				int h = H[B[x]] * H[B[y]];
				C[i].dis[A[h]] = D[i][B[x]] + D[i][B[y]];
				if (C[i].dis[A[h]] <= dis) Hp[A[h]].add(i);
				if (y == 1) {
					C[i].dis[6] = D[i][B[0]] + D[i][B[1]] + D[i][B[2]];
					if (C[i].dis[6] <= dis) Hp[6].add(i);
				}
			}
		}
	}
}

int findCity(int n, int des[4]) {
	int h = 1;
	f(i, 0, n) h *= H[des[i]];
	if (Hp[A[h]].N) {
		int id = Hp[A[h]].H[0];
		C[id].pr++;
		f(i, 0, 7) if (C[id].hid[i] < Hp[i].N&&C[id].dis[i] <= lD) Hp[i].mod(C[id].hid[i]);
		return id;
	}
	return -1;
}
#elif 0
#include <iostream>
using namespace std;
#define NM 10

int weight[NM][NM];
int result[NM][NM];

void main(int n){
	n = NM;
	for (int i = 0; i < NM; i++) {
		for (int j = 0; j < NM; j++) {
			weight[i][j] = 1;
		}
	}
	for (int i = 0; i < NM; i++) {
		for (int j = 0; j < NM; j++) {
			cout << result[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	int i, j, k;
	for (k = 0; k < n; k++)	{
		for (i = 0; i < n; i++)	{
			if (k == 0)	{
				for (j = 0; j < n; j++)			{
					result[i][j] = weight[i][j];
				}
			}
			/*
			for (j = 0; j < n; j++)		{
				if (result[i][j] > result[i][k] + result[k][j] )	{
					result[i][j] = result[i][k] + result[k][j];
				}
			}
			*/
		}
	}

	for (int i = 0; i < NM; i++) {
		for (int j = 0; j < NM; j++) {
			cout << result[i][j] << " "; 
		}
		cout << endl; 
	}
}
#elif 0
#include<vector>
using namespace std;
#define NM 3'000
int L, N, *DT;
struct A { int c, d; };
vector<A> v[NM];
void init(int N, int mDownTown[]) {
	::N = N; DT = mDownTown;
	for (int i = 0; i < N; i++) { v[i].clear(); }

}
void newLine(int M, int mCityIDs[], int mDistances[]){
	for (int i = 0; i < M - 1; i++) {
		v[mCityIDs[i]].push_back({ mCityIDs[i + 1], mDistances[i] });
		v[mCityIDs[i + 1]].push_back({ mCityIDs[i], mDistances[i] });
	}
}
void changeLimitDistance(int mLimitDistance) {	L = mLimitDistance; }
int findCity(int mOpt, int mDestinations[]) {

}
#elif 0
//ref. Evanescence 님.
#define MAXN 3000
#define MAXC 601
#define R register
#define rint R int

struct CITY {
	int idx;
	int road[2];
	int next;
}city[MAXC];
int cidx;

int map[MAXC];

struct ROAD {

	int cid;
	int cost;
	int next;
}road[MAXN];
int ridx;

int find_cid(int cval) {

	rint h = cval % MAXC;
	rint id = map[h];
	while (id && city[id].idx != cval)
		id = city[id].next;

	if (id == 0) {

		city[cidx].idx = cval;
		city[cidx].road[0] = city[cidx].road[1] = 0;
		city[cidx].next = map[h];
		map[h] = cidx;
		id = cidx++;
	}
	return id;
}

int init(int N, int sCity[], int eCity[], int mCost[]) {
	cidx = ridx = 1;
	for (rint i = 0; i < MAXC; i++)
		map[i] = 0;

	rint st, en;
	for (rint i = 0; i < N; i++) {

		st = find_cid(sCity[i]);
		en = find_cid(eCity[i]);

		road[ridx] = { en, mCost[i] };
		road[ridx].next = city[st].road[0];
		city[st].road[0] = ridx++;

		road[ridx] = { st, mCost[i] };
		road[ridx].next = city[en].road[1];
		city[en].road[1] = ridx++;
	}

	return cidx - 1;
}

void add(int sCity, int eCity, int mCost) {

	rint st, en;

	st = find_cid(sCity);
	en = find_cid(eCity);

	road[ridx] = { en, mCost };
	road[ridx].next = city[st].road[0];
	city[st].road[0] = ridx++;

	road[ridx] = { st, mCost };
	road[ridx].next = city[en].road[1];
	city[en].road[1] = ridx++;
}

void djistra(int start, int order, int lcost[]) {

	for (rint i = 1; i < cidx; i++)
		lcost[i] = 0xfffffff;
	lcost[start] = 0;

	rint q[MAXN];
	rint f = 0, r = 0;
	q[r++] = start;

	while (f < r) {

		rint tval = q[f++];
		for (rint i = city[tval].road[order]; i != 0; i = road[i].next) {

			if (lcost[road[i].cid] > lcost[tval] + road[i].cost) {

				lcost[road[i].cid] = lcost[tval] + road[i].cost;
				q[r++] = road[i].cid;
			}
		}
	}
}

int cost(int mHub) {

	rint ret = 0;
	rint key = find_cid(mHub);
	rint local_cost[MAXC];

	djistra(key, 0, local_cost);

	for (rint i = 1; i < cidx; i++)
		ret += local_cost[i];

	djistra(key, 1, local_cost);

	for (rint i = 1; i < cidx; i++)
		ret += local_cost[i];

	return ret;
}
#elif 0
#include<unordered_map>
using namespace std;
unordered_map<int, int> h; int idx;
const int NMAX = 600;
int m1[NMAX + 1][NMAX + 1], m2[NMAX + 1][NMAX + 1];

int init(int N, int sCity[], int eCity[], int mCost[]) {
	idx = 0;
	for (int i = 0; i < NMAX; i++) {
		for (int j = 0; j < NMAX; j++) {
			if (i == j)
				m1[i][j] = 0, m2[i][j] = 0;
			else
				m1[i][j] = 987654321, m2[i][j] = 987654321;
		}
	}
	for (int i = 0; i < N; i++) {
		int s, e;
		if (!h.count(sCity[i])) {
			h[sCity[i]] = idx++;
		}
		if (!h.count(eCity[i])) {
			h[eCity[i]] = idx++;
		}
		s = h[sCity[i]];
		e = h[eCity[i]];
		m1[s][e] = mCost[i];
		m2[e][s] = mCost[i];
	}
	//printf("(%s) %d\n", __FUNCTION__, idx);
	return idx;
}
void add(int sCity, int eCity, int mCost) {
	int s, e;
	if (!h.count(sCity)) {
		h[sCity] = idx++;
		s = idx;
	}
	if (!h.count(eCity)) {
		h[eCity] = idx++;
		e = idx;
	}
	s = h[sCity];
	e = h[eCity];
	m1[s][e] = mCost;
	m2[e][s] = mCost;
}
int d[NMAX + 1], v[NMAX + 1];
void dijkstra(int n, int s, int(&m)[NMAX + 1][NMAX + 1]) {
	int k;
	for (int i = 0; i < n; i++)
		d[i] = 987654321, v[i] = 0;

	d[s] = 0;
	for (int i = 0; i < n; i++) {
		int min = 987654321;
		for (int j = 0; j < n; j++) {
			if (v[j] == 0 && min > d[j]) {
				min = d[j];
				k = j;
			}
		}
		v[k] = 1;
		for (int j = 0; j < n; j++) {
			if (d[j] > d[k] + m[k][j])
			{
				d[j] = d[k] + m[k][j];
			}
		}
	}
}
int cost(int mHub) {
	int ret = 0;
	int hb = h[mHub];
	//floyd(idx);

	dijkstra(idx, hb, m1);
	for (int i = 0; i < idx; i++) {
		ret += d[i];
	}
	dijkstra(idx, hb, m2);
	for (int i = 0; i < idx; i++) {
		ret += d[i];
	}
	//printf("(%s) %d\n", __FUNCTION__ , ret);
	return ret;
}
#elif 0
#include<iostream>
#include<queue>
#include<unordered_map>
using namespace std;
struct A {
	int e, c;
	bool operator<(const A &r)const {
		return c > r.c;
	}
} a[10];
priority_queue<A> pq;

int main() {
	a[0] = { 10, 20 };
	a[1] = { 20, 20 };
	a[2] = { 30, 10 };
	a[3] = { 10, 10 };
	a[4] = { 10, 60 };
	sort(a, a + 3);
	for (auto &i : a) {
		cout << i.c << endl;
	}
	cout << "-------------" << endl;
	pq.push({ 10, 20 });
	pq.push({ 20, 20 });
	pq.push({ 30, 10 });
	pq.push({ 10, 10 });
	pq.push({ 10, 60 });
	while (!pq.empty()) {
		cout << pq.top().c << endl; pq.pop();
	}


}
#elif 1
#include<iostream>
#include<queue>
#include<unordered_map>
using namespace std;
#define NM	601
#define INF 987654321
unordered_map<int, int> um; int umdx;
struct A {
	int e, c;
	bool operator<(const A &r)const {
		return c > r.c;
	}
};
vector<A> v1[NM], v2[NM];
int d[NM];
int init(int N, int sCity[], int eCity[], int mCost[]) {
	int ret = -1;
	um.clear(); umdx = 0;
	for (int i = 0; i < NM; i++) {
		v1[i].clear();
		v2[i].clear();
	}
	for (int i = 0; i < N; i++) {
				
		if (um.count(sCity[i]) == 0)  um[sCity[i]] = ++umdx;
		if (um.count(eCity[i]) == 0)  um[eCity[i]] = ++umdx;
		/*
		um[sCity[i]] = sCity[i]; 
		um[eCity[i]] = eCity[i];
		*/
		
		
		v1[um[sCity[i]]].push_back({ um[eCity[i]], mCost[i] });
		v2[um[eCity[i]]].push_back({ um[sCity[i]], mCost[i] });
	}
	ret = um.size();
	//printf("%s %d\n", __FUNCTION__, ret);
	return ret; 
}
void add(int sCity, int eCity, int mCost) {
	
	if (um.count(sCity) == 0)  um[sCity] = ++umdx;
	if (um.count(eCity) == 0)  um[eCity] = ++umdx;
	/*
	um[sCity] = sCity;
	um[eCity] = eCity;
	*/

	v1[um[sCity]].push_back({ um[eCity], mCost });
	v2[um[eCity]].push_back({ um[sCity], mCost });
	//printf("%s %d %d %d\n", __FUNCTION__, sCity, eCity, mCost);
}
priority_queue<A> pq;
int dijkstra(int s, vector<A> (&v)[NM]) {
	int ret = 0;
	for (int i = 0; i <= um.size(); i++) {
		d[i] = INF;
	}
	pq = {};
	d[s] = 0;
	pq.push({ s, 0 });
	while (!pq.empty()) {
		auto curr = pq.top(); pq.pop();
		if (d[curr.e] < curr.c) continue;
		for (int i = 0; i < v[curr.e].size(); i++) {
			auto next = v[curr.e][i];
			if (d[next.e] > d[curr.e] + next.c) {
				d[next.e] = d[curr.e] + next.c;
				pq.push({ next.e, d[next.e] });
			}
		}
	}
	for (int i = 1; i <= um.size(); i++) {
		ret += d[i];
		if (ret > 10000)
			while (0);
	}
	//printf("%s %d\n", __FUNCTION__, ret);
	return ret;
}
int cost(int mHub) {
	int ret = 0;
	//um[mHub] = mHub;
	ret += dijkstra(um[mHub], v1);
	ret += dijkstra(um[mHub], v2);
	//printf("%s %d\n", __FUNCTION__, ret);
	return ret;
}

#elif 0
#include <iostream>
#include <queue>
#define NM	1'000
#define MM	10'000
#define INF 987654321
using namespace std;
struct A {
	int t, d;
	bool operator<(const A &r) const {
		return d < r.d;
	}
};
vector<A> v[NM];
priority_queue<A> pq;
int N, M, X, s, e, t, d[NM];

void dijkstra(int x) {
	for (int i = 0; i < N; i++) d[i] = INF; 
	pq = { };
	pq.push({ x, 0 });
		
	while (!pq.empty()) {
		auto curr = pq.top();	pq.pop();
		
		if (d[curr.t] < curr.d) 
			continue;
		{
			for (auto &next : v[curr.t]) {				
				if (d[next.t] > d[curr.t] + next.d) {
					d[next.t] = d[curr.t] + next.d;
					pq.push({ next.t, d[next.t] });
				}
			}
		}		
	}
	
	
}
int main() {
		
	scanf("%d %d %d ", &N, &M, &X);
	for (int i = 0; i < M; i++) {
		scanf("%d %d %d ", &s, &e, &t);
		v[s].push_back({ e, t });
	}
	dijkstra(X);
	return 0;
}



#elif 0
//22353
#include <iostream>
#include <queue>
using namespace std;

priority_queue<pair<int, int>, vector<pair<int,int>>, less<pair<int, int>>> pq;
//priority_queue<int, vector<int>> pq;
//priority_queue<int, vector<int>, less<>> pq;
int a[] = { 10,30,40,20,10 };
int main() {

	pq.emplace( 10,30 );
	pq.emplace(30,20);
	pq.emplace(40,90);
	pq.emplace(20,20);
	pq.emplace(10,10);

	sort(a, a + sizeof(a) / sizeof(a[0]));
	for (auto &i : a) {
		cout << i << endl;
	}
	cout << "=----------------" << endl;
	sort(a, a + sizeof(a) / sizeof(a[0]), less<>());
	for (auto &i : a) {
		cout << i << endl;
	}
	cout << "=----------------" << endl;
	while (!pq.empty()) {
		cout << pq.top().first << " " << pq.top().second << endl; pq.pop();
	}

	return 0;
}

#elif 0
#include <iostream>
#include <vector>
#include <algorithm>
#define EM 100'001
#define NM 1'000'001
using namespace std;
int s, e, w, vn, en;
struct E { 
	int s, e, w; 
	bool  operator<(E &r) {
		return w < r.w;
	}
};
bool cmp(E &l, E &r) {
	return l.w < r.w;
}
vector<E> v;
int p[NM]; 
int fp(int n) {
	return p[n] == n ? n : p[n] = fp(p[n]);
}
void up(int s, int e) {
	int sp = fp(s);
	int ep = fp(e);
	sp > ep ? p[sp] = ep : p[ep] = sp;
}
void kruskal() {
	int ret = 0;
	sort(v.begin(), v.end());

	for (int i = 0; i < vn; i++) p[i] = i;
	int mmax = 0;
	for (auto &i : v) {
		int sp = fp(i.s);
		int ep = fp(i.e);
		if (sp != ep) {
			up(i.s, i.e);
			mmax = max(mmax, i.w);
			ret += i.w;
		}
	}

	cout << ret << endl;
}
unordered_map<int, int> um; int umid;
struct P { int id, x, y; };
vector<P> a;
int add(int mId, int mX, int mY) {
	for (int i = 0; i < a.size(); i++){
		if (a[i].id > 0) {
			int d = abs(a[i].x - mX) + abs(a[i].y - mY);
			v.push_back({ i, umid + 1, d });
		}
	}
	a.push_back({ umid++, mX, mY });
}
int remove(int mId) {
	a[um[mId]].id = -1;
}
int main() {	
	
	scanf("%d %d ", &vn, &en);
	for (int i = 0; i < en; i++) {
		scanf("%d %d %d ", &s, &e, &w);
		v.push_back({ s,e,w });
	}

	kruskal();
}



#elif 0

#include <stdio.h>
#include <algorithm>
using namespace std;

// 배열 요소들을 바꿔주는 함수
//void swap(int *a, int *b) {
//	int temp = *a;
//	*a = *b;
//	*b = temp;
//}

// 배열의 순열을 생성하는 함수
void permutation(int *a, int n, int r, int depth) {
	if (depth == r) { // 순열의 길이가 r에 도달하면 순열을 출력
		for (int i = 0; i < r; ++i) {
			printf("%d ", a[i]);
		}
		printf("\n");
		return;
	}

	for (int i = depth; i < n; ++i) {
		swap(a[depth], a[i]); // 배열의 요소들을 교환
		permutation(a, n, r, depth + 1); // 재귀적으로 순열 생성
		swap(a[depth], a[i]); // 원래 상태로 되돌림
	}
}

int main() {
	int n = 3; // 배열의 크기
	int r = 3; // 뽑을 요소의 개수
	int a[] = { 1, 2, 3, 4, 5 }; // 배열

	permutation(a, n, r, 0); // 순열 생성 함수 호출

	return 0;
}
#elif 0


#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

#define MAX_N 999
#define MAX_TILE 20005

struct Tile {
	int x;
	int y;
	int used;
};

struct Pii {
	int x;
	int y;
};

const int Delta[5][2] = { {0, 0}, {0, 2}, {1, 1}, {2, 0}, {2, 2} };
unordered_map<int, vector<Pii>> Finder;
Tile Tiles[MAX_TILE];
int Visited[MAX_N][MAX_N];
int N;
int Key[5];

void init(int N, int mInfo[MAX_N][MAX_N])
{
	::N = N;

	Finder.clear();

	int cnt = 0, code = 0;
	for (int x = 0; x < N - 2; x++) {
		int start = x % 2, end = N - 2 - x % 2;
		for (int y = start; y < end; y += 2) {
			cnt = 0;
			for (int d = 0; d < 5; d++) {
				int nx = x + Delta[d][0];
				int ny = y + Delta[d][1];
				Key[d] = mInfo[nx][ny];
				if (mInfo[nx][ny] > 10) cnt++;
			}

			if (cnt < 4) continue;

			code = 0;
			for (int d = 0; d < 5; d++) {
				int val = Key[d] > 10 ? Key[d] - 10 : Key[d] + 10;
				code = code * 16 + val;
			}
			Finder[code].push_back({ x, y });

			if (cnt == 5) {
				for (int i = 0; i < 5; i++) {
					int temp = Key[i];
					for (int j = 1; j <= 5; j++) {
						Key[i] = j;
						code = 0;
						for (int k = 0; k < 5; k++) {
							int val = Key[k] > 10 ? Key[k] - 10 : Key[k] + 10;
							code = code * 16 + val;
						}
						Finder[code].push_back({ x, y });
					}
					Key[i] = temp;
				}
			}
}
	}
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			Visited[i][j] = 0;
		}
	}
	for (int i = 0; i < MAX_TILE + 1; i++) {
		Tiles[i] = { -1, -1, 0 };
	}
}

bool possible(Pii p) {
	for (int d = 0; d < 5; d++) {
		if (Visited[p.x + Delta[d][0]][p.y + Delta[d][1]] == 1)
			return false;
	}
	return true;
}

int addRectTile(int mID, int mTile[3][3])
{
	int code = 0;
	for (int d = 0; d < 5; d++) {
		code = code * 16 + mTile[Delta[d][0]][Delta[d][1]];
	}

	int ret = -1;
	for (auto p : Finder[code]) {
		if (possible(p)) {
			Tiles[mID] = { p.x, p.y, 1 };
			for (int d = 0; d < 5; d++) {
				Visited[p.x + Delta[d][0]][p.y + Delta[d][1]] = 1;
			}
			ret = p.x * 10000 + p.y;
			break;
		}
	}

	return ret;
}

void removeRectTile(int mID)
{
	if (Tiles[mID].used == 1) {
		for (int d = 0; d < 5; d++) {
			Visited[Tiles[mID].x + Delta[d][0]][Tiles[mID].y + Delta[d][1]] = 0;
		}
		Tiles[mID].used = 0;
	}
}

#elif 0
#include<vector>
#include<iostream>
using namespace std;
#define MAX_N 999
int N,(*m)[MAX_N];
int dr[] = {0, 2, 0, 1, 2};
int dc[] = {0, 0, 2, 1, 2};

struct Q { int r, c, s[5]; };
vector<Q> v;
void init(int N, int mInfo[MAX_N][MAX_N])
{
	::N = N, m = mInfo;
	for (int i = 0; i < N - 2; i++) {
		for (int j = i % 2; j < N - 2 + i % 2; j += 2) {
			int cnt = 0; int t[5];
			for (int k = 0; k < 5; k++) {				
				t[k] = m[i + dr[k]][j + dc[k]];
				if (t[k] < 10)
					cnt++;
				if (cnt > 1)
					break;
			}
			if (cnt > 1)
				continue;
			else {
				v.push_back({ i, j, {t} });
			}
		}
	}
}

int addRectTile(int mID, int mTile[3][3])
{
	return 0;
}

void removeRectTile(int mID)
{
}
#elif 0
#include<iostream>
#include<algorithm>
#define VM 10001
#define EM 100001
using namespace std;
struct Edge { int s, e, w; } edge[EM];
bool cmp(Edge const &l, Edge const &r) {return l.w < r.w; };
int V, E, p[VM];
int findparent(int n) {
	return p[n] == n ? n : p[n] = findparent(p[n]);
}
void unionparent(int s, int e) {
	int sp = findparent(s);
	int ep = findparent(e);
	if (sp > ep) p[sp] = ep;
	else p[ep] = sp;
}
int kruskal() {
	int ret = 0;
	for (int i = 0; i < VM; i++) p[i] = i;
	sort(edge, edge + E, cmp);
	for (auto &i : edge) {
		int sp = findparent(i.s);
		int ep = findparent(i.e);

		if (sp != ep) {
			unionparent(i.s, i.e);
			ret += i.w;
		}
	}
	return ret;
}
int main() {
	scanf("%d %d ", &V, &E);
	for (int i = 0; i < E; i++) {
		int s, e, w;
		scanf("%d %d %d ", &s, &e, &w);
		edge[i] = { s,e,w };
	}	
	cout << kruskal() << endl;;
	return 0;
}

#elif 0
#include<iostream>
#include<algorithm>
#define VM 8
#define EM 11
using namespace std;
struct E { int s, e, w; }edge[EM], mst[VM];
bool cmp(E const  &l, E const  &r) { return l.w < r.w; }
int p[VM];
int findparent(int p[], int n) {
	return p[n] == n ? n : p[n]= findparent(p, p[n]);
}
void unionparent(int p[], int s, int e) {
	int sp = findparent(p, s);
	int ep = findparent(p, e);
	if (sp > ep) p[sp] = ep;
	else p[ep] = sp;
}
int kruskal(int p[]) {
	int ret = 0;
	for (int i = 0; i < VM; i++) { p[i] = i; }
	
	sort(edge, edge + EM, cmp);
	int cnt = 0;
	for (auto &i : edge) {
		int sp = findparent(p, i.s);
		int ep = findparent(p, i.e);
		if (sp != ep) {
			unionparent(p, i.s, i.e); 
			mst[cnt++] = i;
			ret += i.w;
		}
		if (cnt == VM - 1)
			break;
	}
	return ret;
}
int main() {
	edge[0] = { 1,7,12 };
	edge[1] = { 1,4,28 };
	edge[2] = { 1,2,67 };
	edge[3] = { 1,5,17 };
	edge[4] = { 2,4,24 };
	edge[5] = { 2,5,62 };
	edge[6] = { 3,5,20 };
	edge[7] = { 3,6,37 };
	edge[8] = { 4,7,13 };
	edge[9] = { 5,6,45 };
	edge[10] = { 5,7,73 };

	cout << kruskal(p) << endl;
	for (auto &i : mst)
		cout << i.s << " " << i.e << endl;
	return 0;
}

#elif 0
#include <iostream>
#include <algorithm>
using namespace std;
//int a[] = { 234,4,6,2,7,456,8,4,2,1 };

int findparent(int p[], int n) {
	return p[n] == n ? n : p[n] = findparent(p, p[n]);
}
void unionparent(int p[], int a, int b) {
	//int ap = findparent(p, a);
	//int bp = findparent(p, b);
	if (a < b) p[b] = a;
	else p[a] = b;	
}
struct E { int s, e, w; } edge[10], mst[13];
bool cmp(const E &l, const E &r) {return l.w < r.w;}
int p[100]; 
int kruskal(E e[], int V, int E) {
	int ret = 0;
	sort(e, e + V, cmp);
	for (int i = 0; i < V; i++) {	p[i] = i;	}

	int cnt = 0;
	for (int i = 0; i < E; i++) {
		int sp = findparent(p, e[i].s);
		int ep = findparent(p, e[i].e); 
		if (sp != ep) {
			unionparent(p, sp, ep);
			mst[cnt++] = e[i];
			ret += e[i].w;
		}
		if (cnt == V - 1)
			break;
	}
	return ret;
}
int main() {

	edge[0] = { 1,7,12 };
	edge[1] = { 1,4,28 };
	edge[2] = { 1,2,67 };
	edge[3] = { 1,5,17 };
	edge[4] = { 2,4,24 };
	edge[5] = { 2,5,62 };
	edge[6] = { 3,5,20 };
	edge[7] = { 3,6,37 };
	edge[8] = { 4,7,13 };

	edge[9] = { 5,6,45 };
	edge[10] = { 5,7,73 };

	cout << kruskal(edge, 10, 11) << endl;
	for (auto &i : mst) {
		cout << i.s << " " << i.e << endl;
	}
	for (int i = 0; i < 10; i++) {
		cout << p[i] << endl;
	}





	//a[0] = { 1,2 };
	//a[1] = { 3,6 };
	//a[2] = { 3,2 };
	//a[3] = { 1,2 };
	//a[4] = { 8,6 };
	//a[5] = { 1,2 };
	//a[6] = { 1,2 };
	//a[7] = { 1,1 };
	//a[8] = { 4,7 };
	//a[9] = { 7,6 };	
	////sort(a, a + 10, cmp());
	//sort(a, a + 10, compare);
	////for (auto &i : a) {	cout << i.f << " " << i.s << endl;	}


	////sort(a, a + sizeof(a) / sizeof(a[0]));
	//sort(a, a + 10, less<>());
	//for (auto &i : a) { cout << i << endl; }
	//sort(a, a + 10, less<int>());
	//for (auto &i : a) { cout << i << endl; }
	//sort(a, a + 10, greater<int>());
	//for (auto &i : a) { cout << i << endl; }
	//sort(a, a + 10, greater<>());
	//for (auto &i : a) { cout << i << endl; }
	//

	return 0;
}

#elif 0
#include <stdio.h>
#include <stdlib.h>
#define V 100
#define E 100

// 간선 구조체 정의
struct Edge {
	int src, dest, weight;
};

// 부모 노드를 찾는 함수
int findParent(int parent[], int node) {
	if (parent[node] == node)
		return node;
	else
		return parent = findParent(parent, parent[node]);
}

// 두 부모 노드를 합치는 함수
void unionSet(int parent[], int x, int y) {
	int parentX = findParent(parent, x);
	int parentY = findParent(parent, y);
	parent[parentX] = parentY;
}

// 크루스칼 알고리즘 함수
void kruskalMST(struct Edge edges[], int V, int E) {
	// 간선을 가중치 순으로 정렬
	qsort(edges, E, sizeof(struct Edge), [](const void *a, const void *b) {
		struct Edge *edge1 = (struct Edge *)a;
		struct Edge *edge2 = (struct Edge *)b;
		return edge1->weight - edge2->weight;
	});

	// 부모 배열 초기화
	int parent[V];
	for (int i = 0; i < V; ++i)
		parent[i] = -1;

	// 최소 신장 트리를 저장할 배열
	struct Edge MST[V - 1];
	int edgeCount = 0;
	for (int i = 0; i < E; ++i) {
		int parentSrc = findParent(parent, edges[i].src);
		int parentDest = findParent(parent, edges[i].dest);
		if (parentSrc != parentDest) {
			MST[edgeCount++] = edges[i];
			unionSet(parent, edges[i].src, edges[i].dest);
		}
		// 모든 간선을 확인하면서 V-1개의 간선을 선택할 때까지 반복
		if (edgeCount == V - 1)
			break;
	}

	// 최소 신장 트리 출력
	printf("Edges in Minimum Spanning Tree:\n");
	for (int i = 0; i < V - 1; ++i) {
		printf("%d - %d : %d\n", MST[i].src, MST[i].dest, MST[i].weight);
	}
}

int main() {
	//int V, E;
	printf("Enter number of vertices and edges: ");
	scanf("%d %d", &V, &E);

	// 간선 리스트 초기화
	struct Edge edges[E];
	printf("Enter source, destination, and weight of each edge:\n");
	for (int i = 0; i < E; ++i) {
		scanf("%d %d %d", &edges[i].src, &edges[i].dest, &edges[i].weight);
	}

	// 크루스칼 알고리즘 호출
	kruskalMST(edges, V, E);

	return 0;
}


#elif 0

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// 간선 구조체 정의
struct Edge {
	int src, dest, weight;
};

// 부모 노드를 찾는 함수
int findParent(vector<int>& parent, int node) {
	if (parent[node] == -1)
		return node;
	return findParent(parent, parent[node]);
}

// 두 부모 노드를 합치는 함수
void unionSet(vector<int>& parent, int x, int y) {
	int parentX = findParent(parent, x);
	int parentY = findParent(parent, y);
	parent[parentX] = parentY;
}

// 크루스칼 알고리즘 함수
void kruskalMST(vector<Edge>& edges, int V) {
	// 간선을 가중치 순으로 정렬
	sort(edges.begin(), edges.end(), [](Edge& a, Edge& b) {
		return a.weight < b.weight;
	});

	// 부모 배열 초기화
	vector<int> parent(V, -1);

	// 최소 신장 트리를 저장할 벡터
	vector<Edge> MST;

	int edgeCount = 0;
	for (Edge& edge : edges) {
		int parentSrc = findParent(parent, edge.src);
		int parentDest = findParent(parent, edge.dest);
		if (parentSrc != parentDest) {
			MST.push_back(edge);
			unionSet(parent, edge.src, edge.dest);
			edgeCount++;
		}
		// 모든 간선을 확인하면서 V-1개의 간선을 선택할 때까지 반복
		if (edgeCount == V - 1)
			break;
	}

	// 최소 신장 트리 출력
	cout << "Edges in Minimum Spanning Tree:" << endl;
	for (Edge& edge : MST) {
		cout << edge.src << " - " << edge.dest << " : " << edge.weight << endl;
	}
}

int main() {

	vector<int> v(10);

	for (auto &i : v) {
		cout << i << endl;
	}
	return 0;


	int V, E;
	cout << "Enter number of vertices and edges: ";
	cin >> V >> E;

	// 간선 리스트 초기화
	vector<Edge> edges(E);
	cout << "Enter source, destination, and weight of each edge:" << endl;
	for (int i = 0; i < E; ++i) {
		cin >> edges[i].src >> edges[i].dest >> edges[i].weight;
	}

	// 크루스칼 알고리즘 호출
	kruskalMST(edges, V);

	return 0;
}

#elif 0
#include<vector>
#include<queue>
using namespace std;
#define NM	350
int(*m)[NM], N, R;
void init(int N, int mRange, int mMap[NM][NM]) {
	::N = N, R = mRange, m = mMap;
}

void add(int mID, int mRow, int mCol) {

}


#elif 0
#include<iostream>
#include<algorithm>
#include<vector>
#define rint register int
#define NM	500
using namespace std;
vector<pair<int, int>> v[NM + 1];
int N, K;
void init(int N, int K, int mRoadAs[], int mRoadBs[], int mLens[])
{
	for (rint i = 0; i <= N; i++) {
		v[i].clear();
	}
	for (rint i = 0; i < K; i++) {
		v[mRoadAs[i]].push_back({ mRoadBs[i], mLens[i] });
		v[mRoadBs[i]].push_back({ mRoadAs[i], mLens[i] });
	}
}
void addRoad(int mRoadA, int mRoadB, int mLen)
{
	v[mRoadA].push_back({ mRoadB, mLen });
	v[mRoadB].push_back({ mRoadA, mLen });
}
int findPath(int mStart, int mEnd, int M, int mStops[])
{
	return 0;
}
#elif 0
#include<iostream>
#include<algorithm>
#include<vector>
#define rint register int
using namespace std;
int N, M, a[10], o[10], v[10];
void perm(int a[], int n, int r, int d) {
	if (d == r) {
		for (rint i = 0; i < r; i++) {
			printf("%d ", o[i]);
		}printf("\n");
	}
	for (rint i = 0; i < n; i++) {
		if (!v[i]) {
			o[d] = a[i];

			v[i] = 1;
			perm(a, n, r, d + 1);
			v[i] = 0;
		}
	}
}
int main() {
	scanf("%d %d ", &N, &M);
	for (rint i = 0; i < N; i++) {
		scanf("%d ", &a[i]);
	}
	//vector<int> v;
	sort(a, a + N);
	//sort(a, a + N, less<int>());	
	//sort(v.begin(), v.end(), greater<int>());	
	//for (rint i = 0; i < N; i++) {
	//	printf("%d ", a[i]);
	//}
	memset(v, 0, sizeof(v));
	memset(o, 0, sizeof(o));
	perm(a, N, M, 0);
	return 0;
}

#elif 0
#include<iostream>
using namespace std;
int N, M, a[10], o[10], v[10];
void perm(int a[], int n, int r, int d) {
	if (d == r) {
		for (int i = 0; i < r; i++) {
			printf("%d ", o[i]);
		}printf("\n");
	}
	for (int i = 0; i < n; i++) {
		if (!v[i]) {
			o[d] = a[i];

			v[i] = 1;
			perm(a, n, r, d + 1);
			v[i] = 0;
		}
	}
}
int main() {
	scanf("%d %d", &N, &M);
	for (int i = 0; i < N; i++) {
		a[i] = i + 1;
	}
	perm(a, N, M, 0);

	return 0;
}
#elif 0
#include<iostream>
using namespace std;
const int NM = 6;
char a[NM] = "ABCDE", o[NM], v[NM];
void perm(char a[], int n, int r, int d) {

	if (d == r) {
		/*
		for (int i = 0; i < r; i++) {
			printf("%c ", o[i]);
		}printf("\n");
		*/
		printf("%s\n", o);
		return;
	}

	for (int i = 0; i < n; i++) {
		if (!v[i]) {
			o[d] = a[i];

			v[i] = 1;
			perm(a, n, r, d + 1);
			v[i] = 0;
		}
	}
}
int main() {
	memset(o, 0, sizeof(o));
	memset(o, 0, sizeof(v));
	perm(a, NM - 1, 3, 0);
	return 0;
}
#elif 0

//[H2409][Pro] 예약버스 
// 1. pair 연습하고 / 다음은 구조체로 한다. 
// 2. vector로 연습하고 / 다음은 배열로 만들어 본다. 
#include<vector>
#define rint register int
using namespace std;
const int NM = 500;//개수임
int N;
vector<pair<int, int>> v[NM + 1];//번호가 1번부터 시작하므로 1을 더해준다
void init(int N, int K, int mRoadAs[], int mRoadBs[], int mLens[]) {
	::N = N;
	//노드 번호는 1번부터 시작하므로
	//구조체도 1번부터 500번까지 사용한다.
	//그러므로 =가 들어가야 한다. 
	for (rint i = 0; i <= N; i++)
		v[i].clear();
	//길의 번호는 의미가 없으므로 개수만 맞춰준다.
	//mRoad에 들어 있는 값이 1~500개임을 유의하라. 
	//vector[시작][끝] = mLens가 들어가는 구조이다.
	//즉, 시작점에서 끝점까지의 갈때 length를 나타내는 배열이다.
	//양방향으로 움직이므로 양쪽을 다 넣어준다. 
	for (rint i = 0; i < K; i++) {
		v[mRoadAs[i]].push_back({ mRoadBs[i], mLens[i] });
		v[mRoadBs[i]].push_back({ mRoadAs[i], mLens[i] });
	}
}
void addRoad(int mRoadA, int mRoadB, int mLen) {
	v[mRoadA].push_back({ mRoadB, mLen });
	v[mRoadB].push_back({ mRoadA, mLen });
}
int findPath(int mStart, int mEnd, int M, int mStops[]) {
	return 0;
}
#elif 0
#include <stdio.h>
const int NM = 4;
int a[] = { 1,2,3,4 }, v[NM], o[NM];
void perm(int a[], int d, int n, int r) {

	if (d == r) {
		for (int i = 0; i < r; i++) {
			printf("%d ", o[i]);
		}
		printf("\n");
		return;
	}

	for (int i = 0; i < n; i++) {
		if (!v[i]) {

			o[d] = a[i];

			v[i] = 1;
			perm(a, d + 1, n, r); // 재귀 함수 
			v[i] = 0;	// 중복 확인 배열 
		}
	}
}

int main() {
	perm(a, 0, NM, 2);
}
#elif 0

#include <iostream>

using namespace std;

void swap(int & a, int & b)
{
	int temp = a;
	a = b;
	b = temp;
}

void permutation(int data[], int depth, int n, int r)
{
	if (depth == r)
	{
		for (int i = 0; i < r; i++)
			cout << data[i] << " ";
		cout << endl;

		return;
	}

	for (int i = depth; i < n; i++)
	{
		swap(data[depth], data[i]);   // 스왑
		permutation(data, depth + 1, n, r);  // ⭐재귀
		swap(data[depth], data[i]);  // 다시 원래 위치로 되돌리기
	}
}
int main()
{
	//char arr[] = { 'a', 'b', 'c', 'd' };
	int arr[] = { 1,2,3,4 };

	permutation(arr, 0, 4, 3); // 4P3

	return 0;
}
#elif 0
//  
//   Dijkstra  응용 &  순열 조합  방법 _개선(Ver 2) : 경유지에서만 다익스트라 수행
//
//   -> 5개의 정류소 대한 순열 조합은 최대 5! 5x4x3x2x1 = 120 가지 
//
//  *Idea:  최단거리를 구하는 문제라 다익스트라을 생각할 수 있다. 그런데 경유지가 있다 (네비게이션? ) 
//          게다가 경유지가 최대 5개이다. --> 경유지에 대한  순열조합이 발생한다.(5! ==> 최대 120개) 
//          출발점에서 조합 첫번째 경유지에 대한 최단 거리를 구하고 다시 경유지에서 다음 경유지 최단 거리 
//          그리고 마지막 경유지에서 도착점에 대한 최단 거리를 구해야 된다  그런데 그때 마다  다익스트라를 돌릴 수는 없다. 
//
//      * 결국 지점간 최단거리를 map_table(matrix)만 있으면  출발점 --->{ 경유지 순열조합} --> 도착점에 대한 거리를 
//        구할 수 있다. 
//      * 경유지점은 무조건 통과해야 되기 때문에 경유지점에서  다익스트라를 수행하여 그결과 값을  map_table  에 반영하고 이를 참조 하자. 
//     
//      * 주의할 사항은 문제조건 2번 출발점에서 출발한 경우 중간에 출발점과 도착점을 통과하면 안되기 때문에 
//        다익스트라 수행시  출발/도착점에 대한 간선 정보를  INF (갈수없음)으로 변경해야 한다. 
//
//
//   각 경유 정류소지점에  대한  Dijkstar 를 돌려  각 점에서부터의 최단 거리를 구하고 
//   전체 각지점  to 지점간 거리 정보를 저장하는  map_table[510][501]  에  update 하고 
//   단 이때 다익스트라를 돌릴 때  문제조건 2. 출발후  도착전  출발지점과 도착지점을 통과할 수 없기 때문에 
//   도착지점이  출발점과 도착점이 되는 path 는 무한대거리로 대체하여 (응용)하여 적응해야 된다. 
//   
//   이렇게 구해진  map_table 을 이용하여 
//   출발점 <-- { 경유지점 순열 조합 } --->도착점  형태로  모든 순열 조합에 대해 거리를 구하고 이 거리중 가장 작은 값을 선택한다. 
//        ------
//         -> 출발점에서 최단거리(다익스트라)수행을 하지 않았기 때문에 반대로 경유지 첫지점에서  출발점까지 최단거리를 확인한다. 
//
//  * 조합을 만들때 방식은  정통적으로  AD  방식으로 DFS  방식으로 만들 수 있고 
//     STL:next_permutation() 함수로도 만들어도 된다. 
//     next_permutation() 함수를 사용할 경우 사전에 데이터를 정렬이 되어 있어야 한다. (정렬을 해야 된다) 
//   
//   * 시간적 여유가 있으면  PQ 를 손코딩을 변경하자 .  (0.78sec -->0.25sec)
//

//#include <queue>
#include <vector>
#include <algorithm>
using namespace std;
#define rint register int
#define INF  (0x2fffffff)  // 최대값  
#define MAX_DATA (10000)

//
// 손코딩 PQ, 시험때 제공되는 소텍 레퍼런스를  copy 해서 활용해도 된다. 
//
template <typename T>
struct priority_queue
{
	T heap[MAX_DATA];
	int sz;
	void clear() { sz = 0; }
	bool empty() { return sz == 0; }
	void push(T val)
	{
		int c = ++sz;
		for (; c > 0 && val < heap[c / 2]; c /= 2) heap[c] = heap[c / 2];
		heap[c] = val;
	}
	T top() { return heap[1]; }
	void pop()
	{
		T val = heap[sz--];
		int c = 2;
		for (; c <= sz; c *= 2)
		{
			if (c + 1 <= sz && heap[c + 1] < heap[c]) c++;
			if (heap[c] < val) heap[c / 2] = heap[c];
			else break;
		}
		heap[c / 2] = val;
	}
};
vector<pair<int, int>>edge[510]; //  dist, to_station
priority_queue<pair<int, int>>pq; // dist, to_station // MIN pq, STL 사용시 주의 하자. 

int N;
void init(int N, int K, int mRoadAs[], int mRoadBs[], int mLens[])
{
	::N = N;
	for (rint i = 1; i <= N; ++i)edge[i].clear();

	for (rint i = 0; i < K; ++i)
	{
		edge[mRoadAs[i]].push_back({ mLens[i],mRoadBs[i] });
		edge[mRoadBs[i]].push_back({ mLens[i],mRoadAs[i] }); //  양방향
	}
}

void addRoad(int mRoadA, int mRoadB, int mLen)
{
	edge[mRoadA].push_back({ mLen,mRoadB });
	edge[mRoadB].push_back({ mLen,mRoadA }); //  양방향
}

// 순열 조합 만들기  최대 120 개  5! = 5x4x3x2x1
int plist[121][5], sel[5]; // 순열 조합 리스트 
int pcnt, M, *stops, visit[5];
void perm(int n)
{
	if (n == M)
	{
		for (rint i = 0; i < M; ++i)
			plist[pcnt][i] = stops[sel[i]];
		pcnt++;
		return;
	}
	for (rint i = 0; i < M; ++i)
	{
		if (visit[i]) continue;
		visit[i] = 1;
		sel[n] = i;
		perm(n + 1);
		visit[i] = 0;
	}
}

int dist[510];
int map[510][510];

// start 지점에서 출발하는 다익스트라 구하기 단 st,ed  로 가는 path 는 제외  
// start 지점에서  출발점, 도착점을 제외한 모든 지점까지 최단거리가 결과이다(dist[])
void dijkstra(int start, int st, int ed)
{
	for (rint i = 1; i <= N; ++i) dist[i] = INF;
	dist[start] = 0;
	//pq = {};
	pq.clear();
	pq.push({ 0,start });
	while (!pq.empty())
	{
		auto it = pq.top(); pq.pop();
		int len = it.first, fm = it.second;
		if (dist[fm] < len) continue;
		if (fm == st || fm == ed) continue; // 중요!  fm  이 start, end 지점은  무시한다. 통과할 수 없다. 

		for (auto &to : edge[fm])
		{
			if (dist[to.second] > to.first + len) {
				dist[to.second] = to.first + len;
				pq.push({ dist[to.second], to.second }); //STL :pq 인 경우 작은 순으로 나와야 하기 때문에 - 를 붙인다. 
			}
		}
	}
}
void update_map(int pos)
{
	for (rint i = 1; i <= N; ++i) map[pos][i] = dist[i];
}
int findPath(int mStart, int mEnd, int M, int mStops[])
{
	stops = mStops;
	::M = M;
	pcnt = 0;
	perm(0); //순열 조합 만들기   결과 pcnt개   plist[][] 순열 조합 생성

#if 0
	// dfs::perm(0) 대신  STD: next_permutation() 이용 방법
	sort(mStops, mStops + M);
	do {
		for (rint i = 0; i < M; ++i) plist[pcnt][i] = mStops[i];
		pcnt++;
	} while (next_permutation(mStops, mStops + M));
#endif // 0

	//
	//  각  정류소 최대 5번의 dijkstra 최단 거리 구해 결과를  전체 정류간간 최대 거리 table  에 update 한다. 
	//
	for (rint i = 0; i < M; ++i)
	{
		dijkstra(mStops[i], mStart, mEnd);
		update_map(mStops[i]);
	}
	//
	// 각 순열 조합별  거리를 구하고 최단 거리를 구한다. 
	//
	//  시작점 --> { 순열 조합 } <---도착점 
	//
	rint min_dist = INF, i, j;

	for (i = 0; i < pcnt; ++i)
	{
		int cur = mStart, nx;
		int dist = 0;
		for (j = 0; j < M; ++j)
		{
			nx = plist[i][j];
			if (j == 0) { // 처음 start-> 첫번째 경로지점은 반대로 찾자 
				if (map[nx][cur] == INF) break;
				dist += map[nx][cur];
			}
			else {
				if (map[cur][nx] == INF) break; // 반드시  check 하자.... 속도 & INF  값을 넘을 수 있다. 
				dist += map[cur][nx];
			}
			cur = nx;
		}
		if (j < M) continue;
		if (map[cur][mEnd] == INF) continue;
		dist += map[cur][mEnd]; // 마지막엔 도착점에서 최종 경유지 확인한다.  
		min_dist = min(min_dist, dist); // 이번 조합이 최단 인지 확인한다. 
	}
	return min_dist == INF ? -1 : min_dist;
}
#elif 0
#include <vector>
#define min(x,y) ((x>y)?y:x)
#define pii pair<int, int>
#define INF 99999999
#define rint int
using namespace std;
vector<pii> v[501];
int val[7][7], stops[7];
int visit[501];
int N, ans;
struct Q {
	int x, c;
}que[25500];
void init(int N, int K, int mRoadAs[], int mRoadBs[], int mLens[])
{
	::N = N;
	for (rint i = 1; i <= N; i++) v[i].clear();
	for (rint i = 0; i < K; i++) {
		v[mRoadAs[i]].push_back({ mRoadBs[i], mLens[i] });
		v[mRoadBs[i]].push_back({ mRoadAs[i], mLens[i] });
	}
}

void addRoad(int mRoadA, int mRoadB, int mLen)
{
	v[mRoadA].push_back({ mRoadB, mLen });
	v[mRoadB].push_back({ mRoadA, mLen });
}

void bfs(int s, int M, int mStops[]) {
	rint wr = 0, rd = 0;
	for (rint i = 1; i <= N; i++)
		visit[i] = INF;

	visit[s] = 0;
	que[wr++] = { s, 0 };
	while (wr != rd) {
		auto t = que[rd++];
		if (t.c > 0 && (t.x == mStops[M - 1] || t.x == mStops[M - 2]))
			continue;

		for (auto y : v[t.x]) {
			int dist = t.c + y.second;
			if (visit[y.first] > dist)
				que[wr++] = { y.first, visit[y.first] = dist };
		}
	}
}

void dfs(int s, int c, int sum, int M) {
	if (c == M - 2) {
		ans = min(ans, sum + val[s][M - 1]);
		return;
	}
	for (rint i = 0; i < M - 2; i++) {
		if (stops[i]) {
			int tmp = stops[i];
			stops[i] = 0;
			dfs(i, c + 1, sum + val[s][i], M);
			stops[i] = tmp;
		}
	}
}
int findPath(int mStart, int mEnd, int M, int mStops[]) {
	ans = INF;
	for (rint i = 0; i < M; i++)
		stops[i] = mStops[i];

	stops[M++] = mStart;
	stops[M++] = mEnd;
	for (rint i = 0; i < M - 1; i++) {
		bfs(stops[i], M, stops);
		for (rint j = 0; j < M; j++)
			val[i][j] = visit[stops[j]];
	}
	dfs(M - 2, 0, 0, M);
	return ans == INF ? -1 : ans;
}
#elif 0
#include<iostream>
#include<queue>
#include<vector>
using namespace std;
#define NM 20'000
int m[NM][NM], vt[NM];
queue<int> q;
int bfs(int K) {
	q.push(K);
	vt[K] = 1;
	while (!q.empty()) {
		int c = q.front();
		q.pop();

		for (int i = 0; i < V; i++) {
			int n = m[c][i];
		}
	}
}
int V, E, K, u, v, w;
int main() {
	int ret;
	scanf("%d %d %d ", &V, &E, &K);
	for (int i = 0; i < E; i++) {
		scanf("%d %d %d ", &u, &v, &w);
		m[u][v] = m[v][u] = w;
	}
	ret = bfs(K);
	return ret;
	return 0;
}

#elif 0
#include<iostream>
#include<queue>
using namespace std;
priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
int main() {
	pq.push({ 500,200 });
	pq.push({ 300,200 });
	pq.push({ 500,100 });
	pq.push({ 200,500 });
	while (!pq.empty()) {
		cout << pq.top().first << " " << pq.top().second << endl;
		pq.pop();
	}
	return 0;
}
#elif 0
int getParent(int p[], int x) {
	if (p[x] == x)
		return x;
	else
		return p[x] = getParent(p, p[x]);
}
void unioParent(int p[], int a, int b) {
	a = getParent(p, a);
	b = getParent(p, b);
	if (a < b) p[b] = a;
	else p[a] = b;
}
int main() {
	int p[11];
	for (int i = 0; i <= 10; i++) {
		p[i] = i;
	}
	unionParent(p, 1, 2);
	unionParent(p, 2, 3);
	unionParent(p, 3, 4);
	unionParent(p, 3, 4);
}

#elif 0
#include<vector> 
#include<queue>
#define NM	5'000
#define INF	987654321 
using namespace std;
vector<pair<int, int>> v[NM];
int d[NM], vt[NM], N, b[NM];
void init(int N, int mBrands[]) {
	::N = N;
	memset(b, 0, sizeof(b));
	for (int i = 0; i < N; i++) {
		b[i] = mBrands[i];
	}
}
void connect(int mHotelA, int mHotelB, int mDistance) {
	v[mHotelA].push_back({ mHotelB, mDistance });
	v[mHotelB].push_back({ mHotelA, mDistance });
}
int merge(int mHotelA, int mHotelB) {
	int ret = 0;
	int brand = b[mHotelB];
	for (int i = 0; i < N; i++) {
		if (b[i] == brand)
			b[i] = b[mHotelA];
	}
	for (int i = 0; i < N; i++) {
		if (b[i] == b[mHotelA])
			ret++;
	}
	//printf("%s(%d, %d)\t=%d\n", __FUNCTION__, mHotelA, mHotelB,ret);
	return ret;
}
int move(int mStart, int mBrandA, int mBrandB) {
	int ret = 0;
	for (int i = 0; i < N; i++) { d[i] = INF, vt[i] = 0; }
	d[mStart] = 0; vt[mStart] = 1;
	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>>	pq;
	pq.push({ 0, mStart });
	while (!pq.empty()) {
		int dist_curr = pq.top().first;
		int curr = pq.top().second;
		vt[curr] = 1;
		pq.pop();

		if (dist_curr > d[curr]) continue;

		for (int i = 0; i < v[curr].size(); i++) {
			int dist_next = dist_curr + v[curr][i].second;
			int next = v[curr][i].first;

			if (!vt[next] && d[next] > dist_next) {
				d[next] = dist_next;
				pq.push({ dist_next, next });
			}
		}
	}

	return ret;

	int mv = INF, k = 0;
	for (int i = 0; i < N; i++) {
		if (i == mStart) continue;
		if (b[i] == mBrandA) {
			if (mv > d[i]) {
				mv = d[i];
				k = i;
			}
		}
	}
	ret += mv;
	d[k] = INF;
	mv = INF, k = 0;
	for (int i = 0; i < N; i++) {
		if (i == mStart) continue;
		if (b[i] == mBrandB) {
			if (mv > d[i]) {
				mv = d[i];
				k = i;
			}
		}
	}
	ret += mv;
	//printf("%s(%d, %d, %d)\t=%d\n", __FUNCTION__, mStart, mBrandA, mBrandA, ret);
	return ret;
}
#elif 0
//
//   Dijkstra (최대거리 찾기: PQ 사용) + Union_find 
//   
//   Node 가 max 5000 이기때문에 다익스트라 구현시  PQ 를 사용해야 속도가 난다. 
//   다익스트라를 다 돌릴 필요 없이 중간에 원하는 Brand 의 호텔을 모두 찾으면  중단한다. 
//   양방향이기 때문에 현재 위치에서 가는호텔과 오는 호텔 별도로 다익스트라를 구현할 필요없이 
//   한번 돌리면서 찾으면 된다. 
//
//   Union_find 는 가장 기본적인 형태로 brand 에 속한 호텔수만 잘 관리하면 된다. 
//    (단 호텔수와 관계없이 주어진 merge 조건으로 합쳐야 된다.  B에서 A 
//

#include <vector>
#include <queue>
using namespace std;
#define rint register int
#define INF (0x2fffffff)

int bparent[55]; //brand parent for union_fion
int bcount[55];// brand 에 속한 호텔수 
// union find  brand 부모 찾기 
int find(int id)
{
	if (bparent[id] == id) return id;
	return bparent[id] = find(bparent[id]);
}

struct Hotel
{
	int bid; // brand id; 
	vector<pair<int, int>>edge; //도로 정보  <to_hotel_id , distance>
	void init(int _bid)
	{
		bid = _bid;
		edge.clear();
	}
}H[5010];

// 다익스트라 기본 변수 
priority_queue<pair<int, int>>pq; // for dijkstra  <distance, to_hotel_id> 
int dist[5010]; // 특정 지점에 모든 지점사이의 최단거리가 기록된다. 
int visit[5010];
int vc;
int N; // 호텔수 

void init(int N, int mBrands[])
{
	::N = N;
	for (rint i = 0; i < 50; ++i) {
		bparent[i] = i; // union find 초기화  자신이 부모이다. 
		bcount[i] = 0; // brand 별 호텔 수 
	}
	for (rint i = 0; i < N; ++i)
	{
		H[i].init(mBrands[i]);
		bcount[mBrands[i]]++; // brand 호텔수 추가 
	}
}

void connect(int mHotelA, int mHotelB, int mDistance)
{
	// 양방향 이기 때문에 양쪽으로 add 한다. 
	H[mHotelA].edge.push_back({ mHotelB,mDistance });
	H[mHotelB].edge.push_back({ mHotelA,mDistance });
}

int merge(int mHotelA, int mHotelB)
{
	int ba = find(H[mHotelA].bid), bb = find(H[mHotelB].bid);
	if (ba == bb) return bcount[ba]; // 같은 brand 인 경우 
	bparent[bb] = ba; // B --> A 로 합친다.
	bcount[ba] += bcount[bb]; // B 의 호텔이 A 호텔로 합친다. 
	return bcount[ba];
}

int move(int mStart, int mBrandA, int mBrandB)
{
	int res = 0;
	pq = {};
	mBrandA = find(mBrandA);
	mBrandB = find(mBrandB);
	vc++;
	for (rint i = 0; i < N; ++i)dist[i] = INF;
	dist[mStart] = 0;
	pq.push({ 0,mStart });
	int find_a = 0, find_b = 0; // a,b brand search 유무

	while (!pq.empty())
	{
		auto it = pq.top(); pq.pop(); // 현재 지점에서 가장 가까운 hotel 을 찾는다. 
		int distance = -it.first, hid = it.second;// hotel id
		if (visit[hid] == vc) continue;
		visit[hid] = vc;

		int bid = find(H[hid].bid); // cur node brand id 

		if (hid != mStart && !find_a && bid == mBrandA) {
			find_a = 1;
			res += dist[hid];
		}
		else if (hid != mStart && !find_b &&bid == mBrandB) {
			find_b = 1;
			res += dist[hid];
		}
		if (find_a*find_b) break; // 모두 찾았다면  break 한다. 
		// 간선 정보를 이용하여 dist 정보의 최소값을 찾아 update 한다. 
		for (auto ed : H[hid].edge) {
			int to_hid = ed.first, to_dist = ed.second;
			if (visit[to_hid] != vc && dist[to_hid] > dist[hid] + to_dist)
			{
				dist[to_hid] = dist[hid] + to_dist;
				pq.push({ -dist[to_hid], to_hid });
			}
		}
	}
	return res;
}
#elif 0
#include <queue>
#include <vector>
#include <functional>
#define INF 0x7fff'ffff
#define pii pair<int, int>
using namespace std;
vector<pii> v[5000];
int group[5000];
int visit[5000];
int par[5000];
int cnt[5000];
int N;

priority_queue<pii, vector<pii>, greater<pii>> pq;

int find_p(int x) { return par[x] == x ? x : par[x] = find_p(par[x]); }

void init(int N, int mBrands[])
{
	::N = N;
	for (int i = 0; i < 50; i++) cnt[i] = 0;
	for (int i = 0; i < N; i++) {
		v[i].clear();
		group[i] = par[mBrands[i]] = mBrands[i];
		cnt[mBrands[i]]++;
	}
}

void connect(int mHotelA, int mHotelB, int mDistance)
{
	v[mHotelA].push_back({ mHotelB, mDistance });
	v[mHotelB].push_back({ mHotelA, mDistance });
}

int merge(int mHotelA, int mHotelB)
{
	int A = find_p(group[mHotelA]), B = find_p(group[mHotelB]);
	if (A != B) {
		par[B] = A;
		cnt[A] += cnt[B];
	}
	return cnt[A];
}

int move(int mStart, int mBrandA, int mBrandB)
{
	for (int i = 0; i < N; i++) visit[i] = INF;
	int ans = 0, flag = 0;
	pii t = { 0, mStart };
	visit[mStart] = 0;
	pq = {};
	while (flag != 3) {
		for (auto x : v[t.second]) {
			if (visit[x.first] > t.first + x.second) {
				visit[x.first] = t.first + x.second;
				pq.push({ visit[x.first], x.first });
			}
		}
		visit[t.second] = -1;
		while (!pq.empty() && visit[t.second] != t.first) {
			t = pq.top(); pq.pop();
		}

		int G = find_p(group[t.second]);
		if (G == mBrandA && flag != 1) {
			ans += visit[t.second];
			flag += 1;
		}
		else if (G == mBrandB && flag != 2) {
			ans += visit[t.second];
			flag += 2;
		}
	}

	return ans;
}
#elif 0
#include<iostream>
#define NM 5'000
#define INF 987654321
int m[NM][NM], b[NM], N;
void init(int N, int mBrands[]) {
	::N = N;
	for (int i = 0; i < N; i++) {
		b[i] = mBrands[i];
		for (int j = 0; j < N; j++) {
			if (i == j)
				m[i][j] = 0;
			else
				m[i][j] = INF;
		}
	}
}
void connect(int mHotelA, int mHotelB, int mDistance) {
	m[mHotelA][mHotelB] = mDistance;
	m[mHotelB][mHotelA] = mDistance;
	//printf("%s(%d, %d, %d)\n", __FUNCTION__, mHotelA, mHotelB, mDistance);
}
int merge(int mHotelA, int mHotelB) {
	int ret = 0;
	int brand = b[mHotelB];
	for (int i = 0; i < N; i++) {
		if (b[i] == brand)
			b[i] = b[mHotelA];
	}
	for (int i = 0; i < N; i++) {
		if (b[i] == b[mHotelA])
			ret++;
	}
	//printf("%s(%d, %d)\t=%d\n", __FUNCTION__, mHotelA, mHotelB,ret);
	return ret;
}
int d[NM], v[NM];
void dijkstra(int start) {
	for (int i = 0; i < N; i++) {
		d[i] = INF, v[i] = 0;
	}
	d[start] = 0;

	int mv, k;
	for (int i = 0; i < N; i++) {
		mv = INF, k = 0;
		for (int j = 0; j < N; j++) {
			if (!v[j] && mv > d[j]) {
				mv = d[j];
				k = j;
			}
		}
		v[k] = 1;
		for (int j = 0; j < N; j++) {
			if (d[j] > d[k] + m[k][j]) {
				d[j] = d[k] + m[k][j];
			}
		}
	}
}
int move(int mStart, int mBrandA, int mBrandB) {
	int ret = 0;
	dijkstra(mStart);

	int mv = INF, k = 0;
	for (int i = 0; i < N; i++) {
		if (i == mStart) continue;
		if (b[i] == mBrandA) {
			if (mv > d[i]) {
				mv = d[i];
				k = i;
			}
		}
	}
	ret += mv;
	d[k] = INF;
	mv = INF, k = 0;
	for (int i = 0; i < N; i++) {
		if (i == mStart) continue;
		if (b[i] == mBrandB) {
			if (mv > d[i]) {
				mv = d[i];
				k = i;
			}
		}
	}
	ret += mv;
	//printf("%s(%d, %d, %d)\t=%d\n", __FUNCTION__, mStart, mBrandA, mBrandA, ret);
	return ret;
}
#elif 0
#define MAX_N 350
int(*m)[MAX_N], R;
struct S { int id, r, c; } s[200]; int idx;
void init(int N, int mRange, int mMap[MAX_N][MAX_N]) {
	::m = mMap, ::R = mRange;

}
#include<queue>
using namespace std;
queue<pair<int, int>> q;
int v[MAX_N + 1][MAX_N + 1]; int dx[4] = { -1, 0, 1, 0 }; int dy[4] = { 0, -1, 0, 1 };
int ts;
int bfs(int f, int t) {
	int cnt = 0;
	q.push({ s[f].r, s[t].c });
	v[s[f].r][s[t].c] = ++ts;
	while (!q.empty()) {
		int y = q.front().first; int x = q.front().second;
		q.pop();

		if (y == s[t].r && x == s[t].c) {
			return cnt;
		}

		for (int i = 0; i < 4; i++) {
			int ny = y + dy[i];	int nx = x + dx[i];
			if (v[ny][nx] == ts) {
				q.push({ ny,nx });
				v[ny][nx] = 1;
				cnt++;
			}
		}
	}
	return -1;
}

void add(int mID, int mRow, int mCol)
{
	s[mID] = { mID, mRow, mRow };
	m[mRow][mCol] = -(mID + 1);
}
int distance(int mFrom, int mTo)
{
	int ret = 0;
	ret = bfs(mFrom, mTo);
	return ret;
}
#elif 0

#include<iostream>
using namespace std;
#define NM 100
#define KM 500
int m[NM + 1][NM + 1];
int N;
void init(int N, int K, int sCity[], int eCity[], int mLimit[]) {
	::N = N;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (i == j)
				m[i][j] = { };
			else
				m[i][j] = 987654321;
		}
	}
	for (int i = 0; i < K; i++) {
		m[sCity[i]][eCity[i]] = { mLimit[i] };
	}
}
void add(int sCity, int eCity, int mLimit) {
	m[sCity][eCity] = mLimit;
}
int d[NM], v[NM];
void dijkstra(int s) {
	int i;	int j;	int minv;	int k;

	for (int i = 0; i < N; i++) {
		d[i] = 987654321, v[i] = 0;
	}
	d[s] = 0;

	for (i = 0; i < N; i++) {
		minv = 987654321;
		for (j = 0; j < N; j++) {
			if (v[j] == 0 && minv > d[j]) {
				minv = d[j];
				k = j;
			}
		}
		v[k] = 1;
		for (j = 0; j < N; j++) {

			if ()
				/*
				if (d[j] > d[k] + m[k][j] ) {
					d[j] = d[k] + m[k][j];
				}
				*/
		}
	}
}
int calculate(int sCity, int eCity) {
	int ret = -1;
	dijkstra(sCity);

	if (d[eCity] < 987654321)
		ret = d[eCity];

	printf("(%s) : %d\n", __FUNCTION__, ret);

	return ret;
}
#elif 0
#include<unordered_map>
using namespace std;
unordered_map<int, int> h; int idx;
const int NMAX = 600;
int m1[NMAX + 1][NMAX + 1], m2[NMAX + 1][NMAX + 1];

int init(int N, int sCity[], int eCity[], int mCost[]) {
	idx = 0;
	for (int i = 0; i < NMAX; i++) {
		for (int j = 0; j < NMAX; j++) {
			if (i == j)
				m1[i][j] = 0, m2[i][j] = 0;
			else
				m1[i][j] = 987654321, m2[i][j] = 987654321;
		}
	}
	for (int i = 0; i < N; i++) {
		int s, e;
		if (!h.count(sCity[i])) {
			h[sCity[i]] = idx++;
		}
		if (!h.count(eCity[i])) {
			h[eCity[i]] = idx++;
		}
		s = h[sCity[i]];
		e = h[eCity[i]];
		m1[s][e] = mCost[i];
		m2[e][s] = mCost[i];
	}
	//printf("(%s) %d\n", __FUNCTION__, idx);
	return idx;
}
void add(int sCity, int eCity, int mCost) {
	int s, e;
	if (!h.count(sCity)) {
		h[sCity] = idx++;
		s = idx;
	}
	if (!h.count(eCity)) {
		h[eCity] = idx++;
		e = idx;
	}
	s = h[sCity];
	e = h[eCity];
	m1[s][e] = mCost;
	m2[e][s] = mCost;
}
/*
void floyd(int n) {
	int i, j, k;
	for (k = 0; k < n; k++) {
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				if (m[i][k] + m[k][j] < m[i][j]) {
					m[i][j] = m[i][k] + m[k][j];
				}
			}
		}
	}
}
*/
int d[NMAX + 1], v[NMAX + 1];
void dijkstra(int n, int s, int(&m)[NMAX + 1][NMAX + 1]) {
	int k;
	for (int i = 0; i < n; i++)
		d[i] = 987654321, v[i] = 0;

	d[s] = 0;
	for (int i = 0; i < n; i++) {
		int min = 987654321;
		for (int j = 0; j < n; j++) {
			if (v[j] == 0 && min > d[j]) {
				min = d[j];
				k = j;
			}
		}
		v[k] = 1;
		for (int j = 0; j < n; j++) {
			if (d[j] > d[k] + m[k][j])
			{
				d[j] = d[k] + m[k][j];
			}
		}
	}
}
int cost(int mHub) {
	int ret = 0;
	int hb = h[mHub];
	//floyd(idx);

	dijkstra(idx, hb, m1);
	for (int i = 0; i < idx; i++) {
		ret += d[i];
	}
	dijkstra(idx, hb, m2);
	for (int i = 0; i < idx; i++) {
		ret += d[i];
	}
	//printf("(%s) %d\n", __FUNCTION__ , ret);
	return ret;
}
#elif 0

#include <stdio.h>
#define INFINITY 999999

int r[101][101];

void floyd(int n) {
	int i, j, k;
	for (k = 0; k < n; k++) {
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				if (r[i][k] + r[k][j] < r[i][j]) {
					r[i][j] = r[i][k] + r[k][j];
				}
			}
		}
	}
}

int main() {
	int T;	int n, m, i, j;
	scanf("%d", &T);

	for (int test_case = 1; test_case <= T; test_case++) {
		scanf("%d %d", &n, &m);
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				r[i][j] = INFINITY;
			}
			r[i][i] = 0;
		}

		for (i = 0; i < m; i++) {
			int st, en, wt;
			scanf("%d %d %d", &st, &en, &wt);
			if (r[st - 1][en - 1] > wt)
			{
				r[st - 1][en - 1] = wt;
			}
		}

		floyd(n);

		printf("#%d\n", test_case);
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				printf("%d ", r[i][j]);
			}
			printf("\n");
		}
	}
}
#elif 
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
//#include <algorithm>
using namespace std;
#define mymin(a,b)	(a)>(b)?(b):(a)

#define N 100
#define INF 100000

int map[N + 1][N + 1];
int visit[N + 1];
int dist[N + 1];
int vertex;
int edge;
int start;
int end;

void dijkstra() {
	dist[start] = 0;
	int k = vertex;
	while (k-- > 0) {

		int v;
		int minv = 987654321;
		for (int i = 1; i <= vertex; i++) {
			if (visit[i] == 0 && dist[i] < minv) {
				minv = dist[i];
				v = i;
			}
		}

		visit[v] = 1;

		for (int j = 1; j <= vertex; j++) {

			if (dist[j] > dist[v] + map[v][j]) {
				dist[j] = dist[v] + map[v][j];
			}
		}
	}
}

int main(void)
{
	int test_case;	int T;	int i;	int j;	int from;	int to;	int value;
	scanf("%d", &T);

	for (test_case = 1; test_case <= T; test_case++)
	{
		scanf("%d %d %d", &vertex, &start, &end);		scanf("%d", &edge);
		for (i = 1; i <= vertex; i++) {
			for (j = 1; j <= vertex; j++) {
				if (i != j) {
					map[i][j] = INF;
				}
			}
		}

		for (i = 1; i <= edge; i++) {
			scanf("%d %d %d", &from, &to, &value);
			map[from][to] = value;
		}

		for (i = 1; i <= vertex; i++) {
			dist[i] = INF;
			visit[i] = 0;
		}

		printf("#%d ", test_case);
		dijkstra();
		printf("%d \n", dist[end]);
	}
	return 0;
}

#elif 0
#include<iostream>
using  namespace std;
const int NMAX = 10'000;
struct R {
	int y, x, h, w, e;
}r[NMAX]; int layer;
void init(int N) {
	layer = 0;
}
void addRect(int mID, int mY, int mX, int mHeight, int mWidth) {
	r[layer++] = { mID, mY, mX, mHeight, mWidth };
}
int select(int mY, int mX) {
	for (int i = layer; i >= 0; i--) {
		if (!r[i].e)
			continue;
		else {
			if (r[i].y <= mY && mY <= r[i].y + r[i].h && r[i].x <= mX && mX <= r[i].x + r[i].w) {

			}
		}
	}

}
void selectAndMove(int y1, int x1, int y2, int x2) {

}
int moveFront(int mID) {

}
int selectAndErase(int mY, int mX) {

}
#elif 0
#include<iostream>
#include<memory.h>
#include<algorithm>
using  namespace std;
const int NMAX = 50'000;
const int BS = 500;
struct T { int r, c, color, exist; }t[NMAX]; int idx;
struct B { int r, l; c, color, exist; }t[NMAX]; int idx;

int N;
void init(int N) {
	::N = N;
	idx = 0;
}
void buildTower(int mRow, int mCol, int mColor) {
	t[idx++] = { mRow, mCol, mColor, true };
}
void removeTower(int mRow, int mCol) {
	for (int i = 0; i < idx; i++) {
		if (t[i].r == mRow && t[i].c == mCol)
			t[i].exist = 0;
	}
}
int countTower(int mRow, int mCol, int mColor, int mDis) {
	if (mRow == 2 && mCol == 2 && mColor == 0 && mDis == 3)
		while (0);
	int ret = 0;
	for (int i = 0; i < idx; i++) {
		int h = abs(t[i].r - mRow);
		int w = abs(t[i].c - mCol);
		if (h <= mDis && w <= mDis && (t[i].color == mColor || mColor == 0) && t[i].exist)
			ret++;
	}
	//printf("%d %s\n", ret, __FUNCTION__);
	return ret;
}
int getClosest(int mRow, int mCol, int mColor) {
	int ret = 0; int minv = 987654321;
	for (int i = 0; i < idx; i++) {
		if ((t[i].color == mColor || mColor == 0) && t[i].exist) {
			int distance = abs(t[i].r - mRow) + abs(t[i].c - mCol);
			minv = min(minv, distance);
		}
	}
	if (minv == 987654321)
		ret = -1;
	else
		ret = minv;

	//printf("%d %s\n", ret, __FUNCTION__);
	return ret;
}
#elif 0
#include<iostream>
#include<memory.h>
using  namespace std;
const int NMAX = 100'000;
const int BUCKET_SIZE = 4;
int type[NMAX];
int _time[NMAX];
int b[NMAX / BUCKET_SIZE];
int N;
void init(int N, int M, int mType[], int mTime[]) {
	::N = N;
	memcpy(type, mType, N * 4);
	memcpy(_time, mTime, N * 4);
	memset(b, 0, sizeof(b));

	for (int i = 0; i < N; i++) {
		b[i / BUCKET_SIZE] += mTime[i];
	}
}
void destroy() {

}
void update(int mID, int mNewTime) {
	int _old = _time[mID];
	int _new = mNewTime;
	int diff = _old - _new;
	b[mID / BUCKET_SIZE] -= diff;
	_time[mID] = mNewTime;
}
int updateByType(int mTypeID, int mRatio256) {
	int ret = 0;
	for (int i = 0; i < N; i++) {
		if (type[i] == mTypeID) {
			int _old = _time[i];
			int _new = (_time[i] * mRatio256) / 256;
			int diff = _old - _new;
			b[i / BUCKET_SIZE] -= diff;
			_time[i] = _new;
			ret += _time[i];
		}
	}
	//printf("%d %s\n", ret, __FUNCTION__);
	return ret;
}
int calculate(int mA, int mB) {
	int ret = 0;
	if (mA > mB)	swap(mA, mB);
	int s = mA / BUCKET_SIZE;
	int e = mB / BUCKET_SIZE;

	if (e - s >= 2) {
		for (int i = mA; i < (s + 1)*BUCKET_SIZE; i++) {
			ret += _time[i];
		}
		for (int i = s + 1; i <= e - 1; i++) {
			ret += b[i];
		}
		for (int i = e * BUCKET_SIZE; i < mB; i++) {
			ret += _time[i];
		}
	}
	else {
		for (int i = mA; i < mB; i++) {
			ret += _time[i];
		}
	}

	//printf("%d %s\n", ret, __FUNCTION__);
	return ret;
}
#elif 0
#include<algorithm>
using namespace std;
#define INF 987654321
#define SIZE 400
#define B 9 
struct Q {
	int cnt, MIN, MAX;
	int num[512];
}bucket[SIZE];
int id;

void add(int M, int mValue[]) {
	int p;
	//while (i < M) {
	for (int i = 0; i < M; i++) {
		p = id / 512;
		bucket[p].num[bucket[p].cnt++] = mValue[i];
		bucket[p].MIN = min(bucket[p].MIN, mValue[i]);
		bucket[p].MAX = max(bucket[p].MAX, mValue[i]);
		++id;
	}
}

void init(int N, int mValue[])
{
	for (int i = 0; i < SIZE; i++) {
		bucket[i].cnt = bucket[i].MAX = 0;
		bucket[i].MIN = INF;
	}
	id = 0;
	add(N, mValue);
}

void update(int p) {
	bucket[p].MIN = INF; bucket[p].MAX = 0;
	for (int i = 0; i < bucket[p].cnt; i++) {
		bucket[p].MIN = min(bucket[p].MIN, bucket[p].num[i]);
		bucket[p].MAX = max(bucket[p].MAX, bucket[p].num[i]);
	}
}

void erase(int mFrom, int mTo)
{
	--mFrom; --mTo;
	int p = 0, j = 0, d = mTo - mFrom + 1;
	for (int i = 0; i <= (id / 512); i++) {

		if (mFrom < p + bucket[i].cnt)
			break;

		p += bucket[i].cnt;
	}
	while (p < mFrom) ++j, ++p;

	while (bucket[i].cnt - j < d) {
		d -= bucket[i].cnt - j;
		bucket[i].cnt -= bucket[i].cnt - j;
		update(i);
		++i; j = 0;
	}
	bucket[i].cnt -= d;
	for (; j < bucket[i].cnt; j++) bucket[i].num[j] = bucket[i].num[j + d];
	update(i);
}

int find(int K)
{
	int i = (id >> B), p = 0, s = INF, e = 0;
	for (; i >= 0; i--) {
		if (K < p + bucket[i].cnt) break;
		p += bucket[i].cnt;
		s = min(bucket[i].MIN, s);
		e = max(bucket[i].MAX, e);
	}
	if (p != K) {
		for (int j = bucket[i].cnt - 1; j >= 0; j--) {
			s = min(bucket[i].num[j], s);
			e = max(bucket[i].num[j], e);
			if (++p == K) break;
		}
	}
	return e - s;
}

#elif 0
#include<iostream>
#include<algorithm>
#include<list>
using  namespace std;

list<int> l;
void add(int M, int mValue[]) {
	for (int i = 0; i < M; i++)
		l.push_back(mValue[i]);
}
void init(int N, int mValue[]) {
	l.clear();
	add(N, mValue);
}
void erase(int mFrom, int mTo) {
	auto f = next(l.begin(), mFrom - 1);
	auto t = next(l.begin(), mTo);
	l.erase(f, t);
}
int find(int K) {
	int k = 0; int minv = 987654321; int maxv = 0;
	for (auto i = l.rbegin(); i != l.rend(); i++)
	{
		if (k == K)
			break;
		minv = min(minv, *i);
		maxv = max(maxv, *i);
		k++;
	}
	return maxv - minv;
}
#elif 0 

#include<iostream>
using  namespace std;
int N, M, a[30'005];
int twopointer(int a[], int len) {
	int ret = 0;
	int s, e, sum;
	s = e = sum = 0;
	while (s < len) {
		if (sum >= M) {
			sum -= a[s];
			s++;
		}
		else {
			sum += a[e];
			e++;
		}
		if (sum == M)
			ret++;
	}
	return ret;
}
int main() {
	cin >> N >> M;
	for (int i = 0; i < N; i++) {
		cin >> a[i];
	}
	cout << twopointer(a, sizeof(a) / sizeof(a[0]));
	return 0;
}

#elif 0
#include <iostream>
#include <vector>

using namespace std;

vector<int> segTree;

// 세그먼트 트리 구성 함수
void buildTree(const vector<int>& arr, int node, int start, int end) {
	cout << __FUNCTION__ << ", ---  node: " << node << ", start: " << start << ", end: " << end << endl;
	if (start == end) {

		segTree[node] = arr[start];

		/*
		//for (auto &i : segTree) {
		for (int i = 0; i < sizeof(segTree) / sizeof(segTree[0]); i++) {
			cout << "[" << i << "] = " << segTree[i]<< "   ";
		}cout << endl;
		*/
	}
	else {
		int mid = (start + end) / 2;
		buildTree(arr, 2 * node, start, mid);
		buildTree(arr, 2 * node + 1, mid + 1, end);
		//segTree[node] = segTree[2 * node] + segTree[2 * node + 1];
	}
}

// 구간 합 계산 함수
int query(const vector<int>& arr, int node, int start, int end, int left, int right) {
	if (right < start || left > end) return 0; // 범위 밖에 있는 경우
	if (left <= start && end <= right) return segTree[node]; // 현재 노드가 찾는 구간 안에 있는 경우

	int mid = (start + end) / 2;
	return query(arr, 2 * node, start, mid, left, right) + query(arr, 2 * node + 1, mid + 1, end, left, right);
}

int main() {
	vector<int> arr = { 1, 2, 3 };// 5, 7, 9, 11 
	int n = arr.size();

	// 세그먼트 트리 배열의 크기는 4 * N으로 설정
	segTree.resize(4 * n);

	// 세그먼트 트리 구성
	buildTree(arr, 1, 0, n - 1);

	// 특정 구간의 합 계산 예시
	int sum = query(arr, 1, 0, n - 1, 1, 3);
	cout << "Sum of elements from index 1 to 3: " << sum << endl;

	return 0;
}



#elif 0
#include<iostream>
using namespace std;

unsigned char mInImage[100] = "aa1aaaaabb2bbccc3ccdd4ddeeeeeeeefff;dkfsa;lkdjfsal;dkfas;lkdfjsaaaaaaaaffffffffdddddddfffffldfjk";
unsigned char mOutImage[100];
char Map[100][100];

int H = 4;
int W = 7;

void print() {
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			cout << Map[i][j];
		}
		cout << endl;
	}
}

unsigned t[100][100];
void rotate(unsigned char mOutImage[]) {

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			t[]
		}
	}
}
int main() {

	cout << mInImage << endl;

	for (int i = 0; i < H*W; i++)
		Map[i / W][i%W] = mInImage[i];

	print();

	for (int i = 0; i < H*W; i++)
		mOutImage[i] = Map[i / W][i%W];

	//cout << mOutImage << endl;

	rotate(mOutImage);

	/*
	unsigned char *ptr = mInImage;
	int k = 0;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Map[i][j] = *ptr++;
		}
	}
	*/

	for (int i = 0; i < H*W; i++)	Map[i / W][i%W] = mInImage[i];




	return 0;
}


#elif 0
#include <queue>
#define N 1000
#define pii pair<int, int>
#define rint register int
using namespace std;


int W, H, ts;
char Map[N][N], cp[N][N];
int visit[N][N];
int arr[N];
int cntM[128], bits[128];
int dx[] = { 1, 0, -1, 0 };
int dy[] = { 0, 1, 0, -1 };

struct TREE {
	int num;
	TREE *L, *R;
}tree[N];

struct Q {
	int x, y;
	TREE * cur;
	bool operator<(const Q &a) const { return a.x != x ? a.x < x : a.y < y; }
}q[N*N + 5];


priority_queue<Q> pq;

void copy(unsigned char Image[]) {
	rint k = 0;
	for (rint i = 0; i < H; i++)
		for (rint j = 0; j < W; j++)
			Image[k++] = Map[i][j];
}

void init(int mWidth, int mHeight, unsigned char mInImage[]) {
	H = mHeight;
	W = mWidth;
	for (rint i = 0; i < H*W; i++) Map[i / W][i%W] = mInImage[i];
}

void rect(int mY, int mX, int mW, int mH, unsigned char mColor, unsigned char mOutImage[])
{
	rint i, j, eX = mX + mH, eY = mY + mW;
	for (i = mX; i < eX; i++)
		for (j = mY; j < eY; j++)
			Map[i][j] = mColor;

	copy(mOutImage);
}

void rotate(unsigned char mOutImage[])
{
	rint i, j, k = 0;
	swap(W, H);
	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++)
			cp[i][j] = Map[W - j - 1][i];

	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++)
			mOutImage[k++] = Map[i][j] = cp[i][j];
}

void paintBucket(int mY, int mX, unsigned char mColor, unsigned char mOutImage[])
{
	rint wr = 0, rd = 0, k = 0, val = Map[mX][mY];
	q[wr++] = { mX, mY };
	Map[mX][mY] = mColor;
	visit[mX][mY] = ++ts;
	while (wr != rd) {
		auto t = q[rd++];
		for (rint i = 0; i < 4; i++) {
			rint x = t.x + dx[i], y = t.y + dy[i];
			if (x < 0 || y < 0 || x == H || y == W || visit[x][y] == ts) continue;
			if (Map[x][y] == val) {
				Map[x][y] = mColor;
				visit[x][y] = ts;
				q[wr++] = { x, y };
			}
		}
	}
	copy(mOutImage);
}

void dfs(TREE * cur, int val, int c) {
	if (!cur) return;
	if (!cur->L && !cur->R) {
		cntM[cur->num] = val;
		bits[cur->num] = c;
		return;
	}
	else {
		dfs(cur->L, val << 1, c + 1);
		dfs(cur->R, (val << 1) + 1, c + 1);
	}
}

int save(unsigned char mOutHuffData[]) {
	rint i, j, id = 0, len = 0;
	for (i = 0; i < 128; i++) cntM[i] = bits[i] = 0;
	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++) {
			if (cntM[Map[i][j]]++ == 0) arr[len++] = Map[i][j];
		}
	pq = {};
	for (i = 0; i < len; i++) {
		tree[++id] = { arr[i] };
		pq.push({ cntM[arr[i]], arr[i], &tree[id] });
	}
	TREE * root = 0;
	while (pq.size() > 1) {
		auto L = pq.top(); pq.pop();
		auto R = pq.top(); pq.pop();
		tree[++id] = { min(L.y, R.y), L.cur, R.cur };
		pq.push({ L.x + R.x, min(L.y, R.y), root = &tree[id] });
	}

	dfs(root, 0, 0);

	rint ans = 0, idx = 0, bit = 0;
	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++) {
			rint v = cntM[Map[i][j]], c = bits[Map[i][j]];
			while (c) {
				mOutHuffData[idx] = (mOutHuffData[idx] << 1) + ((v >> --c) & 1);
				++ans;
				if (++bit == 8) {
					bit = 0;
					mOutHuffData[++idx] = 0;
				}
			}
		}
	return ans;
}
#elif 0
#include <set>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#define pii pair<int, int>
using namespace std;

pair<int, int>	p[6];

struct compare_s {
	const bool operator()(const pair<int, int> &lhs, const pair<int, int> &rhs) const {
		return lhs.first != rhs.first ? lhs.first > rhs.first : lhs.second < rhs.second;
	}
};


bool compare_f(const pair<int, int> &lhs, const pair<int, int> &rhs) {
	return lhs.first != rhs.first ? lhs.first > rhs.first : lhs.second < rhs.second;
};


int main() {

	p[0] = { 10,20 };
	//p[1]( 20,10 );
	p[1] = make_pair(20, 10);
	p[2] = make_pair(10, 50);
	p[3] = make_pair(20, 20);
	p[4] = make_pair(60, 50);
	p[5] = make_pair(1, 5);

	// 비교 구조체를 사용했으며 
	// operator() 연산자를 오버로딩해서 사용하는 방식이므로 
	// 구조체이름 뒤에 operator()를 추가 했다.
	// 반면에 만일 비교 함수를 사용할 경우에는 다음과 같이 사용한다.
	// bool compare(const T&a, const T&b){
	//		reture 비교로직 ;
	// }
	// 이러한 비교 함수를 사용시에는 함수 포인터가 전달되어 연산이 되므로 operator()가 필요 없다(넣으면 안된다)

	pair<int, int> t[6];

	copy(p, p + 6, t);
	sort(t, t + 6, less<pair<int, int>>());
	for (auto &i : t) { cout << i.first << "\t" << i.second << endl; } cout << endl;

	copy(p, p + 6, t);
	sort(t, t + 6, less<>());
	for (auto &i : t) { cout << i.first << "\t" << i.second << endl; } cout << endl;

	copy(p, p + 6, t);
	sort(t, t + 6, greater<pair<int, int>>());
	for (auto &i : t) { cout << i.first << "\t" << i.second << endl; } cout << endl;

	copy(p, p + 6, t);
	sort(t, t + 6, greater<>());
	for (auto &i : t) { cout << i.first << "\t" << i.second << endl; } cout << endl;

	copy(p, p + 6, t);
	sort(t, t + 6, compare_s());
	for (auto &i : t) { cout << i.first << "\t" << i.second << endl; } cout << endl;

	copy(p, p + 6, t);
	sort(t, t + 6, compare_f);
	for (auto &i : t) { cout << i.first << "\t" << i.second << endl; } cout << endl;

	return 0;
}

#elif 0
#include <set>
#include <algorithm>
#include <unordered_map>
#define pii pair<int, int>
using namespace std;
int C, N, id;
struct STU {
	int pos;
	// 핵심 자료구조!!!
	// distance, 학교 ID 를 가진 pair<int,int> 형태의 배열을 학교 개수만큼 가진다.
	// 즉, 학생 한명을 기준으로 학교 개수만큼(최대 10개)의 <거리, 학교ID> 정보를 넣어둔다는 것이다.
	pair<int, int> sc[10];
}student[10005];

struct SC {
	int x, y;
}school[10];

// 각 학교에 할당된 학생
set<pair<int, int>> S[10];
set<pair<int, int>> W[10];
unordered_map<int, int> sid;

void init(int C, int N, int mX[], int mY[]) {

	// 학교에 최대 수용가능한 학생수를 전역 변수로
	// 총 학교의 수를 전역 변수로
	::C = C; ::N = N;

	// 학교에 대한 구조체를 모두 초기화 
	for (int i = 0; i < N; i++) {
		school[i] = { mX[i], mY[i] };
		S[i].clear();
		W[i].clear();
	}

	// 학생 ID에 대한 해싱도 초기화
	sid.clear();
	id = 0;
}

void update(int p, int mid) {
	// 학생 ID로 접근하여 학교와의 거리 정보를 가져온다. 이 값은 오름차순(less)로 정렬되어 있다. 
	int val = student[p].sc[N - 1].first;

	// p 라는 학생을 기준을 10개 학교를 뒤진다. 
	for (int k = 0; k < N; k++) {
		// 학생기준으로 학교 정보(거리, 학교ID)를 가지고 온다.
		pair<int, int> i = student[p].sc[k];
		// i.first는 distance		
		// i.second는 학교 ID 

		int distance = i.first;
		int schoolID = i.second;

		if (S[schoolID].size() < C) {
			// 학생 배정 정보를 그 학교ID로 넣는다. 
			student[p].pos = schoolID;
			// S는 각 학교에 할당된 학생의 정보를 저장한다.
			// set<int,int> 거리와 id정보..
			// 그런 mID에 - 처리한 이유는????
			S[schoolID].insert({ val, -mid });
			return;
		}
		else {
			pair<int, int> cur = *S[schoolID].begin();
			if (cur.first < val || (cur.first == val && mid < -cur.second)) {
				S[schoolID].erase(cur);
				S[schoolID].insert({ val, -mid });
				student[p].pos = schoolID;
				update(sid[-cur.second], -cur.second);
				return;
			}
			else W[schoolID].insert({ -val, mid });
		}
	}
}

void rm_update(int p) {
	if (!W[p].empty()) {
		pii cur = *W[p].begin();
		W[p].erase(cur);
		int cid = sid[cur.second];
		int Prev = student[cid].pos;
		S[Prev].erase({ -cur.first, -cur.second });
		S[p].insert({ -cur.first, -cur.second });
		student[cid].pos = p;
		for (int i = N - 1; i >= 0; i--) {
			W[student[cid].sc[i].second].erase(cur);
			if (student[cid].sc[i].second == p) break;
		}
		rm_update(Prev);
	}
}

int add(int mStudent, int mX, int mY) {

	// 학생 ID에 대해 해싱해서 저장한다.
	// id를 1부터 시작하도록 했다.. 왜? 
	sid[mStudent] = ++id;
	// 학생에 대한 구조체를 초기화 시키고 시작한다. 
	// 일종의 시간 절약 팁? 
	student[id] = {};


	// 학교를 기준으로 거리를 계산하고 이를 학생 구조체 정보에 저장한다. 
	for (int i = 0; i < N; i++) {
		//student[id].sc[i] = { abs(mX - school[i].x) + abs(mY - school[i].y), i };

		// 학교 번호와 학교와의 거리를 학생 구조체에 계산하여 넣어 준다. 
		int distance = abs(mX - school[i].x) + abs(mY - school[i].y);
		// 학생의 정보에 각 학교와 그 학교 같의 거리 정보를 저장한다.
		student[id].sc[i] = { distance, i };

	}

	// 학생 구조체의 학교 정보(거리, 학교ID)를 less, 오름차순(적은거리가 먼저나오는 것)으로 정렬해준다.
	// 거리가 작은순으로 배정하기 위해!!!
	// pair<int,int> 타입도 sort() 함수내에서 정렬이 되며
	// 정렬 기준은 first 인자를 기준으로 정렬된다.
	// default 인 less 의 구조체를 정렬할때 어떠한 순서로 정렬되는가? 
	sort(student[id].sc, student[id].sc + N);

	update(id, mStudent);
	return student[id].pos;
}

int remove(int mStudent) {
	int rid = sid[mStudent], val = student[rid].sc[N - 1].first, p = student[rid].pos;
	S[p].erase({ val, -mStudent });
	for (int i = 0; i < N; i++) W[i].erase({ -val, mStudent });
	rm_update(p);
	return p;
}

int status(int mSchool) { return S[mSchool].size(); }
#elif 0

struct Node {


} node[7'500];

unordered_map<int, int>		h;
int idx;

struct School {

} SC[10];


int C;
void init(int C, int N, int mX[], int mY[]) {
	//각 테스트 케이스의 처음에 호출된다.
	//각 학교가 수용할 수 있는 최대 학생 수 C, 학교의 개수 N이 주어진다.
	//그리고 N개의 학교에 대한 X좌표 값과 Y좌표 값이 배열로 주어진다.
	//N개의 학교는 순차적으로 0부터 N - 1까지 학교 ID를 가진다.
	//동일한 위치에 2개 이상의 학교가 있을 수 있다.
	//Parameters
	//C : 각 학교가 수용할 수 있는 최대 학생 수(2 ≤ C ≤ 1, 000)
	//	N : 학교 개수(3 ≤ N ≤ 10)
	//	(0 ≤ i ＜ N)인 모든 i에 대해,
	//	mX[i]: 학교 i의 X좌표(0 ≤ mX[i] ≤ 10, 000)
	//	mY[i] : 학교 i의 Y좌표(0 ≤ mY[i] ≤ 10, 000) 
	::C = C;
}

int update() {

}

int add(int mStudent, int mX, int mY) {
	//학생 ID가 mStudent인 학생이 추가된다.학생의 위치는(mX, mY)로 주어진다.
	//배정 규칙에 따라, 학교를 배정한다.
	//동일한 위치에 2명 이상의 학생이 있을 수 있다.
	//삭제되었던 학생이 다른 위치에서 다시 추가될 수도 있다.
	//전체 학교 수용 인원보다 많은 학생이 추가되는 경우는 없다.즉 배정에 실패하는 경우는 없다.
	//배정 규칙에 따라 다른 학생들의 배정된 학교가 변경 될 수 있다.
	//Parameters
	//mStudent : 학생 ID(1 ≤ mStudent ≤ 1, 000, 000, 000)
	//	mX : 학생의 X좌표(0 ≤ mX ≤ 10, 000)
	//	mY : 학생의 Y좌표(0 ≤ mY ≤ 10, 000)
	//	Returns
	//	배정된 학교의 ID를 반환한다
}




#elif 0
#include <algorithm>
#include <iostream>
using namespace std;

int main() {
	int a[] = { 456,56,654,6489,4685,78,98,6451,31,312312 };
	sort(a, a + sizeof(a) / sizeof(a[0]), greater<>());

	for (auto &i : a)	cout << i << "\t";
	cout << endl;
	return 0;
}

#elif 0

#define MAX_STUDENT 7500
#define MAX_SCHOOL  10
#define MAX_TABLE   10000

#include <algorithm>

struct school_t;
struct student_t
{
	int id;
	long long priority;
	bool valid;
	school_t *school, *candidate[MAX_SCHOOL];
	student_t* next;
} student_pool[MAX_STUDENT];
int student_idx;

student_t* hash_table[MAX_TABLE];

struct school_t
{
	int id, x, y, n_student;
	int heap_size[2];
	student_t* heap[2][MAX_STUDENT];

	bool compare(int idx, student_t* a, student_t* b)
	{
		const int sign[2] = { 1, -1 };
		return (a->priority - b->priority) * sign[idx] < 0;
	}

	void heap_push(int idx, student_t* s)
	{
		heap[idx][heap_size[idx]] = s;
		int current = heap_size[idx], parent = (current - 1) / 2;
		while (current > 0 && compare(idx, heap[idx][current], heap[idx][parent]))
		{
			student_t* temp = heap[idx][parent];
			heap[idx][parent] = heap[idx][current];
			heap[idx][current] = temp;
			current = parent;
			parent = (current - 1) / 2;
		}
		heap_size[idx]++;
	}

	student_t*  heap_pop(int idx)
	{
		student_t* ret = heap[idx][0];
		heap[idx][0] = heap[idx][--heap_size[idx]];
		int current = 0, lchild = current * 2 + 1, child;
		while (lchild < heap_size[idx])
		{
			if (lchild + 1 == heap_size[idx])
				child = lchild;
			else
				child = compare(idx, heap[idx][lchild], heap[idx][lchild + 1]) ? lchild : lchild + 1;
			if (compare(idx, heap[idx][current], heap[idx][child]))
				break;
			student_t* temp = heap[idx][current];
			heap[idx][current] = heap[idx][child];
			heap[idx][child] = temp;
			current = child;
			lchild = current * 2 + 1;
		}
		return ret;
	}
} schools[MAX_SCHOOL];

int N, C;

void add_hash(student_t* s)
{
	int h = s->id % MAX_TABLE;
	s->next = hash_table[h];
	hash_table[h] = s;
}

student_t* find_hash(int id)
{
	student_t* pos = hash_table[id % MAX_TABLE];
	while (pos)
	{
		if (pos->id == id && pos->valid == true)
			return pos;
		pos = pos->next;
	}
	return 0;
}

int dist(int ax, int ay, int bx, int by)
{
	int dx = (ax > bx) ? ax - bx : bx - ax;
	int dy = (ay > by) ? ay - by : by - ay;
	return dx + dy;
}

void init(int C, int N, int mX[], int mY[])
{
	::C = C, ::N = N;
	student_idx = 0;
	for (register int i = 0; i < MAX_TABLE; i++)
		hash_table[i] = 0;
	for (register int i = 0; i < N; i++)
		schools[i] = { i, mX[i], mY[i], 0, 0, 0, };
}

int add(int mStudent, int mX, int mY)
{
	student_t* student = &student_pool[student_idx++];
	student->id = mStudent;
	student->valid = true;

	int priority[MAX_SCHOOL];
	for (register int i = 0; i < N; i++)
		priority[i] = (dist(mX, mY, schools[i].x, schools[i].y) << 8) + schools[i].id;
	std::sort(priority, priority + N);
	for (register int i = 0; i < N; i++)
		student->candidate[i] = &schools[priority[i] & 0xFF];

	student->priority = ((long long)(priority[N - 1] & 0xFFFFFF00) << 32) - student->id;
	add_hash(student);

	int ret = -1;
	while (1)
	{
		school_t* school = 0;
		for (register int i = 0; i < N; i++)
		{
			school = student->candidate[i];
			while (school->heap_size[0] > 0 && (school->heap[0][0]->valid == false || school->heap[0][0]->school != school))
				school->heap_pop(0);

			if (school->n_student < C || school->heap[0][0]->priority < student->priority)
			{
				school->heap_push(0, student);
				school->n_student++;
				student->school = school;
				if (ret == -1)
					ret = school->id;
				break;
			}
			else
				school->heap_push(1, student);
		}

		if (school->n_student <= C)
			break;

		student = school->heap_pop(0);
		school->n_student--;
	}

	return ret;
}

int remove(int mStudent)
{
	student_t* student = find_hash(mStudent);
	int ret = student->school->id;

	student->valid = false;
	student->school->n_student--;

	school_t* school = student->school;
	while (1)
	{
		while (school->heap_size[1] > 0 && school->heap[1][0]->valid == false)
			school->heap_pop(1);

		if (school->heap_size[1] == 0)
			break;

		student = school->heap_pop(1);
		school_t* prev_school = student->school;
		prev_school->n_student--;

		student->school = school;
		school->heap_push(0, student);
		school->n_student++;

		school = prev_school;
	}

	return ret;
}

int status(int mSchool)
{
	return schools[mSchool].n_student;
}



#elif 0

int a[] = { 5,8,
6,4,
8,9,
4,3,
4,6,
1,2 };

void print() {
	for (auto &i : a) {
		cout << i << " ";
	}
	cout << endl;
}

void heepify(int p_idx, int size) {
	int c_idx = 2 * p_idx + 1;

	if (c_idx < size - 1 && a[c_idx] < a[c_idx + 1])	c_idx++;

	if (c_idx < size && a[p_idx] < a[c_idx]) {
		swap(a[p_idx], a[c_idx]);
		heepify(c_idx, size);
	}
}

void heepsort(int size) {
	for (int i = size / 2 - 1; i >= 0; i--) {
		heepify(i, size);
	}
	print();
	for (int i = size - 1; i > 0; i--) {
		swap(a[0], a[i]);
		heepify(0, i);
	}
}
int main() {
	int size = sizeof(a) / sizeof(a[0]);
	print();
	heepsort(size);
	print();
	return 0;
}


#elif 1


#include <iostream>
#include <queue>

using namespace std;

int c[27];

// 비교 함수 정의 (첫 번째 원소를 기준으로 작은 값이 우선순위가 높음)
//struct Compare {
//	bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const {
//		return a.first > b.first;
//	}
//};



struct cmp {
	bool operator()(const pair<int, char>&a, const pair<int, char> &b) const {
		return a.first != b.first ? a.first < b.first : a.second > b.second;
	}
};
priority_queue<pair<int, char>, vector<pair<int, char>>, cmp>	pq;


int main() {
	//string s = "data structure and algorithm";
	char s[100] = "data structure and algorithm";
	for (int i = 0; s[i] != '\0'; i++) {
		c[s[i] - 'a']++;
	}
	for (int i = 0; i < 'z' - 'a'; i++) {
		pq.emplace(c[i], i + 'a');
	}

	while (!pq.empty()) {
		cout << pq.top().first << " " << pq.top().second << endl;
		pq.pop();
	}

	return 0;
}

#elif 0
#include <iostream>
#include <queue>
using namespace std;

struct Node {
	int x, y;
	Node(int _x, int _y) : x(_x), y(_y) {}

	const Node operator+(const Node &ref) const {
		return Node(x + ref.x, y + ref.y);
	}
	//bool operator<(const Node &ref) const {
	//	return x != ref.x ? x < ref.x : y < ref.y;
	//}
};

struct cmp {
	bool operator()(const Node &r1, const Node &r2) const {
		return r1.x != r2.x ? r1.x < r2.x : r1.y > r2.y;
	}
};

int main() {

	priority_queue<Node, vector<Node>, cmp> pq;

	pq.emplace(10, 20);
	pq.emplace(10, 30);
	pq.emplace(15, 15);
	pq.emplace(12, 50);

	Node a(1, 2);
	Node b(11, 21);
	Node c = a + b;
	pq.emplace(c);

	while (!pq.empty()) {
		cout << pq.top().x << " " << pq.top().y << endl;
		pq.pop();
	}
	return 0;
}

#elif 0
#include <iostream>
#include <windows.h>
#include <algorithm>
using namespace std;

int main()
{
	const long long SIZE = 5000000;
	int *a = new int[SIZE];

	for (long long i = 0; i < SIZE; i++)
		a[i] = rand();

	int t1 = GetTickCount();

	// 람다 정의
	// [] 리턴 타입 및 함수명 생략
	// 
	sort(a, a + SIZE, [](int a, int b)
	{
		return a > b;
	}
	);

	int t2 = GetTickCount() - t1;
	cout << "람다 정렬시간 : " << t2 << endl;
}
#elif 0
#include <iostream>
#include <windows.h>
#include <algorithm>
using namespace std;
class CMP
{
public:
	// 클래스 내부에 함수객체(Function Object)생성
	// 함수 호출 연산자 ()를 연산자 오버로딩 한 것이다.
	// 클래스 내부에 존재하기 때문에 객체 속성을 가지게 되어때문에 inline화 가능
	bool operator()(int a, int b)
	{
		return a > b;
	}
};

// 일반 함수
// 함수포인터 호출과 함께 inline화 되지 않는다.
bool cmp(int a, int b)
{
	return a > b;
}


int main()
{
	const long long SIZE = 5000000;
	int *a = new int[SIZE];

	// 함수객체(Function object)를 포함하는 객체 생성
	CMP cmp2;

	for (long long i = 0; i < SIZE; i++)
		a[i] = rand();

	int t1 = GetTickCount();

	// 일반 함수포인터를 받아 sort하는 경우
	sort(a, a + SIZE, cmp);

	// 함수객체(function object)를 받아 sort하는 경우
	// 전달인자로 객체를 넘겨주지만 CMP객체내부에서 (int,int)형을 오버로딩한 함수객체가 호출됨
	//sort(a, a + SIZE, cmp2);    

	int t2 = GetTickCount() - t1;

	cout << "정렬시간 : " << t2 << endl;

	delete[]a;
}
#elif 0
#include <iostream>
using namespace std;

struct Node {
	int x;
	int y;
	Node() {
		cout << "생성자" << endl;
	}
	Node(int _x, int _y) : x(_x), y(_y) {
		cout << "생성자2" << endl;
	}
	//const Node operator+(const Node &ref) const{
	//	return Node(x + ref.x, y + ref.y);
	//}
};

const Node AddNode(const Node &ref1, const Node&ref2) {
	return Node(ref1.x + ref2.x, ref1.y + ref2.y);
}


int main() {
	//Node a{ 10, 20 };
	//Node b = { 30, 40 };
	//Node c ( 50, 60 );
	//struct Node a(10, 20);
	struct Node d;
	d.x = 10;
	d.y = 20;
	Node e(30, 40);
	//Node e(50, 60);
	e.x = 50;
	e.y = 60;
	cout << d.x << " " << d.y << endl;
	cout << e.x << " " << e.y << endl;
	Node f = AddNode(d, e);
	//Node f = d + e;
	cout << f.x << " " << f.y << endl;
	//cout << a.x << " " << a.y << endl;
	//cout << b.x << " " << b.y << endl;
	//cout << c.x << " " << c.y << endl;


	return 0;
}

#elif 0
#include <iostream>
#include <vector>
#include <queue>
#include <string>
using namespace std;
const int NMAX = 25;
char m[NMAX + 1][NMAX + 1];
int v[NMAX + 1][NMAX + 1];

priority_queue<int, vector<int>, greater<int> >	pq;

int dx[] = { -1, 0, 1, 0 };
int dy[] = { 0, -1, 0, 1 };
int N;

int dfs(int cx, int cy) {
	int ret = 0;
	v[cx][cy] = 1;

	for (int i = 0; i < 4; i++) {
		int nx = cx + dx[i];
		int ny = cy + dy[i];
		if (nx < 0 || ny < 0 || nx >= N || ny >= N)
			continue;
		if (!v[nx][ny] && m[nx][ny] != '0')
			ret += dfs(nx, ny) + 1;
	}

	return ret;
}
int bfs(int sx, int sy) {

	int ret = 0;
	queue<pair<int, int>> q;

	q.emplace(sx, sy);
	v[sx][sy] = 1;

	while (!q.empty()) {

		int cx = q.front().first;
		int cy = q.front().second;
		q.pop();
		ret++;

		for (int i = 0; i < 4; i++) {
			int nx = cx + dx[i];
			int ny = cy + dy[i];

			if (nx < 0 || ny < 0 || nx >= N || ny >= N)
				continue;

			if (!v[nx][ny] && m[nx][ny] != '0') {
				q.emplace(nx, ny);
				v[nx][ny] = 1;
			}
		}
	}
	return ret;
}

int main() {

	scanf("%d ", &N);
	for (int i = 0; i < N; i++) {
		scanf("%s ", m[i]);
	}


	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {


			if (v[i][j] || m[i][j] == '0') continue;

			//int cnt = bfs(i, j);
			int cnt = dfs(i, j) + 1;
			if (cnt)
				pq.emplace(cnt);

		}
	}

	printf("%d\n", pq.size());
	while (!pq.empty()) {

		printf("%d \n", pq.top());	pq.pop();

	}

	//for (int i = 0; i < N; i++) {
	//	for (int j = 0; j < N; j++) {
	//		printf("%c ", m[i][j]);
	//	}
	//	printf("\n");
	//}
	return 0;
}

#elif 0
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

const int NMAX = 100;

vector<int>	E[NMAX + 1];
//int E[NMAX + 1][NMAX + 1];

int N, M;
int v[NMAX + 1];

int bfs(int s) {

	int ret = -1;
	queue<int>	q;
	q.push(s);
	v[s] = 1;

	while (!q.empty()) {
		int c = q.front(); q.pop();
		ret++;

		for (int i = 1; i <= E[c][0]; i++) {
			int n = E[c][i];
			if (v[n])	continue;
			q.push(n), v[n] = 1;
		}
	}
	return ret;
}
int dfs(int s) {
	int ret = 0;
	v[s] = 1;

	//for (int i = 1; i <= E[s][0]; i++) {		
	for (unsigned int i = 0; i < E[s].size(); i++) {
		int n = E[s][i];
		if (v[n])	continue;
		v[n] = 1;
		ret += dfs(n) + 1;
	}
	return ret;
}
int main() {

	scanf("%d %d ", &N, &M);
	for (int i = 0; i < M; i++) {
		int s, e;
		scanf("%d %d ", &s, &e);
		E[s].emplace_back(e);
		E[e].emplace_back(s);
		//E[s][0]++;
		//E[s][E[s][0]] = e;
		//E[e][0]++;
		//E[e][E[e][0]] = s;
	}
	//printf("%d\n", bfs(1));
	printf("%d\n", dfs(1));

	return 0;
}
#elif 0
#include <iostream>
#include <vector>

using namespace std;

void heapify(vector<int>& arr, int n, int i) {

	int p = i; // 최대값을 현재 노드로 설정
	int l = 2 * i + 1; // 왼쪽 자식 노드의 인덱스
	int r = 2 * i + 2; // 오른쪽 자식 노드의 인덱스

	// 왼쪽 자식이 더 크다면
	if (l < n && arr[l] > arr[p]) {
		p = l;
	}

	// 오른쪽 자식이 더 크다면
	if (r < n && arr[r] > arr[p]) {
		p = r;
	}

	// 최대값이 현재 노드가 아니라면 교환
	if (p != i) {
		swap(arr[i], arr[p]);

		// 재귀적으로 하위 트리에 대해 heapify 호출
		heapify(arr, n, p);
	}
}

int main() {
	vector<int> arr = { 4, 10, 3, 5, 1 };
	int n = arr.size();

	cout << "Original Array: ";
	for (int num : arr) {
		cout << num << " ";
	}
	cout << endl;

	// 배열을 힙으로 만들기
	for (int i = n / 2 - 1; i >= 0; i--) {
		heapify(arr, n, i);
	}

	cout << "Heapified Array: ";
	for (int num : arr) {
		cout << num << " ";
	}
	cout << endl;

	return 0;
}


#elif 0
#include <iostream>
using namespace std;

const int N = 9;
int a[N] = { 7,6,5,8,3,5,9,1,6 };

int main() {
	// 먼제 전체 트리 구조를 최대 힙 구조로 바꿉니다. 
	for (int i = 1; i < N; i++) {
		int c = i;
		do {
			int p = (c - 1) / 2;
			if (a[p] < a[c]) {
				swap(a[p], a[c]);
			}
			c = p;
		} while (c != 0);
	}

	for (int i = N - 1; i >= 0; i--) {
		swap(a[0], a[i]);

		int p = 0;
		int c = 1;
		do {
			c = 2 * p + 1;
			if (a[c] < a[c + 1] && )
		}
	}

	// 

	int a;

	cin >> a;
	cout << a;
	setbuf(stdout, NULL);
	return 1;
}

#elif 0
//#include <iostream>
//#include <queue>
//#include <unordered_map>
//#include <vector>
//
//using namespace std;
//
//// 허프만 트리 노드 정의
//struct HuffmanNode {
//	char data;
//	int freq;
//	HuffmanNode* left;
//	HuffmanNode* right;
//
//	HuffmanNode(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
//};
//
//// 우선순위 큐 비교 연산자 정의
//struct CompareNodes {
//	bool operator()(HuffmanNode* a, HuffmanNode* b) {
//		return a->freq > b->freq;
//	}
//};
//
//// 허프만 트리 생성 함수
//HuffmanNode* buildHuffmanTree(const string& data) {
//	// 빈도수 계산
//	unordered_map<char, int> freqMap;
//	for (char ch : data) {
//		freqMap[ch]++;
//	}
//
//	// 우선순위 큐 생성
//	priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes> pq;
//
//	// 각 문자를 단독 노드로 추가
//	for (const auto& entry : freqMap) {
//		pq.push(new HuffmanNode(entry.first, entry.second));
//	}
//
//	// 허프만 트리 구성
//	while (pq.size() > 1) {
//		HuffmanNode* left = pq.top(); pq.pop();
//		HuffmanNode* right = pq.top(); pq.pop();
//
//		HuffmanNode* internalNode = new HuffmanNode('\0', left->freq + right->freq);
//		internalNode->left = left;
//		internalNode->right = right;
//
//		pq.push(internalNode);
//	}
//
//	return pq.top();
//}
//
//// 허프만 코드 출력 함수 (스택 사용)
//void printHuffmanCodes(HuffmanNode* root, unordered_map<char, string>& huffmanCodes) {
//	stack<pair<HuffmanNode*, string>> st;
//	st.push({ root, "" });
//
//	while (!st.empty()) {
//		auto [node, code] = st.top(); st.pop();
//
//		if (node->left) {
//			st.push({ node->left, code + "0" });
//		}
//		if (node->right) {
//			st.push({ node->right, code + "1" });
//		}
//
//		if (!node->left && !node->right) {
//			huffmanCodes[node->data] = code;
//			cout << node->data << ": " << code << endl;
//		}
//	}
//}
//
//// 허프만 코드 생성 및 출력 함수
//void generateHuffmanCodes(const string& data) {
//	HuffmanNode* root = buildHuffmanTree(data);
//
//	unordered_map<char, string> huffmanCodes;
//	cout << "Huffman Codes:" << endl;
//	printHuffmanCodes(root, huffmanCodes);
//
//	// 메모리 해제
//	delete root;
//}
//
//int main() {
//	string data = "abracadabra";
//	generateHuffmanCodes(data);
//
//	return 0;
//}
//
//#elif 0
//#include <iostream>
//#include <queue>
//#include <unordered_map>
//#include <vector>
//
//using namespace std;
//
//// 허프만 트리 노드 정의
//struct HuffmanNode {
//	char data;
//	int freq;
//	HuffmanNode* left;
//	HuffmanNode* right;
//
//	HuffmanNode(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
//};
//
//// 우선순위 큐 비교 연산자 정의
//struct CompareNodes {
//	bool operator()(HuffmanNode* a, HuffmanNode* b) {
//		return a->freq > b->freq;
//	}
//};
//
//// 허프만 트리 생성 함수
//HuffmanNode* buildHuffmanTree(const string& data) {
//	// 빈도수 계산
//	unordered_map<char, int> freqMap;
//	for (char ch : data) {
//		freqMap[ch]++;
//	}
//
//	// 우선순위 큐 생성
//	priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes> pq;
//
//	// 각 문자를 단독 노드로 추가
//	for (const auto& entry : freqMap) {
//		pq.push(new HuffmanNode(entry.first, entry.second));
//	}
//
//	// 허프만 트리 구성
//	while (pq.size() > 1) {
//		HuffmanNode* left = pq.top(); pq.pop();
//		HuffmanNode* right = pq.top(); pq.pop();
//
//		HuffmanNode* internalNode = new HuffmanNode('\0', left->freq + right->freq);
//		internalNode->left = left;
//		internalNode->right = right;
//
//		pq.push(internalNode);
//	}
//
//	return pq.top();
//}
//
//// 허프만 코드 출력 함수 (재귀적)
//void printHuffmanCodes(HuffmanNode* root, string code, unordered_map<char, string>& huffmanCodes) {
//	if (root->left) {
//		printHuffmanCodes(root->left, code + "0", huffmanCodes);
//	}
//	if (root->right) {
//		printHuffmanCodes(root->right, code + "1", huffmanCodes);
//	}
//	if (!root->left && !root->right) {
//		huffmanCodes[root->data] = code;
//		cout << root->data << ": " << code << endl;
//	}
//}
//
//// 허프만 코드 생성 및 출력 함수
//void generateHuffmanCodes(const string& data) {
//	HuffmanNode* root = buildHuffmanTree(data);
//
//	unordered_map<char, string> huffmanCodes;
//	cout << "Huffman Codes:" << endl;
//	printHuffmanCodes(root, "", huffmanCodes);
//
//	// 메모리 해제
//	delete root;
//}
//
//int main() {
//	string data = "abracadabra";
//	generateHuffmanCodes(data);
//
//	return 0;
//}


#elif 0
#include<queue>
#include<iostream>
#include<string.h>

using namespace std;
#define N   1'000
unsigned char P[N + 1][N + 1];
int W, H;

void print() {

	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			//printf("%3d", P[x][y]);
		}
		//printf("\n");
	}
	//printf("\n");

}
void init(int mWidth, int mHeight, unsigned char mInImage[]) {
	//각 test case의 처음에 한번 호출된다.
	//mWidth는 이미지의 가로 크기, mHeight는 세로의 크기이다.mInImage 는 이미지 데이터이며, 1바이트가 1픽셀을 나타낸다. 1픽셀은 0~127의 색상값을 가진다.
	//이미지는 최상단 좌측 픽셀부터 우측으로 순서대로 주어지며,
	//P(x, y)에 해당하는 값은 mInImage[X + Y * mWidth] 이다.
	//__Parameters
	//____mWidth : 이미지의 가로 크기(4 ≤ mWidth ≤ 1, 000)
	//_____mHeight : 이미지의 세로 크기(4 ≤ mHeight ≤ 1, 000)
	//_____mInImage : 입력 이미지(이미지의 크기 = mWidth * mHeight)
	::W = mWidth;
	::H = mHeight;
	for (int y = 0; y < mHeight; y++) {
		for (int x = 0; x < mWidth; x++) {
			P[x][y] = mInImage[x + y * mWidth];
			//printf("%2c ", P[x][y]);
		}
		//printf("\n"); 
	}
	//printf("\n");
	print();
}
void rect(int mX, int mY, int mW, int mH, unsigned char mColor, unsigned char mOutImage[]) {

	//현재 이미지에 사각형을 그린다.
	//현재 이미지에서(mX, mY) 위치를 시작으로(mX + mW - 1, mY + mH - 1) 의 영역을 mColor 색상으로 바꾼다.
	//주어지는 영역이 현재 이미지의 범위를 벗어나는 경우는 없다.
	//mOutImage 는[Fig. 2] 의 픽셀테이터 형식으로 저장한다.
	//__Parameters
	//____mX : 가로 시작 위치
	//_____mY : 세로 시작 위치
	//_____mW : 그리는 영역의 너비
	//_____mH : 그리는 영역의 높이
	//_____mColor : 그려지는 색상(0 ≤ mColor ≤ 127)
	//_____mOutImage : 실행 후 결과 이미지  
	for (int y = mY; y < mY + mH; y++) {
		for (int x = mX; x < mX + mW; x++) {
			P[x][y] = mColor;
		}
	}
	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			mOutImage[x + y * W] = P[x][y];;
			//printf("%2c ", P[x][y]);
		}
		//printf("\n");
	}
	//printf("\n");
	print();

}

int T[N][N];

void swap(int* a, int* b) { int temp = *a; *a = *b; *b = temp; }
void rotate(unsigned char mOutImage[]) {

	//현재 이미지를 시계방향으로 90도 회전 시킨다.
	//mOutImage 는[Fig. 2] 의 픽셀테이터 형식으로 저장한다.
	//__Parameters
	//____mOutImage : 실행 후 결과 이미지.

	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			T[H - 1 - y][x] = P[x][y];
		}
	}
	swap(&W, &H);
	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			mOutImage[x + y * W] = P[x][y] = T[x][y];
			//printf("%2c ", P[x][y]);
		}
		//printf("\n");
	}
	//printf("\n");
	print();

}

int v[N][N];
int dx[4] = { -1, 0, 1, 0 };
int dy[4] = { 0, 1, 0, -1 };
queue<pair<int, int>> q;

void bfs(int x, int y, int nc) {

	memset(v, 0, sizeof(v));

	int oc = P[x][y];

	v[x][y] = 1;
	q.push({ x, y });

	while (!q.empty()) {

		int cx = q.front().first;
		int cy = q.front().second;
		q.pop();

		P[cx][cy] = nc;

		for (int i = 0; i < 4; i++) {
			int nx = cx + dx[i];
			int ny = cy + dy[i];
			if (v[nx][ny] || P[nx][ny] != oc)
				continue;
			v[nx][ny] = 1;
			q.push({ nx, ny });
		}
	}

	print();

}
void paintBucket(int mX, int mY, unsigned char mColor, unsigned char mOutImage[]) {
	//현재 이미지에서(mX, mY) 위치를 시작으로 상하좌우 방향, 동일한 색으로 연결된 영역의 픽셀들이 mColor로 채워진다.
	//주어지는 영역이 현재 이미지의 범위를 벗어나는 경우는 없다.
	//mOutImage 는[Fig. 2] 의 픽셀테이터 형식으로 저장한다.
	//P(mX, mY) 와 동일한 mColor가 주어지는 경우는 없다.
	//__Parameters
	//____mX : 가로 위치. (0 ≤ mX ≤ 너비 – 1)
	//_____mY : 세로 위치. (0 ≤ mY ≤ 높이 – 1)
	//_____mColor : 채워지는 색. (0 ≤ mColor ≤ 127)
	//_____mOutImage : 실행 후 결과 이미지
	bfs(mX, mY, mColor);

	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			mOutImage[x + y * W] = P[x][y];;
			//printf("%4d ", mImage[x + y * W]);
		}
	}

}



int c[128];
unsigned char mImage[N * N];

struct Node {
	int f, c;
	struct Node* L, *R;
}node;

priority_queue<struct Node, vector<struct Node>, less<pair<int, int>>> pq;


int save(unsigned char mOutHuffData[]) {

	memset(mImage, 0, sizeof(mImage));

	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			mImage[x + y * W] = P[x][y];
		}
	}

	for (int i = 0; i < H * W; i++) {
		c[mImage[i]]++;
	}

	for (int i = 0; i < 128; i++) {
		pq.push({ c[i], i });
	}

	while (!pq.empty()) {

	}

	return -1;
}
#elif 0
// [H2339] 파일 저장소

#include<unordered_map>	
using namespace std;


unordered_map<int, int>	h;

int remove(int mId) {

}


#elif 0
// prefix sum
#include<iostream>
using namespace std;


int a[] = { 1,2,3,2,3,2,1,5,4,2,3,1,5,2,4 };
int s[30];

int main() {
	for (int i = 0; i < 14; i++) {
		s[i + 1] = s[i] + a[i];
	}
	for (int i = 0; i < 14; i++) {
		cout << a[i] << " ";
	}
	cout << endl;
	for (int i = 0; i < 14 + 1; i++) {
		cout << s[i] << " ";
	}

	return 0;
}


#elif 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 허프만 트리 노드 정의
typedef struct HuffmanNode {
	char data;
	int freq;
	struct HuffmanNode* left;
	struct HuffmanNode* right;
} HuffmanNode;

// 허프만 트리 생성 함수
HuffmanNode* buildHuffmanTree(char data[], int freq[], int size) {
	// TODO: 허프만 트리 생성 코드 작성

	// 여기에 코드를 작성하세요.

	return NULL;
}

// 허프만 코드 출력 함수 (재귀적)
void printHuffmanCodes(HuffmanNode* root, char code[], int top) {
	if (root->left != NULL) {
		code[top] = '0';
		printHuffmanCodes(root->left, code, top + 1);
	}

	if (root->right != NULL) {
		code[top] = '1';
		printHuffmanCodes(root->right, code, top + 1);
	}

	if (root->left == NULL && root->right == NULL) {
		printf("%c: %s\n", root->data, code);
	}
}

int main() {
	char data[] = { 'a', 'b', 'r', 'a', 'c', 'a', 'd', 'a', 'b', 'r', 'a' };
	int freq[] = { 5, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1 };
	int size = sizeof(data) / sizeof(data[0]);

	// 허프만 트리 생성
	HuffmanNode* root = buildHuffmanTree(data, freq, size);

	if (root == NULL) {
		fprintf(stderr, "허프만 트리 생성 실패\n");
		return 1;
	}

	// 허프만 코드 출력
	char code[100]; // 충분한 크기로 설정
	printf("허프만 코드:\n");
	printHuffmanCodes(root, code, 0);

	// 메모리 해제
	// TODO: 메모리 해제 코드 작성

	return 0;
}


#elif 0
// Hugman coding 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 허프만 트리 노드 정의
typedef struct HuffmanNode {
	char data;
	int freq;
	struct HuffmanNode* left;
	struct HuffmanNode* right;
} HuffmanNode;

// 우선순위 큐 노드 정의
typedef struct PriorityQueueNode {
	HuffmanNode* node;
	struct PriorityQueueNode* next;
} PriorityQueueNode;

// 허프만 트리 노드와 빈도를 저장하는 우선순위 큐
typedef struct {
	PriorityQueueNode* front;
} PriorityQueue;

// 허프만 노드 생성
HuffmanNode* createHuffmanNode(char data, int freq) {
	HuffmanNode* newNode = (HuffmanNode*)malloc(sizeof(HuffmanNode));
	newNode->data = data;
	newNode->freq = freq;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

// 우선순위 큐 노드 생성
PriorityQueueNode* createPriorityQueueNode(HuffmanNode* node) {
	PriorityQueueNode* newNode = (PriorityQueueNode*)malloc(sizeof(PriorityQueueNode));
	newNode->node = node;
	newNode->next = NULL;
	return newNode;
}

// 우선순위 큐 초기화
PriorityQueue* createPriorityQueue() {
	PriorityQueue* queue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
	queue->front = NULL;
	return queue;
}

// 우선순위 큐에 노드 삽입
void enqueue(PriorityQueue* queue, HuffmanNode* node) {
	PriorityQueueNode* newNode = createPriorityQueueNode(node);

	if (queue->front == NULL || node->freq < queue->front->node->freq) {
		newNode->next = queue->front;
		queue->front = newNode;
	}
	else {
		PriorityQueueNode* current = queue->front;

		while (current->next != NULL && current->next->node->freq < node->freq) {
			current = current->next;
		}

		newNode->next = current->next;
		current->next = newNode;
	}
}

// 우선순위 큐에서 노드 추출
HuffmanNode* dequeue(PriorityQueue* queue) {
	if (queue->front == NULL) {
		return NULL;
	}

	PriorityQueueNode* temp = queue->front;
	HuffmanNode* node = temp->node;
	queue->front = temp->next;
	free(temp);
	return node;
}

// 허프만 트리 생성
HuffmanNode* buildHuffmanTree(char data[], int freq[], int size) {
	PriorityQueue* queue = createPriorityQueue();

	for (int i = 0; i < size; i++) {
		HuffmanNode* node = createHuffmanNode(data[i], freq[i]);
		enqueue(queue, node);
	}

	while (queue->front->next != NULL) {
		HuffmanNode* left = dequeue(queue);
		HuffmanNode* right = dequeue(queue);

		HuffmanNode* internalNode = createHuffmanNode('\0', left->freq + right->freq);
		internalNode->left = left;
		internalNode->right = right;

		enqueue(queue, internalNode);
	}

	HuffmanNode* root = dequeue(queue);
	free(queue);

	return root;
}

// 허프만 코드 생성 (재귀적)
void generateHuffmanCodes(HuffmanNode* root, char code[], int top) {
	if (root->left != NULL) {
		code[top] = '0';
		generateHuffmanCodes(root->left, code, top + 1);
	}

	if (root->right != NULL) {
		code[top] = '1';
		generateHuffmanCodes(root->right, code, top + 1);
	}

	if (root->left == NULL && root->right == NULL) {
		printf("%c: %s\n", root->data, code);
	}
}

// 메모리 해제 (허프만 트리)
void freeHuffmanTree(HuffmanNode* root) {
	if (root == NULL) {
		return;
	}

	freeHuffmanTree(root->left);
	freeHuffmanTree(root->right);
	free(root);
}

int main() {
	char data[] = { 'a', 'b', 'c', 'd', 'e' };
	int freq[] = { 5, 9, 12, 13, 16 };
	int size = sizeof(data) / sizeof(data[0]);

	HuffmanNode* root = buildHuffmanTree(data, freq, size);

	char code[100]; // 임의로 충분한 크기로 설정
	printf("Huffman Codes:\n");
	generateHuffmanCodes(root, code, 0);

	// 메모리 해제
	freeHuffmanTree(root);

	return 0;
}


#include<iostream>
using namespace std;
const int N = 10;
int a[N][N];
int mat[][4] = { {1, 2, 3, 4},{5, 6, 7, 8} };

void init() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			a[i][j] = 0;
		}
	}
}
int main() {


}



#elif 0
#include <stdio.h>
int temp[10][10];  // 행과 열을 바꾼 새로운 배열

void rotateRectangle(int mat[][4], int rows, int cols) {
	//int temp[cols][rows];  // 행과 열을 바꾼 새로운 배열

	// 행과 열을 바꿔서 새로운 배열에 저장
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			temp[j][rows - 1 - i] = mat[i][j];
		}
	}
	/*
	void rotate(){
		int N = 10;
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				//cout << a[i][j] << " ";
				m[j][N - 1 - i] = a[i][j];
			}
		}
	}
	*/
	// 새로운 배열의 내용을 원래 배열에 복사
	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			mat[i][j] = temp[i][j];
		}
	}
}

void printRectangle(int mat[][4], int rows, int cols) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			printf("%d ", mat[i][j]);
		}
		printf("\n");
	}
}

int mat[][4] = { {1, 2, 3, 4},
				{5, 6, 7, 8},
				{9, 10, 11, 12} };

int main() {


	int rows = 3; // 배열의 행 수
	int cols = 4; // 배열의 열 수

	printf("Original Rectangle:\n");
	printRectangle(mat, rows, cols);

	// 배열을 90도 회전
	rotateRectangle(mat, rows, cols);

	printf("\nRectangle After 90-degree Rotation:\n");
	printRectangle(mat, cols, rows);

	return 0;
}

#elif 0
#include <stdio.h>

void swap(int& a, int& b) {
	int temp = a;
	a = b;
	b = temp;
}
void rotateMatrix(int mat[][3], int N) {
	// 행과 열을 바꾸고
	for (int i = 0; i < N; i++) {
		for (int j = i + 1; j < N; j++) {
			swap(mat[i][j], mat[j][i]);
		}
	}

	// 열의 순서를 뒤집습니다.
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N / 2; j++) {
			swap(mat[i][j], mat[i][N - 1 - j]);
		}
	}
}

void printMatrix(int mat[][3], int N) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			printf("%d ", mat[i][j]);
		}
		printf("\n");
	}
}

int main() {
	int mat[][3] = { {1, 2, 3},
					{4, 5, 6},
					{7, 8, 9} };

	int N = 3; // 배열의 크기

	printf("Original Matrix:\n");
	printMatrix(mat, N);

	// 배열을 90도 회전
	rotateMatrix(mat, N);

	printf("\nMatrix After 90-degree Rotation:\n");
	printMatrix(mat, N);

	return 0;
}

#elif 0
#include<iostream>
using namespace std;

int a[10][10] = {
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0}
	//{ 0,1,2,3,4,5,6,0,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,0,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
};
int m[10][10];
void rotate90(int arr[][10]) {
	int N = 9; memset(m, 0, sizeof(m) / sizeof(int));
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			m[j][N - i] = arr[i][j];
		}
	}
}
void rotate180(int arr[][10]) {
	int N = 9;	memset(m, 0, sizeof(m) / sizeof(int));
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			m[N - i][N - j] = arr[i][j];
		}
	}
}
void rotate270(int arr[][10]) {
	int N = 9;	memset(m, 0, sizeof(m) / sizeof(int));
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			m[N - j][i] = arr[i][j];
		}
	}
}
void print(int arr[][10]) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			cout << arr[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}
int mat[][4] = { {1, 2, 3, 4},
				{5, 6, 7, 8} };

void copy(int arr[][4]) {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			a[i][j] = arr[i][j];
		}
	}
}
int main() {

	print(a);

	copy(mat);
	print(a);

	rotate90(a);
	print(m);

	rotate180(a);
	print(m);

	rotate270(a);
	print(m);

	return 0;
}
#elif 0
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int *tree;
	int *nums;
	int size;
} SegmentTree;

SegmentTree *createSegmentTree(int nums[], int size) {
	SegmentTree *segmentTree = (SegmentTree *)malloc(sizeof(SegmentTree));
	segmentTree->nums = nums;
	segmentTree->size = size;
	segmentTree->tree = (int *)malloc(4 * size * sizeof(int)); // 대략적으로 4배 정도의 크기로 세그먼트 트리 배열을 생성
	return segmentTree;
}

void buildTree(SegmentTree *segmentTree, int node, int start, int end) {
	if (start == end) {
		segmentTree->tree[node] = segmentTree->nums[start];
	}
	else {
		int mid = (start + end) / 2;
		buildTree(segmentTree, 2 * node, start, mid);
		buildTree(segmentTree, 2 * node + 1, mid + 1, end);
		segmentTree->tree[node] = segmentTree->tree[2 * node] + segmentTree->tree[2 * node + 1];
	}
}

int query(SegmentTree *segmentTree, int node, int start, int end, int left, int right) {
	if (right < start || end < left) {
		return 0;
	}
	else if (left <= start && end <= right) {
		return segmentTree->tree[node];
	}
	else {
		int mid = (start + end) / 2;
		int leftSum = query(segmentTree, 2 * node, start, mid, left, right);
		int rightSum = query(segmentTree, 2 * node + 1, mid + 1, end, left, right);
		return leftSum + rightSum;
	}
}

void update(SegmentTree *segmentTree, int node, int start, int end, int index, int diff) {
	if (index < start || end < index) {
		return;
	}

	segmentTree->tree[node] += diff;

	if (start != end) {
		int mid = (start + end) / 2;
		update(segmentTree, 2 * node, start, mid, index, diff);
		update(segmentTree, 2 * node + 1, mid + 1, end, index, diff);
	}
}

void destroySegmentTree(SegmentTree *segmentTree) {
	free(segmentTree->tree);
	free(segmentTree);
}

int main() {
	int nums[] = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19 };
	int size = sizeof(nums) / sizeof(nums[0]);

	SegmentTree *segmentTree = createSegmentTree(nums, size);

	// Build the segment tree
	buildTree(segmentTree, 1, 0, size - 1);

	// Calculate the sum of elements in the range [2, 7]
	int sum = query(segmentTree, 1, 0, size - 1, 2, 7);
	printf("Sum of elements in range [2, 7]: %d\n", sum);

	// Update the value at index 3 to 10
	update(segmentTree, 1, 0, size - 1, 3, 10);

	// Recalculate the sum after update
	sum = query(segmentTree, 1, 0, size - 1, 2, 7);
	printf("Sum of elements in range [2, 7] after update: %d\n", sum);

	// Clean up
	destroySegmentTree(segmentTree);

	return 0;
}

#elif 0
#include<iostream>
using namespace std;
// Segment tree 알고리즘

int a[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
int tree[4 * 8];

int init(int s, int e, int n) {
	if (s == e) return tree[n] = a[s];
	int m = (s + e) / 2;
	return tree[n] = init(s, m, n * 2) + init(m + 1, e, n * 2 + 1);
}
int sum(int s, int e, int n, int l, int r) {

	//            l ------------- r
	//     s-e          s-e           s-e
	// 범위 밖에 있을 경우
	if (e < l || r < s)		return 0;

	// 범위 안에 있을 경우
	if (l <= s && e <= r)	return tree[n];

	// 범위에 걸쳐 있을 경우 
	int m = (s + e) / 2;
	return sum(s, m, n * 2, l, r) + sum(m + 1, e, n * 2 + 1, l, r);
}
int main() {

	int N = 8;
	init(0, N - 1, 1);	// s, e, 노드

	for (int i = 0; i < 4 * N; i++) {
		cout << i << " = " << tree[i] << endl;
	}


	cout << sum(0, 7, 1, 0, 2) << endl;
	return 0;
}
#elif 0
#include<iostream>
using namespace std;

struct Node {
	int data;
	struct Node *L, *R;
} node;

struct Node* creatnode(int data) {
	Node* newnode = new Node;
	*newnode = { data, NULL, NULL };
	return newnode;
}
void preorderTreversal(Node* root) {
	if (root != 0) {
		printf("%d ", root->data);
		preorderTreversal(root->L);
		preorderTreversal(root->R);
	}
}
void inorderTreversal(Node* root) {
	if (root != 0) {
		inorderTreversal(root->L);
		printf("%d ", root->data);
		inorderTreversal(root->R);
	}
}
void postorderTreversal(Node* root) {
	if (root != 0) {
		postorderTreversal(root->L);
		postorderTreversal(root->R);
		printf("%d ", root->data);
	}
}
int main() {
	Node* root = creatnode(1);
	root->L = creatnode(2);
	root->R = creatnode(3);
	root->L->L = creatnode(4);
	root->L->R = creatnode(5);
	root->R->L = creatnode(6);
	root->R->R = creatnode(7);

	printf("preorderTreversal");
	preorderTreversal(root);

	printf("\ninorderTreversal");
	inorderTreversal(root);

	printf("\npostorderTreversal");
	postorderTreversal(root);

	return 0;
}

#elif 0
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int* tree;
	int* nums;
	int size;
} SegmentTree;

SegmentTree* createSegmentTree(int nums[], int size) {
	SegmentTree* segmentTree = (SegmentTree*)malloc(sizeof(SegmentTree));
	segmentTree->nums = nums;
	segmentTree->size = size;
	segmentTree->tree = (int*)malloc(4 * size * sizeof(int)); // 대략적으로 4배 정도의 크기로 세그먼트 트리 배열을 생성
	return segmentTree;
}

void buildTree(SegmentTree* segmentTree, int node, int start, int end) {
	if (start == end) {
		segmentTree->tree[node] = segmentTree->nums[start];
	}
	else {
		int mid = (start + end) / 2;
		buildTree(segmentTree, 2 * node, start, mid);
		buildTree(segmentTree, 2 * node + 1, mid + 1, end);
		segmentTree->tree[node] = segmentTree->tree[2 * node] + segmentTree->tree[2 * node + 1];
	}
}

int query(SegmentTree* segmentTree, int node, int start, int end, int left, int right) {
	if (right < start || end < left) {
		return 0;
	}
	else if (left <= start && end <= right) {
		return segmentTree->tree[node];
	}
	else {
		int mid = (start + end) / 2;
		int leftSum = query(segmentTree, 2 * node, start, mid, left, right);
		int rightSum = query(segmentTree, 2 * node + 1, mid + 1, end, left, right);
		return leftSum + rightSum;
	}
}

void update(SegmentTree* segmentTree, int node, int start, int end, int index, int diff) {
	if (index < start || end < index) {
		return;
	}

	segmentTree->tree[node] += diff;

	if (start != end) {
		int mid = (start + end) / 2;
		update(segmentTree, 2 * node, start, mid, index, diff);
		update(segmentTree, 2 * node + 1, mid + 1, end, index, diff);
	}
}

int getSum(SegmentTree* segmentTree, int left, int right) {
	return query(segmentTree, 1, 0, segmentTree->size - 1, left, right);
}

void updateValue(SegmentTree* segmentTree, int index, int newValue) {
	int diff = newValue - segmentTree->nums[index];
	segmentTree->nums[index] = newValue;
	update(segmentTree, 1, 0, segmentTree->size - 1, index, diff);
}

void destroySegmentTree(SegmentTree* segmentTree) {
	free(segmentTree->tree);
	free(segmentTree);
}

int main() {
	int nums[] = { 1, 3, 5, 7, 9, 11 };
	int size = sizeof(nums) / sizeof(nums[0]);

	SegmentTree* segmentTree = createSegmentTree(nums, size);

	printf("Sum of elements in range [1, 3]: %d\n", getSum(segmentTree, 1, 3));

	// Update the value at index 2 to 10
	updateValue(segmentTree, 2, 10);

	printf("Sum of elements in range [1, 3] after update: %d\n", getSum(segmentTree, 1, 3));

	// Clean up
	destroySegmentTree(segmentTree);

	return 0;
}

#elif 0
//첫째 줄에 수의 개수 N(1 ≤ N ≤ 1, 000, 000)과 
//M(1 ≤ M ≤ 10, 000), 
//K(1 ≤ K ≤ 10, 000) 가 주어진다.
//M은 수의 변경이 일어나는 횟수이고, 
//K는 구간의 합을 구하는 횟수이다.
//그리고 둘째 줄부터 N + 1번째 줄까지 N개의 수가 주어진다.
//그리고 N + 2번째 줄부터 N + M + K + 1번째 줄까지 세 개의 정수 a, b, c가 주어지는데, 
//a가 1인 경우 b(1 ≤ b ≤ N)번째 수를 c로 바꾸고 
//a가 2인 경우에는 b(1 ≤ b ≤ N)번째 수부터 c(b ≤ c ≤ N)번째 수까지의 합을 구하여 출력하면 된다.
//입력으로 주어지는 모든 수는 - 263보다 크거나 같고, 263 - 1보다 작거나 같은 정수이다.
#include<iostream>
using namespace std;
long long int f[1'000'001];
long long int m[1'000'001];
long long int N, M, K;
void update(long long int i, long long int v) {
	while (i <= N) {
		f[i] += v;
		i += i & -i;
	}
}
long long int sum(long long int i) {
	long long int ret = 0;
	while (i > 0) {
		ret += f[i];
		i -= i & -i;
	}
	return ret;
}

int main() {
	scanf("%lld %lld %lld ", &N, &M, &K);
	for (int i = 1; i <= N; i++) {
		//long long int v;
		//scanf("%lld ", &v);
		scanf("%lld ", &m[i]);
		update(i, m[i]);
	}
	for (int i = 0; i < M + K; i++) {
		long long int a, b, c;
		scanf("%lld %lld %lld", &a, &b, &c);
		if (a == 1)
			update(b, c - m[b]);
		else
			printf("%lld\n", sum(c) - sum(b - 1));
	}
	return 0;
}


#elif 0
#include<iostream>
using namespace std;
const int N = 16;
int f[20];

void update(int i, int v) {
	while (i <= N) {
		f[i] += v;
		i += i & -i;
	}
}
int sum(int i) {
	int ret = 0;
	while (i > 0) {
		ret += f[i];
		i -= i & -i;
	}
	return ret;
}
int main() {
	for (int i = 1; i <= N; i++) {
		update(i, i);
	}
	for (int i = 1; i <= N; i++) {
		cout << sum(i) << endl;
	}
	return 1;
}

#elif 0
#include<iostream>
using namespace std;

void printbin(int a) {
	for (int j = 8; j >= 0; j--) {
		printf("%d", (a >> j) & 1);
	}
	printf("  ");
}
int main() {
	for (int i = 1; i <= 16; i++) {
		printf("%2d :", i);
		printbin(i);
		//printf("%d :", -i);
		printbin(-i);
		//printf("%d :", i);
		printbin(i & -i);
		printf("%d ", i & -i);
		//printf("%d :", i);
		//printbin( i += i & -i);
		printf("\n");
	}
	return 1;
}
#elif 0
#include<iostream>
using namespace std;
int a[10] = { 1,2,3,4,5,6,7,8,9,10 };
int fw[11];

void update(int i, int v) {
	while (i <= 10) {
		fw[i] += v;
		i += i & -i;
	}
}
int sum(int i) {
	int ret = 0;
	while (i > 0) {
		ret += fw[i];
		i -= i & -i;
	}
	return ret;
}
int main() {
	for (int i = 0; i < 10; i++) {
		update(i + 1, a[i]);
	}
	for (int i = 0; i < 10; i++) {
		cout << a[i] << " ";
	}
	cout << endl;
	for (int i = 0; i <= 10; i++) {
		cout << fw[i] << " ";
	}
	cout << endl;
	return 0;
	for (int i = 1; i <= 10; i++) {
		cout << sum(i) << " ";
	}
	cout << endl;
	update(5, 1);
	for (int i = 1; i <= 10; i++) {
		cout << sum(i) << " ";
	}
	return 1;
}
#elif 0
#define rint register int
#include <unordered_map>
#include <vector>
#include <memory.h>
#include <string>

using namespace std;
unordered_map<string, int> map;
int parents[12001][11];
int depth[12001];
int fenwick[1000002];
int idx;
int power[11] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };

int sum(int i) {
	rint ret = 0;
	while (i) {
		ret += fenwick[i];
		i -= (i & -i);
	}
	return ret;
}

void update(int i, int diff) {
	while (i <= 1000001) {
		fenwick[i] += diff;
		i += (i & -i);
	}
}

void init(char mAncestor[], int mDeathday)
{
	memset(parents, -1, sizeof(parents));
	map.clear();
	memset(fenwick, 0, sizeof(fenwick));
	memset(depth, 0, sizeof(depth));
	idx = 0;

	map[string(mAncestor)] = 0;
	update(1, 1);
	update(mDeathday + 2, -1);
}

int add(char mName[], char mParent[], int mBirthday, int mDeathday)
{
	map[string(mName)] = ++idx;
	rint pidx = map[string(mParent)];
	parents[idx][0] = pidx;
	depth[idx] = depth[pidx] + 1;
	for (rint i = 0; i < 11; i++)
	{
		if (parents[idx][i] == -1)
			break;
		parents[idx][i + 1] = parents[parents[idx][i]][i];
	}
	update(mBirthday + 1, 1);
	update(mDeathday + 2, -1);

	return depth[idx];
}

int distance(char mName1[], char mName2[])
{
	rint idx1 = map[string(mName1)];
	rint idx2 = map[string(mName2)];

	if (depth[idx1] < depth[idx2])
		swap(idx1, idx2);
	rint diff = depth[idx1] - depth[idx2];
	rint ret = diff;
	for (rint i = 0; i < 11; i++)
	{
		if (diff & 1) {
			idx1 = parents[idx1][i];
		}
		diff >>= 1;
	}
	if (idx1 == idx2)
		return ret;

	for (rint i = 10; i >= 0; i--)
	{
		if (parents[idx1][i] != -1 && parents[idx1][i] != parents[idx2][i]) {
			ret += (power[i] << 1);
			idx1 = parents[idx1][i];
			idx2 = parents[idx2][i];
		}
	}
	ret += 2;
	return ret;
}

int count(int mDay)
{
	return sum(mDay + 1);
}
#elif 1
//[H2320] 미생물 가계도
//[제약사항]
//1. 각 테스트 케이스 시작 시 init() 함수가 호출된다.
//2. 각 테스트 케이스에서 add() 함수의 호출 횟수는 12, 000 이하이다.
//3. 각 테스트 케이스에서 distance() 함수의 호출 횟수는 50, 000 이하이다.
//4. 각 테스트 케이스에서 count() 함수의 호출 횟수는 30, 000 이하이다.
//5. 모든 후손의 선조로부터의 가계도 거리는 2, 000 이하이다.
//6. 개체의 이름으로 주어지는 문자열은 영문 대소문자와 마지막 ‘＼0’로 이루어져 있다.
#include<iostream> 
#include<vector> 
#include<unordered_map> 
using namespace std;

struct Node {
	int parent, depth, fday, lday;
}node[12'001]; int ndx;

vector<int>	a[12'001];
unordered_map<string, int>	h;
int fw[1'000'005];

void update(int i, int v) {
	while (i <= 1'000'001) {
		fw[i] += v;
		i += i & -i;
	}
}
int sum(int i) {
	int ret = 0;
	while (i > 0) {
		ret += fw[i];
		i -= i & -i;
	}
	return ret;
}
void init(char mAncestor[], int mLastday) {
	//각 테스트 케이스의 처음에 호출된다.
	//기존의 기록들은 모두 삭제된다.
	//mAncestor는 선조의 이름이며, 선조의 생존 기간의 시작은 항상 0이다.
	//Parameters
	//mAncestor : 선조의 이름(3 ≤ | mAncestor | ≤ 11, | A | 는 A 문자열의 길이를 의미한다)
	//mLastday : 선조의 마지막 생존일(0 ≤ mLastDay ≤ 1, 000, 000)
	//memset(fw, 0, sizeof(fw));
	for (int i = 0; i < 1'000'005; i++)	fw[i] = 0;
	//memset(a, 0, sizeof(a));
	for (int i = 0; i < 12'001; i++)	a[i].clear();
	h.clear();
	ndx = 0;
	h[mAncestor] = ndx;
	node[ndx] = { 0, 0, 0, mLastday };
	update(1, 1);
	update(mLastday + 2, -1);
	ndx++;
}
int add(char mName[], char mParent[], int mFirstday, int mLastday) {
	//이름이 mName인 새로운 미생물 개체의 정보가 추가된다.기존에 중복된 이름이 없다는 것이 보장된다.
	//부모의 이름은 mParent이고, 개체의 생존 기간의 시작은 mFirstday, 마지막은 mLastday 이다.
	//이름이 mParent인 개체가 앞서 입력된 기록에 존재한다.
	//추가된 개체와 선조의 가계도 거리를 반환한다.
	//Parameters
	//mName : 개체의 이름(3 ≤ | mName | ≤ 11)
	//mParent : 개체의 부모의 이름(3 ≤ | mParent | ≤ 11)
	//mFirstday : 개체의 첫 생존일(부모의 Firstday ≤ mFirstday ≤ 부모의 Lastday)
	//mLastday : 개체의 마지막 생존일(mFirstday ≤ mLastday ≤ 1, 000, 000)
	//Returns
	//추가된 개체와 선조의 가계도 거리를 반환한다.	
	h[mName] = ndx;
	int p = h[mParent];	int c = h[mName];
	a[p].push_back(c);
	a[c].push_back(p);
	int d = node[p].depth;
	node[ndx] = { p, d + 1, mFirstday, mLastday };
	update(mFirstday + 1, 1);
	update(mLastday + 2, -1);
	ndx++;
	return d + 1;

}
int distance(char mName1[], char mName2[]) {
	//이름이 mName1인 개체와 mName2인 개체 간의 가계도 거리를 반환한다.
	//이름이 mName1, mName2인 개체가 앞서 입력된 기록에 존재한다.
	//Parameters
	//mName1, mName2: 가계도 거리를 알아내려는 두 개체의 이름(3 ≤ | mName1 | , | mName2 | ≤ 11)
	//Returns
	//두 개체 간의 가계도 거리를 반환한다.
	int ret = 0;
	int s = h[mName1];	int e = h[mName2];
	while (node[s].depth > node[e].depth) {
		ret++;
		s = node[s].parent;
	}
	while (node[s].depth < node[e].depth) {
		ret++;
		e = node[e].parent;
	}
	if (s != e) {
		ret += 2 * node[s].depth;
	}
	cout << mName1 << " " << mName2 << " " << ret << endl;
	return ret;
}
int count(int mDay) {
	//지금까지 기록된 미생물 중 생존 기간에 mDay일이 포함되는 개체 수를 반환한다.
	//Parameters
	//mDay : 생존한 개체 수를 조사하는 날짜(0 ≤ mDay ≤ 1, 000, 000)
	//Returns
	//생존 기간에 mDay일이 포함되는 개체 수를 반환한다.
	int ret = sum(mDay + 1);
	cout << mDay << " " << ret << endl;
	return ret;



}

#elif 0
#include <unordered_map>
using namespace std;

#define MAXN    8008

struct NODE {
	int id;
	int parent;
	int ch[2];
	int num;
	int sum;
} nALL[MAXN];

unordered_map<int, int> nHash;
int nidx;

void init(int mId, int mNum) {
	nidx = 0;
	nHash.clear();

	nALL[++nidx] = { mId, 0, {0, 0}, mNum, mNum };
	nHash[mId] = nidx;
}

void updateSum(int idx, int num) {
	if (nALL[idx].parent)
		updateSum(nALL[idx].parent, num);

	nALL[idx].sum += num;
}

int add(int mId, int mNum, int mParent) {
	int parent = nHash[mParent];
	if (nALL[parent].ch[0] && nALL[parent].ch[1])
		return -1;

	nALL[++nidx] = { mId, parent, {0, 0}, mNum, mNum };
	nHash[mId] = nidx;

	if (!nALL[parent].ch[0])
		nALL[parent].ch[0] = nidx;
	else
		nALL[parent].ch[1] = nidx;

	updateSum(parent, mNum);

	return nALL[parent].sum;
}

void removeNode(int idx) {
	if (idx == 0) return;

	if (nALL[idx].ch[0]) removeNode(nALL[idx].ch[0]);
	if (nALL[idx].ch[1]) removeNode(nALL[idx].ch[1]);

	nHash.erase(nALL[idx].id);
}

int remove(int mId) {
	int localidx = nHash[mId];
	if (localidx == 0)
		return -1;

	int ret = nALL[localidx].sum;
	int pidx = nALL[localidx].parent;
	updateSum(pidx, -ret);

	if (nALL[pidx].ch[0] == localidx)
		nALL[pidx].ch[0] = 0;
	else
		nALL[pidx].ch[1] = 0;

	// 삭제
	removeNode(localidx);

	return ret;
}

int cut, maxCut, limit;

int .
(int idx) {
	if (cut >= maxCut)
		return 0;

	if (nALL[idx].num > limit) {
		cut = 12345678;
		return 0;
	}
	if (nALL[idx].sum <= limit)
		return nALL[idx].sum;

	int L = nALL[idx].ch[0] ? dfs(nALL[idx].ch[0]) : 0;
	int R = nALL[idx].ch[1] ? dfs(nALL[idx].ch[1]) : 0;

	if (nALL[idx].num + R + L > limit) {
		cut++;
		(L > R) ? L = 0 : R = 0;
	}
	if (nALL[idx].num + L > limit) {
		cut++;
		L = 0;
	}
	if (nALL[idx].num + R > limit) {
		cut++;
		R = 0;
	}

	return nALL[idx].num + L + R;
}

int reorganize(int M, int K) {
	maxCut = M;
	limit = K;
	cut = 0;

	dfs(1);

	return cut < M;
}
#elif 0
#include<iostream>
#include<vector>
using namespace std;
vector<pair<int, int>>	v;
int main() {

	v.push_back({ 1, 2 });
	v.push_back({ 3, 4 });
	v.push_back({ 5, 6 });

	cout << v[0].first << " " << v[0].second << endl;
	cout << v[1].first << " " << v[1].second << endl;
	cout << v[2].first << " " << v[2].second << endl;
	return 1;
}

#elif 0
#include<iostream>
using namespace std;

void foo(int& a) {
	cout << a << " " << &a << endl;
	a = 10;
	cout << a << " " << &a << endl;
}
int main() {

	int ret = 0;
	cout << ret << " " << &ret << endl;
	foo(ret);
	cout << ret << " " << &ret << endl;
	return 1;
}
#elif 0
#include<unordered_map>
using namespace std;
unordered_map<int, int>	h;
//vector<pair<int, int>>	v[101];
int a[100][100];	// start 에서 end 까지 latency 배열
int v[100];
int dn;
//1. 각 테스트 케이스 시작 시 init() 함수가 한 번 호출된다.
//2. 각 테스트 케이스에서 connect() 함수의 호출 횟수는 10, 000 이하이다.
//3. 각 테스트 케이스에서 measure() 함수의 호출 횟수는 1, 000 이하이다.
//4. 각 테스트 케이스에서 test() 함수의 호출 횟수는 1, 000 이하이다.
//5. 임의의 두 장비의 전송 경로에 있는 장비의 수는 100 이하이다.
void init(int mDevice) {
	//테스트 케이스에 대한 초기화 함수.각 테스트 케이스의 맨 처음 1회 호출된다.
	//초기에 장비 mDevice가 있다.mDevice는 장비 번호이다.
	//Parameters
	//mDevice : 초기 장비 번호(1 ≤ mDevice ≤ 1, 000, 000, 000)
	dn = 0;
	h[mDevice] = dn++;
	memset(a, 0, sizeof(a));
}
void connect(int mOldDevice, int mNewDevice, int mLatency) {
	//새로운 장비 mNewDevice를 추가하고 장비 mNewDevice와 장비 mOldDevice를 전송 속도가 mLatency인 케이블로 연결한다.
	//mOldDevice는 이미 존재하는 장비 번호이다.
	//mNewDevice는 추가되는 새로운 장비 번호이다.
	//Parameters
	//mOldDevice : 이미 존재하는 장비 번호(1 ≤ mOldDevice ≤ 1, 000, 000, 000)
	//mNewDevice : 새롭게 추가되는 장비 번호(1 ≤ mNewDevice ≤ 1, 000, 000, 000)
	//mLatency : 케이블의 전송 속도(1 ≤ mLatency ≤ 10, 000)
	h[mNewDevice] = dn++;
	int o = h[mOldDevice];	int n = h[mNewDevice];
	//v[o].emplace_back(n, mLatency);	
	//v[n].emplace_back(o, mLatency);	
	a[o][n] = a[n][o] = mLatency;
}
int dfs(int s, int e) {
	int ret = 0;

	if (s == e)
		return 0;

	int check = 0;
	for (int i = 0; i < 100; i++) {
		if (!v[s] && a[s][i]) {
			check = 1;
		}
	}

	if (check == 0)
		return 0;

	v[s] = 1;

	for (int i = 0; i < 100; i++) {
		if (!v[s] && a[s][i]) {
			ret += dfs(i, e) + a[s][i];
		}
	}

	return ret;
}
int measure(int mDevice1, int mDevice2) {
	//장비 mDevice1에서 장비 mDevice2로 신호를 전송했을 때 전송 시간을 반환한다.
	//mDevice1와 mDevice2는 이미 존재하는 장비 번호이고 서로 다르다.
	//Parameters
	//mDevice1 : 장비 번호(1 ≤ mDevice1 ≤ 1, 000, 000, 000)
	//mDevice2 : 장비 번호(1 ≤ mDevice2 ≤ 1, 000, 000, 000, mDevice1 ≠ mDevice2)
	//Returns
	//장비 mDevice1에서 장비 mDevice2로 신호를 전송했을 때 전송 시간
	int s = h[mDevice1];	int e = h[mDevice2];
	return dfs(s, e);
}
int test(int mDevice) {
	//신호를 모니터링하는 장비를 장비 mDevice로 하고 테스트를 진행한다.
	//전송 신호가 장비 mDevice를 지나가고 전송 시간이 최대가 되도록 보내는 장비와 받는 장비를 선택하고 이때의 전송 시간을 반환한다. (본문 설명 참조)
	//mDevice는 이미 존재하는 장비 번호이다.
	//함수 호출 시, 이미 존재하는 장비는 2개 이상 있음을 보장한다.
	//Parameters
	//mDevice : 장비 번호(1 ≤ mDevice ≤ 1, 000, 000, 000)
	//Returns
	//장비 mDevice로 전송 신호가 지나갈 때 최대 전송 시간
	int d = h[mDevice];
}



#elif 0
#include<queue>
#include<unordered_map>
struct Cable {
	int Dest;
	int mTime;
	Cable(int x = 0, int y = 0) :Dest(x), mTime(y) {};
};
std::vector<Cable> P[10'001];
int pid;
std::unordered_map<int, int> umap;
int visited[10001], visid;
void init(int mDevice)
{
	pid = 0;
	umap.clear();
	P[0].clear();
	umap[mDevice] = pid++;
	return;
}
void connect(int mOldDevice, int mNewDevice, int mLatency)
{
	int id = umap[mOldDevice];
	P[id].push_back(Cable(pid, mLatency));
	P[pid].clear();
	P[pid].push_back(Cable(id, mLatency));
	umap[mNewDevice] = pid++;
	return;
}
void dfs(int s, int tg, int d, int& kq, int& kq2) {
	if (s == tg) {
		kq = d;
		return;
	}
	if (d > kq2) kq2 = d;
	visited[s] = visid;
	for (auto next : P[s]) {
		if (visited[next.Dest] == visid) continue;
		dfs(next.Dest, tg, d + next.mTime, kq, kq2);
	}
}
int measure(int mDevice1, int mDevice2)
{
	int dv1 = umap[mDevice1], dv2 = umap[mDevice2];
	++visid;
	int ret = 0, temp = 0;
	dfs(dv1, dv2, 0, ret, temp);
	return ret;
}
int test(int mDevice)
{
	std::priority_queue<int> pq;
	int ID = umap[mDevice];
	visited[ID] = ++visid;
	for (auto next : P[ID]) {
		int d, dmax = 0, temp = 0;
		dfs(next.Dest, -1, 0, temp, dmax);
		d = dmax + next.mTime;
		pq.push(d);
	}
	int ans = pq.top();
	pq.pop();
	if (pq.size() > 0) ans += pq.top();
	return ans;
}
#elif 0

// 매 timestamp가 증가할 때 마다 새롭게 우선순위를 찾아야 한다
// 가장 최근 게시글부터 모든 게시글을 순회하면서 현재 유저가 볼 수 있는 게시글 리스트 만듬
// timestamp 오름차순 순으로 게시글 정보(pid, uid, likE)를 관리한다
// --->>> 매번 feed 조회 시 유저가 볼 수 있는 게시글을 탐색해야 한다 --->>> 시간 초과 발생

// 매번 getFeed를 할 때 마다 모든 게시글을 순회하며 list를 만드는 것은 비효율적이다
// 대신 각 user 별로 작성한 post를 관리한다면 현재 user와 follow 한 user가 작성한 post만 조회하면 된다
// 먼저 post를 timestamp 기준으로 quick 정렬한다
// 1000초 이내 post는 like 기준으로 다시 quick 정렬한다
// 1000초 이내와 1000초 초과에 다른 룰을 적용해서 공통된 기준 score를 만들고 이 score를 기준으로 한 번만 정렬한다면? -> 시간 차이 크지 않음
// 정렬 대상 리스트를 최소로 줄여야 시간을 줄일 수 있다 -> 1000초 초과의 경우 각 유저 게시글 리스트가 이미 오름차순으로 정렬 돼 있다
// 필요한 1000초 초과 갯수만큼 각 유저별로 게시글을 선택해 정렬한다면 최적화이다

#include <malloc.h>
//#include <stdio.h>

#define MAX_STAMP 100001
#define MAX_USER 1001

int Like[MAX_STAMP];
int TimeStamp[MAX_STAMP];

int UserP[MAX_USER];
int PrevP[MAX_STAMP];

int User[MAX_USER][MAX_USER];
int UserIdx[MAX_USER];

int Score[MAX_STAMP];
int ScoreP[MAX_STAMP];

void scoreSort(int st, int ed) {
	register int stt = st + 1;
	register int edd = ed;
	register int pivot = st;
	register int temp;
	do {
		while (Score[stt] > Score[pivot] && stt <= ed) stt++;
		while (Score[edd] < Score[pivot] && edd >= st + 1) edd--;
		if (stt < edd) {
			temp = Score[stt];
			Score[stt] = Score[edd];
			Score[edd] = temp;
			temp = ScoreP[stt];
			ScoreP[stt] = ScoreP[edd];
			ScoreP[edd] = temp;
		}
	} while (stt <= edd);
	temp = Score[edd];
	Score[edd] = Score[pivot];
	Score[pivot] = temp;
	temp = ScoreP[edd];
	ScoreP[edd] = ScoreP[pivot];
	ScoreP[pivot] = temp;
	if (st < edd - 1)
		scoreSort(st, edd - 1);
	if (edd + 1 < ed && edd + 1 <= 9)
		scoreSort(edd + 1, ed);
}

void init(int N)
{
	for (register int i = 1; i <= N; i++) {
		User[i][0] = i;
		UserIdx[i] = 1;
		UserP[i] = 0;
	}
}

void follow(int uID1, int uID2, int timestamp)
{
	User[uID1][UserIdx[uID1]++] = uID2;
}

void makePost(int uID, int pID, int timestamp)
{
	Like[pID] = 0;
	TimeStamp[pID] = timestamp;
	PrevP[pID] = UserP[uID];
	UserP[uID] = pID;
}

void like(int pID, int timestamp)
{
	Like[pID]++;
}

void getFeed(int uID, int timestamp, int pIDList[])
{
	register int scoreIdx = 0;
	register int userLeng = UserIdx[uID];
	for (register int u = 0; u < userLeng; u++) {
		register int pid = UserP[User[uID][u]];
		while (pid != 0) {
			if (TimeStamp[pid] >= timestamp - 1000) {
				ScoreP[scoreIdx] = pid;
				Score[scoreIdx++] = Like[pid] * 1000000 + TimeStamp[pid];
			}
			else
				break;
			pid = PrevP[pid];
		}
	}
	register int remain = 10 - scoreIdx;
	if (scoreIdx < 10) {
		for (register int u = 0; u < userLeng; u++) {
			register int pid = UserP[User[uID][u]];
			register int count = remain;
			while (pid != 0) {
				if (count == 0)
					break;
				if (TimeStamp[pid] < timestamp - 1000) {
					ScoreP[scoreIdx] = pid;
					Score[scoreIdx++] = TimeStamp[pid];
					count--;
				}
				pid = PrevP[pid];
			}
		}
	}
	scoreSort(0, scoreIdx - 1);
	for (register int i = 0; i < 10; i++) {
		pIDList[i] = ScoreP[i];
	}
}
#elif 0
#include<iostream>
using namespace std;
//1. 게시글을 등록한다.각 게시글들은 등록되는 시점의 timestamp 를 가지고 있다.
//2. 다른 사용자를 “follow” 한다.“follow”를 하게 되면, 그 사용자의 게시글을 볼 수 있다.
//3. 특정 게시글에 “like”를 추가한다.
//4. 특정 사용자를 기준으로 자신이 게시한 글과 자신이 “follow” 한 사용자의 게시글 중 우선 순위가 높은 글부터 내림차순으로 최대 10 개의 게시글을 보여준다.
//각 게시글의 우선순위를 계산하는 방법은 다음과 같다.
//1. 게시된 지 1, 000 초 이내인 게시글이 그렇지 않은 게시글보다 우선순위가 높다.
//2. 게시된 지 1, 000 초 이내인 게시글들 중에는 “like” 가 많은 게시글의 우선순위가 높다.
//3. 게시된 지 1, 000 초 이내이면서 “like” 의 개수가 같은 게시글들 중에는 “timestamp” 가 높은 게시글의 우선순위가 높다.
//4. 게시된 지 1, 000 초를 초과한 게시글의 경우, “timestamp” 가 높은 게시글의 우선순위가 높다.
//[제약사항]
//1. 사용자 수 N 은 2 이상 1, 000 이하의 정수이다. (2 ≤ N ≤ 1, 000)
//2. timestamp 는 1 에서 시작하고 최대 100, 000 까지 증가한다.
//3. 모든 함수들은 timestamp 오름차순으로 호출된다.
//4. 모든 게시물들의 timestamp 는 서로 다르다.
//5. follow(), makePost(), like() 함수의 호출 횟수는 각각 100, 000 회 이하이다.
//6. getFeed() 함수의 호출 횟수는 5, 000 회 이하이다.
//7. 특정 사용자가 한번 “follow” 한 사용자를 다시 “follow” 하는 호출은 발생하지 않는다.
struct Node {
	int id, like, timestamp;
	struct Node* next, *prev;
}node[100'001];
struct List {
	Node* pHead = new Node;
	Node* pTail = new Node;
	void link(Node* f, Node* b) {
		f->next = b;
		b->prev = f;
	}
	void initialize() {
		link(pHead, pTail);
	}
	bool isEmpty() {
		return pHead->next == pTail;
	}
	void erase(Node* node) {
		link(node->prev, node->next);
	}
	void insert_back(Node* node) {
		link(pTail->prev, node);
		link(node, pTail->next);
	}
	void splice(List* list) {
		if (isEmpty())	return;
		link(pTail->prev, list->pHead->next);
		link(list->pTail->prev, pTail);
		list->initialize();
	}
}list[1'001];
int flw[1'001][1'001];
void init(int N) {
	//각 testcase 시작 시 초기화를 위해 1번 호출된다.
	//Parameters
	//N : 사용자 수(2 ≤ N ≤ 1, 000)
	memset(node, 0, sizeof(node));
	//memset(flw, 0, sizeof(flw));

	for (int i = 0; i <= N; i++) {
		list[i].initialize();
		flw[i][0] = 1;
		flw[i][flw[i][0]] = i;
	}

}
void follow(int uID1, int uID2, int timestamp) {
	//“uID1” 사용자가 “uID2” 사용자를 “follow” 한다.
	//“uID1” 사용자는 “uID2” 사용자의 모든 게시글을 볼 수 있다.
	//Parameters
	//uID1, uID2 : 사용자의 id(1 ≤ uID1, uID2 ≤ N)
	//timestamp : 현재 시점의 “timestamp”(1 ≤ timestamp ≤ 100, 000)
	flw[uID1][0]++;
	flw[uID1][flw[uID1][0]] = uID2;

}
void makePost(int uID, int pID, int timestamp) {
	//“uID” 사용자가 “pID” 게시글을 게시한다.
	//Parameters
	//uID : 사용자의 ID(1 ≤ uID ≤ N)
	//pID : 게시글의 ID(1 부터 오름차순으로 주어진다.)
	//timestamp : 현재 시점의 “timestamp”(1 ≤ timestamp ≤ 100, 000)
	node[pID] = { uID, 0, timestamp };
	list[uID].insert_back(&node[pID]);

}
void like(int pID, int timestamp) {
	//“pID” 게시글에 “like” 를 1 번 추가 한다.
	//“pID” 는 makePost() 에서 전달되었던 값으로만 주어 진다.
	//Parameters
	//pID : “like” 를 추가할 게시글의 pID
	//timestamp : 현재 시점의 “timestamp”(1 ≤ timestamp ≤ 100, 000)
	node[pID].like++;
}
void getFeed(int uID, int timestamp, int pIDList[]) {
	//현재 “timestamp” 를 기준으로 “uID” 사용자에게 보여지는 최대 10 개의 게시글의 “pID” 들을 찾아 우선순위의 내림차순으로 “pIDList[]” 배열에 저장하여 반환 한다.
	//Parameters
	//uID : 사용자의 id(1 ≤ uID ≤ N)
	//timestamp : 현재 시점의 timestamp(1 ≤ timestamp ≤ 100, 000)
	//pIDList[] : 보여지는 게시글의 pID 들을 저장하는 배열	

	int k = flw[uID][0];

	for (int i = 1; i <= k; i++) {
		int id = flw[uID][i];


	}
}

#elif 0
#include<iostream>
using namespace std;
struct C {
	int Price, Performance;
	bool operator < (const struct C& a)const {
		return a.Performance != Performance ? a.Performance > Performance : a.Price < Price;
	}
};
struct Result {
	int mPerformance, mPrice;
};
int ADD;
int cnt[2][3];

void init(int mCharge) {
	//각 테스트 케이스의 처음에 호출되며, 창고 간 운송료를 알려준다.
	//기존의 부품 목록은 모두 초기화된다.
	//Parameters
	//mCharge : 창고 간 운송료(1 ≤ mCharge ≤ 100, 000)
	ADD = mCharge;
	memset(cnt, 0, sizeof(cnt));
}
int stock(int mType, int mPrice, int mPerformance, int mPosition) {
	//부품 목록에 새로운 부품을 추가한다.	//종류와 가격, 성능이 모두 같은 입력이 testcase 내에서 중복되어 들어오지 않는다.
	//Parameters
	//mType : 부품 종류(0 ≤ mType ≤ 2)
	//mPrice : 부품 가격(1 ≤ mPrice ≤ 100, 000)
	//mPerformance : 부품 성능(1 ≤ mPerformance ≤ 1, 000, 000)
	//mPosition : 부품이 있는 창고 번호(0 ≤ mPosition ≤ 1)
	//Returns
	//입고된 창고에 동일한 type인 부품의 가짓수를 반환한다.
	return cnt[mPosition][mType]++;
}
Result order(int mBudget) {
	//소비자의 예산이 mBudget일 때 예산 안에서 최대의 성능을 내는 부품 조합을 선택한다.	//같은 성능이면 가격이 가장 낮은 조합을 선택한다.	//부품이 출고되면 즉각 채워 넣으므로 품절되지 않는다.
	//Parameters
	//mBudget : 소비자의 예산(1 ≤ mBudget ≤ 500, 000)
	//Returns
	//주어진 예산으로 조립이 가능할 경우, Result 구조체의 mPerformance에 컴퓨터의 성능을, mPrice에 컴퓨터의 가격을 반환한다.
	//주어진 예산으로 조립이 불가능할 경우, mPerformance와 mPrice 모두 0을 반환한다.
	//mBudget내에서 -> BFC, DFC, DP 등 완탐방식으로? 
	//Performance가 높게(최하 성능의 부품이 가장 높도록)?  
}
#elif 0
#include <vector>
#include <algorithm>
using namespace std;
struct Result
{
	int mPrice;
	int mPerformance;
};
vector<Result> v[2][3];	// v[창고][종류] -> 가격+성능
int ADD, id;

void init(int mCharge)
{
	ADD = mCharge;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 3; j++)
			v[i][j].clear();
}

int stock(int mType, int mPrice, int mPerformance, int mPosition)
{
	v[mPosition][mType].push_back({ mPrice, mPerformance });
	return v[mPosition][mType].size();
}

Result order(int mBudget)
{
	Result res = { 0, 0 };
	int s = 0, e = 1000001;
	while (s <= e) {
		int mp[2][3] = { { 99999999, 99999999, 99999999 }, { 99999999, 99999999, 99999999 } };
		int m = (s + e) >> 1;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 3; j++)
				for (auto k : v[i][j]) {
					if (k.mPerformance >= m) mp[i][j] = min(mp[i][j], k.mPrice);
				}
		int A = mp[0][0] + mp[0][1] + mp[0][2];
		int B = mp[1][0] + mp[1][1] + mp[1][2];
		int C = min(mp[0][0], mp[1][0]) + min(mp[0][1], mp[1][1]) + min(mp[0][2], mp[1][2]) + ADD;
		int MIN = min(min(A, B), C);
		if (MIN <= mBudget) {
			res = { MIN, m };
			s = m + 1;
		}
		else {
			e = m - 1;
		}
	}
	return res;
}
#elif 0
struct Result { int row;	int col; };
struct Bacteria { int id;	int size;	int time; };

int m[100][100];

int main() {

	int N = 10, r = 5, c = 5;
	int k = 0;
	while (N--) {
		//k=0

		int i = r - k;
		int j = c - k;

		for (
			; i < 10; i++) {
			for (int j = 0; j < 10; j++) {

			}
		}
	}
	return 1;
}
void init(int N) {
	//각 테스트 케이스의 처음에 호출된다.
	//N* N 크기의 배양기에 박테리아는 없다.
	//시각은 0 이다.
	//Parameters
	//N : 배양기 한 변의 길이(10 ≤ N ≤ 200)
}
Result putBacteria(int mTime, int mRow, int mCol, Bacteria mBac) {
	//mTime 시각에(mRow, mCol) 셀에 박테리아가 투입된다.
	//박테리아는 mBac.id 종류, mBac.size 번식 크기, mBac.time 수명 시간을 가지고 있다.
	//mTime에 소멸되는 박테리아가 있을 경우, 박테리아 소멸 된 후 투입된다.
	//투입된 박테리아는 순식간에 번식을 한다. (번식 방법은 본문 참고)
	//- 번식 크기만큼 번식에 성공하면, 마지막 번식 위치를 Result 에 저장하고 반환한다.
	//- 번식 크기만큼 번식에 실패하면, 박테리아는 소멸하고[Result.row = 0, Result.col = 0]을 반환한다.
	//배양기의(mRow, mCol) 셀에 이미 다른 종류의 박테리아가 살아 있어 빈 셀이 아닐 경우, 아무 동작도 하지 않고[Result.row = 0, Result.col = 0]을 반환한다.
	//mBac.id 는 맨 처음 호출할 때 1이고 그 다음 호출할 때마다 1씩 증가한다.
	//mBac.id 박테리아는 mBac.size 번식 크기만큼 번식에 성공할 경우(mTime + mBac.time) 시각에 소멸한다.
	//Parameters
	//mTime : 박테리아가 투입된 시각(1 ≤ mTime ≤ 1, 000, 000)
	//mRow : 박테리아가 투입된 세로 위치(1 ≤ mRow ≤ N)
	//mCol : 박테리아가 투입된 가로 위치(1 ≤ mCol ≤ N)
	//mBac.id : 투입된 박테리아의 종류(1 ≤ mBac.id ≤ 3, 000)
	//mBac.size : 투입된 박테리아의 번식 크기(1 ≤ mBac.size ≤ 2, 000)
	//mBac.time : 투입된 박테리아의 수명 시간(1 ≤ mBac.time ≤ 20, 000)
	//Returns
	//Result.row : 마지막으로 번식한 row 위치, 번식에 실패할 경우 0
	//Result.col : 마지막으로 번식한 col 위치, 번식에 실패할 경우 0
}
int killBacteria(int mTime, int mRow, int mCol) {
	//mTime 시각에(mRow, mCol) 셀에 살아 있는 박테리아를 소멸시키고, 박테리아 종류를 반환한다.
	//(mRow, mCol) 셀에 박테리아가 없을 경우 아무 동작도 하지 않고 0 을 반환한다.
	//mTime 시각에(mRow, mCol) 셀에 있는 박테리아가 소멸된 경우, 아무 동작도 하지 않고 0 을 반환한다.
	//Parameters
	//mTime : 박테리아를 소멸시키는 시각(1 ≤ mTime ≤ 1, 000, 000)
	//mRow : 박테리아를 소멸시키는 세로 위치(1 ≤ mRow ≤ N)
	//mCol : 박테리아를 소멸시키는 가로 위치(1 ≤ mCol ≤ N)
	//Returns

}
int checkCell(int mTime, int mRow, int mCol) {
	//mTime 시각에 배양기(mRow, mCol) 위치에 박테리아의 종류를 반환한다.
	//박테리아가 없을 경우 0 을 반환한다.
	//mTime 시각에(mRow, mCol) 셀이 있는 박테리아가 소멸된 경우 0 을 반환한다.
	//Parameters
	//mTime : 셀의 박테리아 종류를 파악하는 시각(1 ≤ mTime ≤ 1, 000, 000)
	//mRow : 셀의 세로 위치(1 ≤ mRow ≤ N)
	//mCol : 셀의 가로 위치(1 ≤ mCol ≤ N)
	//Returns
	//박테리아 종류, 박테리아가 없거나 소멸된 경우 0
}

#elif 0
#define MAXN 50000
#define HASH_SIZE (26426) 

struct Node {
	int index;
	Node* next, *prev;
} node_pool[MAXN], table[HASH_SIZE];

int node_cnt;
#define node() &node_pool[node_cnt++]

int N;
char data[MAXN + 1];

void add(Node* node, int key) {
	node->next = table[key].next;

	if (table[key].next)
		table[key].next->prev = node;

	node->prev = &table[key];
	table[key].next = node;
}

#define hash(str, i) (str[i] - 'a') << 10 | (str[i + 1] - 'a') << 5 | (str[i + 2] - 'a')

void init(int N, char str[])
{
	::N = N, node_cnt = 0;

	for (int i = 0; i < HASH_SIZE; i++)
		table[i].next = 0;

	for (int i = 0; i < N - 2; i++) {
		int key = hash(str, i);
		Node* node = node();
		node->index = i;
		add(node, key);
	}

	for (int i = 0; i <= N; i++)
		data[i] = str[i];
}

void del(Node* node) {
	node->prev->next = node->next;
	if (node->next) node->next->prev = node->prev;
}

int indices[MAXN];
#define swap(a,b) { int t; t=a; a=b; b=t;}
void quick_sort(int start, int end) {
	if (start >= end) return;
	int pivot = start, left = start + 1, right = end;
	while (left <= right) {
		while (indices[pivot] >= indices[left] && left <= end) left++;
		while (indices[pivot] <= indices[right] && right > start) right--;
		if (left <= right) swap(indices[left], indices[right])
		else swap(indices[pivot], indices[right])
	}
	quick_sort(start, right - 1);
	quick_sort(right + 1, end);
}

int change(char str1[], char str2[])
{
	int key = hash(str1, 0);
	int index = -3, ans = 0, len = 0;
	for (Node* node = table[key].next; node; node = node->next)
		indices[len++] = node->index;
	quick_sort(0, len - 1);
	for (int i = 0; i < len; i++) {
		if (indices[i] - index < 3) continue;
		index = indices[i];
		data[index] = str2[0], data[index + 1] = str2[1], data[index + 2] = str2[2];
		ans++;
		for (int j = index - 2; j <= index + 2; j++) {
			if (j < 0 || j >= N - 2) continue;
			del(&node_pool[j]);
			add(&node_pool[j], hash(data, j));
		}
	}
	return ans;
}
void result(char ret[])
{
	for (int i = 0; i <= N; i++) ret[i] = data[i];
}


#elif 0
#include<iostream>
#include<cstring>
using namespace std;
char s[50000];
int N;


//1. 각 테스트 케이스 시작 시 init() 함수가 호출된다.
//2. 문자열의 길이는 최대 50, 000 이다.
//3. 각 테스트 케이스에서 문자열은 영어 소문자로 이루어져 있으며 항상 ’\0’ 으로 끝난다.
//4. 변환할 문자열의 길이는 항상 3 이다.
//5. 각 테스트 케이스에서 change() 함수가 호출되는 횟수는 최대 50, 000 이다.result() 함수는 1 번만 호출 된다.

void init(int N, char init_string[]) {
	::N = N;
	//각 테스트 케이스의 처음에 호출된다.
	//변환을 할 원본 문자열이 주어진다.
	//Parameters
	//N : 원본 문자열의 길이
	//init_string[] : 원본 문자열.	
	int i = 0;
	for (; i < N; i++)		s[i] = init_string[i];
	s[i] = '\0';
}
int change(char string_A[], char string_B[]) {
	//string_A 와 일치하는 부분문자열을 string_B로 바꾼다.
	//Parameters
	//string_A : 규칙을 이루는 문자열에서 찾아 변환시킬 문자열이다.
	//string_B : string_A 를 대체할 문자열
	//(string_A, string_B) 의 각 길이는 3이다.
	//Returns
	//num : 변경한 부분 문자열의 개수를 반환한다.


}
void result(char ret[]) {
	//최종 문자열 상태를 ret 배열에 반환한다.
	//이 함수는 각 테스트케이스 마지막에 한번만 호출된다.
}

#elif 0
//BILL = Bool Int Long Long
#include <iostream>
#include <memory.h>
using namespace std;
#define r register
#define rint r int

int a[26];
int b[26][26];
int c[26][26][26];
int d[26][26][26][26];
char str[300000], *s; ;
int ibegin, rev, istr;

void init(char mStr[]) {
	istr = 0;
	rev = 1;
	ibegin = 100000;
	s = &str[ibegin];

	memset(a, 0, sizeof(a));
	memset(b, 0, sizeof(b));
	memset(c, 0, sizeof(c));
	memset(d, 0, sizeof(d));
	rint len = 0;

	for (; mStr[len]; len++)
		s[len] = mStr[len] - 'a';

	istr = len;
	for (rint i = 0; i < len; i++) {
		a[s[i]]++;

		if (i < len - 1) b[s[i]][s[i + 1]]++;
		if (i < len - 2) c[s[i]][s[i + 1]][s[i + 2]]++;
		if (i < len - 3) d[s[i]][s[i + 1]][s[i + 2]][s[i + 3]]++;
	}
}

void appendWord(char mWord[]) {
	rint len; char x[5] = {};
	for (len = 0; mWord[len]; len++) x[len] = mWord[len] - 'a';
	if (rev) {
		for (rint i = 0; i < len; i++) {
			s[istr] = x[i]; s[istr + 1] = 0;
			d[s[istr - 3]][s[istr - 2]][s[istr - 1]][s[istr]]++;
			c[s[istr - 2]][s[istr - 1]][s[istr]]++;
			b[s[istr - 1]][s[istr]]++;
			a[s[istr]]++;
			istr++;
		}
	}
	else {
		for (rint i = 0; i < len; i++) {
			s = &str[--ibegin]; s[0] = x[i]; istr++;
			a[s[0]]++;
			b[s[0]][s[1]]++;
			c[s[0]][s[1]][s[2]]++;
			d[s[0]][s[1]][s[2]][s[3]]++;
		}
	}
}

void cut(int k) {
	if (rev) {
		for (rint h = 0; h < k; h++) {
			istr--;
			d[s[istr - 3]][s[istr - 2]][s[istr - 1]][s[istr]]--;
			c[s[istr - 2]][s[istr - 1]][s[istr]]--;
			b[s[istr - 1]][s[istr]]--;
			a[s[istr]]--; s[istr] = 0;
		}
	}
	else {
		for (rint h = 0; h < k; h++) {
			a[s[0]]--;
			b[s[0]][s[1]]--;
			c[s[0]][s[1]][s[2]]--;
			d[s[0]][s[1]][s[2]][s[3]]--;
			ibegin++; istr--; s[0] = 0; s++;
		}
	}
}

void reverse() { rev = !rev; }

int countOccurrence(char mWord[]) {
	rint len, res; char x[5] = { }; for (len = 0; mWord[len]; len++) x[len] = mWord[len] - 'a';
	switch (len) {
	case 1: res = a[x[0]]; break;
	case 2: if (rev) res = b[x[0]][x[1]]; else res = b[x[1]][x[0]]; break;
	case 3: if (rev) res = c[x[0]][x[1]][x[2]]; else res = c[x[2]][x[1]][x[0]]; break;
	case 4: if (rev) res = d[x[0]][x[1]][x[2]][x[3]]; else res = d[x[3]][x[2]][x[1]][x[0]]; break;
	}
	return res;
}
#elif 0
#include<deque>
#include<string>
#include<string.h>
#include<algorithm>
using namespace std;

bool rev;
deque<char> dq;
int cnt[27 * 27 * 27 * 27]; // a:1 ~ z:26

void updateCnt(int c) {
	int idx = rev ? 0 : dq.size() - 1;
	int hash = 0, i = 0, base = 1;
	for (; i < 4; i++, base *= 27) {
		if (idx < 0 || idx >= dq.size()) break;

		if (rev) hash = hash * 27 + dq[idx++] - 96; // 'a' ascii = 97
		else hash += (dq[idx--] - 96) * base;

		cnt[hash] += c;
	}
}

void init(char mStr[])
{
	rev = 0;
	dq.clear();
	memset(cnt, 0, sizeof(cnt));
	for (int i = 0; mStr[i]; i++) {
		dq.push_back(mStr[i]);
		updateCnt(1);
	}
}

void appendWord(char mWord[])
{
	for (int i = 0; mWord[i]; i++) {
		if (rev) dq.push_front(mWord[i]);
		else dq.push_back(mWord[i]);
		updateCnt(1);
	}
}

void cut(int k)
{
	for (int i = 0; i < k; i++) {
		updateCnt(-1);
		if (rev) dq.pop_front();
		else dq.pop_back();
	}
}

void reverse()
{
	rev = !rev;
}

int countOccurrence(char mWord[])
{
	string str = mWord;
	if (rev) reverse(str.begin(), str.end());

	int hash = 0;
	for (auto c : str) hash = hash * 27 + c - 96;
	return cnt[hash];
}
#elif 1
#include <deque>
#include <unordered_map>
#include <string>
#include <algorithm>
using namespace std;

deque<char> dq;
unordered_map<string, int> cnt;
bool R;

void update(int n) {
	int idx;
	if (R)
		idx = 0;
	else
		idx = dq.size() - 1;

	string str;
	for (int i = 0; i < 4; i++) {
		if (idx < 0 || idx >= dq.size())
			break;

		if (R)
			str += dq[idx++];
		else
			str.insert(str.begin(), dq[idx--]);

		cnt[str] += n;
	}
}
void init(char mStr[])
{
	cnt.clear();
	dq.clear();
	R = false;
	while (*mStr != '\0') {
		dq.push_back(*mStr++);
		update(1);
	}
}
void appendWord(char mWord[])
{
	while (*mWord != '\0') {
		if (R) dq.push_front(*mWord++);
		else dq.push_back(*mWord++);
		update(1);
	}
}
void cut(int k)
{
	while (k--) {
		update(-1);
		if (R) dq.pop_front();
		else dq.pop_back();
	}
}
void reverse()
{
	R = !R;
}
int countOccurrence(char mWord[])
{
	string str = mWord;
	if (R) reverse(str.begin(), str.end());
	return cnt[str];
}
#elif 0
#include<cstring>
#include<iostream>
using namespace std;
/*
1. 문자열 뒤에 짧은 문자열을 붙인다.
2. 문자열 끝 부분을 삭제한다.
3. 문자열을 뒤집는다.
4. 짧은 문자열이 주어졌을 때, 해당 문자열의 등장 횟수를 센다.

1. 각 테스트 케이스 시작 시 init() 함수가 호출된다.
2. 각 테스트 케이스에서 appendWord() 함수의 호출 횟수는 30,000 이하이다.
3. 각 테스트 케이스에서 cut() 함수의 호출 횟수는 100 이하이다.
4. 각 테스트 케이스에서 reverse() 함수의 호출 횟수는 30,000 이하이다.
5. 각 테스트 케이스에서 countOccurrence() 함수의 호출 횟수는 30,000 이하이다.
*/

char* s[30001];

void init(char mStr[]) {
	//각 테스트 케이스의 처음에 호출된다.
	//해당 테스트 케이스에서 초기 문자열은 mStr 이다.
	//문자열 mStr 은 영문 소문자만으로 구성되어 있으며 ‘＼0’으로 끝난다.
	//mStr 의 길이는 1 이상 30, 000 이하이다.
	//Parameters
	//mStr : 초기 문자열(1 ≤ | mStr | ≤ 30, 000, | a | 는 문자열 a의 길이를 의미한다.)
}

void appendWord(char mWord[]) {
	/*현재 문자열의 끝에 mWord 문자열을 붙인다.
	문자열 mWord 은 영문 소문자만으로 구성되어 있으며 ‘＼0’으로 끝난다.
	mWord 의 길이는 1 이상 4 이하이다.
	Parameters
	mWord : 끝에 붙일 문자열(1 ≤ | mWord | ≤ 4)
	*/
}
void cut(int k) {
	/*현재 문자열의 끝에서부터 k 개 문자를 제거한다.
	이 함수가 호출되는 시점에,
	k 는 현재 문자열 길이보다 작음이 보장된다.
	Parameters
	k : 제거할 문자의 개수(1 ≤ k < 문자열 길이)	*/
}
void reverse() {
	//현재 문자열 전체를 뒤집는다.
}
int countOccurrence(char mWord[]) {
	//현재 문자열에서 mWord 문자열이 부분 문자열(substring)로 등장하는 횟수를 반환한다.
	//문자열 mWord 는 영문 소문자만으로 구성되어 있으며 ‘＼0’으로 끝난다.
	//mWord 의 길이는 1 이상 4 이하이다.
	//문자열이 겹치는 경우는 중복해서 센다.
	//예를 들어 “ababa”에서 “aba”가 등장하는 횟수는 2회이다.
	//현재 문자열 길이에 비례하는 탐색 시간을 가지는 경우 시간 초과가 발생할 수 있다.
	//Parameters
	//mWord : 등장 빈도를 계산할 단어(1 ≤ | mWord | ≤ 4)
	//Returns
	//현재 문자열에서 mWord 단어의 등장 횟수를 반환한다.
	return 1;
}


int main() {
	char a[100] = "ababa";
	char b[100] = "aba";
	//cout << a << " " << b << endl;

	/*char* s = a;
	for (int i = 0; i < 5; i++) {
		cout << s++ << " ";
	}*/

	char* s = a;
	while (s = strstr(s, b)) {
		cout << s++ << endl;
	}


	return 1;
}
#elif 0
#include<iostream>
using namespace std;

struct Trie {
	bool end;
	Trie* ch[11];
	Trie() {
		for (int i = 0; i < 11; i++)
			ch[i] = nullptr;
		end = 0;
	}
	~Trie() {
		for (int i = 0; i < 11; i++)
			if (ch[i])
				delete ch[i];
		end = 0;
	}
	bool insert(const char* s) {
		if (!*s) {
			if (this->end)
				return false;
			else {
				this->end = 1;
				return true;
			}
		}

		int k = *s - '0';
		if (!ch[k]) {
			if (this->end)
				return false;

			ch[k] = new Trie;
		}
		return ch[k]->insert(s + 1);
	}
	bool find(const char* s) {
		if (!*s) {
			if (end) return true;
			else return false;
		}

		int k = *s - '0';
		if (!ch[k])		return false;
		return ch[k]->find(s + 1);
	}
	bool check(const char* s) {
		if (!*s) {
			return true;	// 접두어가 같다
		}

		int k = *s - '0';
		if (!ch[k])
			return false;	// 접두어가 다르다
		return
			ch[k]->check(s + 1);
	}
};

char s[11];
int main() {
	/*
	첫째 줄에 테스트 케이스의 개수 t가 주어진다. (1 ≤ t ≤ 50)
	각 테스트 케이스의 첫째 줄에는 전화번호의 수 n이 주어진다. (1 ≤ n ≤ 10000)
	다음 n개의 줄에는 목록에 포함되어 있는 전화번호가 하나씩 주어진다.
	전화번호의 길이는 길어야 10자리이며, 목록에 있는 두 전화번호가 같은 경우는 없다.
	*/

	setbuf(stdout, NULL);
	freopen("sample_input.txt", "r", stdin);

	int t, n;
	scanf(" %d", &t);
	for (int i = 0; i < t; i++) {
		scanf(" %d", &n);
		Trie trie;
		int ans = 0;
		while (n--) {
			scanf(" %s", s);

			if (!trie.insert(s)) {
				ans = 1;
				break;
			}
		}
		if (ans == 1)
			printf("NO - %d\n", i);
		else
			printf("YES- %d\n", i);
		ans = 0;
	}
	return 1;
}

#elif 0 

#include<iostream>
#include<unordered_map>
#include<unordered_set>
#include<set>
using namespace std;

set<string> 	s;

int main() {

	setbuf(stdout, NULL);
	freopen("sample_input.txt", "r", stdin);

	int N, M, ret = 0;
	string str;

	scanf("%d %d ", &N, &M);
	for (int i = 0; i < N; i++) {
		cin >> str;
		s.insert(str);
	}

	for (int i = 0; i < M; i++) {
		cin >> str;
		if (s.count(str)) 			ret++;
	}

	cout << ret << endl;

	return 0;
}


#elif 0
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif _CRT_SECURE_NO_WARNINGS

#include<iostream>
using namespace std;

char str[501];


struct Trie {
	Trie* ch[26];
	bool end;
	Trie() {
		end = 0;
		//memset(ch, (int)nullptr, sizeof(ch));
		for (int i = 0; i < 26; i++)
		{
			ch[i] = nullptr;
		}
	}
	~Trie() {
		for (int i = 0; i < 26; i++)
		{
			if (ch[i])
			{
				delete ch[i];
			}
		}
	}
	void insert(const char* s) {
		if (!*s) {
			end = 1;
			return;
		}

		int a = *s - 'a';
		if (!ch[a])			ch[a] = new Trie;

		ch[a]->insert(s + 1);
	}
	bool find(const char* s) {
		if (!*s) {
			if (end)			return true;
			else				return false;
		}

		int a = *s - 'a';
		if (!ch[a])			return false;

		return ch[a]->find(s + 1);
	}
};
int main() {

	setbuf(stdout, NULL);
	freopen("sample_input.txt", "r", stdin);


	int N, M, ret = 0;
	Trie trie;

	scanf("%d %d ", &N, &M);
	for (int i = 0; i < N; i++) {
		scanf("%s ", str);
		trie.insert(str);
	}

	for (int i = 0; i < M; i++) {
		scanf("%s ", str);
		if (trie.find(str)) {
			ret++;
			//cout << str << endl;
		}
	}

	cout << ret << endl;

	return 0;
}

#elif 0
#include<bits/stdc++.h>
#define tej __attribute((optimize("Ofast")))
using namespace std;
struct data1 {
	int arr[3];
};
struct compare {
	bool operator()(const data1& a, const data1& b) const {
		return a.arr[0] > b.arr[0];
	}
};
vector<data1> v[101];
void init(int N, int K, int sCity[], int eCity[], int mCost[], int mTime[]) {
	for (int i = 0; i < N; i++) v[i].clear();
	for (int i = 0; i < K; i++) {
		v[sCity[i]].push_back({ mTime[i], mCost[i], eCity[i] });
	}
}

void add(int sCity, int eCity, int mCost, int mTime) {
	v[sCity].push_back({ mTime, mCost, eCity });
	return;
}

int cost(int M, int sCity, int eCity) {
	priority_queue<data1, vector<data1>, compare> pq;
	pq.push({ 0,0,sCity });
	while (!pq.empty()) {
		data1 temp = pq.top();
		if (temp.arr[2] == eCity) return  temp.arr[0];
		pq.pop();
		for (auto i : v[temp.arr[2]]) {
			if ((i.arr[1] + temp.arr[1]) > M) continue;
			pq.push({ temp.arr[0] + i.arr[0], temp.arr[1] + i.arr[1], i.arr[2] });
		}
	}
	return -1;
}
#elif 0
#include<map>
#include<iostream> 
//#include<pair> 
using namespace std;

map<int, pair<int, int>>	c[105];

map<int, int>	m[10];

void init(int N, int K, int sCity[], int eCity[], int mCost[], int mTime[]) {

	for (int i = 0; i < N; i++) 		c[i].clear();

	for (int i = 0; i < K; i++) {
		c[sCity[i]].insert({ eCity[i], {mCost[i], mTime[i]} });
	}



	//cout << c[0].first 


}

int main() {


	c[0].insert({ 1, {1, 1} });
	c[0].insert({ 3, {3, 3} });
	c[0].insert({ 0, {0, 0} });
	c[0].insert({ 2, {2, 2} });
	c[0].insert({ 2, {2, 2} });
	c[0].insert({ 2, {2, 2} });
	c[0].insert({ 2, {2, 2} });

	//c[0].emplace(1, 2, 3);
	//c[2].emplace(4, 5, 6);
	//auto& [e, c, t] : c[0];

	for (const auto& it : c[0]) {
		int key = it.first;
		int v1 = it.second.first;
		int v2 = it.second.second;
		cout << key << "\t" << v1 << '\t' << v2 << endl;

	}
	return 1;
}

#elif 0
int mstrlen(const char* s) {
	int len = 0;
	while (s[len] != '\0')
		len++;
	return len;
}

void mstrcpy(char* dest, const char* src) {
	int i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = src[i];
}

#define MAX_N 2000
#define HASH_C 4001

int temp[HASH_C][HASH_C];

struct Result {
	int correct_cnt;
	char correct[5][11];
} result[HASH_C];

struct lastFind {
	int time;
	char word[11];
} last[MAX_N];

unsigned long hash(const char* str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
	{
		hash = (((hash << 5) + hash) + c) % HASH_C;
	}

	return hash % HASH_C;
}


bool isCandidate(char* ori, char* correct)
{
	int cnt = 0;
	int diff_value = mstrlen(correct) - mstrlen(ori);

	if (diff_value < 0) {
		while (*ori) {
			if (*ori != *correct)
				cnt++;
			else
				correct++;
			ori++;
		}
	}
	else if (diff_value > 0) {
		while (*correct) {
			if (*ori != *correct)
				cnt++;
			else
				ori++;
			correct++;
		}
	}
	else {
		while (*ori) {
			if (*ori++ != *correct++)
				cnt++;
		}
	}

	return cnt == 1 ? true : false;
}

void init(int n) {
	temp[HASH_C][HASH_C] = { 0 };
	result[HASH_C] = { 0 };
	last[MAX_N] = { 0 };
}

int search(int mId, int searchTimestamp, char searchWord[11], char correctWord[5][11])
{
	int ret;
	int i;
	int key = hash(searchWord);

	if (searchTimestamp - last[mId].time <= 10) {
		if (isCandidate(searchWord, last[mId].word)) {
			int hkeyWrong = hash(last[mId].word);
			temp[key][hkeyWrong]++;
			if (temp[key][hkeyWrong] == 3) {
				mstrcpy(result[hkeyWrong].correct[result[key].correct_cnt], searchWord);
				result[hkeyWrong].correct_cnt++;
			}
		}
	}

	ret = result[key].correct_cnt;
	for (i = 0; i < ret; i++) {
		mstrcpy(correctWord[i], result[key].correct[i]);
	}

	last[mId].time = searchTimestamp;
	mstrcpy(last[mId].word, searchWord);

	return ret;
}
#elif 0
#include<unordered_map>
#include<unordered_set>
#include<string>
using namespace std;
int pre[2001];
char word[50001][11];
unordered_map<string, unordered_map<string, unordered_set<int>>> um;

void init(int N) {
	um.clear();
	memset(pre, -0x3f, sizeof(pre));
}

int check(char* s, char* t) {
	int n = strlen(s);
	int m = strlen(t);

	if (abs(n - m) > 1) return 0;

	if (n == m) {
		int cnt = 0;
		for (int i = 0; i < n; i++) {
			cnt += s[i] == t[i];
		}
		return cnt == n - 1;
	}
	else if (n > m) {
		swap(s, t);
		swap(n, m);
	}

	int le = 0, ri = n - 1;
	while (le < n && s[le] == t[le]) le++;
	while (ri >= 0 && s[ri] == t[ri + 1]) ri--;
	return le >= ri;
}

int cnt = 0;
int search(int mId, int searchTimestamp, char searchWord[11], char correctWord[5][11]) {
	int ret = 0;
	cnt++;

	strcpy(word[searchTimestamp], searchWord);

	auto i = um.find(searchWord);
	if (i != um.end()) {
		unordered_map<string, unordered_set<int>>& v = i->second;
		for (auto& i : v) {
			if (i.second.size() >= 3) {
				strcpy(correctWord[ret++], i.first.c_str());
			}
		}
	}

	if (searchTimestamp - pre[mId] <= 10) {
		if (check(searchWord, word[pre[mId]]))
			um[word[pre[mId]]][searchWord].insert(mId);
	}

	pre[mId] = searchTimestamp;
	return ret;
}
#elif 0
#include <bits/stdc++.h>
#include <unordered_set>
#include <set>
using namespace std;
const int MAX = 2005;
pair<int, string> lstsearch[MAX];
map<pair<string, string>, unordered_set<int>> cand;
map<string, vector<string>> cor;


void init(int n) {
	cand.clear();
	cor.clear();
	for (int i = 1; i <= n; i++)
		lstsearch[i] = { -11,"" };
}

bool istypo(string a, string b) {

	int n = a.length();
	int m = b.length();
	if (n > m) {
		swap(a, b);
		swap(n, m);
	}
	if (n == m) {
		int cnt = 0;
		for (int i = 0; i < n; i++)
			if (a[i] != b[i])cnt++;

		return cnt == 1;
	}
	if (n == m - 1) {
		int i = 0, j = 0;
		while (i < n && j < m) {
			if (a[i] != b[j])j++;
			else {
				i++;
				j++;
			}
		}
		return (i == n);
	}

	return false;
}

int search(int mId, int searchTimestamp, char searchWord[11], char correctWord[5][11]) {

	if (lstsearch[mId].first >= searchTimestamp - 10 && istypo(lstsearch[mId].second, string(searchWord))) {
		string typo = lstsearch[mId].second;
		string corr(searchWord);

		if (cand[{typo, corr}].size() < 3) {
			cand[{typo, corr}].insert(mId);
			if (cand[{typo, corr}].size() == 3)
				cor[typo].push_back(corr);
		}

	}

	lstsearch[mId] = { searchTimestamp,string(searchWord) };

	int cnt = 0;
	for (auto k : cor[string(searchWord)]) {
		strcpy(correctWord[cnt++], k.c_str());
	}
	return cnt;


}
#elif 0
#include<iostream>
using namespace std;


struct Trie {
	Trie* ch[26];
	bool end;
	Trie() {
		end = 0;
		memset(ch, (int)nullptr, sizeof(ch));
	}
	~Trie() {
		for (int i = 0; i < 26; i++)
		{
			if (ch[i])
			{
				delete ch[i];
			}
		}
	}
	void insert(const char* s) {
		if (*s == '\0') {
			this->end = true;
		}
		else {
			int a = *s - 'a';
			if (!ch[a])
				ch[a] = new Trie;
			ch[a]->insert(s + 1);
		}
	}
	bool find(const char* s) {
		if (*s == '\0') {
			if (end)
				return true;
			else
				return false;
		}
		else {
			int a = *s - 'a';
			if (!ch[a])
				return false;
			else
				ch[a]->find(s + 1);
		}
	}
};

int main() {

	Trie* root = new Trie;

	root->insert("galaxy");
	root->insert("galacy");
	root->insert("glacy");
	root->insert("gallaxy");
	root->insert("galaxy");
	root->insert("galaxy");
	root->insert("glaxy");
	root->insert("glaxy");
	root->insert("galaxy");

	cout << root->find("galaxy") << endl;
	cout << root->find("glaxy") << endl;
	cout << root->find("gaxy") << endl;
	cout << root->find("galacy") << endl;
	cout << root->find("") << endl;
	cout << root->find("abc") << endl;


	return 1;
}

#elif 0
#include<iostream>
using namespace std;

struct Trie {
	bool is_terminal; // this represents end of string
	Trie* children[ALPHABETS];

	// Constructor
	Trie() : is_terminal(false) {
		memset(children, 0, sizeof(children));
	}

	// Delete all children
	~Trie() {
		for (int i = 0; i < ALPHABETS; ++i) {
			if (children[i])
				delete children[i];
		}
	}

	void insert(const char* key) {
		if (*key == '\0') {
			is_terminal = true;
		}
		else {
			int index = char_to_index(*key);

			if (children[index] == 0)
				children[index] = new Trie();
			children[index]->insert(key + 1);
		}
	}

	Trie* find(const char* key) {
		if (*key == 0) {
			return this;
		}

		int index = char_to_index(*key);
		if (children[index] == 0) {
			return NULL;
		}

		return children[index]->find(key + 1);
	}

	bool string_exist(const char* key) {
		if (*key == 0 && is_terminal) {
			return true;
		}

		int index = char_to_index(*key);
		if (children[index] == 0) {
			return false;
		}
		return children[index]->string_exist(key + 1);
	}

};

#elif 0
#include<iostream>
using namespace std;

int mstrlen(const char* a) {
	int ret = 0;
	while (a[ret] != '\0')
		ret++;
	return ret;
}
void mstrcpy(char* d, const char* s) {
	int k = 0;
	while (s[k] != '\0') {
		d[k] = s[k];
		k++;
	}
	d[k] = s[k];
}

int main() {
	char a[100] = "abcdef";
	char b[100] = "ABCD";
	string c = "1234567890";

	cout << sizeof(a) << endl;
	cout << sizeof(b) << endl;
	cout << sizeof(c) << endl;

	cout << a << endl;
	cout << b << endl;
	cout << c << endl;

	mstrcpy(a, b);
	mstrcpy(b, c);
	cout << a << endl;
	cout << b << endl;


	cout << mstrlen("") << endl;
	cout << mstrlen("abd") << endl;
	return 1;
}
#elif 0
#define MAX_CHARACTER (400005)
#define MAX_WORD (80005)
#define newCharacter() (&cPool[cCnt++])
#define newWord() (&wPool[wCnt++])
struct character {
	char w;
	character* prev;
	character* next;
}st, ed, cPool[MAX_CHARACTER];
character* cursorC;
int cCnt;

struct word {
	character* first;
	word* prev;
	word* next;
}stW, edW, wPool[MAX_WORD];
word* cursorW;
int wCnt;

void insertWord(character* first) {
	word* n = newWord();
	*n = { 0,0,0 };
	n->first = first;
	n->prev = cursorW->prev;
	cursorW->prev->next = n;
	n->next = cursorW;
	cursorW->prev = n;
}

void insertCharacter(char w) {
	character* n = newCharacter();
	*n = { 0,0,0 };
	n->w = w;
	n->prev = cursorC->prev;
	cursorC->prev->next = n;
	n->next = cursorC;
	cursorC->prev = n;
}

void insertString(char mStr) {

	int cond1 = ((cursorC->prev->w == '_') << 1) | (cursorC->w == '_');
	int cond2 = ((cursorC->prev->w != '_' && cursorC->prev->w != '$') << 1) | (cursorC->w != '_' && cursorC->w != '$');
	insertCharacter(mStr);
	if (mStr == '_') {
		switch (cond1)
		{
		case 0:
			if (cursorC->prev != st.next && cursorC != &ed)
				insertWord(cursorW->first);
			insertWord(cursorC->prev);
			if (cursorC->prev != st.next && cursorC != &ed)
				cursorW->first = cursorC;
			break;
		case 1:
			cursorW->first = cursorC->prev;
			break;
		default:
			break;
		}
	}
	else {
		switch (cond2)
		{
		case 0:
			if (cursorC->prev != st.next && cursorC != &ed)
				insertWord(cursorW->first);
			insertWord(cursorC->prev);
			if (cursorC->prev != st.next && cursorC != &ed)
				cursorW->first = cursorC;
			break;
		case 1:
			cursorW->first = cursorC->prev;
			break;
		default:
			break;
		}
	}

}

void init()
{
	cCnt = wCnt = 0;

	st = { '$',0,&ed };
	ed = { '$',&st,0 };
	cursorC = &ed;

	stW = { &st,0,&edW };
	edW = { &ed,&stW,0 };
	cursorW = &edW;
}

char putString(char mStr[])
{
	register char* str = mStr;
	register int param = 0;
	register word* lastW = 0, *firstW = 0;
	bool chk = true;
	while (chk) {
		switch (str[0]) {
		case 'R':
			param = str[1] - '0';
			str += 2;
			while (cursorC != &ed && param) { //end of line
				if (cursorC->next == cursorW->next->first)
					cursorW = cursorW->next;
				cursorC = cursorC->next;
				param--;
			}

			break;
		case 'L':
			param = str[1] - '0';
			str += 2;
			while (cursorC->prev != &st && param) { // st->next is first
				if (cursorC == cursorW->first)
					cursorW = cursorW->prev;
				cursorC = cursorC->prev;
				param--;
			}
			break;
		case 'F':
			param = str[1] - '0';
			str += 2;
			lastW = edW.prev->first->w == '_' ? edW.prev : &edW; //last is _ or word 
			if (cursorW != &edW && cursorW->next != lastW && cursorW->next != &edW) {
				while (cursorW->next != lastW && param) {
					cursorW = cursorW->next;
					if (cursorW->first->w != '_')
						--param;
				}
				cursorC = cursorW->next->first->prev;
			}
			break;
		case 'B':
			param = str[1] - '0';
			str += 2;
			firstW = stW.next->first->w == '_' ? stW.next : &stW; //last is _ or word
			if (cursorW->prev != firstW) {
				while (cursorW->prev != firstW && param) {
					cursorW = cursorW->prev;
					if (cursorW->first->w != '_')
						--param;
				}
				cursorC = cursorW->first;
			}
			break;
		case 'D':
			param = str[1] - '0';
			str += 2;
			while (cursorC != &ed && param) {

				if (cursorC->next == cursorW->next->first) { //move to next word
					if (cursorW->first->next == cursorW->next->first) { //erase cur word
						cursorW->prev->next = cursorW->next;
						cursorW->next->prev = cursorW->prev;
						cursorW = cursorW->next;

						// check same kind word attach
						if (cursorW->prev != &stW && cursorW != &edW) {
							cursorW->prev->next = cursorW->next;
							cursorW->next->prev = cursorW->prev;
							cursorW = cursorW->prev;
						}

					}
					else
						cursorW = cursorW->next;
				}
				else if (cursorC == cursorW->first) // if first change
					cursorW->first = cursorC->next;
				cursorC->prev->next = cursorC->next;
				cursorC->next->prev = cursorC->prev;
				cursorC = cursorC->next;
				param--;
			}
			break;
		default:
			if (*str)
				insertString(*str++);
			else
				chk = false;
			break;
		}
	}
	return cursorC->w;
}

char getWord(int mX)
{

	register word* wst = &stW;
	while (wst != &edW && mX) {
		wst = wst->next;
		if (wst->first->w != '_')
			--mX;
	}
	return mX ? '$' : wst->first->w;
}
#elif 0

#include<list>
using namespace std;

struct Node {
	char ch;
	Node* prev, *next;
}node[200005];


#elif 0

struct Node {
	int id;
	Node* prev, *next;
}node[1000];

struct List {
	Node* pHead = new Node;
	Node* pTail = new Node;
	void initialize() {
		link(pHead, pTail);
	}
	bool isEmpty() {
		return (pHead->next == pTail);
	}
	static void link(Node* f, Node* b) {
		f->next = b;
		b->prev = f;
	}
	static void erase(Node* node) {
		link(node->prev, node->next);
	}
	void insert_back(Node* node) {
		link(pTail->prev, node);
		link(node->next, pTail);
	}
	void splice(List* list) {
		if (list->isEmpty())	return;
		link(pTail->prev, list->pHead->next);
		link(list->pTail->prev, pTail);
		list->initialize();
	}
}list[6][6];

#elif 0

struct Node {
	int id, team;
	Node* prev, *next;
}node[100000];

struct List {
	Node* pHead = new Node;
	Node* pTail = new Node;

	void initialize() {
		link(pHead, pTail);
	}
	bool isEmpty() {
		return (pHead->next == pTail);
	}
	static void link(Node* f, Node* b) {
		f->next = b;
		b->prev = f;
	}
	static void erase(Node* node) {
		link(node->prev, node->next);
	}
	void insert_back(Node* node) {
		link(pTail->prev, node);
		link(node, pTail);
	}
	void splice(List* list) {
		if (list->isEmpty()) return;
		link(pTail->prev, list->pHead->next);
		link(list->pTail->prev, pTail);
		list->initialize();
	}
}list[6][6];
#elif 0
#include <bits/stdc++.h>
using namespace std;
//priority_queue<int>	pq;
vector<int>		v;

int BS_loop(int s, int e, int key) {

	while (s <= e)
	{
		int m = (s + e) / 2;
		if (v[m] == key)
			return m;
		else if (v[m] < key)
			s = m + 1;
		else
			e = m - 1;
	}
	return -1;
}

int BS_rec(int s, int e, int key) {

	if (s > e)
		return -1;
	int m = (s + e) / 2;

	if (v[m] == key)
		return m;
	else if (v[m] < key)
		BS_rec(m + 1, e, key);
	else
		BS_rec(s, m - 1, key);

	//return - 1;
}

int main() {

	//for (int i = 0; i < 100; i++) 
	{
		v.push_back(123);
		v.push_back(323);
		v.push_back(14);
		v.push_back(34);
		v.push_back(999);
		v.push_back(0);
		v.push_back(11);
		v.push_back(666);
	}

	for (auto it : v) {
		cout << it << endl;
	}
	sort(v.begin(), v.end());
	for (auto it : v) {
		cout << it << endl;
	}

	//cout << BS_loop(0, v.size() - 1, 323) << endl;
	cout << BS_rec(0, v.size() - 1, 323) << endl;
	return 1;
}


#elif 0
#include <unordered_map>
using namespace std;
unordered_map<int, int> M;
struct Q {
	int cnt;
	int p, c;
	int cid[3];
}q[18000];
int id, N;

void init(int N, int mId[], int mNum[]) {
	id = 0;
	::N = N;
	M.clear();
	for (int i = 0; i < N; i++) {
		q[++id] = { mNum[i] };
		M[mId[i]] = id;
	}
}

int add(int mId, int mNum, int mParent) {
	int pid = M[mParent];
	if (q[pid].c >= 3) return -1;
	q[++id] = { mNum, pid, };
	M[mId] = id;
	q[pid].c++;
	for (int i = 0; i < 3; i++) {
		if (!q[pid].cid[i]) {
			q[pid].cid[i] = id;
			break;
		}
	}
	int p = id;
	while (p) q[p = q[p].p].cnt += mNum;
	return q[pid].cnt;
}

int remove(int mId) {
	int rid = M[mId];
	if (!rid) return -1;
	int p = q[rid].p;
	q[p].c--;
	for (int i = 0; i < 3; i++) {
		if (q[p].cid[i] == rid) {
			q[p].cid[i] = 0;
			break;
		}
	}
	while (p > 0) {
		q[p].cnt -= q[rid].cnt;
		p = q[p].p;
	}
	q[rid].p = -1;
	M[mId] = 0;
	return p == -1 ? -1 : q[rid].cnt;
}

int distribute(int K) {
	int s = 1, e = 0, i;

	for (i = 1; i <= N; i++)
		e = e > q[i].cnt ? e : q[i].cnt;

	while (s <= e) {
		int m = (s + e) >> 1;
		int res = 0;

		for (i = 1; i <= N; i++)
			res += (q[i].cnt < m) ? q[i].cnt : m;

		if (res <= K)
			s = m + 1;

		else e = m - 1;
	}
	return s - 1;
}

#elif 0
#include<bits/stdc++.h>
using namespace std;

#define NODE_MAX	8000

struct Node {
	int ID;
	int NUM;
	int P, L, R;
} node[NODE_MAX + 5];

unordered_map<int, int>		H;

int id;

void init(int mID, int mNum) {
	id = 0;
	H.insert({ mID, id });
	node[0] = { mID, mNum, 0, 0, 0 };
}

int add(int mID, int mNum, int mParent) {

	id++;
	H.insert({ mID, id });
	int P = H[mParent];

	if (!node[P].L)
		node[P].L = id;
	else if (!node[P].R)
		node[P].R = id;
	else
		return -1;

	node[id] = { mID, mNum, P, 0, 0 };
}

int count(int P) {

}

int remove(int mID) {

	if (H.find(mID) == H.end())	return -1;

	count(node[H[mID]].P);

	H.erase(mID);
}


#elif 0
#include<bits/stdc++.h>
using namespace std;

int main() {

	unordered_map<string, int> um;

	if (um.empty()) {
		cout << "map이 비어 있습니다" << endl;
	}

	um.insert(make_pair("key", 1));
	um["banana"] = 2;
	um.insert({ "melon", 3 });

	cout << "map의 크기는 " << um.size() << endl;

	for (pair<string, int> elem : um) {
		cout << elem.first << " " << elem.second << endl;
	}

	if (um.find("banana") != um.end()) {
		um.erase("banana");
	}

	for (auto elem : um) {
		cout << elem.first << " " << elem.second << endl;
	}
	return 1;
}
#elif 0
// BFS - Queue 
#include<bits/stdc++.h>
using namespace std;
#define rint register int

bool v[9];
vector<int> graph[9]{
	{},
	{ 2,3,8 },
	{ 1, 7 },
	{ 1, 4, 5},
	{ 3, 5},
	{ 3, 4},
	{ 7},
	{ 2,6,8},
	{ 1,7}
};

void bfs(int s) {
	queue<int> q;
	q.push(s);
	v[s] = true;
	while (!q.empty()) {
		int curr = q.front();
		q.pop();
		cout << curr << ' ';
		for (rint i = 0; i < graph[curr].size(); i++) {
			int next = graph[curr][i];
			if (!v[next]) {
				q.push(next);
				v[next] = true;
			}
		}
	}
}

int main() {
	bfs(1);
	return 1;
}
#elif 0

#include<bits/stdc++.h>
using namespace std;
#define rint register int
bool v[9];
vector<int> graph[9] = {
	{},
	{ 2,3,8 },
	{ 1, 7 },
	{ 1, 4, 5},
	{ 3, 5},
	{ 3, 4},
	{ 7},
	{ 2,6,8},
	{ 1,7}
};

void dfs(int s) {
	v[s] = true;
	cout << s << ' ';
	for (rint i = 0; i < graph[s].size(); i++) {
		int next = graph[s][i];
		if (!v[next])
			dfs(next);
	}
}

int main() {
	dfs(1);
	return 1;
}

/*
1 2 7 6 8 3 4 5
*/

#elif 0 
#include<bits/stdc++.h>
using namespace std;

stack<int> s({ 5,2,3,7,1,4 });
queue<int> q({ 5,2,3,7,1,4 });

int main() {

	s.push(5);
	s.push(100);
	s.push(200);
	s.push(300);
	s.push(400);
	s.pop();
	s.push(500);
	while (!s.empty()) {
		printf("%d ", s.top());
		s.pop();
	}

	cout << endl;

	q.push(5);
	q.push(100);
	q.push(200);
	q.push(300);
	q.push(400);
	q.pop();
	q.push(500);
	while (!q.empty()) {
		printf("%d ", q.front());
		q.pop();
	}

	return 1;
}

#elif 0



#include<bits/stdc++.h>
using namespace std;
#define N	12
int a[N]{ 1,9,3,8,4,5,5,9,10,3,4,5 };
int tree[N * 4];



int init(int s, int e, int node) {
	if (s == e)
		return tree[node] = a[s];

	int m = (s + e) / 2;
	return tree[node] = init(s, m, node * 2) + init(m + 1, e, node * 2 + 1);
}


int sum(int s, int e, int node, int l, int r) {
	// 범위 밖
	if (r < s || e < l)		return 0;
	// 범위 내
	else if (l <= s && e <= r)	return tree[node];
	int m = (s + e) / 2;
	return sum(s, m, node * 2, l, r) + sum(m + 1, e, node * 2, l, r);
}

void update(int s, int e, int node, int idx, int diff) {
	// 범위밖
	if (idx < s || e < idx) {
		return;
	}
	// 범위내
	else {
		tree[node] += diff;
		if (s == e) return;
		int m = (s + e) / 2;
		update(s, m, node * 2, idx, diff);
		update(m + 1, e, node * 2, idx, diff);
	}
}

int main() {
	init(0, N - 1, 1);
	cout << "0~12 더하기" << sum(0, N - 1, 1, 0, 12) << endl;
	update(0, N - 1, 1, 5, -5);
	cout << "0~12 더하기" << sum(0, N - 1, 1, 0, 12) << endl;
	return 1;
}

#elif 0
#include <iostream>
#include <vector>
using namespace std;


vector<int>	type[1005];

void init(int N, int M, int mType[], int mTime[]) {


	for (int i = 0; i < N; i++) {
		type[i].clear();
		type[i] = { mType[i] };
	}


}

#elif 0
#include <queue>
using namespace std;

#define NN	4000

vector<pair<int, int>>	map[NN];

priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>>	pq[NN][NN];


void init(int N, int L, int sCity[], int eCity[], int mCost[], int mTime[]) {

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			pq[i][j] = {};
		}
	}

	for (int i = 0; i < L; i++) {
		pq[sCity[i]][eCity[i]].push({ mTime[i], mCost[i] });
	}
}

void add(int sCity, int eCity, int mCost, int mTime) {

	pq[sCity][eCity].push({ mTime, mCost });

}


int cost(int sCity, int eCity, int mCost) {
	int ret = 0;

	pair<int, int> a = pq[sCity][eCity].top();
	int time = a.first;
	int cost = a.second;

	while () {


	}

	return ret;
}

#elif 0
// 코드 참고 : https://github.com/ndb796/python-for-coding-test

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

bool visited[9];
vector<int> graph[9];

// BFS 함수 정의
void bfs(int start) {
	queue<int> q;
	q.push(start); // 첫 노드를 queue에 삽입
	visited[start] = true; // 첫 노드를 방문 처리

	// 큐가 빌 때까지 반복
	while (!q.empty()) {
		// 큐에서 하나의 원소를 뽑아 출력
		int x = q.front();
		q.pop();
		cout << x << ' ';
		// 해당 원소와 연결된, 아직 방문하지 않은 원소들을 큐에 삽입
		for (int i = 0; i < graph[x].size(); i++) {
			int y = graph[x][i];
			if (!visited[y]) {
				q.push(y);
				visited[y] = true;
			}
		}
	}
}

int main(void) {
	// 노드 1에 연결된 노드 정보 저장 
	graph[1].push_back(2);
	graph[1].push_back(3);
	graph[1].push_back(8);

	// 노드 2에 연결된 노드 정보 저장 
	graph[2].push_back(1);
	graph[2].push_back(7);

	// 노드 3에 연결된 노드 정보 저장 
	graph[3].push_back(1);
	graph[3].push_back(4);
	graph[3].push_back(5);

	// 노드 4에 연결된 노드 정보 저장 
	graph[4].push_back(3);
	graph[4].push_back(5);

	// 노드 5에 연결된 노드 정보 저장 
	graph[5].push_back(3);
	graph[5].push_back(4);

	// 노드 6에 연결된 노드 정보 저장 
	graph[6].push_back(7);

	// 노드 7에 연결된 노드 정보 저장 
	graph[7].push_back(2);
	graph[7].push_back(6);
	graph[7].push_back(8);

	// 노드 8에 연결된 노드 정보 저장 
	graph[8].push_back(1);
	graph[8].push_back(7);

	bfs(1);
}

#elif 0

#include<iostream>
#include<vector>
#include<queue>
using namespace std;
#define N	(6) 

vector<pair<int, int>> map[7];
int d[7];

void dijkstra(int start) {
	//priority_queue<pair<int, int>, vector<pair<int,int>>, less<pair<int,int>>> pq ; 


	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>  > pq;
	//queue<pair<int, int>> pq;
	//priority_queue<pair<int, int>> pq;  

	d[start] = 0;
	pq.push(make_pair(start, 0));

	while (!pq.empty())
	{
		int curr = pq.top().first;
		int distance_curr = pq.top().second;
		pq.pop();

		if (d[curr] < distance_curr) continue;

		// curr 부터 연결되어 있는 간선을 순회하면서 각 거리의 최단거리를 업데이트 한다. 
		for (int i = 0; i < map[curr].size(); i++) {
			int next = map[curr][i].first;
			int distance_next = distance_curr + map[curr][i].second;

			if (d[next] > distance_next) {
				d[next] = distance_next;
				pq.push({ next, distance_next });
			}
		}
	}
}

int main() {


	for (int i = 1; i <= N; i++) {
		d[i] = 1e9;
	}

	map[1].push_back({ 2, 2 });
	map[1].push_back({ 3, 5 });
	map[1].push_back(make_pair(4, 1));

	map[2].push_back(make_pair(1, 2));
	map[2].push_back(make_pair(3, 3));
	map[2].push_back(make_pair(4, 2));

	map[3].push_back(make_pair(1, 5));
	map[3].push_back(make_pair(2, 3));
	map[3].push_back(make_pair(4, 3));
	map[3].push_back(make_pair(5, 1));
	map[3].push_back(make_pair(6, 5));

	map[4].push_back(make_pair(1, 1));
	map[4].push_back(make_pair(2, 2));
	map[4].push_back(make_pair(3, 3));
	map[4].push_back(make_pair(5, 1));

	map[5].push_back(make_pair(3, 1));
	map[5].push_back(make_pair(4, 1));
	map[5].push_back(make_pair(6, 2));

	map[6].push_back(make_pair(3, 5));
	map[6].push_back(make_pair(5, 2));

	dijkstra(1);




	for (int i = 1; i <= N; i++) {
		printf("%d ", d[i]);
	}
	return 1;
}


#elif 0
#include <unordered_map>
#include <queue>
#include <vector> 
#include <algorithm>
#include <iostream>
using namespace std;

int emptySize;
priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq; //start, size
unordered_map<int, int> Hash;
int fn;
vector<pair<int, int>> file[12005]; //start, size

void init(int N) {
	emptySize = N;
	pq = {};
	// 시작되는 부분부터 먼저 채워 넣기 위해 
	// 시작 시점을 기준으로 PQ로 넣는다.  (PQ가 시작 시점이 작은 순으로 큐되도록)
	pq.push({ 1, N });
	for (int i = 0; i < fn; i++) file[i].clear();
	fn = 0;
	Hash.clear();
}

int add(int mId, int mSize) {
	if (emptySize < mSize) return -1;

	emptySize -= mSize;
	int fid = Hash[mId] = fn++;

	while (mSize) {
		pair<int, int> a = pq.top();
		// Framgmented 된 빈공간중에 시작 시점이 가장 빠른 조각을 가져온다.  
		pq.pop();
		int start = a.first;
		int space = a.second;

		// 마지막으로 공간을 채우고 나면 			
		// Fragmented 된 공간을 재 계산하여 push()해 준다. 
		if (space > mSize) {
			pq.push({ start + mSize, a.second - mSize });
			space = mSize;
		}

#if 1
		// 추가적으로 저장되고 이어서 저장될 때
		if (file[fid].size() && file[fid].back().first + file[fid].back().second == start) {
			file[fid].back().second += space;
		}
		// 처음 또는 조각되어 저장될 때 
		else
			file[fid].push_back({ start, space });
#endif

		// 채울수 있는 공간만큼만 채우고(빼주고) 아직 못채운 크기만을 남겨준다. 
		mSize -= space;
	}
	return file[fid][0].first;
}

int remove(int mId) {
	int fid = Hash[mId];
	int ret = file[fid].size();
	for (auto v : file[fid]) {
		pq.push(v);
		emptySize += v.second;
	}
	file[fid].clear();
	return ret;
}

int count(int mStart, int mEnd) {
	int ret = 0;
	for (int i = 0; i < fn; i++) {
		for (auto v : file[i]) {
			int s = v.first;
			int e = v.first + v.second - 1;
			if (mStart <= e && s <= mEnd) {
				ret++; break;
			}
		}
	}
	return ret;
}
#elif 0
#include<set>
#include<vector>
#include<unordered_map>
using namespace std;

int mp[5005];
vector<pair<int, int>> g[10005];
int n, dis[5005];

void init(int N, int mBrands[])
{
	n = N;
	for (int i = 0; i < N; i++)
	{
		g[i].clear();
		mp[i] = mBrands[i];
	}
}

void connect(int mHotelA, int mHotelB, int mDistance)
{
	g[mHotelA].push_back({ mHotelB, mDistance });
	g[mHotelB].push_back({ mHotelA, mDistance });
}

int merge(int mHotelA, int mHotelB)
{
	int s = 0;
	int a = mp[mHotelA];
	int b = mp[mHotelB];
	for (int i = 0; i < n; i++)
	{
		if (mp[i] == a || mp[i] == b)
		{
			mp[i] = a;
			s++;
		}
	}
	return s;
}

int move(int mStart, int mBrandA, int mBrandB)
{
	set<pair<int, int>> sp;
	for (int i = 0; i <= n; i++)
		dis[i] = 1000000;
	dis[mStart] = 0;
	int a = 0, b = 0;
	sp.insert({ 0,mStart });

	while (!sp.empty())
	{
		auto it = *sp.begin();
		int node = it.second;
		int dist = it.first;
		sp.erase(it);

		if (dist > dis[node]) continue;

		if (node != mStart) {
			if (mBrandA == mBrandB) {
				if (mp[node] == mBrandA) {
					if (a)
						b = dist;
					else
						a = dist;
				}
			}
			else {
				if (mp[node] == mBrandA)
					if (!a)
						a = dist;
				if (mp[node] == mBrandB)
					if (!b)
						b = dist;
			}
		}

		if (a && b) return a + b;

		for (auto it : g[node])
		{
			if ((dis[node] + it.second) < dis[it.first])
			{
				dis[it.first] = dist + it.second;
				sp.insert({ dis[it.first], it.first });
			}
		}

	}
}
#elif 0
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

int main() {

	unordered_map<string, int> um;

	if (um.empty()) {
		cout << "unordered_map은 비어있습니다" << endl;
	}

	um.insert(make_pair("key", 1));
	um["banana"] = 2;
	um.insert({ "melon",3 });

	cout << "unordered_map의 크기는 " << um.size() << " 입니다" << endl;

	// auto로 해도 무방
	for (pair<string, int> elem : um) {
		//for (auto elem : um) {
		cout << "key : " << elem.first << " value : " << elem.second << endl;
	}

	// find 대신 count로 확인 가능
	if (um.find("banana") != um.end()) {
		um.erase("banana");
	}

	cout << "unordered_map의 크기는 " << um.size() << " 입니다" << endl;
	for (auto elem : um) {
		cout << "key : " << elem.first << " value : " << elem.second << endl;
	}

	return 1;
}


#elif  0
#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

vector<pair<int, int>> q;

bool compare(pair<int, int> a, pair<int, int> b) {
	return a.first > b.first;
}

int main() {

	q.push_back(make_pair(4, 6));
	q.push_back(make_pair(1, 3));
	q.push_back(make_pair(2, 4));
	q.push_back(make_pair(3, 5));


	for (int i = 0; i < q.size(); i++) {
		cout << q[i].first << "\t" << q[i].second << endl;
	}
	cout << endl;

	sort(q.begin(), q.end(), less<pair<int, int>>());

	for (int i = 0; i < q.size(); i++) {
		cout << q[i].first << "\t" << q[i].second << endl;
	}
	return 1;
	cout << endl;
}


#elif 0
#include <iostream>
#include <vector>
using namespace std;

int main() {
	//vector<vector<int>> v(10, vector<int>(5,7));
	vector<pair<int, int>> v[10];

	v[0].push_back(make_pair(0, 2));
	v[0].push_back(make_pair(1, 2));
	v[0].push_back(make_pair(2, 2));
	v[0].push_back(make_pair(3, 2));
	v[0].push_back(make_pair(4, 2));
	v[0].push_back(make_pair(5, 2));


	cout << endl;
	/*for (int i = 0; i < v.size(); i++) {
		for (int j = 0; j < v[i].size(); j++) {
			cout << v[i][j] << " ";
		}
		cout << endl;
	}*/


	return EXIT_SUCCESS;
}

#elif 0
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define V 6 // 정점의 수

// 최단 경로를 찾을 그래프의 인접 행렬 표현
int graph[V][V] = {
	{0, 2, 0, 0, 0, 3},
	{2, 0, 4, 0, 0, 1},
	{0, 4, 0, 5, 0, 0},
	{0, 0, 5, 0, 0, 7},
	{0, 0, 0, 0, 0, 2},
	{3, 1, 0, 7, 2, 0}
};

// 최소 거리 값을 찾기 위한 보조 함수
int minDistance(int dist[], int sptSet[]) {
	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++) {
		if (sptSet[v] == 0 && dist[v] <= min) {
			min = dist[v];
			min_index = v;
		}
	}

	return min_index;
}

// 다익스트라 알고리즘을 사용하여 최단 경로를 찾는 함수
void dijkstra(int start) {
	int dist[V]; // 최단 거리를 저장하는 배열
	int sptSet[V]; // 최단 경로가 확보된 정점을 나타내는 배열

	// 초기화
	for (int i = 0; i < V; i++) {
		dist[i] = INT_MAX;
		sptSet[i] = 0;
	}

	dist[start] = 0; // 시작점의 거리는 0

	// 모든 정점을 방문
	for (int count = 0; count < V - 1; count++) {
		int u = minDistance(dist, sptSet);
		sptSet[u] = 1;

		// 선택된 정점을 통해 인접한 정점들의 최단 거리 갱신
		for (int v = 0; v < V; v++) {
			if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v]) {
				dist[v] = dist[u] + graph[u][v];
			}
		}
	}

	// 결과 출력
	printf("정점\t\t최단 거리\n");
	for (int i = 0; i < V; i++) {
		printf("%d\t\t%d\n", i, dist[i]);
	}
}

int main() {
	int start_vertex = 0; // 시작 정점

	dijkstra(start_vertex);

	return 0;
}

#elif 0
#include<iostream>
#include<vector>
#include<queue>
using namespace std;
#define N	(6) 

vector<pair<int, int>> map[7];
int d[7];

void dijkstra(int start) {
	//priority_queue<pair<int, int>, vector<pair<int,int>>, less<pair<int,int>>> pq ; 
	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>  > pq;
	//priority_queue<pair<int, int>> pq;  

	d[start] = 0;
	pq.push(make_pair(start, 0));

	while (!pq.empty())
	{
		int curr = pq.top().first;
		int distance_curr = pq.top().second;
		//int distance_curr = -pq.top().second;
		pq.pop();
		cout << "curr = " << curr << endl;
		cout << "distance_curr = " << distance_curr << endl;

		if (d[curr] < distance_curr) continue;
		cout << "d[curr] = " << d[curr] << endl;
		cout << "distance_cur = " << distance_curr << endl;
		cout << "d[curr] < distance_curr  = " << (d[curr] < distance_curr) << endl;

		// curr 부터 연결되어 있는 간선을 순회하면서 각 거리의 최단거리를 업데이트 한다. 
		for (int i = 0; i < map[curr].size(); i++) {
			int next = map[curr][i].first;
			int distance_next = distance_curr + map[curr][i].second;
			cout << "next = " << next << endl;
			cout << "distance_next = " << distance_next << endl;

			if (d[next] > distance_next) {
				d[next] = distance_next;
				cout << "d[" << next << "] = " << distance_next << endl;
				pq.push(make_pair(next, distance_next));
				//pq.push(make_pair(next, -distance_next));
			}
		}
	}
}

int main() {

	for (int i = 1; i <= N; i++) {
		d[i] = 1e9;
	}

	map[1].push_back({ 2, 2 });
	map[1].push_back({ 3, 5 });
	map[1].push_back(make_pair(4, 1));

	map[2].push_back(make_pair(1, 2));
	map[2].push_back(make_pair(3, 3));
	map[2].push_back(make_pair(4, 2));

	map[3].push_back(make_pair(1, 5));
	map[3].push_back(make_pair(2, 3));
	map[3].push_back(make_pair(4, 3));
	map[3].push_back(make_pair(5, 1));
	map[3].push_back(make_pair(6, 5));

	map[4].push_back(make_pair(1, 1));
	map[4].push_back(make_pair(2, 2));
	map[4].push_back(make_pair(3, 3));
	map[4].push_back(make_pair(5, 1));

	map[5].push_back(make_pair(3, 1));
	map[5].push_back(make_pair(4, 1));
	map[5].push_back(make_pair(6, 2));

	map[6].push_back(make_pair(3, 5));
	map[6].push_back(make_pair(5, 2));

	dijkstra(1);




	for (int i = 1; i <= N; i++) {
		printf("%d ", d[i]);
	}
	return 1;
}


#elif 0
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include<iostream>
#include<limits>
#define N 6
using namespace std;

int INF(1e9);
int map[6][6] = {
	{0,2,5,1,INF,INF},
	{2,0,3,2,INF,INF},
	{5,3,0,3,1, 5},
	{1,2,3,0,1,INF},
	{INF, INF, 1,1,0,2},
	{INF, INF, 5, INF, 2, 0}
};

int d[N];
bool v[N];


int getSmallidx() {
	int idx = 0;
	int mv = INF;
	for (int i = 0; i < N; i++) {
		if (!v[i] && mv > d[i]) {
			mv = d[i];
			idx = i;
		}
	}
	return idx;
}
void dijkstra(int s) {
	for (int i = 0; i < N; i++) {
		d[i] = map[s][i];
	}
	v[s] = 1;

	for (int i = 0; i < N - 1; i++) {
		int cur = getSmallidx();
		v[cur] = 1;
		//cout << next << endl;		

		for (int j = 0; j < N; j++) {
			if (!v[j]) {
				if (d[j] > d[cur] + map[cur][j]) {
					d[j] = d[cur] + map[cur][j];
				}
			}
		}
	}
}

int main() {
	dijkstra(0);
	cout << endl;
	for (int i = 0; i < N; i++) {
		cout << d[i] << endl;
	}
	return 1;
}
#elif 0
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

//#include<bits/stdc++.h>
#include<iostream>
#include<vector>
#include<typeinfo>
#include<queue>

using namespace std;  //C++의 표준 라이브러리도 사용자가 직접 만든 식별자와 충돌하지 않게 하기 위해 C++은 표준 라이브러리의 모든 기능을 std namespace안에 넣어두게 되었다.

int n;
//vector<int> arr;
int arr[10000];

priority_queue<int> h;

int main() {
	setbuf(stdout, NULL);
	freopen("input.txt", "r", stdin);

	cin >> n;
	for (int i = 0; i < n; i++) {
		int x;
		cin >> x;
		//arr.push_back(x); 
		//arr[i] = x;
		h.push(x);
	}

	for (auto i : arr) {
		//cout << i << endl;
	}

	/*for (int i = 0; i < n; i++) {
		h.push(-arr[i]);
	}*/

	while (!h.empty()) {
		cout << h.top() << endl;
		h.pop();
	}

	return 1;
}


#elif 0
#include <unordered_map>
#include <vector>
#include <iostream>
#include <algorithm>

#define _abs(x) ( (x)>0 ? (x):-(x) )

using namespace std;


int mK, mL;

struct node {
	int id;
	int x, y, c, d;
	int alive;
} NODE[20001];
int NC;

node* newNode() {
	return &NODE[NC++];
}
vector<node*> mylist[41][41];
unordered_map <int, node* > HASH;
vector< node*> templist;

void init(int K, int L)
{
	mK = K;
	mL = L;
	HASH.clear();
	NC = 0;

	for (int i = 0; i < 41; ++i)
		for (int j = 0; j < 41; ++j)
			mylist[i][j].clear();


}

void addSample(int mID, int mX, int mY, int mC)
{
	node* temp = newNode();
	temp->id = mID;
	temp->x = mX;
	temp->y = mY;
	temp->c = mC;
	temp->d = -1;
	temp->alive = true;

	mylist[mX / 100][mY / 100].push_back(temp);

	HASH[mID] = temp;


}

void deleteSample(int mID)
{
	HASH[mID]->alive = false;

}


bool myfunction(node* a, node* b) {
	if (a->d == b->d) {
		if (a->x == b->x) {
			return a->y < b->y;
		}
		else {
			return a->x < b->x;
		}
	}
	else {
		return a->d < b->d;
	}
}

int predict(int mX, int mY)
{
	templist.clear();

	int gx1 = (mX - mL) / 100;
	int gx2 = (mX + mL) / 100;

	int gy1 = (mY - mL) / 100;
	int gy2 = (mY + mL) / 100;


	for (int i = gx1; i <= gx2; i++) {
		for (int j = gy1; j <= gy2; j++) {
			for (auto* it : mylist[i][j]) {
				if (it->alive == false) continue;
				it->d = (_abs(it->x - mX) + _abs(it->y - mY));

				if (mL >= it->d) {

					templist.push_back(it);
				}
			}
		}
	}

	if (templist.size() < mK) { return -1; }

	partial_sort(templist.begin(), templist.begin() + mK, templist.end(), myfunction);

	int count[12] = { 0, 0,0,0,0,0,0,0,0,0,0,0 };
	int maxC = 0;

	int mycount = 0;
	for (auto it : templist) {
		if (mycount++ >= mK) break;

		count[it->c]++;
		if (maxC < count[it->c]) {
			maxC = count[it->c];
		}
	}

	int targetC = 0;
	for (int i = 0; i < 12; i++) {
		if (count[i] == maxC) {
			return i;
		}
	}

}
#elif 0
#include <iostream>
#define M_MAX   40
using namespace std;

int map[M_MAX][M_MAX];

void init() {
	for (int i = 0; i < M_MAX; i++) {
		for (int j = 0; j < M_MAX; j++) {
			map[i][j] = j;
		}
	}
}

void print() {
	for (int i = 0; i < M_MAX; i++) {
		for (int j = 0; j < M_MAX; j++) {
			cout << map[i][j] << " ";
		}
		cout << endl;
	}
}


int main() {
	init();
	print();




	return 1;
}



#elif 0

struct Node {
	int id, team;
	Node* prev, *next;
}node[100000];

struct List {
	Node* pHead = new Node;
	Node* pTail = new Node;

	void initialize() {
		link(pHead, pTail);
	}
	bool isEmpty() {
		return (pHead->next == pTail);
	}
	static void link(Node* f, Node* b) {
		f->next = b;
		b->prev = f;
	}
	static void erase(Node* node) {
		link(node->prev, node->next);
	}
	void insert_back(Node* node) {
		link(pTail->prev, node);
		link(node, pTail);
	}
	void splice(List* list) {
		if (list->isEmpty()) return;
		link(pTail->prev, list->pHead->next);
		link(list->pTail->prev, pTail);
		list->initialize();
	}
}list[6][6];


struct Node {
	int id, team;
	Node* prev, *next;
}node[100000];

struct List {
	Node* phead = new Node;
	Node* ptail = new Node;

	static void link(Node* f, Node* b)
	{
		f->next = b;
		b->prev = f;
	}
	static void erase(Node* node)
	{
		link(node->prev, node->next);
	}
	void insert_back(Node* node)
	{
		link(ptail->prev, node);
		link(node, ptail);
	}
	void initialize()
	{
		link(phead, ptail);
	}
	bool isEmpty()
	{
		return phead->next == ptail;
	}
	void splice(List* list)
	{
		if (list->isEmpty()) return;
		link(ptail->prev, list->phead->next);
		link(list->ptail->prev, ptail);
		list->initialize();
	}
}list[6][6];


void init()
{
	for (int i = 1; i < 6; i++) {
		for (int j = 1; j < 6; j++) {
			list[i][j].initialize();
		}
	}
}

void hire(int mId, int mTeam, int mScore)
{
	node[mId] = { mId, mTeam };
	list[mTeam][mScore].insert_back(node[mId]);
}
void fire(int mId)
{
	List::erase(node[mId]);
}

void updateSoldier(int mId, int mScore)
{
	fire(mId);
	hire(mId, node[mId].team, mScore);
}

void updateTeam(int mTeam, int mChangeScore)
{
	if (mChangeScore > 0) {
		for (int i = 4; i > = 1; i--) {
			int newscore = (i + mChangeScore) > 5 ? 5 : (i + mChangeScore);
			list[mTeam][newscore].spice(&list[mTeam][i]);
		}
	}
	else if (mChangeScore < 0) {
		for (int i = )

	}
}

int bestSoldier(int mTeam)
{

}

#elif 0
#include <iostream>

using namespace std;

class Node {
	friend class DLL;
private:
	int data;
	Node* pNext;
	Node* pPrev;
public:
	Node() {
		data = 0;
		this->pPrev = NULL;
		this->pNext = NULL;
	}
	Node(int data) {
		this->data = data;
		this->pPrev = NULL;
		this->pNext = NULL;
	}
	~Node() {}
};

class DLL {
private:
	Node* pHead;
	Node* pTail;
	Node* pCursor;
public:
	DLL() {
		pHead = new Node();
		pTail = new Node();
		pCursor = new Node();
		pHead->pNext = pTail;
		pTail->pPrev = pHead;
	}
	~DLL() {}
	void insertion(int data) {
		Node* temp = new Node(data);
		pCursor = pHead->pNext;
		pHead->pNext = temp;
		temp->pNext = pCursor;
		pCursor->pPrev = temp;
		temp->pPrev = pHead;
	}
	void deletion(int data) {
		if (pHead->pNext == pTail) cout << "No node exists" << endl;
		else {
			pCursor = pHead->pNext;
			while (pCursor != pTail) {
				if (pCursor->data == data) {
					pCursor->pPrev->pNext = pCursor->pNext;
					pCursor->pNext->pPrev = pCursor->pPrev;
					delete pCursor;
					return;
				}
				else {
					pCursor = pCursor->pNext;
				}
			}
		}
	}

	void traversal() {
		if (pHead->pNext == pTail) cout << "No node exists" << endl;
		else {
			pCursor = pHead->pNext;
			while (pCursor != pTail) {
				cout << pCursor->data << " ";
				pCursor = pCursor->pNext;
			}
			cout << endl;
		}
	}

	void reverseTraversal() {
		if (pTail->pPrev == pHead) cout << "No node exists" << endl;
		else {
			pCursor = pTail->pPrev;
			while (pCursor != pHead) {
				cout << pCursor->data << " ";
				pCursor = pCursor->pPrev;
			}
			cout << endl;
		}
	}

	int size() {
		int size = 0;
		if (pHead->pNext == pTail) return size;
		else {
			pCursor = pHead->pNext;
			while (pCursor != pTail) {
				size++;
				pCursor = pCursor->pNext;
			}
			return size;
		}
	}
};




int main() {

	DLL dll;
	dll.insertion(1);
	dll.insertion(2);
	dll.insertion(3);
	dll.traversal();
	dll.reverseTraversal();
	cout << "After deletion" << endl;
	dll.deletion(3);
	dll.traversal();
	dll.reverseTraversal();
	cout << "size: " << dll.size() << endl;

	return 0;
}
#elif 0
#include <stdio.h>

#define MAX_VERTEX 30

int vertex;
int map[MAX_VERTEX][MAX_VERTEX];
int visit[MAX_VERTEX];

void depthFirstSearch(int v)
{
	int i;
	visit[v] = 1;
	for (i = 1; i <= vertex; i++)
	{
		if (map[v][i] == 1 && !visit[i])
		{
			printf("%d ", i);
			depthFirstSearch(i);
		}
	}
}

int main(void)
{
	int T;
	int test_case;
	int i, j;
	int start;
	int v1;
	int v2;

	scanf("%d", &T);

	for (test_case = 1; test_case <= T; test_case++)
	{
		for (i = 0; i < MAX_VERTEX; i++)
		{
			for (j = 0; j < MAX_VERTEX; j++)
			{
				map[i][j] = 0;
			}
			visit[i] = 0;
		}

		scanf("%d %d", &vertex, &start);

		while (true)
		{
			scanf("%d %d", &v1, &v2);
			if (v1 == -1 && v2 == -1)
			{
				break;
			}
			map[v1][v2] = map[v2][v1] = 1;
		}

		printf("#%d ", test_case);
		printf("%d ", start);
		depthFirstSearch(start);
		printf("\n");
	}

	return 0;
}

#include <stdio.h>

#define MAX_VERTEX 30

int map[MAX_VERTEX][MAX_VERTEX];
int visit[MAX_VERTEX];
int vertex;
int edge;
int maxEdge;
int start;
int end;

void depthFirstSearch(int v, int depth)
{
	int i;
	if (v == end)
	{
		if (maxEdge < 0 || depth < maxEdge)
		{
			maxEdge = depth;
		}
		return;
	}

	visit[v] = 1;
	for (i = 1; i <= vertex; i++)
	{
		if (map[v][i] == 1 && !visit[i])
		{
			depthFirstSearch(i, depth + 1);
			visit[i] = 0;
		}
	}
}


int main(void)
{
	int T;
	int test_case;
	int i;
	int v1;
	int v2;

	scanf("%d", &T);

	for (test_case = 1; test_case <= T; test_case++)
	{
		scanf("%d %d %d %d", &vertex, &edge, &start, &end);

		for (i = 0; i < edge; i++)
		{
			scanf("%d %d", &v1, &v2);
			map[v1][v2] = 1;
		}

		maxEdge = -1;
		depthFirstSearch(start, 0);
		printf("#%d %d\n", test_case, maxEdge);
	}
	return 0;
}

#include <stdio.h>

#define MAX_M 100

int T;    // # of test case
int M;    // # of element in array
int N;    // # of numbers to search
int arr[MAX_M];

void binarySearch(int* arr, int low, int high, int target)
{
	int mid;
	if (low > high)
	{
		printf("-1 ");
		return;
	}

	mid = (low + high) / 2;

	if (target < arr[mid])
	{
		binarySearch(arr, low, mid - 1, target);
	}
	else if (arr[mid] < target)
	{
		binarySearch(arr, mid + 1, high, target);
	}
	else
	{
		printf("%d ", mid);
		return;
	}
}

int main(void)
{
	int targetValue;
	scanf("%d", &T);

	for (int test_case = 1; test_case <= T; test_case++)
	{
		printf("#%d ", test_case);
		scanf("%d %d", &M, &N);

		for (int i = 0; i < M; i++)
		{
			scanf("%d", &arr[i]);
		}

		for (int i = 0; i < N; i++)
		{
			scanf("%d", &targetValue);
			binarySearch(arr, 0, M - 1, targetValue);
		}
		printf("\n");
	}
	return 0;
}



#include <stdio.h>

#define MAX_N 50

int MAP[MAX_N + 2][MAX_N + 2];
int queue[MAX_N * MAX_N][3];
int row;
int column;
int head;
int rear;

int isEmpty()
{
	return (head <= rear) ? 1 : 0;
}

int enqueue(int x, int y, int c)
{
	queue[head][0] = x;
	queue[head][1] = y;
	queue[head][2] = c;
	head++;
	return 1;
}

int dequeue(int *x, int *y, int *c)
{
	if (isEmpty())
	{
		return 0;
	}
	*x = queue[rear][0];
	*y = queue[rear][1];
	*c = queue[rear][2];
	rear++;
	return 1;
}

int breadthFirstSearch()
{
	int x;
	int y;
	int c;

	enqueue(1, 1, 0);
	MAP[1][1] = 0;
	while (!isEmpty())
	{
		dequeue(&x, &y, &c);
		if (x == column && y == row)
		{
			return c;
		}
		if (x + 1 <= column && MAP[x + 1][y])
		{
			enqueue(x + 1, y, c + 1);
			MAP[x + 1][y] = 0;
		}
		if (y + 1 <= row && MAP[x][y + 1])
		{
			enqueue(x, y + 1, c + 1);
			MAP[x][y + 1] = 0;
		}
		if (x - 1 > 0 && MAP[x - 1][y])
		{
			enqueue(x - 1, y, c + 1);
			MAP[x - 1][y] = 0;
		}
		if (y - 1 > 0 && MAP[x][y - 1])
		{
			enqueue(x, y - 1, c + 1);
			MAP[x][y - 1] = 0;
		}
	}
	return -1;
}


int main(void)
{
	int test_case;
	int T;

	scanf("%d", &T);

	for (test_case = 1; test_case <= T; test_case++)
	{
		head = 0;
		rear = 0;
		scanf("%d %d", &row, &column);

		for (int i = 1; i <= row; i++)
		{
			for (int j = 1; j <= column; j++)
			{
				scanf("%d", &MAP[j][i]);
			}
		}
		printf("#%d %d\n", test_case, breadthFirstSearch());
	}
	return 0;
}
#endif



#if 0
﻿#if 1
#include<iostream>
using namespace std;
const int Nmax = 150000 * 2 + 30000;
char a[Nmax];
int s, e, r;

void init(char mStr[]) {
	s = e = 150000 - 1, r = 0;
	while (*mStr)
		a[e++] = *mStr++;
}
void appendStr(char mWord[]) {
	if (r)
		while (*mWord) 	a[s--] = *mWord++;
	else
		while (*mWord) 	a[e++] = *mWord++;
}
void reverse() { r = !r; }

void cut(int k) {
	if (r)
		a[s = s + k] = '\0';
	else
		a[e = e - k] = '\0';
}

int cnt_sub(int l, char p[]) {
	int ret = 0;
	for (int i = s + 1; i <= e - 1 - l; i++) {
		bool matched = true;
		for (int j = 0; j < l; j++) {
			if (p[j] != a[i + j]) matched = false;
		}
		if (matched)
			ret++;
	}
	return ret;
}


int substring(char mWord[]) {
	int ret = 0, int l = 0;
	while (*mWord++) l++;

	if (r) {
		if (l == 4)	swap(mWord[3], mWord[0]), swap(mWord[2], mWord[1]);
		if (l == 3)	swap(mWord[2], mWord[0]);
		if (l == 2)	swap(mWord[1], mWord[0]);
	}

	return cnt_sub(l, mWord);
}

void print_a() {
	for (int i = s + 1; i <= e - 1; i++) {
		cout << a[i] << " ";
	}
	cout << endl;
}
int main() {
	char tem[1000] = "0123456789";
	char tem1[1000] = "abcd";
	char tem2[1000] = "efgh";

	init(tem);
	print_a();
	cut(3);
	print_a();
	reverse();
	cut(3);
	print_a();
	reverse();
	appendStr(tem1);
	print_a();
	appendStr(tem2);
	print_a();
	reverse();
	appendStr(tem1);
	print_a();
	appendStr(tem2);
	print_a();
	cut(4);
	print_a();
	cut(4);
	print_a();
	reverse();
	cut(4);
	print_a();
	cut(4);
	print_a();


}
#elif 0
// KMP
#include<iostream>
using namespace std;

int f[100000];


void getpi(char* p, int M) {
	f[0] = 0;
	for (int j = 0, i = 1; i < M; i++) {
		while (j > 0 && p[j] != p[i])
			j = f[j - 1];
		if (p[j] == p[i])	f[i] = ++j;
		else				f[i] = 0;
	}
	//for (int i = 0; i < strlen(p); i++)
	//	cout << "[" << i << "]" << f[i] << endl;
}
int kmp(int N, char s[], int M, char p[]) {
	int ret = 0;
	getpi(p, M);
	for (int j = 0, i = 0; i < N; i++) {
		while (p[j] != s[i] && j > 0)		j = f[j - 1];
		if (p[j] == s[i]) {
			if (j == M - 1) {
				j = f[j];
				ret++;
			}
			else
				j++;
		}
	}
	return ret;
}


int FindString(int N, char* A, int M, char* B) {
	return kmp(N, A, M, B);
}


#elif 0
#include<iostream>
using namespace std;

int f[100000];
int l = 0;


void getpi(char* p, int M) {
	f[0] = 0;
	for (int j = 0, i = 1; i < M; i++) {
		while (j > 0 && p[j] != p[i])
			j = f[j - 1];
		if (p[j] == p[i])	f[i] = ++j;
		else				f[i] = 0;
	}


	while (i < M)


		for (int i = 0; i < strlen(p); i++)
			cout << "[" << i << "]" << f[i] << endl;
}

int kmp(int N, char s[], int M, char p[]) {
	int ret = 0;
	getpi(p, M);
	for (int j = 0, i = 0; i < N; i++) {
		while (j > 0 && p[j] != s[i])
			j = f[j - 1];
		if (p[j] == s[i]) {
			if (j == M - 1) {
				ret++;
				j = f[j];
			}
			else
				j++;
		}
		return ret;
	}

	int FindString(int N, char* A, int M, char* B) {
		return kmp(N, A, M, B);
	}

	int main() {
		char A[1000] = "banaba";
		char B[1000] = "ababababc";
		FindString(strlen(A), A, strlen(B), B);
	}

#elif 0

#include<iostream>
using namespace std;

int f[100000];
int l = 0;

void getpi(char* p, int M) {
	f[0] = 0;
	for (int j = 0, i = 1; i < M; i++) {
		//if (p[j] != p[i]) j = 0, f[i] = 0;//f[j - 1];
		while (p[j] != p[i] && j > 0)		j = f[j - 1];
		if (p[j] == p[i]) 	f[i] = ++j;
		else				f[i] = 0;
	}
	/*for (int i = 0; i < strlen(p); i++)
		cout << i << "=" << f[i] << endl;*/
}

int kmp(int N, char s[], int M, char p[]) {
	int ret = 0;
	getpi(p, M);
	for (int j = 0, i = 0; i < N; i++) {
		while (p[j] != s[i] && j > 0)		j = f[j - 1];
		if (p[j] == s[i]) {
			if (j == M - 1) {
				j = f[j];
				//cout << "ans =" << i - strlen(p) + 1 << endl;
				ret++;
			}
			else
				j++;
		}
	}
	return ret;
}

int FindString(int N, char* A, int M, char* B) {

	return kmp(N, A, M, B);

}

#elif 0
#include<iostream>
using namespace std;

int f[1000];
int l = 0;

void getpi(char *p) {
	f[0] = 0;
	for (int j = 0, i = 1; i < strlen(p); i++) {
		while (p[j] != p[i] && j > 0)		j = f[j - 1];
		if (p[j] == p[i])	f[i] = ++j;
		else				f[i] = 0;
	}

	for (int i = 0; i < strlen(p); i++)
		cout << i << "=" << f[i] << endl;
}
void kmp(char s[], char p[]) {
	getpi(p);
	//return;
	for (int j = 0, i = 0; i < strlen(s); i++) {
		while (p[j] != s[i] && j > 0)		j = f[j - 1];
		if (p[j] == s[i]) {
			if (j == strlen(p) - 1) {
				j = f[j];
				cout << "ans =" << i - strlen(p) + 1 << endl;
			}
			else
				j++;
		}
	}
}

int FindString(int N, char* A, int M, char* B) {
	int answer = 0;



	return answer;
}

#elif 0

#include<stdio.h>
#define Nmax 20000

int Q[Nmax], f, r, N;

void init() {
	N = 10, f = r = -1;
}
void push_q(int a) {
	Q[++r] = a;
}
int pop_q() {
	return Q[++f];
}
int main() {
	init();
	for (int i = 0; i < N; i++) {
		int tmp;
		scanf(" %d", &tmp);
		push_q(tmp);
	}

	for (int i = 0; i < N; i++) {
		printf("%d\n", pop_q());
	}

	return 0;
}
#elif 0

#include<stdio.h>

char hands[9][99] = {
	"Top", "1 Pair", "2 Pair", "Triple", "Straight",
	"Flush", "Full House","4 Card", "Straight Flush" };

char S[7]; int N[7], P[5];
void max(int& a, int b) {
	if (a < b) a = b;
}

int recognize() {
	int res = 0;

	bool flush = 1;
	for (int i = 1; i < 5; i++)
		if (S[P[i - 1]] != S[P[i]])
			flush = 0;
	if (flush) max(res, 5);

	int c[15] = { 0, };
	for (int i = 0; i < 5; i++)
		c[N[P[i]]]++;

	int pair = 0, triple = 0, four = 0;
	for (int i = 1; i <= 13; i++) {
		if (c[i] == 2) pair++;
		if (c[i] == 3) triple++;
		if (c[i] == 4) four++;
	}
	if (pair == 1) max(res, 1);
	if (pair == 2) max(res, 2);
	if (triple) max(res, 3);
	if (pair && triple) max(res, 6);
	if (four) max(res, 7);

	bool straight = 0;
	c[14] = c[1];
	for (int i = 5; i <= 14; i++) {
		if (c[i - 4] && c[i - 3] && c[i - 2] && c[i - 1] && c[i]) {
			straight = 1;
			break;
		}
	}
	if (straight) max(res, 4);
	if (straight && flush) max(res, 8);
	return res;
}

int pick(int a, int c) {
	int res = 0;
	if (c == 5) {
		res = recognize();
	}
	else if (a < 7) {
		P[c] = a;
		int x = pick(a + 1, c + 1);
		int y = pick(a + 1, c);
		max(res, x);
		max(res, y);
	}
	return res;
}

int T;
int main() {
	int tc; scanf("%d", &T);
	for (int tc = 1; tc <= T; tc++) {
		for (int i = 0; i < 7; i++)
			scanf(" %c %d", &S[i], &N[i]);
		printf("#%d %s\n", tc, hands[pick(0, 0)]);
	}
	return 0;
}
#elif 0

// [1강 4번] 최대 부분 배열
#include<bits/stdc++.h>
using namespace std;
#define Nmax 200000

int a[Nmax], D[Nmax];


int main()
{
	int test_case, T;
	cin >> T;

	for (test_case = 1; test_case <= T; ++test_case)
	{
		int ans = 0;
		int N;
		cin >> N;
		for (int i = 0; i < N; i++)
		{
			cin >> a[i];
			if (i == 0) {
				ans = D[i] = a[i];
				continue;
			}

			D[i] = max(a[i], D[i - 1] + a[i]);
			ans = max(ans, D[i]);
		}

		cout << "#" << test_case << " " << ans << endl;
	}
	return 0;
}

#elif 0

typedef long long ll;
int tmpA[200000];
int tmpB[200000];
int cmp(ll a1, ll b1, ll a2, ll b2) {
	return (((a1 * b2) + b1) >= ((a2 * b1) + b2));
}

void mergeSort(int s, int e, int* a, int* b) {
	if (s >= e)	return;
	int mid = (s + e) / 2;
	mergeSort(s, mid, a, b);
	mergeSort(mid + 1, e, a, b);
	int i = s, j = mid + 1, k = s;
	while (i <= mid && j <= e) {
		if (cmp((ll)a[i], (ll)b[i], (ll)a[j], (ll)b[j])) {
			tmpA[k] = a[i]; tmpB[k++] = b[i++];
		}
		else {
			tmpA[k] = a[j]; tmpB[k++] = b[j++];
		}
	}
	while (i <= mid) {
		tmpA[k] = a[i];
		tmpB[k++] = b[i++];
	}
	while (j <= e) {
		tmpA[k] = a[j];
		tmpB[k++] = b[j++];
	}
	for (i = s; i <= e; i++) {
		a[i] = tmpA[i];
		b[i] = tmpB[i];
	}
}

int MinRailSpeed(int N, int* a, int* b) {
	ll v = 1;
	long rem = 1000000007;
	mergeSort(0, N - 1, a, b);
	for (int i = 0; i < N; i++) {
		v = (int)(((long long)(a[i]) * v + b[i]) % 1000000007);
	}
	return (int)v;
}
#elif 0
#include <bits/stdc++.h>
typedef long long ll;
using namespace std;

pair<int, int> pii[200000];

bool comp(const pair<int, int>& i0, const pair<int, int>& i1) {
	return (((ll)i1.first * i0.second + (ll)i1.second) < ((ll)i0.first * i1.second + (ll)i0.second));
}

int MinRailSpeed(int N, int* a, int* b) {
	int v = 1;
	for (int i = 0; i < N; i++) 	pii[i] = { a[i], b[i] };

	sort(pii, pii + N, comp);

	for (int i = 0; i < N; i++) {
		v = (int)(((long long)(pii[i].first) * v + pii[i].second) % 1000000007);
	}
	return v;
}


#elif 0
//Test 222
//Test 333
//Test 444
//Test 555

#include<iostream>
#include<algorithm>

using namespace std;

bool compare(const pair<int, int>& a, const pair<int, int>& b)
{
	//If the first number is same
	if (a.first == b.first)
		return a.second > b.second; //The second number in Descending order
	return a.first > b.first; //The first number of bigger numbers to move forward -> Descending order
}

int main() {
	int N;
	pair<int, int> arr[1000];
	cin >> N;
	for (int i = 0; i < N; i++) {
		int a, b;
		cin >> a >> b;
		arr[i] = { a,b };
		//cin >> arr[i].first >> arr[i].second;		
	}

	//Descending order
	//sort(arr, arr + N, compare);
	sort(arr, arr + N, compare);

	//Print
	cout << endl;
	for (int i = 0; i < N; i++)
		cout << arr[i].first << " " << arr[i].second << endl;
	return 0;
}
#elif 0
//[1강 3번] 아나그램 
#include<iostream>
using namespace std;
int a[26];

bool compare() {
	for (int i = 0; i < 26; i++) {
		if (a[i] != 0)	return false;
	}
	return true;
}

int FindAnagram(int l1, char* s1, int l2, char* s2) {
	int ans = 0;
	for (int i = 0; i < 26; i++)	a[i] = 0;
	for (int i = 0; i < l1; i++)	a[s1[i] - 'a'] ++;

	// 슬라이딩 윈도우 방식을 위해
	// N-1까지 연산을 해 놓는다.
	for (int i = 0; i < l1 - 1; i++)	a[s2[i] - 'a'] --;

	// 슬라이딩이 하나씩 증가할때 마다 계산을 진행한다.
	for (int i = l1 - 1; i < l2; i++) {
		// 슬라이딩 되면서 a배열의 값을 증가하고 감소하고를 진행하면서 
		// 아나그램인지를 판별한다. 
		a[s2[i] - 'a'] --;
		if (compare())	ans++;
		a[s2[i - (l1 - 1)] - 'a'] ++;
	}
	return ans;
}

#elif 0
#include <stdio.h>
#include <algorithm>
#include <iostream>
using namespace std;

int main() {


	int N, a, b, c, d;
	scanf("%d ", &N);
	for (int tc = 1; tc <= N; tc++) {
		scanf("%d %d %d %d", &a, &b, &c, &d);
		printf("#%d %d\n", tc, max(min(b, d) - max(a, c), 0));
	}


	return 0;
}

#elif 0
#include <stdio.h>
#include <algorithm>
int a, b, c, d, x, y, z, t, T;
int main() {
	scanf("%d", &T);
	for (t = 1; t <= T; t++) {
		scanf("%d%d%d%d", &a, &b, &c, &d);
		x = b > d ? d : b;
		y = a > c ? a : c;
		z = x > y ? x - y : 0;
		printf("#%d %d\n", t, z);
	}
}

#elif 0
#include <bits/stdc++.h>
using namespace std;

int main() {

	cout << INT_MAX << endl;
	cout << UINT_MAX << endl;
	cout << LONG_MAX << endl;
	cout << ULONG_MAX << endl;
	cout << LLONG_MAX << endl;
	cout << ULLONG_MAX << endl;
	return 0;

}

#elif 0

#include <algorithm>
using namespace std;

extern int CalcFinalSpeed(int N, int* a, int* b, int* p);

int arr[20000];
int rank[20000];

int MinRailSpeed(int N, int* a, int* b) {
	int answer = 0;

	for (int i = 0; i < N; i++)
	{
		arr[i] = (a[i] - 1) / b[i];
	}

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (arr[i] < arr[j])
				::rank[i]++;
		}
	}


	sort(a, a + N);


	return answer;
}


#elif 0 
#include <iostream>
#include <algorithm>
using namespace std;

bool compare(int a, int b) {
	return a > b;
}

class Student {
public:
	string name;
	int score;
	Student(string name, int score) {
		this->name = name;
		this->score = score;;
	}
	bool operator < (Student& student) {
		return this->score < student.score;
	}
};
int main() {
	int a[10] = { 10,		3,  4, 5,2,4,6,6, 9, 0 };

	Student student = { Student("Lee", 90) };

	sort(a, a + 10, compare);
	for (int i = 0; i < 10; i++) {
		cout << a[i] << " ";
	}
	return 0;
}

#elif 0
#define NULL 0
#define MAX_NODE	20000
#define MAX_HASH_SIZE	30000

struct Node {
	int id, pos, size;
	Node* prev;
	Node* next;
	Node* up;
	Node* down;
	Node* fprev;
	Node* fnext;
};

Node NodePool[MAX_NODE];
int NodePoolCnt, Usage;
Node* Head;
Node* Top;
Node* Bottom;
Node* FreeHead;

struct HashId {
	int key;
	Node* data;
};
HasId HashIdTbl[MAX_HASH_SIZE];

Node* findId(int key) {
	unsigned long h = key % MAX_HASH_SIZE;
	int cnt = MAX_HASH_SIZE;
	while (HashIdTbl[h].key != -1 && cnt--) {
		if (HashIdTbl[h].key = key) {
			return HashIdTbl[h].data;
		}
	}
	return NULL;
}


#elif 0
#include <iostream>
#include <algorithm>
#include <set>
using namespace std;

void print(set<int>& s, set<int>::iterator it) {
	if (it == s.end())
		cout << "end\n";
	else
		cout << *it << '\n';
}

int main() {
	set<int> s;

	///오름차순으로 저장///
	s = { 5,2,4,1,3,7,6 };

	for (auto it = s.begin(); it != s.end(); ++it)    //O(n)
		cout << *it << ' ';
	cout << '\n';

	for (auto x : s)    //O(nlogn)
		cout << x << ' ';
	cout << '\n';

	return 0;
}
#elif 0
#include <iostream>
#include <set>
#include <string>

using namespace std;

int main() {

	// set
	set<string> s;


	// insert(element)
	s.insert("abc");
	s.insert("abc");
	s.insert("abc");
	s.insert("abc");
	s.insert("abc");
	s.insert("abc");
	s.insert("def");
	s.insert("ghi");
	s.insert("jkl");


	// erase(element)
	s.erase("jkl");


	// empty(), size()
	if (!s.empty()) cout << "s size: " << s.size() << '\n';


	// find(element)
	cout << *s.find("abc") << '\n';
	cout << *s.find("def") << '\n';


	// count(element)
	cout << "abc count: " << s.count("abc") << '\n';


	// begin(), end()
	cout << "traverse" << '\n';
	for (auto it = s.begin(); it != s.end(); it++) {
		cout << "value: " << *it << '\n';
	}

	return 0;

}
#elif 0
#include <iostream>
#include <string>
#include <unordered_map>

int main()
{
	std::unordered_map<std::string, int> scores;
	int hcnt = 0;
	scores["Evan"] = ++hcnt;
	scores["Lee"] = ++hcnt;
	scores["Jung"] = ++hcnt;

	for (auto it = scores.begin(); it != scores.end(); ++it)
	{
		std::cout << it->first << " : " << it->second << std::endl;
	}

	return 0;
}
#elif 0
#include <unordered_map>
#include <set>
#include <queue>
#include <string>
using namespace std;
const int LM = 20003;
int hcnt, tick, totSize;
struct Access {
	int id, tick;
};
struct Range {
	int s, e;
	bool operator<(const Range& r) const {
		return s < r.s;
	}
};
struct File {
	int s, e, tick;
}file[LM];

set<Range> emptyMem;
queue<Access> accessLog;
unordered_map<int, int> htab;

void init(int n) {
	htab.clear();
	totSize = tick = hcnt = 0;
	emptyMem = { {0,n - 1} };
	accessLog = {};
	memset(file, 0, sizeof(file));
}

int access(int dataID, int dataSize) {
	int id = htab[dataID];
	if (!id) htab[dataID] = id = ++hcnt;

	if (!file[id].tick) {
		totSize += dataSize;

		auto it = emptyMem.begin();
	}
}


#elif 0


struct Cache {
	Cache* prev, *next;
	int id, start, size;
};
struct LRU {
	int id;
	LRU* prev, *next;
};

Cache* cachedHead;
Cache* cacheTail;
LRU* lruHead;
LRU* lruTail;


#elif 0

#include <iostream>
using namespace std;

template<typename T>
class C_DATA
{
private:
	T m_tData;

public:
	C_DATA();
	void setData(T tData);
	T getData();

};

int main()
{
	C_DATA<float> cData;
	cData.setData(10.10f);
	printf("%f \n", cData.getData());
	return 0;
}

template<typename T>
C_DATA<T>::C_DATA() :
	m_tData()
{
}

template<typename T>
void C_DATA<T>::setData(T tData)
{
	m_tData = tData;
}

template<typename T>
T C_DATA<T>::getData()
{
	return m_tData;
}



#elif 0
//[H2125] 조립식 컴퓨터
//모범답안

#include <iostream>
using namespace std;

struct Complex {
	int real;
	int imaginary;
};

template<typename T>
T sum(T a, T b) {
	return a + b;
}

template<>
Complex sum(Complex a, Complex b) {
	return Complex{
		a.real + b.real,
		a.imaginary + b.imaginary
	};
}

int main() {
	cout << sum(1, 2) << endl; // 3
	cout << sum(1.1, 2.2) << endl; // 3.3
	Complex{ 1, 2 };

	Complex aa;
	aa = sum(Complex{ 1, 2 }, Complex{ 3, 4 }); // Complex { 4, 6 }
	cout << aa.real << aa.imaginary << endl;

	Complex aaa = Complex{ 10,20 };
	Complex bbb = Complex{ 10,20 };
	aa = sum(aaa, bbb);
	cout << aa.real << aa.imaginary << endl;
	//sum(1.1, 2.2) << endl; // 3.3
	return 0;
}

#elif 0
#include <stdio.h>
#include <string.h>
#define Swap(a, b) { int t = (a); (a) = (b); (b) = t; }

struct Node {
	char data;
	Node* prev, *next;
}node[1000];
int idx = 0;

Node* myalloc() {
	return &node[idx++];
}

Node* pHead = myalloc();
Node* pTail = myalloc();


void addnode(Node* t, char c)
{


}
int main() {
	char a[100], b[100];
	strcpy(a, "Evan Lee");
	strcpy(b, "evan Lee");

	/*int a = 10;
	int b = 20;
	Swap(a, b);
	printf("%d %d \n", a, b);*/
	printf("%s \n", a);
	printf("%s \n", b);
	printf("%d \n", strcmp(a, b));


	return 0;
}

#elif 0
#include<bits/stdc++.h>
using namespace std;
typedef pair<int, int> pii;
typedef pair<pii, int> pp;
#define xx first
#define yy second
pii dir[4] = { {0,-1},{1,0},{0,1},{-1,0} };
pii operator+(const pii& p1, const pii& p2) {
	return pii(p1.xx + p2.xx, p1.yy + p2.yy);
}
vector<pp> r[2];
void init(int N) {
	r[0] = { {{N,N},0} };
	r[1] = { {{N,N},2} };
}
void addRail(int d1, int d2) {
	int pd = r[d1].back().yy, cd;
	if (d2 == 0) cd = (pd + 1) % 4;
	if (d2 == 1) cd = pd;
	if (d2 == 2) cd = (pd + 3) % 4;
	r[d1].emplace_back(r[d1].back().xx + dir[pd], cd);
}
int delRail(int x, int y) {
	for (int i = 0, j; i < 2; ++i) {
		for (j = 0; j < r[i].size(); ++j)
			if (r[i][j].xx == pii(x, y)) break;
		if (j == r[i].size()) continue;
		int ret = r[i].size() - j;
		if (r[0].back().xx + dir[r[0].back().yy] == r[1].back().xx &&
			r[1].back().xx + dir[r[1].back().yy] == r[0].back().xx)
		{
			for (int k = r[i].size() - 1; k > j; --k)
				r[!i].emplace_back(r[i][k].xx, (r[i][k - 1].yy + 2) % 4);
			ret = 1;
		}
		r[i].erase(r[i].begin() + j, r[i].end());
		return ret;
	}
	return 0;
}
#elif 0
const int N = 5000 + 1;
struct Node {
	Node* prev, *next;
	bool direction;
	int r, c;
}node[2 * N][2 * N];

struct List {
	Node* pHead = new Node();
	Node* pTail = new Node();

	static void link(Node* front, Node* back)
	{
		front->next = back;
		back->prev = front;
	}
	static void erase(Node* node)
	{
		link(node->prev, node->next);
	}
	void initialize()
	{
		link(pHead, pTail);
	}
	bool isEmpty()
	{
		return pHead->next == pTail;
	}
	void push_front(Node* node)
	{
		link(pHead, node);
		link(node, pHead->next);
	}
	void push_rear(Node* node)
	{
		link(node, pTail);
		link(pTail->next, node);
	}
}l;

void init(int N)
{
	l.initialize();
}

void addRail(int mFront, int mDirection)
{

	if (mFront == 1)
	{
		Node* Head = l.pHead->next;
		Head->r
			Head->c

			//l.push_front(node);
	}
	else
	{
		l.push_rear(node);
	}
}

int delRail(int mRow, int mCol)
{
	Node* it = &node[mRow][mCol];

	if (it->direction == 1)
	{
		for (; it != pTail; it++)
		{

		}
	}

}



#elif 0
#include<iostream>
#define rint register int 
const int MAX_ID = 100000;
const int MAX_TEAM = 5;
const int MAX_SCORE = 5;
const int MIN_TEAM = 1;
const int MIN_SCORE = 1;

struct Node {
	int id, team;
	Node* prev, *next;
}soldier[MAX_ID + 1];


struct List {
#if 0
	Node* phead = new Node();
	Node* ptail = new Node();
#else 
	Node head, tail;
	Node* phead = &head;
	Node* ptail = &tail;
#endif

	List() { phead->prev = ptail->next = nullptr; }
	//~List() { delete phead, ptail; }
	static void link(Node* front, Node* back) {
		front->next = back;
		back->prev = front;
	}
	static void erase(Node* node) {
		link(node->prev, node->next);
	}
	void initialize() {
		link(phead, ptail);
	}
	void insert_back(Node* node) {
		link(ptail->prev, node);
		link(node, ptail);
	}
	bool isEmpty() {
		return (phead->next == ptail);
	}
	void splice(List* list) {
		if (list->isEmpty())	return;
		link(ptail->prev, list->phead->next);
		link(list->ptail->prev, ptail);
		list->initialize();
	}
}soldierGroup[MAX_TEAM + 1][MAX_SCORE + 1];

void init() {
	for (rint i = 1; i <= 5; i++)
		for (rint j = 1; j <= 5; j++)
			soldierGroup[i][j].initialize();
}

void hire(int mId, int mTeam, int mScore) {
	soldier[mId] = { mId, mTeam };
	soldierGroup[mTeam][mScore].insert_back(soldier + mId);
}

void fire(int mId) {
	List::erase(soldier + mId);
}

void updateSoldier(int mId, int mScore) {
	fire(mId);
	hire(mId, soldier[mId].team, mScore);
}


void updateTeam(int mTeam, int mChangeScore) {
	if (mChangeScore > 0) {
		for (rint i = MAX_SCORE - 1; i >= 1; i--) {
			int newScore = i + mChangeScore;
			if (newScore > MAX_SCORE)newScore = MAX_SCORE;
			soldierGroup[mTeam][newScore].splice(&soldierGroup[mTeam][i]);
		}
	}
	else if (mChangeScore < 0) {
		for (rint i = MIN_SCORE + 1; i <= MAX_SCORE; i++) {
			int newScore = i + mChangeScore;
			if (newScore < MIN_SCORE)newScore = MIN_SCORE;
			soldierGroup[mTeam][newScore].splice(&soldierGroup[mTeam][i]);
		}
	}
}

int bestSoldier(int mTeam) {

	List* maxScoreG;
	for (rint i = 5; i >= 1; i--) {
		if (!soldierGroup[mTeam][i].isEmpty()) {
			maxScoreG = &soldierGroup[mTeam][i];
			break;
		}
	}

	int ret = 0;
	Node* it = maxScoreG->phead->next;
	for (; it != maxScoreG->ptail; it = it->next) {
		if (ret < it->id)	ret = it->id;
	}
	return ret;
}

#elif 0
//#define _Evan_
#include<iostream>
#include<list>
using namespace std;

list<int> li[6][6];

struct Mercenary
{
	int team;
	list<int>::iterator iter;
}mer[100003];

void init() {
	for (int i = 1; i <= 5; i++)
		for (int j = 1; j <= 5; j++)
			li[i][j].clear();
}

void hire(int id, int team, int score) {
	mer[id] = { team, li[team][score].insert(li[team][score].end(), id) };
}

void fire(int id) {
	*mer[id].iter = 0;
}

void updateSoldier(int id, int score) {
	*mer[id].iter = 0;
	hire(id, mer[id].team, score);
}

void updateTeam(int team, int newScore) {
	if (newScore < 0)
		for (int i = 2; i <= 5; i++) {
			int score = max(1, i + newScore);
			li[team][score].splice(li[team][score].end(), li[team][i]);
		}
	else if (newScore > 0)
		for (int i = 4; i >= 1; i--) {
			int score = min(5, i + newScore);
			li[team][score].splice(li[team][score].end(), li[team][i]);
		}
}

int bestSoldier(int team) {
	int ret = 0;
	for (int i = 5; i >= 1; i--) {
		if (li[team][i].empty()) continue;
		for (auto x : li[team][i]) ret = max(x, ret);
		if (ret) break;
	}
	return ret;
}

#elif 0
struct Node {
	int id, team;
	Node* prev, *next;
}node[100000];

struct List {
	Node* phead = new Node;
	Node* ptail = new Node;

	static void link(Node* f, Node* b)
	{
		f->next = b;
		b->prev = f;
	}
	static void erase(Node* node)
	{
		link(node->prev, node->next);
	}
	void insert_back(Node* node)
	{
		link(ptail->prev, node);
		link(node, ptail);
	}
	void initialize()
	{
		link(phead, ptail);
	}
	bool isEmpty()
	{
		return phead->next == ptail;
	}
	void splice(List* list)
	{
		if (list->isEmpty()) return;
		link(ptail->prev, list->phead->next);
		link(list->ptail->prev, ptail);
		list->initialize();
	}
}list[6][6];

void init()
{
	for (int i = 1; i < 6; i++)
		for (int j = 1; j < 6; j++)
			list[i][j].initialize();
}

void hire(int mId, int mTeam, int mScore)
{
	node[mId] = { mId, mTeam };
	list[mTeam][mScore].insert_back(node + mId);
}

void fire(int mId)
{
	List::erase(node + mId);
}

void updateSoldier(int mId, int mScore)
{
	fire(mId);
	hire(mId, node[mId].team, mScore);
}

void updateTeam(int mTeam, int mChangeScore)
{
	if (mChangeScore > 0)
	{
		for (int i = 4; i >= 1; i--)
		{
			int newscore = (i + mChangeScore) > 5 ? 5 : (i + mChangeScore);
			list[mTeam][newscore].splice(&list[mTeam][i]);
		}
	}
	else if (mChangeScore < 0)
	{
		for (int i = 2; i <= 5; i++)
		{
			int newscore = (i + mChangeScore) < 1 ? 1 : (i + mChangeScore);
			list[mTeam][newscore].splice(&list[mTeam][i]);
		}
	}
}

int bestSoldier(int mTeam)
{
	List* l = nullptr;
	for (int i = 5; i > 0; i--)
	{
		if (!list[mTeam][i].isEmpty())
		{
			l = &list[mTeam][i];
			break;
		}
	}

	int ret = 0;
	for (Node* iter = l->phead->next; iter != l->ptail; iter = iter->next)
	{
		if (ret < iter->id)	ret = iter->id;
	}
	return ret;

}
#elif 0
#define MAX_SOLDIER     100000
#define MAX_SCORE       5
#define MAX_TEAM        5

struct SOLDIER {
	int id;
	int team;
	SOLDIER* prev;
	SOLDIER* next;
};
SOLDIER soldier[MAX_SOLDIER + 1];

SOLDIER head[MAX_TEAM + 1][MAX_SCORE + 1]; // NOT pointer, BUT object
SOLDIER tail[MAX_TEAM + 1][MAX_SCORE + 1];

void init() {
	register int i, j;

	for (i = 1; i <= MAX_TEAM; ++i)
		for (j = 1; j <= MAX_SCORE; ++j) {
			head[i][j].next = &tail[i][j];
			tail[i][j].prev = &head[i][j];
		}
}

void hire(int mID, int mTeam, int mScore) {

	register SOLDIER* p = &soldier[mID];
	SOLDIER& T = tail[mTeam][mScore];

	p->id = mID;
	p->team = mTeam;

	T.prev->next = p;
	p->prev = T.prev;
	T.prev = p;
	p->next = &T;
}

void fire(int mID)
{
	register SOLDIER* p = &soldier[mID];
	p->prev->next = p->next;
	p->next->prev = p->prev;
}

void updateSoldier(int mID, int mScore) {

	register SOLDIER* p = &soldier[mID];
	p->prev->next = p->next;
	p->next->prev = p->prev;

	SOLDIER& T = tail[p->team][mScore];

	T.prev->next = p;
	p->prev = T.prev;
	T.prev = p;
	p->next = &T;
}

void updateTeam(int mTeam, int mChangeScore)
{
	if (mChangeScore == 0)
		return;
	int start, end, dir;

	if (mChangeScore > 0)
		start = MAX_SCORE - 1, end = 0, dir = -1;
	else
		start = 2, end = MAX_SCORE + 1, dir = 1;

	for (register int i = start; i != end; i += dir) {
		int ns = (i + mChangeScore > 5) ? 5 : i + mChangeScore;
		ns = ns < 1 ? 1 : ns;

		SOLDIER& Hos = head[mTeam][i];    // original score
		SOLDIER& Tos = tail[mTeam][i];
		SOLDIER& Tns = tail[mTeam][ns];  // new score   

		Tns.prev->next = Hos.next;
		Hos.next->prev = Tns.prev;
		Tns.prev = Tos.prev;
		Tos.prev->next = &Tns;

		Tos.prev = &Hos;
		Hos.next = &Tos;
	}

}

int bestSoldier(int mTeam)
{
	register SOLDIER* p;
	register int i;
	int max_mid = -1;
	for (i = MAX_SCORE; i >= 1; --i) {
		for (p = head[mTeam][i].next; p != &tail[mTeam][i]; p = p->next) {
			if (p->id > max_mid)
				max_mid = p->id;
		}
		if (max_mid > 0)
			break;
	}
	return max_mid;
}

#elif 0
#include<iostream>
using namespace std;

const int N = 10;
struct Node
{
	int a, b;
	Node()
	{
		a = 10, b = 20;
	}
	Node(int a, int b) :a(a), b(b) {}

};
Node node[N];


bool operator<(Node n1, Node n2) { return n1.a < n2.a ? 1 : 0; }


void printnode()
{
	for (int i = 0; i < N; i++)
		cout << node[i].a << " " << node[i].b << endl;
}
int main() {

	printnode();


	//Node A, B;
	Node A(100, 200);
	Node B(1000, 2000);

	//= { 100, 200 };
	//B = { 1000, 2000 };

	if (B < A)
		cout << "A" << endl;
	else
		cout << "B" << endl;


	return 0;
}

#elif 0
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <unordered_map>
using namespace std;
vector<int> hm;
const int N = 10;
int a[N];
volatile int o = 0;

int main() {


	for (int i = 0; i < N; i++)
	{
		hm.push_back(i);
	}

	o = 0;
	for (int& name : hm)
	{
		//cout << name << endl;
		//while (0) {};

		o++;
		//cout << o << endl;
	}

	o = 0;
	for (int i = 0; i < hm.size(); i++)
	{
		o++;
		//cout << o << endl;
	}

	o = 0;
	for (vector<int>::iterator it = hm.begin(); it != hm.end(); it++)
	{
		o++;
		cout << *it << endl;
	}

	cout << hm.front() << endl;
	cout << *(--(hm.end())) << endl;

	return 0;
}


#elif 0

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

string solution(vector<string> participant, vector<string> completion) {
	//string answer = "";
	unordered_map<string, int> participants;
	for (const string& name : participant)
	{
		participants[name]++;
	}

	for (const string& name : completion)
	{
		participants[name]--;
	}

	for (auto pair : participants)
	{
		if (pair.second > 0)
			return pair.first;
	}

	//return answer;
}


#elif 0
#include<unordered_map>
#include<iostream>
#include<string>
using namespace std;

unordered_map<string, int>	htab;

int main() {
	htab["Evan"] = 1;
	htab["Jung"] = 2;
	htab["Choi"] = 1;

	cout << htab["Evan"] << endl;

	return 0;
}
#elif 0
#include<unordered_map>
#include<iostream>
using namespace std;

unordered_map<int, int> m;


void printmap() {
	for (auto& i : m)
		cout << i.first << ":" << i.second << endl;

}
int main() {

	m[10000000] = 1;
	m[20000000] = 2;
	m[30000000] = 3;
	m[40000000] = 4;

	printmap();
	m.clear();

	printmap();

	m[10000000] = 1;
	m[20000000] = 2;
	m[30000000] = 3;
	m[40000000] = 4;

	printmap();

	cout << m[20000000] << endl;
	return 1;
}

#elif 0
#include <iostream>
#include <unordered_map>
using namespace std;
int main() {
	unordered_map<string, int> m; // map<key, value>로 구성
	m.insert({ "Kally", 25 });
	m.insert(pair<string, int>("Tom", 32));
	m.insert(make_pair("John", 73));

	m.inert({ "Kelly", 10 });
	m.insert(pair<string, int>(""))
		m["Evan"] = 51;
	m["Evan"] = 53;
	// insert된 노드 확인
	// 1번째 방법: while 문 처럼 괄호 안에 조건이 맞지 않을 때까지 반복

	m.erase("Kally");
	for (auto iter = m.begin(); iter != m.end(); iter++) {
		cout << iter->first << ":" << iter->second << " ";
	} cout << endl;
	// 2번째 방법: 범위 기반 for문
	for (pair<string, int> iter : m) {
		cout << iter.first << ":" << iter.second << " ";
	} cout << endl;
	for (auto& i : m) {
		cout << i.first << ":" << i.second << " ";
	} cout << endl;

	cout << m["Tom"] << ":" << m["tom"] << endl;

	return 0;
}

#elif 0
#include<unordered_set>
#include<unordered_map>
using namespace std;

const int LM = 10003;
unordered_set<int> chart[LM];
unordered_map<int, int> htab;
int fear[LM], curFear[LM], parent[LM], idcnt;
int total, cnt;

int getId(int x) {
	if (!htab[x]) return htab[x] = ++idcnt;
	return htab[x];
}

void init() {
	htab.clear();
	for (int i = 1; i <= idcnt; i++) chart[i].clear();
	htab[10000] = idcnt = 1;
	total = cnt = 0;
}

int delta;
int dfs(int x, int cmd = 0) {
	if (cmd == 1 && fear[x]) curFear[x] += delta, total += delta;
	if (cmd == 2) total += (fear[x] - curFear[x]), curFear[x] = fear[x];
	if (cmd == 3 && fear[x]) total -= curFear[x], cnt--;
	int sum = curFear[x];
	for (auto y : chart[x]) sum += dfs(y, cmd);
	return sum;
}

int cmdAdd(int id, int teamID, int fearIndex) {
	id = getId(id), teamID = getId(teamID);
	chart[teamID].insert(id);
	curFear[id] = fear[id] = fearIndex;
	parent[id] = teamID;
	total += fearIndex;
	if (fearIndex) cnt++;
	return dfs(teamID);
}

int cmdMove(int id, int teamID) {
	id = getId(id), teamID = getId(teamID);
	chart[teamID].insert(id);
	chart[parent[id]].erase(id);
	parent[id] = teamID;
	return dfs(teamID);
}

int cmdInfect(int id) {
	if (cnt) delta = total / cnt;
	return dfs(getId(id), 1);
}
int cmdRecover(int id) {
	return dfs(getId(id), 2);
}
int cmdRemove(int id) {
	id = getId(id);
	chart[parent[id]].erase(id);
	return dfs(id, 3);
}

#elif 0
#define NULL 0
#define MAX_RIDE 100
#define MAX_HASH_SIZE 200
#define MAX_PEOPLE_SIZE 10000


struct People {
	int num;
	int priority;
};


struct Ride {
	int idx;
	int id;
	int duration;
	int capacity;
	int people;
	int time;
};


People PeoplePool[MAX_PEOPLE_SIZE];
int PeoplePoolCnt;
Ride RidePool[MAX_RIDE];
Ride* RideArr[MAX_RIDE];
int RideCnt;


struct HashId {
	int key;
	Ride* data;
};
HashId HashIdTbl[MAX_HASH_SIZE];


Ride* findId(int key) {
	unsigned long h = key % MAX_HASH_SIZE;
	int cnt = MAX_HASH_SIZE;


	while (HashIdTbl[h].key != -1 && cnt--) {
		if (HashIdTbl[h].key == key) {
			return HashIdTbl[h].data;
		}
		h = (h + 1) % MAX_HASH_SIZE;
	}
	return NULL;
}


void addId(int key, Ride* data) {
	unsigned long h = key % MAX_HASH_SIZE;


	while (HashIdTbl[h].key != -1) {
		h = (h + 1) % MAX_HASH_SIZE;
	}


	HashIdTbl[h].key = key;
	HashIdTbl[h].data = data;
}


People* Heap[MAX_RIDE][MAX_PEOPLE_SIZE];
int HeapSize[MAX_RIDE];


void heapPush(int rideIdx, People* value) {
	Heap[rideIdx][HeapSize[rideIdx]] = value;


	int current = HeapSize[rideIdx];
	while (current > 0 && Heap[rideIdx][current]->priority > Heap[rideIdx][(current - 1) / 2]->priority) {
		People* temp = Heap[rideIdx][(current - 1) / 2];
		Heap[rideIdx][(current - 1) / 2] = Heap[rideIdx][current];
		Heap[rideIdx][current] = temp;
		current = (current - 1) / 2;
	}


	HeapSize[rideIdx]++;
}


People* heapPop(int rideIdx) {
	if (HeapSize[rideIdx] <= 0) {
		return NULL;
	}


	People* ret = Heap[rideIdx][0];
	HeapSize[rideIdx]--;


	Heap[rideIdx][0] = Heap[rideIdx][HeapSize[rideIdx]];


	int current = 0;
	while (current * 2 + 1 < HeapSize[rideIdx]) {
		int child;
		if (current * 2 + 2 == HeapSize[rideIdx]) {
			child = current * 2 + 1;
		}
		else {
			child = Heap[rideIdx][current * 2 + 1]->priority > Heap[rideIdx][current * 2 + 2]->priority ? current * 2 + 1 : current * 2 + 2;
		}


		if (Heap[rideIdx][current]->priority > Heap[rideIdx][child]->priority)
			break;


		People* temp = Heap[rideIdx][current];
		Heap[rideIdx][current] = Heap[rideIdx][child];
		Heap[rideIdx][child] = temp;


		current = child;
	}
	return ret;
}


People* newPeople(int priority, int num) {
	People* ret = &PeoplePool[PeoplePoolCnt++];
	ret->priority = priority;
	ret->num = num;
	return ret;
}


void process(Ride* ride, int tStamp) {
	while (ride->time <= tStamp && HeapSize[ride->idx] > 0) {
		int remainder = ride->capacity;
		while (remainder > 0 && HeapSize[ride->idx] > 0) {
			People* curr = Heap[ride->idx][0];
			if (remainder < curr->num) {
				curr->num -= remainder;
				ride->people -= remainder;
				break;
			}


			remainder -= curr->num;
			ride->people -= curr->num;
			heapPop(ride->idx);
		}
		ride->time += ride->duration;
	}
}


////////////////////////////////////////////////


void init(int N, int mId[], int mDuration[], int mCapacity[]) {
	RideCnt = PeoplePoolCnt = 0;
	for (int i = 0; i < MAX_HASH_SIZE; ++i) {
		HashIdTbl[i].key = -1;
	}


	for (int i = 0; i < MAX_RIDE; ++i) {
		HeapSize[i] = 0;
	}


	for (int i = 0; i < N; ++i) {
		Ride* ride = &RidePool[RideCnt++];
		addId(mId[i], ride);
		ride->idx = i;
		ride->id = mId[i];
		ride->duration = mDuration[i];
		ride->capacity = mCapacity[i];
		ride->people = ride->time = 0;
		RideArr[i] = ride;
	}
}


int add(int tStamp, int mId, int mNum, int mPriority) {
	Ride* ride = findId(mId);
	process(ride, tStamp - 1);
	if (ride->time < tStamp) {
		ride->time = tStamp;
	}


	ride->people += mNum;
	heapPush(ride->idx, newPeople(mPriority, mNum));
	process(ride, tStamp);


	if (!HeapSize[ride->idx])
		return 0;
	return Heap[ride->idx][0]->priority;
}


void search(int tStamp, int mCount, int mId[], int mWait[]) {
	for (int i = 0; i < RideCnt; ++i) {
		process(&RidePool[i], tStamp);
	}


	for (int i = 0; i < mCount; ++i) {
		for (int j = i + 1; j < RideCnt; ++j) {
			if (RideArr[i]->people < RideArr[j]->people
				|| (RideArr[i]->people == RideArr[j]->people && RideArr[i]->id < RideArr[j]->id)) {
				Ride* tmp = RideArr[i];
				RideArr[i] = RideArr[j];
				RideArr[j] = tmp;
			}
		}
		mId[i] = RideArr[i]->id;
		mWait[i] = RideArr[i]->people;
	}
}


#elif 0
#include<list>
#include<iostream>
using namespace std;


void printlist(list<int> l) {
	list<int>::iterator it;
	for (it = l.begin(); it != l.end(); ++it) {
		cout << *it << endl;
	}
}
int main() {
	list<int> l;
	list<int>::iterator it, check;

	l.push_back(1);
	l.push_back(2);
	l.push_back(3);
	l.push_back(4);


	check = l.insert(l.begin(), 1000);

	it = l.begin();



	cout << *check << endl;
	cout << *it << endl;
	//cout << *l.begin() << endl;

	printlist(l);
	return 1;
}


#elif 0
#define N 100000
#define Max(a,b)	((a)>(b)?(a):(b))
int func1() {
	int ret = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			ret = Max(i, j);
		}
	}
	return ret;
}
int func2() {
	int ret = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (i < j)	ret = j;
		}
	}
	return ret;
}


int main() {

	func1();

	func2();

}

#elif 0
const int MIN_ID = 1;
const int MAX_ID = 100000;
const int MIN_TEAM = 1;
const int MAX_TEAM = 5;
const int MIN_SCORE = 1;
const int MAX_SCORE = 5;


struct Node
{
	int id;
	int team;
	Node* prev;
	Node* next;
} soldier[MAX_ID + 1];


struct List
{
	Node head;
	Node tail;


	static void link(Node* front, Node* back)
	{
		front->next = back;
		back->prev = front;
	}


	static void erase(Node* node)
	{
		link(node->prev, node->next);
	}


	void initialize()
	{
		link(&head, &tail);
	}


	void insert(Node* node)
	{
		link(tail.prev, node);
		link(node, &tail);
	}


	bool isEmpty()
	{
		return (head.next == &tail);
	}


	void splice(List* list)
	{
		if (list->isEmpty())
			return;
		link(tail.prev, list->head.next);
		link(list->tail.prev, &tail);
		list->initialize();
	}
} soldierGroup[MAX_TEAM + 1][MAX_SCORE + 1];


void init()
{
	for (int i = MIN_TEAM; i <= MAX_TEAM; i++)
		for (int j = MIN_SCORE; j <= MAX_SCORE; j++)
			soldierGroup[i][j].initialize();
}


void hire(int mID, int mTeam, int mScore)
{
	soldier[mID].id = mID;
	soldier[mID].team = mTeam;
	soldierGroup[mTeam][mScore].insert(soldier + mID);
}


void fire(int mID)
{
	List::erase(soldier + mID);
}


void updateSoldier(int mID, int mScore)
{
	List::erase(soldier + mID);
	soldierGroup[soldier[mID].team][mScore].insert(soldier + mID);
}


void updateTeam(int mTeam, int mChangeScore)
{
	if (mChangeScore > 0)
	{
		for (int i = MAX_SCORE - 1; i >= MIN_SCORE; i--)
		{
			int newScore = i + mChangeScore;
			if (newScore > MAX_SCORE)
				newScore = MAX_SCORE;
			soldierGroup[mTeam][newScore].splice(&soldierGroup[mTeam][i]);
		}
	}
	else if (mChangeScore < 0)
	{
		for (int i = MIN_SCORE + 1; i <= MAX_SCORE; i++)
		{
			int newScore = i + mChangeScore;
			if (newScore < MIN_SCORE)
				newScore = MIN_SCORE;
			soldierGroup[mTeam][newScore].splice(&soldierGroup[mTeam][i]);
		}
	}
}


int bestSoldier(int mTeam)
{
	List* maxScoreGroup;
	for (int i = MAX_SCORE; i >= MIN_SCORE; i--)
	{
		if (!soldierGroup[mTeam][i].isEmpty())
		{
			maxScoreGroup = &soldierGroup[mTeam][i];
			break;
		}
	}


	int maxId = MIN_ID - 1;
	Node* maxScoreSoldier = maxScoreGroup->head.next;
	while (maxScoreSoldier != &(maxScoreGroup->tail))
	{
		if (maxId < maxScoreSoldier->id)
			maxId = maxScoreSoldier->id;
		maxScoreSoldier = maxScoreSoldier->next;
	}
	return maxId;
}
#elif 0
#include<iostream> 
using namespace std;
#define rint register int
const int MIN_ID = 1;
const int MAX_ID = 100000;
const int MIN_TEAM = 1;
const int MAX_TEAM = 5;
const int MIN_SCORE = 1;
const int MAX_SCORE = 5;

struct Node {
	int id, team;
	Node* prev, *next;
}soldier[MAX_ID + 1];

struct List {
	Node* phead = new Node();
	Node* ptail = new Node;

	List() { phead->prev = ptail->next = nullptr; }
	~List() { delete phead, ptail; }

	static void link(Node* front, Node* back) {
		front->next = back;
		back->prev = front;
	}
	static void erase(Node* node) {
		link(node->prev, node->next);
	}
	void initialize() {
		link(phead, ptail);
	}
	void insert_back(Node* node) {
		link(ptail->prev, node);
		link(node, ptail);
	}
	bool isEmpty() {
		return (phead->next == ptail);
	}
	void splice(List* list) {
		if (list->isEmpty()) return;
		link(ptail->prev, list->phead->next);
		link(list->ptail->prev, ptail);
		list->initialize();
	}
}soldierGroup[MAX_TEAM + 1][MAX_SCORE + 1];

void init() {
	for (rint i = 1; i <= MAX_TEAM; i++)
		for (rint j = 1; j <= MAX_SCORE; j++)
			soldierGroup[i][j].initialize();
}

void hire(int mID, int mTeam, int mScore) {
	soldier[mID] = { mID, mTeam };
	soldierGroup[mTeam][mScore].insert_back(&soldier[mID]);
}

void fire(int mID) {
	List::erase(&soldier[mID]);
}

void updateSoldier(int mID, int mScore) {
	fire(mID);
	hire(mID, soldier[mID].team, mScore);
}

void updateTeam(int mTeam, int mChangeScore) {
	if (mChangeScore > 0) {
		for (rint i = MAX_SCORE - 1; i >= 1; i--) {
			int newScore = i + mChangeScore;
			if (newScore > MAX_SCORE)newScore = MAX_SCORE;
			soldierGroup[mTeam][newScore].splice(&soldierGroup[mTeam][i]);
		}
	}
	else if (mChangeScore < 0) {
		for (rint i = MIN_SCORE + 1; i <= MAX_SCORE; i++) {
			int newScore = i + mChangeScore;
			if (newScore < MIN_SCORE)newScore = MIN_SCORE;
			soldierGroup[mTeam][newScore].splice(&soldierGroup[mTeam][i]);
		}
	}
}

//int bestSoldier(int mTeam){	
//	List* maxScoreGroup = nullptr;
//	int ret = 0;
//	for (rint i = MAX_SCORE; i >= MIN_SCORE; i--) {		
//		if (!soldierGroup[mTeam][i].isEmpty()) {
//			maxScoreGroup = &soldierGroup[mTeam][i];
//			break;
//		}
//	}	
//	
//	Node* maxSoldier = maxScoreGroup->phead->next;
//	while (maxSoldier != maxScoreGroup->ptail) {
//		if (ret < maxSoldier->id)	ret = maxSoldier->id;
//		maxSoldier = maxSoldier->next;
//	}	
//	return ret;
//}

int bestSoldier(int mTeam) {

	List* maxScoreG;
	for (rint i = 5; i >= 1; i--) {
		if (!soldierGroup[mTeam][i].isEmpty()) {
			maxScoreG = &soldierGroup[mTeam][i];
			break;
		}
	}

	int ret = 0;
	Node* it = maxScoreG->phead->next;
	for (; it != maxScoreG->ptail; it = it->next) {
		if (ret < it->id)	ret = it->id;
	}
	return ret;
}


//
//int main() {
//	init();
//	hire(1, 1, 5);
//	hire(2, 1, 5);
//	hire(3, 1, 5);
//	hire(4, 1, 5);
//	hire(5, 1, 5);
//
//	hire(100, 1, 5);
//	
//	Node* it = soldierGroup[1][5].head.next;
//	for (it ; it->next != nullptr; it = it->next) {
//		cout << it->mId << endl;
//	}
//	cout << bestSoldier(1) << endl;
//
//	//cout << bestSoldier(1) << endl;
//}

#elif 0

#include<list>
#include<algorithm>
using namespace std;

list<int> li[6][6];

struct Mercenary {
	int team;
	list<int>::iterator iter;
}mer[100003];

void init() {
	for (int i = 1; i <= 5; i++)
		for (int j = 1; j <= 5; j++)
			li[i][j].clear();
}

void hire(int id, int team, int score) {
	mer[id] = { team, li[team][score].insert(li[team][score].end(), id) };
}

void fire(int id) {
	*mer[id].iter = 0;
}

void updateSoldier(int id, int score) {
	*mer[id].iter = 0;
	hire(id, mer[id].team, score);
}

void updateTeam(int team, int newScore) {
	if (newScore < 0)
		for (int i = 2; i <= 5; i++) {
			int score = max(1, i + newScore);
			li[team][score].splice(li[team][score].end(), li[team][i]);
		}
	else if (newScore > 0)
		for (int i = 4; i >= 1; i--) {
			int score = min(5, i + newScore);
			li[team][score].splice(li[team][score].end(), li[team][i]);
		}
}

int bestSoldier(int team) {
	int ret = 0;
	for (int i = 5; i >= 1; i--) {
		if (li[team][i].empty()) continue;
		for (auto x : li[team][i]) ret = max(x, ret);
		if (ret) break;
	}
	return ret;
}
#elif 0
#define MAX 100001

struct Node {
	int team;
	int next, prev;
}P[MAX];

struct Team {
	int h[6];
	int t[6];
}T[6];

void init() {
	for (int i = 1; i < 6; i++)
		for (int j = 1; j < 6; j++) T[i].h[j] = T[i].t[j] = -1;
}

void hire(int mID, int mTeam, int mScore) {
	P[mID].team = mTeam;
	P[mID].next = -1;
	if (T[mTeam].h[mScore] < 0) {
		T[mTeam].h[mScore] = T[mTeam].t[mScore] = mID;
		P[mID].prev = -1;
	}
	else {
		P[mID].prev = T[mTeam].t[mScore];
		P[T[mTeam].t[mScore]].next = mID;
		T[mTeam].t[mScore] = mID;
	}
}

void fire(int mID) {
	if (P[mID].prev < 0) {
		for (int i = 1; i < 6; i++) {
			if (T[P[mID].team].h[i] == mID) {
				T[P[mID].team].h[i] = P[mID].next;
				P[mID].next > -1 ? P[P[mID].next].prev = -1 : T[P[mID].team].t[i] = -1;
				break;
			}
		}
	}
	else {
		P[P[mID].prev].next = P[mID].next;
		P[mID].next > -1 ? P[P[mID].next].prev = P[mID].prev : 0;
	}
	for (int i = 1; i < 6; i++)
		T[P[mID].team].t[i] == mID ? T[P[mID].team].t[i] = P[mID].prev : 0;
}

void updateSoldier(int mID, int mScore) {
	fire(mID);
	hire(mID, P[mID].team, mScore);
}

void updateTeam(int mTeam, int mChangeScore) {
	int h[6], t[6];
	for (int i = 1; i < 6; i++) h[i] = t[i] = -1;
	for (int i = 1; i < 6; i++) {
		if (T[mTeam].h[i] < 0) continue;
		int j = i + mChangeScore;
		j < 1 ? j = 1 : j>5 ? j = 5 : 0;
		if (h[j] < 0) {
			h[j] = T[mTeam].h[i];
			t[j] = T[mTeam].t[i];
		}
		else {
			P[t[j]].next = T[mTeam].h[i];
			P[T[mTeam].h[i]].prev = t[j];
			t[j] = T[mTeam].t[i];
		}
	}
	for (int i = 1; i < 6; i++) {
		T[mTeam].h[i] = h[i];
		T[mTeam].t[i] = t[i];
	}
}

int bestSoldier(int mTeam) {
	int ID = -1;
	for (int i = 5; i > 0; i--) {
		if (T[mTeam].h[i] < 0) continue;
		int id = T[mTeam].h[i];
		while (id > -1) {
			ID < id ? ID = id : 0;
			id = P[id].next;
		}
		break;
	}
	return ID;
}

#elif 0
/*
1<= mNum <= 100
0<= mId <= 1,000,000,000
1<= tStamp <= 100,000,000
add() 10,000회
전체 함수 호출 20,000회
*/



void iniit(int mNum, int mId[], int mCapacity[], int mDuration) {

}

void add(int tSramp, int mId, int num, int mPriority) {

}

void search(int tStamp, int mNum, int mId[], int mWait[]) {

}

#elif 0
#include<iostream>

#define rint register int

class TRIE {
private:
	TRIE* child[26];
public:
	TRIE() {
		for (rint i = 0; i < 26; i++)	child[i] = nullptr;
	}

	~TRIE() {
		for (rint i = 0; i < 26; i++)	if (child[i] != nullptr)	delete child[i];
	}

	void add(char* s) {
		if (*s == '\0') {
			return;
		}
		else {
			int n = *s - 'a';
			if (child[n] == nullptr)	child[n] = new TRIE();
			child[n]->add(s + 1);
		}
	}

	bool find(char* s) {
		if (*s == '\0') {
			return 1;
		}
		else {
			int n = *s - 'a';
			if (child[n] == nullptr)	return 0;
			child[n]->find(s + 1);
		}
	}
};

int main() {

	TRIE* t = new TRIE;
	char a[1000];
	for (rint i = 0; i < 5; i++) {
		std::cin >> a;
		t->add(a);
	}

	for (rint i = 0; i < 5; i++) {
		std::cin >> a;
		std::cout << t->find(a) << std::endl;
	}

	return 1;
}
#elif 0
//#define rint register int
#include<iostream>
#include<string.h>

using namespace std;

struct Trie {
	Trie* child[26];

	Trie() {
		memset(child, 0, sizeof(child));
	}
	~Trie() {
		for (int i = 0; i < 26; i++) {
			if (child[i])
				delete(child[i]);
		}
	}
	void insert(const char* s) {
		if (*s == '\0') return;
		else {
			int n = *s - 'a';
			if (child[n] == NULL) {
				child[n] = new Trie;
			}
			child[n]->insert(s + 1);
		}
		return;
	}
	Trie* find(const char* s) {
		if (*s == '\0')	return this;
		else {
			int n = *s - 'a';
			if (child[n] == NULL)
				return NULL;
			return child[n]->find(s + 1);
		}
	}
};

int main() {


	Trie* tr = new Trie;
	tr->insert("HELLO");
	tr->insert("HELL");
	tr->insert("HALO");
	tr->insert("TRAVEL");

	cout << tr->find("HELLO") << endl;
	cout << tr->find("HALO") << endl;
	cout << tr->find("TRAVELEEEEE") << endl;
	cout << tr->find("BEE") << endl;

	delete tr;
	return 1;
}
#elif 0
#define rint register int
const int ALPS = 26;
#include<stdio.h>

class TRI {
private:
	TRI* child[26];
public:
	TRI TN[100000];
	TRI() {
		for (rint i = 0; i < ALPS; i++) {
			child[i] = nullptr;
		}
	}

	~TN() {

	}
	int tonum(char c) { return c - 'a'; }
	void insert(const char* s) {
		if (*s == '\0')	return;
		int n = tonum(*s);
		if (child[n] = NULL) {
			child[n] = &tri[idx++];
		}
		child[n]->insert(s + 1);
	}

	bool find(const char* s) {
		if (*s == '\0')	return 1;
		int n = tonum(*s);
		if (child[n] = NULL) {
			return 0;
		}
		child[n]->find(s + 1);
	}
};

TN tri[1000000];
int idx = 0;


int main() {
	int ret = -1;
	//TN root;
	tri[0].insert("like");
	ret = tri[0].find("like");
	printf("%d  \n", ret);
	return 1;
}
#elif 0

#include<stdio.h> 
#include<string.h>
#include<ctype.h>

#define rint	register int
#define ALP		26 



int idx = 0;
struct TN {
	TN* child[26];
	TN() {
		for (rint i = 0; i < ALP; i++) 	child[i] = '\0';
	}
	~TN() {
		for (rint i = 0; i < ALP; i++) 	if (child[i] != NULL) delete child[i];
	}

	TN* myalloc() {
		return &Tri[idx++];
	}

	void insert(char* str) {
		if (*str == '\0')	return;

		int n = *str - 'a';
		if (child[n] == NULL)
			child[n] = myalloc();

		child[n]->insert(str + 1);
	}
};
TN Tri[100000];
int idx = 0;




#elif 0
#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;
const int ALP = 26;

class TN {
private:
	TN* c[ALP];

public:
	TN() {
		for (int i = 0; i < ALP; i++)
			c[i] = NULL;
	}
	~TN() {
		for (int i = 0; i < ALP; i++)
			if (c[i] != NULL)
				delete c[i];
	}
	int tonum(char c) {        //문자를 숫자로 변환.
		return tolower(c) - 'a';    //대문자인 경우는 소문자로 변환.
	}

	void insert(const char* words) {
		if (*words == '\0')        //입력받은 words가 '\0'일 경우, 즉 문자열 끝인 경우.
			return;

		int next = tonum(*words);

		if (c[next] == NULL) {
			c[next] = new TN();
		}
		c[next]->insert(words + 1);
	}

	bool find(const char* words) {
		int next = tonum(*words);

		if (*words == '\0')
			return true;

		if (c[next] == NULL)
			return false;

		return c[next]->find(words + 1);
	}
};

int main(void) {

	TN tri;

	tri.insert("like");

	if (tri.find("like"))
		cout << true << endl;
	else
		cout << false << endl;

	if (tri.find("bike"))
		cout << true << endl;
	else
		cout << false << endl;


	return 0;
}

#elif 0
#include "string.h"

#define ll		unsigned long 
#define rint	register int 
#define MTB		50000
struct {
	char s[20];
	int hit;
}w[MTB];

ll hash(char s[]) {
	ll h = 5381;
	while (*s++) 		h = ((h << 5) + h + *s) % MTB;
	return h % MTB;
}

void init() {
	for (rint i = 0; i < MTB; i++)		w[i] = { 0,0 };
}

void inputWord(char mWord[20]) {
	ll h = hash(mWord);
	int cnt = MTB;
	while (w[h].s[0] != 0 && cnt--) {
		if (strcmp(w[h].s, mWord) == 0) {
			w[h].hit++;
			return;
		}
		h = (h + 1) % MTB;
	}
}

int recommend(char mUser[20], char mAnswer[20]) {


}

void banWord(char mWord[20]) {

}
#elif 0
#define rint register int
int m[100000], ln;
void init() {
	ln = 0;
}
void makeWall(int h[5]) {
	for (rint i = 0; i < 5; i++)		m[i + ln] = h[i];
	ln += 5;
}
int matchPiece(int h[5]) {

	for (rint i = ln - 5; i >= 0; i--) {
		if (
			h[4] + m[i + 0] == h[3] + m[i + 1] &&
			h[3] + m[i + 1] == h[2] + m[i + 2] &&
			h[2] + m[i + 2] == h[1] + m[i + 3] &&
			h[1] + m[i + 3] == h[0] + m[i + 4]
			)
		{
			for (rint j = i + 5; j < ln; j++) {
				m[j - 5] = m[j];
			}
			ln -= 5;
			return i + 1;
		}
	}
	return -1;
}
#elif 0
#define MAX_NODE 10000

struct Node {
	int data;
	Node* next;
};

Node node[MAX_NODE];
int nodeCnt = 0;
Node* pH;

Node* getNode(int data) {
	node[nodeCnt].data = data;
	node[nodeCnt].next = nullptr;
	return &node[nodeCnt++];
}

Node* myalloc() {
	return &node[nodeCnt++];
}

void init() {
	pH = myalloc();
	pH->next = nullptr;
}

void addNode2Head(int data) {
	Node* p = myalloc();	p->data = data;
	p->next = pH->next;
	pH->next = p;
}

void addNode2Tail(int data) {
	Node* p = myalloc();	p->data = data;
	Node* cur = pH;
	while (cur->next) cur = cur->next;
	p->next = nullptr;
	cur->next = p;
}

void addNode2Num(int data, int num) {
	Node* p = myalloc();	p->data = data;
	Node* cur = pH;
	while (--num) cur = cur->next;
	p->next = cur->next;
	cur->next = p;
}

void removeNode(int data) {
	Node* cur = pH;
	while (cur->next) {
		if (cur->next->data == data) {
			cur->next = cur->next->next;
			return;
		}
		cur = cur->next;
	}
}

int getList(int output[MAX_NODE]) {
	int ret = 0;
	for (Node* iter = pH->next; iter; iter = iter->next) {
		output[ret++] = iter->data;
	}
	return ret;
}
#elif 0
#define MAX_NODE 10000

struct Node {
	int data;
	Node* prev;
	Node* next;
};

Node node[MAX_NODE];
int nodeCnt = 0;
Node* pH, *pT;

Node* myalloc() {
	return &node[nodeCnt++];
}

Node* getNode(int data) {
	node[nodeCnt].data = data;
	node[nodeCnt].prev = nullptr;
	node[nodeCnt].next = nullptr;
	return &node[nodeCnt++];
}

void init() {
	pH = myalloc();
	pT = myalloc();
	pH->next = pT;
	pH->prev = nullptr;
	pT->next = nullptr;
	pT->prev = pH;
}

void addNode2Here(Node* phere, int data) {
	Node* p = myalloc();
	p->data = data;
	Node* L = phere->prev;
	phere->prev = p;
	p->next = phere;
	L->next = p;
	p->prev = L;
}

void addNode2Head(int data) {
	addNode2Here(pH->next, data);
}

void addNode2Tail(int data) {
	addNode2Here(pT, data);
}

void addNode2Num(int data, int num) {
	Node* iter = pH;
	while (num--) {
		iter = iter->next;
	}
	addNode2Here(iter, data);
}

int findNode(int data) {
	Node* iter = pH;
	int ret = 0;
	while ((iter = iter->next) != pT) {
		ret++;
		if (iter->data == data)
			break;
	}
	return ret;
}

void removeNode(int data) {
	Node* iter = pH;
	while ((iter = iter->next) != pT) {
		if (iter->data == data) {
			Node* R = iter->next;
			Node* L = iter->prev;
			L->next = R;
			R->prev = L;
		}
	}
}

int getList(int output[MAX_NODE]) {
	int ret = 0;
	Node* cur = pH;
	while ((cur = cur->next) != pT) {
		output[ret++] = iter->data;
	}
	return ret;
}

int getReversedList(int output[MAX_NODE]) {
	int ret = 0;
	Node* iter = pT;
	while ((iter = iter->prev) != pH) {
		output[ret++] = iter->data;
	}
	return ret;
}

#elif 0
#include<stdio.h>
#include<string.h>

char s[50000][7];

//struct {
//	char s[7];
//	bool del;
//}a[50000];
int idx = 0;

void init() {
	idx = 0;
}

void addWord(char str[]) {
	strcpy(s[idx++], str);
}

bool mystrcmp(char* s1, char* s2) {
	while (*s2 != NULL) {
		if (*s2 == '*') {
			while (*s1 != *(s2 + 1)) {
				s1++;
			}
			s2++;
			//continue;
		}
		else if (*s1 != *s2) {
			return 0;
		}
		s1++; s2++;
	}
	if (*s1 != NULL)	return 0;
	return 1;
}

int removeWord(char str[]) {
	int ret = 0;
	for (int i = 0; i < idx; i++) {
		if (s[i][0] == NULL) continue;
		if (mystrcmp(s[i], str))
			ret += 1;
		s[i][0] = NULL;
	}
	return ret;
}

int searchWord(char str[]) {
	int ret = 0;
	for (int i = 0; i < idx; i++) {
		if (s[i][0] == NULL) continue;
		ret += mystrcmp(s[i], str);
	}
	return ret;
}


int main() {
	char aa[100] = "slay";
	char bb[100] = "slay*";
	printf("%d \n", mystrcmp(aa, bb));
}
#elif 0

#include<stdio.h>
#include<string.h>

char s[50000][7];

//struct {
//	char s[7];
//	bool del;
//}a[50000];
int idx = 0;

void init() {
	idx = 0;
}

void addWord(char str[]) {
	strcpy(s[idx++], str);
}

int removeWord(char str[]) {
	for (int i = 0; i < idx; i++) {
		if (strcmp(s[i], str))
			s[i][0] = '\0';
	}
}

int searchWord(char str[]) {
	int ret = 0;
	for (int i = 0; i < idx; i++) {
		char* b = s[i];
		if (*b == NULL)	continue;
		for (int j = 0; str[j] != NULL; j++) {
			if (str[j] == *b) {
				if (str[j] == NULL) {
					ret++; break;
				}
				b++;	continue;
			}
			else if (str[j] == '*') {
				continue;
			}
			else {
				break;
			}
		}
	}
	return ret;
}


#elif 0
//간편송금
';'#define MAX_NAME_LEN(4 + 1)

const int MAXTABLE = 27 * 27 * 27 * 27 + 3;
int userTable[MAXTABLE];    // userTable[고객키값] = 고객ID;
int userN;

struct Node {
	int user, group, asset; // user: userN
	Node* prev, *next;
	Node* alloc(int _user, int _group, int _asset, Node* _prev, Node* _next) {
		user = _user, group = _group, asset = _asset;
		prev = _prev, next = _next;
		if (prev) prev->next = this;
		if (next) next->prev = this;
		return this;
	}
	void erase() {
		if (prev) prev->next = this->next;
		if (next) next->prev = this->prev;
	}
	void move(Node* _prev) {    // 노드를 맨 앞으로 옮기기(우선순위 조정에 쓰일 것)
		this->erase();
		prev = _prev;
		next = _prev->next;
		if (prev) prev->next = this;
		if (next) next->prev = this;
	}
}buf[60003], *user[30003], *number[100003]; //user: 고객, number: 계좌 번호
int cbuf;

int hash(const char* s) {
	int ret = 0;
	for (; *s; s++)
		ret = (ret * 27) + *s - 'a' + 1;
	return ret;
}

void init() {
	cbuf = userN = 0;
	for (int i = 0; i < MAXTABLE; i++)
		userTable[i] = 0;
	for (int i = 0; i <= 100000; i++)
		number[i] = 0;
}

void openAccount(int mTime, char mName[MAX_NAME_LEN], char mGroup[MAX_NAME_LEN], int mNumber, int mAsset) {
	int x = hash(mName);
	if (!userTable[x]) {    //새로운 고객
		userTable[x] = ++userN;
		user[userN] = buf[cbuf++].alloc(0, 0, 0, 0, 0); // dummy node
	}

	int userID = userTable[x];
	int groupKey = hash(mGroup);
	number[mNumber] = buf[cbuf++].alloc(userID, groupKey, mAsset, user[userID], user[userID]->next);
}

int closeAccount(int mTime, int mNumber) {
	Node* p = number[mNumber];
	p->erase();
	number[mNumber] = 0;
	return p->asset;
}

int exchange(Node* giver, Node* receiver, int value) {
	if (!giver || !receiver || giver->asset < value)
		return -1;
	giver->asset -= value;
	receiver->asset += value;
	giver->move(user[giver->user]);
	receiver->move(user[receiver->user]);

	return receiver->asset;
}
int sendByNumber(int mTime, int mNumber, int mReceiveNumber, int mValue) {
	return exchange(number[mNumber], number[mReceiveNumber], mValue);
}

int sendByName(int mTime, int mNumber, char mReceiveName[MAX_NAME_LEN], int mValue) {
	int receiverID = userTable[hash(mReceiveName)];
	if (!user[receiverID])
		return -1;
	return exchange(number[mNumber], user[receiverID]->next, mValue);
}

void sendBonus(int mTime, char mGroup[MAX_NAME_LEN], int mValue) {
	int groupKey = hash(mGroup);
	for (int i = 1; i <= userN; i++) {
		Node* p = user[i]->next;
		while (p) {
			if (p->group == groupKey) {
				p->asset += mValue;
				p->move(user[i]);
				break;
			}
			p = p->next;
		}
	}
}


#elif 0

struct { int section;    int heat;    int erased; int mark; }news[50001];
int news_idx; int SEC[100001]; int top[21];
void init() {
	for (int i = 1; i <= 50000; i++)
		news[i] = { -1, 0, 0, 0 };
	for (int i = 0; i < 100000; i++)
		SEC[i] = -1;
}
void addNews(int mSection, int mNewsId) {
	news_idx = mNewsId;
	news[news_idx] = { mSection, 1, 0, 0 };
}
void eraseNews(int mNewsId) {
	news[mNewsId].erased = 1;
}
void readNews(int mUserId, int mNewsId) {
	SEC[mUserId] = news[mNewsId].section;
	++news[mNewsId].heat;
}
void changeSection(int mNewsId, int mSection) {
	news[mNewsId].section = mSection;
}
int getList(int mUserId, int mList[]) {
	int section = SEC[mUserId];

	for (int j = news_idx; j > 0; j--)
		news[j].mark = 0;

	int temp[50005];
	for (int j = news_idx; j > 0; j--) {
		temp[j] = news[j].heat;
	}

	int cnt = 0;
	for (int i = 0; i < 10; i++) {
		int MaxV = 0; int MaxK = 0;
		for (int j = news_idx; j > 0; j--) {
			break;
			if (news[j].erased == 1 || news[j].mark == 1)   continue;
			int k = news[j].heat;
			if (section == news[j].section)  k += 10;
			if (MaxV < k) {
				MaxV = k;
				MaxK = j;
			}
		}
		if (MaxK == 0) break;
		mList[i] = MaxK;
		news[MaxK].mark = 1;
		cnt++;
	}

	return cnt;
}

#elif 0
/*
30°³ Å×½ºÆ®ÄÉÀÌ½º¸¦ ÇÕÃÄ¼­ C++ ÀÇ °æ¿ì 4ÃÊ
Èü, Á¤Àû ¸Þ¸ð¸® ÇÕÃÄ¼­ 256MB ÀÌ³», ½ºÅÃ ¸Þ¸ð¸® 1MB ÀÌ³»

¿©·¯ ÀÎÅÍ³Ý ´º½º ±â»ç¸¦ ÇÑµ¥ ¸ð¾Æ º¸¿©ÁÖ´Â ´º½º ÇÇµå ¼­ºñ½º¸¦ Á¦ÀÛÇÏ·Á ÇÑ´Ù.
±â»ç´Â 0 ~ 9 ÀÇ Á¤¼ö·Î Ç¥ÇöµÇ´Â 10 °³ ºÐ¾ß Áß ÇÏ³ª·Î ºÐ·ùµÈ´Ù.
±â»çÀÇ ºÐ¾ß´Â »ý¼ºµÉ ¶§ Á¤ÇØÁö¸ç ³ªÁß¿¡ º¯°æµÉ ¼öµµ ÀÖ´Ù.
Æ¯Á¤ »ç¿ëÀÚ¿¡°Ô Á¡¼ö°¡ ³ôÀº »óÀ§ 10°³ÀÇ ±â»ç¸¦ ÃßÃµ ±â»ç·Î º¸¿©ÁØ´Ù.
±â»çÀÇ Á¡¼ö´Â ¸ðµç »ç¿ëÀÚ°¡ ÇØ´ç ±â»ç¸¦ ÀÐÀº ÃÑ È½¼öÀÌ´Ù.
À§ Á¡¼ö¿¡ ÇØ´ç »ç¿ëÀÚÀÇ °ü½É ºÐ¾ß¿Í ÀÏÄ¡ÇÏ´Â °æ¿ì 10Á¡ÀÇ °¡»êÁ¡À» ´õÇÑ´Ù.
Á¡¼ö°¡ °°Àº °æ¿ì¿¡´Â ÃÖ±Ù¿¡ µî·ÏµÈ ±â»ç°¡ ´õ ³ôÀº ¼øÀ§¸¦ °®°Ô µÈ´Ù.
»ç¿ëÀÚÀÇ °ü½É ºÐ¾ß´Â ÇØ´ç »ç¿ëÀÚ°¡ °¡Àå ÃÖ±Ù¿¡ ÀÐÀº ±â»çÀÇ ºÐ¾ßÀÌ´Ù.
ÀÌÈÄ ÇØ´ç ±â»çÀÇ ºÐ¾ß°¡ º¯°æµÇ¾îµµ »ç¿ëÀÚÀÇ °ü½ÉºÐ¾ß´Â º¯°æµÇÁö ¾Ê´Â´Ù.
ÇØ´ç »ç¿ëÀÚ°¡ ±â»ç¸¦ ÇÑ ¹øµµ ÀÐÁö ¾Ê¾Ò´Ù¸é °ü½É ºÐ¾ß´Â ¾ø´Ù.
A
void init()
°¢ Å×½ºÆ® ÄÉÀÌ½ºÀÇ Ã³À½¿¡ È£ÃâµÈ´Ù.

void addNews(int mSection, int mNewsId)
mSection ºÐ¾ßÀÌ°í °íÀ¯¹øÈ£°¡ mNewsId ÀÎ ±â»ç°¡ Ãß°¡µÈ´Ù.
°¢ Å×½ºÆ® ÄÉÀÌ½º¿¡¼­ mNewsId °ªÀº 1ºÎÅÍ ½ÃÀÛÇÏ¿© addNews ÇÔ¼ö°¡ ºÒ¸± ¶§¸¶´Ù 1¾¿ Áõ°¡ÇÑ´Ù.
Parameters
   mSection : ±â»çÀÇ ºÐ¾ß ¹øÈ£ ( 0 ¡Â mSection ¡Â 9 )
   mNewsId : ±â»çÀÇ °íÀ¯¹øÈ£ ( 1 ¡Â mNewsId ¡Â 50,000 )

void eraseNews(int mNewsId)
±â»çÀÇ °íÀ¯¹øÈ£°¡ mNewsId ÀÎ ±â»ç°¡ »èÁ¦µÈ´Ù.
Parameters
   mNewsId : ±â»çÀÇ °íÀ¯¹øÈ£ ( 1 ¡Â mNewsId ¡Â 50,000 )

void readNews(int mUserId, int mNewsId)
¾ÆÀÌµð mUserId ÀÎ »ç¿ëÀÚ°¡ mNewsId ÀÎ ±â»ç¸¦ ÀÐ´Â´Ù.
¾ÆÀÌµð mUserId ÀÎ »ç¿ëÀÚÀÇ °ü½É ºÐ¾ß°¡ mNewsId ±â»çÀÇ ºÐ¾ß·Î º¯°æµÈ´Ù.
Parameters
   mUserId : ±â»ç¸¦ ÀÐÀº »ç¿ëÀÚÀÇ ¾ÆÀÌµð ( 1 ¡Â mUserId ¡Â 100,000 )
   mNewsId : ±â»çÀÇ °íÀ¯¹øÈ£ ( 1 ¡Â mNewsId ¡Â 50,000 )

void changeSection(int mNewsId, int mSection)
±â»çÀÇ °íÀ¯¹øÈ£°¡ mNewsId ÀÎ ±â»ç°¡ mSection ºÐ¾ß·Î º¯°æµÈ´Ù.
mSectionÀº ±âÁ¸ ±â»çÀÇ ºÐ¾ß¿Í ´Ù¸¥ °ÍÀÌ º¸ÀåµÈ´Ù.
Parameters
   mNewsId : ±â»çÀÇ °íÀ¯¹øÈ£ ( 1 ¡Â mNewsId ¡Â 50,000 )
   mSection : ±â»çÀÇ ºÐ¾ß ¹øÈ£ ( 0 ¡Â mSection ¡Â 9 )

int getList(int mUserId, int mList[])
¾ÆÀÌµð mUserId ÀÇ »ç¿ëÀÚ¿¡°Ô º¸ÀÌ´Â ÃßÃµ ±â»çÀÇ ¹øÈ£¸¦ ¼øÀ§´ë·Î mList ¿¡ ÀúÀåÇÑ´Ù.
ÃÖ´ë 10°³ÀÇ ±â»ç¸¦ ÀúÀåÇÏ¸ç, ±â»çÀÇ ÃÑ °³¼ö°¡ 10°³ ¹Ì¸¸ÀÌ¶ó¸é ±â»çÀÇ °³¼ö¸¸Å­ ÀúÀåÇÑ´Ù.
mList¿¡ ´ã±ä ±â»çÀÇ °³¼ö¸¦ ¹ÝÈ¯ÇÑ´Ù.
Parameters
   mUserId : »ç¿ëÀÚÀÇ ¾ÆÀÌµð ( 1 ¡Â mUserId ¡Â 100,000 )
   mList : ÃßÃµ ±â»ç ¸®½ºÆ®¸¦ ÀúÀåÇÒ ¹è¿­
Returns
   mList¿¡ ´ã±ä ±â»çÀÇ °³¼ö


 [Á¦¾à»çÇ×]
1. °¢ Å×½ºÆ® ÄÉÀÌ½º ½ÃÀÛ ½Ã init() ÇÔ¼ö°¡ È£ÃâµÈ´Ù.
2. eraseNews(), readNews(), changeSection() ÇÔ¼öÀÇ mNewsId´Â Çö ½ÃÁ¡¿¡ Á¸ÀçÇÏ´Â ±â»çÀÇ ¹øÈ£ÀÓÀÌ º¸ÀåµÈ´Ù.
3. °¢ Å×½ºÆ® ÄÉÀÌ½º¿¡¼­ addNews() ÇÔ¼öÀÇ È£Ãâ È½¼ö´Â ÃÖ´ë 50,000 ¹øÀÌ´Ù.
4. °¢ Å×½ºÆ® ÄÉÀÌ½º¿¡¼­ eraseNews() ÇÔ¼öÀÇ È£Ãâ È½¼ö´Â ÃÖ´ë 5,000 ¹øÀÌ´Ù
5. °¢ Å×½ºÆ® ÄÉÀÌ½º¿¡¼­ readNews() ÇÔ¼öÀÇ È£Ãâ È½¼ö´Â ÃÖ´ë 50,000 ¹øÀÌ´Ù.
6. °¢ Å×½ºÆ® ÄÉÀÌ½º¿¡¼­ changeSection() ÇÔ¼öÀÇ È£Ãâ È½¼ö´Â ÃÖ´ë 5,000 ¹øÀÌ´Ù.
7. °¢ Å×½ºÆ® ÄÉÀÌ½º¿¡¼­ getList() ÇÔ¼öÀÇ È£Ãâ È½¼ö´Â ÃÖ´ë 50,000 ¹øÀÌ´Ù.

*/
void init()
{

}
void addNews(int mSection, int mNewsId)
{

}
void eraseNews(int mNewsId)
{

}
void readNews(int mUserId, int mNewsId)
{

}
void changeSection(int mNewsId, int mSection)
{

}
int getList(int mUserId, int mList[])
{
	return 0;
}
#elif 0
#include<time.h>
#include<stdio.h> 
#include<stdlib.h> 
#define MAX_N			5
#define MAX_NAME_LEN	7
#define MAX_TAG_LEN		4

//void mstrcpy(char dst[], const char src[]) {
//	int c = 0;
//	while ((dst[c] = src[c]) != '\0') ++c;
//}

//int mstrcmp(const char str1[], const char str2[]) {
//	int c = 0;
//	while (str1[c] != '\0' && str1[c] == str2[c]) ++c;
//	return str1[c] - str2[c];
//}

void init(int M)
{

}

int section[100];   // type ÀúÀå
int section_cnt;

int types[500];
int types_cnt;

void add(char mName[MAX_NAME_LEN])
{
	int c; unsigned long long hash = 0;
	while (c = *mName++) hash = (hash << 8) + c;


}

int moveType(char mType[MAX_TAG_LEN], int mFrom, int mTo)
{
	return 0;
}

void moveName(char mName[MAX_NAME_LEN], int mSection)
{

}

void deleteName(char mName[MAX_NAME_LEN])
{

}

int countBook(int mTypeNum, char mTypes[MAX_N][MAX_TAG_LEN], int mSection)
{
	return 0;
}

int main() {
	char mName[7];
	srand((unsigned int)time(NULL));
	for (int i = 0; i < 5; i++) {
		mName[i] = rand() % 52 + 'a';
		add(mName);
	}


}




#elif 0
int n[11][50001];
int section[50001];
int ncnt;
int delcnt;
int del[50001];

struct USER
{
	int userid;
	int prefer;
}u[100001];

void init()
{
	ncnt = 0;
	delcnt = 0;
	for (int i = 0; i <= 100000; i++)
	{
		u[i].prefer = 10;
	}
	for (int i = 0; i <= 10; i++)
	{
		n[i][0] = 0;
	}
}

void addNews(int mSection, int mNewsId)
{
	ncnt++;
	for (int i = 0; i <= 10; i++)
	{
		n[i][mNewsId] = 0;
	}
	n[mSection][mNewsId] = 10;
	section[mNewsId] = mSection;
	del[mNewsId] = 0;
}

void eraseNews(int mNewsId)
{
	delcnt++;
	del[mNewsId] = 1;
}

void readNews(int mUserId, int mNewsId)
{
	u[mUserId].prefer = section[mNewsId];
	for (int i = 0; i <= 10; i++)
	{
		n[i][mNewsId]++;
	}
}

void changeSection(int mNewsId, int mSection)
{
	n[section[mNewsId]][mNewsId] -= 10;
	section[mNewsId] = mSection;
	n[mSection][mNewsId] += 10;
}

int getList(int mUserId, int mList[])
{
	int* my_n;
	for (int i = 0; i < 10; i++)
		mList[i] = 0;
	my_n = n[u[mUserId].prefer];

	int end = 10;

	if (end > ncnt - delcnt)
		end = ncnt - delcnt;

	for (int i = 1; i <= ncnt; i++)
	{
		if (del[i])continue;
		if (my_n[mList[9]] > my_n[i])continue;

		int j;
		for (j = 9; j > 0; j--)
		{
			if (my_n[mList[j - 1]] > my_n[i])break;
			mList[j] = mList[j - 1];
		}
		mList[j] = i;
	}
	return end;
}

#elif 0
#define MAX_N 2005
#define rint register int

struct _cell {
	int r1, c1, r2, c2;
	int size, del;
}CELL[MAX_N], backup[MAX_N];

int cellCnt;
int backupCnt;
int check;
int row, col;
int visit[MAX_N];
void init(int R, int C)
{
	row = R, col = C;
	cellCnt = 0;
	backupCnt = -1;
	check = 0;
	for (rint i = 0; i < MAX_N; i++) {
		visit[i] = 0;
	}
}

void copy(int r1, int c1, int r2, int c2, int rect[]) {
	rect[0] = r1, rect[1] = c1, rect[2] = r2, rect[3] = c2;
}
void copy(int i, int rect[]) {
	rect[0] = CELL[i].r1, rect[1] = CELL[i].c1, rect[2] = CELL[i].r2, rect[3] = CELL[i].c2;
}

void getRect(int r, int c, int rect[])
{
	for (rint i = 0; i < cellCnt; i++) {
		if (CELL[i].del) continue;
		int r1 = CELL[i].r1;
		int c1 = CELL[i].c1;
		int r2 = CELL[i].r2;
		int c2 = CELL[i].c2;

		if (r >= r1 && r <= r2 && c >= c1 && c <= c2) {
			copy(i, rect);
			return;
		}
	}
	copy(r, c, r, c, rect);
}
void backUp() {
	backupCnt = 0;
	for (rint i = 0; i < cellCnt; i++) {
		if (CELL[i].del) continue;
		backup[backupCnt++] = CELL[i];
	}
}

int mergeCells(int cnt, int rs[], int cs[], int rect[])
{
	for (rint k = 0; k < cnt; k++) {
		int r = rs[k], c = cs[k];
		for (rint i = k + 1; i < cnt; i++) {
			if (r == rs[i] && c == cs[i])
				return 0;
		}
	}

	check++;
	int select[15];
	int selectCnt = 0;
	int startR = row + 1, startC = col + 1, endR = 0, endC = 0;
	int sumSize = 0;
	for (rint k = 0; k < cnt; k++) {
		int r = rs[k], c = cs[k];
		int found = 0;
		for (rint i = 0; i < cellCnt; i++) {
			if (CELL[i].del) continue;
			int r1 = CELL[i].r1;
			int c1 = CELL[i].c1;
			int r2 = CELL[i].r2;
			int c2 = CELL[i].c2;

			if (r >= r1 && r <= r2 && c >= c1 && c <= c2) {
				if (visit[i] == check) {
					return 0;
				}
				else {
					visit[i] = check;
				}
				select[selectCnt++] = i;
				sumSize += CELL[i].size;
				if (startR > r1) startR = r1;
				if (endR < r2) endR = r2;
				if (startC > c1) startC = c1;
				if (endC < c2) endC = c2;
				found = 1;
				break;
			}
		}
		if (found == 1) continue;
		sumSize++;
		if (startR > r) startR = r;
		if (endR < r) endR = r;
		if (startC > c) startC = c;
		if (endC < c) endC = c;
	}
	int s = (endR - startR + 1) * (endC - startC + 1);
	if (s != sumSize) return 0;
	backUp();
	for (rint i = 0; i < selectCnt; i++) {
		CELL[select[i]].del = 1;
	}
	CELL[cellCnt++] = { startR, startC, endR, endC, sumSize, 0 };
	copy(startR, startC, endR, endC, rect);
	return 1;
}

int splitCell(int r, int c, int rect[])
{
	for (rint i = 0; i < cellCnt; i++) {
		if (CELL[i].del) continue;
		int r1 = CELL[i].r1;
		int c1 = CELL[i].c1;
		int r2 = CELL[i].r2;
		int c2 = CELL[i].c2;

		if (r >= r1 && r <= r2 && c >= c1 && c <= c2) {
			backUp();
			CELL[i].del = 1;
			copy(i, rect);
			return 1;
		}
	}
	return 0;
}

void undo()
{
	if (backupCnt == -1) return;
	_cell tmp[MAX_N];
	int tmpCnt = 0;
	for (rint i = 0; i < cellCnt; i++) {
		if (CELL[i].del) continue;
		tmp[tmpCnt++] = CELL[i];
	}
	for (rint i = 0; i < backupCnt; i++) {
		CELL[i] = backup[i];
	}
	cellCnt = backupCnt;
	for (rint i = 0; i < tmpCnt; i++) {
		backup[i] = tmp[i];
	}
	backupCnt = tmpCnt;
}

int checkRectangle(int r1, int c1, int r2, int c2)
{
	for (rint i = 0; i < cellCnt; i++) {
		if (CELL[i].del) continue;
		int sr = CELL[i].r1;
		int sc = CELL[i].c1;
		int er = CELL[i].r2;
		int ec = CELL[i].c2;

		if (er < r1 || sr > r2 || ec < c1 || sc > c2) {
			continue;
		}
		if (er <= r2 && sr >= r1 && ec <= c2 && sc >= c1) {
			continue;
		}
		return 1;
	}
	return 0;
}


#elif 0




#define MAX_N           5
#define MAX_NAME_LEN    7
#define MAX_TAG_LEN     4

#define MAX_TYPE 504
#define MAX_TYPE_TABLE 2011

#define MAX_BOOK 50000
#define MAX_BOOK_TABLE 100019
#define NULL 0

struct Book {
	long long value;
	Book* next;
	int section;
	int count_book_call;
};

int count_book_call = 0;
int book_heap_size = 0;
Book books[MAX_BOOK];
Book* book_table[MAX_BOOK_TABLE];

struct Type {
	int value;
	Type* next;
	int id;
};

int type_heap_size = 1;
Type types[MAX_TYPE];
Type* type_table[MAX_TYPE_TABLE];
int book_counter[MAX_TYPE];

Book* type_books[MAX_TYPE][MAX_BOOK];

__inline long long str2ll(char* s) {
	long long value = 0;
	while (*s) value = (value << 8) + *s++;
	return value;
}

__inline int get_type_id(int value) {
	int hash = value % MAX_TYPE_TABLE;
	register Type* type = type_table[hash];

	while (type && type->value != value) type = type->next;

	if (!type) {
		type = &types[type_heap_size++];
		*type = { value, type_table[hash], type_heap_size - 1 };
		type_table[hash] = type;
	}

	return type->id;
}

__inline int find_type_id(long long value) {
	register Type* type = type_table[value % MAX_TYPE_TABLE];
	while (type && type->value != value) type = type->next;
	if (type) return type->id;
	return 0;
}

__inline Book* find_book(long long value) {
	register Book* book = book_table[value % MAX_BOOK_TABLE];
	while (book && book->value != value) book = book->next;
	return book;
}

void init(int M) {
	count_book_call = 0;
	book_heap_size = 0;
	type_heap_size = 1;

	register int index = MAX_BOOK_TABLE;
	while (--index != -1) book_table[index] = NULL;

	index = MAX_TYPE_TABLE;
	while (--index != -1) type_table[index] = NULL;

	index = MAX_TYPE;
	while (--index != -1) book_counter[index] = 0;
}

void add(char mName[MAX_NAME_LEN], register int mTypeNum, char mTypes[MAX_N][MAX_TAG_LEN], int mSection) {
	long long value = str2ll(mName);
	int hash = value % MAX_BOOK_TABLE;

	Book* book = &books[book_heap_size++];

	*book = { value, book_table[hash], mSection, 0 };
	book_table[hash] = book;

	register int type_id;
	while (--mTypeNum != -1) {
		type_id = get_type_id(str2ll(mTypes[mTypeNum]));
		type_books[type_id][book_counter[type_id]++] = book;
	}
}

int moveType(char mType[MAX_TAG_LEN], register int mFrom, register int mTo) {
	int type_id = find_type_id(str2ll(mType));

	register int counter = book_counter[type_id];
	register Book** book = &type_books[type_id][0];
	register int count = 0;
	while (--counter != -1) {
		if ((*book)->section == mFrom) {
			(*book)->section = mTo;
			count++;
		}
		book++;
	}

	return count;
}

void moveName(char mName[MAX_NAME_LEN], int mSection) {
	find_book(str2ll(mName))->section = mSection;
}

void deleteName(char mName[MAX_NAME_LEN]) {
	find_book(str2ll(mName))->section = 0;
}

int countBook(register int mTypeNum, char mTypes[MAX_N][MAX_TAG_LEN], int mSection) {
	count_book_call++;

	int type_id;

	register Book** book;

	register int count = 0;
	register int counter;
	while (--mTypeNum != -1) {
		type_id = find_type_id(str2ll(mTypes[mTypeNum]));
		book = &type_books[type_id][0];
		counter = book_counter[type_id];
		while (--counter != -1) {
			if ((*book)->section == mSection && (*book)->count_book_call != count_book_call) {
				(*book)->count_book_call = count_book_call;
				count++;
			}
			book++;
		}
	}
	return count;
}
#elif 0
#define MAX_N			5
#define MAX_NAME_LEN	7
#define MAX_TAG_LEN		4

struct BOOK {
	//int id;
	char name[10];
	//int nkey;
	int type[6];
	int tcnt;
	BOOK* next;
};

BOOK bk[50000]; int bid;

BOOK* SectionType[100][500]; int SectionTypecnt[100];
BOOK* SectionBook[100]; int SectionBook



cnt[100];

int stcnt;
int sn[100][50000]; int sncnt;

BOOK* myalloc() {
	return &bk[bid++];
}
void mstrcpy(char dst[], const char src[]) {
	int c = 0;
	while ((dst[c] = src[c]) != '\0') ++c;
}

int mstrcmp(const char str1[], const char str2[]) { int c = 0;	while (str1[c] != '\0' && str1[c] == str2[c]) ++c;	return str1[c] - str2[c]; }
void init(int M) {
	bid = stcnt = sncnt = 0;
}
void add(char mName[MAX_NAME_LEN], int mTypeNum, char mTypes[MAX_N][MAX_TAG_LEN], int mSection) {
	BOOK* pNew = myalloc();
	mstrcpy(pNew[bid].name, mName);

	for (int i = 0; i < mTypeNum; i++) {
		int ht = 0, char* str = mTypes[i];
		while (*str++) {
			ht = (ht << 8) + *str - 'a';
		}
		pNew[bid].type[i] = ht;
		//st[mSection][stcnt++] = ht;
		SectionType[mSection]
	}
	bk[bcnt].tcnt = mTypeNum;
	sn[mSection][bcnt] = bcnt;
}
int moveType(char mType[MAX_TAG_LEN], int mFrom, int mTo) {

	return 0;
}

void moveName(char mName[MAX_NAME_LEN], int mSection)
{

}

void deleteName(char mName[MAX_NAME_LEN])
{

}

int countBook(int mTypeNum, char mTypes[MAX_N][MAX_TAG_LEN], int mSection)
{
	return 0;
}
#elif 0
#include<stdio.h>
#define NULL 0


struct Trie {
	bool terminal;
	Trie* children[26];

	Trie() : terminal(false) {
		for (int i = 0; i < 26; i++) children[i] = 0;
	}
	void insert(const char* key) {
		if (*key == '\0') {
			terminal = 1; return;
		}
		int nextkey = *key - 'a';
		if (!children[nextkey])
			children[nextkey] = malloc(Trie);
	}
};


#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <algorithm>

using namespace std;

class Node
{
public:
	Node* childs[26] = { NULL };
	bool isWord = false;

	Node() {}
};

class Trie
{
public:
	Node* root;

	Trie() { this->root = new Node(); }

	void insert(const char* str)
	{
		Node* cur = this->root;

		for (int i = 0; i < strlen(str); i++)
		{

			if (cur->childs[str[i] - 'a'] != NULL) cur = cur->childs[str[i] - 'a'];
			else
			{
				cur->childs[str[i] - 'a'] = new Node();
				cur = cur->childs[str[i] - 'a'];
			}
		}

		cur->isWord = true;
	}

	bool find(const char* str)
	{
		Node* cur = this->root;

		for (int i = 0; i < strlen(str); i++)
		{
			if (cur->childs[str[i] - 'a'] == NULL) return 0;
			else cur = cur->childs[str[i] - 'a'];
		}

		if (cur->isWord) return 1;
		else return 0;
	}
};

int main(void)
{
	Trie trie;

	trie.insert("tree");

	cout << trie.find("tree") << '\n';
	cout << trie.find("tre") << '\n';

	return 0;
}





Trie* malloc()

//ÃâÃ³: https://www.crocus.co.kr/1053 [Crocus]
#elif 0
char PL[10000][1000];

struct Node {
	int rn;
	Node* next;
} NODE[10000];

int PLn, LLn;

void insert(int l, char a[]) {
	strcpy(PL[PLn++], a);

	for (int i = l; i < LLn; i++) {
		LL[i].rn++;
		LL[i].pl = PLn;
	}

	LL[l].pl = PLn;
}
void delete(int l, char a[]) {
	for (int i = l; i < LLn; i++) LL[i].rn++;
	LL[l].pl = PLn;
}


#elif 0
struct PT {
	int t, l, uid;
} P[100005];
int FL[1005][1005], pcnt, tc;
bool operator < (PT a, PT b) {
	return a.l == b.l ? a.t < b.t : a.l < b.l;
}
void init(int N) {
	++tc; pcnt = 0;
	for (int i = 1; i <= N; i++)    FL[i][i] = tc;
}
void follow(int u1, int u2, int ts) {
	FL[u1][u2] = tc;
}
void makePost(int uID, int pID, int ts) {
	P[++pcnt] = { ts, 0, uID };
}
void like(int pID, int ts) {
	P[pID].l++;
}
void getFeed(int uID, int ts, int pL[]) {
	int a[11], k = 0;
	for (int i = pcnt; i; i--) {
		if (FL[uID][P[i].uid] != tc)
			continue;
		if (P[i].t + 1000 >= ts) {
			int j;
			for (j = k; j && P[a[j - 1]] < P[i]; j--)
				a[j] = a[j - 1];
			a[j] = i; k += k < 10;
		}
		else if (k >= 10) goto exit;
		else a[k++] = i;
	}
exit:
	for (int ii = 0; ii < 10; ii++)    pL[ii] = a[ii];
}


#elif 0
#define MAXUSER 1001
#define MAX 100005

struct Post { int uid, like, time; } p[MAX];
int pLen, i, tc = 0, fol[MAXUSER][MAXUSER] = { 0 };

void init(int N)
{
	tc++;
	pLen = 0;
	for (i = 0; i < MAXUSER; i++) fol[i][i] = tc;
}

void follow(int uID1, int uID2, int timestamp)
{
	fol[uID1][uID2] = tc;
}

void makePost(int uID, int pID, int timestamp)
{
	p[++pLen] = { uID, 0, timestamp };
}

void like(int pID, int timestamp)
{
	p[pID].like++;
}

bool operator < (Post a, Post b) {
	return  a.like == b.like ? a.time > b.time : a.like > b.like;
}

int* res; int resLen;
void insertSort(int j) {
	for (i = resLen; i && p[j] < p[res[i - 1]]; i--) res[i] = res[i - 1];
	res[i] = j;
	resLen += resLen < 10;
}

void getFeed(int uID, int timestamp, int pIDList[])
{
	res = pIDList;
	resLen = 0;
	for (int j = pLen; j; j--) {
		if (fol[uID][p[j].uid] != tc) continue;
		if (p[j].time + 1000 >= timestamp) insertSort(j);
		else if (resLen >= 10) return;
		else res[resLen++] = j;
	}
}

#elif 0
char F[1010][1010];

struct str {
	int uID, ts, like;
} B[100010];
int L;

void init(int N) {
	for (int i = 1; i <= N; i++) for (int j = 1; j <= N; j++) F[i][j] = (i == j);
	L = 0;
}

void follow(int uID1, int uID2, int timestamp) {
	F[uID1][uID2] = 1;
}

void makePost(int uID, int pID, int timestamp) {
	++L;
	B[L].uID = uID;
	B[L].ts = timestamp;
	B[L].like = 0;
}

void like(int pID, int timestamp) {
	B[pID].like += 1;
}

void getFeed(int uID, int timestamp, int pIDList[]) {
	int tp = 0, res[11];
	for (int i = 0; i < 11; i++) res[i] = -1;
	for (int i = L; i; i--) {
		if (!F[uID][B[i].uID]) continue;
		int dt = timestamp - B[i].ts;
		if (dt > 1000) {
			if (tp == 10) break;
			res[tp++] = i;
		}
		else {
			res[tp] = i;
			for (int a = tp; a; a--) {
				if (B[res[a]].like > B[res[a - 1]].like) {
					int t = res[a]; res[a] = res[a - 1]; res[a - 1] = t;
				}
				else break;
			}
			if (tp < 10) ++tp;
		}
	}
	for (int i = 0; i < tp; i++) pIDList[i] = res[i];
}



#elif 0
#define MSG_N_MAX 	1000005
#define HSIZE 	    0x7A7B 
#define UMAX 	    1005

unsigned long v;
int FL[UMAX][UMAX], fcnt[UMAX];

struct MSG {
	int uID;    int time;
	bool operator < (MSG m) { return time < m.time ? 1 : 0; }
} Msg[MSG_N_MAX];
struct M {
	char key[10];
	int mcnt;
	int m[6];
	void add(int mID) {
		int i;
		for (i = mcnt; i && Msg[m[i - 1]] < Msg[mID]; i--)   m[i] = m[i - 1];
		m[i] = mID;
		mcnt += mcnt < 5;
	}
} Hm[HSIZE], Um[UMAX], ret;

int mstrcmp(const char* a, const char* b) {
	int i;
	for (i = 0; a[i] != '\0'; ++i) if (a[i] != b[i]) return a[i] - b[i];
	return a[i] - b[i];
}
void mstrcpy(char* dest, const char* src) {
	int i = 0;
	while (src[i] != '\0') { dest[i] = src[i]; i++; }
	dest[i] = src[i];
}

void init() {
	for (int i = 0; i < HSIZE; i++)     Hm[i].mcnt = 0;
	for (int i = 0; i < UMAX; i++)      Um[i].mcnt = 0, FL[i][0] = i, fcnt[i] = 1;
}

char a[10];
void createMessage(int mID, int uID, char s[]) {
	int time = ((s[0] - '0') * 100000 + (s[1] - '0') * 10000 + (s[3] - '0') * 1000 + (s[4] - '0') * 100 + (s[6] - '0') * 10 + (s[7] - '0')) * 1000 + 1000 - uID;
	Msg[mID] = { uID, time };
	Um[uID].add(mID);

	s += 8;
	while (*s++)
	{
		int i;
		for (i = 0, v = 0; (a[i] = *++s) > ' '; i++, v = (((v << 8)) + *s) % HSIZE);
		a[i] = '\0';
		int sk = 1;
		while (Hm[v].key[0] != 0 && (sk = (mstrcmp(Hm[v].key, a) != 0)))
			v = (v + 1) % HSIZE;

		if (sk == 1) mstrcpy(Hm[v].key, a);
		Hm[v].add(mID);
	}
}
void followUser(int userID1, int userID2) {
	FL[userID1][fcnt[userID1]++] = userID2;
}
int searchByHashtag(char s[], int retIDs[]) {
	int i;
	for (i = 0, v = 0; (a[i] = *++s) > ' '; i++, v = (((v << 8)) + *s) % HSIZE);
	a[i] = '\0';
	while (Hm[v].key[0] != 0 && mstrcmp(Hm[v].key, a) != 0)
		v = (v + 1) % HSIZE;

	for (int i = 0; i < Hm[v].mcnt; i++)	retIDs[i] = Hm[v].m[i];
	return Hm[v].mcnt;
}
int getMessages(int userID, int retIDs[]) {
	ret.mcnt = 0;
	for (int i = 0; i < fcnt[userID]; i++)
		for (int uID = FL[userID][i], j = 0; j < Um[uID].mcnt; j++)
			ret.add(Um[uID].m[j]);
	for (int i = 0; i < ret.mcnt; i++)	retIDs[i] = ret.m[i];
	return ret.mcnt;
}


#elif 0
#include<stdio.h>
#define MAX_USER      1000
#define MAX_TAG       5000

// uID´ç follow uID
int F[MAX_USER][MAX_USER];
int fcnt[MAX_USER];

// uID´ç °¡Áø mID
//int USER_MSG[MAX_USER][50000];
int USER_MSG[MAX_USER][6];
int mcnt[100];
// ht´ç °¡Áø mID
//int HT_MSG[MAX_TAG][50000];
int HT_MSG[MAX_TAG][6];
int hmcnt[MAX_TAG];

// mID´ç Tm
int MSG_TM[50000];
#define M (1024*1024)


void init() {
	pintf("%d \n", sizeof(F) / M);
	pintf("%d \n", sizeof(fcnt) / M);
	pintf("%d \n", sizeof(USER_MSG) / M);
	pintf("%d \n", sizeof(mcnt) / M);
	pintf("%d \n", sizeof(HT_MSG) / M);
	pintf("%d \n", sizeof(hmcnt) / M);
	pintf("%d \n", sizeof(MSG_TM) / M);


	for (int i = 0; i < 1000; i++) fcnt[i] = 0, mcnt[i] = 0;
	for (int i = 0; i < 5000; i++) hmcnt[i] = 0;
}

void add(int mID)
{




	if (MSG_TM[retIDs[4]] > MSG_TM[mID]) return;
	else MSG_TM[retIDs[4]] = mID;

	int temp = MSG_TM[mID];

	int j = 3;
	while (temp > MSG_TM[retIDs[j]] && j >= 0) {
		retIDs[j + 1] = retIDs[j];
		j = j - 1;
	}
	MSG_TM[j + 1] = mID;


	HT_MSG[ht][hmcnt[ht]++] = msgID;
	MSG_TM[ht] = time;



}


void createMessage(int msgID, int userID, char msgData[])
{
	USER_MSG[userID][mcnt[userID]++] = msgID;

	int time = ((msgData[0] - '0') * 100000 +
		(msgData[1] - '0') * 10000 +
		(msgData[3] - '0') * 1000 +
		(msgData[4] - '0') * 100 +
		(msgData[6] - '0') * 10 +
		msgData[7] - '0') * 1000 + 1000 - userID;

	char* str = &msgData[9];

	unsigned long long key = 0, ht = 0;
	while (*str != '\0') {
		if (*str == '#') {
			key = 0, ht = 0;
			str++;
			//continue;
		}
		else if (*str == ' ' || *str == '\0') {
			ht = key % 5000;
			while (hmcnt[ht] != 0) {
				ht = (ht + 1) % 5000;
			}


			HT_MSG[ht][hmcnt[ht]++] = msgID;
			MSG_TM[ht] = time;



		}
		key = (key << 8) + *str;
		str++;
	}
	while (0);
}
void followUser(int userID1, int userID2) {
	F[userID1][fcnt[userID1]++] = userID2;
}
void Top5(int mID, int retIDs[])
{
	if (MSG_TM[retIDs[4]] > MSG_TM[mID]) return;
	else MSG_TM[retIDs[4]] = mID;

	int temp = MSG_TM[mID];

	int j = 3;
	while (temp > MSG_TM[retIDs[j]] && j >= 0) {
		retIDs[j + 1] = retIDs[j];
		j = j - 1;
	}
	MSG_TM[j + 1] = mID;
}

int searchByHashtag(char tagName[], int retIDs[])
{
	for (int i = 0; i < 5; i++) retIDs[i] = 0;

	unsigned long long key = 0, ht = 0;
	tagName++;
	while (*tagName != '\0') {
		key = (key << 8) + *tagName;
		tagName++;
	}
	ht = key % 5000;
	while (hmcnt[ht] != 0) {
		ht = (ht + 1) % 5000;
	}

	for (int i = 0; i < hmcnt[ht]; i++) {
		int mID = HT_MSG[ht][i];
		for (int j = 0; j < hmcnt[ht]; j++) {
			int mID = HT_MSG[ht][j];
			Top5(mID, retIDs);
		}
	}
	for (int i = 0; i < 5; i++)
		if (retIDs[i] == 0) return i;
	return 0;
}

int getMessages(int userID, int retIDs[])
{
	for (int i = 0; i < 5; i++) retIDs[i] = 0;

	for (int i = 0; i < fcnt[userID]; i++) {
		int uID = F[userID][i];
		for (int j = 0; j < mcnt[userID]; j++) {
			int mID = USER_MSG[uID][j];
			Top5(mID, retIDs);
		}
	}

	for (int i = 0; i < 5; i++)
		if (retIDs[i] == 0) return i;

	return 0;
}
#elif 0







#define int register int
int FL[1000][1000];
int Fcnt[1000];
int USER_MSG[1000][50000];
int Mcnt[1000];
int MSG_TM[1000000];

void init()
{
}

void createMessage(int msgID, int userID, register char msgData[])
{
	FL[userID][Fcnt[userID]++] = userID;
	USER_MSG[userID][Mcnt[userID]++] = msgID;

	unsigned long long time = ((msgData[0] - '0') * 1e5 + (msgData[1] - '0') * 10000 + (msgData[3] - '0') * 1000 + (msgData[4] - '0') * 100 + (msgData[5] - '0') * 10 + (msgData[6] - '0') * 1) * 1000 - userID;
	MSG_TM[msgID] = time;

}

void followUser(int userID1, int userID2)
{
	FL[userID1][Fcnt[userID1]++] = userID2;
}

int searchByHashtag(char tagName[], int retIDs[])
{
}

int getMessages(int userID, int retIDs[])
{
	for (int i = 0; Fcnt[userID]; i++) {
		int uID = FL[userID][i];
		for (int j = 0; Mcnt[uID]; j++) {
			int mID = USER_MSG[i][j];
			SS(mID);
		}
	}

}




struct User {


};


#elif 0

#include <iostream>
using namespace std;

struct NODE {
	int v;
	NODE* next;
}a[2000];
int arr_idx = 0;

NODE* malloc() { return &a[arr_idx++]; }
NODE* pHead;

void pint_node() {
	for (NODE* iter = pHead->next; iter != NULL; iter = iter->next) {
		cout << iter->v << " ";
	}
	cout << endl;
}
void main() {
	arr_idx = 0;
	pHead = malloc();
	pHead->next = NULL;
	NODE* p;
	for (int i = 0; i < 10; i++) {
		p = malloc();
		p->v = i;
		p->next = pHead->next;
		pHead->next = p;
	}
	pint_node();

	NODE* prev = pHead;
	for (NODE* iter = pHead->next; iter != NULL; iter = iter->next) {
		if (iter->v == 5) {
			prev->next = iter->next;
		}
		prev = iter;
	}
	pint_node();


}


#elif 0
#include<iostream>
using namespace std;

int arr_idx = 0;

struct NODE {
	int v;
	NODE* prev;
	NODE* next;
}a[20000];


NODE* malloc(void) {
	return &a[arr_idx++];
}


NODE* pTail;
NODE* pHead;

void pint_node(void);

void main() {
	arr_idx = 0;;

	pHead = malloc();
	pTail = malloc();

	pTail->prev = pHead;
	pHead->next = pTail;

	NODE* p;

	for (int i = 0; i < 10; i++) {
		p = malloc();
		p->v = i;

		p->prev = pTail->prev;
		pTail->prev = p;

		p->next = p->prev->next;
		p->prev->next = p;
	}
	pint_node();


	// Remove form head
	for (NODE* iter = pHead->next; iter != pTail; iter = iter->next) {
		if (iter->v == 5) {
			iter->prev->next = iter->next;
			iter->next->prev = iter->prev;
			cout << "Delete node 5 ..." << iter->v << endl << endl;
			break;
		}
	}
	pint_node();


	for (NODE* iter = pTail->prev; iter != pHead; iter = iter->prev) {
		if (iter->v == 3) {
			iter->prev->next = iter->next;
			iter->next->prev = iter->prev;
			cout << "Delete node 3 ..." << iter->v << endl << endl;
			break;
		}
	}
	pint_node();

	for (NODE* iter = pTail->prev; iter != pHead; iter = iter->prev) {
		iter->prev->next = iter->next;
		iter->next->prev = iter->prev;
		cout << "Delete all node ..." << iter->v << endl;
	}
	cout << endl;

	for (int i = 0; i < 10; i++) {
		p = malloc();
		p->v = i;

		p->prev = pTail->prev;
		pTail->prev = p;

		p->next = p->prev->next;
		p->prev->next = p;
	}
	pint_node();
}


void pint_node(void) {
	cout << "from head ~~" << endl;
	for (NODE* iter = pHead->next; iter != pTail; iter = iter->next) {
		cout << iter->v << " ";
	}
	cout << endl;

	cout << "from tail ~~" << endl;
	for (NODE* iter = pTail->prev; iter != pHead; iter = iter->prev) {
		cout << iter->v << " ";
	}
	cout << endl;
}















#elif 0



#include<iostream>
using namespace std;

int arr_idx = 0;

struct NODE {
	int v;
	NODE* prev;
	NODE* next;
}a[20000];


NODE* malloc(void) {
	return &a[arr_idx++];
}


NODE* pTail;
NODE* pHead;

void pint_node(void);
void init() {}
void createMessage(int msgID, int userID, char msgData[]) {}
void followUser(int userID1, int userID2) {}
int searchByHashtag(char tagName[], int retIDs[]) { return 0; }
int getMessages(int userID, int retIDs[]) { return 0; }


void doublelinked() {
	arr_idx = 0;;

	pHead = malloc();
	pTail = malloc();

	pTail->prev = pHead;
	pHead->next = pTail;

	NODE* p;

	for (int i = 0; i < 10; i++) {
		p = malloc();
		p->v = i;

		p->prev = pTail->prev;
		pTail->prev = p;

		p->next = p->prev->next;
		p->prev->next = p;
	}
	pint_node();


	// Remove form head
	for (NODE* iter = pHead->next; iter != pTail; iter = iter->next) {
		if (iter->v = 5) {
			iter->prev->next = iter->next;
			iter->next->prev = iter->prev;
		}
		cout << "Delete node 5 ..." << iter->v << endl << endl;
	}
	pint_node();

	for (NODE* iter = pTail->prev; iter != pHead; iter = iter->prev) {
		if (iter->v = 3) {
			iter->prev->next = iter->next;
			iter->next->prev = iter->prev;
		}
		cout << "Delete node 3 ..." << iter->v << endl << endl;
	}
	pint_node();

	for (NODE* iter = pTail->prev; iter != pHead; iter = iter->prev) {
		iter->prev->next = iter->next;
		iter->next->prev = iter->prev;

		cout << "Delete all node ..." << iter->v << endl << endl;
	}

	for (int i = 0; i < 10; i++) {
		p = malloc();
		p->v = i;

		p->prev = pTail->prev;
		pTail->prev = p;

		p->next = p->prev->next;
		p->prev->next = p;
	}
	pint_node();
}


void pint_node(void) {
	cout << "from head ~~" << endl;
	for (NODE* iter = pHead->next; iter != pTail; iter->next) {
		cout << iter->v << " ";
	}
	cout << endl;

	cout << "from tail ~~" << endl;
	for (NODE* iter = pTail->prev; iter != pHead; iter->prev) {
		cout << iter->v << " ";
	}
	cout << endl;
}


#elif 0
#include <stdio.h>

struct Msg {
	int mTM;
};

struct User {
	Msg* msg;
} USER[1000];


typedef struct node {
	int data;
	struct node* next;
} Node;

typedef struct list {
	Node* head;
	Node* tail;
	int size;
} List;

void createlist(List* list) {

	list->head = (Node*)malloc(sizeof(Node));
	list->tail = (Node*)malloc(sizeof(Node));
	list->head->next = list->tail;
	list->tail->next = list->tail;
	list->size = 0;
}
void addFirst(List* list, int data) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = list->head->next;
	list->head->next = newNode;
	list->size++;
}
void addLast(List* list, int data) {
	Node* last = list->head;

	while (last->next != list->tail)
		last = last->next;

	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = list->tail;
	last->next = newNode;
	list->size++;
}

Node* searchNode(List* list, int data) {
	Node* node = list->head->next;
	while (node != list->tail) {
		if (node->data == data)
			return node;
		node = node->next;
	}
	pintf("µ¥ÀÌÅÍ¸¦ Ã£Áö ¸øÇß½À´Ï´Ù.\n");

	return NULL;
}

void removeNode(List* list, int data) {
	Node* node = list->head;
	while (node->next != list->tail) {
		if (node->next->data == data) {
			Node* delNode = node->next;
			node->next = delNode->next;
			free(delNode);
			list->size--;
			return;
		}
		node = node->next;
	}
	pintf("µ¥ÀÌÅÍ¸¦ Ã£Áö ¸øÇß½À´Ï´Ù.\n");
}



void pintList(List* list) {
	Node* node = list->head->next;
	int i = 1;
	while (node != list->tail) {
		pintf("%d ¹øÂ° ³ëµå µ¥ÀÌÅÍ :%d\n", i++, node->data);
		node = node->next;
	}
}
void distroyList(List* list) {
	Node* node = list->head;
	while (node != list->tail) {
		Node* delNode = node;
		node = delNode->next;
		free(delNode);
	}
	free(list->head);
	free(list->tail);
}

int main() {

	int i;
	List list;
	createlist(&list);

	for (i = 1; i <= 5; i++)
		addLast(&list, i);
	for (i = 11; i <= 15; i++)
		addFirst(&list, i);
	removeNode(&list, 11);
	removeNode(&list, 15);
	removeNode(&list, 5);
	removeNode(&list, 4);
	removeNode(&list, 50);

	Node* node = searchNode(&list, 14);
	pintf("search :%d\n", node->data);

	node = searchNode(&list, 12);
	pintf("search :%d\n", node->data);

	node = searchNode(&list, 3);
	pintf("search :%d\n", node->data);

	pintList(&list);
	return 0;
}


#elif 0
#define SHRT_MAX      32767
#define USHRT_MAX     0xffff
#define INT_MIN     (-2147483647 - 1)
#define INT_MAX       2147483647
#define UINT_MAX      0xffffffff
#define LONG_MIN    (-2147483647L - 1)
#define LONG_MAX      2147483647L
#define ULONG_MAX     0xffffffffUL
#define LLONG_MAX     9223372036854775807i64
#define LLONG_MIN   (-9223372036854775807i64 - 1)
#define ULLONG_MAX    0xffffffffffffffffui64
#define MAX_USER	1000
#define MAX_MSGS	50000
#define MAX_MID		1000000
#define MAX_TAG     5000
#define MAX_P_ID	10
#define MAX_KEY		10 
#define MAX_DATA	128
#define MAX_TABLE	5000

/*
struct _Msg {
	int uID;
	int mTM;
}MSG[MAX_MID];
*/
int MSG_Srtd[MAX_MID];


struct mmm {
	int aa;
	struct mmm* bbb;
}mmm[1000000];

//int MAP[1000][1000000];

int F[MAX_USER][MAX_USER];  // F[ÀÚ±âÀÚ½Å][FollowÇÏ´Â uID]
int FC[MAX_USER];   //FC[±× ÀÚ½Å] FollowÇÏ´Â uID°¹¼ö
int F_Srtd[MAX_USER][6];

/*
struct {
	int fcnt;
	int fID[MAX_USER];
}F[MAX_USER];
*/
struct {
	int key;
	int mcnt;
	int mID[6];
}HT[MAX_TABLE];

int mstrcmp(const char* a, const char* b);
void mstrcpy(char* dest, const char* src);
unsigned long hash(const char* str);

void init() {
	for (int i = 0; i < MAX_USER; i++) {
		FC[i] = 0;
		for (int j = 0; j < MAX_USER; j++)
			F[i][j] = (i == j);
	}
}

void add_msg(int h, int mID, int userID) {


}


void createMessage(int mID, int userID, char msgData[]) {
	int k = 0; char a[20];
	int time = (msgData[0] - '0') * 1e5 + (msgData[1] - '0') * 1e4 + (msgData[3] - '0') * 1e3 + (msgData[4] - '0') * 1e2 + (msgData[6] - '0') * 10 + (msgData[7] - '0') * 1;
	time = time * 1e3 + userID;

	MSG_Srtd[mID] = time;
	char* str = msgData + 8;

	unsigned long long h, key;
	while (*str++) {
		if (*str == '#')
		{
			h = 0, key = 0;
			while (*str++ != ' ')
				key = (key << 8) + *str;

			h = key % MAX_TABLE;

			while (HT[h].key != key)
				h = (h + 1) % MAX_TABLE;
			HT[h].key == key;
			HT[h].mcnt++;

			{
				int j = HT[h].mcnt - 1;
				while ((time > MSG_Srtd[HT[h].mID[j]]) && (j >= 0))
				{
					HT[h].mID[j + 1] = HT[h].mID[j];
					j = j - 1;
				}
				HT[h].mID[j + 1] = time;
			}
		}
	}
}

void followUser(int userID1, int userID2) {
	F[userID1][FC[userID1]++] = userID2;
}

/*userID À¯Àú°¡ µî·ÏÇÑ ¸Þ½ÃÁö¿Í ÆÈ·Î¿ìÇÏ°í ÀÖ´Â À¯ÀúµéÀÇ ¸Þ½ÃÁöµéÀ» Ã£¾Æ ÇØ´ç ¸Þ½ÃÁöÀÇ id ¸¦ retIDs[] ¿¡ ÀúÀåÇÏ°í, ÀúÀåÇÑ ¸Þ½ÃÁöÀÇ id °³¼ö¸¦ ¸®ÅÏÇÑ´Ù.*/
int getMessages(int userID, int retIDs[]) {
	for (int i = 0; i < FC[userID]; i++) {
		//F[userID][i]
		F_Srtd[MAX_USER][6];


	}
	return 0;
}


int searchByHashtag(char tagName[], int retIDs[]) {

#if 0
	unsigned long h = hash(key); int cnt = MAX_TABLE;

	while (HT[h].key[0] != 0 && cnt--) {
		if (mstrcmp(HT[h].key, key) == 0) {


			for (int i = 0; i < HT[h].mcnt; i++) {
				list5up(HT[h].mID[i], retIDs);
			}






			return 1;
		}
		h = (h + 1) % MAX_TABLE;
	}


#endif
	return 0;
}




int mstrcmp(const char* a, const char* b) {
	int i;
	for (i = 0; a[i] != '\0'; ++i)
		if (a[i] != b[i])
			return a[i] - b[i];
	return a[i] - b[i];
}

void mstrcpy(char* dest, const char* src) {
	int i = 0;
	while (src[i] != '\0') {
		dest[i] = src[i]; i++;
	}
	dest[i] = src[i];
}

unsigned long hash(const char* str) {
	unsigned long hash = 5381;	int c;
	while (c = *str++) {
		hash = (((hash << 5) + hash) + c) % MAX_TABLE;
	}
	return hash % MAX_TABLE;
}


/*
int find(const char* key, char* data)
{
	unsigned long h = hash(key);
	int cnt = MAX_TABLE;

	while (HT[h].key[0] != 0 && cnt--)
	{
		if (strcmp(HT[h].key, key) == 0)
		{
			strcpy(data, HT[h].data);
			return 1;
		}
		h = (h + 1) % MAX_TABLE;
	}
	return 0;
}


int add(const char* key, char* data)
{
	unsigned long h = hash(key);

	while (HT[h].key[0] != 0)
	{
		if (strcmp(HT[h].key, key) == 0)
		{
			return 0;
		}

		h = (h + 1) % MAX_TABLE;
	}
	strcpy(HT[h].key, key);
	strcpy(HT[h].data, data);
	return 1;
}
*/



#elif 0
#include<iostream>
#define NMAX	1000
using namespace std;
int map[1000][1000];
int cnt;

void init() {
	cnt = 0;
	for (int i = 0; i < NMAX; i++)
		for (int j = 0; j < NMAX; j++)
			map[i][j] = 0;
}

void fill(int x1, int y1, int x2, int y2)
{
	for (int i = x1; i <= x2; i++)
	{
		for (int j = y1; j <= y2; j++)
		{

			if (map[i][j] == -1)	continue;

			if (map[i][j] < 3)
			{
				map[i][j]++;
				if (map[i][j] == 3)
				{
					cnt++;
					map[i][j] = -1;
				}
			}
		}
	}
}

int main() {
	init();
	int testcast, n, x1, y1, x2, y2;
	cin >> testcast;
	for (int tc = 1; tc <= testcast; tc++)
	{
		cin >> n;
		for (int j = 0; j < n; j++)
		{
			cin >> x1 >> y1 >> x2 >> y2;
			fill(x1, y1, x2, y2);
		}
		cout << "#" << tc << " " << cnt << endl;
	}

}

#elif 0
#include<iostream>
using namespace std;

int main() {
	int tc;
	cin >> tc;
	for (int i = 1; i <= tc; i++)
	{
		int a, b;
		cin >> a >> b;
		cout << "#" << i << " " << (a + b) % 24 << endl;
	}
	return 1;
}
#elif  0
#include<iostream>
using namespace std;

#define ull	unsigned long long 

ull hash(char* s)
{
	ull h = 0;
	char c;
	while (c = *s++)
	{
		h = (h << 8) + c;
	}
	return h;
}

ull hashtoidx(char* s)
{
	ull h = hash(s);
	char c;
	while (c = *s++)
	{
		h = (h << 8) + c;
	}
	return h;
}



int main() {
	cout << sizeof(unsigned long long int) << endl;
	cout << hex << ULLONG_MAX << endl;
}

#elif 0
#include <iostream>
#include <string>
#include <vector>
#include <limits.h>
using namespace std;

#define MAX_HT	200000
struct {
	char key[21];
	int cnt;
}ht[MAX_HT];

int hash(char* s)
{
	int h = 5381, c;
	while (c = *s++)
	{
		h = (((h << 5) + h) + c) % MAX_HT;
	}
	return h;
}
void addmem(char s[])
{
	int h = ::hash(s);
	while (ht[h].key[0])
	{
		if (strcmp(ht[h].key, s) == 0)	break;
		else h = (h + 1) % MAX_HT;
	}
	strcpy(ht[h].key, s);
	ht[h].cnt++;
}

void delmem(char s[])
{
	int h = ::hash(s);
	while (ht[h].key[0])
	{
		if (strcmp(ht[h].key, s) == 0)	break;
		else h = (h + 1) % MAX_HT;
	}
	//strcpy(ht[h].key, s);
	ht[h].cnt--;
}

int getcnt(char* s)
{
	int h = ::hash(s);
	while (ht[h].key[0])
	{
		if (strcmp(ht[h].key, s) == 0)	break;
		else h = (h + 1) % MAX_HT;
	}
	return ht[h].cnt;
}

//string solution(vector<string> participant, vector<string> completion) {

void main() {

	vector<string> participant{ "Evan", "Jung", "Choi" };
	vector<string> completion{ "Evan", "Jung" };

	for (int i = 0; i < MAX_HT; i++)
	{
		ht[i].cnt = 0;
	}


	string ans = "";
	char str[1000];


	for (auto& name : participant) {
		strcpy(str, name.c_str());
		addmem(str);
	}

	for (auto& name : completion) {
		strcpy(str, name.c_str());
		delmem(str);
	}

	//for (auto& rest : ht) {
	//	//cout << rest.first << endl;
	//	return rest.first;
	//}

	for (int i = 0; i < MAX_HT; i++)
	{
		if (ht[i].cnt > 0)
		{

			string aaa(ht[i].key);
			//return  aaa; 
			/*cout << INT_MAX << endl;
			cout << UINT_MAX << endl;
			cout << LONG_MAX << endl;
			cout << ULONG_MAX << endl*/;
			printf("%0x \n", INT_MAX);
			printf("%0x \n", UINT_MAX);
			printf("%0x \n", LONG_MAX);
			printf("%0x \n", ULONG_MAX);
		}
	}
	//return ans;
}

//int main() {
//
//	vector<string> v;
//
//	v.push_back("Evan");
//	v.push_back("Jung");
//	v.push_back("Choi");
//
//	char s[100];
//	for (auto& name : v)
//	{
//		strcpy(s, name.c_str());
//		cout << s << endl;
//	}
//
//
//	return 0;
//}
#elif 0
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

unordered_map<string, int> ht;
//
//int main() {
//
//	ht["Evan"] = 1;
//	ht["Jung"] = 1;
//	ht["Choi"] = 1;
//
//	ht.erase("Evan");
//	ht.erase("__Choi"); 
//
//	for (auto& rest : ht)
//		cout << rest.first;
//	return 0;
//}

string solution(vector<string> participant, vector<string> completion) {
	for (auto& name : participant) {
		ht[name] ++;
	}

	for (auto& name : completion) {
		ht.erase(name);
	}

	for (auto& rest : ht) {
		//cout << rest.first << endl;
		return rest.first;
	}
}

#elif 0
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

string solution(vector<string> participant, vector<string> completion) {
	//string answer = "";
	unordered_map<string, int> participants;
	for (const string& name : participant)
	{
		participants[name]++;
	}

	for (const string& name : completion)
	{
		participants[name]--;
	}

	for (auto pair : participants)
	{
		if (pair.second > 0)
			return pair.first;
	}

	//return answer;
}



#elif 0


#include<unordered_map>
#include<iostream>
using namespace std;

unordered_map<int, int>	ht;

int main() {

	ht[1231234] = 100;
	ht[555231234] = 200;
	ht[9555231234] = 400;

	cout << ht[1231234] << endl;
	auto it = ht.find(1231234);
	cout << ht.find(1231234)->first << " " << ht.find(1231234)->second << endl;


	return 0;
}
#elif 0
#include<iostream>
#include<assert.h>
using namespace std;
#define MAX_HT	100000
#define rint register int 

struct {
	int key, idx;
}ht[MAX_HT];

void addId(int key, int id)
{
	int h = key % MAX_HT;
	while (ht[h].key)
	{
		if (ht[h].key == key)	break;
		else h = (h + 1) % MAX_HT;
	}
	ht[h] = { key, id };
}

int getId(int key)
{
	int h = key % MAX_HT;
	while (ht[h].key)
	{
		if (ht[h].key == key)	return ht[h].idx;
		else h = (h + 1) % MAX_HT;
	}
	assert(0);
	return false;
}
void init() {
	for (rint i = 0; i < MAX_HT; i++)	ht[i].key = 0;
}
int main() {


	init();
	addId(100040000, 1);
	addId(200020000, 2);
	addId(300006000, 3);
	addId(400000200, 4);
	addId(500000010, 5);

	cout << getId(300006000) << endl;
	cout << getId(400000200) << endl;
	cout << getId(500000010) << endl;

	for (rint i = 0; i < MAX_HT; i++)
	{
		if (ht[i].key) cout << ht[i].key << " " << ht[i].idx << " " << i << endl;
	}

	return 0;
}
#elif 0
#include<stdio.h>
#include<assert.h>
#include<iostream>
using namespace std;
#define MAX_HT	100000
#define rint register int

struct {
	int key;
	int id;
}ht[MAX_HT];


void addId(int key, int id)
{
	int h = key % MAX_HT;
	while (ht[h].key)
	{
		if (ht[h].key == key)	break;
		else h = (h + 1) % MAX_HT;
	}
	ht[h] = { key, id };
}

int getId(int key)
{
	int h = key % MAX_HT;
	while (ht[h].key)
	{
		if (ht[h].key == key)	return ht[h].id;
		h = (h + 1) % MAX_HT;
	}
	assert(0);
	return false;
}

void init() {
	for (rint i = 0; i < MAX_HT; i++)
	{
		ht[i].key = 0;
	}
}

int main()
{
	init();
	addId(100000000, 1);
	addId(200000000, 2);
	addId(300000000, 3);
	addId(400000000, 4);
	addId(500000000, 5);

	cout << getId(400000000) << endl;
	cout << getId(500000000) << endl;
	return 0;
}
#elif 0
#include<iostream>
#define rint register int

struct Node {
	int id, Osize, Vsize;
	Node* prev, *next;
};


#define MAX_HT	10001*2

struct {
	int key;
}ht[MAX_HT];

int hash(int key) {
	int h = 5381;
	h = key % MAX_HT;
	return h;
}

//#define hash(key)	(key%MAX_HT)

int getId(int key) {
	int h = key % MAX_HT;
	int cnt = MAX_HT;
	while (ht[h].key != 0 && cnt--)
	{
		if (ht[h].key == key)
		{
			return h;
		}
		h = (h + 1) % MAX_HT;
	}
	printf("E2 \n");
	return -1;
}

int addId(int key)
{
	int h = key % MAX_HT;
	while (ht[h].key != 0)
	{
		if (ht[h].key == key)
		{
			printf("E1 \n");
			return 0;
		}
		h = (h + 1) % MAX_HT;
	}
	ht[h].key = key;
	return h;
}

void init()
{
	for (rint i = 0; i < MAX_HT; i++)
		ht[i].key = 0;
}

int cmdAdd(int newID, int pID, int fileSize)
{
	int idx = add(newID);

}

int cmdMove(int tID, int pID)

int cmdInfect(int tID)

int cmdRecover(int tID)

int cmdRemove(int tID)


#elif 0

#include<stdio.h>

template <typename T>
void Swap(T& a, T& b)
{
	T t;
	t = a; a = b; b = t;
}

//struct tag_st {
//	int i; double d;
//};

template<typename T>
void Print(T& a, T& b)
{
	printf("Print %d, %d \n", (T)a, (T)b);

}


int main() {

	int a = 3, b = 4;
	double c = 1.2, d = 3.4;
	char e = 'e', f = 'f';
	//tag_st g = { 1, 2.3 }, h = { 4,5.6 };
	Print(a, b);
	Swap(a, b);
	Print(a, b);

	Print(c, d);
	Swap(c, d);
	Print(c, d);

	Print(e, f);
	Swap(e, f);
	Print(e, f);
	//Swap(g, h);

	return 0;
}


#elif 0
#include<iostream>
#include<unordered_map>
using namespace std;

unordered_map<string, int> htab;


int main() {


	htab["Evan"] = 1;
	htab["Jung"] = 1000;
	htab["Choi"] = 3000;

	for (auto& elem : htab)
		cout << elem.first << elem.second << endl;

	//cout << htab.find("Evan")
	return 0;
}

#elif 0
#define _STL_
#include<stdio.h>

#ifdef _STL_
#include<stdlib.h> 
#else
int atoi(char* s)
{
	char c;
	int ret = 0;
	while ((c = *s++) != NULL)
	{
		if (c < '0' || c > '9')	return ret;
		ret = ret * 10 + (c - '0');
	}
	return ret;
}
#endif

int main()
{
	char a[100];
	scanf("%s ", a);

	printf("%d \n", atoi(a));

	return 0;
}

#elif 0
#include<iostream>
using namespace std;
int main() {
	for (int i = -10; i < 10; i++) {
		for (int j = -10; j < 10; j++) {
			int ret = (i + j) > 5 ? 5 : (i + j) < 1 ? 1 : (i + j);
			cout << i << "+" << j << "=" << ret << endl;
		}
	}
	return 1;
}

#elif 0
#include<list>
//#include<algorithm>
using namespace std;

list<int> li[6][6];

struct Mercenary {
	int team;
	list<int>::iterator iter;
}mer[100003];

void init() {
	for (int i = 1; i <= 5; i++)
		for (int j = 1; j <= 5; j++)
			li[i][j].clear();
}

void hire(int id, int team, int score) {
	mer[id] = { team, li[team][score].insert(li[team][score].end(), id) };
}

void fire(int id) {
	*mer[id].iter = 0;
}

void updateSoldier(int id, int score) {
	*mer[id].iter = 0;
	hire(id, mer[id].team, score);
}

void updateTeam(int team, int newScore) {
	if (newScore < 0)
		for (int i = 2; i <= 5; i++) {
			int score = max(1, i + newScore);
			li[team][score].splice(li[team][score].end(), li[team][i]);
		}
	else if (newScore > 0)
		for (int i = 4; i >= 1; i--) {
			int score = min(5, i + newScore);
			li[team][score].splice(li[team][score].end(), li[team][i]);
		}
}

int bestSoldier(int team) {
	int ret = 0;
	for (int i = 5; i >= 1; i--) {
		if (li[team][i].empty()) continue;
		for (auto x : li[team][i]) ret = max(x, ret);
		if (ret) break;
	}
	return ret;
}

#elif 0
#define rint register int

//ID_node ids[100000];

struct ID {
	int team, score, fired;
}ids[100000];


struct T_node {

	int mcnt;
};
T_node teams[5];

void updateSoldier(int mID, int mScore) {
	id[mID].score = mScore;
}

void updatTeam(int mTeam, int mChangeScore) {
	T_node team = teams[mTeam];

	for (rint i = 0; i < team.mcnt; i++) {
		team.id[i].score

	}
}

#elif 0
#include<iostream>
#include<list>
using namespace std;
#define MAX_NODE 10000

list<int> l;

void init() {

}

void addNode2Head(int data) {
	l.push_front(data);
}

void addNode2Tail(int data) {
	l.push_back(data);
}

void addNode2Num(int data, int num) {
	list<int>::iterator iter = l.begin();
	for (int i = 0; i < num; i++)		iter++;
	l.insert(iter, data);
}

void removeNode(int data) {
	list<int>::iterator it = find(l.begin(), l.end(), data);
	if (it == l.end())	return;
	else l.erase(it);
}

int getList(int output[MAX_NODE]) {
	int ret = 0;
	for (int i : l) {
		ret++;
		cout << i << endl;
	}
	return ret;
}

#elif 0
#include<iostream>
#include<list>

using namespace std;

int main()
{
	list<int> int_list;

	int_list.push_back(1);
	int_list.push_back(2);
	int_list.push_back(3);
	int_list.push_front(4);
	int_list.push_front(5);

	if (int_list.empty())
		cout << "Is empty" << endl;
	else
		cout << "This size : " << int_list.size() << endl;

	for (int i : int_list)
	{
		cout << i << endl; // ���
	}

	list<int>::iterator iter = int_list.begin();
	iter++;
	cout << "2��°" << endl;
	int_list.erase(iter);

	iter = int_list.begin();
	iter++;
	cout << "2��°�� 9����" << endl;
	int_list.insert(iter, 9);

	for (int i : int_list)
	{
		cout << i << endl; //���
	}

	cout << "ù�� ° �� : " << int_list.front() << endl;

	cout << "��� �����" << endl;
	int_list.clear();

	if (int_list.empty())
		cout << "Is empty" << endl;
	else
		cout << "This size : " << int_list.size() << endl;

	return 0;
}
#elif 0

#include<stdio.h>
#include<string.h>

char s[50000][7];

//struct {
//	char s[7];
//	bool del;
//}a[50000];
int idx = 0;

void init() {
	idx = 0;
}

void addWord(char str[]) {
	strcpy(s[idx++], str);
}


bool mystrcmp(char* s1, char* s2) {
	while (*s2 != NULL) {
		if (*s2 == '*') {
			while (*s1 != *(s2 + 1)) {
				s1++;
			}
			s2++;
			//continue;
		}
		else if (*s1 != *s2) {
			return 0;
		}
		s1++; s2++;
	}
	if (*s1 != NULL)	return 0;
	return 1;
}

int removeWord(char str[]) {
	int ret = 0;
	for (int i = 0; i < idx; i++) {
		if (mystrcmp(s[i], str))
			ret += 1;
	}
	return ret;
}

int searchWord(char str[]) {
	int ret = 0;
	for (int i = 0; i < idx; i++) {
		ret += mystrcmp(s[i], str);
	}
	return ret;
}

#elif 0
#include<stdlib.h>
#include<stdio.h>

struct {
	int sT, eT;
}CiD[10000];

struct {

}RiD[10000];


int myatoi(char* s) {
	int ret = 0; char c;
	while ((c = *s++) != NULL) {
		if (c > '9' || c < '0')	return ret;
		ret = ret * 10 + (c - '0');
	}
	return ret;
}
int main() {
	char a[30] = "2019:12:31-23:13:27";
	printf("%d \n", atoi(a));
	printf("%d \n", atoi(&a[5]));
	printf("%d \n", atoi(&a[8]));

	int date, time;
	date = atoi(a) * 10000 + atoi(&a[5]) * 100 + atoi(&a[8]);
	time = atoi(&a[11]) * 10000 + atoi(&a[14]) * 100 + atoi(&a[17]);
	printf("%d \n", date);
	printf("%d \n", time);
	date = myatoi(a) * 10000 + myatoi(&a[5]) * 100 + myatoi(&a[8]);
	time = myatoi(&a[11]) * 10000 + myatoi(&a[14]) * 100 + myatoi(&a[17]);
	printf("%d \n", date);
	printf("%d \n", time);
	return 1;
}



#elif 0


#include <cstdio>
#include <list>
#include <iterator>
#include <string>
#include <iostream>
using namespace std;
void printlist(list<char> l) {
	for (list<char>::iterator iter = l.begin(); iter != l.end(); iter++) {
		cout << *iter;
	}
	cout << endl;
}
int main() {
	string str, input;
	list<char> l;
	int N, i;
	char buf;

	cin >> str;
	scanf("%d%c", &N, &buf);
	for (i = 0; i < str.length(); i++) {
		l.push_back(str[i]);
	}

	list<char>::iterator iter = l.end();

	for (i = 0; i < N; i++) {
		getline(cin, input);
		if (input[0] == 'L') {
			if (iter != l.begin()) iter--;
		}
		else if (input[0] == 'D') {
			if (iter != l.end()) iter++;
		}
		else if (input[0] == 'B') {
			if (iter != l.begin()) iter = l.erase(--iter);
		}
		else {
			l.insert(iter, input[2]);
		}
	}

	printlist(l);
	return 0;
}
#elif 0
#include<stdio.h>
char a[600000 + 5];
struct NODE {
	char data;
	NODE* prev, *next;
};
NODE node[600000 + 5];
NODE* pHead, *pTail;
int idx = 0;
NODE* myalloc() {
	return &node[idx++];
}
void addnode(NODE* t, char c) {
	NODE* p = myalloc();
	p->data = c;
	NODE* L = t->prev;
	t->prev = p;
	p->next = t;
	L->next = p;
	p->prev = L;
}
NODE* delnode(NODE* t) {
	if (t == pTail || t == pHead)	return pTail;
	NODE* L = t->prev;
	NODE* R = t->next;
	L->next = R;
	R->prev = L;
	return R;
}
void printlist() {
	for (NODE* iter = pHead->next; iter != pTail; iter = iter->next) {
		printf("%c", iter->data);
	}
	printf("\n");
}
int main() {
	idx = 0;
	pHead = myalloc();
	pTail = myalloc();
	pHead->next = pTail;
	pHead->prev = NULL;
	pTail->next = NULL;
	pTail->prev = pHead;
	NODE* cur = pTail;

	scanf("%s ", a);
	for (int i = 0; a[i] != NULL; i++) {
		addnode(cur, a[i]);
	}

	int N;
	scanf("%d ", &N);
	for (int i = 0; i < N; i++) {
		char cmd, ch;
		scanf("%c ", &cmd);
		switch (cmd) {
		case 'L':
			if (cur->prev != pHead) cur = cur->prev;
			break;
		case 'D':
			if (cur != pTail) cur = cur->next;
			break;
		case 'B':
			if (cur->prev != pHead) 	delnode(cur->prev);
			break;
		case 'P':
			scanf("%c ", &ch);
			addnode(cur, ch);
			break;
		default:
			break;
		}
	}
	printlist();
	return 0;
}



#elif 0

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <cstdio>
#include <list>
#include <iterator>
#include <string>
#include <iostream>
using namespace std;

#define MAX_STR 600
#define MAX_CMD 500000

struct NODE {
	char data;
	NODE* prev, *next;
};
NODE b[600000 + 5];
int idx = 0;
char a[600000];


NODE* myalloc() {
	return &b[idx++];
}

NODE* pTail = myalloc();
NODE* pHead = myalloc();


void genstr(char* a) {
	srand((unsigned int)time(NULL));
	for (int i = 0; i < MAX_STR; i++) {
		a[i] = rand() % 26 + 'a';
	}
}
void gencmd(char* cmd, char* ch) {
	switch (rand() % 4) {
	case 0:
		*cmd = 'L';
		break;
	case 1:
		*cmd = 'D';
		break;
	case 2:
		*cmd = 'B';
		break;
	case 3:
		*cmd = 'P';
		*ch = rand() % 26 + 'a';
		break;
	}
}

void addnode(NODE* t, char c) {
	NODE* p = myalloc();
	p->data = c;
	NODE* L = t->prev;
	t->prev = p;
	p->next = t;
	L->next = p;
	p->prev = L;
}

NODE* delnode(NODE* t) {
	if (t == pHead || t == pTail)	return NULL;
	NODE* L = t->prev;
	NODE* R = t->next;
	L->next = R;
	R->prev = L;
	return R;
}

void printlist() {
	for (NODE* iter = pHead->next; iter != NULL; iter = iter->next) {
		printf("%c", iter->data);
	}
	printf("\n");
}


NODE* cur;
void myinit() {
	pHead->next = pTail;
	pHead->prev = NULL;
	pTail->next = NULL;
	pTail->prev = pHead;
	cur = pTail;
	for (int i = 0; a[i] != NULL; i++) {
		addnode(pTail, a[i]);
	}
}
void mymain(char cmd, char ch) {

	switch (cmd) {
	case 'L':
		if (cur->prev != pHead) cur = cur->prev;
		break;
	case 'D':
		if (cur != pTail) cur = cur->next;
		break;
	case 'B':
		if (cur->prev != pHead) cur = delnode(cur->prev);
		break;
	case 'P':
		addnode(cur, ch);
		break;
	}
}

int main() {
	//genstr(a);
	scanf("%s ", a);
	myinit();
	//printlist();
	char cmd, ch = 0;

	int M;
	scanf("%d ", &M);
	for (int i = 0; i < M; i++) {
		//gencmd(&cmd, &ch);
		scanf("%c ", &cmd);
		if (cmd == 'P')	scanf("%c ", &ch);
		mymain(cmd, ch);
	}

	printlist();
	return 1;
}


#elif 0
#include<stdio.h>

char s[600000 + 1];

struct NODE {
	char c;
	NODE* prev, *next;
}a[100000 + 1];
int idx = 0;

NODE* myalloc() {
	return &a[idx++];
}

NODE* pHead = myalloc();
NODE* pTail = myalloc();
NODE* cur = pTail;

void addNode(NODE* t, char c) {
	NODE* p = myalloc();
	p->c = c;

	NODE* L = t->prev;
	t->prev = p;
	p->next = t;

	L->next = p;
	p->prev = t;
}

void delNode(NODE* d) {
	if (d == pHead || d == pTail)	return;
	d->prev->next = d->next;
	d->next->prev = d->prev;
}
void print_list() {
	for (NODE* iter = pHead->next; iter != pTail; iter = iter->next)
		printf("%c", iter->c);
	printf("\n");
}
int main() {
	pHead->next = pTail;
	pHead->prev = NULL;
	pTail->next = NULL;
	pTail->prev = pHead;

	scanf("%s ", s);
	for (int i = 0; s[i] != 0; i++)
		addNode(pTail, s[i]);


	print_list();

	for (NODE* iter = pHead->next; iter != pTail; iter = iter->next)
		if (iter->c == ';')
			delNode(iter);

	print_list();
}
#elif 0
#include<stdio.h>

char s[600000 + 1];

struct NODE {
	char c;
	NODE* prev, *next;
}a[100000 + 1];
int idx = 0;

NODE* myalloc() {
	return &a[idx++];
}

NODE* pHead = myalloc();
NODE* pTail = myalloc();
NODE* cur = pTail;

void addNode(NODE* here, char c) {
	NODE* p = myalloc();
	p->c = c;
	p->prev = here->prev;
	here->prev = p;

	p->next = here;
	p->prev->next = p;
}
//
//void addLast(char c) {
//	NODE* p = myalloc();
//	p->c = c; 
//	p->prev = pTail->prev;
//	pTail->prev = p;
//
//	p->next = p->prev->next;
//	p->prev->next = p; 
//}

void delNode(NODE* del) {
	if (del == pHead || del == pTail)	return;
	del->prev->next = del->next;
	del->next->prev = del->prev;
}


int main() {
	pHead->next = pTail;
	pTail->prev = pHead;


	scanf("%s ", s);
	for (int i = 0; s[i] != 0; i++) addLast(s[i]);

	int N;
	scanf("%d ", &N);
	for (int i = 0; i < N; i++) {
		char cmd;
		scanf("%c ", &cmd);
		switch (cmd) {
		case 'L':
			cur = cur->prev;
			break;
		case 'D':
			cur = cur->next;
			break;
		case 'B':
			delNode();
			break;
		case 'P':
			break;
		default:
			break;
		}

	}
}
#elif 0
#include <cstdio>

struct NODE {
	char ch;
	NODE* nxt, *prv;
};

NODE HEAD;
NODE TAIL;
NODE* cur;

char str[100000 + 1];

void init() {
	HEAD.nxt = &TAIL;
	HEAD.prv = NULL;
	TAIL.nxt = NULL;
	TAIL.prv = &HEAD;

	cur = &TAIL;
}

void insertNode(char ch) {
	NODE* p = new NODE;
	p->ch = ch;

	p->nxt = cur;
	p->prv = cur->prv;
	cur->prv->nxt = p;
	cur->prv = p;
}

void deleteNode()
{
	NODE* del = cur->prv;
	if (del == &HEAD) return;

	cur->prv->prv->nxt = cur;
	cur->prv = cur->prv->prv;

	delete del;
}

void curLeft()
{
	if (cur->prv->prv == NULL) return;
	cur = cur->prv;
}

void curRight()
{
	if (cur->nxt == NULL) return;
	cur = cur->nxt;
}

void print()
{
	NODE* head = &HEAD;
	head = head->nxt;

	for (;;)
	{
		if (head->nxt == NULL) break;

		printf("%c", head->ch);

		head = head->nxt;
	}
}

int main()
{
	int i;
	int N = 0;
	char ch = 0;

	init();
	scanf("%s", str);
	for (i = 0; str[i] != 0; i++)
	{
		insertNode(str[i]);
	}

	scanf("%d", &N);
	for (i = 0; i < N; i++)
	{
		scanf(" %c", &ch);
		if (ch == 'L')
		{
			curLeft();
		}
		else if (ch == 'D')
		{
			curRight();
		}
		else if (ch == 'B')
		{
			deleteNode();
		}
		else if (ch == 'P')
		{
			scanf("  %c", &ch);
			insertNode(ch);
		}
	}

	print();
	printf("\n");

	return 0;
}


#elif 0
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#define M	80000000

#define Swap(a, b)	do {int temp=(a); (a)=(b); (b) = temp;}while(0);
int a[M], b[M], c[M], ao[M], bo[M], co[M];
long long int is_cnt = 0, hs_cnt = 0, ss_cnt = 0;

void printArray(int a[]) {
	//return;
	for (int i = 0; i < 10; i++)
		printf("%8d ", a[i]);
	printf("\n");
}
void is(int a[]) {
	int cnt = 0;
	for (int i = 1; i < M; i++) {
		int key = a[i];
		int j = i - 1;
		while (j >= 0 && a[j] < key) {
			a[j + 1] = a[j];
			j = j - 1;
			is_cnt++;
		}
		a[j + 1] = key;
	}
}

void ss(int a[]) {
	//int cnt = 0;
	for (int i = 0; i < 10; i++) {
		int temp = a[i], idx = i;
		for (int j = i + 1; j < M; j++) {
			if (temp < a[j]) {
				temp = a[j]; idx = j;
			}
			ss_cnt++;
		}
		Swap(a[i], a[idx]);
	}
}

//int hy(int a[], int n) {
//	for (int i = 1; i < n; i++) {
//		int c = i;
//		while (c > 0) {
//			int root = (c - 1) / 2;
//			if (a[root] < a[c])	Swap(a[root], a[c]);
//			c = root;
//			hs_cnt++;
//		}		
//	}
//	return a[0];
//}


void heapify(int arr[], int n, int i) {
	int largest = i;
	int l = 2 * i + 1;
	int r = 2 * i + 2;
	hs_cnt++;
	if (l < n && arr[l] > arr[largest])
		largest = l;

	if (r < n && arr[r] > arr[largest])
		largest = r;

	if (largest != i) {
		Swap(arr[i], arr[largest]);
		heapify(arr, n, largest);
	}
}

void heapSort(int arr[], int n) {
	for (int i = n / 2 - 1; i >= 0; i--)
		heapify(arr, n, i);

	int cnt = 0;
	for (int i = n - 1; i >= 0; i--) {

		if (cnt < 10) {
			co[cnt] = arr[0];
			cnt++;
		}
		else
			return;

		Swap(arr[0], arr[i]);
		heapify(arr, i, 0);
	}
}


int main() {

	srand((unsigned int)time(NULL));
	for (int i = 0; i < M; i++) a[i] = b[i] = c[i] = rand() % M;
	//printArray(a);
	//printArray(b);
	//printArray(c);

	//is(a);
	//printArray(a);

	ss(b);

	printArray(b);

	//for (int i = M, cnt = 0; i >= 0 && cnt < M; i--) {
	//	hy(b, i);
	//	o[cnt++] = b[0];
	//	Swap(b[0], b[i - 1]);
	//}

	heapSort(c, M);

	printArray(co);

	printf("%lld \n%lld \n%lld \n", is_cnt, ss_cnt, hs_cnt);
	return 1;

}


#elif 0
#include<stdio.h>
int main() {
	int T, tc, N, M;
	char Map[100][100];
	scanf("%d ", &T);
	for (int tc = 1; tc <= T; tc++) {
		scanf("%d %d ", &N, &M);
		for (int i = 0; i < N; i++)
			for (int j = 0; j < M; j++)
				scanf("%c ", &Map[i][j]);

		for (int i = 0; i < N; i++)
			for (int j = 0; j < M; j++) {
				if (Map[i][j] == '#') {
					if (i == N - 1 || j == M - 1)
						goto err;
					if (Map[i + 1][j] == '#' && Map[i][j + 1] == '#' && Map[i + 1][j + 1] == '#')
						Map[i][j] = '.', Map[i + 1][j] = '.', Map[i][j + 1] = '.', Map[i + 1][j + 1] = '.';
					else
						goto err;

				}

			}

	pass:
		printf("#%d %s\n", tc, "YES");
		continue;

	err:
		printf("#%d %s\n", tc, "NO");

	}
	return 0;
}
#elif 0
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#define Swap(a,b)	do{int temp = (a) ; (a) = (b) ; (b) =temp;} while(0)

int number = 9;
int heap[9] = { 7,6,  5,8,  9,5,  6,1, 3 };
int o[9];

int heapify(int* a, int num) {
	for (int i = 1; i < num; i++) {
		int c = i;
		while (c > 0) {
			int root = (c - 1) / 2;
			if (a[root] < a[c])	Swap(a[root], a[c]);
			c = root;
		}
	}
	return heap[0];
}

int main() {
	int k = 0;
	srand((unsigned int)time(NULL));
	while (k < 5) {
		int cnt = 0;

		for (int i = 0; i < 9; i++)
			heap[i] = rand() % 100;

		for (int i = number; i >= 0 && cnt < 9; i--) {
			o[cnt++] = heapify(heap, i);
			Swap(heap[i - 1], heap[0]);
		}


		for (int i = 0; i < cnt; i++)		printf("%3d ", heap[i]);
		printf("\n");
		for (int i = 0; i < cnt; i++)		printf("%3d ", o[i]);
		printf("\n");
		k++;
	}
	return 1;
}
#elif 0
int n[11][50001];
int section[50001];
int ncnt;
int delcnt;
int del[50001];

struct USER {
	int userid;
	int prefer;
}u[100001];

void init() {
	ncnt = 0;
	delcnt = 0;
	for (int i = 0; i <= 100000; i++)
		u[i].prefer = 10;
	for (int i = 0; i <= 10; i++)
		n[i][0] = 0;
}

void addNews(int mSection, int mNewsId) {
	ncnt++;
	for (int i = 0; i <= 10; i++)
		n[i][mNewsId] = 0;
	n[mSection][mNewsId] = 10;
	section[mNewsId] = mSection;
	del[mNewsId] = 0;
}

void eraseNews(int mNewsId) {
	delcnt++;
	del[mNewsId] = 1;
}

void readNews(int mUserId, int mNewsId) {
	u[mUserId].prefer = section[mNewsId];
	for (int i = 0; i <= 10; i++)
		n[i][mNewsId]++;
}

void changeSection(int mNewsId, int mSection)
{
	n[section[mNewsId]][mNewsId] -= 10;
	section[mNewsId] = mSection;
	n[mSection][mNewsId] += 10;
}

int getList(int mUserId, int mList[])
{
	int* my_n;
	for (int i = 0; i < 10; i++)
		mList[i] = 0;
	my_n = n[u[mUserId].prefer];

	int end = 10;

	if (end > ncnt - delcnt)
		end = ncnt - delcnt;

	for (int i = 1; i <= ncnt; i++) {
		if (del[i]) continue;
		if (my_n[mList[9]] > my_n[i]) continue;

		int j;
		for (j = 9; j > 0; j--) {
			if (my_n[mList[j - 1]] > my_n[i])break;
			mList[j] = mList[j - 1];
		}
		mList[j] = i;
	}
	return end;
}
#elif 0
#include<stdio.h> 
#include<stdlib.h>
#include<time.h>
#define Swap(a,b) {int temp = (a); (a) =(b) ; (b) =temp;}
#define N	20
#define M	10
#define MOD 100

void printArray(int a[], int n) {

	for (int i = 0; i < n; i++)
		printf("[%2d] %2d\n", i, a[i]);
	printf("-------------------------------\n");
}

int main() {

	int a[1000], b[1000], c[1000];
	srand((unsigned int)time(NULL));

	for (int i = 0; i < N; i++)		a[i] = rand() % 100;

	for (int i = 1; i < N; i++) {
		int key = a[i];
		int j = i - 1;
		while (j >= 0 && a[j] < key) {
			a[j + 1] = a[j];
			j = j - 1;
		}
		a[j + 1] = key;
	}

	printArray(a, N);

	for (int i = 0; i < N; i++)		b[i] = a[i] = rand() % MOD;

	for (int i = 0; i < M; i++) {
		int MinV = 99999, MinK;
		for (int j = 0; j < N; j++) {
			if (MinV > b[j]) {
				MinV = b[j], MinK = j;
			}
		}
		b[MinK] = 99999;
		c[i] = MinK;
	}

	printArray(a, N);
	printArray(b, N);
	printArray(c, M);

	for (int i = 0; i < M; i++)
		printf("[%2d] %2d\n", i, a[c[i]]);
	printf("-------------------------------\n");



	for (int i = 0; i < N; i++)		b[i] = a[i] = rand() % MOD;
	a[20] = 55;

	printArray(a, N + 1);

	for (int i = 1; i < N + 1; i++) {
		int Key = a[i];
		int j = i - 1;
		while (j >= 0 && a[j] < Key) {
			a[j + 1] = a[j];
			j = j - 1;
		}
		a[j + 1] = Key;
	}

	printArray(a, N + 1);


	//for (int i = 1; i < N; i++) 
	{
		int Key = 55;
		int j = N - 1;
		while (j >= 0 && a[j] < Key) {
			a[j + 1] = a[j];
			j = j - 1;
		}
		a[j + 1] = Key;
	}

	printArray(a, N + 1);


	//for (int i = 1; i < N; i++) 
	{
		int Key = 55;
		int j = N - 1;
		while (j >= 0 && a[j] < Key) {
			a[j + 1] = a[j];
			j = j - 1;
		}
		a[j + 1] = Key;
	}

	printArray(a, N + 1);




	return 1;
}


#elif 0
#include <stdio.h>

void show_operaton_priority() {
	printf("%d \n", 1 << 1 + 1);
	printf("%d \n", (1 << 1) + 1);
}


#include<time.h>
void show_oerationtime() {
	clock_t start = clock();
	//....s
	printf("%lf \n", (clock() - start) / CLOCKS_PER_SEC);
}

void for_loop_operating() {

	for (printf("1"); printf("2"); printf("3")) {
		while (0);
		printf("\n");
	}

}

int main() {
	show_operaton_priority();
	for_loop_operating();
	return 0;
}
#elif 0
#include<stdio.h>
#include<malloc.h>


struct Node {
	int data;
	struct Node* prev;
	struct Node* next;
};

Node* list_create(int _data) {
	Node* node = (Node*)malloc(sizeof(Node));
	node->prev = NULL;
	node->next = NULL;
	node->data = _data;
	return node;
}

// 새로운 노드를 받아서 
Node* list_insert(Node* _head, Node* new_node) {
	Node* next = _head->next;
	_head->next = new_node;
	new_node->next = next;
	new_node->prev = _head;
	if (next != NULL) {
		next->prev = new_node;
	}
	return new_node;
}

int list_erase(Node* head, int _data) {
	Node* it = head->next;
	int ret = 0;

	while (it != NULL) {
		if (it->data == _data) {
			Node* prev = it->prev;
			Node* next = it->next;
			Node* tmp = it;
			it = it->next;

			prev->next = next;
			if (next != NULL) {
				next->prev = prev;
			}

			free(tmp);
			ret++;
		}
		else {
			it = it->next;
		}
	}
	return ret;
}

int main(int argc, char* argv[]) {
	int T, N;
	setbuf(stdout, NULL);

	scanf("%d", &T);

	for (int test_case = 1; test_case <= T; test_case++) {
		scanf("%d", &N);

		Node* head = list_create(NULL);
		printf("#%d", test_case);
		for (int i = 0; i < N; i++) {
			int mode, data;
			scanf("%d%d", &mode, &data);

			if (mode == 1) {		// 노드 추가 명령
				Node* node = list_create(data);
				list_insert(head, node);
			}
			else if (mode == 2)
			{
				printf(" %d", list_erase(head, data));
			}
		}

		while (head != NULL)
		{
			Node* tmp = head;
			head = head->next;
			free(tmp);
		}
		printf("\n");
	}0
	return 0;
}
#elif 0
#include<iostream>
using namespace std;
struct Trie {
	Trie* ch[26];
	bool end;
	Trie() {
		end = false;
		for (int i = 0; i < 26; i++) ch[i] = NULL;
	}
	~Trie() {
		for (int i = 0; i < 26; i++) if (ch[i]) delete ch[i];
	}
	void insert(const char* s) {
		if (!*s) {
			this->end = true;
			return;
		}
		int now = *s - 'A';
		if (!ch[now]) ch[now] = new Trie;
		ch[now]->insert(s + 1);
	}
	bool find(const char* s) {
		if (!*s) {
			if (end) return true;
			return false;
		}
		int now = *s - 'A';
		if (!ch[now]) return false;
		return ch[now]->find(s + 1);
	}
};
int main(void) {
	Trie* root = new Trie;
	string s;
	root->insert(s.c_str());
	string tmp = "AAA";
	if (root->find(tmp.c_str())) cout << "Find!";
	else cout << "No";
	delete root;
}

#elif 0
#include <cstdio>

struct NODE {
	char ch;
	NODE* nxt, *prv;
};

NODE HEAD;
NODE TAIL;
NODE* cur;

char str[100000 + 1];

void init() {
	HEAD.nxt = &TAIL;
	HEAD.prv = NULL;
	TAIL.nxt = NULL;
	TAIL.prv = &HEAD;

	cur = &TAIL;
}

void insertNode(char ch)
{
	NODE* p = new NODE;
	p->ch = ch;

	p->nxt = cur;
	p->prv = cur->prv;
	cur->prv->nxt = p;
	cur->prv = p;
}

void deleteNode()
{
	NODE* del = cur->prv;
	if (del == &HEAD) return;

	cur->prv->prv->nxt = cur;
	cur->prv = cur->prv->prv;

	delete del;
}

void curLeft()
{
	if (cur->prv->prv == NULL) return;
	cur = cur->prv;
}

void curRight()
{
	if (cur->nxt == NULL) return;
	cur = cur->nxt;
}

void print()
{
	NODE* head = &HEAD;
	head = head->nxt;

	for (;;)
	{
		if (head->nxt == NULL) break;

		printf("%c", head->ch);

		head = head->nxt;
	}
}

int main()
{
	int i;
	int N = 0;
	char ch = 0;

	init();
	scanf("%s", str);
	for (i = 0; str[i] != 0; i++)
	{
		insertNode(str[i]);
	}

	scanf("%d", &N);
	for (i = 0; i < N; i++)
	{
		scanf(" %c", &ch);
		if (ch == 'L')
		{
			curLeft();
		}
		else if (ch == 'D')
		{
			curRight();
		}
		else if (ch == 'B')
		{
			deleteNode();
		}
		else if (ch == 'P')
		{
			scanf("  %c", &ch);
			insertNode(ch);
		}
	}

	print();
	printf("\n");

	return 0;
}


#elif 0
#include<iostream>
using namespace std;
struct Trie {
	Trie* ch[26];
	bool end;
	Trie() {
		end = false;
		for (int i = 0; i < 26; i++) ch[i] = NULL;
	}
	~Trie() {
		for (int i = 0; i < 26; i++) if (ch[i]) delete ch[i];
	}
	void insert(const char* s) {
		if (!*s) {
			this->end = true;
			return;
		}
		int now = *s - 'A';
		if (!ch[now]) ch[now] = new Trie;
		ch[now]->insert(s + 1);
	}
	bool find(const char* s) {
		if (!*s) {
			if (end) return true;
			return false;
		}
		int now = *s - 'A';
		if (!ch[now]) return false;
		return ch[now]->find(s + 1);
	}
};
int main(void) {
	Trie* root = new Trie;
	string s;
	root->insert(s.c_str());
	string tmp = "AAA";
	if (root->find(tmp.c_str())) cout << "Find!";
	else cout << "No";
	delete root;
}



#elif 0
#include<iostream>
using  namespace std;

int main() {

	int cnt = (int)1e9;
	cout << cnt << endl;



	int temp;
	for (int i = 0; i < cnt; i++) {
		temp = cnt << 3 ;
	}







	for (int i = 0; i < cnt; i++) {
		temp = cnt * 8;
	}



	return 0;
}


#elif 0 

#include<iostream>
using  namespace std;
int N, M, a[30'005];
int twopointer(int a[], int len) {
	int ret = 0;
	int s, e, sum;
	s = e = sum = 0;
	while (s < len) {
		if (sum >= M) {
			sum -= a[s];
			s++;
		}
		else {
			sum += a[e]; 
			e++;
		}
		if (sum == M)
			ret++;
	}
	return ret;
}
int main() {
	cin >> N >> M;
	for (int i = 0; i < N; i++) {
		cin >> a[i];
	}
	cout << twopointer(a, sizeof(a) / sizeof(a[0]));
	return 0;
}

#elif 0
#include <iostream>
#include <vector>

using namespace std;

vector<int> segTree;

// 세그먼트 트리 구성 함수
void buildTree(const vector<int>& arr, int node, int start, int end) {
	cout << __FUNCTION__ << ", ---  node: " << node << ", start: " << start << ", end: " << end << endl;
	if (start == end) {
		
		segTree[node] = arr[start];
		
		/*
		//for (auto &i : segTree) {
		for (int i = 0; i < sizeof(segTree) / sizeof(segTree[0]); i++) {
			cout << "[" << i << "] = " << segTree[i]<< "   ";
		}cout << endl;
		*/
	}
	else {
		int mid = (start + end) / 2;
		buildTree(arr, 2 * node, start, mid);
		buildTree(arr, 2 * node + 1, mid + 1, end);
		//segTree[node] = segTree[2 * node] + segTree[2 * node + 1];
	}
}

// 구간 합 계산 함수
int query(const vector<int>& arr, int node, int start, int end, int left, int right) {
	if (right < start || left > end) return 0; // 범위 밖에 있는 경우
	if (left <= start && end <= right) return segTree[node]; // 현재 노드가 찾는 구간 안에 있는 경우

	int mid = (start + end) / 2;
	return query(arr, 2 * node, start, mid, left, right) + query(arr, 2 * node + 1, mid + 1, end, left, right);
}

int main() {
	vector<int> arr = { 1, 2, 3 };// 5, 7, 9, 11 
	int n = arr.size();

	// 세그먼트 트리 배열의 크기는 4 * N으로 설정
	segTree.resize(4 * n);

	// 세그먼트 트리 구성
	buildTree(arr, 1, 0, n - 1);

	// 특정 구간의 합 계산 예시
	int sum = query(arr, 1, 0, n - 1, 1, 3);
	cout << "Sum of elements from index 1 to 3: " << sum << endl;

	return 0;
}



#elif 0
#include<iostream>
using namespace std;

unsigned char mInImage[100] = "aa1aaaaabb2bbccc3ccdd4ddeeeeeeeefff;dkfsa;lkdjfsal;dkfas;lkdfjsaaaaaaaaffffffffdddddddfffffldfjk";
unsigned char mOutImage[100];
char Map[100][100];

int H = 4;
int W = 7;

void print() {
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			cout << Map[i][j];
		}
		cout << endl;
	}
}

unsigned t[100][100];
void rotate(unsigned char mOutImage[]) {

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			t[]
		}
	}
}
int main() {

	cout << mInImage << endl;

	for (int i = 0; i < H*W; i++)
		Map[i / W][i%W] = mInImage[i];

	print();

	for (int i = 0; i < H*W; i++)
		mOutImage[i] = Map[i / W][i%W];

	//cout << mOutImage << endl;

	rotate(mOutImage);

	/*
	unsigned char *ptr = mInImage;
	int k = 0;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Map[i][j] = *ptr++;
		}
	}
	*/

	for (int i = 0; i < H*W; i++)	Map[i / W][i%W] = mInImage[i];




	return 0;
}


#elif 0
#include <queue>
#define N 1000
#define pii pair<int, int>
#define rint register int
using namespace std;


int W, H, ts;
char Map[N][N], cp[N][N];
int visit[N][N];
int arr[N];
int cntM[128], bits[128];
int dx[] = { 1, 0, -1, 0 };
int dy[] = { 0, 1, 0, -1 };

struct TREE {
	int num;
	TREE *L, *R;
}tree[N];

struct Q {
	int x, y;
	TREE * cur;
	bool operator<(const Q &a) const { return a.x != x ? a.x < x : a.y < y; }
}q[N*N + 5];


priority_queue<Q> pq;

void copy(unsigned char Image[]) {
	rint k = 0;
	for (rint i = 0; i < H; i++)
		for (rint j = 0; j < W; j++)
			Image[k++] = Map[i][j];
}

void init(int mWidth, int mHeight, unsigned char mInImage[]) {
	H = mHeight;
	W = mWidth;
	for (rint i = 0; i < H*W; i++) Map[i / W][i%W] = mInImage[i];
}

void rect(int mY, int mX, int mW, int mH, unsigned char mColor, unsigned char mOutImage[])
{
	rint i, j, eX = mX + mH, eY = mY + mW;
	for (i = mX; i < eX; i++)
		for (j = mY; j < eY; j++)
			Map[i][j] = mColor;

	copy(mOutImage);
}

void rotate(unsigned char mOutImage[])
{
	rint i, j, k = 0;
	swap(W, H);
	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++)
			cp[i][j] = Map[W - j - 1][i];

	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++)
			mOutImage[k++] = Map[i][j] = cp[i][j];
}

void paintBucket(int mY, int mX, unsigned char mColor, unsigned char mOutImage[])
{
	rint wr = 0, rd = 0, k = 0, val = Map[mX][mY];
	q[wr++] = { mX, mY };
	Map[mX][mY] = mColor;
	visit[mX][mY] = ++ts;
	while (wr != rd) {
		auto t = q[rd++];
		for (rint i = 0; i < 4; i++) {
			rint x = t.x + dx[i], y = t.y + dy[i];
			if (x < 0 || y < 0 || x == H || y == W || visit[x][y] == ts) continue;
			if (Map[x][y] == val) {
				Map[x][y] = mColor;
				visit[x][y] = ts;
				q[wr++] = { x, y };
			}
		}
	}
	copy(mOutImage);
}

void dfs(TREE * cur, int val, int c) {
	if (!cur) return;
	if (!cur->L && !cur->R) {
		cntM[cur->num] = val;
		bits[cur->num] = c;
		return;
	}
	else {
		dfs(cur->L, val << 1, c + 1);
		dfs(cur->R, (val << 1) + 1, c + 1);
	}
}

int save(unsigned char mOutHuffData[]) {
	rint i, j, id = 0, len = 0;
	for (i = 0; i < 128; i++) cntM[i] = bits[i] = 0;
	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++) {
			if (cntM[Map[i][j]]++ == 0) arr[len++] = Map[i][j];
		}
	pq = {};
	for (i = 0; i < len; i++) {
		tree[++id] = { arr[i] };
		pq.push({ cntM[arr[i]], arr[i], &tree[id] });
	}
	TREE * root = 0;
	while (pq.size() > 1) {
		auto L = pq.top(); pq.pop();
		auto R = pq.top(); pq.pop();
		tree[++id] = { min(L.y, R.y), L.cur, R.cur };
		pq.push({ L.x + R.x, min(L.y, R.y), root = &tree[id] });
	}

	dfs(root, 0, 0);

	rint ans = 0, idx = 0, bit = 0;
	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++) {
			rint v = cntM[Map[i][j]], c = bits[Map[i][j]];
			while (c) {
				mOutHuffData[idx] = (mOutHuffData[idx] << 1) + ((v >> --c) & 1);
				++ans;
				if (++bit == 8) {
					bit = 0;
					mOutHuffData[++idx] = 0;
				}
			}
		}
	return ans;
}
#elif 0
#include <set>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#define pii pair<int, int>
using namespace std;

pair<int, int>	p[6];

struct compare_s {
	const bool operator()(const pair<int, int> &lhs, const pair<int, int> &rhs) const {
		return lhs.first != rhs.first ? lhs.first > rhs.first : lhs.second < rhs.second;
	}
};


bool compare_f(const pair<int, int> &lhs, const pair<int, int> &rhs) {
	return lhs.first != rhs.first ? lhs.first > rhs.first : lhs.second < rhs.second;
};


int main() {

	p[0] = { 10,20 };
	//p[1]( 20,10 );
	p[1] = make_pair(20, 10);
	p[2] = make_pair(10, 50);
	p[3] = make_pair(20, 20);
	p[4] = make_pair(60, 50);
	p[5] = make_pair(1, 5);

	// 비교 구조체를 사용했으며 
	// operator() 연산자를 오버로딩해서 사용하는 방식이므로 
	// 구조체이름 뒤에 operator()를 추가 했다.
	// 반면에 만일 비교 함수를 사용할 경우에는 다음과 같이 사용한다.
	// bool compare(const T&a, const T&b){
	//		reture 비교로직 ;
	// }
	// 이러한 비교 함수를 사용시에는 함수 포인터가 전달되어 연산이 되므로 operator()가 필요 없다(넣으면 안된다)

	pair<int, int> t[6];

	copy(p, p + 6, t);
	sort(t, t + 6, less<pair<int, int>>());
	for (auto &i : t) { cout << i.first << "\t" << i.second << endl; } cout << endl;

	copy(p, p + 6, t);
	sort(t, t + 6, less<>());
	for (auto &i : t) { cout << i.first << "\t" << i.second << endl; } cout << endl;

	copy(p, p + 6, t);
	sort(t, t + 6, greater<pair<int, int>>());
	for (auto &i : t) { cout << i.first << "\t" << i.second << endl; } cout << endl;

	copy(p, p + 6, t);
	sort(t, t + 6, greater<>());
	for (auto &i : t) { cout << i.first << "\t" << i.second << endl; } cout << endl;

	copy(p, p + 6, t);
	sort(t, t + 6, compare_s());
	for (auto &i : t) { cout << i.first << "\t" << i.second << endl; } cout << endl;

	copy(p, p + 6, t);
	sort(t, t + 6, compare_f);
	for (auto &i : t) { cout << i.first << "\t" << i.second << endl; } cout << endl;

	return 0;
}

#elif 0
#include <set>
#include <algorithm>
#include <unordered_map>
#define pii pair<int, int>
using namespace std;
int C, N, id;
struct STU {
	int pos;
	// 핵심 자료구조!!!
	// distance, 학교 ID 를 가진 pair<int,int> 형태의 배열을 학교 개수만큼 가진다.
	// 즉, 학생 한명을 기준으로 학교 개수만큼(최대 10개)의 <거리, 학교ID> 정보를 넣어둔다는 것이다.
	pair<int, int> sc[10];
}student[10005];

struct SC {
	int x, y;
}school[10];

// 각 학교에 할당된 학생
set<pair<int, int>> S[10];
set<pair<int, int>> W[10];
unordered_map<int, int> sid;

void init(int C, int N, int mX[], int mY[]) {

	// 학교에 최대 수용가능한 학생수를 전역 변수로
	// 총 학교의 수를 전역 변수로
	::C = C; ::N = N;

	// 학교에 대한 구조체를 모두 초기화 
	for (int i = 0; i < N; i++) {
		school[i] = { mX[i], mY[i] };
		S[i].clear();
		W[i].clear();
	}

	// 학생 ID에 대한 해싱도 초기화
	sid.clear();
	id = 0;
}

void update(int p, int mid) {
	// 학생 ID로 접근하여 학교와의 거리 정보를 가져온다. 이 값은 오름차순(less)로 정렬되어 있다. 
	int val = student[p].sc[N - 1].first;

	// p 라는 학생을 기준을 10개 학교를 뒤진다. 
	for (int k = 0; k < N; k++) {
		// 학생기준으로 학교 정보(거리, 학교ID)를 가지고 온다.
		pair<int, int> i = student[p].sc[k];
		// i.first는 distance		
		// i.second는 학교 ID 

		int distance = i.first;
		int schoolID = i.second;

		if (S[schoolID].size() < C) {
			// 학생 배정 정보를 그 학교ID로 넣는다. 
			student[p].pos = schoolID;
			// S는 각 학교에 할당된 학생의 정보를 저장한다.
			// set<int,int> 거리와 id정보..
			// 그런 mID에 - 처리한 이유는????
			S[schoolID].insert({ val, -mid });
			return;
		}
		else {
			pair<int, int> cur = *S[schoolID].begin();
			if (cur.first < val || (cur.first == val && mid < -cur.second)) {
				S[schoolID].erase(cur);
				S[schoolID].insert({ val, -mid });
				student[p].pos = schoolID;
				update(sid[-cur.second], -cur.second);
				return;
			}
			else W[schoolID].insert({ -val, mid });
		}
	}
}

void rm_update(int p) {
	if (!W[p].empty()) {
		pii cur = *W[p].begin();
		W[p].erase(cur);
		int cid = sid[cur.second];
		int Prev = student[cid].pos;
		S[Prev].erase({ -cur.first, -cur.second });
		S[p].insert({ -cur.first, -cur.second });
		student[cid].pos = p;
		for (int i = N - 1; i >= 0; i--) {
			W[student[cid].sc[i].second].erase(cur);
			if (student[cid].sc[i].second == p) break;
		}
		rm_update(Prev);
	}
}

int add(int mStudent, int mX, int mY) {

	// 학생 ID에 대해 해싱해서 저장한다.
	// id를 1부터 시작하도록 했다.. 왜? 
	sid[mStudent] = ++id;
	// 학생에 대한 구조체를 초기화 시키고 시작한다. 
	// 일종의 시간 절약 팁? 
	student[id] = {};


	// 학교를 기준으로 거리를 계산하고 이를 학생 구조체 정보에 저장한다. 
	for (int i = 0; i < N; i++) {
		//student[id].sc[i] = { abs(mX - school[i].x) + abs(mY - school[i].y), i };

		// 학교 번호와 학교와의 거리를 학생 구조체에 계산하여 넣어 준다. 
		int distance = abs(mX - school[i].x) + abs(mY - school[i].y);
		// 학생의 정보에 각 학교와 그 학교 같의 거리 정보를 저장한다.
		student[id].sc[i] = { distance, i };

	}

	// 학생 구조체의 학교 정보(거리, 학교ID)를 less, 오름차순(적은거리가 먼저나오는 것)으로 정렬해준다.
	// 거리가 작은순으로 배정하기 위해!!!
	// pair<int,int> 타입도 sort() 함수내에서 정렬이 되며
	// 정렬 기준은 first 인자를 기준으로 정렬된다.
	// default 인 less 의 구조체를 정렬할때 어떠한 순서로 정렬되는가? 
	sort(student[id].sc, student[id].sc + N);

	update(id, mStudent);
	return student[id].pos;
}

int remove(int mStudent) {
	int rid = sid[mStudent], val = student[rid].sc[N - 1].first, p = student[rid].pos;
	S[p].erase({ val, -mStudent });
	for (int i = 0; i < N; i++) W[i].erase({ -val, mStudent });
	rm_update(p);
	return p;
}

int status(int mSchool) { return S[mSchool].size(); }
#elif 0

struct Node {


} node[7'500];

unordered_map<int, int>		h;
int idx;

struct School {

} SC[10];


int C;
void init(int C, int N, int mX[], int mY[]) {
	//각 테스트 케이스의 처음에 호출된다.
	//각 학교가 수용할 수 있는 최대 학생 수 C, 학교의 개수 N이 주어진다.
	//그리고 N개의 학교에 대한 X좌표 값과 Y좌표 값이 배열로 주어진다.
	//N개의 학교는 순차적으로 0부터 N - 1까지 학교 ID를 가진다.
	//동일한 위치에 2개 이상의 학교가 있을 수 있다.
	//Parameters
	//C : 각 학교가 수용할 수 있는 최대 학생 수(2 ≤ C ≤ 1, 000)
	//	N : 학교 개수(3 ≤ N ≤ 10)
	//	(0 ≤ i ＜ N)인 모든 i에 대해,
	//	mX[i]: 학교 i의 X좌표(0 ≤ mX[i] ≤ 10, 000)
	//	mY[i] : 학교 i의 Y좌표(0 ≤ mY[i] ≤ 10, 000) 
	::C = C;
}

int update() {

}

int add(int mStudent, int mX, int mY) {
	//학생 ID가 mStudent인 학생이 추가된다.학생의 위치는(mX, mY)로 주어진다.
	//배정 규칙에 따라, 학교를 배정한다.
	//동일한 위치에 2명 이상의 학생이 있을 수 있다.
	//삭제되었던 학생이 다른 위치에서 다시 추가될 수도 있다.
	//전체 학교 수용 인원보다 많은 학생이 추가되는 경우는 없다.즉 배정에 실패하는 경우는 없다.
	//배정 규칙에 따라 다른 학생들의 배정된 학교가 변경 될 수 있다.
	//Parameters
	//mStudent : 학생 ID(1 ≤ mStudent ≤ 1, 000, 000, 000)
	//	mX : 학생의 X좌표(0 ≤ mX ≤ 10, 000)
	//	mY : 학생의 Y좌표(0 ≤ mY ≤ 10, 000)
	//	Returns
	//	배정된 학교의 ID를 반환한다
}




#elif 0
#include <algorithm>
#include <iostream>
using namespace std;

int main() {
	int a[] = { 456,56,654,6489,4685,78,98,6451,31,312312 };
	sort(a, a + sizeof(a) / sizeof(a[0]), greater<>());

	for (auto &i : a)	cout << i << "\t";
	cout << endl;
	return 0;
}

#elif 0

#define MAX_STUDENT 7500
#define MAX_SCHOOL  10
#define MAX_TABLE   10000

#include <algorithm>

struct school_t;
struct student_t
{
	int id;
	long long priority;
	bool valid;
	school_t *school, *candidate[MAX_SCHOOL];
	student_t* next;
} student_pool[MAX_STUDENT];
int student_idx;

student_t* hash_table[MAX_TABLE];

struct school_t
{
	int id, x, y, n_student;
	int heap_size[2];
	student_t* heap[2][MAX_STUDENT];

	bool compare(int idx, student_t* a, student_t* b)
	{
		const int sign[2] = { 1, -1 };
		return (a->priority - b->priority) * sign[idx] < 0;
	}

	void heap_push(int idx, student_t* s)
	{
		heap[idx][heap_size[idx]] = s;
		int current = heap_size[idx], parent = (current - 1) / 2;
		while (current > 0 && compare(idx, heap[idx][current], heap[idx][parent]))
		{
			student_t* temp = heap[idx][parent];
			heap[idx][parent] = heap[idx][current];
			heap[idx][current] = temp;
			current = parent;
			parent = (current - 1) / 2;
		}
		heap_size[idx]++;
	}

	student_t*  heap_pop(int idx)
	{
		student_t* ret = heap[idx][0];
		heap[idx][0] = heap[idx][--heap_size[idx]];
		int current = 0, lchild = current * 2 + 1, child;
		while (lchild < heap_size[idx])
		{
			if (lchild + 1 == heap_size[idx])
				child = lchild;
			else
				child = compare(idx, heap[idx][lchild], heap[idx][lchild + 1]) ? lchild : lchild + 1;
			if (compare(idx, heap[idx][current], heap[idx][child]))
				break;
			student_t* temp = heap[idx][current];
			heap[idx][current] = heap[idx][child];
			heap[idx][child] = temp;
			current = child;
			lchild = current * 2 + 1;
		}
		return ret;
	}
} schools[MAX_SCHOOL];

int N, C;

void add_hash(student_t* s)
{
	int h = s->id % MAX_TABLE;
	s->next = hash_table[h];
	hash_table[h] = s;
}

student_t* find_hash(int id)
{
	student_t* pos = hash_table[id % MAX_TABLE];
	while (pos)
	{
		if (pos->id == id && pos->valid == true)
			return pos;
		pos = pos->next;
	}
	return 0;
}

int dist(int ax, int ay, int bx, int by)
{
	int dx = (ax > bx) ? ax - bx : bx - ax;
	int dy = (ay > by) ? ay - by : by - ay;
	return dx + dy;
}

void init(int C, int N, int mX[], int mY[])
{
	::C = C, ::N = N;
	student_idx = 0;
	for (register int i = 0; i < MAX_TABLE; i++)
		hash_table[i] = 0;
	for (register int i = 0; i < N; i++)
		schools[i] = { i, mX[i], mY[i], 0, 0, 0, };
}

int add(int mStudent, int mX, int mY)
{
	student_t* student = &student_pool[student_idx++];
	student->id = mStudent;
	student->valid = true;

	int priority[MAX_SCHOOL];
	for (register int i = 0; i < N; i++)
		priority[i] = (dist(mX, mY, schools[i].x, schools[i].y) << 8) + schools[i].id;
	std::sort(priority, priority + N);
	for (register int i = 0; i < N; i++)
		student->candidate[i] = &schools[priority[i] & 0xFF];

	student->priority = ((long long)(priority[N - 1] & 0xFFFFFF00) << 32) - student->id;
	add_hash(student);

	int ret = -1;
	while (1)
	{
		school_t* school = 0;
		for (register int i = 0; i < N; i++)
		{
			school = student->candidate[i];
			while (school->heap_size[0] > 0 && (school->heap[0][0]->valid == false || school->heap[0][0]->school != school))
				school->heap_pop(0);

			if (school->n_student < C || school->heap[0][0]->priority < student->priority)
			{
				school->heap_push(0, student);
				school->n_student++;
				student->school = school;
				if (ret == -1)
					ret = school->id;
				break;
			}
			else
				school->heap_push(1, student);
		}

		if (school->n_student <= C)
			break;

		student = school->heap_pop(0);
		school->n_student--;
	}

	return ret;
}

int remove(int mStudent)
{
	student_t* student = find_hash(mStudent);
	int ret = student->school->id;

	student->valid = false;
	student->school->n_student--;

	school_t* school = student->school;
	while (1)
	{
		while (school->heap_size[1] > 0 && school->heap[1][0]->valid == false)
			school->heap_pop(1);

		if (school->heap_size[1] == 0)
			break;

		student = school->heap_pop(1);
		school_t* prev_school = student->school;
		prev_school->n_student--;

		student->school = school;
		school->heap_push(0, student);
		school->n_student++;

		school = prev_school;
	}

	return ret;
}

int status(int mSchool)
{
	return schools[mSchool].n_student;
}



#elif 0

int a[] = { 5,8,
6,4,
8,9,
4,3,
4,6,
1,2 };

void print() {
	for (auto &i : a) {
		cout << i << " ";
	}
	cout << endl;
}

void heepify(int p_idx, int size) {
	int c_idx = 2 * p_idx + 1;

	if (c_idx < size - 1 && a[c_idx] < a[c_idx + 1])	c_idx++;

	if (c_idx < size && a[p_idx] < a[c_idx]) {
		swap(a[p_idx], a[c_idx]);
		heepify(c_idx, size);
	}
}

void heepsort(int size) {
	for (int i = size / 2 - 1; i >= 0; i--) {
		heepify(i, size);
	}
	print();
	for (int i = size - 1; i > 0; i--) {
		swap(a[0], a[i]);
		heepify(0, i);
	}
}
int main() {
	int size = sizeof(a) / sizeof(a[0]);
	print();
	heepsort(size);
	print();
	return 0;
}


#elif 0


#include <iostream>
#include <queue>

using namespace std;

int c[27];

// 비교 함수 정의 (첫 번째 원소를 기준으로 작은 값이 우선순위가 높음)
//struct Compare {
//	bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const {
//		return a.first > b.first;
//	}
//};



struct cmp {
	bool operator()(const pair<int, char>&a, const pair<int, char> &b) const {
		return a.first != b.first ? a.first < b.first : a.second > b.second;
	}
};
priority_queue<pair<int, char>, vector<pair<int, char>>, cmp>	pq;


int main() {
	//string s = "data structure and algorithm";
	char s[100] = "data structure and algorithm";
	for (int i = 0; s[i] != '\0'; i++) {
		c[s[i] - 'a']++;
	}
	for (int i = 0; i < 'z' - 'a'; i++) {
		pq.emplace(c[i], i + 'a');
	}

	while (!pq.empty()) {
		cout << pq.top().first << " " << pq.top().second << endl;
		pq.pop();
	}

	return 0;
}

#elif 0
#include <iostream>
#include <queue>
using namespace std;

struct Node {
	int x, y;
	Node(int _x, int _y) : x(_x), y(_y) {}

	const Node operator+(const Node &ref) const {
		return Node(x + ref.x, y + ref.y);
	}
	//bool operator<(const Node &ref) const {
	//	return x != ref.x ? x < ref.x : y < ref.y;
	//}
};

struct cmp {
	bool operator()(const Node &r1, const Node &r2) const {
		return r1.x != r2.x ? r1.x < r2.x : r1.y > r2.y;
	}
};

int main() {

	priority_queue<Node, vector<Node>, cmp> pq;

	pq.emplace(10, 20);
	pq.emplace(10, 30);
	pq.emplace(15, 15);
	pq.emplace(12, 50);

	Node a(1, 2);
	Node b(11, 21);
	Node c = a + b;
	pq.emplace(c);

	while (!pq.empty()) {
		cout << pq.top().x << " " << pq.top().y << endl;
		pq.pop();
	}
	return 0;
}

#elif 0
#include <iostream>
#include <windows.h>
#include <algorithm>
using namespace std;

int main()
{
	const long long SIZE = 5000000;
	int *a = new int[SIZE];

	for (long long i = 0; i < SIZE; i++)
		a[i] = rand();

	int t1 = GetTickCount();

	// 람다 정의
	// [] 리턴 타입 및 함수명 생략
	// 
	sort(a, a + SIZE, [](int a, int b)
	{
		return a > b;
	}
	);

	int t2 = GetTickCount() - t1;
	cout << "람다 정렬시간 : " << t2 << endl;
}
#elif 0
#include <iostream>
#include <windows.h>
#include <algorithm>
using namespace std;
class CMP
{
public:
	// 클래스 내부에 함수객체(Function Object)생성
	// 함수 호출 연산자 ()를 연산자 오버로딩 한 것이다.
	// 클래스 내부에 존재하기 때문에 객체 속성을 가지게 되어때문에 inline화 가능
	bool operator()(int a, int b)
	{
		return a > b;
	}
};

// 일반 함수
// 함수포인터 호출과 함께 inline화 되지 않는다.
bool cmp(int a, int b)
{
	return a > b;
}


int main()
{
	const long long SIZE = 5000000;
	int *a = new int[SIZE];

	// 함수객체(Function object)를 포함하는 객체 생성
	CMP cmp2;

	for (long long i = 0; i < SIZE; i++)
		a[i] = rand();

	int t1 = GetTickCount();

	// 일반 함수포인터를 받아 sort하는 경우
	sort(a, a + SIZE, cmp);

	// 함수객체(function object)를 받아 sort하는 경우
	// 전달인자로 객체를 넘겨주지만 CMP객체내부에서 (int,int)형을 오버로딩한 함수객체가 호출됨
	//sort(a, a + SIZE, cmp2);    

	int t2 = GetTickCount() - t1;

	cout << "정렬시간 : " << t2 << endl;

	delete[]a;
}
#elif 0
#include <iostream>
using namespace std;

struct Node {
	int x;
	int y;
	Node() {
		cout << "생성자" << endl;
	}
	Node(int _x, int _y) : x(_x), y(_y) {
		cout << "생성자2" << endl;
	}
	//const Node operator+(const Node &ref) const{
	//	return Node(x + ref.x, y + ref.y);
	//}
};

const Node AddNode(const Node &ref1, const Node&ref2) {
	return Node(ref1.x + ref2.x, ref1.y + ref2.y);
}


int main() {
	//Node a{ 10, 20 };
	//Node b = { 30, 40 };
	//Node c ( 50, 60 );
	//struct Node a(10, 20);
	struct Node d;
	d.x = 10;
	d.y = 20;
	Node e(30, 40);
	//Node e(50, 60);
	e.x = 50;
	e.y = 60;
	cout << d.x << " " << d.y << endl;
	cout << e.x << " " << e.y << endl;
	Node f = AddNode(d, e);
	//Node f = d + e;
	cout << f.x << " " << f.y << endl;
	//cout << a.x << " " << a.y << endl;
	//cout << b.x << " " << b.y << endl;
	//cout << c.x << " " << c.y << endl;


	return 0;
}

#elif 0
#include <iostream>
#include <vector>
#include <queue>
#include <string>
using namespace std;
const int NMAX = 25;
char m[NMAX + 1][NMAX + 1];
int v[NMAX + 1][NMAX + 1];

priority_queue<int, vector<int>, greater<int> >	pq;

int dx[] = { -1, 0, 1, 0 };
int dy[] = { 0, -1, 0, 1 };
int N;

int dfs(int cx, int cy) {
	int ret = 0;
	v[cx][cy] = 1;

	for (int i = 0; i < 4; i++) {
		int nx = cx + dx[i];
		int ny = cy + dy[i];
		if (nx < 0 || ny < 0 || nx >= N || ny >= N)
			continue;
		if (!v[nx][ny] && m[nx][ny] != '0')
			ret += dfs(nx, ny) + 1;
	}

	return ret;
}
int bfs(int sx, int sy) {

	int ret = 0;
	queue<pair<int, int>> q;

	q.emplace(sx, sy);
	v[sx][sy] = 1;

	while (!q.empty()) {

		int cx = q.front().first;
		int cy = q.front().second;
		q.pop();
		ret++;

		for (int i = 0; i < 4; i++) {
			int nx = cx + dx[i];
			int ny = cy + dy[i];

			if (nx < 0 || ny < 0 || nx >= N || ny >= N)
				continue;

			if (!v[nx][ny] && m[nx][ny] != '0') {
				q.emplace(nx, ny);
				v[nx][ny] = 1;
			}
		}
	}
	return ret;
}

int main() {

	scanf("%d ", &N);
	for (int i = 0; i < N; i++) {
		scanf("%s ", m[i]);
	}


	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {


			if (v[i][j] || m[i][j] == '0') continue;

			//int cnt = bfs(i, j);
			int cnt = dfs(i, j) + 1;
			if (cnt)
				pq.emplace(cnt);

		}
	}

	printf("%d\n", pq.size());
	while (!pq.empty()) {

		printf("%d \n", pq.top());	pq.pop();

	}

	//for (int i = 0; i < N; i++) {
	//	for (int j = 0; j < N; j++) {
	//		printf("%c ", m[i][j]);
	//	}
	//	printf("\n");
	//}
	return 0;
}

#elif 0
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

const int NMAX = 100;

vector<int>	E[NMAX + 1];
//int E[NMAX + 1][NMAX + 1];

int N, M;
int v[NMAX + 1];

int bfs(int s) {

	int ret = -1;
	queue<int>	q;
	q.push(s);
	v[s] = 1;

	while (!q.empty()) {
		int c = q.front(); q.pop();
		ret++;

		for (int i = 1; i <= E[c][0]; i++) {
			int n = E[c][i];
			if (v[n])	continue;
			q.push(n), v[n] = 1;
		}
	}
	return ret;
}
int dfs(int s) {
	int ret = 0;
	v[s] = 1;

	//for (int i = 1; i <= E[s][0]; i++) {		
	for (unsigned int i = 0; i < E[s].size(); i++) {
		int n = E[s][i];
		if (v[n])	continue;
		v[n] = 1;
		ret += dfs(n) + 1;
	}
	return ret;
}
int main() {

	scanf("%d %d ", &N, &M);
	for (int i = 0; i < M; i++) {
		int s, e;
		scanf("%d %d ", &s, &e);
		E[s].emplace_back(e);
		E[e].emplace_back(s);
		//E[s][0]++;
		//E[s][E[s][0]] = e;
		//E[e][0]++;
		//E[e][E[e][0]] = s;
	}
	//printf("%d\n", bfs(1));
	printf("%d\n", dfs(1));

	return 0;
}
#elif 0
#include <iostream>
#include <vector>

using namespace std;

void heapify(vector<int>& arr, int n, int i) {

	int p = i; // 최대값을 현재 노드로 설정
	int l = 2 * i + 1; // 왼쪽 자식 노드의 인덱스
	int r = 2 * i + 2; // 오른쪽 자식 노드의 인덱스

	// 왼쪽 자식이 더 크다면
	if (l < n && arr[l] > arr[p]) {
		p = l;
	}

	// 오른쪽 자식이 더 크다면
	if (r < n && arr[r] > arr[p]) {
		p = r;
	}

	// 최대값이 현재 노드가 아니라면 교환
	if (p != i) {
		swap(arr[i], arr[p]);

		// 재귀적으로 하위 트리에 대해 heapify 호출
		heapify(arr, n, p);
	}
}

int main() {
	vector<int> arr = { 4, 10, 3, 5, 1 };
	int n = arr.size();

	cout << "Original Array: ";
	for (int num : arr) {
		cout << num << " ";
	}
	cout << endl;

	// 배열을 힙으로 만들기
	for (int i = n / 2 - 1; i >= 0; i--) {
		heapify(arr, n, i);
	}

	cout << "Heapified Array: ";
	for (int num : arr) {
		cout << num << " ";
	}
	cout << endl;

	return 0;
}


#elif 0
#include <iostream>
using namespace std;

const int N = 9;
int a[N] = { 7,6,5,8,3,5,9,1,6 };

int main() {
	// 먼제 전체 트리 구조를 최대 힙 구조로 바꿉니다. 
	for (int i = 1; i < N; i++) {
		int c = i;
		do {
			int p = (c - 1) / 2;
			if (a[p] < a[c]) {
				swap(a[p], a[c]);
			}
			c = p;
		} while (c != 0);
	}

	for (int i = N - 1; i >= 0; i--) {
		swap(a[0], a[i]);

		int p = 0;
		int c = 1;
		do {
			c = 2 * p + 1;
			if (a[c] < a[c + 1] && )
		}
	}

	// 

	int a;

	cin >> a;
	cout << a;
	setbuf(stdout, NULL);
	return 1;
}

#elif 0
//#include <iostream>
//#include <queue>
//#include <unordered_map>
//#include <vector>
//
//using namespace std;
//
//// 허프만 트리 노드 정의
//struct HuffmanNode {
//	char data;
//	int freq;
//	HuffmanNode* left;
//	HuffmanNode* right;
//
//	HuffmanNode(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
//};
//
//// 우선순위 큐 비교 연산자 정의
//struct CompareNodes {
//	bool operator()(HuffmanNode* a, HuffmanNode* b) {
//		return a->freq > b->freq;
//	}
//};
//
//// 허프만 트리 생성 함수
//HuffmanNode* buildHuffmanTree(const string& data) {
//	// 빈도수 계산
//	unordered_map<char, int> freqMap;
//	for (char ch : data) {
//		freqMap[ch]++;
//	}
//
//	// 우선순위 큐 생성
//	priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes> pq;
//
//	// 각 문자를 단독 노드로 추가
//	for (const auto& entry : freqMap) {
//		pq.push(new HuffmanNode(entry.first, entry.second));
//	}
//
//	// 허프만 트리 구성
//	while (pq.size() > 1) {
//		HuffmanNode* left = pq.top(); pq.pop();
//		HuffmanNode* right = pq.top(); pq.pop();
//
//		HuffmanNode* internalNode = new HuffmanNode('\0', left->freq + right->freq);
//		internalNode->left = left;
//		internalNode->right = right;
//
//		pq.push(internalNode);
//	}
//
//	return pq.top();
//}
//
//// 허프만 코드 출력 함수 (스택 사용)
//void printHuffmanCodes(HuffmanNode* root, unordered_map<char, string>& huffmanCodes) {
//	stack<pair<HuffmanNode*, string>> st;
//	st.push({ root, "" });
//
//	while (!st.empty()) {
//		auto [node, code] = st.top(); st.pop();
//
//		if (node->left) {
//			st.push({ node->left, code + "0" });
//		}
//		if (node->right) {
//			st.push({ node->right, code + "1" });
//		}
//
//		if (!node->left && !node->right) {
//			huffmanCodes[node->data] = code;
//			cout << node->data << ": " << code << endl;
//		}
//	}
//}
//
//// 허프만 코드 생성 및 출력 함수
//void generateHuffmanCodes(const string& data) {
//	HuffmanNode* root = buildHuffmanTree(data);
//
//	unordered_map<char, string> huffmanCodes;
//	cout << "Huffman Codes:" << endl;
//	printHuffmanCodes(root, huffmanCodes);
//
//	// 메모리 해제
//	delete root;
//}
//
//int main() {
//	string data = "abracadabra";
//	generateHuffmanCodes(data);
//
//	return 0;
//}
//
//#elif 0
//#include <iostream>
//#include <queue>
//#include <unordered_map>
//#include <vector>
//
//using namespace std;
//
//// 허프만 트리 노드 정의
//struct HuffmanNode {
//	char data;
//	int freq;
//	HuffmanNode* left;
//	HuffmanNode* right;
//
//	HuffmanNode(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
//};
//
//// 우선순위 큐 비교 연산자 정의
//struct CompareNodes {
//	bool operator()(HuffmanNode* a, HuffmanNode* b) {
//		return a->freq > b->freq;
//	}
//};
//
//// 허프만 트리 생성 함수
//HuffmanNode* buildHuffmanTree(const string& data) {
//	// 빈도수 계산
//	unordered_map<char, int> freqMap;
//	for (char ch : data) {
//		freqMap[ch]++;
//	}
//
//	// 우선순위 큐 생성
//	priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes> pq;
//
//	// 각 문자를 단독 노드로 추가
//	for (const auto& entry : freqMap) {
//		pq.push(new HuffmanNode(entry.first, entry.second));
//	}
//
//	// 허프만 트리 구성
//	while (pq.size() > 1) {
//		HuffmanNode* left = pq.top(); pq.pop();
//		HuffmanNode* right = pq.top(); pq.pop();
//
//		HuffmanNode* internalNode = new HuffmanNode('\0', left->freq + right->freq);
//		internalNode->left = left;
//		internalNode->right = right;
//
//		pq.push(internalNode);
//	}
//
//	return pq.top();
//}
//
//// 허프만 코드 출력 함수 (재귀적)
//void printHuffmanCodes(HuffmanNode* root, string code, unordered_map<char, string>& huffmanCodes) {
//	if (root->left) {
//		printHuffmanCodes(root->left, code + "0", huffmanCodes);
//	}
//	if (root->right) {
//		printHuffmanCodes(root->right, code + "1", huffmanCodes);
//	}
//	if (!root->left && !root->right) {
//		huffmanCodes[root->data] = code;
//		cout << root->data << ": " << code << endl;
//	}
//}
//
//// 허프만 코드 생성 및 출력 함수
//void generateHuffmanCodes(const string& data) {
//	HuffmanNode* root = buildHuffmanTree(data);
//
//	unordered_map<char, string> huffmanCodes;
//	cout << "Huffman Codes:" << endl;
//	printHuffmanCodes(root, "", huffmanCodes);
//
//	// 메모리 해제
//	delete root;
//}
//
//int main() {
//	string data = "abracadabra";
//	generateHuffmanCodes(data);
//
//	return 0;
//}


#elif 0
#include<queue>
#include<iostream>
#include<string.h>

using namespace std;
#define N   1'000
unsigned char P[N + 1][N + 1];
int W, H;

void print() {

	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			//printf("%3d", P[x][y]);
		}
		//printf("\n");
	}
	//printf("\n");

}
void init(int mWidth, int mHeight, unsigned char mInImage[]) {
	//각 test case의 처음에 한번 호출된다.
	//mWidth는 이미지의 가로 크기, mHeight는 세로의 크기이다.mInImage 는 이미지 데이터이며, 1바이트가 1픽셀을 나타낸다. 1픽셀은 0~127의 색상값을 가진다.
	//이미지는 최상단 좌측 픽셀부터 우측으로 순서대로 주어지며,
	//P(x, y)에 해당하는 값은 mInImage[X + Y * mWidth] 이다.
	//__Parameters
	//____mWidth : 이미지의 가로 크기(4 ≤ mWidth ≤ 1, 000)
	//_____mHeight : 이미지의 세로 크기(4 ≤ mHeight ≤ 1, 000)
	//_____mInImage : 입력 이미지(이미지의 크기 = mWidth * mHeight)
	::W = mWidth;
	::H = mHeight;
	for (int y = 0; y < mHeight; y++) {
		for (int x = 0; x < mWidth; x++) {
			P[x][y] = mInImage[x + y * mWidth];
			//printf("%2c ", P[x][y]);
		}
		//printf("\n"); 
	}
	//printf("\n");
	print();
}
void rect(int mX, int mY, int mW, int mH, unsigned char mColor, unsigned char mOutImage[]) {

	//현재 이미지에 사각형을 그린다.
	//현재 이미지에서(mX, mY) 위치를 시작으로(mX + mW - 1, mY + mH - 1) 의 영역을 mColor 색상으로 바꾼다.
	//주어지는 영역이 현재 이미지의 범위를 벗어나는 경우는 없다.
	//mOutImage 는[Fig. 2] 의 픽셀테이터 형식으로 저장한다.
	//__Parameters
	//____mX : 가로 시작 위치
	//_____mY : 세로 시작 위치
	//_____mW : 그리는 영역의 너비
	//_____mH : 그리는 영역의 높이
	//_____mColor : 그려지는 색상(0 ≤ mColor ≤ 127)
	//_____mOutImage : 실행 후 결과 이미지  
	for (int y = mY; y < mY + mH; y++) {
		for (int x = mX; x < mX + mW; x++) {
			P[x][y] = mColor;
		}
	}
	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			mOutImage[x + y * W] = P[x][y];;
			//printf("%2c ", P[x][y]);
		}
		//printf("\n");
	}
	//printf("\n");
	print();

}

int T[N][N];

void swap(int* a, int* b) { int temp = *a; *a = *b; *b = temp; }
void rotate(unsigned char mOutImage[]) {

	//현재 이미지를 시계방향으로 90도 회전 시킨다.
	//mOutImage 는[Fig. 2] 의 픽셀테이터 형식으로 저장한다.
	//__Parameters
	//____mOutImage : 실행 후 결과 이미지.

	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			T[H - 1 - y][x] = P[x][y];
		}
	}
	swap(&W, &H);
	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			mOutImage[x + y * W] = P[x][y] = T[x][y];
			//printf("%2c ", P[x][y]);
		}
		//printf("\n");
	}
	//printf("\n");
	print();

}

int v[N][N];
int dx[4] = { -1, 0, 1, 0 };
int dy[4] = { 0, 1, 0, -1 };
queue<pair<int, int>> q;

void bfs(int x, int y, int nc) {

	memset(v, 0, sizeof(v));

	int oc = P[x][y];

	v[x][y] = 1;
	q.push({ x, y });

	while (!q.empty()) {

		int cx = q.front().first;
		int cy = q.front().second;
		q.pop();

		P[cx][cy] = nc;

		for (int i = 0; i < 4; i++) {
			int nx = cx + dx[i];
			int ny = cy + dy[i];
			if (v[nx][ny] || P[nx][ny] != oc)
				continue;
			v[nx][ny] = 1;
			q.push({ nx, ny });
		}
	}

	print();

}
void paintBucket(int mX, int mY, unsigned char mColor, unsigned char mOutImage[]) {
	//현재 이미지에서(mX, mY) 위치를 시작으로 상하좌우 방향, 동일한 색으로 연결된 영역의 픽셀들이 mColor로 채워진다.
	//주어지는 영역이 현재 이미지의 범위를 벗어나는 경우는 없다.
	//mOutImage 는[Fig. 2] 의 픽셀테이터 형식으로 저장한다.
	//P(mX, mY) 와 동일한 mColor가 주어지는 경우는 없다.
	//__Parameters
	//____mX : 가로 위치. (0 ≤ mX ≤ 너비 – 1)
	//_____mY : 세로 위치. (0 ≤ mY ≤ 높이 – 1)
	//_____mColor : 채워지는 색. (0 ≤ mColor ≤ 127)
	//_____mOutImage : 실행 후 결과 이미지
	bfs(mX, mY, mColor);

	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			mOutImage[x + y * W] = P[x][y];;
			//printf("%4d ", mImage[x + y * W]);
		}
	}

}



int c[128];
unsigned char mImage[N * N];

struct Node {
	int f, c;
	struct Node* L, *R;
}node;

priority_queue<struct Node, vector<struct Node>, less<pair<int, int>>> pq;


int save(unsigned char mOutHuffData[]) {

	memset(mImage, 0, sizeof(mImage));

	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			mImage[x + y * W] = P[x][y];
		}
	}

	for (int i = 0; i < H * W; i++) {
		c[mImage[i]]++;
	}

	for (int i = 0; i < 128; i++) {
		pq.push({ c[i], i });
	}

	while (!pq.empty()) {

	}

	return -1;
}
#elif 0
// [H2339] 파일 저장소

#include<unordered_map>	
using namespace std;


unordered_map<int, int>	h;

int remove(int mId) {

}


#elif 0
// prefix sum
#include<iostream>
using namespace std;


int a[] = { 1,2,3,2,3,2,1,5,4,2,3,1,5,2,4 };
int s[30];

int main() {
	for (int i = 0; i < 14; i++) {
		s[i + 1] = s[i] + a[i];
	}
	for (int i = 0; i < 14; i++) {
		cout << a[i] << " ";
	}
	cout << endl;
	for (int i = 0; i < 14 + 1; i++) {
		cout << s[i] << " ";
	}

	return 0;
}


#elif 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 허프만 트리 노드 정의
typedef struct HuffmanNode {
	char data;
	int freq;
	struct HuffmanNode* left;
	struct HuffmanNode* right;
} HuffmanNode;

// 허프만 트리 생성 함수
HuffmanNode* buildHuffmanTree(char data[], int freq[], int size) {
	// TODO: 허프만 트리 생성 코드 작성

	// 여기에 코드를 작성하세요.

	return NULL;
}

// 허프만 코드 출력 함수 (재귀적)
void printHuffmanCodes(HuffmanNode* root, char code[], int top) {
	if (root->left != NULL) {
		code[top] = '0';
		printHuffmanCodes(root->left, code, top + 1);
	}

	if (root->right != NULL) {
		code[top] = '1';
		printHuffmanCodes(root->right, code, top + 1);
	}

	if (root->left == NULL && root->right == NULL) {
		printf("%c: %s\n", root->data, code);
	}
}

int main() {
	char data[] = { 'a', 'b', 'r', 'a', 'c', 'a', 'd', 'a', 'b', 'r', 'a' };
	int freq[] = { 5, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1 };
	int size = sizeof(data) / sizeof(data[0]);

	// 허프만 트리 생성
	HuffmanNode* root = buildHuffmanTree(data, freq, size);

	if (root == NULL) {
		fprintf(stderr, "허프만 트리 생성 실패\n");
		return 1;
	}

	// 허프만 코드 출력
	char code[100]; // 충분한 크기로 설정
	printf("허프만 코드:\n");
	printHuffmanCodes(root, code, 0);

	// 메모리 해제
	// TODO: 메모리 해제 코드 작성

	return 0;
}


#elif 0
// Hugman coding 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 허프만 트리 노드 정의
typedef struct HuffmanNode {
	char data;
	int freq;
	struct HuffmanNode* left;
	struct HuffmanNode* right;
} HuffmanNode;

// 우선순위 큐 노드 정의
typedef struct PriorityQueueNode {
	HuffmanNode* node;
	struct PriorityQueueNode* next;
} PriorityQueueNode;

// 허프만 트리 노드와 빈도를 저장하는 우선순위 큐
typedef struct {
	PriorityQueueNode* front;
} PriorityQueue;

// 허프만 노드 생성
HuffmanNode* createHuffmanNode(char data, int freq) {
	HuffmanNode* newNode = (HuffmanNode*)malloc(sizeof(HuffmanNode));
	newNode->data = data;
	newNode->freq = freq;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

// 우선순위 큐 노드 생성
PriorityQueueNode* createPriorityQueueNode(HuffmanNode* node) {
	PriorityQueueNode* newNode = (PriorityQueueNode*)malloc(sizeof(PriorityQueueNode));
	newNode->node = node;
	newNode->next = NULL;
	return newNode;
}

// 우선순위 큐 초기화
PriorityQueue* createPriorityQueue() {
	PriorityQueue* queue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
	queue->front = NULL;
	return queue;
}

// 우선순위 큐에 노드 삽입
void enqueue(PriorityQueue* queue, HuffmanNode* node) {
	PriorityQueueNode* newNode = createPriorityQueueNode(node);

	if (queue->front == NULL || node->freq < queue->front->node->freq) {
		newNode->next = queue->front;
		queue->front = newNode;
	}
	else {
		PriorityQueueNode* current = queue->front;

		while (current->next != NULL && current->next->node->freq < node->freq) {
			current = current->next;
		}

		newNode->next = current->next;
		current->next = newNode;
	}
}

// 우선순위 큐에서 노드 추출
HuffmanNode* dequeue(PriorityQueue* queue) {
	if (queue->front == NULL) {
		return NULL;
	}

	PriorityQueueNode* temp = queue->front;
	HuffmanNode* node = temp->node;
	queue->front = temp->next;
	free(temp);
	return node;
}

// 허프만 트리 생성
HuffmanNode* buildHuffmanTree(char data[], int freq[], int size) {
	PriorityQueue* queue = createPriorityQueue();

	for (int i = 0; i < size; i++) {
		HuffmanNode* node = createHuffmanNode(data[i], freq[i]);
		enqueue(queue, node);
	}

	while (queue->front->next != NULL) {
		HuffmanNode* left = dequeue(queue);
		HuffmanNode* right = dequeue(queue);

		HuffmanNode* internalNode = createHuffmanNode('\0', left->freq + right->freq);
		internalNode->left = left;
		internalNode->right = right;

		enqueue(queue, internalNode);
	}

	HuffmanNode* root = dequeue(queue);
	free(queue);

	return root;
}

// 허프만 코드 생성 (재귀적)
void generateHuffmanCodes(HuffmanNode* root, char code[], int top) {
	if (root->left != NULL) {
		code[top] = '0';
		generateHuffmanCodes(root->left, code, top + 1);
	}

	if (root->right != NULL) {
		code[top] = '1';
		generateHuffmanCodes(root->right, code, top + 1);
	}

	if (root->left == NULL && root->right == NULL) {
		printf("%c: %s\n", root->data, code);
	}
}

// 메모리 해제 (허프만 트리)
void freeHuffmanTree(HuffmanNode* root) {
	if (root == NULL) {
		return;
	}

	freeHuffmanTree(root->left);
	freeHuffmanTree(root->right);
	free(root);
}

int main() {
	char data[] = { 'a', 'b', 'c', 'd', 'e' };
	int freq[] = { 5, 9, 12, 13, 16 };
	int size = sizeof(data) / sizeof(data[0]);

	HuffmanNode* root = buildHuffmanTree(data, freq, size);

	char code[100]; // 임의로 충분한 크기로 설정
	printf("Huffman Codes:\n");
	generateHuffmanCodes(root, code, 0);

	// 메모리 해제
	freeHuffmanTree(root);

	return 0;
}


#include<iostream>
using namespace std;
const int N = 10;
int a[N][N];
int mat[][4] = { {1, 2, 3, 4},{5, 6, 7, 8} };

void init() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			a[i][j] = 0;
		}
	}
}
int main() {


}



#elif 0
#include <stdio.h>
int temp[10][10];  // 행과 열을 바꾼 새로운 배열

void rotateRectangle(int mat[][4], int rows, int cols) {
	//int temp[cols][rows];  // 행과 열을 바꾼 새로운 배열

	// 행과 열을 바꿔서 새로운 배열에 저장
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			temp[j][rows - 1 - i] = mat[i][j];
		}
	}
	/*
	void rotate(){
		int N = 10;
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				//cout << a[i][j] << " ";
				m[j][N - 1 - i] = a[i][j];
			}
		}
	}
	*/
	// 새로운 배열의 내용을 원래 배열에 복사
	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			mat[i][j] = temp[i][j];
		}
	}
}

void printRectangle(int mat[][4], int rows, int cols) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			printf("%d ", mat[i][j]);
		}
		printf("\n");
	}
}

int mat[][4] = { {1, 2, 3, 4},
				{5, 6, 7, 8},
				{9, 10, 11, 12} };

int main() {


	int rows = 3; // 배열의 행 수
	int cols = 4; // 배열의 열 수

	printf("Original Rectangle:\n");
	printRectangle(mat, rows, cols);

	// 배열을 90도 회전
	rotateRectangle(mat, rows, cols);

	printf("\nRectangle After 90-degree Rotation:\n");
	printRectangle(mat, cols, rows);

	return 0;
}

#elif 0
#include <stdio.h>

void swap(int& a, int& b) {
	int temp = a;
	a = b;
	b = temp;
}
void rotateMatrix(int mat[][3], int N) {
	// 행과 열을 바꾸고
	for (int i = 0; i < N; i++) {
		for (int j = i + 1; j < N; j++) {
			swap(mat[i][j], mat[j][i]);
		}
	}

	// 열의 순서를 뒤집습니다.
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N / 2; j++) {
			swap(mat[i][j], mat[i][N - 1 - j]);
		}
	}
}

void printMatrix(int mat[][3], int N) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			printf("%d ", mat[i][j]);
		}
		printf("\n");
	}
}

int main() {
	int mat[][3] = { {1, 2, 3},
					{4, 5, 6},
					{7, 8, 9} };

	int N = 3; // 배열의 크기

	printf("Original Matrix:\n");
	printMatrix(mat, N);

	// 배열을 90도 회전
	rotateMatrix(mat, N);

	printf("\nMatrix After 90-degree Rotation:\n");
	printMatrix(mat, N);

	return 0;
}

#elif 0
#include<iostream>
using namespace std;

int a[10][10] = {
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0}
	//{ 0,1,2,3,4,5,6,0,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
	//{ 0,1,2,0,4,5,6,7,8,9},
	//{ 0,1,2,3,4,5,6,7,8,9},
};
int m[10][10];
void rotate90(int arr[][10]) {
	int N = 9; memset(m, 0, sizeof(m) / sizeof(int));
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			m[j][N - i] = arr[i][j];
		}
	}
}
void rotate180(int arr[][10]) {
	int N = 9;	memset(m, 0, sizeof(m) / sizeof(int));
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			m[N - i][N - j] = arr[i][j];
		}
	}
}
void rotate270(int arr[][10]) {
	int N = 9;	memset(m, 0, sizeof(m) / sizeof(int));
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			m[N - j][i] = arr[i][j];
		}
	}
}
void print(int arr[][10]) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			cout << arr[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}
int mat[][4] = { {1, 2, 3, 4},
				{5, 6, 7, 8} };

void copy(int arr[][4]) {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			a[i][j] = arr[i][j];
		}
	}
}
int main() {

	print(a);

	copy(mat);
	print(a);

	rotate90(a);
	print(m);

	rotate180(a);
	print(m);

	rotate270(a);
	print(m);

	return 0;
}
#elif 0
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int *tree;
	int *nums;
	int size;
} SegmentTree;

SegmentTree *createSegmentTree(int nums[], int size) {
	SegmentTree *segmentTree = (SegmentTree *)malloc(sizeof(SegmentTree));
	segmentTree->nums = nums;
	segmentTree->size = size;
	segmentTree->tree = (int *)malloc(4 * size * sizeof(int)); // 대략적으로 4배 정도의 크기로 세그먼트 트리 배열을 생성
	return segmentTree;
}

void buildTree(SegmentTree *segmentTree, int node, int start, int end) {
	if (start == end) {
		segmentTree->tree[node] = segmentTree->nums[start];
	}
	else {
		int mid = (start + end) / 2;
		buildTree(segmentTree, 2 * node, start, mid);
		buildTree(segmentTree, 2 * node + 1, mid + 1, end);
		segmentTree->tree[node] = segmentTree->tree[2 * node] + segmentTree->tree[2 * node + 1];
	}
}

int query(SegmentTree *segmentTree, int node, int start, int end, int left, int right) {
	if (right < start || end < left) {
		return 0;
	}
	else if (left <= start && end <= right) {
		return segmentTree->tree[node];
	}
	else {
		int mid = (start + end) / 2;
		int leftSum = query(segmentTree, 2 * node, start, mid, left, right);
		int rightSum = query(segmentTree, 2 * node + 1, mid + 1, end, left, right);
		return leftSum + rightSum;
	}
}

void update(SegmentTree *segmentTree, int node, int start, int end, int index, int diff) {
	if (index < start || end < index) {
		return;
	}

	segmentTree->tree[node] += diff;

	if (start != end) {
		int mid = (start + end) / 2;
		update(segmentTree, 2 * node, start, mid, index, diff);
		update(segmentTree, 2 * node + 1, mid + 1, end, index, diff);
	}
}

void destroySegmentTree(SegmentTree *segmentTree) {
	free(segmentTree->tree);
	free(segmentTree);
}

int main() {
	int nums[] = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19 };
	int size = sizeof(nums) / sizeof(nums[0]);

	SegmentTree *segmentTree = createSegmentTree(nums, size);

	// Build the segment tree
	buildTree(segmentTree, 1, 0, size - 1);

	// Calculate the sum of elements in the range [2, 7]
	int sum = query(segmentTree, 1, 0, size - 1, 2, 7);
	printf("Sum of elements in range [2, 7]: %d\n", sum);

	// Update the value at index 3 to 10
	update(segmentTree, 1, 0, size - 1, 3, 10);

	// Recalculate the sum after update
	sum = query(segmentTree, 1, 0, size - 1, 2, 7);
	printf("Sum of elements in range [2, 7] after update: %d\n", sum);

	// Clean up
	destroySegmentTree(segmentTree);

	return 0;
}

#elif 0
#include<iostream>
using namespace std;
// Segment tree 알고리즘

int a[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
int tree[4 * 8];

int init(int s, int e, int n) {
	if (s == e) return tree[n] = a[s];
	int m = (s + e) / 2;
	return tree[n] = init(s, m, n * 2) + init(m + 1, e, n * 2 + 1);
}
int sum(int s, int e, int n, int l, int r) {

	//            l ------------- r
	//     s-e          s-e           s-e
	// 범위 밖에 있을 경우
	if (e < l || r < s)		return 0;

	// 범위 안에 있을 경우
	if (l <= s && e <= r)	return tree[n];

	// 범위에 걸쳐 있을 경우 
	int m = (s + e) / 2;
	return sum(s, m, n * 2, l, r) + sum(m + 1, e, n * 2 + 1, l, r);
}
int main() {

	int N = 8;
	init(0, N - 1, 1);	// s, e, 노드

	for (int i = 0; i < 4 * N; i++) {
		cout << i << " = " << tree[i] << endl;
	}


	cout << sum(0, 7, 1, 0, 2) << endl;
	return 0;
}
#elif 0
#include<iostream>
using namespace std;

struct Node {
	int data;
	struct Node *L, *R;
} node;

struct Node* creatnode(int data) {
	Node* newnode = new Node;
	*newnode = { data, NULL, NULL };
	return newnode;
}
void preorderTreversal(Node* root) {
	if (root != 0) {
		printf("%d ", root->data);
		preorderTreversal(root->L);
		preorderTreversal(root->R);
	}
}
void inorderTreversal(Node* root) {
	if (root != 0) {
		inorderTreversal(root->L);
		printf("%d ", root->data);
		inorderTreversal(root->R);
	}
}
void postorderTreversal(Node* root) {
	if (root != 0) {
		postorderTreversal(root->L);
		postorderTreversal(root->R);
		printf("%d ", root->data);
	}
}
int main() {
	Node* root = creatnode(1);
	root->L = creatnode(2);
	root->R = creatnode(3);
	root->L->L = creatnode(4);
	root->L->R = creatnode(5);
	root->R->L = creatnode(6);
	root->R->R = creatnode(7);

	printf("preorderTreversal");
	preorderTreversal(root);

	printf("\ninorderTreversal");
	inorderTreversal(root);

	printf("\npostorderTreversal");
	postorderTreversal(root);

	return 0;
}

#elif 0
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int* tree;
	int* nums;
	int size;
} SegmentTree;

SegmentTree* createSegmentTree(int nums[], int size) {
	SegmentTree* segmentTree = (SegmentTree*)malloc(sizeof(SegmentTree));
	segmentTree->nums = nums;
	segmentTree->size = size;
	segmentTree->tree = (int*)malloc(4 * size * sizeof(int)); // 대략적으로 4배 정도의 크기로 세그먼트 트리 배열을 생성
	return segmentTree;
}

void buildTree(SegmentTree* segmentTree, int node, int start, int end) {
	if (start == end) {
		segmentTree->tree[node] = segmentTree->nums[start];
	}
	else {
		int mid = (start + end) / 2;
		buildTree(segmentTree, 2 * node, start, mid);
		buildTree(segmentTree, 2 * node + 1, mid + 1, end);
		segmentTree->tree[node] = segmentTree->tree[2 * node] + segmentTree->tree[2 * node + 1];
	}
}

int query(SegmentTree* segmentTree, int node, int start, int end, int left, int right) {
	if (right < start || end < left) {
		return 0;
	}
	else if (left <= start && end <= right) {
		return segmentTree->tree[node];
	}
	else {
		int mid = (start + end) / 2;
		int leftSum = query(segmentTree, 2 * node, start, mid, left, right);
		int rightSum = query(segmentTree, 2 * node + 1, mid + 1, end, left, right);
		return leftSum + rightSum;
	}
}

void update(SegmentTree* segmentTree, int node, int start, int end, int index, int diff) {
	if (index < start || end < index) {
		return;
	}

	segmentTree->tree[node] += diff;

	if (start != end) {
		int mid = (start + end) / 2;
		update(segmentTree, 2 * node, start, mid, index, diff);
		update(segmentTree, 2 * node + 1, mid + 1, end, index, diff);
	}
}

int getSum(SegmentTree* segmentTree, int left, int right) {
	return query(segmentTree, 1, 0, segmentTree->size - 1, left, right);
}

void updateValue(SegmentTree* segmentTree, int index, int newValue) {
	int diff = newValue - segmentTree->nums[index];
	segmentTree->nums[index] = newValue;
	update(segmentTree, 1, 0, segmentTree->size - 1, index, diff);
}

void destroySegmentTree(SegmentTree* segmentTree) {
	free(segmentTree->tree);
	free(segmentTree);
}

int main() {
	int nums[] = { 1, 3, 5, 7, 9, 11 };
	int size = sizeof(nums) / sizeof(nums[0]);

	SegmentTree* segmentTree = createSegmentTree(nums, size);

	printf("Sum of elements in range [1, 3]: %d\n", getSum(segmentTree, 1, 3));

	// Update the value at index 2 to 10
	updateValue(segmentTree, 2, 10);

	printf("Sum of elements in range [1, 3] after update: %d\n", getSum(segmentTree, 1, 3));

	// Clean up
	destroySegmentTree(segmentTree);

	return 0;
}

#elif 0
//첫째 줄에 수의 개수 N(1 ≤ N ≤ 1, 000, 000)과 
//M(1 ≤ M ≤ 10, 000), 
//K(1 ≤ K ≤ 10, 000) 가 주어진다.
//M은 수의 변경이 일어나는 횟수이고, 
//K는 구간의 합을 구하는 횟수이다.
//그리고 둘째 줄부터 N + 1번째 줄까지 N개의 수가 주어진다.
//그리고 N + 2번째 줄부터 N + M + K + 1번째 줄까지 세 개의 정수 a, b, c가 주어지는데, 
//a가 1인 경우 b(1 ≤ b ≤ N)번째 수를 c로 바꾸고 
//a가 2인 경우에는 b(1 ≤ b ≤ N)번째 수부터 c(b ≤ c ≤ N)번째 수까지의 합을 구하여 출력하면 된다.
//입력으로 주어지는 모든 수는 - 263보다 크거나 같고, 263 - 1보다 작거나 같은 정수이다.
#include<iostream>
using namespace std;
long long int f[1'000'001];
long long int m[1'000'001];
long long int N, M, K;
void update(long long int i, long long int v) {
	while (i <= N) {
		f[i] += v;
		i += i & -i;
	}
}
long long int sum(long long int i) {
	long long int ret = 0;
	while (i > 0) {
		ret += f[i];
		i -= i & -i;
	}
	return ret;
}

int main() {
	scanf("%lld %lld %lld ", &N, &M, &K);
	for (int i = 1; i <= N; i++) {
		//long long int v;
		//scanf("%lld ", &v);
		scanf("%lld ", &m[i]);
		update(i, m[i]);
	}
	for (int i = 0; i < M + K; i++) {
		long long int a, b, c;
		scanf("%lld %lld %lld", &a, &b, &c);
		if (a == 1)
			update(b, c - m[b]);
		else
			printf("%lld\n", sum(c) - sum(b - 1));
	}
	return 0;
}


#elif 0
#include<iostream>
using namespace std;
const int N = 16;
int f[20];

void update(int i, int v) {
	while (i <= N) {
		f[i] += v;
		i += i & -i;
	}
}
int sum(int i) {
	int ret = 0;
	while (i > 0) {
		ret += f[i];
		i -= i & -i;
	}
	return ret;
}
int main() {
	for (int i = 1; i <= N; i++) {
		update(i, i);
	}
	for (int i = 1; i <= N; i++) {
		cout << sum(i) << endl;
	}
	return 1;
}

#elif 0
#include<iostream>
using namespace std;

void printbin(int a) {
	for (int j = 8; j >= 0; j--) {
		printf("%d", (a >> j) & 1);
	}
	printf("  ");
}
int main() {
	for (int i = 1; i <= 16; i++) {
		printf("%2d :", i);
		printbin(i);
		//printf("%d :", -i);
		printbin(-i);
		//printf("%d :", i);
		printbin(i & -i);
		printf("%d ", i & -i);
		//printf("%d :", i);
		//printbin( i += i & -i);
		printf("\n");
	}
	return 1;
}
#elif 0
#include<iostream>
using namespace std;
int a[10] = { 1,2,3,4,5,6,7,8,9,10 };
int fw[11];

void update(int i, int v) {
	while (i <= 10) {
		fw[i] += v;
		i += i & -i;
	}
}
int sum(int i) {
	int ret = 0;
	while (i > 0) {
		ret += fw[i];
		i -= i & -i;
	}
	return ret;
}
int main() {
	for (int i = 0; i < 10; i++) {
		update(i + 1, a[i]);
	}
	for (int i = 0; i < 10; i++) {
		cout << a[i] << " ";
	}
	cout << endl;
	for (int i = 0; i <= 10; i++) {
		cout << fw[i] << " ";
	}
	cout << endl;
	return 0;
	for (int i = 1; i <= 10; i++) {
		cout << sum(i) << " ";
	}
	cout << endl;
	update(5, 1);
	for (int i = 1; i <= 10; i++) {
		cout << sum(i) << " ";
	}
	return 1;
}
#elif 0
#define rint register int
#include <unordered_map>
#include <vector>
#include <memory.h>
#include <string>

using namespace std;
unordered_map<string, int> map;
int parents[12001][11];
int depth[12001];
int fenwick[1000002];
int idx;
int power[11] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };

int sum(int i) {
	rint ret = 0;
	while (i) {
		ret += fenwick[i];
		i -= (i & -i);
	}
	return ret;
}

void update(int i, int diff) {
	while (i <= 1000001) {
		fenwick[i] += diff;
		i += (i & -i);
	}
}

void init(char mAncestor[], int mDeathday)
{
	memset(parents, -1, sizeof(parents));
	map.clear();
	memset(fenwick, 0, sizeof(fenwick));
	memset(depth, 0, sizeof(depth));
	idx = 0;

	map[string(mAncestor)] = 0;
	update(1, 1);
	update(mDeathday + 2, -1);
}

int add(char mName[], char mParent[], int mBirthday, int mDeathday)
{
	map[string(mName)] = ++idx;
	rint pidx = map[string(mParent)];
	parents[idx][0] = pidx;
	depth[idx] = depth[pidx] + 1;
	for (rint i = 0; i < 11; i++)
	{
		if (parents[idx][i] == -1)
			break;
		parents[idx][i + 1] = parents[parents[idx][i]][i];
	}
	update(mBirthday + 1, 1);
	update(mDeathday + 2, -1);

	return depth[idx];
}

int distance(char mName1[], char mName2[])
{
	rint idx1 = map[string(mName1)];
	rint idx2 = map[string(mName2)];

	if (depth[idx1] < depth[idx2])
		swap(idx1, idx2);
	rint diff = depth[idx1] - depth[idx2];
	rint ret = diff;
	for (rint i = 0; i < 11; i++)
	{
		if (diff & 1) {
			idx1 = parents[idx1][i];
		}
		diff >>= 1;
	}
	if (idx1 == idx2)
		return ret;

	for (rint i = 10; i >= 0; i--)
	{
		if (parents[idx1][i] != -1 && parents[idx1][i] != parents[idx2][i]) {
			ret += (power[i] << 1);
			idx1 = parents[idx1][i];
			idx2 = parents[idx2][i];
		}
	}
	ret += 2;
	return ret;
}

int count(int mDay)
{
	return sum(mDay + 1);
}
#elif 0
//[H2320] 미생물 가계도
//[제약사항]
//1. 각 테스트 케이스 시작 시 init() 함수가 호출된다.
//2. 각 테스트 케이스에서 add() 함수의 호출 횟수는 12, 000 이하이다.
//3. 각 테스트 케이스에서 distance() 함수의 호출 횟수는 50, 000 이하이다.
//4. 각 테스트 케이스에서 count() 함수의 호출 횟수는 30, 000 이하이다.
//5. 모든 후손의 선조로부터의 가계도 거리는 2, 000 이하이다.
//6. 개체의 이름으로 주어지는 문자열은 영문 대소문자와 마지막 ‘＼0’로 이루어져 있다.
#include<iostream> 
#include<vector> 
#include<unordered_map> 
using namespace std;

struct Node {
	int parent, depth, fday, lday;
}node[12'001]; int ndx;

vector<int>	a[12'001];
unordered_map<string, int>	h;
int fw[1'000'005];

void update(int i, int v) {
	while (i <= 1'000'001) {
		fw[i] += v;
		i += i & -i;
	}
}
int sum(int i) {
	int ret = 0;
	while (i > 0) {
		ret += fw[i];
		i -= i & -i;
	}
	return ret;
}
void init(char mAncestor[], int mLastday) {
	//각 테스트 케이스의 처음에 호출된다.
	//기존의 기록들은 모두 삭제된다.
	//mAncestor는 선조의 이름이며, 선조의 생존 기간의 시작은 항상 0이다.
	//Parameters
	//mAncestor : 선조의 이름(3 ≤ | mAncestor | ≤ 11, | A | 는 A 문자열의 길이를 의미한다)
	//mLastday : 선조의 마지막 생존일(0 ≤ mLastDay ≤ 1, 000, 000)
	//memset(fw, 0, sizeof(fw));
	for (int i = 0; i < 1'000'005; i++)	fw[i] = 0;
	//memset(a, 0, sizeof(a));
	for (int i = 0; i < 12'001; i++)	a[i].clear();
	h.clear();
	ndx = 0;
	h[mAncestor] = ndx;
	node[ndx] = { 0, 0, 0, mLastday };
	update(1, 1);
	update(mLastday + 2, -1);
	ndx++;
}
int add(char mName[], char mParent[], int mFirstday, int mLastday) {
	//이름이 mName인 새로운 미생물 개체의 정보가 추가된다.기존에 중복된 이름이 없다는 것이 보장된다.
	//부모의 이름은 mParent이고, 개체의 생존 기간의 시작은 mFirstday, 마지막은 mLastday 이다.
	//이름이 mParent인 개체가 앞서 입력된 기록에 존재한다.
	//추가된 개체와 선조의 가계도 거리를 반환한다.
	//Parameters
	//mName : 개체의 이름(3 ≤ | mName | ≤ 11)
	//mParent : 개체의 부모의 이름(3 ≤ | mParent | ≤ 11)
	//mFirstday : 개체의 첫 생존일(부모의 Firstday ≤ mFirstday ≤ 부모의 Lastday)
	//mLastday : 개체의 마지막 생존일(mFirstday ≤ mLastday ≤ 1, 000, 000)
	//Returns
	//추가된 개체와 선조의 가계도 거리를 반환한다.	
	h[mName] = ndx;
	int p = h[mParent];	int c = h[mName];
	a[p].push_back(c);
	a[c].push_back(p);
	int d = node[p].depth;
	node[ndx] = { p, d + 1, mFirstday, mLastday };
	update(mFirstday + 1, 1);
	update(mLastday + 2, -1);
	ndx++;
	return d + 1;

}
int distance(char mName1[], char mName2[]) {
	//이름이 mName1인 개체와 mName2인 개체 간의 가계도 거리를 반환한다.
	//이름이 mName1, mName2인 개체가 앞서 입력된 기록에 존재한다.
	//Parameters
	//mName1, mName2: 가계도 거리를 알아내려는 두 개체의 이름(3 ≤ | mName1 | , | mName2 | ≤ 11)
	//Returns
	//두 개체 간의 가계도 거리를 반환한다.
	int ret = 0;
	int s = h[mName1];	int e = h[mName2];
	while (node[s].depth > node[e].depth) {
		ret++;
		s = node[s].parent;
	}
	while (node[s].depth < node[e].depth) {
		ret++;
		e = node[e].parent;
	}
	if (s != e) {
		ret += 2 * node[s].depth;
	}
	cout << mName1 << " " << mName2 << " " << ret << endl;
	return ret;
}
int count(int mDay) {
	//지금까지 기록된 미생물 중 생존 기간에 mDay일이 포함되는 개체 수를 반환한다.
	//Parameters
	//mDay : 생존한 개체 수를 조사하는 날짜(0 ≤ mDay ≤ 1, 000, 000)
	//Returns
	//생존 기간에 mDay일이 포함되는 개체 수를 반환한다.
	int ret = sum(mDay + 1);
	cout << mDay << " " << ret << endl;
	return ret;



}

#elif 0
#include <unordered_map>
using namespace std;

#define MAXN    8008

struct NODE {
	int id;
	int parent;
	int ch[2];
	int num;
	int sum;
} nALL[MAXN];

unordered_map<int, int> nHash;
int nidx;

void init(int mId, int mNum) {
	nidx = 0;
	nHash.clear();

	nALL[++nidx] = { mId, 0, {0, 0}, mNum, mNum };
	nHash[mId] = nidx;
}

void updateSum(int idx, int num) {
	if (nALL[idx].parent)
		updateSum(nALL[idx].parent, num);

	nALL[idx].sum += num;
}

int add(int mId, int mNum, int mParent) {
	int parent = nHash[mParent];
	if (nALL[parent].ch[0] && nALL[parent].ch[1])
		return -1;

	nALL[++nidx] = { mId, parent, {0, 0}, mNum, mNum };
	nHash[mId] = nidx;

	if (!nALL[parent].ch[0])
		nALL[parent].ch[0] = nidx;
	else
		nALL[parent].ch[1] = nidx;

	updateSum(parent, mNum);

	return nALL[parent].sum;
}

void removeNode(int idx) {
	if (idx == 0) return;

	if (nALL[idx].ch[0]) removeNode(nALL[idx].ch[0]);
	if (nALL[idx].ch[1]) removeNode(nALL[idx].ch[1]);

	nHash.erase(nALL[idx].id);
}

int remove(int mId) {
	int localidx = nHash[mId];
	if (localidx == 0)
		return -1;

	int ret = nALL[localidx].sum;
	int pidx = nALL[localidx].parent;
	updateSum(pidx, -ret);

	if (nALL[pidx].ch[0] == localidx)
		nALL[pidx].ch[0] = 0;
	else
		nALL[pidx].ch[1] = 0;

	// 삭제
	removeNode(localidx);

	return ret;
}

int cut, maxCut, limit;

int .
(int idx) {
	if (cut >= maxCut)
		return 0;

	if (nALL[idx].num > limit) {
		cut = 12345678;
		return 0;
	}
	if (nALL[idx].sum <= limit)
		return nALL[idx].sum;

	int L = nALL[idx].ch[0] ? dfs(nALL[idx].ch[0]) : 0;
	int R = nALL[idx].ch[1] ? dfs(nALL[idx].ch[1]) : 0;

	if (nALL[idx].num + R + L > limit) {
		cut++;
		(L > R) ? L = 0 : R = 0;
	}
	if (nALL[idx].num + L > limit) {
		cut++;
		L = 0;
	}
	if (nALL[idx].num + R > limit) {
		cut++;
		R = 0;
	}

	return nALL[idx].num + L + R;
}

int reorganize(int M, int K) {
	maxCut = M;
	limit = K;
	cut = 0;

	dfs(1);

	return cut < M;
}
#elif 0
#include<iostream>
#include<vector>
using namespace std;
vector<pair<int, int>>	v;
int main() {

	v.push_back({ 1, 2 });
	v.push_back({ 3, 4 });
	v.push_back({ 5, 6 });

	cout << v[0].first << " " << v[0].second << endl;
	cout << v[1].first << " " << v[1].second << endl;
	cout << v[2].first << " " << v[2].second << endl;
	return 1;
}

#elif 0
#include<iostream>
using namespace std;

void foo(int& a) {
	cout << a << " " << &a << endl;
	a = 10;
	cout << a << " " << &a << endl;
}
int main() {

	int ret = 0;
	cout << ret << " " << &ret << endl;
	foo(ret);
	cout << ret << " " << &ret << endl;
	return 1;
}
#elif 0
#include<unordered_map>
using namespace std;
unordered_map<int, int>	h;
//vector<pair<int, int>>	v[101];
int a[100][100];	// start 에서 end 까지 latency 배열
int v[100];
int dn;
//1. 각 테스트 케이스 시작 시 init() 함수가 한 번 호출된다.
//2. 각 테스트 케이스에서 connect() 함수의 호출 횟수는 10, 000 이하이다.
//3. 각 테스트 케이스에서 measure() 함수의 호출 횟수는 1, 000 이하이다.
//4. 각 테스트 케이스에서 test() 함수의 호출 횟수는 1, 000 이하이다.
//5. 임의의 두 장비의 전송 경로에 있는 장비의 수는 100 이하이다.
void init(int mDevice) {
	//테스트 케이스에 대한 초기화 함수.각 테스트 케이스의 맨 처음 1회 호출된다.
	//초기에 장비 mDevice가 있다.mDevice는 장비 번호이다.
	//Parameters
	//mDevice : 초기 장비 번호(1 ≤ mDevice ≤ 1, 000, 000, 000)
	dn = 0;
	h[mDevice] = dn++;
	memset(a, 0, sizeof(a));
}
void connect(int mOldDevice, int mNewDevice, int mLatency) {
	//새로운 장비 mNewDevice를 추가하고 장비 mNewDevice와 장비 mOldDevice를 전송 속도가 mLatency인 케이블로 연결한다.
	//mOldDevice는 이미 존재하는 장비 번호이다.
	//mNewDevice는 추가되는 새로운 장비 번호이다.
	//Parameters
	//mOldDevice : 이미 존재하는 장비 번호(1 ≤ mOldDevice ≤ 1, 000, 000, 000)
	//mNewDevice : 새롭게 추가되는 장비 번호(1 ≤ mNewDevice ≤ 1, 000, 000, 000)
	//mLatency : 케이블의 전송 속도(1 ≤ mLatency ≤ 10, 000)
	h[mNewDevice] = dn++;
	int o = h[mOldDevice];	int n = h[mNewDevice];
	//v[o].emplace_back(n, mLatency);	
	//v[n].emplace_back(o, mLatency);	
	a[o][n] = a[n][o] = mLatency;
}
int dfs(int s, int e) {
	int ret = 0;

	if (s == e)
		return 0;

	int check = 0;
	for (int i = 0; i < 100; i++) {
		if (!v[s] && a[s][i]) {
			check = 1;
		}
	}

	if (check == 0)
		return 0;

	v[s] = 1;

	for (int i = 0; i < 100; i++) {
		if (!v[s] && a[s][i]) {
			ret += dfs(i, e) + a[s][i];
		}
	}

	return ret;
}
int measure(int mDevice1, int mDevice2) {
	//장비 mDevice1에서 장비 mDevice2로 신호를 전송했을 때 전송 시간을 반환한다.
	//mDevice1와 mDevice2는 이미 존재하는 장비 번호이고 서로 다르다.
	//Parameters
	//mDevice1 : 장비 번호(1 ≤ mDevice1 ≤ 1, 000, 000, 000)
	//mDevice2 : 장비 번호(1 ≤ mDevice2 ≤ 1, 000, 000, 000, mDevice1 ≠ mDevice2)
	//Returns
	//장비 mDevice1에서 장비 mDevice2로 신호를 전송했을 때 전송 시간
	int s = h[mDevice1];	int e = h[mDevice2];
	return dfs(s, e);
}
int test(int mDevice) {
	//신호를 모니터링하는 장비를 장비 mDevice로 하고 테스트를 진행한다.
	//전송 신호가 장비 mDevice를 지나가고 전송 시간이 최대가 되도록 보내는 장비와 받는 장비를 선택하고 이때의 전송 시간을 반환한다. (본문 설명 참조)
	//mDevice는 이미 존재하는 장비 번호이다.
	//함수 호출 시, 이미 존재하는 장비는 2개 이상 있음을 보장한다.
	//Parameters
	//mDevice : 장비 번호(1 ≤ mDevice ≤ 1, 000, 000, 000)
	//Returns
	//장비 mDevice로 전송 신호가 지나갈 때 최대 전송 시간
	int d = h[mDevice];
}



#elif 0
#include<queue>
#include<unordered_map>
struct Cable {
	int Dest;
	int mTime;
	Cable(int x = 0, int y = 0) :Dest(x), mTime(y) {};
};
std::vector<Cable> P[10'001];
int pid;
std::unordered_map<int, int> umap;
int visited[10001], visid;
void init(int mDevice)
{
	pid = 0;
	umap.clear();
	P[0].clear();
	umap[mDevice] = pid++;
	return;
}
void connect(int mOldDevice, int mNewDevice, int mLatency)
{
	int id = umap[mOldDevice];
	P[id].push_back(Cable(pid, mLatency));
	P[pid].clear();
	P[pid].push_back(Cable(id, mLatency));
	umap[mNewDevice] = pid++;
	return;
}
void dfs(int s, int tg, int d, int& kq, int& kq2) {
	if (s == tg) {
		kq = d;
		return;
	}
	if (d > kq2) kq2 = d;
	visited[s] = visid;
	for (auto next : P[s]) {
		if (visited[next.Dest] == visid) continue;
		dfs(next.Dest, tg, d + next.mTime, kq, kq2);
	}
}
int measure(int mDevice1, int mDevice2)
{
	int dv1 = umap[mDevice1], dv2 = umap[mDevice2];
	++visid;
	int ret = 0, temp = 0;
	dfs(dv1, dv2, 0, ret, temp);
	return ret;
}
int test(int mDevice)
{
	std::priority_queue<int> pq;
	int ID = umap[mDevice];
	visited[ID] = ++visid;
	for (auto next : P[ID]) {
		int d, dmax = 0, temp = 0;
		dfs(next.Dest, -1, 0, temp, dmax);
		d = dmax + next.mTime;
		pq.push(d);
	}
	int ans = pq.top();
	pq.pop();
	if (pq.size() > 0) ans += pq.top();
	return ans;
}
#elif 0

// 매 timestamp가 증가할 때 마다 새롭게 우선순위를 찾아야 한다
// 가장 최근 게시글부터 모든 게시글을 순회하면서 현재 유저가 볼 수 있는 게시글 리스트 만듬
// timestamp 오름차순 순으로 게시글 정보(pid, uid, likE)를 관리한다
// --->>> 매번 feed 조회 시 유저가 볼 수 있는 게시글을 탐색해야 한다 --->>> 시간 초과 발생

// 매번 getFeed를 할 때 마다 모든 게시글을 순회하며 list를 만드는 것은 비효율적이다
// 대신 각 user 별로 작성한 post를 관리한다면 현재 user와 follow 한 user가 작성한 post만 조회하면 된다
// 먼저 post를 timestamp 기준으로 quick 정렬한다
// 1000초 이내 post는 like 기준으로 다시 quick 정렬한다
// 1000초 이내와 1000초 초과에 다른 룰을 적용해서 공통된 기준 score를 만들고 이 score를 기준으로 한 번만 정렬한다면? -> 시간 차이 크지 않음
// 정렬 대상 리스트를 최소로 줄여야 시간을 줄일 수 있다 -> 1000초 초과의 경우 각 유저 게시글 리스트가 이미 오름차순으로 정렬 돼 있다
// 필요한 1000초 초과 갯수만큼 각 유저별로 게시글을 선택해 정렬한다면 최적화이다

#include <malloc.h>
//#include <stdio.h>

#define MAX_STAMP 100001
#define MAX_USER 1001

int Like[MAX_STAMP];
int TimeStamp[MAX_STAMP];

int UserP[MAX_USER];
int PrevP[MAX_STAMP];

int User[MAX_USER][MAX_USER];
int UserIdx[MAX_USER];

int Score[MAX_STAMP];
int ScoreP[MAX_STAMP];

void scoreSort(int st, int ed) {
	register int stt = st + 1;
	register int edd = ed;
	register int pivot = st;
	register int temp;
	do {
		while (Score[stt] > Score[pivot] && stt <= ed) stt++;
		while (Score[edd] < Score[pivot] && edd >= st + 1) edd--;
		if (stt < edd) {
			temp = Score[stt];
			Score[stt] = Score[edd];
			Score[edd] = temp;
			temp = ScoreP[stt];
			ScoreP[stt] = ScoreP[edd];
			ScoreP[edd] = temp;
		}
	} while (stt <= edd);
	temp = Score[edd];
	Score[edd] = Score[pivot];
	Score[pivot] = temp;
	temp = ScoreP[edd];
	ScoreP[edd] = ScoreP[pivot];
	ScoreP[pivot] = temp;
	if (st < edd - 1)
		scoreSort(st, edd - 1);
	if (edd + 1 < ed && edd + 1 <= 9)
		scoreSort(edd + 1, ed);
}

void init(int N)
{
	for (register int i = 1; i <= N; i++) {
		User[i][0] = i;
		UserIdx[i] = 1;
		UserP[i] = 0;
	}
}

void follow(int uID1, int uID2, int timestamp)
{
	User[uID1][UserIdx[uID1]++] = uID2;
}

void makePost(int uID, int pID, int timestamp)
{
	Like[pID] = 0;
	TimeStamp[pID] = timestamp;
	PrevP[pID] = UserP[uID];
	UserP[uID] = pID;
}

void like(int pID, int timestamp)
{
	Like[pID]++;
}

void getFeed(int uID, int timestamp, int pIDList[])
{
	register int scoreIdx = 0;
	register int userLeng = UserIdx[uID];
	for (register int u = 0; u < userLeng; u++) {
		register int pid = UserP[User[uID][u]];
		while (pid != 0) {
			if (TimeStamp[pid] >= timestamp - 1000) {
				ScoreP[scoreIdx] = pid;
				Score[scoreIdx++] = Like[pid] * 1000000 + TimeStamp[pid];
			}
			else
				break;
			pid = PrevP[pid];
		}
	}
	register int remain = 10 - scoreIdx;
	if (scoreIdx < 10) {
		for (register int u = 0; u < userLeng; u++) {
			register int pid = UserP[User[uID][u]];
			register int count = remain;
			while (pid != 0) {
				if (count == 0)
					break;
				if (TimeStamp[pid] < timestamp - 1000) {
					ScoreP[scoreIdx] = pid;
					Score[scoreIdx++] = TimeStamp[pid];
					count--;
				}
				pid = PrevP[pid];
			}
		}
	}
	scoreSort(0, scoreIdx - 1);
	for (register int i = 0; i < 10; i++) {
		pIDList[i] = ScoreP[i];
	}
}
#elif 0
#include<iostream>
using namespace std;
//1. 게시글을 등록한다.각 게시글들은 등록되는 시점의 timestamp 를 가지고 있다.
//2. 다른 사용자를 “follow” 한다.“follow”를 하게 되면, 그 사용자의 게시글을 볼 수 있다.
//3. 특정 게시글에 “like”를 추가한다.
//4. 특정 사용자를 기준으로 자신이 게시한 글과 자신이 “follow” 한 사용자의 게시글 중 우선 순위가 높은 글부터 내림차순으로 최대 10 개의 게시글을 보여준다.
//각 게시글의 우선순위를 계산하는 방법은 다음과 같다.
//1. 게시된 지 1, 000 초 이내인 게시글이 그렇지 않은 게시글보다 우선순위가 높다.
//2. 게시된 지 1, 000 초 이내인 게시글들 중에는 “like” 가 많은 게시글의 우선순위가 높다.
//3. 게시된 지 1, 000 초 이내이면서 “like” 의 개수가 같은 게시글들 중에는 “timestamp” 가 높은 게시글의 우선순위가 높다.
//4. 게시된 지 1, 000 초를 초과한 게시글의 경우, “timestamp” 가 높은 게시글의 우선순위가 높다.
//[제약사항]
//1. 사용자 수 N 은 2 이상 1, 000 이하의 정수이다. (2 ≤ N ≤ 1, 000)
//2. timestamp 는 1 에서 시작하고 최대 100, 000 까지 증가한다.
//3. 모든 함수들은 timestamp 오름차순으로 호출된다.
//4. 모든 게시물들의 timestamp 는 서로 다르다.
//5. follow(), makePost(), like() 함수의 호출 횟수는 각각 100, 000 회 이하이다.
//6. getFeed() 함수의 호출 횟수는 5, 000 회 이하이다.
//7. 특정 사용자가 한번 “follow” 한 사용자를 다시 “follow” 하는 호출은 발생하지 않는다.
struct Node {
	int id, like, timestamp;
	struct Node* next, *prev;
}node[100'001];
struct List {
	Node* pHead = new Node;
	Node* pTail = new Node;
	void link(Node* f, Node* b) {
		f->next = b;
		b->prev = f;
	}
	void initialize() {
		link(pHead, pTail);
	}
	bool isEmpty() {
		return pHead->next == pTail;
	}
	void erase(Node* node) {
		link(node->prev, node->next);
	}
	void insert_back(Node* node) {
		link(pTail->prev, node);
		link(node, pTail->next);
	}
	void splice(List* list) {
		if (isEmpty())	return;
		link(pTail->prev, list->pHead->next);
		link(list->pTail->prev, pTail);
		list->initialize();
	}
}list[1'001];
int flw[1'001][1'001];
void init(int N) {
	//각 testcase 시작 시 초기화를 위해 1번 호출된다.
	//Parameters
	//N : 사용자 수(2 ≤ N ≤ 1, 000)
	memset(node, 0, sizeof(node));
	//memset(flw, 0, sizeof(flw));

	for (int i = 0; i <= N; i++) {
		list[i].initialize();
		flw[i][0] = 1;
		flw[i][flw[i][0]] = i;
	}

}
void follow(int uID1, int uID2, int timestamp) {
	//“uID1” 사용자가 “uID2” 사용자를 “follow” 한다.
	//“uID1” 사용자는 “uID2” 사용자의 모든 게시글을 볼 수 있다.
	//Parameters
	//uID1, uID2 : 사용자의 id(1 ≤ uID1, uID2 ≤ N)
	//timestamp : 현재 시점의 “timestamp”(1 ≤ timestamp ≤ 100, 000)
	flw[uID1][0]++;
	flw[uID1][flw[uID1][0]] = uID2;

}
void makePost(int uID, int pID, int timestamp) {
	//“uID” 사용자가 “pID” 게시글을 게시한다.
	//Parameters
	//uID : 사용자의 ID(1 ≤ uID ≤ N)
	//pID : 게시글의 ID(1 부터 오름차순으로 주어진다.)
	//timestamp : 현재 시점의 “timestamp”(1 ≤ timestamp ≤ 100, 000)
	node[pID] = { uID, 0, timestamp };
	list[uID].insert_back(&node[pID]);

}
void like(int pID, int timestamp) {
	//“pID” 게시글에 “like” 를 1 번 추가 한다.
	//“pID” 는 makePost() 에서 전달되었던 값으로만 주어 진다.
	//Parameters
	//pID : “like” 를 추가할 게시글의 pID
	//timestamp : 현재 시점의 “timestamp”(1 ≤ timestamp ≤ 100, 000)
	node[pID].like++;
}
void getFeed(int uID, int timestamp, int pIDList[]) {
	//현재 “timestamp” 를 기준으로 “uID” 사용자에게 보여지는 최대 10 개의 게시글의 “pID” 들을 찾아 우선순위의 내림차순으로 “pIDList[]” 배열에 저장하여 반환 한다.
	//Parameters
	//uID : 사용자의 id(1 ≤ uID ≤ N)
	//timestamp : 현재 시점의 timestamp(1 ≤ timestamp ≤ 100, 000)
	//pIDList[] : 보여지는 게시글의 pID 들을 저장하는 배열	

	int k = flw[uID][0];

	for (int i = 1; i <= k; i++) {
		int id = flw[uID][i];


	}
}

#elif 0
#include<iostream>
using namespace std;
struct C {
	int Price, Performance;
	bool operator < (const struct C& a)const {
		return a.Performance != Performance ? a.Performance > Performance : a.Price < Price;
	}
};
struct Result {
	int mPerformance, mPrice;
};
int ADD;
int cnt[2][3];

void init(int mCharge) {
	//각 테스트 케이스의 처음에 호출되며, 창고 간 운송료를 알려준다.
	//기존의 부품 목록은 모두 초기화된다.
	//Parameters
	//mCharge : 창고 간 운송료(1 ≤ mCharge ≤ 100, 000)
	ADD = mCharge;
	memset(cnt, 0, sizeof(cnt));
}
int stock(int mType, int mPrice, int mPerformance, int mPosition) {
	//부품 목록에 새로운 부품을 추가한다.	//종류와 가격, 성능이 모두 같은 입력이 testcase 내에서 중복되어 들어오지 않는다.
	//Parameters
	//mType : 부품 종류(0 ≤ mType ≤ 2)
	//mPrice : 부품 가격(1 ≤ mPrice ≤ 100, 000)
	//mPerformance : 부품 성능(1 ≤ mPerformance ≤ 1, 000, 000)
	//mPosition : 부품이 있는 창고 번호(0 ≤ mPosition ≤ 1)
	//Returns
	//입고된 창고에 동일한 type인 부품의 가짓수를 반환한다.
	return cnt[mPosition][mType]++;
}
Result order(int mBudget) {
	//소비자의 예산이 mBudget일 때 예산 안에서 최대의 성능을 내는 부품 조합을 선택한다.	//같은 성능이면 가격이 가장 낮은 조합을 선택한다.	//부품이 출고되면 즉각 채워 넣으므로 품절되지 않는다.
	//Parameters
	//mBudget : 소비자의 예산(1 ≤ mBudget ≤ 500, 000)
	//Returns
	//주어진 예산으로 조립이 가능할 경우, Result 구조체의 mPerformance에 컴퓨터의 성능을, mPrice에 컴퓨터의 가격을 반환한다.
	//주어진 예산으로 조립이 불가능할 경우, mPerformance와 mPrice 모두 0을 반환한다.
	//mBudget내에서 -> BFC, DFC, DP 등 완탐방식으로? 
	//Performance가 높게(최하 성능의 부품이 가장 높도록)?  
}
#elif 0
#include <vector>
#include <algorithm>
using namespace std;
struct Result
{
	int mPrice;
	int mPerformance;
};
vector<Result> v[2][3];	// v[창고][종류] -> 가격+성능
int ADD, id;

void init(int mCharge)
{
	ADD = mCharge;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 3; j++)
			v[i][j].clear();
}

int stock(int mType, int mPrice, int mPerformance, int mPosition)
{
	v[mPosition][mType].push_back({ mPrice, mPerformance });
	return v[mPosition][mType].size();
}

Result order(int mBudget)
{
	Result res = { 0, 0 };
	int s = 0, e = 1000001;
	while (s <= e) {
		int mp[2][3] = { { 99999999, 99999999, 99999999 }, { 99999999, 99999999, 99999999 } };
		int m = (s + e) >> 1;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 3; j++)
				for (auto k : v[i][j]) {
					if (k.mPerformance >= m) mp[i][j] = min(mp[i][j], k.mPrice);
				}
		int A = mp[0][0] + mp[0][1] + mp[0][2];
		int B = mp[1][0] + mp[1][1] + mp[1][2];
		int C = min(mp[0][0], mp[1][0]) + min(mp[0][1], mp[1][1]) + min(mp[0][2], mp[1][2]) + ADD;
		int MIN = min(min(A, B), C);
		if (MIN <= mBudget) {
			res = { MIN, m };
			s = m + 1;
		}
		else {
			e = m - 1;
		}
	}
	return res;
}
#elif 0
struct Result { int row;	int col; };
struct Bacteria { int id;	int size;	int time; };

int m[100][100];

int main() {

	int N = 10, r = 5, c = 5;
	int k = 0;
	while (N--) {
		//k=0

		int i = r - k;
		int j = c - k;

		for (
			; i < 10; i++) {
			for (int j = 0; j < 10; j++) {

			}
		}
	}
	return 1;
}
void init(int N) {
	//각 테스트 케이스의 처음에 호출된다.
	//N* N 크기의 배양기에 박테리아는 없다.
	//시각은 0 이다.
	//Parameters
	//N : 배양기 한 변의 길이(10 ≤ N ≤ 200)
}
Result putBacteria(int mTime, int mRow, int mCol, Bacteria mBac) {
	//mTime 시각에(mRow, mCol) 셀에 박테리아가 투입된다.
	//박테리아는 mBac.id 종류, mBac.size 번식 크기, mBac.time 수명 시간을 가지고 있다.
	//mTime에 소멸되는 박테리아가 있을 경우, 박테리아 소멸 된 후 투입된다.
	//투입된 박테리아는 순식간에 번식을 한다. (번식 방법은 본문 참고)
	//- 번식 크기만큼 번식에 성공하면, 마지막 번식 위치를 Result 에 저장하고 반환한다.
	//- 번식 크기만큼 번식에 실패하면, 박테리아는 소멸하고[Result.row = 0, Result.col = 0]을 반환한다.
	//배양기의(mRow, mCol) 셀에 이미 다른 종류의 박테리아가 살아 있어 빈 셀이 아닐 경우, 아무 동작도 하지 않고[Result.row = 0, Result.col = 0]을 반환한다.
	//mBac.id 는 맨 처음 호출할 때 1이고 그 다음 호출할 때마다 1씩 증가한다.
	//mBac.id 박테리아는 mBac.size 번식 크기만큼 번식에 성공할 경우(mTime + mBac.time) 시각에 소멸한다.
	//Parameters
	//mTime : 박테리아가 투입된 시각(1 ≤ mTime ≤ 1, 000, 000)
	//mRow : 박테리아가 투입된 세로 위치(1 ≤ mRow ≤ N)
	//mCol : 박테리아가 투입된 가로 위치(1 ≤ mCol ≤ N)
	//mBac.id : 투입된 박테리아의 종류(1 ≤ mBac.id ≤ 3, 000)
	//mBac.size : 투입된 박테리아의 번식 크기(1 ≤ mBac.size ≤ 2, 000)
	//mBac.time : 투입된 박테리아의 수명 시간(1 ≤ mBac.time ≤ 20, 000)
	//Returns
	//Result.row : 마지막으로 번식한 row 위치, 번식에 실패할 경우 0
	//Result.col : 마지막으로 번식한 col 위치, 번식에 실패할 경우 0
}
int killBacteria(int mTime, int mRow, int mCol) {
	//mTime 시각에(mRow, mCol) 셀에 살아 있는 박테리아를 소멸시키고, 박테리아 종류를 반환한다.
	//(mRow, mCol) 셀에 박테리아가 없을 경우 아무 동작도 하지 않고 0 을 반환한다.
	//mTime 시각에(mRow, mCol) 셀에 있는 박테리아가 소멸된 경우, 아무 동작도 하지 않고 0 을 반환한다.
	//Parameters
	//mTime : 박테리아를 소멸시키는 시각(1 ≤ mTime ≤ 1, 000, 000)
	//mRow : 박테리아를 소멸시키는 세로 위치(1 ≤ mRow ≤ N)
	//mCol : 박테리아를 소멸시키는 가로 위치(1 ≤ mCol ≤ N)
	//Returns
	//(mRow, mCol) 셀의 박테리아 종류, 없을 경우 0
}
int checkCell(int mTime, int mRow, int mCol) {
	//mTime 시각에 배양기(mRow, mCol) 위치에 박테리아의 종류를 반환한다.
	//박테리아가 없을 경우 0 을 반환한다.
	//mTime 시각에(mRow, mCol) 셀이 있는 박테리아가 소멸된 경우 0 을 반환한다.
	//Parameters
	//mTime : 셀의 박테리아 종류를 파악하는 시각(1 ≤ mTime ≤ 1, 000, 000)
	//mRow : 셀의 세로 위치(1 ≤ mRow ≤ N)
	//mCol : 셀의 가로 위치(1 ≤ mCol ≤ N)
	//Returns
	//박테리아 종류, 박테리아가 없거나 소멸된 경우 0
}

#elif 0
#define MAXN 50000
#define HASH_SIZE  2222

struct Node {
	int index;
	Node* next, *prev;
} node_pool[MAXN], table[HASH_SIZE];

int node_cnt;
#define node() &node_pool[node_cnt++]

int N;
char data[MAXN + 1];

void add(Node* node, int key) {
	node->next = table[key].next;

	if (table[key].next)
		table[key].next->prev = node;

	node->prev = &table[key];
	table[key].next = node;
}

#define hash(str, i) (str[i] - 'a') << 10 | (str[i + 1] - 'a') << 5 | (str[i + 2] - 'a')

void init(int N, char str[])
{
	::N = N, node_cnt = 0;

	for (int i = 0; i < HASH_SIZE; i++)
		table[i].next = 0;

	for (int i = 0; i < N - 2; i++) {
		int key = hash(str, i);
		Node* node = node();
		node->index = i;
		add(node, key);
	}

	for (int i = 0; i <= N; i++)
		data[i] = str[i];
}

void del(Node* node) {
	node->prev->next = node->next;
	if (node->next) node->next->prev = node->prev;
}

int indices[MAXN];
#define swap(a,b) { int t; t=a; a=b; b=t;}
void quick_sort(int start, int end) {
	if (start >= end) return;
	int pivot = start, left = start + 1, right = end;
	while (left <= right) {
		while (indices[pivot] >= indices[left] && left <= end) left++;
		while (indices[pivot] <= indices[right] && right > start) right--;
		if (left <= right) swap(indices[left], indices[right])
		else swap(indices[pivot], indices[right])
	}
	quick_sort(start, right - 1);
	quick_sort(right + 1, end);
}

int change(char str1[], char str2[])
{
	int key = hash(str1, 0);
	int index = -3, ans = 0, len = 0;
	for (Node* node = table[key].next; node; node = node->next)
		indices[len++] = node->index;
	quick_sort(0, len - 1);
	for (int i = 0; i < len; i++) {
		if (indices[i] - index < 3) continue;
		index = indices[i];
		data[index] = str2[0], data[index + 1] = str2[1], data[index + 2] = str2[2];
		ans++;
		for (int j = index - 2; j <= index + 2; j++) {
			if (j < 0 || j >= N - 2) continue;
			del(&node_pool[j]);
			add(&node_pool[j], hash(data, j));
		}
	}
	return ans;
}
void result(char ret[])
{
	for (int i = 0; i <= N; i++) ret[i] = data[i];
}


#elif 0
#include<iostream>
#include<cstring>
using namespace std;
char s[50000];
int N;


//1. 각 테스트 케이스 시작 시 init() 함수가 호출된다.
//2. 문자열의 길이는 최대 50, 000 이다.
//3. 각 테스트 케이스에서 문자열은 영어 소문자로 이루어져 있으며 항상 ’\0’ 으로 끝난다.
//4. 변환할 문자열의 길이는 항상 3 이다.
//5. 각 테스트 케이스에서 change() 함수가 호출되는 횟수는 최대 50, 000 이다.result() 함수는 1 번만 호출 된다.

void init(int N, char init_string[]) {
	::N = N;
	//각 테스트 케이스의 처음에 호출된다.
	//변환을 할 원본 문자열이 주어진다.
	//Parameters
	//N : 원본 문자열의 길이
	//init_string[] : 원본 문자열.	
	int i = 0;
	for (; i < N; i++)		s[i] = init_string[i];
	s[i] = '\0';
}
int change(char string_A[], char string_B[]) {
	//string_A 와 일치하는 부분문자열을 string_B로 바꾼다.
	//Parameters
	//string_A : 규칙을 이루는 문자열에서 찾아 변환시킬 문자열이다.
	//string_B : string_A 를 대체할 문자열
	//(string_A, string_B) 의 각 길이는 3이다.
	//Returns
	//num : 변경한 부분 문자열의 개수를 반환한다.


}
void result(char ret[]) {
	//최종 문자열 상태를 ret 배열에 반환한다.
	//이 함수는 각 테스트케이스 마지막에 한번만 호출된다.
}

#elif 0
//BILL = Bool Int Long Long
#include <iostream>
#include <memory.h>
using namespace std;
#define r register
#define rint r int

int a[26];
int b[26][26];
int c[26][26][26];
int d[26][26][26][26];
char str[300000], *s; ;
int ibegin, rev, istr;

void init(char mStr[]) {
	istr = 0;
	rev = 1;
	ibegin = 100000;
	s = &str[ibegin];

	memset(a, 0, sizeof(a));
	memset(b, 0, sizeof(b));
	memset(c, 0, sizeof(c));
	memset(d, 0, sizeof(d));
	rint len = 0;

	for (; mStr[len]; len++)
		s[len] = mStr[len] - 'a';

	istr = len;
	for (rint i = 0; i < len; i++) {
		a[s[i]]++;

		if (i < len - 1) b[s[i]][s[i + 1]]++;
		if (i < len - 2) c[s[i]][s[i + 1]][s[i + 2]]++;
		if (i < len - 3) d[s[i]][s[i + 1]][s[i + 2]][s[i + 3]]++;
	}
}

void appendWord(char mWord[]) {
	rint len; char x[5] = {};
	for (len = 0; mWord[len]; len++) x[len] = mWord[len] - 'a';
	if (rev) {
		for (rint i = 0; i < len; i++) {
			s[istr] = x[i]; s[istr + 1] = 0;
			d[s[istr - 3]][s[istr - 2]][s[istr - 1]][s[istr]]++;
			c[s[istr - 2]][s[istr - 1]][s[istr]]++;
			b[s[istr - 1]][s[istr]]++;
			a[s[istr]]++;
			istr++;
		}
	}
	else {
		for (rint i = 0; i < len; i++) {
			s = &str[--ibegin]; s[0] = x[i]; istr++;
			a[s[0]]++;
			b[s[0]][s[1]]++;
			c[s[0]][s[1]][s[2]]++;
			d[s[0]][s[1]][s[2]][s[3]]++;
		}
	}
}

void cut(int k) {
	if (rev) {
		for (rint h = 0; h < k; h++) {
			istr--;
			d[s[istr - 3]][s[istr - 2]][s[istr - 1]][s[istr]]--;
			c[s[istr - 2]][s[istr - 1]][s[istr]]--;
			b[s[istr - 1]][s[istr]]--;
			a[s[istr]]--; s[istr] = 0;
		}
	}
	else {
		for (rint h = 0; h < k; h++) {
			a[s[0]]--;
			b[s[0]][s[1]]--;
			c[s[0]][s[1]][s[2]]--;
			d[s[0]][s[1]][s[2]][s[3]]--;
			ibegin++; istr--; s[0] = 0; s++;
		}
	}
}

void reverse() { rev = !rev; }

int countOccurrence(char mWord[]) {
	rint len, res; char x[5] = { }; for (len = 0; mWord[len]; len++) x[len] = mWord[len] - 'a';
	switch (len) {
	case 1: res = a[x[0]]; break;
	case 2: if (rev) res = b[x[0]][x[1]]; else res = b[x[1]][x[0]]; break;
	case 3: if (rev) res = c[x[0]][x[1]][x[2]]; else res = c[x[2]][x[1]][x[0]]; break;
	case 4: if (rev) res = d[x[0]][x[1]][x[2]][x[3]]; else res = d[x[3]][x[2]][x[1]][x[0]]; break;
	}
	return res;
}
#elif 0
#include<deque>
#include<string>
#include<string.h>
#include<algorithm>
using namespace std;

bool rev;
deque<char> dq;
int cnt[27 * 27 * 27 * 27]; // a:1 ~ z:26

void updateCnt(int c) {
	int idx = rev ? 0 : dq.size() - 1;
	int hash = 0, i = 0, base = 1;
	for (; i < 4; i++, base *= 27) {
		if (idx < 0 || idx >= dq.size()) break;

		if (rev) hash = hash * 27 + dq[idx++] - 96; // 'a' ascii = 97
		else hash += (dq[idx--] - 96) * base;

		cnt[hash] += c;
	}
}

void init(char mStr[])
{
	rev = 0;
	dq.clear();
	memset(cnt, 0, sizeof(cnt));
	for (int i = 0; mStr[i]; i++) {
		dq.push_back(mStr[i]);
		updateCnt(1);
	}
}

void appendWord(char mWord[])
{
	for (int i = 0; mWord[i]; i++) {
		if (rev) dq.push_front(mWord[i]);
		else dq.push_back(mWord[i]);
		updateCnt(1);
	}
}

void cut(int k)
{
	for (int i = 0; i < k; i++) {
		updateCnt(-1);
		if (rev) dq.pop_front();
		else dq.pop_back();
	}
}

void reverse()
{
	rev = !rev;
}

int countOccurrence(char mWord[])
{
	string str = mWord;
	if (rev) reverse(str.begin(), str.end());

	int hash = 0;
	for (auto c : str) hash = hash * 27 + c - 96;
	return cnt[hash];
}
#elif 0
#include <deque>
#include <unordered_map>
#include <string>
#include <algorithm>
using namespace std;

deque<char> dq;
unordered_map<string, int> cnt;
bool R;

void update(int n) {
	int idx;
	if (R)
		idx = 0;
	else
		idx = dq.size() - 1;

	string str;
	for (int i = 0; i < 4; i++) {
		if (idx < 0 || idx >= dq.size())
			break;

		if (R)
			str += dq[idx++];
		else
			str.insert(str.begin(), dq[idx--]);

		cnt[str] += n;
	}
}
void init(char mStr[])
{
	cnt.clear();
	dq.clear();
	R = false;
	while (*mStr != '\0') {
		dq.push_back(*mStr++);
		update(1);
	}
}
void appendWord(char mWord[])
{
	while (*mWord != '\0') {
		if (R) dq.push_front(*mWord++);
		else dq.push_back(*mWord++);
		update(1);
	}
}
void cut(int k)
{
	while (k--) {
		update(-1);
		if (R) dq.pop_front();
		else dq.pop_back();
	}
}
void reverse()
{
	R = !R;
}
int countOccurrence(char mWord[])
{
	string str = mWord;
	if (R) reverse(str.begin(), str.end());
	return cnt[str];
}
#elif 0
#include<cstring>
#include<iostream>
using namespace std;
/*
1. 문자열 뒤에 짧은 문자열을 붙인다.
2. 문자열 끝 부분을 삭제한다.
3. 문자열을 뒤집는다.
4. 짧은 문자열이 주어졌을 때, 해당 문자열의 등장 횟수를 센다.

1. 각 테스트 케이스 시작 시 init() 함수가 호출된다.
2. 각 테스트 케이스에서 appendWord() 함수의 호출 횟수는 30,000 이하이다.
3. 각 테스트 케이스에서 cut() 함수의 호출 횟수는 100 이하이다.
4. 각 테스트 케이스에서 reverse() 함수의 호출 횟수는 30,000 이하이다.
5. 각 테스트 케이스에서 countOccurrence() 함수의 호출 횟수는 30,000 이하이다.
*/

char* s[30001];

void init(char mStr[]) {
	//각 테스트 케이스의 처음에 호출된다.
	//해당 테스트 케이스에서 초기 문자열은 mStr 이다.
	//문자열 mStr 은 영문 소문자만으로 구성되어 있으며 ‘＼0’으로 끝난다.
	//mStr 의 길이는 1 이상 30, 000 이하이다.
	//Parameters
	//mStr : 초기 문자열(1 ≤ | mStr | ≤ 30, 000, | a | 는 문자열 a의 길이를 의미한다.)
}

void appendWord(char mWord[]) {
	/*현재 문자열의 끝에 mWord 문자열을 붙인다.
	문자열 mWord 은 영문 소문자만으로 구성되어 있으며 ‘＼0’으로 끝난다.
	mWord 의 길이는 1 이상 4 이하이다.
	Parameters
	mWord : 끝에 붙일 문자열(1 ≤ | mWord | ≤ 4)
	*/
}
void cut(int k) {
	/*현재 문자열의 끝에서부터 k 개 문자를 제거한다.
	이 함수가 호출되는 시점에,
	k 는 현재 문자열 길이보다 작음이 보장된다.
	Parameters
	k : 제거할 문자의 개수(1 ≤ k < 문자열 길이)	*/
}
void reverse() {
	//현재 문자열 전체를 뒤집는다.
}
int countOccurrence(char mWord[]) {
	//현재 문자열에서 mWord 문자열이 부분 문자열(substring)로 등장하는 횟수를 반환한다.
	//문자열 mWord 는 영문 소문자만으로 구성되어 있으며 ‘＼0’으로 끝난다.
	//mWord 의 길이는 1 이상 4 이하이다.
	//문자열이 겹치는 경우는 중복해서 센다.
	//예를 들어 “ababa”에서 “aba”가 등장하는 횟수는 2회이다.
	//현재 문자열 길이에 비례하는 탐색 시간을 가지는 경우 시간 초과가 발생할 수 있다.
	//Parameters
	//mWord : 등장 빈도를 계산할 단어(1 ≤ | mWord | ≤ 4)
	//Returns
	//현재 문자열에서 mWord 단어의 등장 횟수를 반환한다.
	return 1;
}


int main() {
	char a[100] = "ababa";
	char b[100] = "aba";
	//cout << a << " " << b << endl;

	/*char* s = a;
	for (int i = 0; i < 5; i++) {
		cout << s++ << " ";
	}*/

	char* s = a;
	while (s = strstr(s, b)) {
		cout << s++ << endl;
	}


	return 1;
}
#elif 0
#include<iostream>
using namespace std;

struct Trie {
	bool end;
	Trie* ch[11];
	Trie() {
		for (int i = 0; i < 11; i++)
			ch[i] = nullptr;
		end = 0;
	}
	~Trie() {
		for (int i = 0; i < 11; i++)
			if (ch[i])
				delete ch[i];
		end = 0;
	}
	bool insert(const char* s) {
		if (!*s) {
			if (this->end)
				return false;
			else {
				this->end = 1;
				return true;
			}
		}

		int k = *s - '0';
		if (!ch[k]) {
			if (this->end)
				return false;

			ch[k] = new Trie;
		}
		return ch[k]->insert(s + 1);
	}
	bool find(const char* s) {
		if (!*s) {
			if (end) return true;
			else return false;
		}

		int k = *s - '0';
		if (!ch[k])		return false;
		return ch[k]->find(s + 1);
	}
	bool check(const char* s) {
		if (!*s) {
			return true;	// 접두어가 같다
		}

		int k = *s - '0';
		if (!ch[k])
			return false;	// 접두어가 다르다
		return
			ch[k]->check(s + 1);
	}
};

char s[11];
int main() {
	/*
	첫째 줄에 테스트 케이스의 개수 t가 주어진다. (1 ≤ t ≤ 50)
	각 테스트 케이스의 첫째 줄에는 전화번호의 수 n이 주어진다. (1 ≤ n ≤ 10000)
	다음 n개의 줄에는 목록에 포함되어 있는 전화번호가 하나씩 주어진다.
	전화번호의 길이는 길어야 10자리이며, 목록에 있는 두 전화번호가 같은 경우는 없다.
	*/

	setbuf(stdout, NULL);
	freopen("sample_input.txt", "r", stdin);

	int t, n;
	scanf(" %d", &t);
	for (int i = 0; i < t; i++) {
		scanf(" %d", &n);
		Trie trie;
		int ans = 0;
		while (n--) {
			scanf(" %s", s);

			if (!trie.insert(s)) {
				ans = 1;
				break;
			}
		}
		if (ans == 1)
			printf("NO - %d\n", i);
		else
			printf("YES- %d\n", i);
		ans = 0;
	}
	return 1;
}

#elif 0 

#include<iostream>
#include<unordered_map>
#include<unordered_set>
#include<set>
using namespace std;

set<string> 	s;

int main() {

	setbuf(stdout, NULL);
	freopen("sample_input.txt", "r", stdin);

	int N, M, ret = 0;
	string str;

	scanf("%d %d ", &N, &M);
	for (int i = 0; i < N; i++) {
		cin >> str;
		s.insert(str);
	}

	for (int i = 0; i < M; i++) {
		cin >> str;
		if (s.count(str)) 			ret++;
	}

	cout << ret << endl;

	return 0;
}


#elif 0
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif _CRT_SECURE_NO_WARNINGS

#include<iostream>
using namespace std;

char str[501];


struct Trie {
	Trie* ch[26];
	bool end;
	Trie() {
		end = 0;
		//memset(ch, (int)nullptr, sizeof(ch));
		for (int i = 0; i < 26; i++)
		{
			ch[i] = nullptr;
		}
	}
	~Trie() {
		for (int i = 0; i < 26; i++)
		{
			if (ch[i])
			{
				delete ch[i];
			}
		}
	}
	void insert(const char* s) {
		if (!*s) {
			end = 1;
			return;
		}

		int a = *s - 'a';
		if (!ch[a])			ch[a] = new Trie;

		ch[a]->insert(s + 1);
	}
	bool find(const char* s) {
		if (!*s) {
			if (end)			return true;
			else				return false;
		}

		int a = *s - 'a';
		if (!ch[a])			return false;

		return ch[a]->find(s + 1);
	}
};
int main() {

	setbuf(stdout, NULL);
	freopen("sample_input.txt", "r", stdin);


	int N, M, ret = 0;
	Trie trie;

	scanf("%d %d ", &N, &M);
	for (int i = 0; i < N; i++) {
		scanf("%s ", str);
		trie.insert(str);
	}

	for (int i = 0; i < M; i++) {
		scanf("%s ", str);
		if (trie.find(str)) {
			ret++;
			//cout << str << endl;
		}
	}

	cout << ret << endl;

	return 0;
}

#elif 0
#include<bits/stdc++.h>
#define tej __attribute((optimize("Ofast")))
using namespace std;
struct data1 {
	int arr[3];
};
struct compare {
	bool operator()(const data1& a, const data1& b) const {
		return a.arr[0] > b.arr[0];
	}
};
vector<data1> v[101];
void init(int N, int K, int sCity[], int eCity[], int mCost[], int mTime[]) {
	for (int i = 0; i < N; i++) v[i].clear();
	for (int i = 0; i < K; i++) {
		v[sCity[i]].push_back({ mTime[i], mCost[i], eCity[i] });
	}
}

void add(int sCity, int eCity, int mCost, int mTime) {
	v[sCity].push_back({ mTime, mCost, eCity });
	return;
}

int cost(int M, int sCity, int eCity) {
	priority_queue<data1, vector<data1>, compare> pq;
	pq.push({ 0,0,sCity });
	while (!pq.empty()) {
		data1 temp = pq.top();
		if (temp.arr[2] == eCity) return  temp.arr[0];
		pq.pop();
		for (auto i : v[temp.arr[2]]) {
			if ((i.arr[1] + temp.arr[1]) > M) continue;
			pq.push({ temp.arr[0] + i.arr[0], temp.arr[1] + i.arr[1], i.arr[2] });
		}
	}
	return -1;
}
#elif 0
#include<map>
#include<iostream> 
//#include<pair> 
using namespace std;

map<int, pair<int, int>>	c[105];

map<int, int>	m[10];

void init(int N, int K, int sCity[], int eCity[], int mCost[], int mTime[]) {

	for (int i = 0; i < N; i++) 		c[i].clear();

	for (int i = 0; i < K; i++) {
		c[sCity[i]].insert({ eCity[i], {mCost[i], mTime[i]} });
	}



	//cout << c[0].first 


}

int main() {


	c[0].insert({ 1, {1, 1} });
	c[0].insert({ 3, {3, 3} });
	c[0].insert({ 0, {0, 0} });
	c[0].insert({ 2, {2, 2} });
	c[0].insert({ 2, {2, 2} });
	c[0].insert({ 2, {2, 2} });
	c[0].insert({ 2, {2, 2} });

	//c[0].emplace(1, 2, 3);
	//c[2].emplace(4, 5, 6);
	//auto& [e, c, t] : c[0];

	for (const auto& it : c[0]) {
		int key = it.first;
		int v1 = it.second.first;
		int v2 = it.second.second;
		cout << key << "\t" << v1 << '\t' << v2 << endl;

	}
	return 1;
}

#elif 0
int mstrlen(const char* s) {
	int len = 0;
	while (s[len] != '\0')
		len++;
	return len;
}

void mstrcpy(char* dest, const char* src) {
	int i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = src[i];
}

#define MAX_N 2000
#define HASH_C 4001

int temp[HASH_C][HASH_C];

struct Result {
	int correct_cnt;
	char correct[5][11];
} result[HASH_C];

struct lastFind {
	int time;
	char word[11];
} last[MAX_N];

unsigned long hash(const char* str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
	{
		hash = (((hash << 5) + hash) + c) % HASH_C;
	}

	return hash % HASH_C;
}


bool isCandidate(char* ori, char* correct)
{
	int cnt = 0;
	int diff_value = mstrlen(correct) - mstrlen(ori);

	if (diff_value < 0) {
		while (*ori) {
			if (*ori != *correct)
				cnt++;
			else
				correct++;
			ori++;
		}
	}
	else if (diff_value > 0) {
		while (*correct) {
			if (*ori != *correct)
				cnt++;
			else
				ori++;
			correct++;
		}
	}
	else {
		while (*ori) {
			if (*ori++ != *correct++)
				cnt++;
		}
	}

	return cnt == 1 ? true : false;
}

void init(int n) {
	temp[HASH_C][HASH_C] = { 0 };
	result[HASH_C] = { 0 };
	last[MAX_N] = { 0 };
}

int search(int mId, int searchTimestamp, char searchWord[11], char correctWord[5][11])
{
	int ret;
	int i;
	int key = hash(searchWord);

	if (searchTimestamp - last[mId].time <= 10) {
		if (isCandidate(searchWord, last[mId].word)) {
			int hkeyWrong = hash(last[mId].word);
			temp[key][hkeyWrong]++;
			if (temp[key][hkeyWrong] == 3) {
				mstrcpy(result[hkeyWrong].correct[result[key].correct_cnt], searchWord);
				result[hkeyWrong].correct_cnt++;
			}
		}
	}

	ret = result[key].correct_cnt;
	for (i = 0; i < ret; i++) {
		mstrcpy(correctWord[i], result[key].correct[i]);
	}

	last[mId].time = searchTimestamp;
	mstrcpy(last[mId].word, searchWord);

	return ret;
}
#elif 0
#include<unordered_map>
#include<unordered_set>
#include<string>
using namespace std;
int pre[2001];
char word[50001][11];
unordered_map<string, unordered_map<string, unordered_set<int>>> um;

void init(int N) {
	um.clear();
	memset(pre, -0x3f, sizeof(pre));
}

int check(char* s, char* t) {
	int n = strlen(s);
	int m = strlen(t);

	if (abs(n - m) > 1) return 0;

	if (n == m) {
		int cnt = 0;
		for (int i = 0; i < n; i++) {
			cnt += s[i] == t[i];
		}
		return cnt == n - 1;
	}
	else if (n > m) {
		swap(s, t);
		swap(n, m);
	}

	int le = 0, ri = n - 1;
	while (le < n && s[le] == t[le]) le++;
	while (ri >= 0 && s[ri] == t[ri + 1]) ri--;
	return le >= ri;
}

int cnt = 0;
int search(int mId, int searchTimestamp, char searchWord[11], char correctWord[5][11]) {
	int ret = 0;
	cnt++;

	strcpy(word[searchTimestamp], searchWord);

	auto i = um.find(searchWord);
	if (i != um.end()) {
		unordered_map<string, unordered_set<int>>& v = i->second;
		for (auto& i : v) {
			if (i.second.size() >= 3) {
				strcpy(correctWord[ret++], i.first.c_str());
			}
		}
	}

	if (searchTimestamp - pre[mId] <= 10) {
		if (check(searchWord, word[pre[mId]]))
			um[word[pre[mId]]][searchWord].insert(mId);
	}

	pre[mId] = searchTimestamp;
	return ret;
}
#elif 0
#include <bits/stdc++.h>
#include <unordered_set>
#include <set>
using namespace std;
const int MAX = 2005;
pair<int, string> lstsearch[MAX];
map<pair<string, string>, unordered_set<int>> cand;
map<string, vector<string>> cor;


void init(int n) {
	cand.clear();
	cor.clear();
	for (int i = 1; i <= n; i++)
		lstsearch[i] = { -11,"" };
}

bool istypo(string a, string b) {

	int n = a.length();
	int m = b.length();
	if (n > m) {
		swap(a, b);
		swap(n, m);
	}
	if (n == m) {
		int cnt = 0;
		for (int i = 0; i < n; i++)
			if (a[i] != b[i])cnt++;

		return cnt == 1;
	}
	if (n == m - 1) {
		int i = 0, j = 0;
		while (i < n && j < m) {
			if (a[i] != b[j])j++;
			else {
				i++;
				j++;
			}
		}
		return (i == n);
	}

	return false;
}

int search(int mId, int searchTimestamp, char searchWord[11], char correctWord[5][11]) {

	if (lstsearch[mId].first >= searchTimestamp - 10 && istypo(lstsearch[mId].second, string(searchWord))) {
		string typo = lstsearch[mId].second;
		string corr(searchWord);

		if (cand[{typo, corr}].size() < 3) {
			cand[{typo, corr}].insert(mId);
			if (cand[{typo, corr}].size() == 3)
				cor[typo].push_back(corr);
		}

	}

	lstsearch[mId] = { searchTimestamp,string(searchWord) };

	int cnt = 0;
	for (auto k : cor[string(searchWord)]) {
		strcpy(correctWord[cnt++], k.c_str());
	}
	return cnt;


}
#elif 0
#include<iostream>
using namespace std;


struct Trie {
	Trie* ch[26];
	bool end;
	Trie() {
		end = 0;
		memset(ch, (int)nullptr, sizeof(ch));
	}
	~Trie() {
		for (int i = 0; i < 26; i++)
		{
			if (ch[i])
			{
				delete ch[i];
			}
		}
	}
	void insert(const char* s) {
		if (*s == '\0') {
			this->end = true;
		}
		else {
			int a = *s - 'a';
			if (!ch[a])
				ch[a] = new Trie;
			ch[a]->insert(s + 1);
		}
	}
	bool find(const char* s) {
		if (*s == '\0') {
			if (end)
				return true;
			else
				return false;
		}
		else {
			int a = *s - 'a';
			if (!ch[a])
				return false;
			else
				ch[a]->find(s + 1);
		}
	}
};

int main() {

	Trie* root = new Trie;

	root->insert("galaxy");
	root->insert("galacy");
	root->insert("glacy");
	root->insert("gallaxy");
	root->insert("galaxy");
	root->insert("galaxy");
	root->insert("glaxy");
	root->insert("glaxy");
	root->insert("galaxy");

	cout << root->find("galaxy") << endl;
	cout << root->find("glaxy") << endl;
	cout << root->find("gaxy") << endl;
	cout << root->find("galacy") << endl;
	cout << root->find("") << endl;
	cout << root->find("abc") << endl;


	return 1;
}

#elif 0
#include<iostream>
using namespace std;

struct Trie {
	bool is_terminal; // this represents end of string
	Trie* children[ALPHABETS];

	// Constructor
	Trie() : is_terminal(false) {
		memset(children, 0, sizeof(children));
	}

	// Delete all children
	~Trie() {
		for (int i = 0; i < ALPHABETS; ++i) {
			if (children[i])
				delete children[i];
		}
	}

	void insert(const char* key) {
		if (*key == '\0') {
			is_terminal = true;
		}
		else {
			int index = char_to_index(*key);

			if (children[index] == 0)
				children[index] = new Trie();
			children[index]->insert(key + 1);
		}
	}

	Trie* find(const char* key) {
		if (*key == 0) {
			return this;
		}

		int index = char_to_index(*key);
		if (children[index] == 0) {
			return NULL;
		}

		return children[index]->find(key + 1);
	}

	bool string_exist(const char* key) {
		if (*key == 0 && is_terminal) {
			return true;
		}

		int index = char_to_index(*key);
		if (children[index] == 0) {
			return false;
		}
		return children[index]->string_exist(key + 1);
	}

};

#elif 0
#include<iostream>
using namespace std;

int mstrlen(const char* a) {
	int ret = 0;
	while (a[ret] != '\0')
		ret++;
	return ret;
}
void mstrcpy(char* d, const char* s) {
	int k = 0;
	while (s[k] != '\0') {
		d[k] = s[k];
		k++;
	}
	d[k] = s[k];
}

int main() {
	char a[100] = "abcdef";
	char b[100] = "ABCD";
	string c = "1234567890";

	cout << sizeof(a) << endl;
	cout << sizeof(b) << endl;
	cout << sizeof(c) << endl;

	cout << a << endl;
	cout << b << endl;
	cout << c << endl;

	mstrcpy(a, b);
	mstrcpy(b, c);
	cout << a << endl;
	cout << b << endl;


	cout << mstrlen("") << endl;
	cout << mstrlen("abd") << endl;
	return 1;
}
#elif 0
#define MAX_CHARACTER (400005)
#define MAX_WORD (80005)
#define newCharacter() (&cPool[cCnt++])
#define newWord() (&wPool[wCnt++])
struct character {
	char w;
	character* prev;
	character* next;
}st, ed, cPool[MAX_CHARACTER];
character* cursorC;
int cCnt;

struct word {
	character* first;
	word* prev;
	word* next;
}stW, edW, wPool[MAX_WORD];
word* cursorW;
int wCnt;

void insertWord(character* first) {
	word* n = newWord();
	*n = { 0,0,0 };
	n->first = first;
	n->prev = cursorW->prev;
	cursorW->prev->next = n;
	n->next = cursorW;
	cursorW->prev = n;
}

void insertCharacter(char w) {
	character* n = newCharacter();
	*n = { 0,0,0 };
	n->w = w;
	n->prev = cursorC->prev;
	cursorC->prev->next = n;
	n->next = cursorC;
	cursorC->prev = n;
}

void insertString(char mStr) {

	int cond1 = ((cursorC->prev->w == '_') << 1) | (cursorC->w == '_');
	int cond2 = ((cursorC->prev->w != '_' && cursorC->prev->w != '$') << 1) | (cursorC->w != '_' && cursorC->w != '$');
	insertCharacter(mStr);
	if (mStr == '_') {
		switch (cond1)
		{
		case 0:
			if (cursorC->prev != st.next && cursorC != &ed)
				insertWord(cursorW->first);
			insertWord(cursorC->prev);
			if (cursorC->prev != st.next && cursorC != &ed)
				cursorW->first = cursorC;
			break;
		case 1:
			cursorW->first = cursorC->prev;
			break;
		default:
			break;
		}
	}
	else {
		switch (cond2)
		{
		case 0:
			if (cursorC->prev != st.next && cursorC != &ed)
				insertWord(cursorW->first);
			insertWord(cursorC->prev);
			if (cursorC->prev != st.next && cursorC != &ed)
				cursorW->first = cursorC;
			break;
		case 1:
			cursorW->first = cursorC->prev;
			break;
		default:
			break;
		}
	}

}

void init()
{
	cCnt = wCnt = 0;

	st = { '$',0,&ed };
	ed = { '$',&st,0 };
	cursorC = &ed;

	stW = { &st,0,&edW };
	edW = { &ed,&stW,0 };
	cursorW = &edW;
}

char putString(char mStr[])
{
	register char* str = mStr;
	register int param = 0;
	register word* lastW = 0, *firstW = 0;
	bool chk = true;
	while (chk) {
		switch (str[0]) {
		case 'R':
			param = str[1] - '0';
			str += 2;
			while (cursorC != &ed && param) { //end of line
				if (cursorC->next == cursorW->next->first)
					cursorW = cursorW->next;
				cursorC = cursorC->next;
				param--;
			}

			break;
		case 'L':
			param = str[1] - '0';
			str += 2;
			while (cursorC->prev != &st && param) { // st->next is first
				if (cursorC == cursorW->first)
					cursorW = cursorW->prev;
				cursorC = cursorC->prev;
				param--;
			}
			break;
		case 'F':
			param = str[1] - '0';
			str += 2;
			lastW = edW.prev->first->w == '_' ? edW.prev : &edW; //last is _ or word 
			if (cursorW != &edW && cursorW->next != lastW && cursorW->next != &edW) {
				while (cursorW->next != lastW && param) {
					cursorW = cursorW->next;
					if (cursorW->first->w != '_')
						--param;
				}
				cursorC = cursorW->next->first->prev;
			}
			break;
		case 'B':
			param = str[1] - '0';
			str += 2;
			firstW = stW.next->first->w == '_' ? stW.next : &stW; //last is _ or word
			if (cursorW->prev != firstW) {
				while (cursorW->prev != firstW && param) {
					cursorW = cursorW->prev;
					if (cursorW->first->w != '_')
						--param;
				}
				cursorC = cursorW->first;
			}
			break;
		case 'D':
			param = str[1] - '0';
			str += 2;
			while (cursorC != &ed && param) {

				if (cursorC->next == cursorW->next->first) { //move to next word
					if (cursorW->first->next == cursorW->next->first) { //erase cur word
						cursorW->prev->next = cursorW->next;
						cursorW->next->prev = cursorW->prev;
						cursorW = cursorW->next;

						// check same kind word attach
						if (cursorW->prev != &stW && cursorW != &edW) {
							cursorW->prev->next = cursorW->next;
							cursorW->next->prev = cursorW->prev;
							cursorW = cursorW->prev;
						}

					}
					else
						cursorW = cursorW->next;
				}
				else if (cursorC == cursorW->first) // if first change
					cursorW->first = cursorC->next;
				cursorC->prev->next = cursorC->next;
				cursorC->next->prev = cursorC->prev;
				cursorC = cursorC->next;
				param--;
			}
			break;
		default:
			if (*str)
				insertString(*str++);
			else
				chk = false;
			break;
		}
	}
	return cursorC->w;
}

char getWord(int mX)
{

	register word* wst = &stW;
	while (wst != &edW && mX) {
		wst = wst->next;
		if (wst->first->w != '_')
			--mX;
	}
	return mX ? '$' : wst->first->w;
}
#elif 0

#include<list>
using namespace std;

struct Node {
	char ch;
	Node* prev, *next;
}node[200005];


#elif 0

struct Node {
	int id;
	Node* prev, *next;
}node[1000];

struct List {
	Node* pHead = new Node;
	Node* pTail = new Node;
	void initialize() {
		link(pHead, pTail);
	}
	bool isEmpty() {
		return (pHead->next == pTail);
	}
	static void link(Node* f, Node* b) {
		f->next = b;
		b->prev = f;
	}
	static void erase(Node* node) {
		link(node->prev, node->next);
	}
	void insert_back(Node* node) {
		link(pTail->prev, node);
		link(node->next, pTail);
	}
	void splice(List* list) {
		if (list->isEmpty())	return;
		link(pTail->prev, list->pHead->next);
		link(list->pTail->prev, pTail);
		list->initialize();
	}
}list[6][6];

#elif 0

struct Node {
	int id, team;
	Node* prev, *next;
}node[100000];

struct List {
	Node* pHead = new Node;
	Node* pTail = new Node;

	void initialize() {
		link(pHead, pTail);
	}
	bool isEmpty() {
		return (pHead->next == pTail);
	}
	static void link(Node* f, Node* b) {
		f->next = b;
		b->prev = f;
	}
	static void erase(Node* node) {
		link(node->prev, node->next);
	}
	void insert_back(Node* node) {
		link(pTail->prev, node);
		link(node, pTail);
	}
	void splice(List* list) {
		if (list->isEmpty()) return;
		link(pTail->prev, list->pHead->next);
		link(list->pTail->prev, pTail);
		list->initialize();
	}
}list[6][6];
#elif 0
#include <bits/stdc++.h>
using namespace std;
//priority_queue<int>	pq;
vector<int>		v;

int BS_loop(int s, int e, int key) {

	while (s <= e)
	{
		int m = (s + e) / 2;
		if (v[m] == key)
			return m;
		else if (v[m] < key)
			s = m + 1;
		else
			e = m - 1;
	}
	return -1;
}

int BS_rec(int s, int e, int key) {

	if (s > e)
		return -1;
	int m = (s + e) / 2;

	if (v[m] == key)
		return m;
	else if (v[m] < key)
		BS_rec(m + 1, e, key);
	else
		BS_rec(s, m - 1, key);

	//return - 1;
}

int main() {

	//for (int i = 0; i < 100; i++) 
	{
		v.push_back(123);
		v.push_back(323);
		v.push_back(14);
		v.push_back(34);
		v.push_back(999);
		v.push_back(0);
		v.push_back(11);
		v.push_back(666);
	}

	for (auto it : v) {
		cout << it << endl;
	}
	sort(v.begin(), v.end());
	for (auto it : v) {
		cout << it << endl;
	}

	//cout << BS_loop(0, v.size() - 1, 323) << endl;
	cout << BS_rec(0, v.size() - 1, 323) << endl;
	return 1;
}


#elif 0
#include <unordered_map>
using namespace std;
unordered_map<int, int> M;
struct Q {
	int cnt;
	int p, c;
	int cid[3];
}q[18000];
int id, N;

void init(int N, int mId[], int mNum[]) {
	id = 0;
	::N = N;
	M.clear();
	for (int i = 0; i < N; i++) {
		q[++id] = { mNum[i] };
		M[mId[i]] = id;
	}
}

int add(int mId, int mNum, int mParent) {
	int pid = M[mParent];
	if (q[pid].c >= 3) return -1;
	q[++id] = { mNum, pid, };
	M[mId] = id;
	q[pid].c++;
	for (int i = 0; i < 3; i++) {
		if (!q[pid].cid[i]) {
			q[pid].cid[i] = id;
			break;
		}
	}
	int p = id;
	while (p) q[p = q[p].p].cnt += mNum;
	return q[pid].cnt;
}

int remove(int mId) {
	int rid = M[mId];
	if (!rid) return -1;
	int p = q[rid].p;
	q[p].c--;
	for (int i = 0; i < 3; i++) {
		if (q[p].cid[i] == rid) {
			q[p].cid[i] = 0;
			break;
		}
	}
	while (p > 0) {
		q[p].cnt -= q[rid].cnt;
		p = q[p].p;
	}
	q[rid].p = -1;
	M[mId] = 0;
	return p == -1 ? -1 : q[rid].cnt;
}

int distribute(int K) {
	int s = 1, e = 0, i;

	for (i = 1; i <= N; i++)
		e = e > q[i].cnt ? e : q[i].cnt;

	while (s <= e) {
		int m = (s + e) >> 1;
		int res = 0;

		for (i = 1; i <= N; i++)
			res += (q[i].cnt < m) ? q[i].cnt : m;

		if (res <= K)
			s = m + 1;

		else e = m - 1;
	}
	return s - 1;
}

#elif 0
#include<bits/stdc++.h>
using namespace std;

#define NODE_MAX	8000

struct Node {
	int ID;
	int NUM;
	int P, L, R;
} node[NODE_MAX + 5];

unordered_map<int, int>		H;

int id;

void init(int mID, int mNum) {
	id = 0;
	H.insert({ mID, id });
	node[0] = { mID, mNum, 0, 0, 0 };
}

int add(int mID, int mNum, int mParent) {

	id++;
	H.insert({ mID, id });
	int P = H[mParent];

	if (!node[P].L)
		node[P].L = id;
	else if (!node[P].R)
		node[P].R = id;
	else
		return -1;

	node[id] = { mID, mNum, P, 0, 0 };
}

int count(int P) {

}

int remove(int mID) {

	if (H.find(mID) == H.end())	return -1;

	count(node[H[mID]].P);

	H.erase(mID);
}


#elif 0
#include<bits/stdc++.h>
using namespace std;

int main() {

	unordered_map<string, int> um;

	if (um.empty()) {
		cout << "map이 비어 있습니다" << endl;
	}

	um.insert(make_pair("key", 1));
	um["banana"] = 2;
	um.insert({ "melon", 3 });

	cout << "map의 크기는 " << um.size() << endl;

	for (pair<string, int> elem : um) {
		cout << elem.first << " " << elem.second << endl;
	}

	if (um.find("banana") != um.end()) {
		um.erase("banana");
	}

	for (auto elem : um) {
		cout << elem.first << " " << elem.second << endl;
	}
	return 1;
}
#elif 0
// BFS - Queue 
#include<bits/stdc++.h>
using namespace std;
#define rint register int

bool v[9];
vector<int> graph[9]{
	{},
	{ 2,3,8 },
	{ 1, 7 },
	{ 1, 4, 5},
	{ 3, 5},
	{ 3, 4},
	{ 7},
	{ 2,6,8},
	{ 1,7}
};

void bfs(int s) {
	queue<int> q;
	q.push(s);
	v[s] = true;
	while (!q.empty()) {
		int curr = q.front();
		q.pop();
		cout << curr << ' ';
		for (rint i = 0; i < graph[curr].size(); i++) {
			int next = graph[curr][i];
			if (!v[next]) {
				q.push(next);
				v[next] = true;
			}
		}
	}
}

int main() {
	bfs(1);
	return 1;
}
#elif 0

#include<bits/stdc++.h>
using namespace std;
#define rint register int
bool v[9];
vector<int> graph[9] = {
	{},
	{ 2,3,8 },
	{ 1, 7 },
	{ 1, 4, 5},
	{ 3, 5},
	{ 3, 4},
	{ 7},
	{ 2,6,8},
	{ 1,7}
};

void dfs(int s) {
	v[s] = true;
	cout << s << ' ';
	for (rint i = 0; i < graph[s].size(); i++) {
		int next = graph[s][i];
		if (!v[next])
			dfs(next);
	}
}

int main() {
	dfs(1);
	return 1;
}

/*
1 2 7 6 8 3 4 5
*/

#elif 0 
#include<bits/stdc++.h>
using namespace std;

stack<int> s({ 5,2,3,7,1,4 });
queue<int> q({ 5,2,3,7,1,4 });

int main() {

	s.push(5);
	s.push(100);
	s.push(200);
	s.push(300);
	s.push(400);
	s.pop();
	s.push(500);
	while (!s.empty()) {
		printf("%d ", s.top());
		s.pop();
	}

	cout << endl;

	q.push(5);
	q.push(100);
	q.push(200);
	q.push(300);
	q.push(400);
	q.pop();
	q.push(500);
	while (!q.empty()) {
		printf("%d ", q.front());
		q.pop();
	}

	return 1;
}

#elif 0



#include<bits/stdc++.h>
using namespace std;
#define N	12
int a[N]{ 1,9,3,8,4,5,5,9,10,3,4,5 };
int tree[N * 4];



int init(int s, int e, int node) {
	if (s == e)
		return tree[node] = a[s];

	int m = (s + e) / 2;
	return tree[node] = init(s, m, node * 2) + init(m + 1, e, node * 2 + 1);
}


int sum(int s, int e, int node, int l, int r) {
	// 범위 밖
	if (r < s || e < l)		return 0;
	// 범위 내
	else if (l <= s && e <= r)	return tree[node];
	int m = (s + e) / 2;
	return sum(s, m, node * 2, l, r) + sum(m + 1, e, node * 2, l, r);
}

void update(int s, int e, int node, int idx, int diff) {
	// 범위밖
	if (idx < s || e < idx) {
		return;
	}
	// 범위내
	else {
		tree[node] += diff;
		if (s == e) return;
		int m = (s + e) / 2;
		update(s, m, node * 2, idx, diff);
		update(m + 1, e, node * 2, idx, diff);
	}
}

int main() {
	init(0, N - 1, 1);
	cout << "0~12 더하기" << sum(0, N - 1, 1, 0, 12) << endl;
	update(0, N - 1, 1, 5, -5);
	cout << "0~12 더하기" << sum(0, N - 1, 1, 0, 12) << endl;
	return 1;
}

#elif 0
#include <iostream>
#include <vector>
using namespace std;


vector<int>	type[1005];

void init(int N, int M, int mType[], int mTime[]) {


	for (int i = 0; i < N; i++) {
		type[i].clear();
		type[i] = { mType[i] };
	}


}

#elif 0
#include <queue>
using namespace std;

#define NN	4000

vector<pair<int, int>>	map[NN];

priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>>	pq[NN][NN];


void init(int N, int L, int sCity[], int eCity[], int mCost[], int mTime[]) {

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			pq[i][j] = {};
		}
	}

	for (int i = 0; i < L; i++) {
		pq[sCity[i]][eCity[i]].push({ mTime[i], mCost[i] });
	}
}

void add(int sCity, int eCity, int mCost, int mTime) {

	pq[sCity][eCity].push({ mTime, mCost });

}


int cost(int sCity, int eCity, int mCost) {
	int ret = 0;

	pair<int, int> a = pq[sCity][eCity].top();
	int time = a.first;
	int cost = a.second;

	while () {


	}

	return ret;
}

#elif 0
// 코드 참고 : https://github.com/ndb796/python-for-coding-test

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

bool visited[9];
vector<int> graph[9];

// BFS 함수 정의
void bfs(int start) {
	queue<int> q;
	q.push(start); // 첫 노드를 queue에 삽입
	visited[start] = true; // 첫 노드를 방문 처리

	// 큐가 빌 때까지 반복
	while (!q.empty()) {
		// 큐에서 하나의 원소를 뽑아 출력
		int x = q.front();
		q.pop();
		cout << x << ' ';
		// 해당 원소와 연결된, 아직 방문하지 않은 원소들을 큐에 삽입
		for (int i = 0; i < graph[x].size(); i++) {
			int y = graph[x][i];
			if (!visited[y]) {
				q.push(y);
				visited[y] = true;
			}
		}
	}
}

int main(void) {
	// 노드 1에 연결된 노드 정보 저장 
	graph[1].push_back(2);
	graph[1].push_back(3);
	graph[1].push_back(8);

	// 노드 2에 연결된 노드 정보 저장 
	graph[2].push_back(1);
	graph[2].push_back(7);

	// 노드 3에 연결된 노드 정보 저장 
	graph[3].push_back(1);
	graph[3].push_back(4);
	graph[3].push_back(5);

	// 노드 4에 연결된 노드 정보 저장 
	graph[4].push_back(3);
	graph[4].push_back(5);

	// 노드 5에 연결된 노드 정보 저장 
	graph[5].push_back(3);
	graph[5].push_back(4);

	// 노드 6에 연결된 노드 정보 저장 
	graph[6].push_back(7);

	// 노드 7에 연결된 노드 정보 저장 
	graph[7].push_back(2);
	graph[7].push_back(6);
	graph[7].push_back(8);

	// 노드 8에 연결된 노드 정보 저장 
	graph[8].push_back(1);
	graph[8].push_back(7);

	bfs(1);
}

#elif 0

#include<iostream>
#include<vector>
#include<queue>
using namespace std;
#define N	(6) 

vector<pair<int, int>> map[7];
int d[7];

void dijkstra(int start) {
	//priority_queue<pair<int, int>, vector<pair<int,int>>, less<pair<int,int>>> pq ; 


	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>  > pq;
	//queue<pair<int, int>> pq;
	//priority_queue<pair<int, int>> pq;  

	d[start] = 0;
	pq.push(make_pair(start, 0));

	while (!pq.empty())
	{
		int curr = pq.top().first;
		int distance_curr = pq.top().second;
		pq.pop();

		if (d[curr] < distance_curr) continue;

		// curr 부터 연결되어 있는 간선을 순회하면서 각 거리의 최단거리를 업데이트 한다. 
		for (int i = 0; i < map[curr].size(); i++) {
			int next = map[curr][i].first;
			int distance_next = distance_curr + map[curr][i].second;

			if (d[next] > distance_next) {
				d[next] = distance_next;
				pq.push({ next, distance_next });
			}
		}
	}
}

int main() {


	for (int i = 1; i <= N; i++) {
		d[i] = 1e9;
	}

	map[1].push_back({ 2, 2 });
	map[1].push_back({ 3, 5 });
	map[1].push_back(make_pair(4, 1));

	map[2].push_back(make_pair(1, 2));
	map[2].push_back(make_pair(3, 3));
	map[2].push_back(make_pair(4, 2));

	map[3].push_back(make_pair(1, 5));
	map[3].push_back(make_pair(2, 3));
	map[3].push_back(make_pair(4, 3));
	map[3].push_back(make_pair(5, 1));
	map[3].push_back(make_pair(6, 5));

	map[4].push_back(make_pair(1, 1));
	map[4].push_back(make_pair(2, 2));
	map[4].push_back(make_pair(3, 3));
	map[4].push_back(make_pair(5, 1));

	map[5].push_back(make_pair(3, 1));
	map[5].push_back(make_pair(4, 1));
	map[5].push_back(make_pair(6, 2));

	map[6].push_back(make_pair(3, 5));
	map[6].push_back(make_pair(5, 2));

	dijkstra(1);




	for (int i = 1; i <= N; i++) {
		printf("%d ", d[i]);
	}
	return 1;
}


#elif 0
#include <unordered_map>
#include <queue>
#include <vector> 
#include <algorithm>
#include <iostream>
using namespace std;

int emptySize;
priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq; //start, size
unordered_map<int, int> Hash;
int fn;
vector<pair<int, int>> file[12005]; //start, size

void init(int N) {
	emptySize = N;
	pq = {};
	// 시작되는 부분부터 먼저 채워 넣기 위해 
	// 시작 시점을 기준으로 PQ로 넣는다.  (PQ가 시작 시점이 작은 순으로 큐되도록)
	pq.push({ 1, N });
	for (int i = 0; i < fn; i++) file[i].clear();
	fn = 0;
	Hash.clear();
}

int add(int mId, int mSize) {
	if (emptySize < mSize) return -1;

	emptySize -= mSize;
	int fid = Hash[mId] = fn++;

	while (mSize) {
		pair<int, int> a = pq.top();
		// Framgmented 된 빈공간중에 시작 시점이 가장 빠른 조각을 가져온다.  
		pq.pop();
		int start = a.first;
		int space = a.second;

		// 마지막으로 공간을 채우고 나면 			
		// Fragmented 된 공간을 재 계산하여 push()해 준다. 
		if (space > mSize) {
			pq.push({ start + mSize, a.second - mSize });
			space = mSize;
		}

#if 1
		// 추가적으로 저장되고 이어서 저장될 때
		if (file[fid].size() && file[fid].back().first + file[fid].back().second == start) {
			file[fid].back().second += space;
		}
		// 처음 또는 조각되어 저장될 때 
		else
			file[fid].push_back({ start, space });
#endif

		// 채울수 있는 공간만큼만 채우고(빼주고) 아직 못채운 크기만을 남겨준다. 
		mSize -= space;
	}
	return file[fid][0].first;
}

int remove(int mId) {
	int fid = Hash[mId];
	int ret = file[fid].size();
	for (auto v : file[fid]) {
		pq.push(v);
		emptySize += v.second;
	}
	file[fid].clear();
	return ret;
}

int count(int mStart, int mEnd) {
	int ret = 0;
	for (int i = 0; i < fn; i++) {
		for (auto v : file[i]) {
			int s = v.first;
			int e = v.first + v.second - 1;
			if (mStart <= e && s <= mEnd) {
				ret++; break;
			}
		}
	}
	return ret;
}
#elif 0
#include<set>
#include<vector>
#include<unordered_map>
using namespace std;

int mp[5005];
vector<pair<int, int>> g[10005];
int n, dis[5005];

void init(int N, int mBrands[])
{
	n = N;
	for (int i = 0; i < N; i++)
	{
		g[i].clear();
		mp[i] = mBrands[i];
	}
}

void connect(int mHotelA, int mHotelB, int mDistance)
{
	g[mHotelA].push_back({ mHotelB, mDistance });
	g[mHotelB].push_back({ mHotelA, mDistance });
}

int merge(int mHotelA, int mHotelB)
{
	int s = 0;
	int a = mp[mHotelA];
	int b = mp[mHotelB];
	for (int i = 0; i < n; i++)
	{
		if (mp[i] == a || mp[i] == b)
		{
			mp[i] = a;
			s++;
		}
	}
	return s;
}

int move(int mStart, int mBrandA, int mBrandB)
{
	set<pair<int, int>> sp;
	for (int i = 0; i <= n; i++)
		dis[i] = 1000000;
	dis[mStart] = 0;
	int a = 0, b = 0;
	sp.insert({ 0,mStart });

	while (!sp.empty())
	{
		auto it = *sp.begin();
		int node = it.second;
		int dist = it.first;
		sp.erase(it);

		if (dist > dis[node]) continue;

		if (node != mStart) {
			if (mBrandA == mBrandB) {
				if (mp[node] == mBrandA) {
					if (a)
						b = dist;
					else
						a = dist;
				}
			}
			else {
				if (mp[node] == mBrandA)
					if (!a)
						a = dist;
				if (mp[node] == mBrandB)
					if (!b)
						b = dist;
			}
		}

		if (a && b) return a + b;

		for (auto it : g[node])
		{
			if ((dis[node] + it.second) < dis[it.first])
			{
				dis[it.first] = dist + it.second;
				sp.insert({ dis[it.first], it.first });
			}
		}

	}
}
#elif 0
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

int main() {

	unordered_map<string, int> um;

	if (um.empty()) {
		cout << "unordered_map은 비어있습니다" << endl;
	}

	um.insert(make_pair("key", 1));
	um["banana"] = 2;
	um.insert({ "melon",3 });

	cout << "unordered_map의 크기는 " << um.size() << " 입니다" << endl;

	// auto로 해도 무방
	for (pair<string, int> elem : um) {
		//for (auto elem : um) {
		cout << "key : " << elem.first << " value : " << elem.second << endl;
	}

	// find 대신 count로 확인 가능
	if (um.find("banana") != um.end()) {
		um.erase("banana");
	}

	cout << "unordered_map의 크기는 " << um.size() << " 입니다" << endl;
	for (auto elem : um) {
		cout << "key : " << elem.first << " value : " << elem.second << endl;
	}

	return 1;
}


#elif  0
#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

vector<pair<int, int>> q;

bool compare(pair<int, int> a, pair<int, int> b) {
	return a.first > b.first;
}

int main() {

	q.push_back(make_pair(4, 6));
	q.push_back(make_pair(1, 3));
	q.push_back(make_pair(2, 4));
	q.push_back(make_pair(3, 5));


	for (int i = 0; i < q.size(); i++) {
		cout << q[i].first << "\t" << q[i].second << endl;
	}
	cout << endl;

	sort(q.begin(), q.end(), less<pair<int, int>>());

	for (int i = 0; i < q.size(); i++) {
		cout << q[i].first << "\t" << q[i].second << endl;
	}
	return 1;
	cout << endl;
}


#elif 0
#include <iostream>
#include <vector>
using namespace std;

int main() {
	//vector<vector<int>> v(10, vector<int>(5,7));
	vector<pair<int, int>> v[10];

	v[0].push_back(make_pair(0, 2));
	v[0].push_back(make_pair(1, 2));
	v[0].push_back(make_pair(2, 2));
	v[0].push_back(make_pair(3, 2));
	v[0].push_back(make_pair(4, 2));
	v[0].push_back(make_pair(5, 2));


	cout << endl;
	/*for (int i = 0; i < v.size(); i++) {
		for (int j = 0; j < v[i].size(); j++) {
			cout << v[i][j] << " ";
		}
		cout << endl;
	}*/


	return EXIT_SUCCESS;
}

#elif 0
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define V 6 // 정점의 수

// 최단 경로를 찾을 그래프의 인접 행렬 표현
int graph[V][V] = {
	{0, 2, 0, 0, 0, 3},
	{2, 0, 4, 0, 0, 1},
	{0, 4, 0, 5, 0, 0},
	{0, 0, 5, 0, 0, 7},
	{0, 0, 0, 0, 0, 2},
	{3, 1, 0, 7, 2, 0}
};

// 최소 거리 값을 찾기 위한 보조 함수
int minDistance(int dist[], int sptSet[]) {
	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++) {
		if (sptSet[v] == 0 && dist[v] <= min) {
			min = dist[v];
			min_index = v;
		}
	}

	return min_index;
}

// 다익스트라 알고리즘을 사용하여 최단 경로를 찾는 함수
void dijkstra(int start) {
	int dist[V]; // 최단 거리를 저장하는 배열
	int sptSet[V]; // 최단 경로가 확보된 정점을 나타내는 배열

	// 초기화
	for (int i = 0; i < V; i++) {
		dist[i] = INT_MAX;
		sptSet[i] = 0;
	}

	dist[start] = 0; // 시작점의 거리는 0

	// 모든 정점을 방문
	for (int count = 0; count < V - 1; count++) {
		int u = minDistance(dist, sptSet);
		sptSet[u] = 1;

		// 선택된 정점을 통해 인접한 정점들의 최단 거리 갱신
		for (int v = 0; v < V; v++) {
			if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v]) {
				dist[v] = dist[u] + graph[u][v];
			}
		}
	}

	// 결과 출력
	printf("정점\t\t최단 거리\n");
	for (int i = 0; i < V; i++) {
		printf("%d\t\t%d\n", i, dist[i]);
	}
}

int main() {
	int start_vertex = 0; // 시작 정점

	dijkstra(start_vertex);

	return 0;
}

#elif 0
#include<iostream>
#include<vector>
#include<queue>
using namespace std;
#define N	(6) 

vector<pair<int, int>> map[7];
int d[7];

void dijkstra(int start) {
	//priority_queue<pair<int, int>, vector<pair<int,int>>, less<pair<int,int>>> pq ; 
	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>  > pq;
	//priority_queue<pair<int, int>> pq;  

	d[start] = 0;
	pq.push(make_pair(start, 0));

	while (!pq.empty())
	{
		int curr = pq.top().first;
		int distance_curr = pq.top().second;
		//int distance_curr = -pq.top().second;
		pq.pop();
		cout << "curr = " << curr << endl;
		cout << "distance_curr = " << distance_curr << endl;

		if (d[curr] < distance_curr) continue;
		cout << "d[curr] = " << d[curr] << endl;
		cout << "distance_cur = " << distance_curr << endl;
		cout << "d[curr] < distance_curr  = " << (d[curr] < distance_curr) << endl;

		// curr 부터 연결되어 있는 간선을 순회하면서 각 거리의 최단거리를 업데이트 한다. 
		for (int i = 0; i < map[curr].size(); i++) {
			int next = map[curr][i].first;
			int distance_next = distance_curr + map[curr][i].second;
			cout << "next = " << next << endl;
			cout << "distance_next = " << distance_next << endl;

			if (d[next] > distance_next) {
				d[next] = distance_next;
				cout << "d[" << next << "] = " << distance_next << endl;
				pq.push(make_pair(next, distance_next));
				//pq.push(make_pair(next, -distance_next));
			}
		}
	}
}

int main() {

	for (int i = 1; i <= N; i++) {
		d[i] = 1e9;
	}

	map[1].push_back({ 2, 2 });
	map[1].push_back({ 3, 5 });
	map[1].push_back(make_pair(4, 1));

	map[2].push_back(make_pair(1, 2));
	map[2].push_back(make_pair(3, 3));
	map[2].push_back(make_pair(4, 2));

	map[3].push_back(make_pair(1, 5));
	map[3].push_back(make_pair(2, 3));
	map[3].push_back(make_pair(4, 3));
	map[3].push_back(make_pair(5, 1));
	map[3].push_back(make_pair(6, 5));

	map[4].push_back(make_pair(1, 1));
	map[4].push_back(make_pair(2, 2));
	map[4].push_back(make_pair(3, 3));
	map[4].push_back(make_pair(5, 1));

	map[5].push_back(make_pair(3, 1));
	map[5].push_back(make_pair(4, 1));
	map[5].push_back(make_pair(6, 2));

	map[6].push_back(make_pair(3, 5));
	map[6].push_back(make_pair(5, 2));

	dijkstra(1);




	for (int i = 1; i <= N; i++) {
		printf("%d ", d[i]);
	}
	return 1;
}


#elif 0
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include<iostream>
#include<limits>
#define N 6
using namespace std;

int INF(1e9);
int map[6][6] = {
	{0,2,5,1,INF,INF},
	{2,0,3,2,INF,INF},
	{5,3,0,3,1, 5},
	{1,2,3,0,1,INF},
	{INF, INF, 1,1,0,2},
	{INF, INF, 5, INF, 2, 0}
};

int d[N];
bool v[N];


int getSmallidx() {
	int idx = 0;
	int mv = INF;
	for (int i = 0; i < N; i++) {
		if (!v[i] && mv > d[i]) {
			mv = d[i];
			idx = i;
		}
	}
	return idx;
}
void dijkstra(int s) {
	for (int i = 0; i < N; i++) {
		d[i] = map[s][i];
	}
	v[s] = 1;

	for (int i = 0; i < N - 1; i++) {
		int cur = getSmallidx();
		v[cur] = 1;
		//cout << next << endl;		

		for (int j = 0; j < N; j++) {
			if (!v[j]) {
				if (d[j] > d[cur] + map[cur][j]) {
					d[j] = d[cur] + map[cur][j];
				}
			}
		}
	}
}

int main() {
	dijkstra(0);
	cout << endl;
	for (int i = 0; i < N; i++) {
		cout << d[i] << endl;
	}
	return 1;
}
#elif 0
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

//#include<bits/stdc++.h>
#include<iostream>
#include<vector>
#include<typeinfo>
#include<queue>

using namespace std;  //C++의 표준 라이브러리도 사용자가 직접 만든 식별자와 충돌하지 않게 하기 위해 C++은 표준 라이브러리의 모든 기능을 std namespace안에 넣어두게 되었다.

int n;
//vector<int> arr;
int arr[10000];

priority_queue<int> h;

int main() {
	setbuf(stdout, NULL);
	freopen("input.txt", "r", stdin);

	cin >> n;
	for (int i = 0; i < n; i++) {
		int x;
		cin >> x;
		//arr.push_back(x); 
		//arr[i] = x;
		h.push(x);
	}

	for (auto i : arr) {
		//cout << i << endl;
	}

	/*for (int i = 0; i < n; i++) {
		h.push(-arr[i]);
	}*/

	while (!h.empty()) {
		cout << h.top() << endl;
		h.pop();
	}

	return 1;
}


#elif 0
#include <unordered_map>
#include <vector>
#include <iostream>
#include <algorithm>

#define _abs(x) ( (x)>0 ? (x):-(x) )

using namespace std;


int mK, mL;

struct node {
	int id;
	int x, y, c, d;
	int alive;
} NODE[20001];
int NC;

node* newNode() {
	return &NODE[NC++];
}
vector<node*> mylist[41][41];
unordered_map <int, node* > HASH;
vector< node*> templist;

void init(int K, int L)
{
	mK = K;
	mL = L;
	HASH.clear();
	NC = 0;

	for (int i = 0; i < 41; ++i)
		for (int j = 0; j < 41; ++j)
			mylist[i][j].clear();


}

void addSample(int mID, int mX, int mY, int mC)
{
	node* temp = newNode();
	temp->id = mID;
	temp->x = mX;
	temp->y = mY;
	temp->c = mC;
	temp->d = -1;
	temp->alive = true;

	mylist[mX / 100][mY / 100].push_back(temp);

	HASH[mID] = temp;


}

void deleteSample(int mID)
{
	HASH[mID]->alive = false;

}


bool myfunction(node* a, node* b) {
	if (a->d == b->d) {
		if (a->x == b->x) {
			return a->y < b->y;
		}
		else {
			return a->x < b->x;
		}
	}
	else {
		return a->d < b->d;
	}
}

int predict(int mX, int mY)
{
	templist.clear();

	int gx1 = (mX - mL) / 100;
	int gx2 = (mX + mL) / 100;

	int gy1 = (mY - mL) / 100;
	int gy2 = (mY + mL) / 100;


	for (int i = gx1; i <= gx2; i++) {
		for (int j = gy1; j <= gy2; j++) {
			for (auto* it : mylist[i][j]) {
				if (it->alive == false) continue;
				it->d = (_abs(it->x - mX) + _abs(it->y - mY));

				if (mL >= it->d) {

					templist.push_back(it);
				}
			}
		}
	}

	if (templist.size() < mK) { return -1; }

	partial_sort(templist.begin(), templist.begin() + mK, templist.end(), myfunction);

	int count[12] = { 0, 0,0,0,0,0,0,0,0,0,0,0 };
	int maxC = 0;

	int mycount = 0;
	for (auto it : templist) {
		if (mycount++ >= mK) break;

		count[it->c]++;
		if (maxC < count[it->c]) {
			maxC = count[it->c];
		}
	}

	int targetC = 0;
	for (int i = 0; i < 12; i++) {
		if (count[i] == maxC) {
			return i;
		}
	}

}
#elif 0
#include <iostream>
#define M_MAX   40
using namespace std;

int map[M_MAX][M_MAX];

void init() {
	for (int i = 0; i < M_MAX; i++) {
		for (int j = 0; j < M_MAX; j++) {
			map[i][j] = j;
		}
	}
}

void print() {
	for (int i = 0; i < M_MAX; i++) {
		for (int j = 0; j < M_MAX; j++) {
			cout << map[i][j] << " ";
		}
		cout << endl;
	}
}


int main() {
	init();
	print();




	return 1;
}



#elif 0

struct Node {
	int id, team;
	Node* prev, *next;
}node[100000];

struct List {
	Node* pHead = new Node;
	Node* pTail = new Node;

	void initialize() {
		link(pHead, pTail);
	}
	bool isEmpty() {
		return (pHead->next == pTail);
	}
	static void link(Node* f, Node* b) {
		f->next = b;
		b->prev = f;
	}
	static void erase(Node* node) {
		link(node->prev, node->next);
	}
	void insert_back(Node* node) {
		link(pTail->prev, node);
		link(node, pTail);
	}
	void splice(List* list) {
		if (list->isEmpty()) return;
		link(pTail->prev, list->pHead->next);
		link(list->pTail->prev, pTail);
		list->initialize();
	}
}list[6][6];


struct Node {
	int id, team;
	Node* prev, *next;
}node[100000];

struct List {
	Node* phead = new Node;
	Node* ptail = new Node;

	static void link(Node* f, Node* b)
	{
		f->next = b;
		b->prev = f;
	}
	static void erase(Node* node)
	{
		link(node->prev, node->next);
	}
	void insert_back(Node* node)
	{
		link(ptail->prev, node);
		link(node, ptail);
	}
	void initialize()
	{
		link(phead, ptail);
	}
	bool isEmpty()
	{
		return phead->next == ptail;
	}
	void splice(List* list)
	{
		if (list->isEmpty()) return;
		link(ptail->prev, list->phead->next);
		link(list->ptail->prev, ptail);
		list->initialize();
	}
}list[6][6];


void init()
{
	for (int i = 1; i < 6; i++) {
		for (int j = 1; j < 6; j++) {
			list[i][j].initialize();
		}
	}
}

void hire(int mId, int mTeam, int mScore)
{
	node[mId] = { mId, mTeam };
	list[mTeam][mScore].insert_back(node[mId]);
}
void fire(int mId)
{
	List::erase(node[mId]);
}

void updateSoldier(int mId, int mScore)
{
	fire(mId);
	hire(mId, node[mId].team, mScore);
}

void updateTeam(int mTeam, int mChangeScore)
{
	if (mChangeScore > 0) {
		for (int i = 4; i > = 1; i--) {
			int newscore = (i + mChangeScore) > 5 ? 5 : (i + mChangeScore);
			list[mTeam][newscore].spice(&list[mTeam][i]);
		}
	}
	else if (mChangeScore < 0) {
		for (int i = )

	}
}

int bestSoldier(int mTeam)
{

}

#elif 0
#include <iostream>

using namespace std;

class Node {
	friend class DLL;
private:
	int data;
	Node* pNext;
	Node* pPrev;
public:
	Node() {
		data = 0;
		this->pPrev = NULL;
		this->pNext = NULL;
	}
	Node(int data) {
		this->data = data;
		this->pPrev = NULL;
		this->pNext = NULL;
	}
	~Node() {}
};

class DLL {
private:
	Node* pHead;
	Node* pTail;
	Node* pCursor;
public:
	DLL() {
		pHead = new Node();
		pTail = new Node();
		pCursor = new Node();
		pHead->pNext = pTail;
		pTail->pPrev = pHead;
	}
	~DLL() {}
	void insertion(int data) {
		Node* temp = new Node(data);
		pCursor = pHead->pNext;
		pHead->pNext = temp;
		temp->pNext = pCursor;
		pCursor->pPrev = temp;
		temp->pPrev = pHead;
	}
	void deletion(int data) {
		if (pHead->pNext == pTail) cout << "No node exists" << endl;
		else {
			pCursor = pHead->pNext;
			while (pCursor != pTail) {
				if (pCursor->data == data) {
					pCursor->pPrev->pNext = pCursor->pNext;
					pCursor->pNext->pPrev = pCursor->pPrev;
					delete pCursor;
					return;
				}
				else {
					pCursor = pCursor->pNext;
				}
			}
		}
	}

	void traversal() {
		if (pHead->pNext == pTail) cout << "No node exists" << endl;
		else {
			pCursor = pHead->pNext;
			while (pCursor != pTail) {
				cout << pCursor->data << " ";
				pCursor = pCursor->pNext;
			}
			cout << endl;
		}
	}

	void reverseTraversal() {
		if (pTail->pPrev == pHead) cout << "No node exists" << endl;
		else {
			pCursor = pTail->pPrev;
			while (pCursor != pHead) {
				cout << pCursor->data << " ";
				pCursor = pCursor->pPrev;
			}
			cout << endl;
		}
	}

	int size() {
		int size = 0;
		if (pHead->pNext == pTail) return size;
		else {
			pCursor = pHead->pNext;
			while (pCursor != pTail) {
				size++;
				pCursor = pCursor->pNext;
			}
			return size;
		}
	}
};




int main() {

	DLL dll;
	dll.insertion(1);
	dll.insertion(2);
	dll.insertion(3);
	dll.traversal();
	dll.reverseTraversal();
	cout << "After deletion" << endl;
	dll.deletion(3);
	dll.traversal();
	dll.reverseTraversal();
	cout << "size: " << dll.size() << endl;

	return 0;
}
#elif 0
#include <stdio.h>

#define MAX_VERTEX 30

int vertex;
int map[MAX_VERTEX][MAX_VERTEX];
int visit[MAX_VERTEX];

void depthFirstSearch(int v)
{
	int i;
	visit[v] = 1;
	for (i = 1; i <= vertex; i++)
	{
		if (map[v][i] == 1 && !visit[i])
		{
			printf("%d ", i);
			depthFirstSearch(i);
		}
	}
}

int main(void)
{
	int T;
	int test_case;
	int i, j;
	int start;
	int v1;
	int v2;

	scanf("%d", &T);

	for (test_case = 1; test_case <= T; test_case++)
	{
		for (i = 0; i < MAX_VERTEX; i++)
		{
			for (j = 0; j < MAX_VERTEX; j++)
			{
				map[i][j] = 0;
			}
			visit[i] = 0;
		}

		scanf("%d %d", &vertex, &start);

		while (true)
		{
			scanf("%d %d", &v1, &v2);
			if (v1 == -1 && v2 == -1)
			{
				break;
			}
			map[v1][v2] = map[v2][v1] = 1;
		}

		printf("#%d ", test_case);
		printf("%d ", start);
		depthFirstSearch(start);
		printf("\n");
	}

	return 0;
}

#include <stdio.h>

#define MAX_VERTEX 30

int map[MAX_VERTEX][MAX_VERTEX];
int visit[MAX_VERTEX];
int vertex;
int edge;
int maxEdge;
int start;
int end;

void depthFirstSearch(int v, int depth)
{
	int i;
	if (v == end)
	{
		if (maxEdge < 0 || depth < maxEdge)
		{
			maxEdge = depth;
		}
		return;
	}

	visit[v] = 1;
	for (i = 1; i <= vertex; i++)
	{
		if (map[v][i] == 1 && !visit[i])
		{
			depthFirstSearch(i, depth + 1);
			visit[i] = 0;
		}
	}
}


int main(void)
{
	int T;
	int test_case;
	int i;
	int v1;
	int v2;

	scanf("%d", &T);

	for (test_case = 1; test_case <= T; test_case++)
	{
		scanf("%d %d %d %d", &vertex, &edge, &start, &end);

		for (i = 0; i < edge; i++)
		{
			scanf("%d %d", &v1, &v2);
			map[v1][v2] = 1;
		}

		maxEdge = -1;
		depthFirstSearch(start, 0);
		printf("#%d %d\n", test_case, maxEdge);
	}
	return 0;
}

#include <stdio.h>

#define MAX_M 100

int T;    // # of test case
int M;    // # of element in array
int N;    // # of numbers to search
int arr[MAX_M];

void binarySearch(int* arr, int low, int high, int target)
{
	int mid;
	if (low > high)
	{
		printf("-1 ");
		return;
	}

	mid = (low + high) / 2;

	if (target < arr[mid])
	{
		binarySearch(arr, low, mid - 1, target);
	}
	else if (arr[mid] < target)
	{
		binarySearch(arr, mid + 1, high, target);
	}
	else
	{
		printf("%d ", mid);
		return;
	}
}

int main(void)
{
	int targetValue;
	scanf("%d", &T);

	for (int test_case = 1; test_case <= T; test_case++)
	{
		printf("#%d ", test_case);
		scanf("%d %d", &M, &N);

		for (int i = 0; i < M; i++)
		{
			scanf("%d", &arr[i]);
		}

		for (int i = 0; i < N; i++)
		{
			scanf("%d", &targetValue);
			binarySearch(arr, 0, M - 1, targetValue);
		}
		printf("\n");
	}
	return 0;
}



#include <stdio.h>

#define MAX_N 50

int MAP[MAX_N + 2][MAX_N + 2];
int queue[MAX_N * MAX_N][3];
int row;
int column;
int head;
int rear;

int isEmpty()
{
	return (head <= rear) ? 1 : 0;
}

int enqueue(int x, int y, int c)
{
	queue[head][0] = x;
	queue[head][1] = y;
	queue[head][2] = c;
	head++;
	return 1;
}

int dequeue(int *x, int *y, int *c)
{
	if (isEmpty())
	{
		return 0;
	}
	*x = queue[rear][0];
	*y = queue[rear][1];
	*c = queue[rear][2];
	rear++;
	return 1;
}

int breadthFirstSearch()
{
	int x;
	int y;
	int c;

	enqueue(1, 1, 0);
	MAP[1][1] = 0;
	while (!isEmpty())
	{
		dequeue(&x, &y, &c);
		if (x == column && y == row)
		{
			return c;
		}
		if (x + 1 <= column && MAP[x + 1][y])
		{
			enqueue(x + 1, y, c + 1);
			MAP[x + 1][y] = 0;
		}
		if (y + 1 <= row && MAP[x][y + 1])
		{
			enqueue(x, y + 1, c + 1);
			MAP[x][y + 1] = 0;
		}
		if (x - 1 > 0 && MAP[x - 1][y])
		{
			enqueue(x - 1, y, c + 1);
			MAP[x - 1][y] = 0;
		}
		if (y - 1 > 0 && MAP[x][y - 1])
		{
			enqueue(x, y - 1, c + 1);
			MAP[x][y - 1] = 0;
		}
	}
	return -1;
}


int main(void)
{
	int test_case;
	int T;

	scanf("%d", &T);

	for (test_case = 1; test_case <= T; test_case++)
	{
		head = 0;
		rear = 0;
		scanf("%d %d", &row, &column);

		for (int i = 1; i <= row; i++)
		{
			for (int j = 1; j <= column; j++)
			{
				scanf("%d", &MAP[j][i]);
			}
		}
		printf("#%d %d\n", test_case, breadthFirstSearch());
	}
	return 0;
}
#endif



#if 0
﻿#if 1
#include<iostream>
using namespace std;
const int Nmax = 150000 * 2 + 30000;
char a[Nmax];
int s, e, r;

void init(char mStr[]) {
	s = e = 150000 - 1, r = 0;
	while (*mStr)
		a[e++] = *mStr++;
}
void appendStr(char mWord[]) {
	if (r)
		while (*mWord) 	a[s--] = *mWord++;
	else
		while (*mWord) 	a[e++] = *mWord++;
}
void reverse() { r = !r; }

void cut(int k) {
	if (r)
		a[s = s + k] = '\0';
	else
		a[e = e - k] = '\0';
}

int cnt_sub(int l, char p[]) {
	int ret = 0;
	for (int i = s + 1; i <= e - 1 - l; i++) {
		bool matched = true;
		for (int j = 0; j < l; j++) {
			if (p[j] != a[i + j]) matched = false;
		}
		if (matched)
			ret++;
	}
	return ret;
}


int substring(char mWord[]) {
	int ret = 0, int l = 0;
	while (*mWord++) l++;

	if (r) {
		if (l == 4)	swap(mWord[3], mWord[0]), swap(mWord[2], mWord[1]);
		if (l == 3)	swap(mWord[2], mWord[0]);
		if (l == 2)	swap(mWord[1], mWord[0]);
	}

	return cnt_sub(l, mWord);
}

void print_a() {
	for (int i = s + 1; i <= e - 1; i++) {
		cout << a[i] << " ";
	}
	cout << endl;
}
int main() {
	char tem[1000] = "0123456789";
	char tem1[1000] = "abcd";
	char tem2[1000] = "efgh";

	init(tem);
	print_a();
	cut(3);
	print_a();
	reverse();
	cut(3);
	print_a();
	reverse();
	appendStr(tem1);
	print_a();
	appendStr(tem2);
	print_a();
	reverse();
	appendStr(tem1);
	print_a();
	appendStr(tem2);
	print_a();
	cut(4);
	print_a();
	cut(4);
	print_a();
	reverse();
	cut(4);
	print_a();
	cut(4);
	print_a();


}
#elif 0
// KMP
#include<iostream>
using namespace std;

int f[100000];


void getpi(char* p, int M) {
	f[0] = 0;
	for (int j = 0, i = 1; i < M; i++) {
		while (j > 0 && p[j] != p[i])
			j = f[j - 1];
		if (p[j] == p[i])	f[i] = ++j;
		else				f[i] = 0;
	}
	//for (int i = 0; i < strlen(p); i++)
	//	cout << "[" << i << "]" << f[i] << endl;
}
int kmp(int N, char s[], int M, char p[]) {
	int ret = 0;
	getpi(p, M);
	for (int j = 0, i = 0; i < N; i++) {
		while (p[j] != s[i] && j > 0)		j = f[j - 1];
		if (p[j] == s[i]) {
			if (j == M - 1) {
				j = f[j];
				ret++;
			}
			else
				j++;
		}
	}
	return ret;
}


int FindString(int N, char* A, int M, char* B) {
	return kmp(N, A, M, B);
}


#elif 0
#include<iostream>
using namespace std;

int f[100000];
int l = 0;


void getpi(char* p, int M) {
	f[0] = 0;
	for (int j = 0, i = 1; i < M; i++) {
		while (j > 0 && p[j] != p[i])
			j = f[j - 1];
		if (p[j] == p[i])	f[i] = ++j;
		else				f[i] = 0;
	}


	while (i < M)


		for (int i = 0; i < strlen(p); i++)
			cout << "[" << i << "]" << f[i] << endl;
}

int kmp(int N, char s[], int M, char p[]) {
	int ret = 0;
	getpi(p, M);
	for (int j = 0, i = 0; i < N; i++) {
		while (j > 0 && p[j] != s[i])
			j = f[j - 1];
		if (p[j] == s[i]) {
			if (j == M - 1) {
				ret++;
				j = f[j];
			}
			else
				j++;
		}
		return ret;
	}

	int FindString(int N, char* A, int M, char* B) {
		return kmp(N, A, M, B);
	}

	int main() {
		char A[1000] = "banaba";
		char B[1000] = "ababababc";
		FindString(strlen(A), A, strlen(B), B);
	}

#elif 0

#include<iostream>
using namespace std;

int f[100000];
int l = 0;

void getpi(char* p, int M) {
	f[0] = 0;
	for (int j = 0, i = 1; i < M; i++) {
		//if (p[j] != p[i]) j = 0, f[i] = 0;//f[j - 1];
		while (p[j] != p[i] && j > 0)		j = f[j - 1];
		if (p[j] == p[i]) 	f[i] = ++j;
		else				f[i] = 0;
	}
	/*for (int i = 0; i < strlen(p); i++)
		cout << i << "=" << f[i] << endl;*/
}

int kmp(int N, char s[], int M, char p[]) {
	int ret = 0;
	getpi(p, M);
	for (int j = 0, i = 0; i < N; i++) {
		while (p[j] != s[i] && j > 0)		j = f[j - 1];
		if (p[j] == s[i]) {
			if (j == M - 1) {
				j = f[j];
				//cout << "ans =" << i - strlen(p) + 1 << endl;
				ret++;
			}
			else
				j++;
		}
	}
	return ret;
}

int FindString(int N, char* A, int M, char* B) {

	return kmp(N, A, M, B);

}

#elif 0
#include<iostream>
using namespace std;

int f[1000];
int l = 0;

void getpi(char *p) {
	f[0] = 0;
	for (int j = 0, i = 1; i < strlen(p); i++) {
		while (p[j] != p[i] && j > 0)		j = f[j - 1];
		if (p[j] == p[i])	f[i] = ++j;
		else				f[i] = 0;
	}

	for (int i = 0; i < strlen(p); i++)
		cout << i << "=" << f[i] << endl;
}
void kmp(char s[], char p[]) {
	getpi(p);
	//return;
	for (int j = 0, i = 0; i < strlen(s); i++) {
		while (p[j] != s[i] && j > 0)		j = f[j - 1];
		if (p[j] == s[i]) {
			if (j == strlen(p) - 1) {
				j = f[j];
				cout << "ans =" << i - strlen(p) + 1 << endl;
			}
			else
				j++;
		}
	}
}

int FindString(int N, char* A, int M, char* B) {
	int answer = 0;



	return answer;
}

#elif 0

#include<stdio.h>
#define Nmax 20000

int Q[Nmax], f, r, N;

void init() {
	N = 10, f = r = -1;
}
void push_q(int a) {
	Q[++r] = a;
}
int pop_q() {
	return Q[++f];
}
int main() {
	init();
	for (int i = 0; i < N; i++) {
		int tmp;
		scanf(" %d", &tmp);
		push_q(tmp);
	}

	for (int i = 0; i < N; i++) {
		printf("%d\n", pop_q());
	}

	return 0;
}
#elif 0

#include<stdio.h>

char hands[9][99] = {
	"Top", "1 Pair", "2 Pair", "Triple", "Straight",
	"Flush", "Full House","4 Card", "Straight Flush" };

char S[7]; int N[7], P[5];
void max(int& a, int b) {
	if (a < b) a = b;
}

int recognize() {
	int res = 0;

	bool flush = 1;
	for (int i = 1; i < 5; i++)
		if (S[P[i - 1]] != S[P[i]])
			flush = 0;
	if (flush) max(res, 5);

	int c[15] = { 0, };
	for (int i = 0; i < 5; i++)
		c[N[P[i]]]++;

	int pair = 0, triple = 0, four = 0;
	for (int i = 1; i <= 13; i++) {
		if (c[i] == 2) pair++;
		if (c[i] == 3) triple++;
		if (c[i] == 4) four++;
	}
	if (pair == 1) max(res, 1);
	if (pair == 2) max(res, 2);
	if (triple) max(res, 3);
	if (pair && triple) max(res, 6);
	if (four) max(res, 7);

	bool straight = 0;
	c[14] = c[1];
	for (int i = 5; i <= 14; i++) {
		if (c[i - 4] && c[i - 3] && c[i - 2] && c[i - 1] && c[i]) {
			straight = 1;
			break;
		}
	}
	if (straight) max(res, 4);
	if (straight && flush) max(res, 8);
	return res;
}

int pick(int a, int c) {
	int res = 0;
	if (c == 5) {
		res = recognize();
	}
	else if (a < 7) {
		P[c] = a;
		int x = pick(a + 1, c + 1);
		int y = pick(a + 1, c);
		max(res, x);
		max(res, y);
	}
	return res;
}

int T;
int main() {
	int tc; scanf("%d", &T);
	for (int tc = 1; tc <= T; tc++) {
		for (int i = 0; i < 7; i++)
			scanf(" %c %d", &S[i], &N[i]);
		printf("#%d %s\n", tc, hands[pick(0, 0)]);
	}
	return 0;
}
#elif 0

// [1강 4번] 최대 부분 배열
#include<bits/stdc++.h>
using namespace std;
#define Nmax 200000

int a[Nmax], D[Nmax];


int main()
{
	int test_case, T;
	cin >> T;

	for (test_case = 1; test_case <= T; ++test_case)
	{
		int ans = 0;
		int N;
		cin >> N;
		for (int i = 0; i < N; i++)
		{
			cin >> a[i];
			if (i == 0) {
				ans = D[i] = a[i];
				continue;
			}

			D[i] = max(a[i], D[i - 1] + a[i]);
			ans = max(ans, D[i]);
		}

		cout << "#" << test_case << " " << ans << endl;
	}
	return 0;
}

#elif 0

typedef long long ll;
int tmpA[200000];
int tmpB[200000];
int cmp(ll a1, ll b1, ll a2, ll b2) {
	return (((a1 * b2) + b1) >= ((a2 * b1) + b2));
}

void mergeSort(int s, int e, int* a, int* b) {
	if (s >= e)	return;
	int mid = (s + e) / 2;
	mergeSort(s, mid, a, b);
	mergeSort(mid + 1, e, a, b);
	int i = s, j = mid + 1, k = s;
	while (i <= mid && j <= e) {
		if (cmp((ll)a[i], (ll)b[i], (ll)a[j], (ll)b[j])) {
			tmpA[k] = a[i]; tmpB[k++] = b[i++];
		}
		else {
			tmpA[k] = a[j]; tmpB[k++] = b[j++];
		}
	}
	while (i <= mid) {
		tmpA[k] = a[i];
		tmpB[k++] = b[i++];
	}
	while (j <= e) {
		tmpA[k] = a[j];
		tmpB[k++] = b[j++];
	}
	for (i = s; i <= e; i++) {
		a[i] = tmpA[i];
		b[i] = tmpB[i];
	}
}

int MinRailSpeed(int N, int* a, int* b) {
	ll v = 1;
	long rem = 1000000007;
	mergeSort(0, N - 1, a, b);
	for (int i = 0; i < N; i++) {
		v = (int)(((long long)(a[i]) * v + b[i]) % 1000000007);
	}
	return (int)v;
}
#elif 0
#include <bits/stdc++.h>
typedef long long ll;
using namespace std;

pair<int, int> pii[200000];

bool comp(const pair<int, int>& i0, const pair<int, int>& i1) {
	return (((ll)i1.first * i0.second + (ll)i1.second) < ((ll)i0.first * i1.second + (ll)i0.second));
}

int MinRailSpeed(int N, int* a, int* b) {
	int v = 1;
	for (int i = 0; i < N; i++) 	pii[i] = { a[i], b[i] };

	sort(pii, pii + N, comp);

	for (int i = 0; i < N; i++) {
		v = (int)(((long long)(pii[i].first) * v + pii[i].second) % 1000000007);
	}
	return v;
}


#elif 0
//Test 222
//Test 333
//Test 444
//Test 555

#include<iostream>
#include<algorithm>

using namespace std;

bool compare(const pair<int, int>& a, const pair<int, int>& b)
{
	//If the first number is same
	if (a.first == b.first)
		return a.second > b.second; //The second number in Descending order
	return a.first > b.first; //The first number of bigger numbers to move forward -> Descending order
}

int main() {
	int N;
	pair<int, int> arr[1000];
	cin >> N;
	for (int i = 0; i < N; i++) {
		int a, b;
		cin >> a >> b;
		arr[i] = { a,b };
		//cin >> arr[i].first >> arr[i].second;		
	}

	//Descending order
	//sort(arr, arr + N, compare);
	sort(arr, arr + N, compare);

	//Print
	cout << endl;
	for (int i = 0; i < N; i++)
		cout << arr[i].first << " " << arr[i].second << endl;
	return 0;
}
#elif 0
//[1강 3번] 아나그램 
#include<iostream>
using namespace std;
int a[26];

bool compare() {
	for (int i = 0; i < 26; i++) {
		if (a[i] != 0)	return false;
	}
	return true;
}

int FindAnagram(int l1, char* s1, int l2, char* s2) {
	int ans = 0;
	for (int i = 0; i < 26; i++)	a[i] = 0;
	for (int i = 0; i < l1; i++)	a[s1[i] - 'a'] ++;

	// 슬라이딩 윈도우 방식을 위해
	// N-1까지 연산을 해 놓는다.
	for (int i = 0; i < l1 - 1; i++)	a[s2[i] - 'a'] --;

	// 슬라이딩이 하나씩 증가할때 마다 계산을 진행한다.
	for (int i = l1 - 1; i < l2; i++) {
		// 슬라이딩 되면서 a배열의 값을 증가하고 감소하고를 진행하면서 
		// 아나그램인지를 판별한다. 
		a[s2[i] - 'a'] --;
		if (compare())	ans++;
		a[s2[i - (l1 - 1)] - 'a'] ++;
	}
	return ans;
}

#elif 0
#include <stdio.h>
#include <algorithm>
#include <iostream>
using namespace std;

int main() {


	int N, a, b, c, d;
	scanf("%d ", &N);
	for (int tc = 1; tc <= N; tc++) {
		scanf("%d %d %d %d", &a, &b, &c, &d);
		printf("#%d %d\n", tc, max(min(b, d) - max(a, c), 0));
	}


	return 0;
}

#elif 0
#include <stdio.h>
#include <algorithm>
int a, b, c, d, x, y, z, t, T;
int main() {
	scanf("%d", &T);
	for (t = 1; t <= T; t++) {
		scanf("%d%d%d%d", &a, &b, &c, &d);
		x = b > d ? d : b;
		y = a > c ? a : c;
		z = x > y ? x - y : 0;
		printf("#%d %d\n", t, z);
	}
}

#elif 0
#include <bits/stdc++.h>
using namespace std;

int main() {

	cout << INT_MAX << endl;
	cout << UINT_MAX << endl;
	cout << LONG_MAX << endl;
	cout << ULONG_MAX << endl;
	cout << LLONG_MAX << endl;
	cout << ULLONG_MAX << endl;
	return 0;

}

#elif 0

#include <algorithm>
using namespace std;

extern int CalcFinalSpeed(int N, int* a, int* b, int* p);

int arr[20000];
int rank[20000];

int MinRailSpeed(int N, int* a, int* b) {
	int answer = 0;

	for (int i = 0; i < N; i++)
	{
		arr[i] = (a[i] - 1) / b[i];
	}

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (arr[i] < arr[j])
				::rank[i]++;
		}
	}


	sort(a, a + N);


	return answer;
}


#elif 0 
#include <iostream>
#include <algorithm>
using namespace std;

bool compare(int a, int b) {
	return a > b;
}

class Student {
public:
	string name;
	int score;
	Student(string name, int score) {
		this->name = name;
		this->score = score;;
	}
	bool operator < (Student& student) {
		return this->score < student.score;
	}
};
int main() {
	int a[10] = { 10,		3,  4, 5,2,4,6,6, 9, 0 };

	Student student = { Student("Lee", 90) };

	sort(a, a + 10, compare);
	for (int i = 0; i < 10; i++) {
		cout << a[i] << " ";
	}
	return 0;
}

#elif 0
#define NULL 0
#define MAX_NODE	20000
#define MAX_HASH_SIZE	30000

struct Node {
	int id, pos, size;
	Node* prev;
	Node* next;
	Node* up;
	Node* down;
	Node* fprev;
	Node* fnext;
};

Node NodePool[MAX_NODE];
int NodePoolCnt, Usage;
Node* Head;
Node* Top;
Node* Bottom;
Node* FreeHead;

struct HashId {
	int key;
	Node* data;
};
HasId HashIdTbl[MAX_HASH_SIZE];

Node* findId(int key) {
	unsigned long h = key % MAX_HASH_SIZE;
	int cnt = MAX_HASH_SIZE;
	while (HashIdTbl[h].key != -1 && cnt--) {
		if (HashIdTbl[h].key = key) {
			return HashIdTbl[h].data;
		}
	}
	return NULL;
}


#elif 0
#include <iostream>
#include <algorithm>
#include <set>
using namespace std;

void print(set<int>& s, set<int>::iterator it) {
	if (it == s.end())
		cout << "end\n";
	else
		cout << *it << '\n';
}

int main() {
	set<int> s;

	///오름차순으로 저장///
	s = { 5,2,4,1,3,7,6 };

	for (auto it = s.begin(); it != s.end(); ++it)    //O(n)
		cout << *it << ' ';
	cout << '\n';

	for (auto x : s)    //O(nlogn)
		cout << x << ' ';
	cout << '\n';

	return 0;
}
#elif 0
#include <iostream>
#include <set>
#include <string>

using namespace std;

int main() {

	// set
	set<string> s;


	// insert(element)
	s.insert("abc");
	s.insert("abc");
	s.insert("abc");
	s.insert("abc");
	s.insert("abc");
	s.insert("abc");
	s.insert("def");
	s.insert("ghi");
	s.insert("jkl");


	// erase(element)
	s.erase("jkl");


	// empty(), size()
	if (!s.empty()) cout << "s size: " << s.size() << '\n';


	// find(element)
	cout << *s.find("abc") << '\n';
	cout << *s.find("def") << '\n';


	// count(element)
	cout << "abc count: " << s.count("abc") << '\n';


	// begin(), end()
	cout << "traverse" << '\n';
	for (auto it = s.begin(); it != s.end(); it++) {
		cout << "value: " << *it << '\n';
	}

	return 0;

}
#elif 0
#include <iostream>
#include <string>
#include <unordered_map>

int main()
{
	std::unordered_map<std::string, int> scores;
	int hcnt = 0;
	scores["Evan"] = ++hcnt;
	scores["Lee"] = ++hcnt;
	scores["Jung"] = ++hcnt;

	for (auto it = scores.begin(); it != scores.end(); ++it)
	{
		std::cout << it->first << " : " << it->second << std::endl;
	}

	return 0;
}
#elif 0
#include <unordered_map>
#include <set>
#include <queue>
#include <string>
using namespace std;
const int LM = 20003;
int hcnt, tick, totSize;
struct Access {
	int id, tick;
};
struct Range {
	int s, e;
	bool operator<(const Range& r) const {
		return s < r.s;
	}
};
struct File {
	int s, e, tick;
}file[LM];

set<Range> emptyMem;
queue<Access> accessLog;
unordered_map<int, int> htab;

void init(int n) {
	htab.clear();
	totSize = tick = hcnt = 0;
	emptyMem = { {0,n - 1} };
	accessLog = {};
	memset(file, 0, sizeof(file));
}

int access(int dataID, int dataSize) {
	int id = htab[dataID];
	if (!id) htab[dataID] = id = ++hcnt;

	if (!file[id].tick) {
		totSize += dataSize;

		auto it = emptyMem.begin();
	}
}


#elif 0


struct Cache {
	Cache* prev, *next;
	int id, start, size;
};
struct LRU {
	int id;
	LRU* prev, *next;
};

Cache* cachedHead;
Cache* cacheTail;
LRU* lruHead;
LRU* lruTail;


#elif 0

#include <iostream>
using namespace std;

template<typename T>
class C_DATA
{
private:
	T m_tData;

public:
	C_DATA();
	void setData(T tData);
	T getData();

};

int main()
{
	C_DATA<float> cData;
	cData.setData(10.10f);
	printf("%f \n", cData.getData());
	return 0;
}

template<typename T>
C_DATA<T>::C_DATA() :
	m_tData()
{
}

template<typename T>
void C_DATA<T>::setData(T tData)
{
	m_tData = tData;
}

template<typename T>
T C_DATA<T>::getData()
{
	return m_tData;
}



#elif 0
//[H2125] 조립식 컴퓨터
//모범답안

#include <iostream>
using namespace std;

struct Complex {
	int real;
	int imaginary;
};

template<typename T>
T sum(T a, T b) {
	return a + b;
}

template<>
Complex sum(Complex a, Complex b) {
	return Complex{
		a.real + b.real,
		a.imaginary + b.imaginary
	};
}

int main() {
	cout << sum(1, 2) << endl; // 3
	cout << sum(1.1, 2.2) << endl; // 3.3
	Complex{ 1, 2 };

	Complex aa;
	aa = sum(Complex{ 1, 2 }, Complex{ 3, 4 }); // Complex { 4, 6 }
	cout << aa.real << aa.imaginary << endl;

	Complex aaa = Complex{ 10,20 };
	Complex bbb = Complex{ 10,20 };
	aa = sum(aaa, bbb);
	cout << aa.real << aa.imaginary << endl;
	//sum(1.1, 2.2) << endl; // 3.3
	return 0;
}

#elif 0
#include <stdio.h>
#include <string.h>
#define Swap(a, b) { int t = (a); (a) = (b); (b) = t; }

struct Node {
	char data;
	Node* prev, *next;
}node[1000];
int idx = 0;

Node* myalloc() {
	return &node[idx++];
}

Node* pHead = myalloc();
Node* pTail = myalloc();


void addnode(Node* t, char c)
{


}
int main() {
	char a[100], b[100];
	strcpy(a, "Evan Lee");
	strcpy(b, "evan Lee");

	/*int a = 10;
	int b = 20;
	Swap(a, b);
	printf("%d %d \n", a, b);*/
	printf("%s \n", a);
	printf("%s \n", b);
	printf("%d \n", strcmp(a, b));


	return 0;
}

#elif 0
#include<bits/stdc++.h>
using namespace std;
typedef pair<int, int> pii;
typedef pair<pii, int> pp;
#define xx first
#define yy second
pii dir[4] = { {0,-1},{1,0},{0,1},{-1,0} };
pii operator+(const pii& p1, const pii& p2) {
	return pii(p1.xx + p2.xx, p1.yy + p2.yy);
}
vector<pp> r[2];
void init(int N) {
	r[0] = { {{N,N},0} };
	r[1] = { {{N,N},2} };
}
void addRail(int d1, int d2) {
	int pd = r[d1].back().yy, cd;
	if (d2 == 0) cd = (pd + 1) % 4;
	if (d2 == 1) cd = pd;
	if (d2 == 2) cd = (pd + 3) % 4;
	r[d1].emplace_back(r[d1].back().xx + dir[pd], cd);
}
int delRail(int x, int y) {
	for (int i = 0, j; i < 2; ++i) {
		for (j = 0; j < r[i].size(); ++j)
			if (r[i][j].xx == pii(x, y)) break;
		if (j == r[i].size()) continue;
		int ret = r[i].size() - j;
		if (r[0].back().xx + dir[r[0].back().yy] == r[1].back().xx &&
			r[1].back().xx + dir[r[1].back().yy] == r[0].back().xx)
		{
			for (int k = r[i].size() - 1; k > j; --k)
				r[!i].emplace_back(r[i][k].xx, (r[i][k - 1].yy + 2) % 4);
			ret = 1;
		}
		r[i].erase(r[i].begin() + j, r[i].end());
		return ret;
	}
	return 0;
}
#elif 0
const int N = 5000 + 1;
struct Node {
	Node* prev, *next;
	bool direction;
	int r, c;
}node[2 * N][2 * N];

struct List {
	Node* pHead = new Node();
	Node* pTail = new Node();

	static void link(Node* front, Node* back)
	{
		front->next = back;
		back->prev = front;
	}
	static void erase(Node* node)
	{
		link(node->prev, node->next);
	}
	void initialize()
	{
		link(pHead, pTail);
	}
	bool isEmpty()
	{
		return pHead->next == pTail;
	}
	void push_front(Node* node)
	{
		link(pHead, node);
		link(node, pHead->next);
	}
	void push_rear(Node* node)
	{
		link(node, pTail);
		link(pTail->next, node);
	}
}l;

void init(int N)
{
	l.initialize();
}

void addRail(int mFront, int mDirection)
{

	if (mFront == 1)
	{
		Node* Head = l.pHead->next;
		Head->r
			Head->c

			//l.push_front(node);
	}
	else
	{
		l.push_rear(node);
	}
}

int delRail(int mRow, int mCol)
{
	Node* it = &node[mRow][mCol];

	if (it->direction == 1)
	{
		for (; it != pTail; it++)
		{

		}
	}

}



#elif 0
#include<iostream>
#define rint register int 
const int MAX_ID = 100000;
const int MAX_TEAM = 5;
const int MAX_SCORE = 5;
const int MIN_TEAM = 1;
const int MIN_SCORE = 1;

struct Node {
	int id, team;
	Node* prev, *next;
}soldier[MAX_ID + 1];


struct List {
#if 0
	Node* phead = new Node();
	Node* ptail = new Node();
#else 
	Node head, tail;
	Node* phead = &head;
	Node* ptail = &tail;
#endif

	List() { phead->prev = ptail->next = nullptr; }
	//~List() { delete phead, ptail; }
	static void link(Node* front, Node* back) {
		front->next = back;
		back->prev = front;
	}
	static void erase(Node* node) {
		link(node->prev, node->next);
	}
	void initialize() {
		link(phead, ptail);
	}
	void insert_back(Node* node) {
		link(ptail->prev, node);
		link(node, ptail);
	}
	bool isEmpty() {
		return (phead->next == ptail);
	}
	void splice(List* list) {
		if (list->isEmpty())	return;
		link(ptail->prev, list->phead->next);
		link(list->ptail->prev, ptail);
		list->initialize();
	}
}soldierGroup[MAX_TEAM + 1][MAX_SCORE + 1];

void init() {
	for (rint i = 1; i <= 5; i++)
		for (rint j = 1; j <= 5; j++)
			soldierGroup[i][j].initialize();
}

void hire(int mId, int mTeam, int mScore) {
	soldier[mId] = { mId, mTeam };
	soldierGroup[mTeam][mScore].insert_back(soldier + mId);
}

void fire(int mId) {
	List::erase(soldier + mId);
}

void updateSoldier(int mId, int mScore) {
	fire(mId);
	hire(mId, soldier[mId].team, mScore);
}


void updateTeam(int mTeam, int mChangeScore) {
	if (mChangeScore > 0) {
		for (rint i = MAX_SCORE - 1; i >= 1; i--) {
			int newScore = i + mChangeScore;
			if (newScore > MAX_SCORE)newScore = MAX_SCORE;
			soldierGroup[mTeam][newScore].splice(&soldierGroup[mTeam][i]);
		}
	}
	else if (mChangeScore < 0) {
		for (rint i = MIN_SCORE + 1; i <= MAX_SCORE; i++) {
			int newScore = i + mChangeScore;
			if (newScore < MIN_SCORE)newScore = MIN_SCORE;
			soldierGroup[mTeam][newScore].splice(&soldierGroup[mTeam][i]);
		}
	}
}

int bestSoldier(int mTeam) {

	List* maxScoreG;
	for (rint i = 5; i >= 1; i--) {
		if (!soldierGroup[mTeam][i].isEmpty()) {
			maxScoreG = &soldierGroup[mTeam][i];
			break;
		}
	}

	int ret = 0;
	Node* it = maxScoreG->phead->next;
	for (; it != maxScoreG->ptail; it = it->next) {
		if (ret < it->id)	ret = it->id;
	}
	return ret;
}

#elif 0
//#define _Evan_
#include<iostream>
#include<list>
using namespace std;

list<int> li[6][6];

struct Mercenary
{
	int team;
	list<int>::iterator iter;
}mer[100003];

void init() {
	for (int i = 1; i <= 5; i++)
		for (int j = 1; j <= 5; j++)
			li[i][j].clear();
}

void hire(int id, int team, int score) {
	mer[id] = { team, li[team][score].insert(li[team][score].end(), id) };
}

void fire(int id) {
	*mer[id].iter = 0;
}

void updateSoldier(int id, int score) {
	*mer[id].iter = 0;
	hire(id, mer[id].team, score);
}

void updateTeam(int team, int newScore) {
	if (newScore < 0)
		for (int i = 2; i <= 5; i++) {
			int score = max(1, i + newScore);
			li[team][score].splice(li[team][score].end(), li[team][i]);
		}
	else if (newScore > 0)
		for (int i = 4; i >= 1; i--) {
			int score = min(5, i + newScore);
			li[team][score].splice(li[team][score].end(), li[team][i]);
		}
}

int bestSoldier(int team) {
	int ret = 0;
	for (int i = 5; i >= 1; i--) {
		if (li[team][i].empty()) continue;
		for (auto x : li[team][i]) ret = max(x, ret);
		if (ret) break;
	}
	return ret;
}

#elif 0
struct Node {
	int id, team;
	Node* prev, *next;
}node[100000];

struct List {
	Node* phead = new Node;
	Node* ptail = new Node;

	static void link(Node* f, Node* b)
	{
		f->next = b;
		b->prev = f;
	}
	static void erase(Node* node)
	{
		link(node->prev, node->next);
	}
	void insert_back(Node* node)
	{
		link(ptail->prev, node);
		link(node, ptail);
	}
	void initialize()
	{
		link(phead, ptail);
	}
	bool isEmpty()
	{
		return phead->next == ptail;
	}
	void splice(List* list)
	{
		if (list->isEmpty()) return;
		link(ptail->prev, list->phead->next);
		link(list->ptail->prev, ptail);
		list->initialize();
	}
}list[6][6];

void init()
{
	for (int i = 1; i < 6; i++)
		for (int j = 1; j < 6; j++)
			list[i][j].initialize();
}

void hire(int mId, int mTeam, int mScore)
{
	node[mId] = { mId, mTeam };
	list[mTeam][mScore].insert_back(node + mId);
}

void fire(int mId)
{
	List::erase(node + mId);
}

void updateSoldier(int mId, int mScore)
{
	fire(mId);
	hire(mId, node[mId].team, mScore);
}

void updateTeam(int mTeam, int mChangeScore)
{
	if (mChangeScore > 0)
	{
		for (int i = 4; i >= 1; i--)
		{
			int newscore = (i + mChangeScore) > 5 ? 5 : (i + mChangeScore);
			list[mTeam][newscore].splice(&list[mTeam][i]);
		}
	}
	else if (mChangeScore < 0)
	{
		for (int i = 2; i <= 5; i++)
		{
			int newscore = (i + mChangeScore) < 1 ? 1 : (i + mChangeScore);
			list[mTeam][newscore].splice(&list[mTeam][i]);
		}
	}
}

int bestSoldier(int mTeam)
{
	List* l = nullptr;
	for (int i = 5; i > 0; i--)
	{
		if (!list[mTeam][i].isEmpty())
		{
			l = &list[mTeam][i];
			break;
		}
	}

	int ret = 0;
	for (Node* iter = l->phead->next; iter != l->ptail; iter = iter->next)
	{
		if (ret < iter->id)	ret = iter->id;
	}
	return ret;

}
#elif 0
#define MAX_SOLDIER     100000
#define MAX_SCORE       5
#define MAX_TEAM        5

struct SOLDIER {
	int id;
	int team;
	SOLDIER* prev;
	SOLDIER* next;
};
SOLDIER soldier[MAX_SOLDIER + 1];

SOLDIER head[MAX_TEAM + 1][MAX_SCORE + 1]; // NOT pointer, BUT object
SOLDIER tail[MAX_TEAM + 1][MAX_SCORE + 1];

void init() {
	register int i, j;

	for (i = 1; i <= MAX_TEAM; ++i)
		for (j = 1; j <= MAX_SCORE; ++j) {
			head[i][j].next = &tail[i][j];
			tail[i][j].prev = &head[i][j];
		}
}

void hire(int mID, int mTeam, int mScore) {

	register SOLDIER* p = &soldier[mID];
	SOLDIER& T = tail[mTeam][mScore];

	p->id = mID;
	p->team = mTeam;

	T.prev->next = p;
	p->prev = T.prev;
	T.prev = p;
	p->next = &T;
}

void fire(int mID)
{
	register SOLDIER* p = &soldier[mID];
	p->prev->next = p->next;
	p->next->prev = p->prev;
}

void updateSoldier(int mID, int mScore) {

	register SOLDIER* p = &soldier[mID];
	p->prev->next = p->next;
	p->next->prev = p->prev;

	SOLDIER& T = tail[p->team][mScore];

	T.prev->next = p;
	p->prev = T.prev;
	T.prev = p;
	p->next = &T;
}

void updateTeam(int mTeam, int mChangeScore)
{
	if (mChangeScore == 0)
		return;
	int start, end, dir;

	if (mChangeScore > 0)
		start = MAX_SCORE - 1, end = 0, dir = -1;
	else
		start = 2, end = MAX_SCORE + 1, dir = 1;

	for (register int i = start; i != end; i += dir) {
		int ns = (i + mChangeScore > 5) ? 5 : i + mChangeScore;
		ns = ns < 1 ? 1 : ns;

		SOLDIER& Hos = head[mTeam][i];    // original score
		SOLDIER& Tos = tail[mTeam][i];
		SOLDIER& Tns = tail[mTeam][ns];  // new score   

		Tns.prev->next = Hos.next;
		Hos.next->prev = Tns.prev;
		Tns.prev = Tos.prev;
		Tos.prev->next = &Tns;

		Tos.prev = &Hos;
		Hos.next = &Tos;
	}

}

int bestSoldier(int mTeam)
{
	register SOLDIER* p;
	register int i;
	int max_mid = -1;
	for (i = MAX_SCORE; i >= 1; --i) {
		for (p = head[mTeam][i].next; p != &tail[mTeam][i]; p = p->next) {
			if (p->id > max_mid)
				max_mid = p->id;
		}
		if (max_mid > 0)
			break;
	}
	return max_mid;
}

#elif 0
#include<iostream>
using namespace std;

const int N = 10;
struct Node
{
	int a, b;
	Node()
	{
		a = 10, b = 20;
	}
	Node(int a, int b) :a(a), b(b) {}

};
Node node[N];


bool operator<(Node n1, Node n2) { return n1.a < n2.a ? 1 : 0; }


void printnode()
{
	for (int i = 0; i < N; i++)
		cout << node[i].a << " " << node[i].b << endl;
}
int main() {

	printnode();


	//Node A, B;
	Node A(100, 200);
	Node B(1000, 2000);

	//= { 100, 200 };
	//B = { 1000, 2000 };

	if (B < A)
		cout << "A" << endl;
	else
		cout << "B" << endl;


	return 0;
}

#elif 0
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <unordered_map>
using namespace std;
vector<int> hm;
const int N = 10;
int a[N];
volatile int o = 0;

int main() {


	for (int i = 0; i < N; i++)
	{
		hm.push_back(i);
	}

	o = 0;
	for (int& name : hm)
	{
		//cout << name << endl;
		//while (0) {};

		o++;
		//cout << o << endl;
	}

	o = 0;
	for (int i = 0; i < hm.size(); i++)
	{
		o++;
		//cout << o << endl;
	}

	o = 0;
	for (vector<int>::iterator it = hm.begin(); it != hm.end(); it++)
	{
		o++;
		cout << *it << endl;
	}

	cout << hm.front() << endl;
	cout << *(--(hm.end())) << endl;

	return 0;
}


#elif 0

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

string solution(vector<string> participant, vector<string> completion) {
	//string answer = "";
	unordered_map<string, int> participants;
	for (const string& name : participant)
	{
		participants[name]++;
	}

	for (const string& name : completion)
	{
		participants[name]--;
	}

	for (auto pair : participants)
	{
		if (pair.second > 0)
			return pair.first;
	}

	//return answer;
}


#elif 0
#include<unordered_map>
#include<iostream>
#include<string>
using namespace std;

unordered_map<string, int>	htab;

int main() {
	htab["Evan"] = 1;
	htab["Jung"] = 2;
	htab["Choi"] = 1;

	cout << htab["Evan"] << endl;

	return 0;
}
#elif 0
#include<unordered_map>
#include<iostream>
using namespace std;

unordered_map<int, int> m;


void printmap() {
	for (auto& i : m)
		cout << i.first << ":" << i.second << endl;

}
int main() {

	m[10000000] = 1;
	m[20000000] = 2;
	m[30000000] = 3;
	m[40000000] = 4;

	printmap();
	m.clear();

	printmap();

	m[10000000] = 1;
	m[20000000] = 2;
	m[30000000] = 3;
	m[40000000] = 4;

	printmap();

	cout << m[20000000] << endl;
	return 1;
}

#elif 0
#include <iostream>
#include <unordered_map>
using namespace std;
int main() {
	unordered_map<string, int> m; // map<key, value>로 구성
	m.insert({ "Kally", 25 });
	m.insert(pair<string, int>("Tom", 32));
	m.insert(make_pair("John", 73));

	m.inert({ "Kelly", 10 });
	m.insert(pair<string, int>(""))
		m["Evan"] = 51;
	m["Evan"] = 53;
	// insert된 노드 확인
	// 1번째 방법: while 문 처럼 괄호 안에 조건이 맞지 않을 때까지 반복

	m.erase("Kally");
	for (auto iter = m.begin(); iter != m.end(); iter++) {
		cout << iter->first << ":" << iter->second << " ";
	} cout << endl;
	// 2번째 방법: 범위 기반 for문
	for (pair<string, int> iter : m) {
		cout << iter.first << ":" << iter.second << " ";
	} cout << endl;
	for (auto& i : m) {
		cout << i.first << ":" << i.second << " ";
	} cout << endl;

	cout << m["Tom"] << ":" << m["tom"] << endl;

	return 0;
}

#elif 0
#include<unordered_set>
#include<unordered_map>
using namespace std;

const int LM = 10003;
unordered_set<int> chart[LM];
unordered_map<int, int> htab;
int fear[LM], curFear[LM], parent[LM], idcnt;
int total, cnt;

int getId(int x) {
	if (!htab[x]) return htab[x] = ++idcnt;
	return htab[x];
}

void init() {
	htab.clear();
	for (int i = 1; i <= idcnt; i++) chart[i].clear();
	htab[10000] = idcnt = 1;
	total = cnt = 0;
}

int delta;
int dfs(int x, int cmd = 0) {
	if (cmd == 1 && fear[x]) curFear[x] += delta, total += delta;
	if (cmd == 2) total += (fear[x] - curFear[x]), curFear[x] = fear[x];
	if (cmd == 3 && fear[x]) total -= curFear[x], cnt--;
	int sum = curFear[x];
	for (auto y : chart[x]) sum += dfs(y, cmd);
	return sum;
}

int cmdAdd(int id, int teamID, int fearIndex) {
	id = getId(id), teamID = getId(teamID);
	chart[teamID].insert(id);
	curFear[id] = fear[id] = fearIndex;
	parent[id] = teamID;
	total += fearIndex;
	if (fearIndex) cnt++;
	return dfs(teamID);
}

int cmdMove(int id, int teamID) {
	id = getId(id), teamID = getId(teamID);
	chart[teamID].insert(id);
	chart[parent[id]].erase(id);
	parent[id] = teamID;
	return dfs(teamID);
}

int cmdInfect(int id) {
	if (cnt) delta = total / cnt;
	return dfs(getId(id), 1);
}
int cmdRecover(int id) {
	return dfs(getId(id), 2);
}
int cmdRemove(int id) {
	id = getId(id);
	chart[parent[id]].erase(id);
	return dfs(id, 3);
}

#elif 0
#define NULL 0
#define MAX_RIDE 100
#define MAX_HASH_SIZE 200
#define MAX_PEOPLE_SIZE 10000


struct People {
	int num;
	int priority;
};


struct Ride {
	int idx;
	int id;
	int duration;
	int capacity;
	int people;
	int time;
};


People PeoplePool[MAX_PEOPLE_SIZE];
int PeoplePoolCnt;
Ride RidePool[MAX_RIDE];
Ride* RideArr[MAX_RIDE];
int RideCnt;


struct HashId {
	int key;
	Ride* data;
};
HashId HashIdTbl[MAX_HASH_SIZE];


Ride* findId(int key) {
	unsigned long h = key % MAX_HASH_SIZE;
	int cnt = MAX_HASH_SIZE;


	while (HashIdTbl[h].key != -1 && cnt--) {
		if (HashIdTbl[h].key == key) {
			return HashIdTbl[h].data;
		}
		h = (h + 1) % MAX_HASH_SIZE;
	}
	return NULL;
}


void addId(int key, Ride* data) {
	unsigned long h = key % MAX_HASH_SIZE;


	while (HashIdTbl[h].key != -1) {
		h = (h + 1) % MAX_HASH_SIZE;
	}


	HashIdTbl[h].key = key;
	HashIdTbl[h].data = data;
}


People* Heap[MAX_RIDE][MAX_PEOPLE_SIZE];
int HeapSize[MAX_RIDE];


void heapPush(int rideIdx, People* value) {
	Heap[rideIdx][HeapSize[rideIdx]] = value;


	int current = HeapSize[rideIdx];
	while (current > 0 && Heap[rideIdx][current]->priority > Heap[rideIdx][(current - 1) / 2]->priority) {
		People* temp = Heap[rideIdx][(current - 1) / 2];
		Heap[rideIdx][(current - 1) / 2] = Heap[rideIdx][current];
		Heap[rideIdx][current] = temp;
		current = (current - 1) / 2;
	}


	HeapSize[rideIdx]++;
}


People* heapPop(int rideIdx) {
	if (HeapSize[rideIdx] <= 0) {
		return NULL;
	}


	People* ret = Heap[rideIdx][0];
	HeapSize[rideIdx]--;


	Heap[rideIdx][0] = Heap[rideIdx][HeapSize[rideIdx]];


	int current = 0;
	while (current * 2 + 1 < HeapSize[rideIdx]) {
		int child;
		if (current * 2 + 2 == HeapSize[rideIdx]) {
			child = current * 2 + 1;
		}
		else {
			child = Heap[rideIdx][current * 2 + 1]->priority > Heap[rideIdx][current * 2 + 2]->priority ? current * 2 + 1 : current * 2 + 2;
		}


		if (Heap[rideIdx][current]->priority > Heap[rideIdx][child]->priority)
			break;


		People* temp = Heap[rideIdx][current];
		Heap[rideIdx][current] = Heap[rideIdx][child];
		Heap[rideIdx][child] = temp;


		current = child;
	}
	return ret;
}


People* newPeople(int priority, int num) {
	People* ret = &PeoplePool[PeoplePoolCnt++];
	ret->priority = priority;
	ret->num = num;
	return ret;
}


void process(Ride* ride, int tStamp) {
	while (ride->time <= tStamp && HeapSize[ride->idx] > 0) {
		int remainder = ride->capacity;
		while (remainder > 0 && HeapSize[ride->idx] > 0) {
			People* curr = Heap[ride->idx][0];
			if (remainder < curr->num) {
				curr->num -= remainder;
				ride->people -= remainder;
				break;
			}


			remainder -= curr->num;
			ride->people -= curr->num;
			heapPop(ride->idx);
		}
		ride->time += ride->duration;
	}
}


////////////////////////////////////////////////


void init(int N, int mId[], int mDuration[], int mCapacity[]) {
	RideCnt = PeoplePoolCnt = 0;
	for (int i = 0; i < MAX_HASH_SIZE; ++i) {
		HashIdTbl[i].key = -1;
	}


	for (int i = 0; i < MAX_RIDE; ++i) {
		HeapSize[i] = 0;
	}


	for (int i = 0; i < N; ++i) {
		Ride* ride = &RidePool[RideCnt++];
		addId(mId[i], ride);
		ride->idx = i;
		ride->id = mId[i];
		ride->duration = mDuration[i];
		ride->capacity = mCapacity[i];
		ride->people = ride->time = 0;
		RideArr[i] = ride;
	}
}


int add(int tStamp, int mId, int mNum, int mPriority) {
	Ride* ride = findId(mId);
	process(ride, tStamp - 1);
	if (ride->time < tStamp) {
		ride->time = tStamp;
	}


	ride->people += mNum;
	heapPush(ride->idx, newPeople(mPriority, mNum));
	process(ride, tStamp);


	if (!HeapSize[ride->idx])
		return 0;
	return Heap[ride->idx][0]->priority;
}


void search(int tStamp, int mCount, int mId[], int mWait[]) {
	for (int i = 0; i < RideCnt; ++i) {
		process(&RidePool[i], tStamp);
	}


	for (int i = 0; i < mCount; ++i) {
		for (int j = i + 1; j < RideCnt; ++j) {
			if (RideArr[i]->people < RideArr[j]->people
				|| (RideArr[i]->people == RideArr[j]->people && RideArr[i]->id < RideArr[j]->id)) {
				Ride* tmp = RideArr[i];
				RideArr[i] = RideArr[j];
				RideArr[j] = tmp;
			}
		}
		mId[i] = RideArr[i]->id;
		mWait[i] = RideArr[i]->people;
	}
}


#elif 0
#include<list>
#include<iostream>
using namespace std;


void printlist(list<int> l) {
	list<int>::iterator it;
	for (it = l.begin(); it != l.end(); ++it) {
		cout << *it << endl;
	}
}
int main() {
	list<int> l;
	list<int>::iterator it, check;

	l.push_back(1);
	l.push_back(2);
	l.push_back(3);
	l.push_back(4);


	check = l.insert(l.begin(), 1000);

	it = l.begin();



	cout << *check << endl;
	cout << *it << endl;
	//cout << *l.begin() << endl;

	printlist(l);
	return 1;
}


#elif 0
#define N 100000
#define Max(a,b)	((a)>(b)?(a):(b))
int func1() {
	int ret = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			ret = Max(i, j);
		}
	}
	return ret;
}
int func2() {
	int ret = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (i < j)	ret = j;
		}
	}
	return ret;
}


int main() {

	func1();

	func2();

}

#elif 0
const int MIN_ID = 1;
const int MAX_ID = 100000;
const int MIN_TEAM = 1;
const int MAX_TEAM = 5;
const int MIN_SCORE = 1;
const int MAX_SCORE = 5;


struct Node
{
	int id;
	int team;
	Node* prev;
	Node* next;
} soldier[MAX_ID + 1];


struct List
{
	Node head;
	Node tail;


	static void link(Node* front, Node* back)
	{
		front->next = back;
		back->prev = front;
	}


	static void erase(Node* node)
	{
		link(node->prev, node->next);
	}


	void initialize()
	{
		link(&head, &tail);
	}


	void insert(Node* node)
	{
		link(tail.prev, node);
		link(node, &tail);
	}


	bool isEmpty()
	{
		return (head.next == &tail);
	}


	void splice(List* list)
	{
		if (list->isEmpty())
			return;
		link(tail.prev, list->head.next);
		link(list->tail.prev, &tail);
		list->initialize();
	}
} soldierGroup[MAX_TEAM + 1][MAX_SCORE + 1];


void init()
{
	for (int i = MIN_TEAM; i <= MAX_TEAM; i++)
		for (int j = MIN_SCORE; j <= MAX_SCORE; j++)
			soldierGroup[i][j].initialize();
}


void hire(int mID, int mTeam, int mScore)
{
	soldier[mID].id = mID;
	soldier[mID].team = mTeam;
	soldierGroup[mTeam][mScore].insert(soldier + mID);
}


void fire(int mID)
{
	List::erase(soldier + mID);
}


void updateSoldier(int mID, int mScore)
{
	List::erase(soldier + mID);
	soldierGroup[soldier[mID].team][mScore].insert(soldier + mID);
}


void updateTeam(int mTeam, int mChangeScore)
{
	if (mChangeScore > 0)
	{
		for (int i = MAX_SCORE - 1; i >= MIN_SCORE; i--)
		{
			int newScore = i + mChangeScore;
			if (newScore > MAX_SCORE)
				newScore = MAX_SCORE;
			soldierGroup[mTeam][newScore].splice(&soldierGroup[mTeam][i]);
		}
	}
	else if (mChangeScore < 0)
	{
		for (int i = MIN_SCORE + 1; i <= MAX_SCORE; i++)
		{
			int newScore = i + mChangeScore;
			if (newScore < MIN_SCORE)
				newScore = MIN_SCORE;
			soldierGroup[mTeam][newScore].splice(&soldierGroup[mTeam][i]);
		}
	}
}


int bestSoldier(int mTeam)
{
	List* maxScoreGroup;
	for (int i = MAX_SCORE; i >= MIN_SCORE; i--)
	{
		if (!soldierGroup[mTeam][i].isEmpty())
		{
			maxScoreGroup = &soldierGroup[mTeam][i];
			break;
		}
	}


	int maxId = MIN_ID - 1;
	Node* maxScoreSoldier = maxScoreGroup->head.next;
	while (maxScoreSoldier != &(maxScoreGroup->tail))
	{
		if (maxId < maxScoreSoldier->id)
			maxId = maxScoreSoldier->id;
		maxScoreSoldier = maxScoreSoldier->next;
	}
	return maxId;
}
#elif 0
#include<iostream> 
using namespace std;
#define rint register int
const int MIN_ID = 1;
const int MAX_ID = 100000;
const int MIN_TEAM = 1;
const int MAX_TEAM = 5;
const int MIN_SCORE = 1;
const int MAX_SCORE = 5;

struct Node {
	int id, team;
	Node* prev, *next;
}soldier[MAX_ID + 1];

struct List {
	Node* phead = new Node();
	Node* ptail = new Node;

	List() { phead->prev = ptail->next = nullptr; }
	~List() { delete phead, ptail; }

	static void link(Node* front, Node* back) {
		front->next = back;
		back->prev = front;
	}
	static void erase(Node* node) {
		link(node->prev, node->next);
	}
	void initialize() {
		link(phead, ptail);
	}
	void insert_back(Node* node) {
		link(ptail->prev, node);
		link(node, ptail);
	}
	bool isEmpty() {
		return (phead->next == ptail);
	}
	void splice(List* list) {
		if (list->isEmpty()) return;
		link(ptail->prev, list->phead->next);
		link(list->ptail->prev, ptail);
		list->initialize();
	}
}soldierGroup[MAX_TEAM + 1][MAX_SCORE + 1];

void init() {
	for (rint i = 1; i <= MAX_TEAM; i++)
		for (rint j = 1; j <= MAX_SCORE; j++)
			soldierGroup[i][j].initialize();
}

void hire(int mID, int mTeam, int mScore) {
	soldier[mID] = { mID, mTeam };
	soldierGroup[mTeam][mScore].insert_back(&soldier[mID]);
}

void fire(int mID) {
	List::erase(&soldier[mID]);
}

void updateSoldier(int mID, int mScore) {
	fire(mID);
	hire(mID, soldier[mID].team, mScore);
}

void updateTeam(int mTeam, int mChangeScore) {
	if (mChangeScore > 0) {
		for (rint i = MAX_SCORE - 1; i >= 1; i--) {
			int newScore = i + mChangeScore;
			if (newScore > MAX_SCORE)newScore = MAX_SCORE;
			soldierGroup[mTeam][newScore].splice(&soldierGroup[mTeam][i]);
		}
	}
	else if (mChangeScore < 0) {
		for (rint i = MIN_SCORE + 1; i <= MAX_SCORE; i++) {
			int newScore = i + mChangeScore;
			if (newScore < MIN_SCORE)newScore = MIN_SCORE;
			soldierGroup[mTeam][newScore].splice(&soldierGroup[mTeam][i]);
		}
	}
}

//int bestSoldier(int mTeam){	
//	List* maxScoreGroup = nullptr;
//	int ret = 0;
//	for (rint i = MAX_SCORE; i >= MIN_SCORE; i--) {		
//		if (!soldierGroup[mTeam][i].isEmpty()) {
//			maxScoreGroup = &soldierGroup[mTeam][i];
//			break;
//		}
//	}	
//	
//	Node* maxSoldier = maxScoreGroup->phead->next;
//	while (maxSoldier != maxScoreGroup->ptail) {
//		if (ret < maxSoldier->id)	ret = maxSoldier->id;
//		maxSoldier = maxSoldier->next;
//	}	
//	return ret;
//}

int bestSoldier(int mTeam) {

	List* maxScoreG;
	for (rint i = 5; i >= 1; i--) {
		if (!soldierGroup[mTeam][i].isEmpty()) {
			maxScoreG = &soldierGroup[mTeam][i];
			break;
		}
	}

	int ret = 0;
	Node* it = maxScoreG->phead->next;
	for (; it != maxScoreG->ptail; it = it->next) {
		if (ret < it->id)	ret = it->id;
	}
	return ret;
}


//
//int main() {
//	init();
//	hire(1, 1, 5);
//	hire(2, 1, 5);
//	hire(3, 1, 5);
//	hire(4, 1, 5);
//	hire(5, 1, 5);
//
//	hire(100, 1, 5);
//	
//	Node* it = soldierGroup[1][5].head.next;
//	for (it ; it->next != nullptr; it = it->next) {
//		cout << it->mId << endl;
//	}
//	cout << bestSoldier(1) << endl;
//
//	//cout << bestSoldier(1) << endl;
//}

#elif 0

#include<list>
#include<algorithm>
using namespace std;

list<int> li[6][6];

struct Mercenary {
	int team;
	list<int>::iterator iter;
}mer[100003];

void init() {
	for (int i = 1; i <= 5; i++)
		for (int j = 1; j <= 5; j++)
			li[i][j].clear();
}

void hire(int id, int team, int score) {
	mer[id] = { team, li[team][score].insert(li[team][score].end(), id) };
}

void fire(int id) {
	*mer[id].iter = 0;
}

void updateSoldier(int id, int score) {
	*mer[id].iter = 0;
	hire(id, mer[id].team, score);
}

void updateTeam(int team, int newScore) {
	if (newScore < 0)
		for (int i = 2; i <= 5; i++) {
			int score = max(1, i + newScore);
			li[team][score].splice(li[team][score].end(), li[team][i]);
		}
	else if (newScore > 0)
		for (int i = 4; i >= 1; i--) {
			int score = min(5, i + newScore);
			li[team][score].splice(li[team][score].end(), li[team][i]);
		}
}

int bestSoldier(int team) {
	int ret = 0;
	for (int i = 5; i >= 1; i--) {
		if (li[team][i].empty()) continue;
		for (auto x : li[team][i]) ret = max(x, ret);
		if (ret) break;
	}
	return ret;
}
#elif 0
#define MAX 100001

struct Node {
	int team;
	int next, prev;
}P[MAX];

struct Team {
	int h[6];
	int t[6];
}T[6];

void init() {
	for (int i = 1; i < 6; i++)
		for (int j = 1; j < 6; j++) T[i].h[j] = T[i].t[j] = -1;
}

void hire(int mID, int mTeam, int mScore) {
	P[mID].team = mTeam;
	P[mID].next = -1;
	if (T[mTeam].h[mScore] < 0) {
		T[mTeam].h[mScore] = T[mTeam].t[mScore] = mID;
		P[mID].prev = -1;
	}
	else {
		P[mID].prev = T[mTeam].t[mScore];
		P[T[mTeam].t[mScore]].next = mID;
		T[mTeam].t[mScore] = mID;
	}
}

void fire(int mID) {
	if (P[mID].prev < 0) {
		for (int i = 1; i < 6; i++) {
			if (T[P[mID].team].h[i] == mID) {
				T[P[mID].team].h[i] = P[mID].next;
				P[mID].next > -1 ? P[P[mID].next].prev = -1 : T[P[mID].team].t[i] = -1;
				break;
			}
		}
	}
	else {
		P[P[mID].prev].next = P[mID].next;
		P[mID].next > -1 ? P[P[mID].next].prev = P[mID].prev : 0;
	}
	for (int i = 1; i < 6; i++)
		T[P[mID].team].t[i] == mID ? T[P[mID].team].t[i] = P[mID].prev : 0;
}

void updateSoldier(int mID, int mScore) {
	fire(mID);
	hire(mID, P[mID].team, mScore);
}

void updateTeam(int mTeam, int mChangeScore) {
	int h[6], t[6];
	for (int i = 1; i < 6; i++) h[i] = t[i] = -1;
	for (int i = 1; i < 6; i++) {
		if (T[mTeam].h[i] < 0) continue;
		int j = i + mChangeScore;
		j < 1 ? j = 1 : j>5 ? j = 5 : 0;
		if (h[j] < 0) {
			h[j] = T[mTeam].h[i];
			t[j] = T[mTeam].t[i];
		}
		else {
			P[t[j]].next = T[mTeam].h[i];
			P[T[mTeam].h[i]].prev = t[j];
			t[j] = T[mTeam].t[i];
		}
	}
	for (int i = 1; i < 6; i++) {
		T[mTeam].h[i] = h[i];
		T[mTeam].t[i] = t[i];
	}
}

int bestSoldier(int mTeam) {
	int ID = -1;
	for (int i = 5; i > 0; i--) {
		if (T[mTeam].h[i] < 0) continue;
		int id = T[mTeam].h[i];
		while (id > -1) {
			ID < id ? ID = id : 0;
			id = P[id].next;
		}
		break;
	}
	return ID;
}

#elif 0
/*
1<= mNum <= 100
0<= mId <= 1,000,000,000
1<= tStamp <= 100,000,000
add() 10,000회
전체 함수 호출 20,000회
*/



void iniit(int mNum, int mId[], int mCapacity[], int mDuration) {

}

void add(int tSramp, int mId, int num, int mPriority) {

}

void search(int tStamp, int mNum, int mId[], int mWait[]) {

}

#elif 0
#include<iostream>

#define rint register int

class TRIE {
private:
	TRIE* child[26];
public:
	TRIE() {
		for (rint i = 0; i < 26; i++)	child[i] = nullptr;
	}

	~TRIE() {
		for (rint i = 0; i < 26; i++)	if (child[i] != nullptr)	delete child[i];
	}

	void add(char* s) {
		if (*s == '\0') {
			return;
		}
		else {
			int n = *s - 'a';
			if (child[n] == nullptr)	child[n] = new TRIE();
			child[n]->add(s + 1);
		}
	}

	bool find(char* s) {
		if (*s == '\0') {
			return 1;
		}
		else {
			int n = *s - 'a';
			if (child[n] == nullptr)	return 0;
			child[n]->find(s + 1);
		}
	}
};

int main() {

	TRIE* t = new TRIE;
	char a[1000];
	for (rint i = 0; i < 5; i++) {
		std::cin >> a;
		t->add(a);
	}

	for (rint i = 0; i < 5; i++) {
		std::cin >> a;
		std::cout << t->find(a) << std::endl;
	}

	return 1;
}
#elif 0
//#define rint register int
#include<iostream>
#include<string.h>

using namespace std;

struct Trie {
	Trie* child[26];

	Trie() {
		memset(child, 0, sizeof(child));
	}
	~Trie() {
		for (int i = 0; i < 26; i++) {
			if (child[i])
				delete(child[i]);
		}
	}
	void insert(const char* s) {
		if (*s == '\0') return;
		else {
			int n = *s - 'a';
			if (child[n] == NULL) {
				child[n] = new Trie;
			}
			child[n]->insert(s + 1);
		}
		return;
	}
	Trie* find(const char* s) {
		if (*s == '\0')	return this;
		else {
			int n = *s - 'a';
			if (child[n] == NULL)
				return NULL;
			return child[n]->find(s + 1);
		}
	}

};

int main() {


	Trie* tr = new Trie;
	tr->insert("HELLO");
	tr->insert("HELL");
	tr->insert("HALO");
	tr->insert("TRAVEL");

	cout << tr->find("HELLO") << endl;
	cout << tr->find("HALO") << endl;
	cout << tr->find("TRAVELEEEEE") << endl;
	cout << tr->find("BEE") << endl;

	delete tr;
	return 1;
}
#elif 0
#define rint register int
const int ALPS = 26;
#include<stdio.h>

class TRI {
private:
	TRI* child[26];
public:
	TRI TN[100000];
	TRI() {
		for (rint i = 0; i < ALPS; i++) {
			child[i] = nullptr;
		}
	}

	~TN() {

	}
	int tonum(char c) { return c - 'a'; }
	void insert(const char* s) {
		if (*s == '\0')	return;
		int n = tonum(*s);
		if (child[n] = NULL) {
			child[n] = &tri[idx++];
		}
		child[n]->insert(s + 1);
	}

	bool find(const char* s) {
		if (*s == '\0')	return 1;
		int n = tonum(*s);
		if (child[n] = NULL) {
			return 0;
		}
		child[n]->find(s + 1);
	}
};

TN tri[1000000];
int idx = 0;


int main() {
	int ret = -1;
	//TN root;
	tri[0].insert("like");
	ret = tri[0].find("like");
	printf("%d  \n", ret);
	return 1;
}
#elif 0

#include<stdio.h> 
#include<string.h>
#include<ctype.h>

#define rint	register int
#define ALP		26 



int idx = 0;
struct TN {
	TN* child[26];
	TN() {
		for (rint i = 0; i < ALP; i++) 	child[i] = '\0';
	}
	~TN() {
		for (rint i = 0; i < ALP; i++) 	if (child[i] != NULL) delete child[i];
	}

	TN* myalloc() {
		return &Tri[idx++];
	}

	void insert(char* str) {
		if (*str == '\0')	return;

		int n = *str - 'a';
		if (child[n] == NULL)
			child[n] = myalloc();

		child[n]->insert(str + 1);
	}
};
TN Tri[100000];
int idx = 0;




#elif 0
#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;
const int ALP = 26;

class TN {
private:
	TN* c[ALP];

public:
	TN() {
		for (int i = 0; i < ALP; i++)
			c[i] = NULL;
	}
	~TN() {
		for (int i = 0; i < ALP; i++)
			if (c[i] != NULL)
				delete c[i];
	}
	int tonum(char c) {        //문자를 숫자로 변환.
		return tolower(c) - 'a';    //대문자인 경우는 소문자로 변환.
	}

	void insert(const char* words) {
		if (*words == '\0')        //입력받은 words가 '\0'일 경우, 즉 문자열 끝인 경우.
			return;

		int next = tonum(*words);

		if (c[next] == NULL) {
			c[next] = new TN();
		}
		c[next]->insert(words + 1);
	}

	bool find(const char* words) {
		int next = tonum(*words);

		if (*words == '\0')
			return true;

		if (c[next] == NULL)
			return false;

		return c[next]->find(words + 1);
	}
};

int main(void) {

	TN tri;

	tri.insert("like");

	if (tri.find("like"))
		cout << true << endl;
	else
		cout << false << endl;

	if (tri.find("bike"))
		cout << true << endl;
	else
		cout << false << endl;


	return 0;
}

#elif 0
#include "string.h"

#define ll		unsigned long 
#define rint	register int 
#define MTB		50000
struct {
	char s[20];
	int hit;
}w[MTB];

ll hash(char s[]) {
	ll h = 5381;
	while (*s++) 		h = ((h << 5) + h + *s) % MTB;
	return h % MTB;
}

void init() {
	for (rint i = 0; i < MTB; i++)		w[i] = { 0,0 };
}

void inputWord(char mWord[20]) {
	ll h = hash(mWord);
	int cnt = MTB;
	while (w[h].s[0] != 0 && cnt--) {
		if (strcmp(w[h].s, mWord) == 0) {
			w[h].hit++;
			return;
		}
		h = (h + 1) % MTB;
	}
}

int recommend(char mUser[20], char mAnswer[20]) {


}

void banWord(char mWord[20]) {

}
#elif 0
#define rint register int
int m[100000], ln;
void init() {
	ln = 0;
}
void makeWall(int h[5]) {
	for (rint i = 0; i < 5; i++)		m[i + ln] = h[i];
	ln += 5;
}
int matchPiece(int h[5]) {

	for (rint i = ln - 5; i >= 0; i--) {
		if (
			h[4] + m[i + 0] == h[3] + m[i + 1] &&
			h[3] + m[i + 1] == h[2] + m[i + 2] &&
			h[2] + m[i + 2] == h[1] + m[i + 3] &&
			h[1] + m[i + 3] == h[0] + m[i + 4]
			)
		{
			for (rint j = i + 5; j < ln; j++) {
				m[j - 5] = m[j];
			}
			ln -= 5;
			return i + 1;
		}
	}
	return -1;
}
#elif 0
#define MAX_NODE 10000

struct Node {
	int data;
	Node* next;
};

Node node[MAX_NODE];
int nodeCnt = 0;
Node* pH;

Node* getNode(int data) {
	node[nodeCnt].data = data;
	node[nodeCnt].next = nullptr;
	return &node[nodeCnt++];
}

Node* myalloc() {
	return &node[nodeCnt++];
}

void init() {
	pH = myalloc();
	pH->next = nullptr;
}

void addNode2Head(int data) {
	Node* p = myalloc();	p->data = data;
	p->next = pH->next;
	pH->next = p;
}

void addNode2Tail(int data) {
	Node* p = myalloc();	p->data = data;
	Node* cur = pH;
	while (cur->next) cur = cur->next;
	p->next = nullptr;
	cur->next = p;
}

void addNode2Num(int data, int num) {
	Node* p = myalloc();	p->data = data;
	Node* cur = pH;
	while (--num) cur = cur->next;
	p->next = cur->next;
	cur->next = p;
}

void removeNode(int data) {
	Node* cur = pH;
	while (cur->next) {
		if (cur->next->data == data) {
			cur->next = cur->next->next;
			return;
		}
		cur = cur->next;
	}
}

int getList(int output[MAX_NODE]) {
	int ret = 0;
	for (Node* iter = pH->next; iter; iter = iter->next) {
		output[ret++] = iter->data;
	}
	return ret;
}
#elif 0
#define MAX_NODE 10000

struct Node {
	int data;
	Node* prev;
	Node* next;
};

Node node[MAX_NODE];
int nodeCnt = 0;
Node* pH, *pT;

Node* myalloc() {
	return &node[nodeCnt++];
}

Node* getNode(int data) {
	node[nodeCnt].data = data;
	node[nodeCnt].prev = nullptr;
	node[nodeCnt].next = nullptr;
	return &node[nodeCnt++];
}

void init() {
	pH = myalloc();
	pT = myalloc();
	pH->next = pT;
	pH->prev = nullptr;
	pT->next = nullptr;
	pT->prev = pH;
}

void addNode2Here(Node* phere, int data) {
	Node* p = myalloc();
	p->data = data;
	Node* L = phere->prev;
	phere->prev = p;
	p->next = phere;
	L->next = p;
	p->prev = L;
}

void addNode2Head(int data) {
	addNode2Here(pH->next, data);
}

void addNode2Tail(int data) {
	addNode2Here(pT, data);
}

void addNode2Num(int data, int num) {
	Node* iter = pH;
	while (num--) {
		iter = iter->next;
	}
	addNode2Here(iter, data);
}

int findNode(int data) {
	Node* iter = pH;
	int ret = 0;
	while ((iter = iter->next) != pT) {
		ret++;
		if (iter->data == data)
			break;
	}
	return ret;
}

void removeNode(int data) {
	Node* iter = pH;
	while ((iter = iter->next) != pT) {
		if (iter->data == data) {
			Node* R = iter->next;
			Node* L = iter->prev;
			L->next = R;
			R->prev = L;
		}
	}
}

int getList(int output[MAX_NODE]) {
	int ret = 0;
	Node* cur = pH;
	while ((cur = cur->next) != pT) {
		output[ret++] = iter->data;
	}
	return ret;
}

int getReversedList(int output[MAX_NODE]) {
	int ret = 0;
	Node* iter = pT;
	while ((iter = iter->prev) != pH) {
		output[ret++] = iter->data;
	}
	return ret;
}

#elif 0
#include<stdio.h>
#include<string.h>

char s[50000][7];

//struct {
//	char s[7];
//	bool del;
//}a[50000];
int idx = 0;

void init() {
	idx = 0;
}

void addWord(char str[]) {
	strcpy(s[idx++], str);
}

bool mystrcmp(char* s1, char* s2) {
	while (*s2 != NULL) {
		if (*s2 == '*') {
			while (*s1 != *(s2 + 1)) {
				s1++;
			}
			s2++;
			//continue;
		}
		else if (*s1 != *s2) {
			return 0;
		}
		s1++; s2++;
	}
	if (*s1 != NULL)	return 0;
	return 1;
}

int removeWord(char str[]) {
	int ret = 0;
	for (int i = 0; i < idx; i++) {
		if (s[i][0] == NULL) continue;
		if (mystrcmp(s[i], str))
			ret += 1;
		s[i][0] = NULL;
	}
	return ret;
}

int searchWord(char str[]) {
	int ret = 0;
	for (int i = 0; i < idx; i++) {
		if (s[i][0] == NULL) continue;
		ret += mystrcmp(s[i], str);
	}
	return ret;
}


int main() {
	char aa[100] = "slay";
	char bb[100] = "slay*";
	printf("%d \n", mystrcmp(aa, bb));
}
#elif 0

#include<stdio.h>
#include<string.h>

char s[50000][7];

//struct {
//	char s[7];
//	bool del;
//}a[50000];
int idx = 0;

void init() {
	idx = 0;
}

void addWord(char str[]) {
	strcpy(s[idx++], str);
}

int removeWord(char str[]) {
	for (int i = 0; i < idx; i++) {
		if (strcmp(s[i], str))
			s[i][0] = '\0';
	}
}

int searchWord(char str[]) {
	int ret = 0;
	for (int i = 0; i < idx; i++) {
		char* b = s[i];
		if (*b == NULL)	continue;
		for (int j = 0; str[j] != NULL; j++) {
			if (str[j] == *b) {
				if (str[j] == NULL) {
					ret++; break;
				}
				b++;	continue;
			}
			else if (str[j] == '*') {
				continue;
			}
			else {
				break;
			}
		}
	}
	return ret;
}


#elif 0
//간편송금
';'#define MAX_NAME_LEN(4 + 1)

const int MAXTABLE = 27 * 27 * 27 * 27 + 3;
int userTable[MAXTABLE];    // userTable[고객키값] = 고객ID;
int userN;

struct Node {
	int user, group, asset; // user: userN
	Node* prev, *next;
	Node* alloc(int _user, int _group, int _asset, Node* _prev, Node* _next) {
		user = _user, group = _group, asset = _asset;
		prev = _prev, next = _next;
		if (prev) prev->next = this;
		if (next) next->prev = this;
		return this;
	}
	void erase() {
		if (prev) prev->next = this->next;
		if (next) next->prev = this->prev;
	}
	void move(Node* _prev) {    // 노드를 맨 앞으로 옮기기(우선순위 조정에 쓰일 것)
		this->erase();
		prev = _prev;
		next = _prev->next;
		if (prev) prev->next = this;
		if (next) next->prev = this;
	}
}buf[60003], *user[30003], *number[100003]; //user: 고객, number: 계좌 번호
int cbuf;

int hash(const char* s) {
	int ret = 0;
	for (; *s; s++)
		ret = (ret * 27) + *s - 'a' + 1;
	return ret;
}

void init() {
	cbuf = userN = 0;
	for (int i = 0; i < MAXTABLE; i++)
		userTable[i] = 0;
	for (int i = 0; i <= 100000; i++)
		number[i] = 0;
}

void openAccount(int mTime, char mName[MAX_NAME_LEN], char mGroup[MAX_NAME_LEN], int mNumber, int mAsset) {
	int x = hash(mName);
	if (!userTable[x]) {    //새로운 고객
		userTable[x] = ++userN;
		user[userN] = buf[cbuf++].alloc(0, 0, 0, 0, 0); // dummy node
	}

	int userID = userTable[x];
	int groupKey = hash(mGroup);
	number[mNumber] = buf[cbuf++].alloc(userID, groupKey, mAsset, user[userID], user[userID]->next);
}

int closeAccount(int mTime, int mNumber) {
	Node* p = number[mNumber];
	p->erase();
	number[mNumber] = 0;
	return p->asset;
}

int exchange(Node* giver, Node* receiver, int value) {
	if (!giver || !receiver || giver->asset < value)
		return -1;
	giver->asset -= value;
	receiver->asset += value;
	giver->move(user[giver->user]);
	receiver->move(user[receiver->user]);

	return receiver->asset;
}
int sendByNumber(int mTime, int mNumber, int mReceiveNumber, int mValue) {
	return exchange(number[mNumber], number[mReceiveNumber], mValue);
}

int sendByName(int mTime, int mNumber, char mReceiveName[MAX_NAME_LEN], int mValue) {
	int receiverID = userTable[hash(mReceiveName)];
	if (!user[receiverID])
		return -1;
	return exchange(number[mNumber], user[receiverID]->next, mValue);
}

void sendBonus(int mTime, char mGroup[MAX_NAME_LEN], int mValue) {
	int groupKey = hash(mGroup);
	for (int i = 1; i <= userN; i++) {
		Node* p = user[i]->next;
		while (p) {
			if (p->group == groupKey) {
				p->asset += mValue;
				p->move(user[i]);
				break;
			}
			p = p->next;
		}
	}
}


#elif 0

struct { int section;    int heat;    int erased; int mark; }news[50001];
int news_idx; int SEC[100001]; int top[21];
void init() {
	for (int i = 1; i <= 50000; i++)
		news[i] = { -1, 0, 0, 0 };
	for (int i = 0; i < 100000; i++)
		SEC[i] = -1;
}
void addNews(int mSection, int mNewsId) {
	news_idx = mNewsId;
	news[news_idx] = { mSection, 1, 0, 0 };
}
void eraseNews(int mNewsId) {
	news[mNewsId].erased = 1;
}
void readNews(int mUserId, int mNewsId) {
	SEC[mUserId] = news[mNewsId].section;
	++news[mNewsId].heat;
}
void changeSection(int mNewsId, int mSection) {
	news[mNewsId].section = mSection;
}
int getList(int mUserId, int mList[]) {
	int section = SEC[mUserId];

	for (int j = news_idx; j > 0; j--)
		news[j].mark = 0;

	int temp[50005];
	for (int j = news_idx; j > 0; j--) {
		temp[j] = news[j].heat;
	}

	int cnt = 0;
	for (int i = 0; i < 10; i++) {
		int MaxV = 0; int MaxK = 0;
		for (int j = news_idx; j > 0; j--) {
			break;
			if (news[j].erased == 1 || news[j].mark == 1)   continue;
			int k = news[j].heat;
			if (section == news[j].section)  k += 10;
			if (MaxV < k) {
				MaxV = k;
				MaxK = j;
			}
		}
		if (MaxK == 0) break;
		mList[i] = MaxK;
		news[MaxK].mark = 1;
		cnt++;
	}

	return cnt;
}

#elif 0
/*
30°³ Å×½ºÆ®ÄÉÀÌ½º¸¦ ÇÕÃÄ¼­ C++ ÀÇ °æ¿ì 4ÃÊ
Èü, Á¤Àû ¸Þ¸ð¸® ÇÕÃÄ¼­ 256MB ÀÌ³», ½ºÅÃ ¸Þ¸ð¸® 1MB ÀÌ³»

¿©·¯ ÀÎÅÍ³Ý ´º½º ±â»ç¸¦ ÇÑµ¥ ¸ð¾Æ º¸¿©ÁÖ´Â ´º½º ÇÇµå ¼­ºñ½º¸¦ Á¦ÀÛÇÏ·Á ÇÑ´Ù.
±â»ç´Â 0 ~ 9 ÀÇ Á¤¼ö·Î Ç¥ÇöµÇ´Â 10 °³ ºÐ¾ß Áß ÇÏ³ª·Î ºÐ·ùµÈ´Ù.
±â»çÀÇ ºÐ¾ß´Â »ý¼ºµÉ ¶§ Á¤ÇØÁö¸ç ³ªÁß¿¡ º¯°æµÉ ¼öµµ ÀÖ´Ù.
Æ¯Á¤ »ç¿ëÀÚ¿¡°Ô Á¡¼ö°¡ ³ôÀº »óÀ§ 10°³ÀÇ ±â»ç¸¦ ÃßÃµ ±â»ç·Î º¸¿©ÁØ´Ù.
±â»çÀÇ Á¡¼ö´Â ¸ðµç »ç¿ëÀÚ°¡ ÇØ´ç ±â»ç¸¦ ÀÐÀº ÃÑ È½¼öÀÌ´Ù.
À§ Á¡¼ö¿¡ ÇØ´ç »ç¿ëÀÚÀÇ °ü½É ºÐ¾ß¿Í ÀÏÄ¡ÇÏ´Â °æ¿ì 10Á¡ÀÇ °¡»êÁ¡À» ´õÇÑ´Ù.
Á¡¼ö°¡ °°Àº °æ¿ì¿¡´Â ÃÖ±Ù¿¡ µî·ÏµÈ ±â»ç°¡ ´õ ³ôÀº ¼øÀ§¸¦ °®°Ô µÈ´Ù.
»ç¿ëÀÚÀÇ °ü½É ºÐ¾ß´Â ÇØ´ç »ç¿ëÀÚ°¡ °¡Àå ÃÖ±Ù¿¡ ÀÐÀº ±â»çÀÇ ºÐ¾ßÀÌ´Ù.
ÀÌÈÄ ÇØ´ç ±â»çÀÇ ºÐ¾ß°¡ º¯°æµÇ¾îµµ »ç¿ëÀÚÀÇ °ü½ÉºÐ¾ß´Â º¯°æµÇÁö ¾Ê´Â´Ù.
ÇØ´ç »ç¿ëÀÚ°¡ ±â»ç¸¦ ÇÑ ¹øµµ ÀÐÁö ¾Ê¾Ò´Ù¸é °ü½É ºÐ¾ß´Â ¾ø´Ù.
A
void init()
°¢ Å×½ºÆ® ÄÉÀÌ½ºÀÇ Ã³À½¿¡ È£ÃâµÈ´Ù.

void addNews(int mSection, int mNewsId)
mSection ºÐ¾ßÀÌ°í °íÀ¯¹øÈ£°¡ mNewsId ÀÎ ±â»ç°¡ Ãß°¡µÈ´Ù.
°¢ Å×½ºÆ® ÄÉÀÌ½º¿¡¼­ mNewsId °ªÀº 1ºÎÅÍ ½ÃÀÛÇÏ¿© addNews ÇÔ¼ö°¡ ºÒ¸± ¶§¸¶´Ù 1¾¿ Áõ°¡ÇÑ´Ù.
Parameters
   mSection : ±â»çÀÇ ºÐ¾ß ¹øÈ£ ( 0 ¡Â mSection ¡Â 9 )
   mNewsId : ±â»çÀÇ °íÀ¯¹øÈ£ ( 1 ¡Â mNewsId ¡Â 50,000 )

void eraseNews(int mNewsId)
±â»çÀÇ °íÀ¯¹øÈ£°¡ mNewsId ÀÎ ±â»ç°¡ »èÁ¦µÈ´Ù.
Parameters
   mNewsId : ±â»çÀÇ °íÀ¯¹øÈ£ ( 1 ¡Â mNewsId ¡Â 50,000 )

void readNews(int mUserId, int mNewsId)
¾ÆÀÌµð mUserId ÀÎ »ç¿ëÀÚ°¡ mNewsId ÀÎ ±â»ç¸¦ ÀÐ´Â´Ù.
¾ÆÀÌµð mUserId ÀÎ »ç¿ëÀÚÀÇ °ü½É ºÐ¾ß°¡ mNewsId ±â»çÀÇ ºÐ¾ß·Î º¯°æµÈ´Ù.
Parameters
   mUserId : ±â»ç¸¦ ÀÐÀº »ç¿ëÀÚÀÇ ¾ÆÀÌµð ( 1 ¡Â mUserId ¡Â 100,000 )
   mNewsId : ±â»çÀÇ °íÀ¯¹øÈ£ ( 1 ¡Â mNewsId ¡Â 50,000 )

void changeSection(int mNewsId, int mSection)
±â»çÀÇ °íÀ¯¹øÈ£°¡ mNewsId ÀÎ ±â»ç°¡ mSection ºÐ¾ß·Î º¯°æµÈ´Ù.
mSectionÀº ±âÁ¸ ±â»çÀÇ ºÐ¾ß¿Í ´Ù¸¥ °ÍÀÌ º¸ÀåµÈ´Ù.
Parameters
   mNewsId : ±â»çÀÇ °íÀ¯¹øÈ£ ( 1 ¡Â mNewsId ¡Â 50,000 )
   mSection : ±â»çÀÇ ºÐ¾ß ¹øÈ£ ( 0 ¡Â mSection ¡Â 9 )

int getList(int mUserId, int mList[])
¾ÆÀÌµð mUserId ÀÇ »ç¿ëÀÚ¿¡°Ô º¸ÀÌ´Â ÃßÃµ ±â»çÀÇ ¹øÈ£¸¦ ¼øÀ§´ë·Î mList ¿¡ ÀúÀåÇÑ´Ù.
ÃÖ´ë 10°³ÀÇ ±â»ç¸¦ ÀúÀåÇÏ¸ç, ±â»çÀÇ ÃÑ °³¼ö°¡ 10°³ ¹Ì¸¸ÀÌ¶ó¸é ±â»çÀÇ °³¼ö¸¸Å­ ÀúÀåÇÑ´Ù.
mList¿¡ ´ã±ä ±â»çÀÇ °³¼ö¸¦ ¹ÝÈ¯ÇÑ´Ù.
Parameters
   mUserId : »ç¿ëÀÚÀÇ ¾ÆÀÌµð ( 1 ¡Â mUserId ¡Â 100,000 )
   mList : ÃßÃµ ±â»ç ¸®½ºÆ®¸¦ ÀúÀåÇÒ ¹è¿­
Returns
   mList¿¡ ´ã±ä ±â»çÀÇ °³¼ö


 [Á¦¾à»çÇ×]
1. °¢ Å×½ºÆ® ÄÉÀÌ½º ½ÃÀÛ ½Ã init() ÇÔ¼ö°¡ È£ÃâµÈ´Ù.
2. eraseNews(), readNews(), changeSection() ÇÔ¼öÀÇ mNewsId´Â Çö ½ÃÁ¡¿¡ Á¸ÀçÇÏ´Â ±â»çÀÇ ¹øÈ£ÀÓÀÌ º¸ÀåµÈ´Ù.
3. °¢ Å×½ºÆ® ÄÉÀÌ½º¿¡¼­ addNews() ÇÔ¼öÀÇ È£Ãâ È½¼ö´Â ÃÖ´ë 50,000 ¹øÀÌ´Ù.
4. °¢ Å×½ºÆ® ÄÉÀÌ½º¿¡¼­ eraseNews() ÇÔ¼öÀÇ È£Ãâ È½¼ö´Â ÃÖ´ë 5,000 ¹øÀÌ´Ù
5. °¢ Å×½ºÆ® ÄÉÀÌ½º¿¡¼­ readNews() ÇÔ¼öÀÇ È£Ãâ È½¼ö´Â ÃÖ´ë 50,000 ¹øÀÌ´Ù.
6. °¢ Å×½ºÆ® ÄÉÀÌ½º¿¡¼­ changeSection() ÇÔ¼öÀÇ È£Ãâ È½¼ö´Â ÃÖ´ë 5,000 ¹øÀÌ´Ù.
7. °¢ Å×½ºÆ® ÄÉÀÌ½º¿¡¼­ getList() ÇÔ¼öÀÇ È£Ãâ È½¼ö´Â ÃÖ´ë 50,000 ¹øÀÌ´Ù.

*/
void init()
{

}
void addNews(int mSection, int mNewsId)
{

}
void eraseNews(int mNewsId)
{

}
void readNews(int mUserId, int mNewsId)
{

}
void changeSection(int mNewsId, int mSection)
{

}
int getList(int mUserId, int mList[])
{
	return 0;
}
#elif 0
#include<time.h>
#include<stdio.h> 
#include<stdlib.h> 
#define MAX_N			5
#define MAX_NAME_LEN	7
#define MAX_TAG_LEN		4

//void mstrcpy(char dst[], const char src[]) {
//	int c = 0;
//	while ((dst[c] = src[c]) != '\0') ++c;
//}

//int mstrcmp(const char str1[], const char str2[]) {
//	int c = 0;
//	while (str1[c] != '\0' && str1[c] == str2[c]) ++c;
//	return str1[c] - str2[c];
//}

void init(int M)
{

}

int section[100];   // type ÀúÀå
int section_cnt;

int types[500];
int types_cnt;

void add(char mName[MAX_NAME_LEN])
{
	int c; unsigned long long hash = 0;
	while (c = *mName++) hash = (hash << 8) + c;


}

int moveType(char mType[MAX_TAG_LEN], int mFrom, int mTo)
{
	return 0;
}

void moveName(char mName[MAX_NAME_LEN], int mSection)
{

}

void deleteName(char mName[MAX_NAME_LEN])
{

}

int countBook(int mTypeNum, char mTypes[MAX_N][MAX_TAG_LEN], int mSection)
{
	return 0;
}

int main() {
	char mName[7];
	srand((unsigned int)time(NULL));
	for (int i = 0; i < 5; i++) {
		mName[i] = rand() % 52 + 'a';
		add(mName);
	}


}




#elif 0
int n[11][50001];
int section[50001];
int ncnt;
int delcnt;
int del[50001];

struct USER
{
	int userid;
	int prefer;
}u[100001];

void init()
{
	ncnt = 0;
	delcnt = 0;
	for (int i = 0; i <= 100000; i++)
	{
		u[i].prefer = 10;
	}
	for (int i = 0; i <= 10; i++)
	{
		n[i][0] = 0;
	}
}

void addNews(int mSection, int mNewsId)
{
	ncnt++;
	for (int i = 0; i <= 10; i++)
	{
		n[i][mNewsId] = 0;
	}
	n[mSection][mNewsId] = 10;
	section[mNewsId] = mSection;
	del[mNewsId] = 0;
}

void eraseNews(int mNewsId)
{
	delcnt++;
	del[mNewsId] = 1;
}

void readNews(int mUserId, int mNewsId)
{
	u[mUserId].prefer = section[mNewsId];
	for (int i = 0; i <= 10; i++)
	{
		n[i][mNewsId]++;
	}
}

void changeSection(int mNewsId, int mSection)
{
	n[section[mNewsId]][mNewsId] -= 10;
	section[mNewsId] = mSection;
	n[mSection][mNewsId] += 10;
}

int getList(int mUserId, int mList[])
{
	int* my_n;
	for (int i = 0; i < 10; i++)
		mList[i] = 0;
	my_n = n[u[mUserId].prefer];

	int end = 10;

	if (end > ncnt - delcnt)
		end = ncnt - delcnt;

	for (int i = 1; i <= ncnt; i++)
	{
		if (del[i])continue;
		if (my_n[mList[9]] > my_n[i])continue;

		int j;
		for (j = 9; j > 0; j--)
		{
			if (my_n[mList[j - 1]] > my_n[i])break;
			mList[j] = mList[j - 1];
		}
		mList[j] = i;
	}
	return end;
}

#elif 0
#define MAX_N 2005
#define rint register int

struct _cell {
	int r1, c1, r2, c2;
	int size, del;
}CELL[MAX_N], backup[MAX_N];

int cellCnt;
int backupCnt;
int check;
int row, col;
int visit[MAX_N];
void init(int R, int C)
{
	row = R, col = C;
	cellCnt = 0;
	backupCnt = -1;
	check = 0;
	for (rint i = 0; i < MAX_N; i++) {
		visit[i] = 0;
	}
}

void copy(int r1, int c1, int r2, int c2, int rect[]) {
	rect[0] = r1, rect[1] = c1, rect[2] = r2, rect[3] = c2;
}
void copy(int i, int rect[]) {
	rect[0] = CELL[i].r1, rect[1] = CELL[i].c1, rect[2] = CELL[i].r2, rect[3] = CELL[i].c2;
}

void getRect(int r, int c, int rect[])
{
	for (rint i = 0; i < cellCnt; i++) {
		if (CELL[i].del) continue;
		int r1 = CELL[i].r1;
		int c1 = CELL[i].c1;
		int r2 = CELL[i].r2;
		int c2 = CELL[i].c2;

		if (r >= r1 && r <= r2 && c >= c1 && c <= c2) {
			copy(i, rect);
			return;
		}
	}
	copy(r, c, r, c, rect);
}
void backUp() {
	backupCnt = 0;
	for (rint i = 0; i < cellCnt; i++) {
		if (CELL[i].del) continue;
		backup[backupCnt++] = CELL[i];
	}
}

int mergeCells(int cnt, int rs[], int cs[], int rect[])
{
	for (rint k = 0; k < cnt; k++) {
		int r = rs[k], c = cs[k];
		for (rint i = k + 1; i < cnt; i++) {
			if (r == rs[i] && c == cs[i])
				return 0;
		}
	}

	check++;
	int select[15];
	int selectCnt = 0;
	int startR = row + 1, startC = col + 1, endR = 0, endC = 0;
	int sumSize = 0;
	for (rint k = 0; k < cnt; k++) {
		int r = rs[k], c = cs[k];
		int found = 0;
		for (rint i = 0; i < cellCnt; i++) {
			if (CELL[i].del) continue;
			int r1 = CELL[i].r1;
			int c1 = CELL[i].c1;
			int r2 = CELL[i].r2;
			int c2 = CELL[i].c2;

			if (r >= r1 && r <= r2 && c >= c1 && c <= c2) {
				if (visit[i] == check) {
					return 0;
				}
				else {
					visit[i] = check;
				}
				select[selectCnt++] = i;
				sumSize += CELL[i].size;
				if (startR > r1) startR = r1;
				if (endR < r2) endR = r2;
				if (startC > c1) startC = c1;
				if (endC < c2) endC = c2;
				found = 1;
				break;
			}
		}
		if (found == 1) continue;
		sumSize++;
		if (startR > r) startR = r;
		if (endR < r) endR = r;
		if (startC > c) startC = c;
		if (endC < c) endC = c;
	}
	int s = (endR - startR + 1) * (endC - startC + 1);
	if (s != sumSize) return 0;
	backUp();
	for (rint i = 0; i < selectCnt; i++) {
		CELL[select[i]].del = 1;
	}
	CELL[cellCnt++] = { startR, startC, endR, endC, sumSize, 0 };
	copy(startR, startC, endR, endC, rect);
	return 1;
}

int splitCell(int r, int c, int rect[])
{
	for (rint i = 0; i < cellCnt; i++) {
		if (CELL[i].del) continue;
		int r1 = CELL[i].r1;
		int c1 = CELL[i].c1;
		int r2 = CELL[i].r2;
		int c2 = CELL[i].c2;

		if (r >= r1 && r <= r2 && c >= c1 && c <= c2) {
			backUp();
			CELL[i].del = 1;
			copy(i, rect);
			return 1;
		}
	}
	return 0;
}

void undo()
{
	if (backupCnt == -1) return;
	_cell tmp[MAX_N];
	int tmpCnt = 0;
	for (rint i = 0; i < cellCnt; i++) {
		if (CELL[i].del) continue;
		tmp[tmpCnt++] = CELL[i];
	}
	for (rint i = 0; i < backupCnt; i++) {
		CELL[i] = backup[i];
	}
	cellCnt = backupCnt;
	for (rint i = 0; i < tmpCnt; i++) {
		backup[i] = tmp[i];
	}
	backupCnt = tmpCnt;
}

int checkRectangle(int r1, int c1, int r2, int c2)
{
	for (rint i = 0; i < cellCnt; i++) {
		if (CELL[i].del) continue;
		int sr = CELL[i].r1;
		int sc = CELL[i].c1;
		int er = CELL[i].r2;
		int ec = CELL[i].c2;

		if (er < r1 || sr > r2 || ec < c1 || sc > c2) {
			continue;
		}
		if (er <= r2 && sr >= r1 && ec <= c2 && sc >= c1) {
			continue;
		}
		return 1;
	}
	return 0;
}


#elif 0




#define MAX_N           5
#define MAX_NAME_LEN    7
#define MAX_TAG_LEN     4

#define MAX_TYPE 504
#define MAX_TYPE_TABLE 2011

#define MAX_BOOK 50000
#define MAX_BOOK_TABLE 100019
#define NULL 0

struct Book {
	long long value;
	Book* next;
	int section;
	int count_book_call;
};

int count_book_call = 0;
int book_heap_size = 0;
Book books[MAX_BOOK];
Book* book_table[MAX_BOOK_TABLE];

struct Type {
	int value;
	Type* next;
	int id;
};

int type_heap_size = 1;
Type types[MAX_TYPE];
Type* type_table[MAX_TYPE_TABLE];
int book_counter[MAX_TYPE];

Book* type_books[MAX_TYPE][MAX_BOOK];

__inline long long str2ll(char* s) {
	long long value = 0;
	while (*s) value = (value << 8) + *s++;
	return value;
}

__inline int get_type_id(int value) {
	int hash = value % MAX_TYPE_TABLE;
	register Type* type = type_table[hash];

	while (type && type->value != value) type = type->next;

	if (!type) {
		type = &types[type_heap_size++];
		*type = { value, type_table[hash], type_heap_size - 1 };
		type_table[hash] = type;
	}

	return type->id;
}

__inline int find_type_id(long long value) {
	register Type* type = type_table[value % MAX_TYPE_TABLE];
	while (type && type->value != value) type = type->next;
	if (type) return type->id;
	return 0;
}

__inline Book* find_book(long long value) {
	register Book* book = book_table[value % MAX_BOOK_TABLE];
	while (book && book->value != value) book = book->next;
	return book;
}

void init(int M) {
	count_book_call = 0;
	book_heap_size = 0;
	type_heap_size = 1;

	register int index = MAX_BOOK_TABLE;
	while (--index != -1) book_table[index] = NULL;

	index = MAX_TYPE_TABLE;
	while (--index != -1) type_table[index] = NULL;

	index = MAX_TYPE;
	while (--index != -1) book_counter[index] = 0;
}

void add(char mName[MAX_NAME_LEN], register int mTypeNum, char mTypes[MAX_N][MAX_TAG_LEN], int mSection) {
	long long value = str2ll(mName);
	int hash = value % MAX_BOOK_TABLE;

	Book* book = &books[book_heap_size++];

	*book = { value, book_table[hash], mSection, 0 };
	book_table[hash] = book;

	register int type_id;
	while (--mTypeNum != -1) {
		type_id = get_type_id(str2ll(mTypes[mTypeNum]));
		type_books[type_id][book_counter[type_id]++] = book;
	}
}

int moveType(char mType[MAX_TAG_LEN], register int mFrom, register int mTo) {
	int type_id = find_type_id(str2ll(mType));

	register int counter = book_counter[type_id];
	register Book** book = &type_books[type_id][0];
	register int count = 0;
	while (--counter != -1) {
		if ((*book)->section == mFrom) {
			(*book)->section = mTo;
			count++;
		}
		book++;
	}

	return count;
}

void moveName(char mName[MAX_NAME_LEN], int mSection) {
	find_book(str2ll(mName))->section = mSection;
}

void deleteName(char mName[MAX_NAME_LEN]) {
	find_book(str2ll(mName))->section = 0;
}

int countBook(register int mTypeNum, char mTypes[MAX_N][MAX_TAG_LEN], int mSection) {
	count_book_call++;

	int type_id;

	register Book** book;

	register int count = 0;
	register int counter;
	while (--mTypeNum != -1) {
		type_id = find_type_id(str2ll(mTypes[mTypeNum]));
		book = &type_books[type_id][0];
		counter = book_counter[type_id];
		while (--counter != -1) {
			if ((*book)->section == mSection && (*book)->count_book_call != count_book_call) {
				(*book)->count_book_call = count_book_call;
				count++;
			}
			book++;
		}
	}
	return count;
}
#elif 0
#define MAX_N			5
#define MAX_NAME_LEN	7
#define MAX_TAG_LEN		4

struct BOOK {
	//int id;
	char name[10];
	//int nkey;
	int type[6];
	int tcnt;
	BOOK* next;
};

BOOK bk[50000]; int bid;

BOOK* SectionType[100][500]; int SectionTypecnt[100];
BOOK* SectionBook[100]; int SectionBook



cnt[100];

int stcnt;
int sn[100][50000]; int sncnt;

BOOK* myalloc() {
	return &bk[bid++];
}
void mstrcpy(char dst[], const char src[]) {
	int c = 0;
	while ((dst[c] = src[c]) != '\0') ++c;
}

int mstrcmp(const char str1[], const char str2[]) { int c = 0;	while (str1[c] != '\0' && str1[c] == str2[c]) ++c;	return str1[c] - str2[c]; }
void init(int M) {
	bid = stcnt = sncnt = 0;
}
void add(char mName[MAX_NAME_LEN], int mTypeNum, char mTypes[MAX_N][MAX_TAG_LEN], int mSection) {
	BOOK* pNew = myalloc();
	mstrcpy(pNew[bid].name, mName);

	for (int i = 0; i < mTypeNum; i++) {
		int ht = 0, char* str = mTypes[i];
		while (*str++) {
			ht = (ht << 8) + *str - 'a';
		}
		pNew[bid].type[i] = ht;
		//st[mSection][stcnt++] = ht;
		SectionType[mSection]
	}
	bk[bcnt].tcnt = mTypeNum;
	sn[mSection][bcnt] = bcnt;
}
int moveType(char mType[MAX_TAG_LEN], int mFrom, int mTo) {

	return 0;
}

void moveName(char mName[MAX_NAME_LEN], int mSection)
{

}

void deleteName(char mName[MAX_NAME_LEN])
{

}

int countBook(int mTypeNum, char mTypes[MAX_N][MAX_TAG_LEN], int mSection)
{
	return 0;
}
#elif 0
#include<stdio.h>
#define NULL 0


struct Trie {
	bool terminal;
	Trie* children[26];

	Trie() : terminal(false) {
		for (int i = 0; i < 26; i++) children[i] = 0;
	}
	void insert(const char* key) {
		if (*key == '\0') {
			terminal = 1; return;
		}
		int nextkey = *key - 'a';
		if (!children[nextkey])
			children[nextkey] = malloc(Trie);
	}
};


#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <algorithm>

using namespace std;

class Node
{
public:
	Node* childs[26] = { NULL };
	bool isWord = false;

	Node() {}
};

class Trie
{
public:
	Node* root;

	Trie() { this->root = new Node(); }

	void insert(const char* str)
	{
		Node* cur = this->root;

		for (int i = 0; i < strlen(str); i++)
		{

			if (cur->childs[str[i] - 'a'] != NULL) cur = cur->childs[str[i] - 'a'];
			else
			{
				cur->childs[str[i] - 'a'] = new Node();
				cur = cur->childs[str[i] - 'a'];
			}
		}

		cur->isWord = true;
	}

	bool find(const char* str)
	{
		Node* cur = this->root;

		for (int i = 0; i < strlen(str); i++)
		{
			if (cur->childs[str[i] - 'a'] == NULL) return 0;
			else cur = cur->childs[str[i] - 'a'];
		}

		if (cur->isWord) return 1;
		else return 0;
	}
};

int main(void)
{
	Trie trie;

	trie.insert("tree");

	cout << trie.find("tree") << '\n';
	cout << trie.find("tre") << '\n';

	return 0;
}





Trie* malloc()

//ÃâÃ³: https://www.crocus.co.kr/1053 [Crocus]
#elif 0
char PL[10000][1000];

struct Node {
	int rn;
	Node* next;
} NODE[10000];

int PLn, LLn;

void insert(int l, char a[]) {
	strcpy(PL[PLn++], a);

	for (int i = l; i < LLn; i++) {
		LL[i].rn++;
		LL[i].pl = PLn;
	}

	LL[l].pl = PLn;
}
void delete(int l, char a[]) {
	for (int i = l; i < LLn; i++) LL[i].rn++;
	LL[l].pl = PLn;
}


#elif 0
struct PT {
	int t, l, uid;
} P[100005];
int FL[1005][1005], pcnt, tc;
bool operator < (PT a, PT b) {
	return a.l == b.l ? a.t < b.t : a.l < b.l;
}
void init(int N) {
	++tc; pcnt = 0;
	for (int i = 1; i <= N; i++)    FL[i][i] = tc;
}
void follow(int u1, int u2, int ts) {
	FL[u1][u2] = tc;
}
void makePost(int uID, int pID, int ts) {
	P[++pcnt] = { ts, 0, uID };
}
void like(int pID, int ts) {
	P[pID].l++;
}
void getFeed(int uID, int ts, int pL[]) {
	int a[11], k = 0;
	for (int i = pcnt; i; i--) {
		if (FL[uID][P[i].uid] != tc)
			continue;
		if (P[i].t + 1000 >= ts) {
			int j;
			for (j = k; j && P[a[j - 1]] < P[i]; j--)
				a[j] = a[j - 1];
			a[j] = i; k += k < 10;
		}
		else if (k >= 10) goto exit;
		else a[k++] = i;
	}
exit:
	for (int ii = 0; ii < 10; ii++)    pL[ii] = a[ii];
}


#elif 0
#define MAXUSER 1001
#define MAX 100005

struct Post { int uid, like, time; } p[MAX];
int pLen, i, tc = 0, fol[MAXUSER][MAXUSER] = { 0 };

void init(int N)
{
	tc++;
	pLen = 0;
	for (i = 0; i < MAXUSER; i++) fol[i][i] = tc;
}

void follow(int uID1, int uID2, int timestamp)
{
	fol[uID1][uID2] = tc;
}

void makePost(int uID, int pID, int timestamp)
{
	p[++pLen] = { uID, 0, timestamp };
}

void like(int pID, int timestamp)
{
	p[pID].like++;
}

bool operator < (Post a, Post b) {
	return  a.like == b.like ? a.time > b.time : a.like > b.like;
}

int* res; int resLen;
void insertSort(int j) {
	for (i = resLen; i && p[j] < p[res[i - 1]]; i--) res[i] = res[i - 1];
	res[i] = j;
	resLen += resLen < 10;
}

void getFeed(int uID, int timestamp, int pIDList[])
{
	res = pIDList;
	resLen = 0;
	for (int j = pLen; j; j--) {
		if (fol[uID][p[j].uid] != tc) continue;
		if (p[j].time + 1000 >= timestamp) insertSort(j);
		else if (resLen >= 10) return;
		else res[resLen++] = j;
	}
}

#elif 0
char F[1010][1010];

struct str {
	int uID, ts, like;
} B[100010];
int L;

void init(int N) {
	for (int i = 1; i <= N; i++) for (int j = 1; j <= N; j++) F[i][j] = (i == j);
	L = 0;
}

void follow(int uID1, int uID2, int timestamp) {
	F[uID1][uID2] = 1;
}

void makePost(int uID, int pID, int timestamp) {
	++L;
	B[L].uID = uID;
	B[L].ts = timestamp;
	B[L].like = 0;
}

void like(int pID, int timestamp) {
	B[pID].like += 1;
}

void getFeed(int uID, int timestamp, int pIDList[]) {
	int tp = 0, res[11];
	for (int i = 0; i < 11; i++) res[i] = -1;
	for (int i = L; i; i--) {
		if (!F[uID][B[i].uID]) continue;
		int dt = timestamp - B[i].ts;
		if (dt > 1000) {
			if (tp == 10) break;
			res[tp++] = i;
		}
		else {
			res[tp] = i;
			for (int a = tp; a; a--) {
				if (B[res[a]].like > B[res[a - 1]].like) {
					int t = res[a]; res[a] = res[a - 1]; res[a - 1] = t;
				}
				else break;
			}
			if (tp < 10) ++tp;
		}
	}
	for (int i = 0; i < tp; i++) pIDList[i] = res[i];
}



#elif 0
#define MSG_N_MAX 	1000005
#define HSIZE 	    0x7A7B 
#define UMAX 	    1005

unsigned long v;
int FL[UMAX][UMAX], fcnt[UMAX];

struct MSG {
	int uID;    int time;
	bool operator < (MSG m) { return time < m.time ? 1 : 0; }
} Msg[MSG_N_MAX];
struct M {
	char key[10];
	int mcnt;
	int m[6];
	void add(int mID) {
		int i;
		for (i = mcnt; i && Msg[m[i - 1]] < Msg[mID]; i--)   m[i] = m[i - 1];
		m[i] = mID;
		mcnt += mcnt < 5;
	}
} Hm[HSIZE], Um[UMAX], ret;

int mstrcmp(const char* a, const char* b) {
	int i;
	for (i = 0; a[i] != '\0'; ++i) if (a[i] != b[i]) return a[i] - b[i];
	return a[i] - b[i];
}
void mstrcpy(char* dest, const char* src) {
	int i = 0;
	while (src[i] != '\0') { dest[i] = src[i]; i++; }
	dest[i] = src[i];
}

void init() {
	for (int i = 0; i < HSIZE; i++)     Hm[i].mcnt = 0;
	for (int i = 0; i < UMAX; i++)      Um[i].mcnt = 0, FL[i][0] = i, fcnt[i] = 1;
}

char a[10];
void createMessage(int mID, int uID, char s[]) {
	int time = ((s[0] - '0') * 100000 + (s[1] - '0') * 10000 + (s[3] - '0') * 1000 + (s[4] - '0') * 100 + (s[6] - '0') * 10 + (s[7] - '0')) * 1000 + 1000 - uID;
	Msg[mID] = { uID, time };
	Um[uID].add(mID);

	s += 8;
	while (*s++)
	{
		int i;
		for (i = 0, v = 0; (a[i] = *++s) > ' '; i++, v = (((v << 8)) + *s) % HSIZE);
		a[i] = '\0';
		int sk = 1;
		while (Hm[v].key[0] != 0 && (sk = (mstrcmp(Hm[v].key, a) != 0)))
			v = (v + 1) % HSIZE;

		if (sk == 1) mstrcpy(Hm[v].key, a);
		Hm[v].add(mID);
	}
}
void followUser(int userID1, int userID2) {
	FL[userID1][fcnt[userID1]++] = userID2;
}
int searchByHashtag(char s[], int retIDs[]) {
	int i;
	for (i = 0, v = 0; (a[i] = *++s) > ' '; i++, v = (((v << 8)) + *s) % HSIZE);
	a[i] = '\0';
	while (Hm[v].key[0] != 0 && mstrcmp(Hm[v].key, a) != 0)
		v = (v + 1) % HSIZE;

	for (int i = 0; i < Hm[v].mcnt; i++)	retIDs[i] = Hm[v].m[i];
	return Hm[v].mcnt;
}
int getMessages(int userID, int retIDs[]) {
	ret.mcnt = 0;
	for (int i = 0; i < fcnt[userID]; i++)
		for (int uID = FL[userID][i], j = 0; j < Um[uID].mcnt; j++)
			ret.add(Um[uID].m[j]);
	for (int i = 0; i < ret.mcnt; i++)	retIDs[i] = ret.m[i];
	return ret.mcnt;
}


#elif 0
#include<stdio.h>
#define MAX_USER      1000
#define MAX_TAG       5000

// uID´ç follow uID
int F[MAX_USER][MAX_USER];
int fcnt[MAX_USER];

// uID´ç °¡Áø mID
//int USER_MSG[MAX_USER][50000];
int USER_MSG[MAX_USER][6];
int mcnt[100];
// ht´ç °¡Áø mID
//int HT_MSG[MAX_TAG][50000];
int HT_MSG[MAX_TAG][6];
int hmcnt[MAX_TAG];

// mID´ç Tm
int MSG_TM[50000];
#define M (1024*1024)


void init() {
	pintf("%d \n", sizeof(F) / M);
	pintf("%d \n", sizeof(fcnt) / M);
	pintf("%d \n", sizeof(USER_MSG) / M);
	pintf("%d \n", sizeof(mcnt) / M);
	pintf("%d \n", sizeof(HT_MSG) / M);
	pintf("%d \n", sizeof(hmcnt) / M);
	pintf("%d \n", sizeof(MSG_TM) / M);


	for (int i = 0; i < 1000; i++) fcnt[i] = 0, mcnt[i] = 0;
	for (int i = 0; i < 5000; i++) hmcnt[i] = 0;
}

void add(int mID)
{




	if (MSG_TM[retIDs[4]] > MSG_TM[mID]) return;
	else MSG_TM[retIDs[4]] = mID;

	int temp = MSG_TM[mID];

	int j = 3;
	while (temp > MSG_TM[retIDs[j]] && j >= 0) {
		retIDs[j + 1] = retIDs[j];
		j = j - 1;
	}
	MSG_TM[j + 1] = mID;


	HT_MSG[ht][hmcnt[ht]++] = msgID;
	MSG_TM[ht] = time;



}


void createMessage(int msgID, int userID, char msgData[])
{
	USER_MSG[userID][mcnt[userID]++] = msgID;

	int time = ((msgData[0] - '0') * 100000 +
		(msgData[1] - '0') * 10000 +
		(msgData[3] - '0') * 1000 +
		(msgData[4] - '0') * 100 +
		(msgData[6] - '0') * 10 +
		msgData[7] - '0') * 1000 + 1000 - userID;

	char* str = &msgData[9];

	unsigned long long key = 0, ht = 0;
	while (*str != '\0') {
		if (*str == '#') {
			key = 0, ht = 0;
			str++;
			//continue;
		}
		else if (*str == ' ' || *str == '\0') {
			ht = key % 5000;
			while (hmcnt[ht] != 0) {
				ht = (ht + 1) % 5000;
			}


			HT_MSG[ht][hmcnt[ht]++] = msgID;
			MSG_TM[ht] = time;



		}
		key = (key << 8) + *str;
		str++;
	}
	while (0);
}
void followUser(int userID1, int userID2) {
	F[userID1][fcnt[userID1]++] = userID2;
}
void Top5(int mID, int retIDs[])
{
	if (MSG_TM[retIDs[4]] > MSG_TM[mID]) return;
	else MSG_TM[retIDs[4]] = mID;

	int temp = MSG_TM[mID];

	int j = 3;
	while (temp > MSG_TM[retIDs[j]] && j >= 0) {
		retIDs[j + 1] = retIDs[j];
		j = j - 1;
	}
	MSG_TM[j + 1] = mID;
}

int searchByHashtag(char tagName[], int retIDs[])
{
	for (int i = 0; i < 5; i++) retIDs[i] = 0;

	unsigned long long key = 0, ht = 0;
	tagName++;
	while (*tagName != '\0') {
		key = (key << 8) + *tagName;
		tagName++;
	}
	ht = key % 5000;
	while (hmcnt[ht] != 0) {
		ht = (ht + 1) % 5000;
	}

	for (int i = 0; i < hmcnt[ht]; i++) {
		int mID = HT_MSG[ht][i];
		for (int j = 0; j < hmcnt[ht]; j++) {
			int mID = HT_MSG[ht][j];
			Top5(mID, retIDs);
		}
	}
	for (int i = 0; i < 5; i++)
		if (retIDs[i] == 0) return i;
	return 0;
}

int getMessages(int userID, int retIDs[])
{
	for (int i = 0; i < 5; i++) retIDs[i] = 0;

	for (int i = 0; i < fcnt[userID]; i++) {
		int uID = F[userID][i];
		for (int j = 0; j < mcnt[userID]; j++) {
			int mID = USER_MSG[uID][j];
			Top5(mID, retIDs);
		}
	}

	for (int i = 0; i < 5; i++)
		if (retIDs[i] == 0) return i;

	return 0;
}
#elif 0







#define int register int
int FL[1000][1000];
int Fcnt[1000];
int USER_MSG[1000][50000];
int Mcnt[1000];
int MSG_TM[1000000];

void init()
{
}

void createMessage(int msgID, int userID, register char msgData[])
{
	FL[userID][Fcnt[userID]++] = userID;
	USER_MSG[userID][Mcnt[userID]++] = msgID;

	unsigned long long time = ((msgData[0] - '0') * 1e5 + (msgData[1] - '0') * 10000 + (msgData[3] - '0') * 1000 + (msgData[4] - '0') * 100 + (msgData[5] - '0') * 10 + (msgData[6] - '0') * 1) * 1000 - userID;
	MSG_TM[msgID] = time;

}

void followUser(int userID1, int userID2)
{
	FL[userID1][Fcnt[userID1]++] = userID2;
}

int searchByHashtag(char tagName[], int retIDs[])
{
}

int getMessages(int userID, int retIDs[])
{
	for (int i = 0; Fcnt[userID]; i++) {
		int uID = FL[userID][i];
		for (int j = 0; Mcnt[uID]; j++) {
			int mID = USER_MSG[i][j];
			SS(mID);
		}
	}

}




struct User {


};


#elif 0

#include <iostream>
using namespace std;

struct NODE {
	int v;
	NODE* next;
}a[2000];
int arr_idx = 0;

NODE* malloc() { return &a[arr_idx++]; }
NODE* pHead;

void pint_node() {
	for (NODE* iter = pHead->next; iter != NULL; iter = iter->next) {
		cout << iter->v << " ";
	}
	cout << endl;
}
void main() {
	arr_idx = 0;
	pHead = malloc();
	pHead->next = NULL;
	NODE* p;
	for (int i = 0; i < 10; i++) {
		p = malloc();
		p->v = i;
		p->next = pHead->next;
		pHead->next = p;
	}
	pint_node();

	NODE* prev = pHead;
	for (NODE* iter = pHead->next; iter != NULL; iter = iter->next) {
		if (iter->v == 5) {
			prev->next = iter->next;
		}
		prev = iter;
	}
	pint_node();


}


#elif 0
#include<iostream>
using namespace std;

int arr_idx = 0;

struct NODE {
	int v;
	NODE* prev;
	NODE* next;
}a[20000];


NODE* malloc(void) {
	return &a[arr_idx++];
}


NODE* pTail;
NODE* pHead;

void pint_node(void);

void main() {
	arr_idx = 0;;

	pHead = malloc();
	pTail = malloc();

	pTail->prev = pHead;
	pHead->next = pTail;

	NODE* p;

	for (int i = 0; i < 10; i++) {
		p = malloc();
		p->v = i;

		p->prev = pTail->prev;
		pTail->prev = p;

		p->next = p->prev->next;
		p->prev->next = p;
	}
	pint_node();


	// Remove form head
	for (NODE* iter = pHead->next; iter != pTail; iter = iter->next) {
		if (iter->v == 5) {
			iter->prev->next = iter->next;
			iter->next->prev = iter->prev;
			cout << "Delete node 5 ..." << iter->v << endl << endl;
			break;
		}
	}
	pint_node();


	for (NODE* iter = pTail->prev; iter != pHead; iter = iter->prev) {
		if (iter->v == 3) {
			iter->prev->next = iter->next;
			iter->next->prev = iter->prev;
			cout << "Delete node 3 ..." << iter->v << endl << endl;
			break;
		}
	}
	pint_node();

	for (NODE* iter = pTail->prev; iter != pHead; iter = iter->prev) {
		iter->prev->next = iter->next;
		iter->next->prev = iter->prev;
		cout << "Delete all node ..." << iter->v << endl;
	}
	cout << endl;

	for (int i = 0; i < 10; i++) {
		p = malloc();
		p->v = i;

		p->prev = pTail->prev;
		pTail->prev = p;

		p->next = p->prev->next;
		p->prev->next = p;
	}
	pint_node();
}


void pint_node(void) {
	cout << "from head ~~" << endl;
	for (NODE* iter = pHead->next; iter != pTail; iter = iter->next) {
		cout << iter->v << " ";
	}
	cout << endl;

	cout << "from tail ~~" << endl;
	for (NODE* iter = pTail->prev; iter != pHead; iter = iter->prev) {
		cout << iter->v << " ";
	}
	cout << endl;
}















#elif 0



#include<iostream>
using namespace std;

int arr_idx = 0;

struct NODE {
	int v;
	NODE* prev;
	NODE* next;
}a[20000];


NODE* malloc(void) {
	return &a[arr_idx++];
}


NODE* pTail;
NODE* pHead;

void pint_node(void);
void init() {}
void createMessage(int msgID, int userID, char msgData[]) {}
void followUser(int userID1, int userID2) {}
int searchByHashtag(char tagName[], int retIDs[]) { return 0; }
int getMessages(int userID, int retIDs[]) { return 0; }


void doublelinked() {
	arr_idx = 0;;

	pHead = malloc();
	pTail = malloc();

	pTail->prev = pHead;
	pHead->next = pTail;

	NODE* p;

	for (int i = 0; i < 10; i++) {
		p = malloc();
		p->v = i;

		p->prev = pTail->prev;
		pTail->prev = p;

		p->next = p->prev->next;
		p->prev->next = p;
	}
	pint_node();


	// Remove form head
	for (NODE* iter = pHead->next; iter != pTail; iter = iter->next) {
		if (iter->v = 5) {
			iter->prev->next = iter->next;
			iter->next->prev = iter->prev;
		}
		cout << "Delete node 5 ..." << iter->v << endl << endl;
	}
	pint_node();

	for (NODE* iter = pTail->prev; iter != pHead; iter = iter->prev) {
		if (iter->v = 3) {
			iter->prev->next = iter->next;
			iter->next->prev = iter->prev;
		}
		cout << "Delete node 3 ..." << iter->v << endl << endl;
	}
	pint_node();

	for (NODE* iter = pTail->prev; iter != pHead; iter = iter->prev) {
		iter->prev->next = iter->next;
		iter->next->prev = iter->prev;

		cout << "Delete all node ..." << iter->v << endl << endl;
	}

	for (int i = 0; i < 10; i++) {
		p = malloc();
		p->v = i;

		p->prev = pTail->prev;
		pTail->prev = p;

		p->next = p->prev->next;
		p->prev->next = p;
	}
	pint_node();
}


void pint_node(void) {
	cout << "from head ~~" << endl;
	for (NODE* iter = pHead->next; iter != pTail; iter->next) {
		cout << iter->v << " ";
	}
	cout << endl;

	cout << "from tail ~~" << endl;
	for (NODE* iter = pTail->prev; iter != pHead; iter->prev) {
		cout << iter->v << " ";
	}
	cout << endl;
}


#elif 0
#include <stdio.h>

struct Msg {
	int mTM;
};

struct User {
	Msg* msg;
} USER[1000];


typedef struct node {
	int data;
	struct node* next;
} Node;

typedef struct list {
	Node* head;
	Node* tail;
	int size;
} List;

void createlist(List* list) {

	list->head = (Node*)malloc(sizeof(Node));
	list->tail = (Node*)malloc(sizeof(Node));
	list->head->next = list->tail;
	list->tail->next = list->tail;
	list->size = 0;
}
void addFirst(List* list, int data) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = list->head->next;
	list->head->next = newNode;
	list->size++;
}
void addLast(List* list, int data) {
	Node* last = list->head;

	while (last->next != list->tail)
		last = last->next;

	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = list->tail;
	last->next = newNode;
	list->size++;
}

Node* searchNode(List* list, int data) {
	Node* node = list->head->next;
	while (node != list->tail) {
		if (node->data == data)
			return node;
		node = node->next;
	}
	pintf("µ¥ÀÌÅÍ¸¦ Ã£Áö ¸øÇß½À´Ï´Ù.\n");

	return NULL;
}

void removeNode(List* list, int data) {
	Node* node = list->head;
	while (node->next != list->tail) {
		if (node->next->data == data) {
			Node* delNode = node->next;
			node->next = delNode->next;
			free(delNode);
			list->size--;
			return;
		}
		node = node->next;
	}
	pintf("µ¥ÀÌÅÍ¸¦ Ã£Áö ¸øÇß½À´Ï´Ù.\n");
}



void pintList(List* list) {
	Node* node = list->head->next;
	int i = 1;
	while (node != list->tail) {
		pintf("%d ¹øÂ° ³ëµå µ¥ÀÌÅÍ :%d\n", i++, node->data);
		node = node->next;
	}
}
void distroyList(List* list) {
	Node* node = list->head;
	while (node != list->tail) {
		Node* delNode = node;
		node = delNode->next;
		free(delNode);
	}
	free(list->head);
	free(list->tail);
}

int main() {

	int i;
	List list;
	createlist(&list);

	for (i = 1; i <= 5; i++)
		addLast(&list, i);
	for (i = 11; i <= 15; i++)
		addFirst(&list, i);
	removeNode(&list, 11);
	removeNode(&list, 15);
	removeNode(&list, 5);
	removeNode(&list, 4);
	removeNode(&list, 50);

	Node* node = searchNode(&list, 14);
	pintf("search :%d\n", node->data);

	node = searchNode(&list, 12);
	pintf("search :%d\n", node->data);

	node = searchNode(&list, 3);
	pintf("search :%d\n", node->data);

	pintList(&list);
	return 0;
}


#elif 0
#define SHRT_MAX      32767
#define USHRT_MAX     0xffff
#define INT_MIN     (-2147483647 - 1)
#define INT_MAX       2147483647
#define UINT_MAX      0xffffffff
#define LONG_MIN    (-2147483647L - 1)
#define LONG_MAX      2147483647L
#define ULONG_MAX     0xffffffffUL
#define LLONG_MAX     9223372036854775807i64
#define LLONG_MIN   (-9223372036854775807i64 - 1)
#define ULLONG_MAX    0xffffffffffffffffui64
#define MAX_USER	1000
#define MAX_MSGS	50000
#define MAX_MID		1000000
#define MAX_TAG     5000
#define MAX_P_ID	10
#define MAX_KEY		10 
#define MAX_DATA	128
#define MAX_TABLE	5000

/*
struct _Msg {
	int uID;
	int mTM;
}MSG[MAX_MID];
*/
int MSG_Srtd[MAX_MID];


struct mmm {
	int aa;
	struct mmm* bbb;
}mmm[1000000];

//int MAP[1000][1000000];

int F[MAX_USER][MAX_USER];  // F[ÀÚ±âÀÚ½Å][FollowÇÏ´Â uID]
int FC[MAX_USER];   //FC[±× ÀÚ½Å] FollowÇÏ´Â uID°¹¼ö
int F_Srtd[MAX_USER][6];

/*
struct {
	int fcnt;
	int fID[MAX_USER];
}F[MAX_USER];
*/
struct {
	int key;
	int mcnt;
	int mID[6];
}HT[MAX_TABLE];

int mstrcmp(const char* a, const char* b);
void mstrcpy(char* dest, const char* src);
unsigned long hash(const char* str);

void init() {
	for (int i = 0; i < MAX_USER; i++) {
		FC[i] = 0;
		for (int j = 0; j < MAX_USER; j++)
			F[i][j] = (i == j);
	}
}

void add_msg(int h, int mID, int userID) {


}


void createMessage(int mID, int userID, char msgData[]) {
	int k = 0; char a[20];
	int time = (msgData[0] - '0') * 1e5 + (msgData[1] - '0') * 1e4 + (msgData[3] - '0') * 1e3 + (msgData[4] - '0') * 1e2 + (msgData[6] - '0') * 10 + (msgData[7] - '0') * 1;
	time = time * 1e3 + userID;

	MSG_Srtd[mID] = time;
	char* str = msgData + 8;

	unsigned long long h, key;
	while (*str++) {
		if (*str == '#')
		{
			h = 0, key = 0;
			while (*str++ != ' ')
				key = (key << 8) + *str;

			h = key % MAX_TABLE;

			while (HT[h].key != key)
				h = (h + 1) % MAX_TABLE;
			HT[h].key == key;
			HT[h].mcnt++;

			{
				int j = HT[h].mcnt - 1;
				while ((time > MSG_Srtd[HT[h].mID[j]]) && (j >= 0))
				{
					HT[h].mID[j + 1] = HT[h].mID[j];
					j = j - 1;
				}
				HT[h].mID[j + 1] = time;
			}
		}
	}
}

void followUser(int userID1, int userID2) {
	F[userID1][FC[userID1]++] = userID2;
}

/*userID À¯Àú°¡ µî·ÏÇÑ ¸Þ½ÃÁö¿Í ÆÈ·Î¿ìÇÏ°í ÀÖ´Â À¯ÀúµéÀÇ ¸Þ½ÃÁöµéÀ» Ã£¾Æ ÇØ´ç ¸Þ½ÃÁöÀÇ id ¸¦ retIDs[] ¿¡ ÀúÀåÇÏ°í, ÀúÀåÇÑ ¸Þ½ÃÁöÀÇ id °³¼ö¸¦ ¸®ÅÏÇÑ´Ù.*/
int getMessages(int userID, int retIDs[]) {
	for (int i = 0; i < FC[userID]; i++) {
		//F[userID][i]
		F_Srtd[MAX_USER][6];


	}
	return 0;
}


int searchByHashtag(char tagName[], int retIDs[]) {

#if 0
	unsigned long h = hash(key); int cnt = MAX_TABLE;

	while (HT[h].key[0] != 0 && cnt--) {
		if (mstrcmp(HT[h].key, key) == 0) {


			for (int i = 0; i < HT[h].mcnt; i++) {
				list5up(HT[h].mID[i], retIDs);
			}






			return 1;
		}
		h = (h + 1) % MAX_TABLE;
	}


#endif
	return 0;
}




int mstrcmp(const char* a, const char* b) {
	int i;
	for (i = 0; a[i] != '\0'; ++i)
		if (a[i] != b[i])
			return a[i] - b[i];
	return a[i] - b[i];
}

void mstrcpy(char* dest, const char* src) {
	int i = 0;
	while (src[i] != '\0') {
		dest[i] = src[i]; i++;
	}
	dest[i] = src[i];
}

unsigned long hash(const char* str) {
	unsigned long hash = 5381;	int c;
	while (c = *str++) {
		hash = (((hash << 5) + hash) + c) % MAX_TABLE;
	}
	return hash % MAX_TABLE;
}


/*
int find(const char* key, char* data)
{
	unsigned long h = hash(key);
	int cnt = MAX_TABLE;

	while (HT[h].key[0] != 0 && cnt--)
	{
		if (strcmp(HT[h].key, key) == 0)
		{
			strcpy(data, HT[h].data);
			return 1;
		}
		h = (h + 1) % MAX_TABLE;
	}
	return 0;
}


int add(const char* key, char* data)
{
	unsigned long h = hash(key);

	while (HT[h].key[0] != 0)
	{
		if (strcmp(HT[h].key, key) == 0)
		{
			return 0;
		}

		h = (h + 1) % MAX_TABLE;
	}
	strcpy(HT[h].key, key);
	strcpy(HT[h].data, data);
	return 1;
}
*/



#elif 0
#include<iostream>
#define NMAX	1000
using namespace std;
int map[1000][1000];
int cnt;

void init() {
	cnt = 0;
	for (int i = 0; i < NMAX; i++)
		for (int j = 0; j < NMAX; j++)
			map[i][j] = 0;
}

void fill(int x1, int y1, int x2, int y2)
{
	for (int i = x1; i <= x2; i++)
	{
		for (int j = y1; j <= y2; j++)
		{

			if (map[i][j] == -1)	continue;

			if (map[i][j] < 3)
			{
				map[i][j]++;
				if (map[i][j] == 3)
				{
					cnt++;
					map[i][j] = -1;
				}
			}
		}
	}
}

int main() {
	init();
	int testcast, n, x1, y1, x2, y2;
	cin >> testcast;
	for (int tc = 1; tc <= testcast; tc++)
	{
		cin >> n;
		for (int j = 0; j < n; j++)
		{
			cin >> x1 >> y1 >> x2 >> y2;
			fill(x1, y1, x2, y2);
		}
		cout << "#" << tc << " " << cnt << endl;
	}

}

#elif 0
#include<iostream>
using namespace std;

int main() {
	int tc;
	cin >> tc;
	for (int i = 1; i <= tc; i++)
	{
		int a, b;
		cin >> a >> b;
		cout << "#" << i << " " << (a + b) % 24 << endl;
	}
	return 1;
}
#elif  0
#include<iostream>
using namespace std;

#define ull	unsigned long long 

ull hash(char* s)
{
	ull h = 0;
	char c;
	while (c = *s++)
	{
		h = (h << 8) + c;
	}
	return h;
}

ull hashtoidx(char* s)
{
	ull h = hash(s);
	char c;
	while (c = *s++)
	{
		h = (h << 8) + c;
	}
	return h;
}



int main() {
	cout << sizeof(unsigned long long int) << endl;
	cout << hex << ULLONG_MAX << endl;
}

#elif 0
#include <iostream>
#include <string>
#include <vector>
#include <limits.h>
using namespace std;

#define MAX_HT	200000
struct {
	char key[21];
	int cnt;
}ht[MAX_HT];

int hash(char* s)
{
	int h = 5381, c;
	while (c = *s++)
	{
		h = (((h << 5) + h) + c) % MAX_HT;
	}
	return h;
}
void addmem(char s[])
{
	int h = ::hash(s);
	while (ht[h].key[0])
	{
		if (strcmp(ht[h].key, s) == 0)	break;
		else h = (h + 1) % MAX_HT;
	}
	strcpy(ht[h].key, s);
	ht[h].cnt++;
}

void delmem(char s[])
{
	int h = ::hash(s);
	while (ht[h].key[0])
	{
		if (strcmp(ht[h].key, s) == 0)	break;
		else h = (h + 1) % MAX_HT;
	}
	//strcpy(ht[h].key, s);
	ht[h].cnt--;
}

int getcnt(char* s)
{
	int h = ::hash(s);
	while (ht[h].key[0])
	{
		if (strcmp(ht[h].key, s) == 0)	break;
		else h = (h + 1) % MAX_HT;
	}
	return ht[h].cnt;
}

//string solution(vector<string> participant, vector<string> completion) {

void main() {

	vector<string> participant{ "Evan", "Jung", "Choi" };
	vector<string> completion{ "Evan", "Jung" };

	for (int i = 0; i < MAX_HT; i++)
	{
		ht[i].cnt = 0;
	}


	string ans = "";
	char str[1000];


	for (auto& name : participant) {
		strcpy(str, name.c_str());
		addmem(str);
	}

	for (auto& name : completion) {
		strcpy(str, name.c_str());
		delmem(str);
	}

	//for (auto& rest : ht) {
	//	//cout << rest.first << endl;
	//	return rest.first;
	//}

	for (int i = 0; i < MAX_HT; i++)
	{
		if (ht[i].cnt > 0)
		{

			string aaa(ht[i].key);
			//return  aaa; 
			/*cout << INT_MAX << endl;
			cout << UINT_MAX << endl;
			cout << LONG_MAX << endl;
			cout << ULONG_MAX << endl*/;
			printf("%0x \n", INT_MAX);
			printf("%0x \n", UINT_MAX);
			printf("%0x \n", LONG_MAX);
			printf("%0x \n", ULONG_MAX);
		}
	}
	//return ans;
}

//int main() {
//
//	vector<string> v;
//
//	v.push_back("Evan");
//	v.push_back("Jung");
//	v.push_back("Choi");
//
//	char s[100];
//	for (auto& name : v)
//	{
//		strcpy(s, name.c_str());
//		cout << s << endl;
//	}
//
//
//	return 0;
//}
#elif 0
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

unordered_map<string, int> ht;
//
//int main() {
//
//	ht["Evan"] = 1;
//	ht["Jung"] = 1;
//	ht["Choi"] = 1;
//
//	ht.erase("Evan");
//	ht.erase("__Choi"); 
//
//	for (auto& rest : ht)
//		cout << rest.first;
//	return 0;
//}

string solution(vector<string> participant, vector<string> completion) {
	for (auto& name : participant) {
		ht[name] ++;
	}

	for (auto& name : completion) {
		ht.erase(name);
	}

	for (auto& rest : ht) {
		//cout << rest.first << endl;
		return rest.first;
	}
}

#elif 0
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

string solution(vector<string> participant, vector<string> completion) {
	//string answer = "";
	unordered_map<string, int> participants;
	for (const string& name : participant)
	{
		participants[name]++;
	}

	for (const string& name : completion)
	{
		participants[name]--;
	}

	for (auto pair : participants)
	{
		if (pair.second > 0)
			return pair.first;
	}

	//return answer;
}



#elif 0


#include<unordered_map>
#include<iostream>
using namespace std;

unordered_map<int, int>	ht;

int main() {

	ht[1231234] = 100;
	ht[555231234] = 200;
	ht[9555231234] = 400;

	cout << ht[1231234] << endl;
	auto it = ht.find(1231234);
	cout << ht.find(1231234)->first << " " << ht.find(1231234)->second << endl;


	return 0;
}
#elif 0
#include<iostream>
#include<assert.h>
using namespace std;
#define MAX_HT	100000
#define rint register int 

struct {
	int key, idx;
}ht[MAX_HT];

void addId(int key, int id)
{
	int h = key % MAX_HT;
	while (ht[h].key)
	{
		if (ht[h].key == key)	break;
		else h = (h + 1) % MAX_HT;
	}
	ht[h] = { key, id };
}

int getId(int key)
{
	int h = key % MAX_HT;
	while (ht[h].key)
	{
		if (ht[h].key == key)	return ht[h].idx;
		else h = (h + 1) % MAX_HT;
	}
	assert(0);
	return false;
}
void init() {
	for (rint i = 0; i < MAX_HT; i++)	ht[i].key = 0;
}
int main() {


	init();
	addId(100040000, 1);
	addId(200020000, 2);
	addId(300006000, 3);
	addId(400000200, 4);
	addId(500000010, 5);

	cout << getId(300006000) << endl;
	cout << getId(400000200) << endl;
	cout << getId(500000010) << endl;

	for (rint i = 0; i < MAX_HT; i++)
	{
		if (ht[i].key) cout << ht[i].key << " " << ht[i].idx << " " << i << endl;
	}

	return 0;
}
#elif 0
#include<stdio.h>
#include<assert.h>
#include<iostream>
using namespace std;
#define MAX_HT	100000
#define rint register int

struct {
	int key;
	int id;
}ht[MAX_HT];


void addId(int key, int id)
{
	int h = key % MAX_HT;
	while (ht[h].key)
	{
		if (ht[h].key == key)	break;
		else h = (h + 1) % MAX_HT;
	}
	ht[h] = { key, id };
}

int getId(int key)
{
	int h = key % MAX_HT;
	while (ht[h].key)
	{
		if (ht[h].key == key)	return ht[h].id;
		h = (h + 1) % MAX_HT;
	}
	assert(0);
	return false;
}

void init() {
	for (rint i = 0; i < MAX_HT; i++)
	{
		ht[i].key = 0;
	}
}

int main()
{
	init();
	addId(100000000, 1);
	addId(200000000, 2);
	addId(300000000, 3);
	addId(400000000, 4);
	addId(500000000, 5);

	cout << getId(400000000) << endl;
	cout << getId(500000000) << endl;
	return 0;
}
#elif 0
#include<iostream>
#define rint register int

struct Node {
	int id, Osize, Vsize;
	Node* prev, *next;
};


#define MAX_HT	10001*2

struct {
	int key;
}ht[MAX_HT];

int hash(int key) {
	int h = 5381;
	h = key % MAX_HT;
	return h;
}

//#define hash(key)	(key%MAX_HT)

int getId(int key) {
	int h = key % MAX_HT;
	int cnt = MAX_HT;
	while (ht[h].key != 0 && cnt--)
	{
		if (ht[h].key == key)
		{
			return h;
		}
		h = (h + 1) % MAX_HT;
	}
	printf("E2 \n");
	return -1;
}

int addId(int key)
{
	int h = key % MAX_HT;
	while (ht[h].key != 0)
	{
		if (ht[h].key == key)
		{
			printf("E1 \n");
			return 0;
		}
		h = (h + 1) % MAX_HT;
	}
	ht[h].key = key;
	return h;
}

void init()
{
	for (rint i = 0; i < MAX_HT; i++)
		ht[i].key = 0;
}

int cmdAdd(int newID, int pID, int fileSize)
{
	int idx = add(newID);

}

int cmdMove(int tID, int pID)

int cmdInfect(int tID)

int cmdRecover(int tID)

int cmdRemove(int tID)


#elif 0

#include<stdio.h>

template <typename T>
void Swap(T& a, T& b)
{
	T t;
	t = a; a = b; b = t;
}

//struct tag_st {
//	int i; double d;
//};

template<typename T>
void Print(T& a, T& b)
{
	printf("Print %d, %d \n", (T)a, (T)b);

}


int main() {

	int a = 3, b = 4;
	double c = 1.2, d = 3.4;
	char e = 'e', f = 'f';
	//tag_st g = { 1, 2.3 }, h = { 4,5.6 };
	Print(a, b);
	Swap(a, b);
	Print(a, b);

	Print(c, d);
	Swap(c, d);
	Print(c, d);

	Print(e, f);
	Swap(e, f);
	Print(e, f);
	//Swap(g, h);

	return 0;
}


#elif 0
#include<iostream>
#include<unordered_map>
using namespace std;

unordered_map<string, int> htab;


int main() {


	htab["Evan"] = 1;
	htab["Jung"] = 1000;
	htab["Choi"] = 3000;

	for (auto& elem : htab)
		cout << elem.first << elem.second << endl;

	//cout << htab.find("Evan")
	return 0;
}

#elif 0
#define _STL_
#include<stdio.h>

#ifdef _STL_
#include<stdlib.h> 
#else
int atoi(char* s)
{
	char c;
	int ret = 0;
	while ((c = *s++) != NULL)
	{
		if (c < '0' || c > '9')	return ret;
		ret = ret * 10 + (c - '0');
	}
	return ret;
}
#endif

int main()
{
	char a[100];
	scanf("%s ", a);

	printf("%d \n", atoi(a));

	return 0;
}

#elif 0
#include<iostream>
using namespace std;
int main() {
	for (int i = -10; i < 10; i++) {
		for (int j = -10; j < 10; j++) {
			int ret = (i + j) > 5 ? 5 : (i + j) < 1 ? 1 : (i + j);
			cout << i << "+" << j << "=" << ret << endl;
		}
	}
	return 1;
}

#elif 0
#include<list>
//#include<algorithm>
using namespace std;

list<int> li[6][6];

struct Mercenary {
	int team;
	list<int>::iterator iter;
}mer[100003];

void init() {
	for (int i = 1; i <= 5; i++)
		for (int j = 1; j <= 5; j++)
			li[i][j].clear();
}

void hire(int id, int team, int score) {
	mer[id] = { team, li[team][score].insert(li[team][score].end(), id) };
}

void fire(int id) {
	*mer[id].iter = 0;
}

void updateSoldier(int id, int score) {
	*mer[id].iter = 0;
	hire(id, mer[id].team, score);
}

void updateTeam(int team, int newScore) {
	if (newScore < 0)
		for (int i = 2; i <= 5; i++) {
			int score = max(1, i + newScore);
			li[team][score].splice(li[team][score].end(), li[team][i]);
		}
	else if (newScore > 0)
		for (int i = 4; i >= 1; i--) {
			int score = min(5, i + newScore);
			li[team][score].splice(li[team][score].end(), li[team][i]);
		}
}

int bestSoldier(int team) {
	int ret = 0;
	for (int i = 5; i >= 1; i--) {
		if (li[team][i].empty()) continue;
		for (auto x : li[team][i]) ret = max(x, ret);
		if (ret) break;
	}
	return ret;
}

#elif 0
#define rint register int

//ID_node ids[100000];

struct ID {
	int team, score, fired;
}ids[100000];


struct T_node {

	int mcnt;
};
T_node teams[5];

void updateSoldier(int mID, int mScore) {
	id[mID].score = mScore;
}

void updatTeam(int mTeam, int mChangeScore) {
	T_node team = teams[mTeam];

	for (rint i = 0; i < team.mcnt; i++) {
		team.id[i].score

	}
}

#elif 0
#include<iostream>
#include<list>
using namespace std;
#define MAX_NODE 10000

list<int> l;

void init() {

}

void addNode2Head(int data) {
	l.push_front(data);
}

void addNode2Tail(int data) {
	l.push_back(data);
}

void addNode2Num(int data, int num) {
	list<int>::iterator iter = l.begin();
	for (int i = 0; i < num; i++)		iter++;
	l.insert(iter, data);
}

void removeNode(int data) {
	list<int>::iterator it = find(l.begin(), l.end(), data);
	if (it == l.end())	return;
	else l.erase(it);
}

int getList(int output[MAX_NODE]) {
	int ret = 0;
	for (int i : l) {
		ret++;
		cout << i << endl;
	}
	return ret;
}

#elif 0
#include<iostream>
#include<list>

using namespace std;

int main()
{
	list<int> int_list;

	int_list.push_back(1);
	int_list.push_back(2);
	int_list.push_back(3);
	int_list.push_front(4);
	int_list.push_front(5);

	if (int_list.empty())
		cout << "Is empty" << endl;
	else
		cout << "This size : " << int_list.size() << endl;

	for (int i : int_list)
	{
		cout << i << endl; // ���
	}

	list<int>::iterator iter = int_list.begin();
	iter++;
	cout << "2��°" << endl;
	int_list.erase(iter);

	iter = int_list.begin();
	iter++;
	cout << "2��°�� 9����" << endl;
	int_list.insert(iter, 9);

	for (int i : int_list)
	{
		cout << i << endl; //���
	}

	cout << "ù�� ° �� : " << int_list.front() << endl;

	cout << "��� �����" << endl;
	int_list.clear();

	if (int_list.empty())
		cout << "Is empty" << endl;
	else
		cout << "This size : " << int_list.size() << endl;

	return 0;
}
#elif 0

#include<stdio.h>
#include<string.h>

char s[50000][7];

//struct {
//	char s[7];
//	bool del;
//}a[50000];
int idx = 0;

void init() {
	idx = 0;
}

void addWord(char str[]) {
	strcpy(s[idx++], str);
}


bool mystrcmp(char* s1, char* s2) {
	while (*s2 != NULL) {
		if (*s2 == '*') {
			while (*s1 != *(s2 + 1)) {
				s1++;
			}
			s2++;
			//continue;
		}
		else if (*s1 != *s2) {
			return 0;
		}
		s1++; s2++;
	}
	if (*s1 != NULL)	return 0;
	return 1;
}

int removeWord(char str[]) {
	int ret = 0;
	for (int i = 0; i < idx; i++) {
		if (mystrcmp(s[i], str))
			ret += 1;
	}
	return ret;
}

int searchWord(char str[]) {
	int ret = 0;
	for (int i = 0; i < idx; i++) {
		ret += mystrcmp(s[i], str);
	}
	return ret;
}

#elif 0
#include<stdlib.h>
#include<stdio.h>

struct {
	int sT, eT;
}CiD[10000];

struct {

}RiD[10000];


int myatoi(char* s) {
	int ret = 0; char c;
	while ((c = *s++) != NULL) {
		if (c > '9' || c < '0')	return ret;
		ret = ret * 10 + (c - '0');
	}
	return ret;
}
int main() {
	char a[30] = "2019:12:31-23:13:27";
	printf("%d \n", atoi(a));
	printf("%d \n", atoi(&a[5]));
	printf("%d \n", atoi(&a[8]));

	int date, time;
	date = atoi(a) * 10000 + atoi(&a[5]) * 100 + atoi(&a[8]);
	time = atoi(&a[11]) * 10000 + atoi(&a[14]) * 100 + atoi(&a[17]);
	printf("%d \n", date);
	printf("%d \n", time);
	date = myatoi(a) * 10000 + myatoi(&a[5]) * 100 + myatoi(&a[8]);
	time = myatoi(&a[11]) * 10000 + myatoi(&a[14]) * 100 + myatoi(&a[17]);
	printf("%d \n", date);
	printf("%d \n", time);
	return 1;
}



#elif 0


#include <cstdio>
#include <list>
#include <iterator>
#include <string>
#include <iostream>
using namespace std;
void printlist(list<char> l) {
	for (list<char>::iterator iter = l.begin(); iter != l.end(); iter++) {
		cout << *iter;
	}
	cout << endl;
}
int main() {
	string str, input;
	list<char> l;
	int N, i;
	char buf;

	cin >> str;
	scanf("%d%c", &N, &buf);
	for (i = 0; i < str.length(); i++) {
		l.push_back(str[i]);
	}

	list<char>::iterator iter = l.end();

	for (i = 0; i < N; i++) {
		getline(cin, input);
		if (input[0] == 'L') {
			if (iter != l.begin()) iter--;
		}
		else if (input[0] == 'D') {
			if (iter != l.end()) iter++;
		}
		else if (input[0] == 'B') {
			if (iter != l.begin()) iter = l.erase(--iter);
		}
		else {
			l.insert(iter, input[2]);
		}
	}

	printlist(l);
	return 0;
}
#elif 0
#include<stdio.h>
char a[600000 + 5];
struct NODE {
	char data;
	NODE* prev, *next;
};
NODE node[600000 + 5];
NODE* pHead, *pTail;
int idx = 0;
NODE* myalloc() {
	return &node[idx++];
}
void addnode(NODE* t, char c) {
	NODE* p = myalloc();
	p->data = c;
	NODE* L = t->prev;
	t->prev = p;
	p->next = t;
	L->next = p;
	p->prev = L;
}
NODE* delnode(NODE* t) {
	if (t == pTail || t == pHead)	return pTail;
	NODE* L = t->prev;
	NODE* R = t->next;
	L->next = R;
	R->prev = L;
	return R;
}
void printlist() {
	for (NODE* iter = pHead->next; iter != pTail; iter = iter->next) {
		printf("%c", iter->data);
	}
	printf("\n");
}
int main() {
	idx = 0;
	pHead = myalloc();
	pTail = myalloc();
	pHead->next = pTail;
	pHead->prev = NULL;
	pTail->next = NULL;
	pTail->prev = pHead;
	NODE* cur = pTail;

	scanf("%s ", a);
	for (int i = 0; a[i] != NULL; i++) {
		addnode(cur, a[i]);
	}

	int N;
	scanf("%d ", &N);
	for (int i = 0; i < N; i++) {
		char cmd, ch;
		scanf("%c ", &cmd);
		switch (cmd) {
		case 'L':
			if (cur->prev != pHead) cur = cur->prev;
			break;
		case 'D':
			if (cur != pTail) cur = cur->next;
			break;
		case 'B':
			if (cur->prev != pHead) 	delnode(cur->prev);
			break;
		case 'P':
			scanf("%c ", &ch);
			addnode(cur, ch);
			break;
		default:
			break;
		}
	}
	printlist();
	return 0;
}



#elif 0

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <cstdio>
#include <list>
#include <iterator>
#include <string>
#include <iostream>
using namespace std;

#define MAX_STR 600
#define MAX_CMD 500000

struct NODE {
	char data;
	NODE* prev, *next;
};
NODE b[600000 + 5];
int idx = 0;
char a[600000];


NODE* myalloc() {
	return &b[idx++];
}

NODE* pTail = myalloc();
NODE* pHead = myalloc();


void genstr(char* a) {
	srand((unsigned int)time(NULL));
	for (int i = 0; i < MAX_STR; i++) {
		a[i] = rand() % 26 + 'a';
	}
}
void gencmd(char* cmd, char* ch) {
	switch (rand() % 4) {
	case 0:
		*cmd = 'L';
		break;
	case 1:
		*cmd = 'D';
		break;
	case 2:
		*cmd = 'B';
		break;
	case 3:
		*cmd = 'P';
		*ch = rand() % 26 + 'a';
		break;
	}
}

void addnode(NODE* t, char c) {
	NODE* p = myalloc();
	p->data = c;
	NODE* L = t->prev;
	t->prev = p;
	p->next = t;
	L->next = p;
	p->prev = L;
}

NODE* delnode(NODE* t) {
	if (t == pHead || t == pTail)	return NULL;
	NODE* L = t->prev;
	NODE* R = t->next;
	L->next = R;
	R->prev = L;
	return R;
}

void printlist() {
	for (NODE* iter = pHead->next; iter != NULL; iter = iter->next) {
		printf("%c", iter->data);
	}
	printf("\n");
}


NODE* cur;
void myinit() {
	pHead->next = pTail;
	pHead->prev = NULL;
	pTail->next = NULL;
	pTail->prev = pHead;
	cur = pTail;
	for (int i = 0; a[i] != NULL; i++) {
		addnode(pTail, a[i]);
	}
}
void mymain(char cmd, char ch) {

	switch (cmd) {
	case 'L':
		if (cur->prev != pHead) cur = cur->prev;
		break;
	case 'D':
		if (cur != pTail) cur = cur->next;
		break;
	case 'B':
		if (cur->prev != pHead) cur = delnode(cur->prev);
		break;
	case 'P':
		addnode(cur, ch);
		break;
	}
}

int main() {
	//genstr(a);
	scanf("%s ", a);
	myinit();
	//printlist();
	char cmd, ch = 0;

	int M;
	scanf("%d ", &M);
	for (int i = 0; i < M; i++) {
		//gencmd(&cmd, &ch);
		scanf("%c ", &cmd);
		if (cmd == 'P')	scanf("%c ", &ch);
		mymain(cmd, ch);
	}

	printlist();
	return 1;
}


#elif 0
#include<stdio.h>

char s[600000 + 1];

struct NODE {
	char c;
	NODE* prev, *next;
}a[100000 + 1];
int idx = 0;

NODE* myalloc() {
	return &a[idx++];
}

NODE* pHead = myalloc();
NODE* pTail = myalloc();
NODE* cur = pTail;

void addNode(NODE* t, char c) {
	NODE* p = myalloc();
	p->c = c;

	NODE* L = t->prev;
	t->prev = p;
	p->next = t;

	L->next = p;
	p->prev = t;
}

void delNode(NODE* d) {
	if (d == pHead || d == pTail)	return;
	d->prev->next = d->next;
	d->next->prev = d->prev;
}
void print_list() {
	for (NODE* iter = pHead->next; iter != pTail; iter = iter->next)
		printf("%c", iter->c);
	printf("\n");
}
int main() {
	pHead->next = pTail;
	pHead->prev = NULL;
	pTail->next = NULL;
	pTail->prev = pHead;

	scanf("%s ", s);
	for (int i = 0; s[i] != 0; i++)
		addNode(pTail, s[i]);


	print_list();

	for (NODE* iter = pHead->next; iter != pTail; iter = iter->next)
		if (iter->c == ';')
			delNode(iter);

	print_list();
}
#elif 0
#include<stdio.h>

char s[600000 + 1];

struct NODE {
	char c;
	NODE* prev, *next;
}a[100000 + 1];
int idx = 0;

NODE* myalloc() {
	return &a[idx++];
}

NODE* pHead = myalloc();
NODE* pTail = myalloc();
NODE* cur = pTail;

void addNode(NODE* here, char c) {
	NODE* p = myalloc();
	p->c = c;
	p->prev = here->prev;
	here->prev = p;

	p->next = here;
	p->prev->next = p;
}
//
//void addLast(char c) {
//	NODE* p = myalloc();
//	p->c = c; 
//	p->prev = pTail->prev;
//	pTail->prev = p;
//
//	p->next = p->prev->next;
//	p->prev->next = p; 
//}

void delNode(NODE* del) {
	if (del == pHead || del == pTail)	return;
	del->prev->next = del->next;
	del->next->prev = del->prev;
}


int main() {
	pHead->next = pTail;
	pTail->prev = pHead;


	scanf("%s ", s);
	for (int i = 0; s[i] != 0; i++) addLast(s[i]);

	int N;
	scanf("%d ", &N);
	for (int i = 0; i < N; i++) {
		char cmd;
		scanf("%c ", &cmd);
		switch (cmd) {
		case 'L':
			cur = cur->prev;
			break;
		case 'D':
			cur = cur->next;
			break;
		case 'B':
			delNode();
			break;
		case 'P':
			break;
		default:
			break;
		}

	}
}
#elif 0
#include <cstdio>

struct NODE {
	char ch;
	NODE* nxt, *prv;
};

NODE HEAD;
NODE TAIL;
NODE* cur;

char str[100000 + 1];

void init() {
	HEAD.nxt = &TAIL;
	HEAD.prv = NULL;
	TAIL.nxt = NULL;
	TAIL.prv = &HEAD;

	cur = &TAIL;
}

void insertNode(char ch) {
	NODE* p = new NODE;
	p->ch = ch;

	p->nxt = cur;
	p->prv = cur->prv;
	cur->prv->nxt = p;
	cur->prv = p;
}

void deleteNode()
{
	NODE* del = cur->prv;
	if (del == &HEAD) return;

	cur->prv->prv->nxt = cur;
	cur->prv = cur->prv->prv;

	delete del;
}

void curLeft()
{
	if (cur->prv->prv == NULL) return;
	cur = cur->prv;
}

void curRight()
{
	if (cur->nxt == NULL) return;
	cur = cur->nxt;
}

void print()
{
	NODE* head = &HEAD;
	head = head->nxt;

	for (;;)
	{
		if (head->nxt == NULL) break;

		printf("%c", head->ch);

		head = head->nxt;
	}
}

int main()
{
	int i;
	int N = 0;
	char ch = 0;

	init();
	scanf("%s", str);
	for (i = 0; str[i] != 0; i++)
	{
		insertNode(str[i]);
	}

	scanf("%d", &N);
	for (i = 0; i < N; i++)
	{
		scanf(" %c", &ch);
		if (ch == 'L')
		{
			curLeft();
		}
		else if (ch == 'D')
		{
			curRight();
		}
		else if (ch == 'B')
		{
			deleteNode();
		}
		else if (ch == 'P')
		{
			scanf("  %c", &ch);
			insertNode(ch);
		}
	}

	print();
	printf("\n");

	return 0;
}


#elif 0
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#define M	80000000

#define Swap(a, b)	do {int temp=(a); (a)=(b); (b) = temp;}while(0);
int a[M], b[M], c[M], ao[M], bo[M], co[M];
long long int is_cnt = 0, hs_cnt = 0, ss_cnt = 0;

void printArray(int a[]) {
	//return;
	for (int i = 0; i < 10; i++)
		printf("%8d ", a[i]);
	printf("\n");
}
void is(int a[]) {
	int cnt = 0;
	for (int i = 1; i < M; i++) {
		int key = a[i];
		int j = i - 1;
		while (j >= 0 && a[j] < key) {
			a[j + 1] = a[j];
			j = j - 1;
			is_cnt++;
		}
		a[j + 1] = key;
	}
}

void ss(int a[]) {
	//int cnt = 0;
	for (int i = 0; i < 10; i++) {
		int temp = a[i], idx = i;
		for (int j = i + 1; j < M; j++) {
			if (temp < a[j]) {
				temp = a[j]; idx = j;
			}
			ss_cnt++;
		}
		Swap(a[i], a[idx]);
	}
}

//int hy(int a[], int n) {
//	for (int i = 1; i < n; i++) {
//		int c = i;
//		while (c > 0) {
//			int root = (c - 1) / 2;
//			if (a[root] < a[c])	Swap(a[root], a[c]);
//			c = root;
//			hs_cnt++;
//		}		
//	}
//	return a[0];
//}


void heapify(int arr[], int n, int i) {
	int largest = i;
	int l = 2 * i + 1;
	int r = 2 * i + 2;
	hs_cnt++;
	if (l < n && arr[l] > arr[largest])
		largest = l;

	if (r < n && arr[r] > arr[largest])
		largest = r;

	if (largest != i) {
		Swap(arr[i], arr[largest]);
		heapify(arr, n, largest);
	}
}

void heapSort(int arr[], int n) {
	for (int i = n / 2 - 1; i >= 0; i--)
		heapify(arr, n, i);

	int cnt = 0;
	for (int i = n - 1; i >= 0; i--) {

		if (cnt < 10) {
			co[cnt] = arr[0];
			cnt++;
		}
		else
			return;

		Swap(arr[0], arr[i]);
		heapify(arr, i, 0);
	}
}


int main() {

	srand((unsigned int)time(NULL));
	for (int i = 0; i < M; i++) a[i] = b[i] = c[i] = rand() % M;
	//printArray(a);
	//printArray(b);
	//printArray(c);

	//is(a);
	//printArray(a);

	ss(b);

	printArray(b);

	//for (int i = M, cnt = 0; i >= 0 && cnt < M; i--) {
	//	hy(b, i);
	//	o[cnt++] = b[0];
	//	Swap(b[0], b[i - 1]);
	//}

	heapSort(c, M);

	printArray(co);

	printf("%lld \n%lld \n%lld \n", is_cnt, ss_cnt, hs_cnt);
	return 1;

}


#elif 0
#include<stdio.h>
int main() {
	int T, tc, N, M;
	char Map[100][100];
	scanf("%d ", &T);
	for (int tc = 1; tc <= T; tc++) {
		scanf("%d %d ", &N, &M);
		for (int i = 0; i < N; i++)
			for (int j = 0; j < M; j++)
				scanf("%c ", &Map[i][j]);

		for (int i = 0; i < N; i++)
			for (int j = 0; j < M; j++) {
				if (Map[i][j] == '#') {
					if (i == N - 1 || j == M - 1)
						goto err;
					if (Map[i + 1][j] == '#' && Map[i][j + 1] == '#' && Map[i + 1][j + 1] == '#')
						Map[i][j] = '.', Map[i + 1][j] = '.', Map[i][j + 1] = '.', Map[i + 1][j + 1] = '.';
					else
						goto err;

				}

			}

	pass:
		printf("#%d %s\n", tc, "YES");
		continue;

	err:
		printf("#%d %s\n", tc, "NO");

	}
	return 0;
}
#elif 0
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#define Swap(a,b)	do{int temp = (a) ; (a) = (b) ; (b) =temp;} while(0)

int number = 9;
int heap[9] = { 7,6,  5,8,  9,5,  6,1, 3 };
int o[9];

int heapify(int* a, int num) {
	for (int i = 1; i < num; i++) {
		int c = i;
		while (c > 0) {
			int root = (c - 1) / 2;
			if (a[root] < a[c])	Swap(a[root], a[c]);
			c = root;
		}
	}
	return heap[0];
}

int main() {
	int k = 0;
	srand((unsigned int)time(NULL));
	while (k < 5) {
		int cnt = 0;

		for (int i = 0; i < 9; i++)
			heap[i] = rand() % 100;

		for (int i = number; i >= 0 && cnt < 9; i--) {
			o[cnt++] = heapify(heap, i);
			Swap(heap[i - 1], heap[0]);
		}


		for (int i = 0; i < cnt; i++)		printf("%3d ", heap[i]);
		printf("\n");
		for (int i = 0; i < cnt; i++)		printf("%3d ", o[i]);
		printf("\n");
		k++;
	}
	return 1;
}
#elif 0
int n[11][50001];
int section[50001];
int ncnt;
int delcnt;
int del[50001];

struct USER {
	int userid;
	int prefer;
}u[100001];

void init() {
	ncnt = 0;
	delcnt = 0;
	for (int i = 0; i <= 100000; i++)
		u[i].prefer = 10;
	for (int i = 0; i <= 10; i++)
		n[i][0] = 0;
}

void addNews(int mSection, int mNewsId) {
	ncnt++;
	for (int i = 0; i <= 10; i++)
		n[i][mNewsId] = 0;
	n[mSection][mNewsId] = 10;
	section[mNewsId] = mSection;
	del[mNewsId] = 0;
}

void eraseNews(int mNewsId) {
	delcnt++;
	del[mNewsId] = 1;
}

void readNews(int mUserId, int mNewsId) {
	u[mUserId].prefer = section[mNewsId];
	for (int i = 0; i <= 10; i++)
		n[i][mNewsId]++;
}

void changeSection(int mNewsId, int mSection)
{
	n[section[mNewsId]][mNewsId] -= 10;
	section[mNewsId] = mSection;
	n[mSection][mNewsId] += 10;
}

int getList(int mUserId, int mList[])
{
	int* my_n;
	for (int i = 0; i < 10; i++)
		mList[i] = 0;
	my_n = n[u[mUserId].prefer];

	int end = 10;

	if (end > ncnt - delcnt)
		end = ncnt - delcnt;

	for (int i = 1; i <= ncnt; i++) {
		if (del[i]) continue;
		if (my_n[mList[9]] > my_n[i]) continue;

		int j;
		for (j = 9; j > 0; j--) {
			if (my_n[mList[j - 1]] > my_n[i])break;
			mList[j] = mList[j - 1];
		}
		mList[j] = i;
	}
	return end;
}
#elif 0
#include<stdio.h> 
#include<stdlib.h>
#include<time.h>
#define Swap(a,b) {int temp = (a); (a) =(b) ; (b) =temp;}
#define N	20
#define M	10
#define MOD 100

void printArray(int a[], int n) {

	for (int i = 0; i < n; i++)
		printf("[%2d] %2d\n", i, a[i]);
	printf("-------------------------------\n");
}

int main() {

	int a[1000], b[1000], c[1000];
	srand((unsigned int)time(NULL));

	for (int i = 0; i < N; i++)		a[i] = rand() % 100;

	for (int i = 1; i < N; i++) {
		int key = a[i];
		int j = i - 1;
		while (j >= 0 && a[j] < key) {
			a[j + 1] = a[j];
			j = j - 1;
		}
		a[j + 1] = key;
	}

	printArray(a, N);

	for (int i = 0; i < N; i++)		b[i] = a[i] = rand() % MOD;

	for (int i = 0; i < M; i++) {
		int MinV = 99999, MinK;
		for (int j = 0; j < N; j++) {
			if (MinV > b[j]) {
				MinV = b[j], MinK = j;
			}
		}
		b[MinK] = 99999;
		c[i] = MinK;
	}

	printArray(a, N);
	printArray(b, N);
	printArray(c, M);

	for (int i = 0; i < M; i++)
		printf("[%2d] %2d\n", i, a[c[i]]);
	printf("-------------------------------\n");



	for (int i = 0; i < N; i++)		b[i] = a[i] = rand() % MOD;
	a[20] = 55;

	printArray(a, N + 1);

	for (int i = 1; i < N + 1; i++) {
		int Key = a[i];
		int j = i - 1;
		while (j >= 0 && a[j] < Key) {
			a[j + 1] = a[j];
			j = j - 1;
		}
		a[j + 1] = Key;
	}

	printArray(a, N + 1);


	//for (int i = 1; i < N; i++) 
	{
		int Key = 55;
		int j = N - 1;
		while (j >= 0 && a[j] < Key) {
			a[j + 1] = a[j];
			j = j - 1;
		}
		a[j + 1] = Key;
	}

	printArray(a, N + 1);


	//for (int i = 1; i < N; i++) 
	{
		int Key = 55;
		int j = N - 1;
		while (j >= 0 && a[j] < Key) {
			a[j + 1] = a[j];
			j = j - 1;
		}
		a[j + 1] = Key;
	}

	printArray(a, N + 1);




	return 1;
}


#elif 0
#include <stdio.h>

void show_operaton_priority() {
	printf("%d \n", 1 << 1 + 1);
	printf("%d \n", (1 << 1) + 1);
}


#include<time.h>
void show_oerationtime() {
	clock_t start = clock();
	//....s
	printf("%lf \n", (clock() - start) / CLOCKS_PER_SEC);
}

void for_loop_operating() {

	for (printf("1"); printf("2"); printf("3")) {
		while (0);
		printf("\n");
	}

}

int main() {
	show_operaton_priority();
	for_loop_operating();
	return 0;
}
#elif 0
#include<stdio.h>
#include<malloc.h>


struct Node {
	int data;
	struct Node* prev;
	struct Node* next;
};

Node* list_create(int _data) {
	Node* node = (Node*)malloc(sizeof(Node));
	node->prev = NULL;
	node->next = NULL;
	node->data = _data;
	return node;
}

// 새로운 노드를 받아서 
Node* list_insert(Node* _head, Node* new_node) {
	Node* next = _head->next;
	_head->next = new_node;
	new_node->next = next;
	new_node->prev = _head;
	if (next != NULL) {
		next->prev = new_node;
	}
	return new_node;
}

int list_erase(Node* head, int _data) {
	Node* it = head->next;
	int ret = 0;

	while (it != NULL) {
		if (it->data == _data) {
			Node* prev = it->prev;
			Node* next = it->next;
			Node* tmp = it;
			it = it->next;

			prev->next = next;
			if (next != NULL) {
				next->prev = prev;
			}

			free(tmp);
			ret++;
		}
		else {
			it = it->next;
		}
	}
	return ret;
}

int main(int argc, char* argv[]) {
	int T, N;
	setbuf(stdout, NULL);

	scanf("%d", &T);

	for (int test_case = 1; test_case <= T; test_case++) {
		scanf("%d", &N);

		Node* head = list_create(NULL);
		printf("#%d", test_case);
		for (int i = 0; i < N; i++) {
			int mode, data;
			scanf("%d%d", &mode, &data);

			if (mode == 1) {		// 노드 추가 명령
				Node* node = list_create(data);
				list_insert(head, node);
			}
			else if (mode == 2)
			{
				printf(" %d", list_erase(head, data));
			}
		}

		while (head != NULL)
		{
			Node* tmp = head;
			head = head->next;
			free(tmp);
		}
		printf("\n");
	}
	return 0;
}
#elif 0
#include<iostream>
using namespace std;
struct Trie {
	Trie* ch[26];
	bool end;
	Trie() {
		end = false;
		for (int i = 0; i < 26; i++) ch[i] = NULL;
	}
	~Trie() {
		for (int i = 0; i < 26; i++) if (ch[i]) delete ch[i];
	}
	void insert(const char* s) {
		if (!*s) {
			this->end = true;
			return;
		}
		int now = *s - 'A';
		if (!ch[now]) ch[now] = new Trie;
		ch[now]->insert(s + 1);
	}
	bool find(const char* s) {
		if (!*s) {
			if (end) return true;
			return false;
		}
		int now = *s - 'A';
		if (!ch[now]) return false;
		return ch[now]->find(s + 1);
	}
};
int main(void) {
	Trie* root = new Trie;
	string s;
	root->insert(s.c_str());
	string tmp = "AAA";
	if (root->find(tmp.c_str())) cout << "Find!";
	else cout << "No";
	delete root;
}

#elif 0
#include <cstdio>

struct NODE {
	char ch;
	NODE* nxt, *prv;
};

NODE HEAD;
NODE TAIL;
NODE* cur;

char str[100000 + 1];

void init() {
	HEAD.nxt = &TAIL;
	HEAD.prv = NULL;
	TAIL.nxt = NULL;
	TAIL.prv = &HEAD;

	cur = &TAIL;
}

void insertNode(char ch)
{
	NODE* p = new NODE;
	p->ch = ch;

	p->nxt = cur;
	p->prv = cur->prv;
	cur->prv->nxt = p;
	cur->prv = p;
}

void deleteNode()
{
	NODE* del = cur->prv;
	if (del == &HEAD) return;

	cur->prv->prv->nxt = cur;
	cur->prv = cur->prv->prv;

	delete del;
}

void curLeft()
{
	if (cur->prv->prv == NULL) return;
	cur = cur->prv;
}

void curRight()
{
	if (cur->nxt == NULL) return;
	cur = cur->nxt;
}

void print()
{
	NODE* head = &HEAD;
	head = head->nxt;

	for (;;)
	{
		if (head->nxt == NULL) break;

		printf("%c", head->ch);

		head = head->nxt;
	}
}

int main()
{
	int i;
	int N = 0;
	char ch = 0;

	init();
	scanf("%s", str);
	for (i = 0; str[i] != 0; i++)
	{
		insertNode(str[i]);
	}

	scanf("%d", &N);
	for (i = 0; i < N; i++)
	{
		scanf(" %c", &ch);
		if (ch == 'L')
		{
			curLeft();
		}
		else if (ch == 'D')
		{
			curRight();
		}
		else if (ch == 'B')
		{
			deleteNode();
		}
		else if (ch == 'P')
		{
			scanf("  %c", &ch);
			insertNode(ch);
		}
	}

	print();
	printf("\n");

	return 0;
}


#elif 0
#include<iostream>
using namespace std;
struct Trie {
	Trie* ch[26];
	bool end;
	Trie() {
		end = false;
		for (int i = 0; i < 26; i++) ch[i] = NULL;
	}
	~Trie() {
		for (int i = 0; i < 26; i++) if (ch[i]) delete ch[i];
	}
	void insert(const char* s) {
		if (!*s) {
			this->end = true;
			return;
		}
		int now = *s - 'A';
		if (!ch[now]) ch[now] = new Trie;
		ch[now]->insert(s + 1);
	}
	bool find(const char* s) {
		if (!*s) {
			if (end) return true;
			return false;
		}
		int now = *s - 'A';
		if (!ch[now]) return false;
		return ch[now]->find(s + 1);
	}
};
int main(void) {
	Trie* root = new Trie;
	string s;
	root->insert(s.c_str());
	string tmp = "AAA";
	if (root->find(tmp.c_str())) cout << "Find!";
	else cout << "No";
	delete root;
}

#endif
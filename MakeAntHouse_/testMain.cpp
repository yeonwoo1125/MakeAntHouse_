﻿#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<cstdio>
#include<Windows.h> //커서 이동
#include<conio.h> //_getch()
#include<stdbool.h> //boolean 타입 사용
#include<time.h> //랜덤 함수 사용
#include<string>
#include <cstring>
#include<fstream> //파일처리
#include <mmsystem.h> //음악 넣기
#include <thread> //thread 

#pragma comment(lib, "winmm.lib") 

using namespace std;

#define MAGIC_KEY 224 //상하좌우 화살표가 들어올 때 선행되어 들어오는 숫자
#define SPACE 32 //스페이스 키 값
#define ENTER 13 //엔터 키 값
#define ESC 27 //esc 키 값

int cntAcc = 0;
string plz_space = "[ 스페이스나 엔터를 눌러주세요. ]";
string plz_key = "[ 아무 키나 눌러주세요. ]";

//파일 처리
ofstream ofs("antHouse.txt", ios::app);
ifstream ifs("antHouse.txt");
FILE* fp1;

//키보드 방향키 값
enum KEYBOARD {
	UP = 72,
	LEFT = 75,
	RIGHT = 77,
	DOWN = 80,
};

//콘솔창 설정
void SetConsoleVIew_main();
void fullScreen();

//화면 그리는 함수
void DrawFirstInfoGame();
void DrawSecondeInfoGame();
void DrawStartGame();
void DrawGameOver();
void DrawGamePass();
void DrawStartMiniGame();
void DrawDieAnt();

int progressBar();

//계정찾기
void DrawFIndAcc();
void FindId();
void FindPw();

void DrawRetryId();
void DrawRetryPwAnswer();

//메뉴 고르기
int guestMenu();
void DrawGuestLogin();
int userMenu();
void DrawLogin();
int mainMenu();
void DrawReadyGame();

//미니게임
int rockPaperScissors();
int quizGame();
int upDownGame();
int timingGame();
int startMiniGame();

//뷰
void InfoGame();
int startGame();


//로그인 관련
int CreateAccount();
void LoginAccount();
int QuestionAccount();
int allAccount();
bool checkSameAccount();
bool checkCntAcc();
bool checkUser(string acc, string pw);

//파일처리
int getFileData();
void setFileData(string acc, string pw, string name, string idAnswer, int pwAnswer, int h);

//먹이 먹는 함수
int eatFeed();

//스레드를 위한 함수
int threadStart();

//마우스 좌표 
void gotoxy(int x, int y) { //커서를 특정 위치로 이동시키는 함수
	COORD Pos;
	Pos.X = 2 * x; //1칸보다는 2칸씩 움직여야 자연스러움
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

//콘솔 크기, 타이틀
void SetConsoleVIew_main() {
	
	system("mode con:cols=100 lines=30"); //가로 50, 세로 30
	system("title Make Ant House By Yeonwoo"); //타이틀
}

//전체화면
void fullScreen() {
	SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_FULLSCREEN_MODE, 0);
}

//로그인 시 필요한 거
class Login { //유저가 로그인 시 계정 저장 및 계정 생성 시 정보 저장
	string user_account; //유저계정
	string user_password; //비번
	bool loginCheck=false; //로그인 성공 여부 파악
	string idAnswer; //아이디 힌트
	int pwAnswer; //비번 힌트
	string user_name; //이름
	int houseSize; //개미집 크기
	 //사용자 별명, 인게임 내 사용되는 이름
public:                        
	Login() {
		this->houseSize = 10;
	}
	void setUsername(string n) { this->user_name = n; }
	string getUsername() { return user_name; }
	void setHouseSize(int h) { this->houseSize += h; }
	int getHouseSize() { return houseSize; }
	void setIdAnswer(string idAns) { this->idAnswer = idAns; }
	string getIdAnswer() { return idAnswer; }
	void setPwAnswer(int pwAns) { this->pwAnswer = pwAns; }
	int getPwAnswer() { return pwAnswer; }
	void setUserAcc(string userAcc) { this->user_account = userAcc; }
	string getUserAcc() { return user_account; }
	string getUserPw() { return user_password; }
	void setUserPw(string userPw) { this->user_password = userPw; }
	void setLoginCheck(bool loginCheck) { this->loginCheck = loginCheck; }
	bool getLoginCheck() { return loginCheck; }

	
	~Login() {}
};
Login *user[3];
Login player;

int eatFeed1 = true;
class Feed {
private:
	int feed_x, feed_y;
	int feedCnt = 3; //기본적으로 집에 3개 생성
	bool checkEatFeed = false;

public://현재 개미집에 생성된 먹이의 수
	int getFeedCnt() { return feedCnt; }
	int getFeedX() { return feed_x; }
	void setFeedX(int x) { this->feed_x = x; }
	int getFeedY() { return feed_y; }
	void setFeedY(int y) { this->feed_y = y; }
	void setFeedCnt(int n) { feedCnt -= n; }
	bool getCheckEatFeed() { return checkEatFeed; }
	void setCheckEatFeed(bool f) { this->checkEatFeed = f; }
	
	int ranFeed() {
		eatFeed1 = true;
		while (true) {
			if (!eatFeed1) return 0;
			if (getFeedCnt() > 9) {
				DrawDieAnt();
				return 0;
			}

			feed_x=rand() % (player.getHouseSize() - 5) + 7; //개미집 내부에 먹이 생성 - >개미집 가로세로보다 작은 수임
			feed_y=4 + rand() % (player.getHouseSize() - 5) + 3;
	
			gotoxy(feed_x, feed_y);
			cout << "@";
			Sleep(5000);
			feedCnt++;
		}
		//5초마다 먹이 생성
		return 0;
	}
};
Feed f1;

bool goMain = true;
//개미 객체
class Ant { //개미집에서 움직일 개미 객체
private:
	int ant_x, ant_y;
	int input = 0;
	string antShape = "***";
	int fx, fy;
public:
	Ant() : ant_x(8), ant_y(6) {}
	int getAntX() { return ant_x; }
	int getAntY() { return ant_y; }

	int moveInHouse() { //개미집 안에서 움직임
		while (true) {
			input = _getch();
			if (input == MAGIC_KEY) {
				input = _getch();
				switch (input)
				{
				case UP:
					gotoxy(ant_x, ant_y); //개미가 지나간 자리 지우기
					cout << "   ";
					if (ant_y < 5) ant_y = 4; //벽이랑 충돌체크
					else ant_y--;
					gotoxy(ant_x, ant_y);
					cout << antShape;
					if (ant_x == f1.getFeedX() && ant_y == f1.getFeedY()) {
						eatFeed1 = false;
						return 0;
					}
					else eatFeed1 = true;
					break;
				case DOWN:
					gotoxy(ant_x, ant_y);
					cout << "   ";
					if (ant_y > player.getHouseSize() + 1) ant_y = player.getHouseSize() + 2;
					else ant_y++;
					gotoxy(ant_x, ant_y);
					cout << antShape;
					if (ant_x == f1.getFeedX() && ant_y == f1.getFeedY()) {
						eatFeed1 = false;
						return 0;
					}
					else eatFeed1 = true;
					break;
				case RIGHT:
					gotoxy(ant_x, ant_y);
					cout << "   ";
					if (ant_x > player.getHouseSize() + 3) {
						ant_x = player.getHouseSize() + 4;
					}
					else ant_x++;
					gotoxy(ant_x, ant_y);
					cout << antShape;
					if (ant_x == f1.getFeedX() && ant_y == f1.getFeedY()) {
						eatFeed1 = false;
						return 0;
					}
					else eatFeed1 = true;
					break;
				case LEFT:
					gotoxy(ant_x, ant_y);
					cout << "   ";
					if (ant_x < 9) ant_x = 8;
					else ant_x--;
					gotoxy(ant_x, ant_y);
					cout << antShape;
					if (ant_x == f1.getFeedX() && ant_y == f1.getFeedY()) { 
						eatFeed1 = false;
						return 0; 
					}
					else eatFeed1 = true;
					break;
				case ESC:
					return 0;
				}
			}
		}
	}

	void drawAntHouse(int r) {
		gotoxy(7, 3);
		for (int i = 0; i < r; i++) { //맨 윗줄
			cout << "▨";
		}
		for (int i = 0; i < r - 1; i++) { //세로 1
			gotoxy(7, 4 + i);
			cout << "▨";
		}
		for (int i = 0; i < r - 1; i++) { //세로 2
			gotoxy(6 + r, 4 + i);
			cout << "▨";
		}
		gotoxy(7, 3 + r);
		for (int i = 0; i < r; i++) { //맨 아랫줄
			cout << "▨";
		}
	}
	~Ant() {}
};
Ant a1;

//사용자가 입력한 계정이 있는 계정인지 체크
bool checkUser(string acc, string pw) { 
	int input = 0;
	for (int i = 0; i < cntAcc; i++) {
		if (user[i]->getUserAcc() == acc && user[i]->getUserPw() == pw) {
			player.setUserAcc(acc);
			player.setUsername(user[i]->getUsername());
			player.setUserPw(pw);
			player.setIdAnswer(user[i]->getIdAnswer());
			player.setPwAnswer(user[i]->getPwAnswer());

			player.setLoginCheck(true);
			system("cls");
			gotoxy(21, 12);
			cout << "로그인 성공";
			gotoxy(18, 15);
			cout << plz_key;
			return true;
		}
		else if (user[i]->getUserPw() != pw || user[i]->getUserAcc() != acc) {
			player.setLoginCheck(false);
			system("cls");
			gotoxy(21, 9);
			cout << "로그인 실패" << endl;
			gotoxy(15, 11);
			cout << "다시 시도하시려면 스페이스를 눌러주세요." << endl;
			gotoxy(15, 12);
			cout << "스페이스를 누르면 로그인을 재시도합니다." << endl;
			gotoxy(17, 13);
			cout << "나가시려면 ESC를 눌러주세요.";
			while (true) {
				input = _getch();
				if (input == ESC) { return false; }
				if (input == SPACE) {
					system("cls");
					string acc;
					string pw;
					gotoxy(16, 8);
					cout << "계정 입력 : ";
					cin >> acc;
					gotoxy(16, 10);
					cout << "비밀번호 입력 : ";
					cin >> pw;
					checkUser(acc, pw);
				}
			}
		}
	}
}

//파일에 저장하기

void setFileData(string acc, string pw, string name, string idAnswer, int pwAnswer, int h) {
	ofs << acc << "\t" << pw << "\t" << name << "\t" << idAnswer << "\t" << pwAnswer<<"\t"<<h ;
	ofs.close();
}
//파일에 저장된 내용 가져와서 저장하기
int getFileData() {
	fp1 = fopen("antHouse.txt", "r");
	char acc[20], pw[20], name[20], accAn[20];
	int  pwAn, h;
	cntAcc = 0;

	string line;
	//라인수세기
	while (!ifs.eof()){
		if (getline(ifs, line)) cntAcc++;
	}
	ifs.close();
	if (cntAcc == 0) return 0;

	for (int i = 0; i < cntAcc; i++) {
		fscanf_s(fp1, "%s", acc, 20);
		fscanf_s(fp1, "%s", pw, 20);
		fscanf_s(fp1, "%s", name, 20);
		fscanf_s(fp1, "%s", accAn, 20);
		fscanf_s(fp1, "%d", &pwAn);
		fscanf_s(fp1, "%d", &h);

		user[i] = new Login;
		user[i]->setUserAcc(acc);
		user[i]->setUserPw(pw);
		user[i]->setUsername(name);
		user[i]->setIdAnswer(accAn);
		user[i]->setPwAnswer(pwAn);
		user[i]->setHouseSize(h);
	}	
	return 0;
}

//개미집에서 보이는 메뉴 그리기
void DrawInGameMenu() {
	gotoxy(25, 10);
	cout << "나가기";
	gotoxy(25, 11);
	cout << "집으로 돌아가기";
}

//메인 메뉴 그리기
void DrawReadyGame() {
	system("cls");    //화면을 클리어 해주는 함수 입니다.
	gotoxy(15, 3);
	cout << "**************************************";
	gotoxy(15, 4);
	cout << "*                                    *";
	gotoxy(15, 5);
	cout << "*              영차영차              *";
	gotoxy(15, 6);
	cout << "*        개미집을 만들어주자         *";
	gotoxy(15, 7);
	cout << "*                                    *";
	gotoxy(15, 8);
	cout << "**************************************";
	gotoxy(22, 11);
	cout << "게임시작";
	gotoxy(22, 13);
	cout << "게임설명";
	gotoxy(19, 15);
	cout << "회원가입 및 계정찾기" << endl;
	gotoxy(22, 17);
	cout << "나 가 기" << endl;

	gotoxy(16, 22);
	cout << plz_space;
}

//게임 설명 그리기
void DrawFirstInfoGame()
{
	system("cls");
	gotoxy(3, 1);
	cout << "┏━━━━━━━━━━━┓";
	gotoxy(3, 3);
	cout << "   게임설명";
	gotoxy(3, 5);
	cout << "┗━━━━━━━━━━━┛";

	gotoxy(3, 10);
	cout << "개미들이 새로운 집으로 이사를 왔어요.";
	gotoxy(3, 11);
	cout << "넓은 집에서 살고 싶은 개미를 도와주세요!";
	gotoxy(3, 14);
	cout << "개미에게 먹이를 먹이고, 집을 넓히기 위한 게임을 해주세요.";
	gotoxy(3, 15);
	cout << "개미의 집에 먹이가 생기면 먹어주세요. 밥을 먹지 않으면 죽을 수도 있습니다.";
	gotoxy(3, 16);
	cout << "밥을 먹고 힘을 내 집을 넓혀주세요!";
	gotoxy(3, 17);
	cout << "당신의 개미의 집이 가장 큰 집이 되도록 키워주세요.";

	gotoxy(38, 24);
	cout << "다음 페이지";
	gotoxy(37, 24);
	cout << ">";
	system("pause>null");
}
//순서 설명 그리기
void DrawSecondeInfoGame()
{
	system("cls");
	gotoxy(3, 1);
	cout << "┏━━━━━━━━━━━┓";
	gotoxy(3, 3);
	cout << "   순서설명";
	gotoxy(3, 5);
	cout << "┗━━━━━━━━━━━┛";

	gotoxy(3, 9);
	cout << "게임을 처음 시작하시는 분은 회원가입을 해주세요.(로그인->회원가입)";
	gotoxy(3, 10);
	cout << "이미 계정이 존재한다면 로그인을 한 후 게임을 진행하면 됩니다.";
	gotoxy(3, 12);
	cout << "계정을 잃어버렸을 경우, 계정 찾기를 통해 계정을 찾아주세요.(로그인->계정 찾기)";
	gotoxy(3, 13);
	cout << "로그인 없이 게임을 하고 싶다면 게스트 로그인을 통해 게임을 즐겨주세요.";
	gotoxy(3, 15);
	cout << "제일 처음 게임을 시작하면, 미니게임을 통해 기본 크기의 개미집을 얻게 됩니다.";
	gotoxy(3, 16);
	cout << "개미집에 생성되는 개미의 먹이를 먹으면 미니게임을 할 수 있습니다. ";
	gotoxy(3, 17);
	cout << "하지만 오랜 시간 먹이를 먹지 않으면 개미가 죽을 수도 있으니 조심하세요.";
	gotoxy(3, 18);
	cout << "또한 개미의 집이 너무 작아진다면 개미가 이사갈 수도 있으니 잘 관리해주세요.";
	gotoxy(3, 20);
	cout << "그럼 즐거운 시간 되세요!";
	gotoxy(17, 24);
	cout << plz_key;
}


//개미 죽는 모습 - 게임 오버 그리기
void DrawDieAnt() { //개미집이 0보다 작아졌을 경우, 먹이를 먹지 않았을 경우
	system("cls");
	if (f1.getFeedCnt() > 9) {
		gotoxy(18, 10);
		cout << player.getUsername() << "님의 개미가 굶어죽었습니다.";
		gotoxy(18, 11);
		cout << player.getUsername() << "님의 집의 크기는 " << player.getHouseSize() << "입니다.";
	}
	else if (player.getHouseSize() <= 0) {
		gotoxy(18, 10);
		cout << player.getUsername() << "님의 집이 부숴져 개미가 이사를 갔습니다.";
	}
}

//미니게임 시작 화면 그리기
void DrawStartMiniGame() {
	system("cls");
	gotoxy(17, 8);
	cout << "|------------------------|";
	gotoxy(17, 9);
	cout << "|   현재 집이 없으므로   |";
	gotoxy(17, 10);
	cout << "|  미니게임을 실행합니다 |";
	gotoxy(17, 11);
	cout << "|------------------------|";
	gotoxy(17, 14);
	cout << plz_key;
	//return 0;
}

//게임 오버 그리기
void DrawGameOver() {
	gotoxy(18, 8);
	cout << "|-----------------|";
	gotoxy(18, 9);
	cout << "|                 |";
	gotoxy(18, 10);
	cout << "| G A M E O V E R |";
	gotoxy(18, 11);
	cout << "|                 |";
	gotoxy(18, 12);
	cout << "|-----------------|";
	gotoxy(17, 15);
	cout << plz_key;
	system("pause>null");
}

//게임 통과 그리기
void DrawGamePass() {
	gotoxy(18, 8);
	cout << "|-----------------|";
	gotoxy(18, 9);
	cout << "|                 |";
	gotoxy(18, 10);
	cout << "| G A M E P A S S |";
	gotoxy(18, 11);
	cout << "|                 |";
	gotoxy(18, 12);
	cout << "|-----------------|";
	gotoxy(17, 14);
	cout << plz_key;
	system("pause>null");
}

//로그인 화면 그리기
void DrawLogin() {
	system("cls");    //화면을 클리어 해주는 함수
	gotoxy(17, 3);
	cout << "******************************" ;
	gotoxy(17, 4);
	cout << "*                            *";
	gotoxy(17, 5);
	cout << "*   회원가입 및  계정 찾기   *";
	gotoxy(17, 6);
	cout << "*                            *";
	gotoxy(17, 7);
	cout << "******************************";

	gotoxy(22, 12);
	cout << "회원가입";
	gotoxy(22, 14);
	cout << "계정찾기";
	gotoxy(22, 16);
	cout << "계정보기";
	gotoxy(22, 18);
	cout << "나 가 기";

	gotoxy(17, 22);
	cout << plz_space;
}

//게스트 로그인 화면 그리기
void DrawGuestLogin() {

	system("cls");
	gotoxy(17, 8);
	cout << "게스트로 로그인 하시겠습니까?";
	gotoxy(13, 9);
	cout << "게스트로 로그인 시 게임 종료 후 저장되지 않습니다.";
	gotoxy(23, 12);
	cout << "로 그 인";
	gotoxy(23, 13);
	cout << "게임시작";
	gotoxy(23, 14);
	cout << "나 가 기";

	gotoxy(17, 17);
	cout << plz_space;
}

//아이디 다시 입력
void DrawRetryId() {
	gotoxy(16, 10);
	cout << "계정명은 10자 이내로 입력해주세요.";
	gotoxy(17, 17);
	cout << plz_key;
	system("pause>null");
}

//비밀번호 다시 입력
void DrawRetryPwAnswer() {
	gotoxy(15, 10);
	cout << "태어난 달은 1~12 중 숫자로 입력해주세요.";
	gotoxy(17, 17);
	cout << plz_key;
	system("pause>null");
}

//계정 3개까지만 만드세요
bool checkCntAcc() {
	if (cntAcc > 2) return false;
	return true;
}

//계정찾기 화면 그리기
void DrawFIndAcc() {
	system("cls");
	gotoxy(20, 3);
	cout << "*****************";
	gotoxy(20, 4);
	cout << "*               *";
	gotoxy(20, 5);
	cout << "*   계정 찾기   *";
	gotoxy(20, 6);
	cout << "*               *";
	gotoxy(20, 7);
	cout << "*****************";
	gotoxy(15, 13);
	cout << "계정명 찾기";
	gotoxy(27, 13);
	cout << "비밀번호 찾기";
	gotoxy(15, 15);
	cout << "로그인 하기";
	gotoxy(27, 15);
	cout << "나 가 기";

	gotoxy(17, 20);
	cout << plz_space;
	
}

//아이디 찾기 화면 그리기
void FindId() {
	string answer;
	int input = 0;
	system("cls");
	gotoxy(20, 3);
	cout << "******************";
	gotoxy(20, 4);
	cout << "*   아이디찾기   *";
	gotoxy(20, 5);
	cout << "******************";
	while (true) {
		system("cls");
		if (cntAcc==0) {
			gotoxy(18, 11);
			cout << "먼저 계정을 생성해주세요.";
			gotoxy(18, 14);
			cout << plz_key;
			system("pause>null");
			break;
		}

		gotoxy(14, 8);
		cout <<"질문에 대한 답을 하시면 힌트가 제공됩니다.";
		gotoxy(14, 10);
		cout << "가장 좋아하는 전공은? : ";
		cin >> answer;

		for (int i = 0; i < cntAcc; i++) {
			if (answer == user[i]->getIdAnswer()) {
				gotoxy(17, 14);
				cout <<user[i]->getUsername() << "님의 아이디는 " << user[i]->getUserAcc() << "입니다";
				gotoxy(19, 18);
				cout << plz_key;
				system("pause>null");
				break;
			}
			else {
				gotoxy(20, 16);
				cout << "다시 입력해주세요.";
				gotoxy(18, 17);
				cout << "ESC를 누르면 종료합니다.";

				input = _getch();
				if (input == ESC) break;
				else continue;
			}
		}
		break;
	}
}

//비밀번호 찾기 화면 그리기
void FindPw() {
	int answer;
	string userId;
	int input = 0;
	system("cls");
	gotoxy(20, 3);
	cout << "********************";
	gotoxy(20, 4);
	cout << "*   비밀번호찾기   *";
	gotoxy(20, 5);
	cout << "********************";
	while (true) {
		system("cls");
		if (cntAcc == 0) {
			gotoxy(18, 11);
			cout << "먼저 계정을 생성해주세요.";
			gotoxy(17, 18);
			cout << plz_key;
			system("pause>null");
			break;
		}

		gotoxy(15, 10);
		cout << "아이디를 입력해주세요. : ";
		cin >> userId;
		gotoxy(15, 12);
		cout << "질문에 대한 답을 하시면 힌트가 제공됩니다.";
		gotoxy(15, 13);
		cout << "태어난 달은? (두글자) :  ";
		cin >> answer;

		for (int i = 0; i < cntAcc; i++) {
			if (answer == user[i]->getPwAnswer()) {
				gotoxy(15, 15);
				cout <<user[i]->getUsername() << "님의 비밀번호는 " << user[i]->getUserPw();
				gotoxy(17, 14);
				cout << plz_key;
				system("pause>null");
				break;
			}
			else {
				gotoxy(20, 15);
				cout << "다시 입력해주세요.";
				gotoxy(18, 16);
				cout << "ESC를 누르면 종료합니다.";

				input = _getch();
				if (input == ESC) break;
				else continue;
			}
		}
		break;
	}
}

int guestMenu() {
	int y = 0; //커서의 y 위치
	int input = 0; //키보드 입력을 받을 변수
	DrawGuestLogin();
	while (true) { //게임 루프

		if (y < 0) { //커서가 위로 그만 올라가게
			y = 2;
		}
		else if (y > 2) { //커서가 아래로 그만 내려가게
			y = 0;
		}

		gotoxy(22, 12 + y); //위치조정
		cout << ">";

		input = _getch();
		//→←↑↓ 방향키를 누를 경우
		if (input == MAGIC_KEY) { //224가 들어옴
			switch (_getch()) //한번 더 받음

			{
			case UP: //위
				--y;
				break;
			case DOWN: //아래
				++y;
				break;
			}
		}
		//메인 메뉴 고름
		else if (input == SPACE || input == ENTER) { //키보드가 스페이스일 경우
			switch (y) { //y위치에 따라 판단
			case 0:
				LoginAccount();
				startMiniGame();
			case 1:
				DrawStartMiniGame();
				system("pause>null");
				startMiniGame();

				break;
			case 2:
				return 0;
			}
		}
	}
}

int userMenu() {
	int y = 0; //커서의 y 위치
	int input = 0; //키보드 입력을 받을 변수
	while (true) { //게임 루프
		DrawLogin(); //준비화면 그리기

		if (y < 0) { //커서가 위로 그만 올라가게
			y = 6;
		}
		else if (y > 6) { //커서가 아래로 그만 내려가게
			y = 0;
		}

		gotoxy(21, 12 + y); //위치조정
		cout << ">";

		input = _getch();
		//→←↑↓ 방향키를 누를 경우
		if (input == MAGIC_KEY) { //224가 들어옴
			switch (_getch()) //한번 더 받음
			{
			case UP: //위
				y-=2;
				break;
			case DOWN: //아래
				y+=2;
				break;
			}
		}
		//메인 메뉴 고름
		else if (input == SPACE || input == ENTER) { //키보드가 스페이스일 경우
			switch (y) { //y위치에 따라 판단
			case 0:
				CreateAccount();
				break;
			case 2:
				QuestionAccount();
				break;
			case 4:
				allAccount();
				break;
			case 6:
				return 0;
			}
		}
	}
}

int mainMenu() {
	int y = 0; //커서의 y 위치
	int input = 0; //키보드 입력을 받을 변수
	while (true) { //게임 루프
		DrawReadyGame(); //준비화면 그리기

		if (y < 0) { //커서가 위로 그만 올라가게
			y = 6;
		}
		else if (y >6) { //커서가 아래로 그만 내려가게
			y = 0;
		}
		if (y == 4) {
			gotoxy(18, 11 + y); //위치조정
			cout << ">";
		}
		else {
			gotoxy(21, 11 + y); //위치조정
			cout << ">";
		}
		input = _getch();
		//→←↑↓ 방향키를 누를 경우
		if (input == MAGIC_KEY) { //224가 들어옴
			switch (_getch()) //한번 더 받음
			{
			case UP: //위
				y-=2;
				break;
			case DOWN: //아래
				y+=2;
				break;
			}
		}
		//메인 메뉴 고름
		else if (input == SPACE || input == ENTER) { //키보드가 스페이스일 경우
			switch (y) { //y위치에 따라 판단
			case 0:
				startGame();
				break;
			case 2:
				InfoGame();
				break;
			case 4:
				userMenu();
				break;
			case 6:
				system("cls");
				for (int i = 0; i < cntAcc; i++)
					delete user[i];
				return 0;
			}
		}
	}
}

//미니 게임실행
int startMiniGame() {
	system("cls");
	gotoxy(17, 10);
	cout << "미니게임을 실행합니다";
	gotoxy(17, 8);
	cout << plz_key;
	system("pause>null");
	int miniGame;
	miniGame = rand() % 4;
	switch (miniGame)
	{
	case 0:
		system("cls");
		rockPaperScissors();
		system("cls");
		break;
	case 1:
		system("cls");
		quizGame();
		system("cls");
		break;
	case 2:
		system("cls");
		upDownGame();
		system("cls");
		break;
	case 3:
		system("cls");
		timingGame();
		break;
	}
	
	threadStart();
	return 0;
}

// 가위바위보
int rockPaperScissors() {
	string user_select;
	string RPS[] = { "가위","바위","보","가위","바위","보","가위","바위","보","가위","바위","보" };
	string com_select;
	int win_cnt = 0;
	int lose_cnt = 0;

	while (true) {
		gotoxy(23, 3);
		cout << "가위바위보";
		gotoxy(14, 8);
		cout << "가위 바위 보 중에 하나를 골라 입력해주세요.";
		gotoxy(37, 5);
		cout << win_cnt << "승 " << lose_cnt << "패 ";
		gotoxy(14, 10);
		cout << player.getUsername() << " : ";
		cin >> user_select;

		com_select = RPS[rand() % 12]; //컴퓨터는 가위바위보 중 하나를 랜덤으로 가져옴
		gotoxy(14, 12);
		cout << "나쁜 개미 : " << com_select;

		if (user_select == "가위") { //유저가 가위를 골랐을 경우
			if (com_select == "바위") {
				gotoxy(22, 14);
				cout << "졌습니다.";
				lose_cnt++;
			}
			else if (com_select == "보") {
				gotoxy(22, 14);
				cout << "이겼습니다.";
				win_cnt++;
			}
			else if (com_select == "가위") {
				gotoxy(22, 14);
				cout << "비겼습니다.";
			}
		}
		else if (user_select == "바위") { //유저가 바위를 골랐을 경우
			if (com_select == "보") {
				gotoxy(22, 14);
				cout << "졌습니다.";
				lose_cnt++;
			}
			else if (com_select == "가위") {
				gotoxy(22, 14);
				cout << "이겼습니다.";
				win_cnt++;
			}
			else if (com_select == "바위") {
				gotoxy(22, 14);
				cout << "비겼습니다.";

			}
		}
		else if (user_select == "보") { //유저가 보를 골랐을 경우
			if (com_select == "가위") {
				gotoxy(22, 14);
				cout << "졌습니다.";
				lose_cnt++;
			}
			else if (com_select == "바위") {
				gotoxy(22, 14);
				cout << "이겼습니다.";
				win_cnt++;
			}
			else if (com_select == "보") {
				gotoxy(22, 14);
				cout << "비겼습니다.";
			}
		}
		else {
			gotoxy(22, 14);
			cout << "잘못 골랐습니다.";
		}

		if (lose_cnt == 2) {
			gotoxy(9, 17);
			cout << "##                 #######            ########       ##########";
			gotoxy(9, 18);
			cout << "##              ###       ###        ##              ##";
			gotoxy(9, 19);
			cout << "##             ###         ###       ##              ##";
			gotoxy(9, 20);
			cout << "##             ###         ###        ########       #########";
			gotoxy(9, 21);
			cout << "##             ###         ###               ##      ##";
			gotoxy(9, 22);
			cout << "##              ###       ###                ##      ##";
			gotoxy(9, 23);
			cout << "##########         #######            ########       ##########";

			Sleep(2000);
			system("cls");
			DrawGameOver();

			player.setHouseSize(-rand() % 5 + 1);
			if (player.getHouseSize() <= 0) DrawDieAnt();
			return 0;
		}
		else if (win_cnt == 2) {
			gotoxy(8, 17);
			cout << "##        ###        ##    ############     ####      ##     ####";
			gotoxy(8, 18);
			cout << " ##      ## ##      ##          ##          ## ##     ##      ###";
			gotoxy(8, 19);
			cout << "  ##    ##   ##    ##           ##          ##  ##    ##       ##";
			gotoxy(8, 20);
			cout << "   ##  ##     ##  ##            ##          ##   ##   ##        #";
			gotoxy(8, 21);
			cout << "    ####       ####             ##          ##    ##  ##        ";
			gotoxy(8, 22);
			cout << "    ###         ###        ############     ##     #####       ##";
			Sleep(2500);
			system("cls");
			DrawGamePass();
			
			player.setHouseSize(rand() % 7 + 2);
			if (player.getHouseSize() <= 0) DrawDieAnt();
			return 0;
		}
		Sleep(1000);
		system("cls"); //화면 지우기
	}
}

//퀴즈 게임
int quizGame() {
	string quiz[] = { "대한민국의 수도는 ?(두글자) : ", "3 * 3 = ", " 3 * 5 + 9 = ","좋아하는 노래를 적어주세요! : ","2 + 3 * 5 = ",
		"부엉이가 수영할 때 내는 소리는 ?(세글자) : ","세상에서 가장 가난한 왕은 ?(네글자) : ","4 / 2 + 6 = ","7 * 8 / 4 = ","깃허브 아이콘의 동물 이름은 ?(세글자) : ", "좋아하는 전공은 ? : ",
		"미림여정의 풀네임은 ? (우리학교이름): ", "자동차가 놀라면 ?(네글자) : ","아몬드가 죽으면 ?(다섯글자) : ", "바닥에 떨어진 붉은 동전의 이름은 ?(네글자) : ","딸기가 직장을 잃으면 ?(네글자) :  ",
		"5 * 4 + 8 = ","9 / 3 * 2 + 7 = ","왕이 헤어질 때 하는 말은 ?(세글자) : ","지금 하고 싶은 말을 적어주세요! : ","좋아하는 동물을 적어주세요! : ","지금 먹고 싶은 음식을 적어주세요! : ",
		};
	string answer[] = { "서울","9","24","","17","첨부엉", "최저임금", "8", "14","고양이","","미림여자정보과학고등학교","카놀라유", "다이아몬드","홍길동전","딸기시럽","28","13","바이킹",
						"","","",};
	string q, user_answer;
	int i;
	int win_cnt = 0;
	int lose_cnt = 0;

	while (true) {
		gotoxy(23, 3);
		cout << "퀴즈 게임";

		i = rand() % 22;
		q = quiz[i];
		gotoxy(14, 10);
		cout << q;
		cin >> user_answer;
		

		if (user_answer == answer[i]) {
			gotoxy(8, 14);
			cout << "####       ##         ##########        ##       ##           ##########";
			gotoxy(8, 15);
			cout << "## ##      ##         ##                 ##     ##            ##########";
			gotoxy(8, 16);
			cout << "##  ##     ##         ##                  ##   ##                 ##";
			gotoxy(8, 17);
			cout << "##   ##    ##         #########            ## ##                  ##";
			gotoxy(8, 18);
			cout << "##    ##   ##         ##                   ## ##                  ##";
			gotoxy(8, 19);
			cout << "##     ##  ##         ##                  ##   ##                 ##";
			gotoxy(8, 20);
			cout << "##      ## ##         ##                 ##     ##                ##";
			gotoxy(8, 21);
			cout << "##       ####         ##########        ##       ##               ##";
			win_cnt++;
			if (answer[i] == "") { //답이 ""일 경우 무조건 정답처리 
				gotoxy(8, 14);
				cout << "####       ##         ##########        ##       ##           ##########";
				gotoxy(8, 15);
				cout << "## ##      ##         ##                 ##     ##            ##########";
				gotoxy(8, 16);
				cout << "##  ##     ##         ##                  ##   ##                 ##";
				gotoxy(8, 17);
				cout << "##   ##    ##         #########            ## ##                  ##";
				gotoxy(8, 18);
				cout << "##    ##   ##         ##                   ## ##                  ##";
				gotoxy(8, 19);
				cout << "##     ##  ##         ##                  ##   ##                 ##";
				gotoxy(8, 20);
				cout << "##      ## ##         ##                 ##     ##                ##";
				gotoxy(8, 21);
				cout << "##       ####         ##########        ##       ##               ##";
				win_cnt++;
			}
		}
		else {
			gotoxy(22, 12);
			cout << "틀렸습니다.";
			lose_cnt++;
		}

		if (lose_cnt == 2) {
			system("cls");
			gotoxy(9, 17);
			cout << "##                 #######            ########       ##########";
			gotoxy(9, 18);
			cout << "##              ###       ###        ##              ##";
			gotoxy(9, 19);
			cout << "##             ###         ###       ##              ##";
			gotoxy(9, 20);
			cout << "##             ###         ###        ########       #########";
			gotoxy(9, 21);
			cout << "##             ###         ###               ##      ##";
			gotoxy(9, 22);
			cout << "##              ###       ###                ##      ##";
			gotoxy(9, 23);
			cout << "##########         #######            ########       ##########";

			Sleep(2500);
			system("cls");
			DrawGameOver();
			player.setHouseSize(-rand() % 5 + 1);

			if (player.getHouseSize() <= 0) DrawDieAnt();
			return 0;
		}
		else if (win_cnt == 2) {
			system("cls");
			gotoxy(8, 17);
			cout << "##        ###        ##    ############     ####      ##     ####";
			gotoxy(8, 18);
			cout << " ##      ## ##      ##          ##          ## ##     ##      ###";
			gotoxy(8, 19);
			cout << "  ##    ##   ##    ##           ##          ##  ##    ##       ##";
			gotoxy(8, 20);
			cout << "   ##  ##     ##  ##            ##          ##   ##   ##        #";
			gotoxy(8, 21);
			cout << "    ####       ####             ##          ##    ##  ##        ";
			gotoxy(8, 22);
			cout << "    ###         ###        ############     ##     #####       ##";
			Sleep(2500);
			system("cls");
			DrawGamePass();
			player.setHouseSize(rand() % 7 + 2);

			return 0;
		}
		Sleep(1500);
		system("cls"); //화면 지우기
	}
}

//업다운 게임
int upDownGame() {
	int com_sel = rand() % 50 + 1;
	int user_sel;
	int cnt = 0;
	while (cnt <= 5) {
		gotoxy(21, 3);
		cout << "업다운 게임";
		gotoxy(11, 6);
		cout << "1~50 까지의 수 중에서 개미가 원하는 숫자를 맞춰보세요!";
		gotoxy(11, 7);
		cout << "기회는 총 5번입니다. 과연 개미는 어떤 수를 골랐을까요? : ";

		cin >> user_sel;
		cnt++;
		if (com_sel == user_sel) {
			gotoxy(11, 10);
			cout << "개미가 고른 수는 " << com_sel << "입니다! 축하합니다~" << endl;
			player.setHouseSize(rand() % 7 + 2);
			system("cls");
			DrawGamePass();
			return 0;
		}
		if (cnt == 5) {
			system("cls");
			gotoxy(10, 10);
			cout << "개미가 고른 수는 " << com_sel << "입니다. 집을 지을 수 없어요ㅠㅠ";
			Sleep(1500);
			system("cls");
			DrawGameOver();
			player.setHouseSize(-rand() % 5 + 1);
			return 0;
		}

		if (com_sel > user_sel) {
			gotoxy(11, 10);
			cout << "개미는 더 큰 수를 골랐습니다!";
			gotoxy(11, 11);
			cout << "남은 기회는 " << 5 - cnt << "번 입니다!";
		}
		else if (com_sel < user_sel) {
			gotoxy(11, 10);
			cout << "개미는 더 작은 수를 골랐습니다!";
			gotoxy(11, 11);
			cout << "남은 기회는 " << 5 - cnt << "번 입니다!";
		}
		Sleep(1500);
		system("cls"); //화면 지우기
	}
}

//타이밍 맞추기 게임(보너스 게임) - 무조건 집이 커짐
int timingGame() {
	timeBeginPeriod(1); //timer interrupt 해상도를 1로 맞춤

	string pointList[4] = { 
		{"아쉬워요"},
		{"조금만 더ㅠㅠ"},
		{"좋아요"},
		{"잘했어요!"}
	};
	int g_timing[] = { 3, 7, 10, 12, 15,19, 21, 22, 25 }; // 타이밍

	string userPoint[9];

	double begin; // 처음 시작시 시간
	double end; // 프로그램 실행 후 반복문안에서 체크할 시간
	double checkC; // begin - end 값
	int tIndex = 0; // 스테이지 체크
	begin = timeGetTime();

	int userP = 0;
	cout << fixed; // 출력 소수점 자리수 고정
	cout.precision(3); // 소수점 밑 3자리까지 출력

	while (true) {
		end = timeGetTime();
		checkC = (end - begin) / 1000; // 경과시간 구하기
		gotoxy(22, 3);
		cout << "타이밍 게임";
		gotoxy(21, 5);
		cout << "타이머 : " << checkC;


		if (_kbhit()) {
			// fabs는 double의 절대값 구하는 함수입니다.
			if (fabs((double)g_timing[tIndex] - checkC) >= (double)1) { // 유저 입력 시간이 1초이상 차이날 경우 
				userP += rand() % 2;
				userPoint[tIndex] = pointList[0];
			}
			// 0.75이하로 차이나고 0.5초 초과로 차이날경우
			else if (fabs((double)g_timing[tIndex] - checkC) <= (double)0.75 && fabs((double)g_timing[tIndex] - checkC) > (double)0.5) {
				userP += rand() % 3 + 1;
				userPoint[tIndex] = pointList[1];
			}
			// 0.5이하로 차이나고 0.25초 초과로 차이날경우
			else if (fabs((double)g_timing[tIndex] - checkC) <= (double)0.5 && fabs((double)g_timing[tIndex] - checkC) > (double)0.25) {
				userP += rand() % 4 + 2;
				userPoint[tIndex] = pointList[2];
			}
			// 0.25 이하로 차이날 경우
			else if (fabs((double)g_timing[tIndex] - checkC) <= (double)0.25) {
				userP += rand() % 5 + 3;
				userPoint[tIndex] = pointList[3];
			}

			_getch(); // 버퍼 비우기
			tIndex++; // 스테이지 상승
		}

		// 유저가 입력하지 않았을 경우 자동으로 Bad값을 저장
		if (checkC > 6 && tIndex == 0) {
			userPoint[tIndex] = pointList[0];
			tIndex++;
		}
		else if (checkC > 11 && tIndex == 1) {
			userPoint[tIndex] = pointList[0];
			tIndex++;
		}
		else if (checkC > 15 && tIndex == 2) {
			userPoint[tIndex] = pointList[0];
			tIndex++;
		}
		else if (checkC > 18 && tIndex == 3) {
			userPoint[tIndex] = pointList[0];
			tIndex++;
		}
		else if (checkC > 21 && tIndex == 4) {
			userPoint[tIndex] = pointList[0];
			tIndex++;
		}
		else if (checkC > (double)26 && tIndex == 5) {
			userPoint[tIndex] = pointList[0];
			tIndex++;
		}
		else if (checkC > (double)30 && tIndex == 6) {
			userPoint[tIndex] = pointList[0];
			tIndex++;
		}
		else if (checkC > (double)35 && tIndex == 7) {
			userPoint[tIndex] = pointList[0];
			tIndex++;
		}
		else if (checkC > (double)38 && tIndex == 8) {
			userPoint[tIndex] = pointList[0];
			tIndex++;
			break;
		}
		int y = 10;
		for (int i = 0; i < 9; i++) { // 현재 스테이지와 유저의 점수를 출력
			gotoxy(20, y++);
			cout << g_timing[i] << " Sec : " << userPoint[i];
		}

		if (tIndex == 9) // 스테이지가 8이 지났을 경우 종료
			break;
		system("cls"); // 콘솔 지우기
	}
	timeEndPeriod(1); // timer interrupt 초기화
	gotoxy(20, 20);
	cout <<"개미집이 " << userP << "만큼 커졌어요!";
	player.setHouseSize(userP);
	system("pause>null");
	return 0;
}

//게임 정보 뷰
void InfoGame() {
	DrawFirstInfoGame();
	DrawSecondeInfoGame();
	system("pause>null");
}

//스레드 실행하는 부분
int threadStart() {
 	system("cls");
	a1.drawAntHouse(player.getHouseSize());
	thread moveInHouse(&Ant::moveInHouse, a1);
	f1.ranFeed();
	if (!eatFeed1) startMiniGame();
	
	return 0;
}

//집 그리기 또는 미니게임 시작
int startGame() { 
	system("cls");
	if (player.getLoginCheck()) {
		startMiniGame();
	}
	else {
		system("cls");
		guestMenu();
	}
	return 0;
}


//아이디 중복 체크
bool checkSameAccount() {
	for (int i = 0; i < cntAcc; i++) {
		if (user[i]->getUserAcc() == user[i+1]->getUserAcc()) {
			return false;
		}
	}
	return true;
}

//계정 생성
int CreateAccount() {
	system("cls");
	string acc;
	string pw;
	string idAnswer;
	int pwAnswer;
	string name;

	while (true) {
		gotoxy(12, 8);
		cout << "생성할 계정의 계정명 입력(10자 이내) : ";
		cin >> acc;
		if (acc.length() > 10) {
			system("cls");
			DrawRetryId();
		}
		else {
			break;
		}
		system("cls");
	}

	gotoxy(12, 10);
	cout << "생성할 계정의 비밀번호 입력 : ";
	cin >> pw;
	
	gotoxy(12, 12);
	cout << "닉네임 입력 : ";
	cin >> name;
	
	gotoxy(12, 14);
	cout << "계정을 잃어버렸을 경우를 대비해, 질문에 대답해주세요. ";
	gotoxy(12, 15);
	cout << "가장 좋아하는 전공은? : ";
	cin >> idAnswer;

	while (true) {
		gotoxy(12, 8);
		cout << "생성할 계정의 계정명 입력(10자 이내) : "<<acc;
		gotoxy(12, 10);
		cout << "생성할 계정의 비밀번호 입력 : "<<pw;
		gotoxy(12, 12);
		cout << "닉네임 입력 : "<<name;
		gotoxy(12, 14);
		cout << "계정을 잃어버렸을 경우를 대비해, 질문에 대답해주세요. ";
		gotoxy(12, 15);
		cout << "가장 좋아하는 전공은? : "<<idAnswer;

		gotoxy(12, 17);
		cout << "태어날 달은? (숫자) : ";
		cin >> pwAnswer;
		if (pwAnswer > 0 && pwAnswer <= 12) {
			break;
		}
		else {
			system("cls");
			DrawRetryPwAnswer();
		}
		system("cls");
	}

	if (checkCntAcc()) {
		setFileData(acc, pw, name, idAnswer, pwAnswer,10);
		system("cls");
		gotoxy(18, 12);
		cout << "계정이 생성되었습니다.";
		gotoxy(17, 17);
		cout << plz_key;
		system("pause>null");
	}
	else {
		system("cls");
		gotoxy(12, 12);
		cout << "현재 생성된 계정이 " << cntAcc << "개 이므로 생성할 수 없습니다.";
		gotoxy(11, 13);
		cout << "계정을 잃어버리셨다면 계정 찾기를 이용해주시기 바랍니다.";
		gotoxy(17, 17);
		cout << plz_key;
		system("pause>null");
	}
	return 0;
}

//생성된 계정 모두 보여주기
int allAccount() {
	getFileData();

	system("cls");
	if (cntAcc == 0) {
		cout <<"계정 없음";
		gotoxy(17, 18);
		cout << plz_key;
		system("pause>null");
		return 0;
	}
	for (int i = 0; i < cntAcc; i++) {
		cout << user[i]->getUserAcc() << endl;
	}
	return 0;
}

//생성한 계정 확인, 로그인하기
void LoginAccount() {
	system("cls");
	string acc;
	string pw;
	gotoxy(15, 8);
	cout << "계정 입력 : ";
	cin >> acc;
	gotoxy(15, 10);
	cout << "비밀번호 입력 : ";
	cin >> pw;

	checkUser(acc, pw);
}

//계정 찾는 거 질문
int QuestionAccount() { 
	//아이디 찾기, 비밀번호 찾기, 로그인하기, 회원가입하기
	int y = 0; //커서의 y 위치
	int x = 0;
	int input = 0; //키보드 입력을 받을 변수
	while (true) { //게임 루프
		DrawFIndAcc(); //준비화면 그리기

		if (y <= 0) { //커서가 위로 그만 올라가게
			y = 0;
		}
		else if (y >= 2) { //커서가 아래로 그만 내려가게
			y = 2;
		}
		if (x <= 0) { //커서가 왼쪽으로 그만가게
			x = 0;
		}
		else if (x >= 12) { //커서가 오른쪽으로 그만가게
			x = 12;
		}
		gotoxy(14 + x, 13 + y); //위치조정
		cout << ">";

		input = _getch();
		//→←↑↓ 방향키를 누를 경우
		if (input == MAGIC_KEY) { //224가 들어옴
			switch (_getch()) //한번 더 받음
			{
			case UP: //위
				y -= 2;
				break;
			case DOWN: //아래
				y += 2;
				break;
			case LEFT:
				x -= 12;
				break;
			case RIGHT:
				x += 12;
				break;
			}
		}
		//메인 메뉴 고름
		else if (input == SPACE || input == ENTER) { //키보드가 스페이스일 경우
			switch (y + x) { //y값과 x값을 더한 값에 따라 판단
			case 0:
				FindId();
				break;
			case 2:
				LoginAccount();
				break;
			case 12:
				FindPw();
				break;
			case 14:
				return 0;
			}
		}
	}
}

//로딩바
int progressBar() {
	int x=11, y=15;
	
	for (int i = 0; i <= 10; i++) {
		system("cls");
		gotoxy(15, 10);
		cout << "저장되어 있는 파일을 불러오는 중입니다.";
		gotoxy(12, y + 1);
		cout << "=============================================== "<<i*10<<"%";
		gotoxy(x+=2, y);
		cout << "***";
		Sleep(450);          //0.5초 대기
	}
	gotoxy(17, 20);
	cout << plz_key;
	system("pause>null");
	return 0;
}

//메인 루프
int main() {
	SetConsoleVIew_main();
	//시작 전 저장된 데이터가 있으면 가져와서 저장
	getFileData();
	progressBar();
	PlaySound("ant's_day", 0, SND_FILENAME | SND_ASYNC | SND_LOOP); //루프 재생
	srand((unsigned int)time(NULL));
	
	mainMenu();

	return 0;
}
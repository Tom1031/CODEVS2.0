#pragma once
#include"game.h"
#include"util.h"
#include<list>
#include<fstream>
#include<iostream>
#include<string>
#include<time.h>
using namespace std;

class Player{
	Game game;//�Q�[���G���W���͏�Ɏg���̂Ŏ����Ă���
	State best_state;//��Ԃ悢���ʂ̏�Ԃ�ۑ����Ă���
	long long best_score;//��Ԃ悢�X�R�A��ۑ����Ă���
	int limit_sum_chain_num;
	int num_good_state;
	int limit_depth;//�{���̈Ӗ��ł̖؂̐[�������@����Ȃ��Ɓ@�ꐶ�v�Z���Ă��܂�
	void insert_state(list<State> &list_state,State state);
	long long int search_tree(State state,list<State> &list_good_state,int sum_num_chain,const int depth,time_t start_time);
	ifstream halt_message;
	time_t start_time;
	int limit_height;
	int turn_updated;
	int start_turn;
	int debug;
	bool is_rensa_now;
	list<State> search_target;
	State save_point;
	void push_garbage();
	void random_action();

public:
	Player(int W,int H,int T,int S,int N,int P):num_good_state(3),limit_sum_chain_num(2),best_state(State()),best_score(0),game(Game(W,H,T,S,N,P)),limit_depth(2),start_time(clock()){
		debug=0;
		is_rensa_now=false;
		save_point=State();
		start_turn=0;
		limit_depth=2;
	};
	void output();
	State calc();
};
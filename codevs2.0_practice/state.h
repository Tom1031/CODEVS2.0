#include<utility>
#include<iostream>
#include"util.h"
using namespace std;
#pragma once

class State{
	friend class Game;
	long long int value;//���܂œ����邱�Ƃ��m�肵���_��
	int depth;//���ɒT������[���@����̓`�F�C���̑���
	pair<int,int> order[1000];//���܂ł����Ȃ��Ă������ߗ� (x,rotation)
	int step_idx;//�����ڂ̃p�b�N�𗎂Ƃ��̂��@0origin
public:
	int field[MAX_H][MAX_W];//�t�B�[���h��\���@����0�ƂȂ�悤�ɕύX
	State():step_idx(0),value(0),depth(2){//�z���g�ɍŏ��̏�Ԃ����Ƃ�
		memset(field,0,sizeof(field));
	}
	//��Ԃ�S�Ďw�肵�ď���������
	long long int get_value()const{return value;}
	void set_value(const long long int max_value){
		value=max_value;
	}
	int get_step_idx()const{return step_idx;}
	int get_depth()const{return depth;}
	void increment_depth(){depth++;}
	void reset_depth(){depth=2;}
	const pair<int,int>* get_order()const{return order;}
	int get_height(){
		int res=0;
		for(int x=0;x<20;x++){
			for(int y=res;y<36;y++){
				if(field[y][x]==0){
					res=y;
					break;
				}
			}
		}
		return res;
	}
};

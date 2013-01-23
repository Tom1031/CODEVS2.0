#include"game.h"
#include<string>
#include<iostream>
#include<cassert>
using namespace std;


bool Game::is_fatal_attacked(){//��΂��U��
	int num=0;
	for(int y=T-1;y>=0;y--){
		for(int x=0;x<T;x++){
			if(packs[turn][0][y][x]>S){
				num++;
			}
		}
	}
	if(num>=10){
		return true;
	}
	return false;
}


void Game::input_stock_garbage(){
	cin>>my_stock_garbage>>enemy_stock_garbage;
	return;
}

int Game::update_enemy_state(){
	int x,r;
	cin>>x>>r;
	return update(state_enemy,x,r,H).second;
}

bool Game::input_pack_changed(){
	bool is_changed=false;
	for(int y=T-1;y>=0;y--){
		for(int x=0;x<T;x++){
			int num;
			cin>>num;
			if(packs[turn][0][y][x]!=num){
				packs[turn][0][y][x]=num;
				is_changed=true;
			}
		}
	}
	string temp;
	cin>>temp;//END�ǂݔ�΂�

	for(int y=T-1;y>=0;y--){
		for(int x=0;x<T;x++){
			int num;
			cin>>num;
			packs_enemy[turn][0][y][x]=num;
		}
	}
	cin>>temp;//END�ǂݔ�΂�

	for(int r=1;r<=3;r++){//��]�����
		for(int y=0;y<T;y++){
			for(int x=0;x<T;x++){
				int nx,ny;
				if(r==1){
					nx=y;
					ny=(T-1)-x;
				}
				else if(r==2){
					nx=(T-1)-x;
					ny=(T-1)-y;
				}
				else{
					nx=(T-1)-y;
					ny=x;

				}
				packs[turn][r][ny][nx]=packs[turn][0][y][x];
				packs_enemy[turn][r][ny][nx]=packs_enemy[turn][0][y][x];
			}
		}
	}
	return is_changed;
}


//state��������āC���Ƃ����W�Ɖ�]���w�肳�ꂽ��C��Ԃ��X�V���āC����ꂽ���_��Ԃ�
pair<int,long long int> Game::update(State &state,const int x,const int r,const int h)const{
	pair<int,long long > res(0,0);
	if(!check_inside(state,x,r)){//�͂ݏo����
		return pair<int,long long >(-1,-1);
	}
	push_pack(state,x,r);
	state.order[state.step_idx]=pair<int,int>(x,r);
	state.step_idx++;//������state�̎��̏�Ԃ��X�V����
	int change_info[4][2][MAX_W+MAX_H+MAX_T];//�O�Ə�Ԃ��ω��������W�̍ő�l�ƍŏ��l���������Ă����i�ω����Ȃ����-1�j
	bool is_drop[MAX_W];
	memset(is_drop,false,sizeof(is_drop));
	for(int drop_x=max(0,x);drop_x<min(W,x+T);drop_x++){
		is_drop[drop_x]=true;
	}

	int chain=0;
	int total=0;
	while(true){
		drop_block(state,change_info,is_drop);
		int num_erased_block=erase(state,change_info,is_drop);
		if(num_erased_block==0) break;
		chain++;
		total+=calc_garbage_block_point(chain,num_erased_block);
//		res.first=chain;
	}
	res.first=chain;
	res.second=total;
	if(!check_over(state,h)){
		return pair<int,long long>(-1,-1);
	}
	return res;
}


void Game::output(const State &state){
	cout<<state.order[turn].first<<" "<<state.order[turn].second<<endl;
	update(now_state,state.order[turn].first,state.order[turn].second,H);
	turn++;
}

void Game::output(int x,int r){
	cout<<x<<" "<<r<<endl;
	update(now_state,x,r,H);
	turn++;
}

bool Game::check_inside(const State &state,const int &left_x,const int &r)const{
	for(int k=0;k<T;k++){
		int x=left_x+k;
		if(x<0 || x>=W){
			for(int y=0;y<T;y++){
				if(packs[state.step_idx][r][y][k]>=1) return false;
			}
		}
	}
	return true;
}

bool Game::check_over(const State &state,const int h)const{
	for(int x=0;x<W;x++){
		if(state.field[min(h,H)][x]!=0) return false;
	}
	return true;
}

//�\�I�Ŏg������@�����g���Ă��Ȃ�
inline long long int Game::calc_point(int C,int turn,int E)const{
	int n=turn/100;
	return ((long long)1<<(min(C,P+n)-1))*max(1,(C-(P+n)+1))*E;
}

//�p�b�N�������Ə�̕��ɔz�u����
void Game::push_pack(State &state,const int &left_x,const int &r)const{
	for(int x_pack=0;x_pack<T;x_pack++){
		int x=left_x+x_pack;
		if(x<0 || x>=(int)W) continue;
		for(int y=0;y<T;y++){
			state.field[H+1+y][x]=packs[state.step_idx][r][y][x_pack];//�t�B�[���h�̏��ɔz�u
		}
	}
}

void Game::drop_block(State &state,int change_info[][2][MAX_W+MAX_H+MAX_T],bool is_drop[MAX_W])const{//�u���b�N��S�ĉ��܂ŗ��Ƃ�
	for(int i=0;i<4;i++)//������
		memset(change_info[i],-1,sizeof(change_info[i]));

	for(int x=0;x<W;x++){
		if(!is_drop[x]){//�����͗��Ƃ��Ȃ��Ă��ǂ�
			continue;
		}
		bool is_continue=true;
		int y=0;
		int y_first_block=0;
		while(is_continue){
			is_continue=false;
			int y_first_empty=H+1+MAX_T;
			//��̃u���b�N������
			for(;y<H+MAX_T+1;y++){
				if(state.field[y][x]==0){//�󂾂��I�I
					y_first_empty=y;
					break;
				}
			}
			if(y_first_empty==H+1+MAX_T) break;//�󂪂Ȃ�����
			y_first_block=max(y,y_first_block+1);
			//�����ɂ���ŏ��̃u���b�N��������
			for(y_first_block=y_first_empty+1;y_first_block<H+1+MAX_T;y_first_block++){
				if(state.field[y_first_block][x]>0){
					is_continue=true;//��������
					break;
				}
			}
			if(!is_continue){
				break;
			}
			swap(state.field[y_first_empty][x],state.field[y_first_block][x]);//��������
			y=y_first_empty+1;

			//�������獂�����̂��߂�ZONE�@�߂�ǂ���I�o�債�Ăˁ@����Ă邱�Ƃ͋�̂Ƃ���ɗ����Ă����Ƃ���ꏊ���������Ă�

			if(change_info[0][0][x]<0){//�c�̍ŏ��l�ɂ���
				change_info[0][0][x]=y_first_empty;
			}else{
				change_info[0][0][x]=min((int)y_first_empty,change_info[0][0][x]);
			}
			if(change_info[0][1][x]<0){//�c�̍ő�l�ɂ���
				change_info[0][1][x]=y_first_empty;
			}else{
				change_info[0][1][x]=max((int)y_first_empty,change_info[0][1][x]);
			}

			//�c�I���@���͉�
			if(change_info[1][0][y_first_empty]<0){//���̍ŏ��l�ɂ���
				change_info[1][0][y_first_empty]=x;
			}else{
				change_info[1][0][y_first_empty]=min((int)x,change_info[1][0][y_first_empty]);
			}

			if(change_info[1][1][y_first_empty]<0){//���̍ő�l�ɂ���
				change_info[1][1][y_first_empty]=x;
			}else{
				change_info[1][1][y_first_empty]=max((int)x,change_info[1][1][y_first_empty]);
			}

			//���͉E�������ɂ���
			int line_idx=y_first_empty+x;//�΂߂̃��C���̔ԍ��@���͂��Ɛ��������̂Ő}�ɏ����Ă������猩�Ă�����
			int pos_idx=x;//���C����̍��W��\��
			if(y_first_empty+x>H+T-1){//���C������̕ӂ���o�Ă�ꍇ
				pos_idx=(H+T-1)-y_first_empty;
			}
			assert(pos_idx>=0);

			if(change_info[2][0][line_idx]<0){//���̍ŏ��l�ɂ���
				change_info[2][0][line_idx]=pos_idx;
			}else{
				change_info[2][0][line_idx]=min(pos_idx,change_info[2][0][line_idx]);
			}

			if(change_info[2][1][line_idx]<0){//���̍ő�l�ɂ���
				change_info[2][1][line_idx]=pos_idx;
			}else{
				change_info[2][1][line_idx]=max(pos_idx,change_info[2][1][line_idx]);
			}

			//�Ō�͍��������ɂ���
			line_idx=y_first_empty+((W-1)-x);//�΂߂̃��C���̔ԍ��@���͂��Ɛ��������̂Ő}�ɏ����Ă������猩�Ă�����
			pos_idx=(W-1)-x;//���C����̍��W��\��
			if(y_first_empty+(W-1)-x>H+T-1){//���C������̕ӂ���o�Ă�ꍇ
				pos_idx=(H+T-1)-y_first_empty;
			}
			assert(pos_idx>=0);

			if(change_info[3][0][line_idx]<0){//���̍ŏ��l�ɂ���
				change_info[3][0][line_idx]=pos_idx;
			}else{
				change_info[3][0][line_idx]=min(pos_idx,change_info[3][0][line_idx]);
			}

			if(change_info[3][1][line_idx]<0){//���̍ő�l�ɂ���
				change_info[3][1][line_idx]=pos_idx;
			}else{
				change_info[3][1][line_idx]=max(pos_idx,change_info[3][1][line_idx]);
			}
		}
	}
}

int Game::erase(State &state,int change_info[][2][MAX_W+MAX_H+MAX_T],bool is_drop[MAX_W])const{
	bool is_erased[MAX_H][MAX_W];//�����Ƃ�����o���Ă���
	memset(is_erased,false,sizeof(is_erased));
	memset(is_drop,false,sizeof(is_drop));

	//[�c�C���C�E���C����][min or max][���W] ����
	unsigned res=0;
	int change_pos[2][MAX_H];
	memset(change_pos,-1,sizeof(change_pos));
	res+=erase_vertically(state,is_erased,change_info[0],change_pos);
	res+=erase_horizontally(state,is_erased,change_info[1],change_pos);
	res+=erase_right_down(state,is_erased,change_info[2],change_pos);
	res+=erase_left_down(state,is_erased,change_info[3],change_pos);

	for(int y=0;y<H+MAX_T;y++){
		if(change_pos[0]<0) continue;
		for(int x=max(change_pos[0][y],0);x<=change_pos[1][y];x++){
			if(is_erased[y][x]){
				if(state.field[y][x]==S){//S�̃u���b�N�������Ă����ꍇ�@����������
					res-=3;
				}
				state.field[y][x]=0;
				is_drop[x]=true;
			}
		}
	}
	return res;
}

int Game::erase_vertically(State &state,bool is_erased[MAX_H][MAX_W],int change_info[2][MAX_W+MAX_H+MAX_T],int change_pos[2][MAX_H])const{
	int res=0;//������u���b�N�̌��𐔂���
	for(int x=0;x<W;x++){//�S�Ă�x���W�ɑ΂��ā@�c���`�F�b�N����
		int front,back=change_info[0][x];
		if(back<0) continue;//�����̗�ɕω��͂Ȃ�����
		int sum=state.field[back][x];//���ڂ��Ă�Ƃ��̍��v�l
		front=back;
		while(sum<S){
			if(front==0) break; //����ȏ�߂�Ȃ���
			front--;
			sum+=state.field[front][x];
		}

		while(front<H+MAX_T && back<H+MAX_T && front<change_info[1][x]){//front,back���͈͓��ɂ�����菈��������
			if(state.field[back][x]<=0 || state.field[back][x]>S){//�u���b�N���Ȃ�or���ז��u���b�N���Ɣ�΂�
				sum=0;//���v�l�̓��Z�b�g
				front=H+MAX_T;
				for(int y=back+1;y<H+MAX_T;y++){
					if(state.field[y][x]>0 && state.field[y][x]<=S){//�u���b�N����������front��back,sum���������ݒ�
						front=y;
						back=y;
						sum=state.field[y][x];
						break;
					}
				}
			}else{//�u���b�N������ꍇ
				if(sum>S){//�ڕW�l�����Ă��!
					sum-=state.field[front][x];//�퓬����J��グ�āCsum�����̕�����
					front++;
				}
				else if(sum==S){//�ڕW�l�W���X�g�I�@�������
					for(int y=front;y<=back;y++){
						is_erased[y][x]=true;//������Ƃ������������Ă���

						if(change_pos[0][y]<0){
							change_pos[0][y]=x;
						}
						else{
							change_pos[0][y]=min(change_pos[0][y],x);
						}
						if(change_pos[1][y]<0){
							change_pos[1][y]=x;
						}
						else{
							change_pos[1][y]=max(change_pos[1][y],x);
						}

					}
					res+=back-front+1;//������u���b�N�̌�������
					sum-=state.field[front][x];//front����i�߂āCsum�����炷
					front++;
				}
				else{//�ڕW�l�ȉ�
					back++;//�������������
					if(back<H+MAX_T) sum+=state.field[back][x];//sum�Ɉ���̕����ǉ�
				}
			}
		}
	}
	return res;
}

int Game::erase_horizontally(State &state,bool is_erased[MAX_H][MAX_W],int change_info[2][MAX_W+MAX_H+MAX_T],int change_pos[2][MAX_H])const{
	int res=0;//������u���b�N�̌��𐔂���

	for(int y=0;y<H+MAX_T;y++){//�S�Ă�y���W�ɑ΂��ā@�����`�F�b�N����		
		int front,back=change_info[0][y];
		if(back<0) continue;//�����̗�ɕω��͂Ȃ�����
		else{
			;
		}
		int sum=state.field[y][back];//���ڂ��Ă�Ƃ��̍��v�l
		front=back;
		while(sum<S){
			if(front==0) break; //����ȏ�߂�Ȃ���
			if(state.field[y][front-1]<=0 || state.field[y][front-1]>=S){//����ȏ㉺����Ƌ� or �ז��u���b�N
				break;
			}
			front--;
			sum+=state.field[y][front];
		}

		while(front<W && back<W && front<=change_info[1][y]){//front,back���͈͓��ɂ�����菈��������
			if(state.field[y][back]<=0 || state.field[y][back]>S){//�u���b�N���Ȃ�or���ז��u���b�N���Ɣ�΂�
				sum=0;//���v�l�̓��Z�b�g
				front=W;
				for(int x=back+1;x<W;x++){
					if(state.field[y][x]>0 && state.field[y][x]<=S){//�u���b�N����������front��back,sum���������ݒ�
						front=x;
						back=x;
						sum=state.field[y][x];
						break;
					}
				}
			}else{//�u���b�N������ꍇ
				if(sum>S){//�ڕW�l�����Ă��!
					sum-=state.field[y][front];//�퓬����J��グ�āCsum�����̕�����
					front++;
				}
				else if(sum==S){//�ڕW�l�W���X�g�I�@�������
					for(int x=front;x<=back;x++){
						is_erased[y][x]=true;//������Ƃ������������Ă���
						if(change_pos[0][y]<0){
							change_pos[0][y]=x;
						}
						else{
							change_pos[0][y]=min(change_pos[0][y],x);
						}
						if(change_pos[1][y]<0){
							change_pos[1][y]=x;
						}
						else{
							change_pos[1][y]=max(change_pos[1][y],x);
						}

					}
					res+=back-front+1;//������u���b�N�̌�������
					sum-=state.field[y][front];//front����i�߂āCsum�����炷
					front++;

				}
				else{//�ڕW�l�ȉ�
					back++;//�������������
					if(back<W) sum+=state.field[y][back];//sum�Ɉ���̕����ǉ�
				}
			}
		}
	}
	return res;
}

int Game::erase_right_down(State &state,bool is_erased[MAX_H][MAX_W],int change_info[2][MAX_W+MAX_H+MAX_T],int change_pos[2][MAX_H])const{
	int res=0;
	const int dx=1,dy=-1;//�i�ޕ����@�E����
	//line_idx�ɂ��Ă͂ǂ����ɐ}�������Ă����̂Ł@���Ă�����

	for(int line_idx=0;line_idx<H+W+T-2;line_idx++){
		int start_x,start_y;
		if(line_idx<=H+T-1){//�����̕ӂ���L�т郉�C��
			start_x=0;
			start_y=line_idx;
		}
		else{//��̕ӂ���L�т郉�C��
			start_x=line_idx-(H+T-1);
			start_y=H+T-1;
		}

		int front,back=change_info[0][line_idx];
		if(back<0) continue;//�����̗�ɕω��͂Ȃ�����
		int front_x,front_y,back_x,back_y;
		back_x=start_x+back*dx;
		back_y=start_y+back*dy;
		int sum=state.field[back_y][back_x];//���ڂ��Ă�Ƃ��̍��v�l
		front=back;
		front_x=back_x;
		front_y=back_y;
		while(sum<S){
			if(front_x == 0 || front_y>=H+T-1) break; //����ȏ㍶��ɍs���Ȃ�
			if(state.field[front_y-dy][front_x-dx]<=0 || state.field[front_y-dy][front_x-dx]>=S){//����ȏ㉺����Ƌ� or �ז��u���b�N
				break;
			}
			front--;
			front_x-=dx;
			front_y-=dy;
			sum+=state.field[front_y][front_x];
		}

		while( (front_x<W && front_y>=0) && (back_x<W && back_y>=0) && front<=change_info[1][line_idx] ){//front,back���͈͓��ɂ�����菈��������
			if(state.field[back_y][back_x]<=0 || state.field[back_y][back_x]>S){//�u���b�N���Ȃ�or���ז��u���b�N���Ɣ�΂�
				sum=0;//���v�l�̓��Z�b�g
				front_x=W;//�ԕ��I��
				for(int k=back+1; start_x+dx*k<W && start_y+dy*k>=0; k++){//���ォ�猩�Ă͂ݏo�����O�܂Ńu���b�N������Ƃ��댩�Ă���
					int now_x=start_x+dx*k,now_y=start_y+dy*k;
					if(state.field[now_y][now_x]>0 && state.field[now_y][now_x]<=S){
						front=k;
						back=k;
						front_x=start_x+dx*front;
						front_y=start_y+dy*front;
						back_x=front_x;
						back_y=front_y;
						sum=state.field[front_y][front_x];
						break;
					}
				}
				if(front_x==W) break;
			}else{//�u���b�N������ꍇ
				if(sum>S){//�ڕW�l�����Ă��!
					sum-=state.field[front_y][front_x];//�퓬����J��グ�āCsum�����̕�����
					front++;
					front_x+=dx;
					front_y+=dy;
				}
				else if(sum==S){//�ڕW�l�W���X�g�I�@�������
					for(int k=front;k<=back;k++){
						int now_x=start_x+dx*k,now_y=start_y+dy*k;
						is_erased[now_y][now_x]=true;//������Ƃ������������Ă���

						if(change_pos[0]<0){//������ꏊ�o����
							change_pos[0][now_y]=now_x;
						}
						else{
							change_pos[0][now_y]=min(change_pos[0][now_y],now_x);
						}
						if(change_pos[1]<0){
							change_pos[1][now_y]=now_x;
						}
						else{
							change_pos[1][now_y]=max(change_pos[1][now_y],now_x);
						}
					}
					res+=back-front+1;//������u���b�N�̌�������
					sum-=state.field[front_y][front_x];//front����i�߂āCsum�����炷
					front++;
					front_x+=dx;
					front_y+=dy;
				}
				else{//�ڕW�l�ȉ�
					back++;//�������������
					back_x+=dx;
					back_y+=dy;
					if(back_x<W && back_y>=0) sum+=state.field[back_y][back_x];//sum�Ɉ���̕����ǉ�
				}
			}
		}
	}
	return res;
}

int Game::erase_left_down(State &state,bool is_erased[MAX_H][MAX_W],int change_info[2][MAX_W+MAX_H+MAX_T],int change_pos[2][MAX_H])const{
	int res=0;
	const int dx=-1,dy=-1;//�i�ޕ����@������
	//line_idx�ɂ��Ă͂ǂ����ɐ}�������Ă����̂Ł@���Ă�����

	/*	for(int i=0;i<H+W-2;i++){
	cout<<i<<": "<<change_info[0][i]<<endl;
	}*/

	for(int line_idx=0;line_idx<H+W+T-2;line_idx++){
		int start_x,start_y;
		if(line_idx<=H+T-1){//�E���̕ӂ���L�т郉�C��
			start_x=W-1;
			start_y=line_idx;
		}
		else{//��̕ӂ���L�т郉�C��
			start_x=(W-1)-(line_idx-(H+T-1));//�E�ォ�獶�ɂ���
			start_y=H+T-1;
		}

		int front,back=change_info[0][line_idx];
		if(back<0) continue;//�����̗�ɕω��͂Ȃ�����
		int front_x,front_y,back_x,back_y;
		back_x=start_x+back*dx;
		back_y=start_y+back*dy;
		int sum=state.field[back_y][back_x];//���ڂ��Ă�Ƃ��̍��v�l
		front=back;
		front_x=back_x;
		front_y=back_y;
		while(sum<S){
			if(front_x == W-1 || front_y>=H+T-1) break; //����ȏ�E��ɍs���Ȃ�
			if(state.field[front_y-dy][front_x-dx]<=0 || state.field[front_y-dy][front_x-dx]>=S){//����ȏ㉺����Ƌ� or �ז��u���b�N
				break;
			}
			front--;
			front_x-=dx;
			front_y-=dy;
			sum+=state.field[front_y][front_x];
		}


		while( (front_x>=0 && front_y>=0) && (back_x>=0 && back_y>=0) && front<=change_info[1][line_idx] ){//front,back���͈͓��ɂ�����菈��������
			if(state.field[back_y][back_x]<=0 || state.field[back_y][back_x]>S){//�u���b�N���Ȃ�or���ז��u���b�N���Ɣ�΂�
				sum=0;//���v�l�̓��Z�b�g
				front_x=W;//�ԕ��I��
				for(int k=back+1; start_x+dx*k>=0 && (int)(start_y+dy*k)>=0; k++){//���ォ�猩�Ă͂ݏo�����O�܂Ńu���b�N������Ƃ��댩�Ă���
					int now_x=start_x+dx*k,now_y=start_y+dy*k;
					if(state.field[now_y][now_x]>0 && state.field[now_y][now_x]<=S){
						front=k;
						back=k;
						front_x=start_x+dx*front;
						front_y=start_y+dy*front;
						back_x=front_x;
						back_y=front_y;
						sum=state.field[front_y][front_x];
						break;
					}
				}
				if(front_x==W) break;
			}else{//�u���b�N������ꍇ
				if(sum>S){//�ڕW�l�����Ă��!
					sum-=state.field[front_y][front_x];//�퓬����J��グ�āCsum�����̕�����
					front++;
					front_x+=dx;
					front_y+=dy;
				}
				else if(sum==S){//�ڕW�l�W���X�g�I�@�������
					for(int k=front;k<=back;k++){
						int now_x=start_x+dx*k,now_y=start_y+dy*k;
						is_erased[now_y][now_x]=true;//������Ƃ������������Ă���
						if(change_pos[0]<0){//������ꏊ�o����
							change_pos[0][now_y]=now_x;
						}
						else{
							change_pos[0][now_y]=min(change_pos[0][now_y],now_x);
						}
						if(change_pos[1]<0){
							change_pos[1][now_y]=now_x;
						}
						else{
							change_pos[1][now_y]=max(change_pos[1][now_y],now_x);
						}

					}
					res+=back-front+1;//������u���b�N�̌�������
					sum-=state.field[front_y][front_x];//front����i�߂āCsum�����炷
					front++;
					front_x+=dx;
					front_y+=dy;
				}
				else{//�ڕW�l�ȉ�
					back++;//�������������
					back_x+=dx;
					back_y+=dy;
					if(back_x>=0 && back_y>=0) sum+=state.field[back_y][back_x];//sum�Ɉ���̕����ǉ�
				}
			}
		}

	}
	return res;
}

void Game::input_pack(){
	for(int i=0;i<N;i++){
		for(int y=T-1;y>=0;y--){
			for(int x=0;x<T;x++){
				int num;
				cin>>num;
				packs[i][0][y][x]=num;
				packs_enemy[i][0][y][x]=num;
			}
		}
		string temp;
		cin>>temp;//END�ǂݔ�΂�

		for(int r=1;r<=3;r++){//��]�����
			for(int y=0;y<T;y++){
				for(int x=0;x<T;x++){
					int nx,ny;
					if(r==1){
						nx=y;
						ny=(T-1)-x;
					}
					else if(r==2){
						nx=(T-1)-x;
						ny=(T-1)-y;
					}
					else{
						nx=(T-1)-y;
						ny=x;

					}
					packs[i][r][ny][nx]=packs[i][0][y][x];
					packs_enemy[i][r][ny][nx]=packs_enemy[i][0][y][x];
				}
			}
		}
	}
}
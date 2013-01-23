#include"Player.h"
#include<string>
#include<time.h>
#include<omp.h>
#include<vector>
#include<cassert>

//�����_���ɃQ�[���I�[�o�[�ɂȂ�Ȃ��s�����s��
void Player::random_action(){
	for(int x=-game.T;x<game.W+game.T;x++){
		for(int r=0;r<4;r++){
			State state=game.get_state();
			if(game.update(state,x,r,game.H).second>=0){
				game.output(x,r);
				search_target.clear();
				return;
			}
		}
	}
	search_target.clear();
	cout<<-100<<" "<<-100<<endl;//GAME OVER
}

//�E�ɃS�~���񂹂�s�����s��
void Player::push_garbage(){
	for(int x=game.W+game.T;x>=-game.T;x--){
		for(int r=0;r<4;r++){
			State state= game.get_state();
			int got_point=game.update(state,x,r,game.H).second;
			if(got_point>=0){
				game.output(x,r);
				search_target.clear();
				return;
			}
		}
	}
	cout<<-100<<" "<<-100<<endl;//GAME OVER
}

//�o�͂�f���o��
void Player::output(){
	bool is_attacked=false;
	if(game.get_turn()>0){
		time_t bef=clock();
		is_attacked=game.input_pack_changed();
		game.update_enemy_state();
		game.input_stock_garbage();
		start_time+=clock()-bef;
	}

	//�c�莞�Ԃ����āC�ۊǂ��Ă�����Ԃ̌��𒲐�����
	if((start_time-clock())/CLOCKS_PER_SEC>=7000 && (start_time-clock())/CLOCKS_PER_SEC>game.get_turn()*10){ //�Ԃɍ��������ɂȂ��ł�
		num_good_state=1;
	}
	else if(num_good_state==1){//�Ԃɍ�������
		num_good_state=3;
	}
	bool is_fatal_attacked=false;

	//�U�����󂯂��I
	if(is_attacked){
		best_score=0;
		is_fatal_attacked=game.is_fatal_attacked();
		//�u���b�N��10�ȏ㗎���Ă���@�������U���������H
		if(is_fatal_attacked){
			save_point=State();//�Z�[�u�|�C���g��j������
			push_garbage();//�E�Ƀu���b�N���񂹂�
			return;
		}
	}

	if((!is_rensa_now && save_point.get_step_idx()<=game.get_turn()) || game.get_turn()==0 || is_attacked){//�A�����łȂ� or �Z�[�u�|�C���g�����łɑO�̃^�[��
		is_rensa_now=false;
		int before_best_score=best_score;
		best_score=0;//���Z�b�g
		save_point=calc();//����1��܂�����ȏ�Ԃ���T���Ă���@��Ԃ悢�s�����Z�[�u�|�C���g�Ƃ��ĕۑ�
		search_target.clear();
		if(before_best_score>=best_score){//�T�����Ă��A���̓_�����オ��Ȃ�������@�A���J�n�t���OON
			is_rensa_now=true;
		}
		else if(best_score<=0){//���Ȃ��@���S�H�@�ꉞ�����_���ȍs�������Ă݂�@
			random_action();
			return;
		}
	}

	/*
	�Z�[�u�|�C���g�͘A���̓r���̍s���񂪁C
	best_state�͘A��������������s���񂪕ۑ�����Ă���
	*/

	//search END
	//�ۑ�����Ă���s�����s��
	if(is_rensa_now){//�A�������@best_state�s�����s��
		save_point=State();
		best_score=0;
		game.output(best_state);//��Ԃ悳�����ȓz��output
		if(game.get_turn()>=best_state.get_step_idx()){
			is_rensa_now=false;//�A�������I
			start_turn=game.get_turn();
		}
	}else{//�r���ł� �Ƃ肠�����Z�[�u�|�C���g�̍s�����s��
		game.output(save_point);
	}
}


//�O�̃v���O���������育��������������R�[�h
State Player::calc(){
	limit_height=game.H;
	int iteration_count=0;
	turn_updated=0;
	best_score=0;
	State res=State();
	while(game.get_turn()<game.N){
		State now_state;
		if(!search_target.empty()){
			now_state=*search_target.begin();
		}else{
			now_state=game.get_state();
		}
		search_tree(now_state,search_target,0,0,clock());//�ŏ��ɐ󂭒T��������
		while(search_target.size()>0){
			time_t time_iteration_start=clock();
			iteration_count++;
			list<State> next_target;//���ɒT�������Ԃ�����
			for(list<State>::iterator target=search_target.begin();target!=search_target.end();target++){
				list<State> list_good_state;
				limit_sum_chain_num=target->get_depth();
				limit_depth=target->get_depth();
				long long value=search_tree(*target,list_good_state,0,0,clock());//���target����T�����J�n
				if(!list_good_state.empty()){//����������������
					int value_got=list_good_state.begin()->get_value();
					if(res.get_value()<value_got){
						res=*list_good_state.begin();
					}
				}else{//�������Ȃ����ƁE�E�E�H�@�[���𑝂₵�Ă݂邩�@�����i���@
					if(target->get_depth()<=2){//3���炢�܂ł͂���Ă݂邩
						target->increment_depth();
						insert_state(next_target,*target);
					}
				}
				int count=0;//�ɏ���������������̂Ł@������Ԃ���h���������̂���������Ȃ��@2���炢�ɂ��Ă���
				for(list<State>::iterator next_state=list_good_state.begin();count<2 && next_state!=list_good_state.end();next_state++){
					if(next_state->get_value()==target->get_value()){
						target->increment_depth();
						if(target->get_depth()<=3){//3���炢�܂ł͂���Ă݂邩
							insert_state(next_target,*target);
							target->reset_depth();
						}
					}
					else{
						next_state->reset_depth();
						insert_state(next_target,*next_state);
					}
					count++;
				}
			}
			if(next_target.empty() || next_target.begin()->get_step_idx()>game.get_turn()+5){//5�^�[���ȏ��̍s���͎ז�������m���������̂ŒT���I���I
				return res;//���̍s�����Ȃ��Ȃ��������ۂ��H
			}
			search_target=next_target;
		}
		return State();//������Ȃ������ꍇ����
	}
	return State();
}

//��Ԃ��^������̂ŁC�悳�����Ȃ炱������X�g�ɓ����C�����łȂ��Ȃ牽�����Ȃ��@�}���\�[�g���Ă�
void Player::insert_state(list<State> &list_state,State state){
	if(state.get_value()<=0) return;

	if(list_state.size()==0){//���X�g�ɂȂɂ������Ă��Ȃ���Ζ������ɓ����
		list_state.push_back(state);
		return;
	}

	if(state.get_value()<list_state.back().get_value()){//���X�g�̈�Ԍ������������E�E�E
		if(list_state.size()<num_good_state){//�܂��������������@���ɂ����
			list_state.push_back(state);
		}
		else{//�����ȏ�@�܂肢��Ȃ��q�ł��������Ƃ�����
			return;//�����������悤�Ȃ�
		}
	}

	//�K�v�Ȏq�ł��邱�Ƃ͂킩�����̂ŁC��납��ǂ��ɓ����ׂ����𒲂ׁC�}���i�}���\�[�g�j

	for (	list<State>::iterator it=list_state.begin(); it!=list_state.end();it++){
		if(it->get_value()==state.get_value()){//�_���������������瓯�����ǂ����̔�����s��
			if(it->get_value()==state.get_value()){//������Ԃ��ۂ��̂ŃX���[
				break;
			}

		}
		else if(it->get_value()<state.get_value()){//���l�����������̂��̂𔭌�������}��
			list_state.insert(it,state);//�����̒��O�ɑ}������ƃ\�[�g���ꂽ�܂�
			break;
		}
	}
	if(list_state.size()>num_good_state)//�����ȏ�̌������X�g�ɓ����Ă���
		list_state.pop_back();//��Ԍ��͂������悤�Ȃ�
	return;
}


/*�Q�[���؂�������@�����������G
sum_num_chain�͍��܂ŋN�������`�F�C���̉񐔂��J�E���g����
�߂�l�͎��̈��œ�����_���̍ő�l�@���ꂪ�傫���قǁC�悢��ԂƂ�����
*/
long long int Player::search_tree(State state,list<State> &list_good_state,int sum_num_chain,int depth,const time_t time){
	if((double)(clock()-time)/CLOCKS_PER_SEC>10.0) return 0;//���Ԃ���������
	if(state.get_step_idx()+game.get_turn()==game.N) return 1;//�Q�[�����I�������̂ŏI��� ��~�����S�z�Ȃ̂�1��Ԃ���	
	if(depth>=3) return 0;//�[��4�ȏ�͎��Ԃ����肷����̂Œ��߂�

	if(limit_depth<depth) return 0;
	long long int res=0;//����1��œ���ꂽ�_���̍ő�_��
	long long int max_value=0;//down_tree�̖߂�l�̍ő�l�i���ꂪ�傫����΁C����state�͗D�G�ł���j

	//�S�Ă�x���W,�����ɂ��Ď��� ��������������Ȃ���������Ȃ�
	for(int x=-game.T+1;x<game.W-5;x++){
		for(int r=0;r<4;r++){
			State copy_state=state;
			//��Ԃ��X�V���Ă݂�
			pair<int,long long>got=game.update(copy_state,x,r,limit_height);
			long long int got_point=got.second;
			int got_chain=got.first;
			if(got_point<0){//GAMEOVER�ɂȂ����̂ł��ǂ�
				continue;
			}
			//�`�F�C���̍��v�����X�R�A�����X�V
			sum_num_chain+=got_chain;
			res=max(res,got_point);

			if(got_point/(state.get_step_idx()-start_turn+1)>best_score){//����ꂽ�_�������݂̍ō��_�𒴂�����
				best_score=got_point/(state.get_step_idx()-start_turn+1);//�ō��_���X�V
				best_state=copy_state;//��Ԃ悢��Ԃ��X�V
				turn_updated=state.get_step_idx();
			}
			//�}����̏����ɓ��Ă͂܂�Ȃ���΂�����
			if(sum_num_chain<limit_sum_chain_num || depth<=2){//�`�F�C������萔���������玟��������@��A�����N����\��������
				max_value=max(max_value,search_tree(copy_state,list_good_state,sum_num_chain,depth+1,time));
			}
			sum_num_chain-=got_chain;
		}
	}
	state.set_value(max_value);//����state�͂��̓_���𓾂邱�Ƃ��ۏ؂���Ă���
	if(depth>=1){
		insert_state(list_good_state,state);//���Đ[���T������ɒl����state�Ȃ́H
	}
	return res;
}


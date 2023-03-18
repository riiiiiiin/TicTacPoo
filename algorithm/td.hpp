#pragma once

#include <unordered_map>
#include "../problem/tictactoe.hpp"
#include "../utils/random_variables.hpp"

//为什么要这么做？原本的class功能不太够用，虽然最后砍下来也差不多了

//将轮到我方落子的状态包装为节点
class Td_Node{
private:
    TicTacToeState _node;
    bool _done;  
public:
    //构造子节点
    Td_Node(TicTacToeState const& state) : _node(state) , _done(false) {}
    ~Td_Node() {}
    friend class TD_Learning;//出现了奇妙的类型兼容err，还是friend省事（
};
//值函数表
std::unordered_map<uint32_t,double> values;

//学习类
class TD_Learning{
private:  
    //可达状态表
    std::unordered_map<uint32_t,Td_Node*> _all_nodes;
    Td_Node _root_node;
    double _alpha , _epsilon0;
    int _iterations;
    //第一步：拓展所有可能情况
    void _expand(Td_Node* current_node){
        //进入X方回合
        TicTacToeState oppo_state=current_node->_node.next(current_node->_node.action_space()[0]);
        /////////////////////////////////////////////////
        ///                for debug                  ///
        /////////////////////////////////////////////////
        //std::cout<<std::endl;//
        //state.show();//
        //for(int i=0;i<state.n_actions();++i){//
            //std::cout<<state.action_space()[i]<<' ';//
        //}//
        //std::cout<<std::endl;//
        /////////////////////////////////////////////////


        //在黑方动作终局：更新白方node
        if(oppo_state.done()){
            current_node->_done = true;
            ////////////////////////////////////////////
            ///              for debug               ///
            ////////////////////////////////////////////
            //std::cout<<"X_ends"<<std::endl;//
            //current_node->_node.show();//
            //std::cout<<std::endl;//
            ////////////////////////////////////////////
            values[current_node->_node.state()] = oppo_state.rewards()[1];
            return;
        }
        //进入白方回合：已拜访状态，未拜访状态（终局，未终局）
        for(int i=0;i<oppo_state.action_space().size();++i){
            TicTacToeState new_state = oppo_state.next(oppo_state.action_space()[i]);
            auto iter = values.find(new_state.state());
            //未拜访状态
            if(iter == values.end()){
                Td_Node* new_node = new Td_Node(new_state);
                _all_nodes[new_state.state()] = new_node;
                //终局
                if(new_state.done()){
                    ////////////////////////////////////
                    //std::cout<<"ends"<<std::endl;//
                    //new_state.show();
                    //std::cout<<std::endl;//
                    ////////////////////////////////////

                    new_node->_done = true;
                    values[new_state.state()] = new_state.rewards()[1];
                }
                //未终局
                else{
                    values[new_state.state()] = 0;
                    _expand(new_node);
                }
            }
        }
        return;
    }
    //第二步：epsilon-greedy采样
    const uint32_t _epsilon_greedy(double epsilon,Td_Node* cur_node_){
        //先模拟对方下一步
        TicTacToeState oppo_state = cur_node_->_node.next(cur_node_->_node.action_space()[0]);
        if(RandomVariables::uniform_real()>epsilon){
            //epsilon-greedy，以1-epsilon概率选择最优
            double max_val_{-3};
            uint32_t ret;
            for(int i = 0;i < oppo_state.n_actions();++i){
                TicTacToeState candidate_state = oppo_state.next(oppo_state.action_space()[i]);
                double cur_val_= values[candidate_state.state()];
                if(cur_val_ >= max_val_){
                    max_val_ = cur_val_;
                    ret = candidate_state.state();
                }
            }
            return ret;
        }
        else{
            //以epsilon概率随机选择
            TicTacToeState candidate_state = oppo_state.next(oppo_state.action_space()[RandomVariables::uniform_int()%oppo_state.n_actions()]);            
            return candidate_state.state();
        }
    }
    //第三步：递归维护值函数表
    void _update_values(double alpha,double epsilon,Td_Node* cur_node){
        //出口条件：走到终局节点
        if(cur_node->_done){
            return;
        }
        //未终局：epsilon-greedy选择下一个节点
        else{
            uint32_t eps_selection = _epsilon_greedy(epsilon,cur_node);
            auto iter = _all_nodes.find(eps_selection);
            assert(iter !=_all_nodes.end());
            Td_Node* selected_node = _all_nodes[eps_selection];           
            _update_values(alpha,epsilon,selected_node);
            values[cur_node->_node.state()] = values[cur_node->_node.state()]
                                            + alpha * (values[selected_node->_node.state()] - values[cur_node->_node.state()]);
            return;
        }
    }
public:
    TD_Learning(double alpha,double epsilon0,int iterations):_root_node(TicTacToeState()) , _alpha(alpha) ,
                                                            _epsilon0(epsilon0) , _iterations(iterations)
                                                            {
                                                                values[_root_node._node.state()]=0;
                                                            }
    ~TD_Learning(){
        for(std::unordered_map<uint32_t,Td_Node*>::iterator it = _all_nodes.begin();it != _all_nodes.end(); ++it){
            delete it->second;
        }
        _all_nodes.clear();
        values.clear();
    }
    //调用拓展函数
    void expand(){
        _expand(&_root_node);
    }
    //对战iteration次，以_alpha作为学习率，递减的epsilon作为epsilon-greedy参量
    void update_values(){
        for(int i = 0 ;i < _iterations ; ++i){           
           _update_values(_alpha,_epsilon0*(1-i/_iterations),&_root_node); 
        }       
        return;
    }
    //for debug:展示kv对
    void show_values(){
        for(std::unordered_map<uint32_t,double>::iterator it=values.begin();it!=values.end();++it){
            std::cout<<it->first<<":"<<it->second<<std::endl;
        }
    }
};
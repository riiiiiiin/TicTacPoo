#pragma once

#include <unordered_map>
#include "../problem/tictactoe.hpp"
#include "../utils/random_variables.hpp"

//将轮到我方落子的状态包装为节点
class Td_Node{
private:
    TicTacToeState _node;
    bool _done;  
public:
    //构造子节点
    Td_Node(TicTacToeState const& state):_node(state),_done(false){}
    ~Td_Node(){}
    friend struct std::hash<Td_Node>;
    friend class Td_Learning;
};

template<>
struct std::hash<Td_Node>{
    size_t operator()(Td_Node& n) const {
        return n._node.state();
    }
};

class Td_Learning{
private:
    std::unordered_map<uint32_t,double> _values;
    std::unordered_map<uint32_t,Td_Node*> _all_nodes;
    Td_Node _root_node;
    double _alpha,_epsilon0;
    int _iterations;
    //第一步：拓展所有可能情况
    void _expand(Td_Node* current_node){
        //保险起见：该节点已终局
        if(current_node->_node.done()){
            current_node->_done=true;
            _values[current_node->_node.state()]=current_node->_node.rewards()[1];
            return;
        }
        TicTacToeState state=current_node->_node;
        /////////////////////////////////////////////////
        //std::cout<<std::endl;//
        //state.show();//
        //for(int i=0;i<state.n_actions();++i){//
            //std::cout<<state.action_space()[i]<<' ';//
        //}//
        //std::cout<<std::endl;//
        /////////////////////////////////////////////////
        state=state.next(state.action_space()[0]);
        ////////////////////////////////////////////////
        //state.show();//
        //std::cout<<std::endl;//
        ////////////////////////////////////////////////
        //在黑方动作终局：更新白方node
        if(state.done()){
            current_node->_done=true;
            ////////////////////////////////////////////
            //std::cout<<"X_ends"<<std::endl;//
            //current_node->_node.show();//
            //std::cout<<std::endl;//
            ////////////////////////////////////////////
            _values[current_node->_node.state()]=state.rewards()[1];
            return;
        }
        //进入白方回合：已拜访状态，未拜访状态（终局，未终局）
        for(int i=0;i<state.action_space().size();++i){
            TicTacToeState new_state = state.next(state.action_space()[i]);
            auto iter = _values.find(new_state.state());
            if(iter==_values.end()){
                Td_Node* new_node= new Td_Node(new_state);
                _all_nodes[new_state.state()]=new_node;
                if(new_state.done()){
                    ////////////////////////////////////
                    //std::cout<<"ends"<<std::endl;//
                    //new_state.show();
                    //std::cout<<std::endl;//
                    ////////////////////////////////////

                    new_node->_done=true;
                    _values[new_state.state()]=new_state.rewards()[1];
                }
                else{
                    _values[new_state.state()]=0;
                    _expand(new_node);
                }
            }
        }
        return;
    }
    //第二步：epsilon-greedy采样
    const uint32_t _epsilon_greedy(double epsilon,Td_Node* cur_node_){
        //std::bad_alloc发生位置
        TicTacToeState state=cur_node_->_node.next(cur_node_->_node.action_space()[0]);
        //
        if(RandomVariables::uniform_real()>epsilon){
            //选择最优
            double max_val_{-3};
            uint32_t ret;
            for(int i=0;i<state.n_actions();++i){
                TicTacToeState next_state = state.next(state.action_space()[i]);
                double cur_val_= _values[next_state.state()];
                if(cur_val_>=max_val_){
                    max_val_=cur_val_;
                    ret=next_state.state();
                }
            }
            return ret;
        }
        else{
            //随机选择
            TicTacToeState next_state =state.next(state.action_space()[RandomVariables::uniform_int()%state.n_actions()]);            
            return next_state.state();
        }
    }
    //第三步：迭代维护值函数表
    void _update_values(double alpha,double epsilon,Td_Node* cur_node){
        if(cur_node->_done){
            return;
        }
        else{
            //已知错误：std::bad_alloc
            uint32_t eps_selection = _epsilon_greedy(epsilon,cur_node);
            auto iter = _all_nodes.find(eps_selection);
            assert(iter!=_all_nodes.end());
            Td_Node* selected_node = _all_nodes[eps_selection];           
            _update_values(alpha,epsilon,selected_node);
            _values[cur_node->_node.state()]=_values[cur_node->_node.state()]
                                            +alpha*(_values[selected_node->_node.state()]-_values[cur_node->_node.state()]);
            return;
        }
    }
public:
    Td_Learning(double alpha,double epsilon0,int iterations):_root_node(TicTacToeState()),_alpha(alpha),
                                                            _epsilon0(epsilon0),_iterations(iterations)
                                                            {
                                                                _values[_root_node._node.state()]=0;
                                                            }
    ~Td_Learning(){
        //std::cout<<_all_nodes.size()<<std::endl;
        for(std::unordered_map<uint32_t,Td_Node*>::iterator it=_all_nodes.begin();it!=_all_nodes.end();++it){
            //std::cout<<it->first<<std::endl;
            delete it->second;
        }
        _all_nodes.clear();
        _values.clear();
    }
    void expand(){
        _expand(&_root_node);
    }
    void update_values(){
        for(int i=0;i<_iterations;++i){           
           _update_values(_alpha,_epsilon0*(1-i/_iterations),&_root_node); 
        }       
        return;
    }
    //for debug:展示kv对
    void show_values(){
        for(std::unordered_map<uint32_t,double>::iterator it=_values.begin();it!=_values.end();++it){
            std::cout<<it->first<<":"<<it->second<<std::endl;
        }
    }
};
#pragma once

#include "../problem/tictactoe.hpp"
#include <unordered_map>

//将轮到我方落子的状态包装为节点
class Td_Node{
private:
    TicTacToeState _node;  
public:
    //构造子节点
    Td_Node(TicTacToeState const& state){
        _node = state;
    }
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
public:
    Td_Learning():_root_node(TicTacToeState()){
        _root_node._node=_root_node._node.next(_root_node._node.action_space()[0]);
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
    void start_expand(){
        expand(&_root_node);
    }
    //第一步：拓展所有可能情况
    void expand(Td_Node* current_node){
        //保险起见：该节点已终局
        if(current_node->_node.done()){
            _values[current_node->_node.state()]=current_node->_node.rewards()[1];
            return;
        }
        //在黑方动作终局：更新白方node
        TicTacToeState state=current_node->_node;
        state=state.next(state.action_space()[0]);
        if(state.done()){
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
                    _values[new_state.state()]=new_state.rewards()[1];
                }
                else{
                    _values[new_state.state()]=0;
                    expand(new_node);
                }
            }
        }
        return;
    }
    //undone
    //第二步：迭代维护值函数表
    void update_values(){
        
    }
    
    //for debug:展示kv对
    void show_values(){
        for(std::unordered_map<uint32_t,double>::iterator it=_values.begin();it!=_values.end();++it){
            std::cout<<it->first<<":"<<it->second<<std::endl;
        }
    }

};
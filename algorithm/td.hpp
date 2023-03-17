#pragma once

#include "../problem/tictactoe.hpp"
#include <unordered_map>

//将轮到我方落子的状态包装为节点
class Td_Node{
private:
    TicTacToeState _node;
    std::vector<uint32_t> _reachable_states;
    //模仿基类风格，继承优良传统
    static constexpr int take_table[9]{
        0b111111111111111100,
        0b111111111111110011,
        0b111111111111001111,
        0b111111111100111111,
        0b111111110011111111,
        0b111111001111111111,
        0b111100111111111111,
        0b110011111111111111,
        0b001111111111111111,
    };
public:
    //构造子节点
    Td_Node(TicTacToeState const& state){
        _node = state;
        update_reachable();
    }
    ~Td_Node(){}
    void update_reachable(){      
        _reachable_states.clear();
        uint32_t try_state=_node.state();
        for(int i=8;i>=0;--i){
            if((try_state >> (i << 1)) & 0b11==0b01){
                try_state &= take_table[i];
                for(int j=i;j<9;++j){
                    uint32_t last_state = try_state;
                    if(((last_state >> (j << 1)) & 0b11)==0b10){
                        last_state &= take_table[j];
                        _reachable_states.push_back(last_state);
                    }
                }
                break;
            }
        }
    }
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
    std::vector<Td_Node> _done_nodes;
    Td_Node _root_node;
public:
    Td_Learning():_root_node(TicTacToeState()){
        _root_node._node=_root_node._node.next(_root_node._node.action_space()[0]);
        _values[_root_node._node.state()]=0;
    }
    //第一步：拓展所有可能情况
    void expand(Td_Node* current_node){
        //保险起见：该节点已终局
        if(current_node->_node.done()){
            _done_nodes.push_back(*current_node);
            _values[current_node->_node.state()]=current_node->_node.rewards()[1];
            return;
        }
        //在黑方动作终局：更新白方node
        TicTacToeState state=current_node->_node;
        state=state.next(state.action_space()[0]);
        if(state.done()){
            _done_nodes.push_back(*current_node);
            _values[current_node->_node.state()]=state.rewards()[1];
            return;
        }
        //进入白方回合：已拜访状态，未拜访状态（终局，未终局）
        for(int i=0;i<state.action_space().size();++i){
            TicTacToeState new_state = state.next(state.action_space()[i]);
            auto iter = _values.find(new_state.state());
            if(iter==_values.end()){
                Td_Node new_node(new_state);
                if(new_state.done()){
                    _done_nodes.push_back(new_node);
                    _values[new_state.state()]=new_state.rewards()[1];
                }
                else{
                    expand(&new_node);
                }
            }
        }
        return;
    }
    //第二步：迭代维护值函数表
}
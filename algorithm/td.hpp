#pragma once

#include "../problem/tictactoe.hpp"
#include <unordered_map>

//将轮到我方落子的状态包装为节点
class Td_Node{
private:
    TicTacToeState _node;
    std::vector<Td_Node*> _parent_nodes;
    std::vector<Td_Node*> _child_nodes;
public:
    //构造子节点
    Td_Node(Td_Node* parent_node,TicTacToeState const& state){
        _parent_nodes.push_back(parent_node);
        _node = state;
    }
    //构造无父节点
    Td_Node(TicTacToeState const& state){
        _node=state;
    }
    ~Td_Node(){
        for(int i=0;i<_child_nodes.size();++i){
            if(_child_nodes[i]!=nullptr){
                delete _child_nodes[i];
                _child_nodes[i]=nullptr;
                _child_nodes.pop_back();
            }
        }
    }
    //将子节点与父节点连接
    void add_as_child(Td_Node* parent_node){
        parent_node->_child_nodes.push_back(this);
        _parent_nodes.push_back(parent_node);
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
    std::unordered_map<Td_Node,double> _values;
    std::vector<Td_Node*> _done_nodes;
    Td_Node _root_node;
public:
    Td_Learning():_root_node(TicTacToeState()){
        _root_node._node=_root_node._node.next(_root_node._node.action_space()[0]);
        _values[_root_node]=0;
    }
    //第一步：拓展所有可能情况
    void expand(Td_Node* current_node){
        //保险起见：该节点已终局
        if(current_node->_node.done()){
            _done_nodes.push_back(current_node);
            _values[*current_node]=current_node->_node.rewards()[1];
            return;
        }
        //在黑方动作终局：更新白方node
        TicTacToeState state=current_node->_node;
        state=state.next(state.action_space()[0]);
        if(state.done()){
            _done_nodes.push_back(current_node);
            _values[*current_node]=state.rewards()[1];
            return;
        }
        //进入白方回合：已拜访状态，未拜访状态（终局，未终局）
    }
}
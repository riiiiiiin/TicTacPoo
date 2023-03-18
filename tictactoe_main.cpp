#include "algorithm/td.hpp"
#include <ctime>
class TicTacToePolicy{
public:
    int operator() (TicTacToeState state){
        
        // 轮到X时，其选择第一个可行动作
        if (state.active_player() == 0){
            
            return state.action_space()[0];
        
        } else {
            // 学习得到值函数表之后，把下面这句话替换成为根据值函数表贪心选择动作
            return state.action_space()[0]; 
        }
    }
};

int main(){
    TicTacToeState state;
    TicTacToePolicy policy;
    std::clock_t t0=clock();
    Td_Learning tdl;
    tdl.start_expand();
    tdl.show_values();
    std::cout<<((double)clock()-t0)/CLOCKS_PER_SEC<<std::endl;
    // TODO: 通过与环境多次交互，学习打败X策略的方法


    // 测试O是否能够打败X的策略
    while (not state.done()){
        auto action = policy(state);
        state = state.next(action);
        state.show();
    }
    return 0;
}
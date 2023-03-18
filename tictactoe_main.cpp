#include "problem/tictactoe.hpp"

class TicTacToePolicy
{
public:
    int operator()(TicTacToeState state)
    {

        // 轮到X时，其选择第一个可行动作
        if (state.active_player() == 0)
        {

            return state.action_space()[0];
        }
        else
        {
            // 学习得到值函数表之后，把下面这句话替换成为根据值函数表贪心选择动作
            return state.action_space()[0];
            /*


            Select best next state ()
            {
                for(acttion state size)
                {
                    if (value_table(_state + action) > value_table(best))
                    {
                        best = _state + action
                    }
                }
                90% do the best
                10% for fun
            }
            add selected state to a queue
            return action

            */
        }
    }
};

int main()
{
    TicTacToeState state;
    TicTacToePolicy policy;

    // TODO: 通过与环境多次交互，学习打败X策略的方法
    // Learning
    /*

    for(learning)
    {
        TicTacToeState Learningstate;
        while(not state.done())
        {
            auto action = policy(state);
            state = state.next(action);
            state.show();
        }
        reward_my_sweety(selected queue,win or lose);
    }



    */

    // 测试O是否能够打败X的策略
    // Testing
    while (not state.done())
    {
        auto action = policy(state);
        state = state.next(action);
        state.show();
    }
    return 0;
}
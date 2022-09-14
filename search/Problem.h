#ifndef CHESS_ENGINE_PROBLEM_H
#define CHESS_ENGINE_PROBLEM_H

#include <vector>

namespace search
{
    template<typename State, typename Action>
    class Problem
    {
    public:
        virtual State initialState() = 0;

        virtual bool isGoal(const State *state) = 0;

        virtual std::vector<Action *> actions(const State *state) = 0;

        virtual State *successor(const State *state, const Action *action) = 0;

        virtual double cost(const State *state, const Action *action) = 0;

        double heuristic(const State *state) { return 0.0; }

        virtual ~Problem() = default;
    };
} // search

#endif //CHESS_ENGINE_PROBLEM_H

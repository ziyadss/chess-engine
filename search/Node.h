#ifndef CHESS_ENGINE_NODE_H
#define CHESS_ENGINE_NODE_H

#include "Problem.h"

namespace search
{
    template<typename State, typename Action>
    class Node
    {
        State *state;

        std::vector<Action *> path;
        // O(1) backtracking at cost of memory, can be reversed by using commented lines below instead
        // (along with similar changes in 'child_node' and 'backtrack' functions below)
        //Node *parent;
        //Action *action;

        double pathCost;
        double heuristicCost;
        int insertionOrder;

        Node(const State *state, const std::vector<Action *> &path, double pathCost, double heuristicCost,
             int insertionOrder) : state(state), path(path), pathCost(pathCost), heuristicCost(heuristicCost),
                                   insertionOrder(insertionOrder) {}

    public:
        explicit Node(const State& state) : state(&state), pathCost(0.0), heuristicCost(0.0), insertionOrder(0) {}

        bool operator<(const Node<State, Action> &other) const;

        bool operator<=(const Node<State, Action> &other) const;

        bool operator>(const Node<State, Action> &other) const;

        bool operator>=(const Node<State, Action> &other) const;

        std::vector<Action *> backtrack() const;

        Node<State, Action> *child(const Problem<State, Action> &problem, const Action *action, int order);
    };
} // search

#endif //CHESS_ENGINE_NODE_H

#ifndef CHESS_ENGINE_NODE_H
#define CHESS_ENGINE_NODE_H

#include "Problem.h"

namespace search
{
    template<typename State, typename Action>
    class Node
    {
        State *m_state;

        std::vector<Action *> m_path;
        // O(1) backtracking at cost of memory, can be reversed by using commented lines below instead
        // (along with similar changes in 'child_node' and 'backtrack' functions below)
        //Node *parent;
        //Action *action;

        double m_pathCost;
        double m_heuristicCost;
        int m_insertionOrder;

        constexpr Node(const State *state, const std::vector<Action *> &path, double pathCost, double heuristicCost, int insertionOrder)
                : m_state(state), m_path(path), m_pathCost(pathCost), m_heuristicCost(heuristicCost), m_insertionOrder(insertionOrder) {}

    public:
        explicit constexpr Node(const State &state) : m_state(&state), m_pathCost(0.0), m_heuristicCost(0.0), m_insertionOrder(0) {}

        constexpr bool operator<(const Node<State, Action> &other) const
        {
            if (this->heuristicCost + this->pathCost == other.heuristicCost + other.pathCost)
                return this->insertionOrder < other.insertionOrder;
            return this->heuristicCost + this->pathCost < other.heuristicCost + other.pathCost;
        }

        constexpr bool operator<=(const Node<State, Action> &other) const
        {
            if (this->heuristicCost + this->pathCost == other.heuristicCost + other.pathCost)
                return this->insertionOrder <= other.insertionOrder;
            return this->heuristicCost + this->pathCost < other.heuristicCost + other.pathCost;
        }

        constexpr bool operator>(const Node<State, Action> &other) const { return other < *this; }

        constexpr bool operator>=(const Node<State, Action> &other) const { return other <= *this; }

        constexpr std::vector<Action *> backtrack() const
        {
            return this->path;
            // O(1) backtracking at cost of memory, can be reversed by using commented lines below instead
            // (along with similar changes in 'child_node' function and 'Node' class above)

            //std::vector<Action *> path;
            //Node<State, Action> *node = this;
            //while (node != nullptr)
            //{
            //    path.insert(path.begin(), node->action);
            //    node = node->parent;
            //}
            //return path;
        }

        constexpr Node<State, Action> *child(const Problem<State, Action> &problem, const Action *action, int order) const
        {
            State *childState = problem.successor(*this->state, action);
            std::vector<Action *> childPath = this->path;
            childPath.push_back(&action);
            double childPathCost = this->pathCost + problem.cost(this->state, action);
            double childHeuristicCost = problem.heuristic(childState);
            return new Node<State, Action>(childState, childPath, childPathCost, childHeuristicCost, order);
        }
    };
} // search

template<typename State, typename Action>
struct std::hash<search::Node<State, Action>>
{
    size_t operator()(const search::Node<State, Action> &node) const
    {
        return hash<State>()(node.state);
    }
};

#endif //CHESS_ENGINE_NODE_H

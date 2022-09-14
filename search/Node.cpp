#include "Node.h"

namespace search
{
    template<typename State, typename Action>
    bool Node<State, Action>::operator<(const Node<State, Action> &other) const
    {
        if (this->heuristicCost + this->pathCost == other.heuristicCost + other.pathCost)
            return this->insertionOrder < other.insertionOrder;
        return this->heuristicCost + this->pathCost < other.heuristicCost + other.pathCost;
    }

    template<typename State, typename Action>
    bool Node<State, Action>::operator<=(const Node<State, Action> &other) const
    {
        if (this->heuristicCost + this->pathCost == other.heuristicCost + other.pathCost)
            return this->insertionOrder <= other.insertionOrder;
        return this->heuristicCost + this->pathCost < other.heuristicCost + other.pathCost;
    }

    template<typename State, typename Action>
    bool Node<State, Action>::operator>(const Node<State, Action> &other) const
    {
        return other < *this;
    }

    template<typename State, typename Action>
    bool Node<State, Action>::operator>=(const Node<State, Action> &other) const
    {
        return other <= *this;
    }

    template<typename State, typename Action>
    std::vector<Action *> Node<State, Action>::backtrack() const
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

    template<typename State, typename Action>
    Node<State, Action> *
    Node<State, Action>::child(const Problem<State, Action> &problem, const Action *action, int order)
    {
        State *childState = problem.successor(*this->state, action);
        std::vector<Action *> childPath = this->path;
        childPath.push_back(&action);
        double childPathCost = this->pathCost + problem.cost(this->state, action);
        double childHeuristicCost = problem.heuristic(childState);
        return new Node<State, Action>(childState, childPath, childPathCost, childHeuristicCost, order);
    }
} // search

template<typename State, typename Action>
struct std::hash<search::Node<State, Action>>
{
    size_t operator()(const search::Node<State, Action> &node) const
    {
        return hash<State>()(node.state);
    }
};

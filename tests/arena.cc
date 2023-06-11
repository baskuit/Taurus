#pragma once

// #include <algorithm/search.hh>
#include <surskit.hh>

#include <unordered_map>
#include <vector>
#include <utility>

template <class State, class Model>
class Arena : PerfectInfoState<SimpleTypes>
{
public:
    struct Types : SimpleTypes
    {
    };

    const std::vector<S *> searches;

    Arena(std::vector<S *> &&searches) : searches{std::move(searches)}
    {
        const size_t size = searches.size();
        this->row_actions.fill(size);
        this->col_actions.fill(size);

        for (size_t i = 0; i < size; ++i)
        {
            this->row_actions[i] = i;
            this->col_actions[i] = i;
        }
    }

    void get_actions() {}

    void apply_actions(
        typename Types::Action row_action,
        typename Types::Action col_action)
    {
        this->is_terminal = true;
        auto row_search = searches[row_action];
        auto col_search = searches[col_action];

        State sub_state = row_search->get_new_state();

        // simulate game
    }

    void play_vs(
        S *row_search, 
        S *col_search, 
        State &initial_state)
    {
    }
};

int main()
{

    using State = MoldState<2>;
    using Model = MonteCarloModel<State>;

    State state{10};
    Model model{0};

    Search<State, MonteCarloModel, Exp3, TreeBandit> s0{state, model};
    std::vector<S *> agents{&s0};
    // TODO:
    /*
    if yuo have a TraversedState<> ptr member then your State must pass static_assertion about being ChanceState
    */

    return 0;
}
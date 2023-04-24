#pragma once
#include "eval.hpp"
#include "surskit.hh"

/*
    Surskit wrapper for Pasy's heuristic
*/

template <class State>
class BattleHeuristic : public DoubleOracleModel<State>
{
    static_assert(std::derived_from<State, AbstractState<typename State::Types::TypeList>>);

public:
    struct Types : DoubleOracleModel<State>::Types
    {
    };

    BattleHeuristic() {}

    void get_inference(
        typename Types::State &state,
        typename Types::Inference &inference)
    {
        const typename Types::Real row_uniform = 1 / (typename Types::Real)state.actions.rows;
        inference.row_policy.fill(state.actions.rows, row_uniform);
        const typename Types::Real col_uniform = 1 / (typename Types::Real)state.actions.cols;
        inference.col_policy.fill(state.actions.cols, col_uniform);

        // const typename Types::Real row_value = .5;
        const typename Types::Real row_value = math::sigmoid(heuristic::battle_eval(state.battle_));
        inference.row_value = row_value;
        inference.col_value = 1 - row_value;
    }
};
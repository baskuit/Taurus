#pragma once
#include "surskit.hh"

template <class RowAlgorithm, class ColAlgorithm>
class Vs
{
public:
    using Real = typename RowAlgorithm::Types::Real;
    using RealVector = typename RowAlgorithm::Types::VectorReal;
    using RowModel = typename RowAlgorithm::Types::Model;
    using ColModel = typename ColAlgorithm::Types::Model;
    using State = typename RowAlgorithm::Types::State;

    prng row_device;
    prng col_device;

    RowAlgorithm row_session;
    ColAlgorithm col_session;

    Vs() {}

    Vs(int row_seed, int col_seed) : row_device(row_seed), col_device(col_seed)
    {
    }

    Vs(RowAlgorithm &row_session, ColAlgorithm &col_session) : row_session(row_session), col_session(col_session) {}

    void run (
        int playouts,
        State &state, 
        RowModel &row_model, 
        ColModel &col_model    
    ) {
        state.get_actions();
        while (!state.is_terminal) {
            RealVector row_strategy(state.actions.rows), col_strategy(state.actions.cols);

            MatrixNode<RowAlgorithm> row_root;
            row_session.run(playouts, row_device, state, row_model, row_root);
            row_session.get_strategies(&row_root, row_strategy, col_strategy);
            auto row_action = state.actions.row_actions[row_device.sample_pdf(row_strategy, state.actions.rows)];

            // Does not work with 2 different searches being run on it?

            // Copy constructor for battle object does not work correctly

            MatrixNode<ColAlgorithm> col_root;
            col_session.run(playouts, col_device, state, col_model, col_root);
            col_session.get_strategies(&col_root, row_strategy, col_strategy);
            auto col_action = state.actions.col_actions[col_device.sample_pdf(col_strategy, state.actions.cols)];

            state.apply_actions(row_action, col_action);
            state.get_actions();
        }
    }
};
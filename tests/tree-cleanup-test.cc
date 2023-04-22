#include "surskit.hh"

const size_t MaxTrace = 180;

int main () {

    using BattleSurskit = BattleSurskit<MaxTrace, 9>;
    using MonteCarlo = MonteCarloModel<BattleSurskit>;
    using Exp3p = Exp3p<MonteCarlo, TreeBandit>;
    using MatrixUCB = MatrixUCB<MonteCarlo, TreeBandit>;
    using Algorithm = Exp3p;

    BattleSurskit tauros_mirror(engine::RBY::monotauros, engine::RBY::monotauros);
    tauros_mirror.get_actions();
    tauros_mirror.apply_actions(
        tauros_mirror.actions.row_actions[0],
        tauros_mirror.actions.col_actions[0]
    );

    for (int i = 0; i < 1; ++i) {
        prng device;
        MonteCarlo model(device);
        Algorithm session;
        MatrixNode<Algorithm> root;
        const int playouts = 50000;
        session.run(playouts, device, tauros_mirror, model, root);
        BattleSurskit::Types::VectorReal row_strategy(9), col_strategy(9);
        session.get_strategies(&root, row_strategy, col_strategy);
        math::print(row_strategy, 9);
        math::print(col_strategy, 9);

        Algorithm::Types::MatrixReal row_value_matrix(4, 4), col_value_matrix(4, 4);
        
        // session.get_ev_matrix(&root, row_value_matrix, col_value_matrix);
        std::cout << i << std::endl;
    }

    return 0;
}
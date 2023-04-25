#include "surskit.hh"

const size_t MaxActions = 4;
const size_t MaxTrace = 60;

int main () {

    using BattleSurskit = BattleSurskit<4, MaxTrace>;
    using MonteCarlo = MonteCarloModel<BattleSurskit>;
    using BattleHeuristic = BattleHeuristic<BattleSurskit>;
    using Model = MonteCarlo;
    using Exp3p = Exp3p<MonteCarlo, TreeBanditThreaded>;
    using MatrixUCB = MatrixUCB<Model, TreeBanditThreaded>;
    using Algorithm = MatrixUCB;

    BattleSurskit tauros_mirror(engine::RBY::monotauros, engine::RBY::monotauros);
    tauros_mirror.get_actions();
    tauros_mirror.apply_actions(
        tauros_mirror.actions.row_actions[0],
        tauros_mirror.actions.col_actions[0]
    );

    const int max_tests = 10;
    for (int test = 0; test < max_tests; ++test) {

        prng device(test);
        Model model(device);
        Algorithm session(1, .05);
        MatrixNode<Algorithm> root;

        session.threads = 3;
        // session.expl_threshold = .05;

        std::cout << session << std::endl;

        const int sub_playouts = 5000000;
        const int playout_iter = 10;
        Algorithm::Types::VectorReal row_strategy(MaxActions), col_strategy(MaxActions);
        for (int i = 0; i < playout_iter; ++i) {
            session.run(sub_playouts, device, tauros_mirror, model, root);
            std::cout << "playouts: " << (i + 1) * sub_playouts << std::endl;
            std::cout << "matrix node count: " << root.count_matrix_nodes() << std::endl;
            std::cout << "strategies: " << std::endl;
            session.get_strategies(&root, row_strategy, col_strategy);
            math::print(row_strategy, MaxActions);
            math::print(col_strategy, MaxActions);

            Algorithm::Types::MatrixReal row_value_matrix(4, 4), col_value_matrix(4, 4);
            session.get_ev_matrix(&root, row_value_matrix, col_value_matrix);
            std::cout << "row payoff matrix: " << std::endl;
            row_value_matrix.print();
            std::cout << "visit matrix: " << std::endl;
            root.stats.visit_matrix.print();
        }

    }

    return 0;
}
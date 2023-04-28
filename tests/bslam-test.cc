#include "surskit.hh"
#include "battle-surskit.hh"

const size_t MaxTrace = 180;

int main () {

    using BattleSurskit = BattleSurskitVector<MaxTrace>;
    using MonteCarlo = MonteCarloModel<BattleSurskit>;
    using Exp3p = Exp3p<MonteCarlo, TreeBandit>;
    using MatrixUCB = MatrixUCB<MonteCarlo, TreeBandit>;
    using Algorithm = MatrixUCB;

    BattleSurskit state(engine::RBY::miedon, engine::RBY::zapjynx);
    state.get_actions();
    state.apply_actions(
        state.actions.row_actions[0],
        state.actions.col_actions[0]
    );

    prng device(1);
    MonteCarlo model(device);
    Algorithm session;
    MatrixNode<Algorithm> root;

    // session.threads = 3;
    // session.expl_threshold = .05;



    const int sub_playouts = 1000000;
    const int playout_iter = 10;
    Algorithm::Types::VectorReal row_strategy(9), col_strategy(9);

    session.run(1000, device, state, model, root);
    // always segfaults if comment out below????
    std::cout << "playouts: " << (0 + 1) * sub_playouts << std::endl;
    std::cout << "matrix node count: " << root.count_matrix_nodes() << std::endl;
    std::cout << "strategies: " << std::endl;
    session.get_strategies(&root, row_strategy, col_strategy);
    math::print(row_strategy, 9);
    math::print(col_strategy, 9);

    Algorithm::Types::MatrixReal row_value_matrix(4, 4), col_value_matrix(4, 4);
    session.get_ev_matrix(&root, row_value_matrix, col_value_matrix);
    std::cout << "row payoff matrix: " << std::endl;
    row_value_matrix.print();
    std::cout << "visit matrix: " << std::endl;
    root.stats.visit_matrix.print();

    // for (int i = 1; i < playout_iter; ++i) {
    //     session.run(sub_playouts, device, state, model, root);
    //     std::cout << "playouts: " << (i + 1) * sub_playouts << std::endl;
    //     std::cout << "matrix node count: " << root.count_matrix_nodes() << std::endl;
    //     std::cout << "strategies: " << std::endl;
    //     session.get_strategies(&root, row_strategy, col_strategy);
    //     math::print(row_strategy, 9);
    //     math::print(col_strategy, 9);

    //     Algorithm::Types::MatrixReal row_value_matrix(4, 4), col_value_matrix(4, 4);
    //     session.get_ev_matrix(&root, row_value_matrix, col_value_matrix);
    //     std::cout << "row payoff matrix: " << std::endl;
    //     row_value_matrix.print();
    //     std::cout << "visit matrix: " << std::endl;
    //     root.stats.visit_matrix.print();
    // }

    // auto cur_chance_node = root.child;
    // while (cur_chance_node != nullptr) {

    //     std::cout << "chance node: " << cur_chance_node->row_idx << ' ' << cur_chance_node->col_idx << std::endl;
    //     auto first_born = cur_chance_node->child;
    //     std::cout << first_born->count_siblings() << std::endl;
    //     cur_chance_node = cur_chance_node->next;
    // }

    return 0;
}
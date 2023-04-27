#include "surskit.hh"
#include "battle-surskit.hh"
#include "battle-heuristic.hh"
#include "vs.hh"

const size_t MaxActions = 9;
const size_t MaxTrace = 180;

int main () {

    using BattleSurskit = BattleSurskitVector<MaxTrace>;

    using MonteCarlo = MonteCarloModel<BattleSurskit>;
    using Heuristic = BattleHeuristic<BattleSurskit>;

    using Exp3pMonteCarlo = Exp3p<MonteCarlo, TreeBandit>;
    using MatrixUCBMonteCarlo = MatrixUCB<MonteCarlo, TreeBandit>;

    using Exp3pHeuristic = Exp3p<Heuristic, TreeBandit>;
    using MatrixUCBHeuristic = MatrixUCB<Heuristic, TreeBandit>;

    BattleSurskit miedon_mirror(engine::RBY::miedon, engine::RBY::miedon);
    miedon_mirror.get_actions();
    miedon_mirror.apply_actions(
        miedon_mirror.actions.row_actions[0],
        miedon_mirror.actions.col_actions[0]
    );

    const int games = 100;
    const int playouts = 20;

    prng row_device, col_device;

    Heuristic heuristic_model;
    MonteCarlo monte_carlo_model(col_device);

    double row_score = 0;
    int total_game_length = 0;

    for (int game = 0; game < games; ++game) {
        auto miedon_mirror_copy = miedon_mirror;
        Vs<Exp3pHeuristic, Exp3pMonteCarlo> arena;
        int game_length = 0;
        arena.run(playouts, miedon_mirror_copy, heuristic_model, monte_carlo_model, game_length);
        total_game_length += game_length;
        row_score += miedon_mirror_copy.row_payoff;
        std::cout << row_score / (game + 1) << std::endl;
        std::cout << "Average score for heuristic_model: " << row_score / (game + 1) << std::endl;
        std::cout << "Average game length: " << total_game_length / (game + 1) << std::endl;
    }

    double row_average_score = row_score / games;

    std::cout << "Playouts per turn: "<< playouts << " Average score for heuristic_model: " << row_average_score << std::endl;

    return 0;
};
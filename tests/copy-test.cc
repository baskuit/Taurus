#include "surskit.hh"
#include "battle-surskit.hh"
#include "battle-heuristic.hh"

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

    std::cout << miedon_mirror.actions.rows << ' ' << miedon_mirror.actions.cols << '\n';
    miedon_mirror.get_actions();
    std::cout << miedon_mirror.actions.rows << ' ' << miedon_mirror.actions.cols << '\n';
    auto miedon_mirror_copy = miedon_mirror;
    std::cout << miedon_mirror_copy.actions.rows << ' ' << miedon_mirror_copy.actions.cols << '\n';
    auto actions_copy = miedon_mirror.actions;
    std::cout << "Actions obj only " << actions_copy.rows << ' ' << actions_copy.cols << std::endl;
    std::cout << (miedon_mirror.transition.obs == miedon_mirror_copy.transition.obs) << std::endl;
    return 0;
};
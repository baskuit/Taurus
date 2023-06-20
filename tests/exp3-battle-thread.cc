#include <surskit.hh>

#include "battle-surskit.hh"

int main()
{
    constexpr size_t LogSize{64};
    const int playouts = 1000000;
    const int trials = 1;

    for (int trial = 0; trial < trials; ++trial)
    {
        using Battle = Battle<LogSize>;
        using Model = EmptyModel<Battle>;
        using Algorithm = TreeBanditThreaded<Exp3<Model>, MatrixNode, ChanceNode>;


        prng device(0);
        Battle battle(engine::RBY::miedon, engine::RBY::miedon);
        battle.get_actions();
        battle.apply_actions(battle.row_actions[0], battle.col_actions[0]);
        battle.get_actions();
        std::cout << battle.row_actions.size() << ' '<< battle.col_actions.size() << std::endl;

        Model model{};
        Algorithm session;
        session.threads = 4;

        Algorithm::Types::MatrixNode root;

        session.run(playouts, device, battle, model, root);

        std::cout << root.count_matrix_nodes() << std::endl;
        std::cout << root.row_actions.size() << ' '<< root.col_actions.size() << std::endl;
    }

    return 0;
}
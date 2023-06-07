#include <surskit.hh>

#include "battle-surskit.hh"


int main()
{
    constexpr size_t LogSize{64};
    const int playouts = 20;
    const int trials = 1;

    for (int trial = 0; trial < trials; ++trial)
    {
        using Battle = Battle<LogSize>;
        using Model = MonteCarloModel<Battle>;
        using Algorithm = Exp3<Model, OffPolicy>;

        prng device(0);
        Battle battle(engine::RBY::miedon, engine::RBY::miedon);
        battle.get_actions();
        battle.apply_actions(battle.row_actions[0], battle.col_actions[0]);
        std::vector<Battle> states{battle};


        Model model(device);
        Algorithm session;

        Algorithm::Types::MatrixNode root;
        std::vector<Algorithm::Types::MatrixNode*> matrix_nodes{&root};

        session.run(playouts, 4, device, states, model, matrix_nodes);

        // std::cout << root.count_matrix_nodes() << std::endl;
        // std::cout << root.row_actions.size() << ' '<< root.col_actions.size() << std::endl;
    }

    return 0;
}
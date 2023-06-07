#include <surskit.hh> 

template <class Algorithm>
void get_games(
    prng &device,
    size_t trials,
    double expl_threshold,
    std::vector<size_t> depth_bound_vec,
    std::vector<size_t> actions_vec,
    size_t iterations,
    typename Algorithm::Types::Model &model)
{

    for (size_t depth_bound : depth_bound_vec) {
        for (size_t actions : actions_vec) {
            for (size_t i = 0; i < trials; ++i) {
                RandomTree state{device, depth_bound, actions, actions, 1, 0};
                MatrixNode<Algorithm> root{};
                Algorithm session{};
                session.run(iterations, device, state, model, root);

                typename Algorithm::Types::VectorReal row_strategy{root.row_actions.size()}, col_strategy{root.col_actions.size()};
                session.get_empirical_strategies(&root, row_strategy, col_strategy);

                auto expl = exploitability<typename Algorithm::Types>(nullptr, row_strategy, col_strategy);

                if (expl > expl_threshold) {
                    exit(1);
                }


            }
        }
    }

}

size_t main()
{

    using Model = MonteCarloModel<RandomTree>;
    using Algorithm = Exp3<Model, TreeBandit>;

    prng device(0);
    Model model(device);

    get_games<Algorithm>(
        device,
        100,
        .05,
        {1},
        {2, 3, 4},
        10000,
        model
    );

    return 0;
}
#include <surskit.hh>

uint64_t seed = 2722585483358202473;
const int depth_bound = 1;
const size_t rows = 3;
const size_t cols = 3;

int main()
{
    using Model = MonteCarloModel<RandomTree>;
    using Algorithm = TreeBandit<Exp3<Model>>;
    using AlgorithmOld = TreeBandit<Exp3Old<Model>>;

    prng device(285608215);

    Model model(device);

    size_t iterations = 1000;

    RandomTree state{seed, depth_bound, rows, cols, 1, 0.0};
    TraversedState<Model> traversed_state(state, model);

    double gamma = .05;

    for (int i = 0; i < 4; ++i)
    {
        std::cout << "Iterations: " << iterations << std::endl;;
        {
            Algorithm session{gamma};
            prng device_copy{device};

            Algorithm::Types::MatrixNode root{};
            session.run(iterations, device_copy, state, model, root);

            Model::Types::VectorReal row_strategy, col_strategy;
            session.get_refined_strategies(root.stats, row_strategy, col_strategy);
            math::print(row_strategy);
            math::print(col_strategy);

            auto expl = math::exploitability<Model::Types>(traversed_state.current_node->stats.nash_payoff_matrix, row_strategy, col_strategy);
            std::cout << static_cast<double>(expl) << std::endl;
        };

        {
            AlgorithmOld session{gamma};
            prng device_copy{device};

            typename AlgorithmOld::Types::MatrixNode root{};
            session.run(iterations, device_copy, state, model, root);

            typename Model::Types::VectorReal row_strategy, col_strategy;
            session.get_refined_strategies(root.stats, row_strategy, col_strategy);
            math::print(row_strategy);
            math::print(col_strategy);

            auto expl = math::exploitability<Model::Types>(traversed_state.current_node->stats.nash_payoff_matrix, row_strategy, col_strategy);
            std::cout << static_cast<double>(expl) << std::endl;
        };

        iterations *= 10;
    }

    return 0;
}
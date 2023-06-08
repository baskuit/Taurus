#include <surskit.hh>

int main()
{


    // struct Empty {};

    std::vector<size_t> db{1};
    std::vector<size_t> a{2, 3, 4};
    std::vector<size_t> c{1};
    std::vector<double> t{0.0};
    std::vector<size_t> trials;
    trials.resize(100);

    prng device(285608215);
    std::cout << device.get_seed() << std::endl;

    RandomTreeGenerator x{device, db, a, c, t, trials};

    MonteCarloModel<RandomTree> model(device);

    using Algorithm = Exp3<MonteCarloModel<RandomTree>>;
    const size_t iterations = 10000;
    const double expl_threshold = .2;


    for (auto state : x)
    {

        // std::cout << "game seed: " << state.device.get_seed() << std::endl;
        MatrixNode<Algorithm> root{};
        Algorithm session{};
        session.run(iterations, device, state, model, root);

        typename Algorithm::Types::VectorReal row_strategy{root.row_actions.size()}, col_strategy{root.col_actions.size()};
        session.get_empirical_strategies(&root, row_strategy, col_strategy);

        TraversedState<typename Algorithm::Types::Model> traversed_state(state, model);
        typename Algorithm::Types::MatrixValue &matrix{traversed_state.current_node->stats.nash_payoff_matrix};
        // std::cout << "matrix:\n" << matrix << std::endl;
        // math::print(matrix);
        auto expl = exploitability<typename Algorithm::Types>(matrix, row_strategy, col_strategy);
        std::cout << "expl: " << expl << std::endl;
        if (expl > expl_threshold)
        {
            std::cout << "ERROR" << std::endl;
            exit(1);
        }
    }

    return 0;
}
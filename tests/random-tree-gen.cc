#include <surskit.hh>

int main()
{

    std::vector<size_t> db{1};
    std::vector<size_t> a{2, 3};
    std::vector<size_t> c{1};
    std::vector<double> t{0.0};
    size_t trials = 10;

    prng device(0);

    RandomTreeGenerator x{
        device, db, a, c, t, trials};

    MonteCarloModel<RandomTree> model(device);

    using Algorithm = Exp3<MonteCarloModel<RandomTree>>;
    const size_t iterations = 1000000;
    const double expl_threshold = .05;

    for (auto state : x)
    {
        // std::cout << tree.payoff.get_row_value() << std::endl;
        MatrixNode<Algorithm> root{};
        Algorithm session{};
        session.run(iterations, device, state, model, root);

        typename Algorithm::Types::VectorReal row_strategy{root.row_actions.size()}, col_strategy{root.col_actions.size()};
        session.get_empirical_strategies(&root, row_strategy, col_strategy);

        typename Algorithm::Types::MatrixValue matrix{state.rows, state.cols};
        auto expl = exploitability<typename Algorithm::Types>(matrix, row_strategy, col_strategy);

        math::print(row_strategy);
        math::print(col_strategy);

        std::cout << expl << std::endl;

        TraversedState<typename Algorithm::Types::Model> traversed_state(state, model);
        if (expl > expl_threshold)
        {
            exit(1);
        }
    }

    return 0;
}
#include <surskit.hh>

int main()
{


    prng device(285608215);
    std::vector<size_t> depth_bounds{1, 2};
    std::vector<size_t> actions{2, 3, 4};
    std::vector<size_t> chance_actions{1};
    std::vector<double> transition_thresholds{0.0};
    size_t states_per{20};

    RandomTreeGenerator generator{device, depth_bounds, actions, chance_actions, transition_thresholds, states_per};

    double expl_threshold{.2};

    using Model = MonteCarloModel<RandomTree>;
    Model model(device);

    const size_t iterations = 1000000;

    for (RandomTree&& state : generator)
    {
        Search<RandomTree, MonteCarloModel, Exp3, TreeBandit> search(state, model);
        search.run(iterations);
        // auto expl = search.get_expl();
        // std::cout << "expl: " << expl << std::endl;
    

        std::vector<double> row_strategy, col_strategy;

        TraversedState<Model> traversed_state(state, model);
        typename Model::Types::MatrixValue &matrix{traversed_state.current_node->stats.nash_payoff_matrix};

        auto expl = exploitability<typename Model::Types>(matrix, Vector(row_strategy), Vector(col_strategy));
        std::cout << "expl: " << expl << std::endl;
        if (expl > expl_threshold)
        {
            std::cout << "ERROR" << std::endl;
            exit(1);
        }
    }

    return 0;
}
#include <surskit.hh>

int main()
{


    // prng device(285608215);
    // std::vector<size_t> depth_bounds{1, 2};
    // std::vector<size_t> actions{2, 3, 4};
    // std::vector<size_t> chance_actions{1};
    // std::vector<double> transition_thresholds{0.0};
    // size_t states_per{10};

    // RandomTreeGenerator generator{device, depth_bounds, actions, chance_actions, transition_thresholds, states_per};

    // double expl_threshold{.2};

    // using Model = MonteCarloModel<RandomTree>;
    // Model model(device);

    // const size_t iterations = 1000000;

    // for (RandomTree&& state : generator)
    // {   
    //     TraversedState<Model> traversed_state(state, model);

    //     Search<RandomTree, MonteCarloModel, Exp3, TreeBandit> search(state, model, traversed_state);
    //     search.run(iterations);
    //     double expl = search.get_exploitability();

    //     std::cout << "depth: " << state.depth_bound << " actions: " << state.rows << std::endl; 
    //     std::cout << "expl: " << expl << std::endl;
    //     if (expl > expl_threshold)
    //     {
    //         std::cout << "ERROR" << std::endl;
    //         exit(1);
    //     }
    // }

    return 0;
}
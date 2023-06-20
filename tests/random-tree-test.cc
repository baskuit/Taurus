#include <surskit.hh>

int main () {

    prng device;

    RandomTree state{device, 1, 2, 2, 1, 0.0};
    MonteCarloModel<RandomTree> model{device};
    TraversedState<MonteCarloModel<RandomTree>> tree{state, model};

    tree.current_node->stats.nash_payoff_matrix;

    // std::cout << tree.current_node->stats.payoff.get_row_value() << std::endl;
    auto &m = tree.current_node->stats.nash_payoff_matrix;
    for (auto v : m) {
        // std::cout << v.get_row_value() << ' ';
    }
    std::cout << std::endl;

    return 0;
}
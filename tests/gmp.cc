#include <surskit.hh>

#include <chrono>
#include <tuple>

size_t foo(const size_t rows, const size_t cols, const size_t max)
{
    game g;
    prng device{};

    size_t entries = rows * cols;

    int row_num[entries];
    int row_den[entries];
    int col_num[entries];
    int col_den[entries];

    for (size_t idx = 0; idx < entries; ++idx)
    {
        row_num[idx] = device.random_int(max);
        row_den[idx] = device.random_int(max) + 1;
        col_num[idx] = row_den[idx] - row_num[idx];
        col_den[idx] = row_den[idx];
    }

    init_game(&g, rows, cols, row_num, row_den, col_num, col_den);

    mpz_t *row_data = alloc_data(rows + 2);
    mpz_t *col_data = alloc_data(cols + 2);

    auto start = std::chrono::high_resolution_clock::now();

    solve(&g, row_data, col_data);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    dealloc_data(row_data, rows + 2);
    dealloc_data(col_data, cols + 2);

    return duration;
}

int main()
{

    RandomTree tree{0, 1, 3, 3, 1, 0.0};
    MonteCarloModel<RandomTree> model{0};
    TraversedState<MonteCarloModel<RandomTree>> traversed_tree{tree, model};

    TraversedState<MonteCarloModel<RandomTree>>::Types::VectorReal row_strategy, col_strategy;

    LRSNash::solve_matrix<TraversedState<MonteCarloModel<RandomTree>>::Types>
        (traversed_tree.current_node->stats.nash_payoff_matrix, row_strategy, col_strategy);

    return 0;
}
#include <surskit.hh>

Matrix<PairDouble> random_matrix(prng &device, const size_t rows, const size_t cols)
{
    Matrix<PairDouble> payoff_matrix{rows, cols};
    for (auto &value : payoff_matrix)
    {
        value.row_value = device.uniform();
        value.col_value = 1 - value.row_value;
    }
    return payoff_matrix;
}

struct AAA {
    using MatrixValue = Matrix<PairDouble>;
    using VectorReal = std::vector<double>;
    using Real = double;
};

int main()
{

    prng device{0};

    for (size_t actions = 2; actions <= 9; ++actions)
    {
        auto payoff_matrix = random_matrix(device, actions, actions);

        std::vector<double> row_strategy{}, col_strategy{};
        auto start = std::chrono::high_resolution_clock::now();

        LRSNash::eee(payoff_matrix, row_strategy, col_strategy);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        double expl = math::exploitability(payoff_matrix, row_strategy, col_strategy);
        std::cout << "lrsnash: actions: " << actions << " expl: " << expl << " microsec: " << duration << std::endl;


        start = std::chrono::high_resolution_clock::now();
        LibGambit::solve_matrix<AAA>(payoff_matrix, row_strategy, col_strategy);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        expl = math::exploitability(payoff_matrix, row_strategy, col_strategy);
        std::cout << "gambit: actions: " << actions << " expl: " << expl << " microsec: " << duration << std::endl;
    }

    return 0;
}
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

struct AAA
{
    using MatrixValue = Matrix<PairDouble>;
    using VectorReal = std::vector<double>;
    using Real = double;
};

void test(prng &device, const size_t num_matrices, const size_t n_discrete)
{

    for (size_t rows = 2; rows <= 9; ++rows)
    {
        for (size_t cols = 2; cols <= 9; ++cols)
        {
            for (size_t matrix_idx = 0; matrix_idx < num_matrices; ++matrix_idx)
            {
                auto payoff_matrix = random_matrix(device, rows, cols);
                std::vector<double> row_strategy{}, col_strategy{};
                LRSNash::eee(payoff_matrix, row_strategy, col_strategy, n_discrete);
            }
        }
    }
}

int main()
{

    prng device{};
    std::cout << "device seed: " << device.get_seed() << std::endl;
    test(device, 1000, 100);

    return 0;
}
#include "surskit.hh"

/*

Simple executable to test that surskit is being linked correctly
(It's a header only library so I think it just 'included'....)

*/

int main()
{

    const int playouts = 10000000;
    const int trials = 100;

    std::cout << "Starting TreeBanditThreaded test with " << playouts << " playouts, " << trials << " trials" << std::endl;

    for (int trial = 0; trial < trials; ++trial)
    {

        using State = MoldState<4>;
        using MonteCarloModel = MonteCarloModel<State>;
        using MatrixUCB = MatrixUCB<MonteCarloModel, TreeBanditThreaded>;

        prng device(0);
        State state(10);
        MonteCarloModel model(device);
        MatrixUCB session;
        session.threads = 3;

        MatrixNode<MatrixUCB> root;

        session.run(playouts, device, state, model, root);

        typename State::Types::VectorReal row_strategy(4), col_strategy(4);
        session.get_strategies(&root, row_strategy, col_strategy);

        math::print(row_strategy, 4);
        math::print(col_strategy, 4);

        std::cout << session << std::endl;
    }

    std::cout << "Starting TreeBanditThreadPool test with " << playouts << " playouts, " << trials << " trials" << std::endl;

    for (int trial = 0; trial < trials; ++trial)
    {

        using State = MoldState<4>;
        using MonteCarloModel = MonteCarloModel<State>;
        using MatrixUCB = MatrixUCB<MonteCarloModel, TreeBanditThreadPool>;

        prng device(0);
        State state(10);
        MonteCarloModel model(device);
        MatrixUCB session;
        session.threads = 3;

        MatrixNode<MatrixUCB> root;

        session.run(playouts, device, state, model, root);

        typename State::Types::VectorReal row_strategy(4), col_strategy(4);
        session.get_strategies(&root, row_strategy, col_strategy);

        math::print(row_strategy, 4);
        math::print(col_strategy, 4);

        std::cout << session << std::endl;
    }

    return 0;
}
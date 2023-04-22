#include "surskit.hh"

int main () {

    using State = MoldState<4>;
    using MonteCarloModel = MonteCarloModel<State>;
    using Exp3p = Exp3p<MonteCarloModel, TreeBanditThreaded>;
    
    prng device(0);
    State state(10);
    MonteCarloModel model(device);
    Exp3p session;

    MatrixNode<Exp3p> root;

    session.run(1000, device, state, model, root);

    typename State::Types::VectorReal row_strategy(4), col_strategy(4);
    session.get_strategies(&root, row_strategy, col_strategy);

    math::print(row_strategy, 4);
    math::print(col_strategy, 4);

    return 0;
}
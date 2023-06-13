#include <surskit.hh>

int main () {
    prng device{};

    W::StateWrapper<RandomTree> state_w{device, 1, 3, 3, 1, 0};
    W::ModelWrapper<MonteCarloModel<RandomTree>> model_w{device};

    W::State& s{state_w};
    W::Model& m{model_w};
    s.get_actions();
    m.get_inference(s);
    
    return 0;
}
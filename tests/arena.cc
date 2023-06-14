#include <surskit.hh>

#include <unordered_map>
#include <vector>
#include <utility>

template <class _State, class _Model>
class Arena : public PerfectInfoState<SimpleTypes>
{
public:
    struct Types : PerfectInfoState<SimpleTypes>::Types
    {
    };

    prng device{};
    W::StateWrapper<_State> state;
    W::ModelWrapper<_Model> model;
    std::vector<W::Search*> searches;

    template <typename _Algorithm>
    Arena(
        const _State& state,
        const _Model& model,
        std::vector<W::SearchWrapper<_Algorithm>> &searches
        ) : state{state}, model{model}
    {
        const size_t size = searches.size();
        this->searches.resize(size);
        this->row_actions.fill(size);
        this->col_actions.fill(size);

        for (int i = 0; i < size; ++i)
        {
            this->searches[i] = &searches[i];
            this->row_actions[i] = typename Types::Action{i};
            this->col_actions[i] = typename Types::Action{i};
        }
    }

    void get_actions() {}

    void reseed(typename Types::Seed)
    {
    }

    void apply_actions(
        typename Types::Action row_action,
        typename Types::Action col_action)
    {
        this->is_terminal = true;
        this->obs = typename Types::Observation{0};
        auto row_search = searches[row_action];
        auto col_search = searches[col_action];

        auto state_copy = state.clone();
        double row_payoff = play_vs(row_search, col_search, state_copy, model);
        this->payoff = typename Types::Value{row_payoff};
    }

    double play_vs(
        W::Search* row_search,
        W::Search* col_search,
        W::State &state,
        W::Model &model
        )
    {
        state.get_actions();
        while (!state.is_terminal()) {
            row_search->run(1000, state, model);
            col_search->run(1000, state, model);
            std::vector<double> row_strategy, col_strategy;
            row_search->get_empirical_strategies(row_strategy, col_strategy);
            ActionIndex row_idx = device.sample_pdf(row_strategy);
            col_search->get_empirical_strategies(row_strategy, col_strategy);
            ActionIndex col_idx = device.sample_pdf(col_strategy);
            state.apply_actions(row_idx, col_idx);
            state.get_actions();
        }
        return state.row_payoff();
    }
};



int main()
{

    using State = Arena<RandomTree, MonteCarloModel<RandomTree>>;
    using Model = MonteCarloModel<State>;
    using Algorithm = Exp3<Model, TreeBandit>;
    std::vector<W::SearchWrapper<Exp3<MonteCarloModel<RandomTree>, TreeBandit>>> agents = {
        {.001}, {.005}, {.01}, {.05}, {.1}, {.2}, {.5}
    };

    prng device{0};
    W::ModelWrapper<MonteCarloModel<RandomTree>> model{device};

    // RandomTreeGenerator generator{0, {1, 2}, {2, 3, 4}, {1}, {0.0}, 10};
    RandomTreeGenerator generator{0, {20}, {4}, {1}, {0.0}, 1};

    for (RandomTree&& random_tree : generator) {
        
        State arena{random_tree, model, agents};

        Model arena_model{1337};
        Algorithm session{};
        MatrixNode<Algorithm> root;

        session.run(1000, device, arena, arena_model, root);
        State::Types::VectorReal row_strategy, col_strategy;
        session.get_empirical_strategies(&root, row_strategy, col_strategy);
        math::print(row_strategy);
        math::print(col_strategy);
    }

    return 0;
}
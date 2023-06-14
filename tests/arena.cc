#include <surskit.hh>

#include <unordered_map>
#include <vector>
#include <utility>

template <class _State, class _Model>
class Arena : PerfectInfoState<SimpleTypes>
{
public:
    struct Types : SimpleTypes
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
            std::cout << '!' << std::endl;

            row_search->run(1000, state, model);
            col_search->run(1000, state, model);
            auto row_strategy = row_search->row_strategy();
            auto col_strategy = row_search->col_strategy();
            ActionIndex row_idx = device.sample_pdf(row_strategy);
            ActionIndex col_idx = device.sample_pdf(col_strategy);
            state.apply_actions(row_idx, col_idx);
            state.get_actions();
        }
        return state.row_payoff();
    }
};

using State = RandomTree;
using Model = MonteCarloModel<RandomTree>;
using Algorithm = Exp3<Model, TreeBandit>;

std::vector<W::SearchWrapper<Algorithm>> agents = {
    {.001}, {.005}, {.01}, {.05}, {.1}, {.2}, {.5}
};

int main()
{
    prng device{0};
    W::ModelWrapper<MonteCarloModel<RandomTree>> model{device};

    // RandomTreeGenerator generator{0, {1, 2}, {2, 3, 4}, {1}, {0.0}, 10};
    RandomTreeGenerator generator{0, {2}, {2}, {1}, {0.0}, 1};

    for (RandomTree&& random_tree : generator) {
        
        Arena<RandomTree, MonteCarloModel<RandomTree>> arena{random_tree, model, agents};
        arena.apply_actions(RandomTree::Types::Action{0}, RandomTree::Types::Action{1});

    }

    //     State state{10};
    //     Model model{0};

    //     Search<State, MonteCarloModel, Exp3, TreeBandit> s0{state, model};
    //     // TODO:
    //     /*
    //     if yuo have a TraversedState<> ptr member then your State must pass static_assertion about being ChanceState
    //     */

    //    Arena<State, Model> arena{std::vector<S*>{&s0}};

    //    arena.apply_actions(SimpleTypes::Action{0}, SimpleTypes::Action{0});

    return 0;
}
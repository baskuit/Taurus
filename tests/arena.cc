#include <surskit.hh>
    using namespace W;

class Arena : PerfectInfoState<SimpleTypes>
{
public:
    struct Types : SimpleTypes
    {
    };


    std::vector<Search *> agents{};

    Arena(std::vector<Search *> &agents) : agents{agents} {}

    template <typename T>
    Arena(std::vector<T> &agents_arg)
    {
        // agents = std::vector<Search*>{};
        for (T &agent : agents_arg)
        {
            agents.push_back(&agent);
        }
    }

    void get_actions()
    {
        const size_t num_agents = agents.size();
        this->row_actions.fill(num_agents);
        this->col_actions.fill(num_agents);
        for (int i = 0; i < num_agents; ++i)
        {
            this->row_actions[i] = typename Types::Action{i};
            this->col_actions[i] = typename Types::Action{i};
        }
    }

    void apply_actions(
        typename Types::Action row_action,
        typename Types::Action col_action)
    {
        Search *row_agent = agents[row_action];
        Search *col_agent = agents[col_action];

        
    }
};

// template <typename T>
// struct EEE {};

int main()
{
    // W::SearchWrapper<int, EEE> x{};
    // prng device{0};

    // std::vector<SearchT<RandomTree, MonteCarloModel, Exp3, TreeBandit>> agents = {
    //     SearchT<RandomTree, MonteCarloModel, Exp3, TreeBandit>{.001},
    //     SearchT<RandomTree, MonteCarloModel, Exp3, TreeBandit>{.005},
    //     SearchT<RandomTree, MonteCarloModel, Exp3, TreeBandit>{.01},
    //     SearchT<RandomTree, MonteCarloModel, Exp3, TreeBandit>{.05},
    //     SearchT<RandomTree, MonteCarloModel, Exp3, TreeBandit>{.1},
    //     SearchT<RandomTree, MonteCarloModel, Exp3, TreeBandit>{.2}};

    // RandomTree tree{device, 20, 3, 3, 1, 0};
    // WrapperT<RandomTree> tree_wrapper{tree};

    // MonteCarloModel<RandomTree> model{device};
    // WrapperT<MonteCarloModel<RandomTree>> model_wrapper{model};

    // Arena state{&tree_wrapper, &model_wrapper, agents};
    // state.apply_actions(Arena::Types::Action{0}, Arena::Types::Action{0});

    return 0;
}
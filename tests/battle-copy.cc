#include <surskit.hh>

#include "battle-surskit.hh"

int main()
{
    using Battle = Battle<64>;
    const int copies = 1000000;

    Battle battle(engine::RBY::miedon, engine::RBY::miedon);

    for (int copy = 0; copy < copies; ++copy)
    {
        Battle battle_ = battle;
        battle_.get_actions();
        const size_t x = battle_.row_actions.size() + battle_.col_actions.size();
        battle_.apply_actions(battle_.row_actions[0], battle_.col_actions[0]);
    }

    return 0;
}
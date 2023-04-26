#include "engine-wrapper.hpp"
#include "eval.hpp"

int main(int argc, char **argv)
{
   // full scope resolution for sake of illustration
   // using namespace engine::RBY;
   
   auto battle = engine::RBY::miedon_zapjynx;
   pkmn_result result;
   pkmn_choice c1;
   pkmn_choice c2;

   std::cout << heuristic::battle_eval(battle.battle_) << std::endl;
   battle.init();

   while(!pkmn_result_type(battle.result))
   {
      battle.c1 = battle.choose_p1();
      battle.c2 = battle.choose_p2();
      battle.result = battle.play_turn();
      std::cout << heuristic::battle_eval(battle.battle_) << std::endl;
   }
   
   engine::RBY::print_result(battle.battle_, battle.result);

   return 0;
}
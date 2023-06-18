#pragma once

#include <surskit.hh>
#include "engine-wrapper.hpp"

template <size_t MaxLog>
class Battle : public PerfectInfoState<BattleTypes<MaxLog>>
{
public:
   struct Types : PerfectInfoState<BattleTypes<MaxLog>>::Types
   {
   };

   typename Types::PRNG device;
   pkmn_gen1_battle battle_{};
   typename Types::Seed seed = 0;
   pkmn_psrng random = {};
   pkmn_result result = PKMN_RESULT_NONE;
   std::array<pkmn_choice, 9> options{0};

   Battle(const engine::RBY::Side<engine::Gen::RBY> &side1, const engine::RBY::Side<engine::Gen::RBY> &side2)
   {
      auto it_s1 = side1.cbegin();
      auto it_e1 = side1.cend();
      auto it_s2 = side2.cbegin();
      auto it_b = std::begin(battle_.bytes);
      for (; it_s1 != it_e1; ++it_s1, ++it_s2, ++it_b)
      {
         *it_b = *it_s1;
         *(it_b + 184) = *it_s2;
      }
      battle_.bytes[368] = 0;
      battle_.bytes[369] = 0;
      battle_.bytes[370] = 0;
      battle_.bytes[371] = 0;
      battle_.bytes[372] = 0;
      battle_.bytes[373] = 0;
      battle_.bytes[374] = 0;
      battle_.bytes[375] = 0;
      for (int i = 0; i < 8; ++i)
      {
         battle_.bytes[376 + i] = seed >> 8 * i;
      }
      pkmn_psrng_init(&random, seed);
      this->prob = true;
   }

   Battle(const Battle &t) : battle_(t.battle_), random(t.random), result(t.result) // TODO
   {
      this->row_actions = t.row_actions;
      this->col_actions = t.col_actions;
   }

    void reseed(typename Types::PRNG &device)
    {
      engine::RBY::set_seed(battle_, device.uniform_64());
    }

   void get_actions()
   { // TODO must be much slower?
      const size_t rows = pkmn_gen1_battle_choices(&battle_, PKMN_PLAYER_P1, pkmn_result_p1(result), this->row_actions.template data<pkmn_choice, A<9>::Array>(), PKMN_CHOICES_SIZE);
      const size_t cols = pkmn_gen1_battle_choices(&battle_, PKMN_PLAYER_P2, pkmn_result_p2(result), this->col_actions.template data<pkmn_choice, A<9>::Array>(), PKMN_CHOICES_SIZE);
      this->row_actions.fill(rows);
      this->col_actions.fill(cols);

      // std::array<pkmn_choice, 9> options;
      // this->row_actions.clear();
      // this->col_actions.clear();
      // const size_t rows = pkmn_gen1_battle_choices(&battle_, PKMN_PLAYER_P1, pkmn_result_p1(result), options.data(), PKMN_CHOICES_SIZE);
      // this->row_actions.insert(this->row_actions.begin(), options.begin(), options.begin() + rows);
      // const size_t cols = pkmn_gen1_battle_choices(&battle_, PKMN_PLAYER_P2, pkmn_result_p2(result), options.data(), PKMN_CHOICES_SIZE);
      // this->col_actions.insert(this->col_actions.begin(), options.begin(), options.begin() + cols);
   }

   void apply_actions(
       typename Types::Action row_action,
       typename Types::Action col_action)
   {
      result = pkmn_gen1_battle_update(
         &battle_, 
         static_cast<pkmn_choice>(row_action), 
         static_cast<pkmn_choice>(col_action), 
         this->obs.template data<uint8_t, MaxLog>(), 
         MaxLog);
      // this->prob = true;

      const pkmn_result_kind r = pkmn_result_type(result);
      // const int x = (r == PKMN_RESULT_WIN);
      // const int y = (r == PKMN_RESULT_LOSE);
      // const int z = 1 - x * y;
      // this->row_payoff = Rational(x + z, 1 + z);
      // this->col_payoff = Rational(y + z, 1 + z);

      if (r)
      {
         this->is_terminal = true;
         if (r == PKMN_RESULT_WIN)
         {
            this->payoff = typename Types::Value{1};
         }
         else if (r == PKMN_RESULT_LOSE)
         {
            this->payoff = typename Types::Value{0};
         }
         else
         {
            this->payoff = typename Types::Value{.5}; //TODO!!!!
         }
      }
   }
};

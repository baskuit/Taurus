#pragma once

#include "surskit.hh"
#include "engine-wrapper.hpp"

template <size_t MaxActions, size_t MaxTrace>
class BattleSurskit : public StateArray<MaxActions, pkmn_choice, std::array<uint8_t, MaxTrace>, bool>
{
public:
   struct Types : StateArray<MaxActions, pkmn_choice, std::array<uint8_t, MaxTrace>, bool>::Types
   {
   };

   prng device;
   pkmn_gen1_battle battle_{};
   std::uint64_t seed = 0;
   pkmn_psrng random = {};
   pkmn_result result = PKMN_RESULT_NONE;
   std::array<pkmn_choice, 9> options{0};

   BattleSurskit(const engine::RBY::Side<engine::Gen::RBY> &side1, const engine::RBY::Side<engine::Gen::RBY> &side2)
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
   }

   BattleSurskit(const BattleSurskit &t) : battle_(t.battle_), seed(device.uniform_64()), random(t.random), result(t.result)
   {
      this->actions = t.actions;
      this->transition = t.transition;
      engine::RBY::set_seed(battle_, seed);
      pkmn_psrng_init(&random, seed); // TODO Is this necessary?
   }

   void get_actions()
   {
      this->actions.rows = pkmn_gen1_battle_choices(&battle_, PKMN_PLAYER_P1, pkmn_result_p1(result), this->actions.row_actions.data(), PKMN_OPTIONS_SIZE);
      this->actions.cols = pkmn_gen1_battle_choices(&battle_, PKMN_PLAYER_P2, pkmn_result_p2(result), this->actions.col_actions.data(), PKMN_OPTIONS_SIZE);
   }

   void apply_actions(
       typename Types::Action row_action,
       typename Types::Action col_action)
   {
      result = pkmn_gen1_battle_update(&battle_, row_action, col_action, this->transition.obs.data(), MaxTrace);
      this->transition.prob = true;

      const pkmn_result_kind r = pkmn_result_type(result);
      if (r)
      {
         this->is_terminal = true;
         if (r == PKMN_RESULT_WIN)
         {
            this->row_payoff = 1;
            this->col_payoff = 0;
         }
         else if (r == PKMN_RESULT_LOSE)
         {
            this->row_payoff = 0;
            this->col_payoff = 1;
         }
         else
         {
            this->row_payoff = .5;
            this->col_payoff = .5;
         }
      }
   }
};

template <size_t MaxTrace>
class BattleSurskitVector : public StateVector<pkmn_choice, std::array<uint8_t, MaxTrace>, bool>
{
public:
   struct Types : StateVector<pkmn_choice, std::array<uint8_t, MaxTrace>, bool>::Types
   {
   };

   prng device;
   pkmn_gen1_battle battle_{};
   std::uint64_t seed = 0;
   pkmn_psrng random = {};
   pkmn_result result = PKMN_RESULT_NONE;
   std::array<pkmn_choice, 9> options{0};

   BattleSurskitVector(const engine::RBY::Side<engine::Gen::RBY> &side1, const engine::RBY::Side<engine::Gen::RBY> &side2)
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
   }

   BattleSurskitVector(const BattleSurskitVector &t) : battle_(t.battle_), seed(device.uniform_64()), random(t.random), result(t.result)
   {
      this->actions = t.actions;
      this->transition = t.transition;
      engine::RBY::set_seed(battle_, seed);
      pkmn_psrng_init(&random, seed);
   }

   void get_actions()
   {
      std::array<pkmn_choice, 9> options;
      this->actions.rows = pkmn_gen1_battle_choices(&battle_, PKMN_PLAYER_P1, pkmn_result_p1(result), options.data(), PKMN_OPTIONS_SIZE);
      std::copy_n(options.begin(), this->actions.rows, std::back_inserter(this->actions.row_actions));
      this->actions.cols = pkmn_gen1_battle_choices(&battle_, PKMN_PLAYER_P2, pkmn_result_p2(result), options.data(), PKMN_OPTIONS_SIZE);
      std::copy_n(options.begin(), this->actions.cols, std::back_inserter(this->actions.col_actions));
   }

   void apply_actions(
       typename Types::Action row_action,
       typename Types::Action col_action)
   {
      result = pkmn_gen1_battle_update(&battle_, row_action, col_action, this->transition.obs.data(), MaxTrace);
      this->transition.prob = true;

      const pkmn_result_kind r = pkmn_result_type(result);
      if (r)
      {
         this->is_terminal = true;
         if (r == PKMN_RESULT_WIN)
         {
            this->row_payoff = 1;
            this->col_payoff = 0;
         }
         else if (r == PKMN_RESULT_LOSE)
         {
            this->row_payoff = 0;
            this->col_payoff = 1;
         }
         else
         {
            this->row_payoff = .5;
            this->col_payoff = .5;
         }
      }
   }
};

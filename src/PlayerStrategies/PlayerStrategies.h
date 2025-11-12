#pragma once
#include "Map/Map.h"
#include "Player/Player.h"
#include <algorithm>
#include <vector>

// Forward declaration
class PlayerStrategy;
class Deck;

// Abstract base class for player strategies
class PlayerStrategy {
  public:
    virtual ~PlayerStrategy() = default;
    virtual void issueOrder(Player* player, Deck* deck) = 0;
    virtual std::vector<Territory*> toDefend(Player* player) = 0;
    virtual std::vector<Territory*> toAttack(Player* player) = 0;
};

// Human player strategy - requires user interaction
class HumanPlayerStrategy : public PlayerStrategy {
  public:
    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*> toDefend(Player* player) override;
    std::vector<Territory*> toAttack(Player* player) override;
};

// Aggressive player strategy - deploy to strongest, then always attack
class AggressivePlayerStrategy : public PlayerStrategy {
  public:
    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*> toDefend(Player* player) override;
    std::vector<Territory*> toAttack(Player* player) override;
};

// Benevolent player strategy - deploy/advance to weakest territories
class BenevolentPlayerStrategy : public PlayerStrategy {
  public:
    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*> toDefend(Player* player) override;
    std::vector<Territory*> toAttack(Player* player) override;
};

// Neutral player strategy - never issues orders - aggressive if attacked
class NeutralPlayerStrategy : public PlayerStrategy {
  public:
    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*> toDefend(Player* player) override;
    std::vector<Territory*> toAttack(Player* player) override;
};

// Cheater player strategy - conquers all adjacent territories
class CheaterPlayerStrategy : public PlayerStrategy {
  public:
    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*> toDefend(Player* player) override;
    std::vector<Territory*> toAttack(Player* player) override;
};
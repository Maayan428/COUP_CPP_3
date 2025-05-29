//maayan428@gmail.com

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <stdexcept>

class Game; //Allows me to use pointers to Game objects in the Player class.

class Player {
protected:
    std::string name;
    int coins;
    bool active;

public:
    Player(const std::string& name);
    virtual ~Player() = default;

    std::string getName() const;
    int getCoins() const;
    bool isActive() const;

    virtual std::string getRole() const = 0; //Varies betweem roles.

    virtual void gather(Game& game);               // Recieve 1 coin
    virtual void tax(Game& game);                  // Recieve 2 coins
    virtual void bribe(Game& game);                // Pay 4, get another turn
    virtual void arrest(Player& target, Game& game); // steal 1 coin from another player
    virtual void sanction(Player& target, Game& game); // Pay 3 and restrict another player to: gather/tax
    virtual void coup(Player& target, Game& game);    // Pay 7 and eliminate another player

    void addCoins(int amount);
    void removeCoins(int amount);
    void eliminate();

    //Special abilities:
    virtual bool canBlockTax() const { return false; }
    virtual bool canBlockBribe() const { return false; }
    virtual bool canBlockCoup() const { return false; }

    // Optional abilities for specific roles
    virtual void invest(Game& game); // For Baron
};

#endif // PLAYER_HPP
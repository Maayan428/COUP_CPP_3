#ifndef ROLE_HPP
#define ROLE_HPP

#include "Player.hpp"

class Governor : public Player {
public:
    Governor(const std::string& name);
    std::string getRole() const override;
    bool canBlockTax() const override;
};

class Spy : public Player {
public:
    Spy(const std::string& name);
    std::string getRole() const override;
    void arrest(Player& target, Game& game) override;
};

class Baron : public Player {
public:
    Baron(const std::string& name);
    std::string getRole() const override;
    void invest(Game& game) override;
};

class General : public Player {
public:
    General(const std::string& name);
    std::string getRole() const override;
    bool canBlockCoup() const override;
    void arrest(Player& target, Game& game) override;
};

class Judge : public Player {
public:
    Judge(const std::string& name);
    std::string getRole() const override;
    bool canBlockBribe() const override;
};

class Merchant : public Player {
public:
    Merchant(const std::string& name);
    std::string getRole() const override;
    void gather(Game& game) override;
    void arrest(Player& target, Game& game) override;
};

#endif // ROLE_HPP
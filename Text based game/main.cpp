#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include "Random.h"

class Creature
{
private:
	std::string m_name{};
	char m_symbol{};
	int m_health{};
	int m_dmgPerAttack{};
	int m_gold{};
	
public:
	Creature(std::string_view name = "?", char symbol ='?', int health = 0, int dps = 0, int gold = 0) 
		: m_name{name}, m_symbol{symbol}, m_health{health}, m_dmgPerAttack{dps}, m_gold{gold}
	{
	}

	int getGold() const { return m_gold; }
	int getDPS() const { return m_dmgPerAttack; }
	int getHealth() const { return m_health; }
	char getSymbol() const { return m_symbol; }
	std::string_view getName() const { return m_name; }
	void updateHealth(int x) { m_health -= x; }
	bool isDead() { return (m_health <= 0); }
	void updateGold(int x) { m_gold += x; }
	void updateDPS(int x) { m_dmgPerAttack += x; }
};

class Player : public Creature
{
private:
	int m_level{};
public:
	Player(std::string_view name = "?", int level = 1)
		: Creature(name, '@', 10, 1, 0), m_level{ level }
	{
	}

	void levelUp() { ++m_level; Creature::updateDPS(1); }
	int getLevel() const { return m_level; }
	bool hasWon() { return ((m_level >= 20) || (this->getHealth() <= 0)); }

	void attack(Creature& monster)
	{
		std::cout << "You hit the " << monster.getName() << " for " << this->getDPS() << " damage!\n";
		monster.updateHealth(this->getDPS());
		if (monster.getHealth() <= 0)
		{
			this->levelUp();
			this->updateGold(monster.getGold());
			std::cout << "You found " << monster.getGold() << " gold.\n";
		}
	}

	bool fight(Creature& monster)
	{
		if ((monster.getHealth() > 0) && (this->getHealth() > 0))
		{
			char temp{};
			std::cout << "Press f to fight or anything else to run away: ";
			std::cin >> temp;
			if (temp == 'f')
			{
				return true;
			}
			return false;
		}
		return false;
	}
};

class Monster : public Creature
{
public:
	enum Type
	{
		dragon,
		orc,
		slime,

		max_types
	};

	Monster(Type type)
		:Creature{ m_data[type] }
	{
	}

	static Monster getRandomMonster()
	{
		return Monster(static_cast<Type>(Random::get(0, 2)));
	}

	void attack(Creature& player)
	{
		std::cout << "The " << this->getName() << " hit you for " << this->getDPS() << " damage.\n";
		player.updateHealth(this->getDPS());
	}
private:
	inline static std::array<Creature, Type::max_types> m_data
	{ {
	{ "dragon", 'D', 20, 4, 100 },
	{ "orc", 'o', 4, 2, 25 },
	{ "slime", 's', 1, 1, 10 }
	} };

	static_assert(std::size(m_data) == max_types);
};

class Potion
{
public:
	enum Type : std::size_t
	{
		small_health,
		med_health,
		large_health,
		small_strength,
		med_strength,
		large_strength,
		poison,

		max_potion,
	};

	Potion(std::size_t x)
		: m_potionValue{potionValue[x]}, m_potionName{potionName[x]}, m_potionType {potionType[x]}
	{
	}

	static Potion getRandomPotion()
	{
		return Potion(static_cast<std::size_t>(Random::get(0, 6)));
	}

	std::string getPotionName() const { return m_potionName; }
	int getPotionValue() const { return m_potionValue; }
	Type getPotionType() const { return m_potionType; }

private:
	int m_potionValue{};
	std::string m_potionName{};
	Type m_potionType{};
	inline static std::array<int, max_potion> potionValue{ 2,5,10,1,3,5,-1 };
	inline static std::array<std::string_view, max_potion> potionName{ "small health", "medium health", "large health potion", "small strength", "medium strength", "large strength", "poison" };
	inline static std::array<Type, max_potion> potionType{ small_health,med_health,large_health,small_strength,med_strength,large_strength,poison };
};

bool flee()
{
	return Random::get(0, 1);
}

void itemDrop(Creature& player)
{
	int i{ Random::get(0,2) };
	if (i == 0)
	{
		std::cout << "You found a mythical potion! Do you want to drink it? [y/n]: ";
		char temp{};
		std::cin >> temp;
		if (temp == 'y')
		{
			Potion p{ Potion::getRandomPotion() };
			std::cout << "You drank a " << p.getPotionName() << " potion.\n";
			switch (p.getPotionType())
			{
			case(Potion::small_health):
			{
				player.updateHealth(-p.getPotionValue());
				return;
			}
			case(Potion::med_health):
			{
				player.updateHealth(-p.getPotionValue());
				return;
			}
			case(Potion::large_health):
			{
				player.updateHealth(-p.getPotionValue());
				return;
			}
			case(Potion::small_strength):
			{
				player.updateDPS(p.getPotionValue());
				return;
			}
			case(Potion::med_strength):
			{
				player.updateDPS(p.getPotionValue());
				return;
			}
			case(Potion::large_strength):
			{
				player.updateDPS(p.getPotionValue());
				return;
			}
			case(Potion::poison):
			{
				player.updateHealth(p.getPotionValue());
				return;
			}
			default:
				return;
			}
		}
		return;
	}
	return;
}

int main()
{
	std::cout << "Enter your name: ";
	std::string name{};
	std::cin >> name;
	Player p{ name };

	while (!p.hasWon())
	{
		Monster m{ Monster::getRandomMonster() };
		std::cout << "You have encountered a " << m.getName() << '(' << m.getSymbol() << ")\n";
		while ((m.getHealth() > 0) && (p.getHealth() > 0))
		{
			if (p.fight(m))
			{
				p.attack(m);
				if (m.getHealth() <= 0) 
				{
					std::cout << "You killed the " << m.getName() << ".\n";
					std::cout << "You are now level " << p.getLevel() << ".\n";
					itemDrop(p);
					break;
				}
				m.attack(p);
			}
			else
			{
				if (flee())
				{
					m = Monster::getRandomMonster();
					break;
				}
				else
				{
					std::cout << "You failed to escape!!!\n";
					m.attack(p);
				}
			}
		}
	}
	if ((p.getHealth() > 0))
	{
		std::cout << "Congrats you beat the dungeon!!! And you have taken " << p.getGold() << " gold!";
	}
	else
	{
		std::cout << "You died at level " << p.getLevel() << " and with " << p.getGold() << " gold.\nToo bad you can't take it with you!!!\n";
	}
	return 0;
}

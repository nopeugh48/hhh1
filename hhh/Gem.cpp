#include "Gem.h"

Gem::Gem() : _name(""), _price(0.0), _weight(0.0) {}
Gem::Gem(const std::string& name, double price, double weight)
    : _name(name), _price(price), _weight(weight) {
}

std::string Gem::getName() const { return _name; }
void Gem::setName(const std::string& name) { _name = name; }
double Gem::getPrice() const { return _price; }
void Gem::setPrice(double price) { _price = price; }
double Gem::getWeight() const { return _weight; }
void Gem::setWeight(double weight) { _weight = weight; }

PreciousGem::PreciousGem() {}
PreciousGem::PreciousGem(const std::string& name, double price, double weight)
    : Gem(name, price, weight) {
}

SemiPreciousGem::SemiPreciousGem() {}
SemiPreciousGem::SemiPreciousGem(const std::string& name, double price, double weight)
    : Gem(name, price, weight) {
}

Diamond::Diamond(const std::string& name, double price, double weight)
    : PreciousGem(name, price, weight) {
}
std::string Diamond::typeName() const { return "Diamond"; }
std::unique_ptr<Gem> Diamond::clone() const { return std::make_unique<Diamond>(*this); }

Ruby::Ruby(const std::string& name, double price, double weight)
    : PreciousGem(name, price, weight) {
}
std::string Ruby::typeName() const { return "Ruby"; }
std::unique_ptr<Gem> Ruby::clone() const { return std::make_unique<Ruby>(*this); }

Amethyst::Amethyst(const std::string& name, double price, double weight)
    : SemiPreciousGem(name, price, weight) {
}
std::string Amethyst::typeName() const { return "Amethyst"; }
std::unique_ptr<Gem> Amethyst::clone() const { return std::make_unique<Amethyst>(*this); }

Topaz::Topaz(const std::string& name, double price, double weight)
    : SemiPreciousGem(name, price, weight) {
}
std::string Topaz::typeName() const { return "Topaz"; }
std::unique_ptr<Gem> Topaz::clone() const { return std::make_unique<Topaz>(*this); }

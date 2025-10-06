#include "Necklace.h"

void Necklace::addGem(const std::shared_ptr<Gem>& gem) { _gems.push_back(gem); }
void Necklace::removeAt(size_t idx) { if (idx < _gems.size()) _gems.erase(_gems.begin() + idx); }
void Necklace::updateAt(size_t idx, const std::shared_ptr<Gem>& gem) { if (idx < _gems.size()) _gems[idx] = gem; }
size_t Necklace::count() const { return _gems.size(); }
std::shared_ptr<Gem> Necklace::getAt(size_t idx) const { return (idx < _gems.size()) ? _gems[idx] : nullptr; }
const std::vector<std::shared_ptr<Gem>>& Necklace::all() const { return _gems; }

double DivisionCalculator::totalWeight(const Necklace& neck) const {
    double total = 0;
    for (auto& g : neck.all()) total += g->getWeight();
    return total;
}
double DivisionCalculator::totalPrice(const Necklace& neck) const {
    double total = 0;
    for (auto& g : neck.all()) total += g->getPrice();
    return total;
}

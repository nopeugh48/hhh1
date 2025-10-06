#pragma once
#include "Gem.h"
#include <vector>
#include <memory>

class Necklace {
private:
    std::vector<std::shared_ptr<Gem>> _gems;
public:
    void addGem(const std::shared_ptr<Gem>& gem);
    void removeAt(size_t idx);
    void updateAt(size_t idx, const std::shared_ptr<Gem>& gem);
    size_t count() const;
    std::shared_ptr<Gem> getAt(size_t idx) const;
    const std::vector<std::shared_ptr<Gem>>& all() const;
};

class DivisionCalculator {
public:
    double totalWeight(const Necklace& neck) const;
    double totalPrice(const Necklace& neck) const;
};


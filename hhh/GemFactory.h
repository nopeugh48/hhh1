#pragma once
#include "Gem.h"
#include <memory>
#include <string>

class GemFactory {
public:
    static std::shared_ptr<Gem> createGem(const std::string& type, const std::string& name, double price, double weight);
};

#include "GemFactory.h"
#include <stdexcept>

std::shared_ptr<Gem> GemFactory::createGem(const std::string& type, const std::string& name, double price, double weight) {
    if (type == "Diamond") return std::make_shared<Diamond>(name, price, weight);
    if (type == "Ruby") return std::make_shared<Ruby>(name, price, weight);
    if (type == "Amethyst") return std::make_shared<Amethyst>(name, price, weight);
    if (type == "Topaz") return std::make_shared<Topaz>(name, price, weight);
    throw std::invalid_argument("Unknown gem type: " + type);
}

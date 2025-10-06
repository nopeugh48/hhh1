#pragma once
#include <string>
#include <memory>

class Gem {
protected:
    std::string _name;
    double _price;
    double _weight;
public:
    Gem();
    Gem(const std::string& name, double price, double weight);
    virtual ~Gem() = default;

    std::string getName() const;
    void setName(const std::string& name);
    double getPrice() const;
    void setPrice(double price);
    double getWeight() const;
    void setWeight(double weight);

    virtual std::string typeName() const = 0;
    virtual std::unique_ptr<Gem> clone() const = 0;
};

class PreciousGem : public Gem {
public:
    PreciousGem();
    PreciousGem(const std::string& name, double price, double weight);
};

class SemiPreciousGem : public Gem {
public:
    SemiPreciousGem();
    SemiPreciousGem(const std::string& name, double price, double weight);
};

// Конкретные
class Diamond : public PreciousGem {
public:
    Diamond(const std::string& name, double price, double weight);
    std::string typeName() const override;
    std::unique_ptr<Gem> clone() const override;
};

class Ruby : public PreciousGem {
public:
    Ruby(const std::string& name, double price, double weight);
    std::string typeName() const override;
    std::unique_ptr<Gem> clone() const override;
};

class Amethyst : public SemiPreciousGem {
public:
    Amethyst(const std::string& name, double price, double weight);
    std::string typeName() const override;
    std::unique_ptr<Gem> clone() const override;
};

class Topaz : public SemiPreciousGem {
public:
    Topaz(const std::string& name, double price, double weight);
    std::string typeName() const override;
    std::unique_ptr<Gem> clone() const override;
};

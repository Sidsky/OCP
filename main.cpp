#include <iostream>
#include <vector>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

// There is a product and a product has a color and size. The goal is to implement a system that can filter
// products based on different attributes. Keep in mind that the product can have extra functionalities in the future
// so the filtering criteria will also need to be modified/extended. Apply OCP design to achieve the solution.

enum class Color {red, yellow, black};
enum class Size {small, medium, large};

template <typename T>
std::ostream & operator <<(typename std::enable_if<std::is_enum<T>::value, std::ostream>::type& stream, const T& e)
{
    return stream << static_cast<typename std::underlying_type<T>::type>(e);
}

struct Product {
    string name;
    Color color;
    Size size;

    Product(const string &name, Color color, Size size) : name(name), color(color), size(size) {}
};

template <typename T>
struct AndSpecification;

template <typename T>
struct Specification
{
    virtual bool is_satisfied (T* item) = 0;

    AndSpecification<T> operator && (Specification<T> && other){
        return AndSpecification<T>(*this, other);
    }
    virtual ~Specification() = default;
};

template <typename T>
struct Filter
{
    virtual vector<T*> filter (vector<T*> items, Specification<T>& spec) = 0;
};

struct ColorSpecification : Specification<Product>
{
    Color color;

    explicit ColorSpecification(Color color) : color(color) {}

    bool is_satisfied(Product *item) override {
        return item ->color == color;
    }
};

struct SizeSpecification : Specification<Product>
{
    Size size;

    explicit SizeSpecification(Size size) : size(size) {}

    bool is_satisfied(Product *item) override {
        return item -> size == size;
    }
};

template <typename T>
struct AndSpecification : Specification<T>
{
    Specification<T>& first;
    Specification<T>& second;

    AndSpecification(Specification<T> &first, Specification<T> &second) : first(first), second(second) {}

    bool is_satisfied(T *item) override {
        return first.is_satisfied(item) && second.is_satisfied(item);
    }
};

struct BetterFilter : Filter<Product>
{
    vector<Product *> filter (vector<Product *> items, Specification<Product> &spec) override {

        vector<Product*> result;

        for (auto& i : items) {
            if (spec.is_satisfied(i)){
                result.push_back(i);
            }
        }

        return result;
    }
};

int main() {
    Product p1{"Apple", Color::red, Size::small};
    Product p2{"Tiger", Color::black, Size::small};
    Product p3{"Spotify", Color::black, Size::large};

    vector<Product*> items;
    items.emplace_back(&p1);
    items.emplace_back(&p2);
    items.emplace_back(&p3);

    BetterFilter bf;

    auto specification = ColorSpecification (Color::black) && SizeSpecification (Size::small);

    for (auto& i: bf.filter(items, specification)) {
        std::cout << i->name << " is small and black \n";
    }
    return 0;
}

#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <vector>

class Line
{
public:
    struct Iterator
    {
    public:
        struct Coord
        {
            Coord(int x, int y, double z) :
                x_(x),
                y_(y),
                z_(z)
            {
            }
            int x_;
            int y_;
            double z_;
            friend Iterator;
        private:
            Coord() :
                x_(),
                y_(),
                z_()
            {
            }
        };
        using iterator_category = std::forward_iterator_tag;
        using diference_type = std::ptrdiff_t;
        using value_type = Coord;
        using pointer = Coord*;
        using reference = Coord&;
        Iterator(
            int x,
            int y,
            double z,
            bool p,
            bool adxgady,
            int i1,
            int g,
            int i2,
            double dz);
        value_type operator*() const { return coord_; }
        pointer operator->() { return &coord_; }
        Iterator operator++();
        Iterator operator++(int)
        {
            Iterator t = *this;
            ++(*this);
            return t;
        }
        friend bool operator==(const Iterator& a, const Iterator& b)
        {
            return a.x_ == b.x_ && a.y_ == b.y_;
        }
        friend bool operator!=(const Iterator& a, const Iterator& b) { return !(a == b); }
        friend Line;
    private:
        Iterator();
        int x_;
        int y_;
        double z_;
        bool p_;
        bool adxgady_;
        int i1_;
        int g_;
        int i2_;
        double dz_;
        value_type coord_;
    };

    Line(int x1, int y1, double z1, int x2, int y2, double z2);

    Iterator begin() const { return begin_; }
    Iterator end() const { return end_; }

private:
    Iterator begin_;
    Iterator end_;
};


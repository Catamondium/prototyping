#pragma once
#include <curses.h>
#include <cmath>
#include <iostream>
#include "iterable_queue.hpp"

enum snakestate
{
    NIL,
    EATEN,
    TAIL,
    WALL,
};

#define DEBUG
struct vec
{
    int x;
    int y;
    vec() = default;
    vec(int x, int y) : x(x), y(y){};
    vec operator+(vec) const;
    bool operator==(vec o) const
    {
        return this->x == o.x && this->y == o.y;
    };

    bool operator!=(vec o) const
    {
        return !(*this == o);
    }

    vec operator-() const
    {
        return {-x, -y};
    };

#ifdef DEBUG
    operator std::string()
    {
        return "P(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    };
#endif
};

vec spawn(int width, int height)
{
    return {(int)std::floor(rand() % width),
            (int)std::floor(rand() % height)};
}

vec vec::operator+(vec other) const
{
    return {this->x + other.x, this->y + other.y};
}

class snake
{
    static constexpr char ch = 'O';
    iterable_queue<vec> body;
    vec vel;
    inline vec &head() { return body.back(); }

public:
    snake() = default;
    snake(vec v)
    {
        body.push(v);
    };

    void dir(int x, int y)
    {
        vec d = {x, y};
#ifndef DEBUG
        if (vel == -d)
            return;
#endif
        vel = d;
    };

    void update(vec &fruit, int width, int height)
    {
        if (body.size() == 0)
            return;

        if (fruit == head())
        {
            body.push(head() + vel);
            fruit = spawn(width, height);
        }

        if (vel != vec(0, 0))
        {
            vec h = head() + vel;
            h.x %= width;
            h.y %= height;
            body.pop();
            body.push(h);
        }

        return;
    }

    void draw(int, int)
    {
        mvaddstr(0, 0, ("SCORE: " + std::to_string(body.size() - 1)).c_str());

#ifdef DEBUG
        mvaddstr(3, 0, std::string(head()).c_str());
#endif

        for (vec &p : body)
        {
            mvaddch(p.y, p.x, ch);
        }
    };
};
#include <array>
#include <curses.h>
#include <algorithm>

#include "hanoi.hpp"
#include "../cursesgame.hpp"

constexpr int odd(int n)
{
    return (2 * n) + 1;
}

std::string drawTower(Tower t)
{
    if (t.empty())
        return "";

    int max = *std::max_element(t.begin(), t.end());
    std::string out;
    for (auto it = t.rbegin(); it != t.rend(); ++it)
    {
        int spaces = (max - *it) / 2;
        out += std::string(spaces, ' ') + std::string(*it, CursesHanoi::ch);

        if (it != t.rend())
            out += "\n";
    }

    return out;
}

struct CursesHanoi final : public CursesGame
{
    static constexpr char ch = '*';
    void init() override
    {
        for (int i = 4; i >= 0; --i)
        {
            towers[0].push(odd(i));
        }
    }

    void loop() override
    {
        std::string twr = printTower(towers[0]);
        for (int i = 0; i < 5; ++i)
        {
            twr += "\n" + twr;
        }
        mvaddstr(height - 1 - 5, 0, twr.c_str());
    }

private:
    std::array<Tower, 3> towers;
    int mov;
};

int main()
{
    CursesHanoi hanoi;
    hanoi.run();
}
//
// Created by Alexey Pashinov on 11/09/20
//

#pragma once

//system includes
#include <random>

namespace utils
{
    std::string random_string(int len)
    {
        std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

        std::random_device rd;
        std::mt19937 generator(rd());

        std::shuffle(str.begin(), str.end(), generator);

        return str.substr(0, len);
    }
}

#pragma once

#include "Types.hpp"
#include <set>

//每个 System 都可以从这个基类继承，从而使得 System Manager可以保留指向 System 的指针列表。
class System{
public:
    std::set<Entity>mEntities;
};
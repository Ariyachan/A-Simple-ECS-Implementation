#pragma once

#include "Types.hpp"
#include <array>
#include <cassert>
#include <unordered_map>

//创建一个数据结构来存放所有同类型的 Component，它本质上是一个简单的紧凑的数组。
//如果将 Entity 作为 Component 数组的索引，那么获取 Entity 相关 Component 的过程将会变得非常简单。
//当Entity被销毁时，数组中的索引不再有效。
//ECS的高性能，实现这一目的的方法就是让数组紧凑的存储在内存中，这意味着应该能够迭代数组中的所有索引，而不需要进行任何的if(valid)检查.
//当Entity销毁时，其拥有的Component数据任然存在数组中，而这部分数据是无效的，因此需要引入if(valid)检查来确保迭代的有效性，这显然会影响到ECS框架的性能
//因此，我们需要始终保持数组装满有效的数据
//解决这个问题的方法是建立一个Entity到数组索引的映射，在访问数组时，通过该映射来查找实际的索引。
//然后，当Entity被销毁时，将数组中的最后一个有效元素移动到删除Entity对应的Component的位置，并更新映射。
//还有一个从数组索引到EntityID的映射，因此，在移动最后一个数组元素时，可以知道使用该索引的Entity是那个，并且可以更新其映射。

//一个接口，以便ComponentManager可以告诉一个通用的ComponentArray。
//一个实体已经被销毁了，并且它需要更新它的数组映射
class IComponentArray{
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray{
public:
    void InsertData(Entity entity,T component){
        assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once");

        //把新的Entity加在末尾，并更新映射
        size_t newIndex = _mSize;
        mEntityToIndexMap[entity] = newIndex;
        mIndexToEntityMap[newIndex] = entity;
        mComponentArray[newIndex] = component;
        ++mSize;
    }

    void RemoveData(Entity entity){
        assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Removing non_existemt component.");

        //把最后一个元素放到被删除的位置，以保持数组数据的紧凑性
        size_t indexOfRemovedEntity = mEntityToIndexMap[entity];    //得到删除的entity索引
        size_t indexOfLastElement = mSize - 1;  //得到最后一个元素的索引
        mComponentArray[indexOfRemovedEntity] = mComponenetArray[indexOfLastElement];   //将最后一个元素的索引放到删除的位置

        //更新映射
        Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement]; //得到最后一个元素的entity
        mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;  //对于entity数组用最后一个entity元素来占据移除的entity
        mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;//更新index到entity的映射

        mEntityToIndexMap.erase(entity);
        mIndexToEntityMap.erase(indexOfLastElement);

        --mSize;
    }

    //公共接口，以便可以在所有的ComponentArray上调用EntityDestroyed();
    T& GetData(Entity entity){
        assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");

        //返回Component的引用
        return mComponentArray[mEntityToIndexMap[entity]];
    }

    void EntityDestroyed(Entity entity) override{
        if(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()){
            RemoveData(entity);
        }
    }

private:
    std::array<T,MAX_ENTITIES> mComponentArray{};
    std::unordered_map<Entity,size_t>mEntityToIndexMap{};
    std::unordered_map<size_t,Entity> mIndexToEntityMap{};
    size_t mSize;
};
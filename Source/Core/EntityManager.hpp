#pragma once

#include "Types.hpp"
#include <array>
#include <cassert>
#include <queue>

class EntityManager{
public:
    //用所有有效的Entity进行初始化
    EntityManager(){
        for(Entity entity = 0; entity < MAX_ENTITIES; ++entity){
            mAvailableEntities.push(entity);
        }
    }

    //当一个Entity被创建时，会从队列的首部取走一个ID
    Entity CreateEntity(){
        assert(mLivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

        Entity id = mAvailableEntities.front();
        mAvailableEntities.pop();
        ++mLivingEntityCount;

        return id;
    }

    //当一个entity被销毁时，它会将销毁的ID加入到队列的尾部
    void DestroyEntity(Entity entity){
        assert(entity < MAX_ENTITIES && "Entity out of range.");

        mSignatures[entity].reset();
        mAvailableEntities.push(entity);
        --mLivingEntityCount;
    }

    void SetSignature(Entity entity,Signature signature){
        assert(entity < MAX_ENTITIES && "Entity out of range.");

        mSignatures[entity] = signature;
    }

    Signature GetSignature(Entity entity){
        assert(entity < MAX_COMPONENTS && "Entity out of range.");

        return mSignatures[entity];
    }

private:
    //所有未使用的ID
    std::queue<Entity> mAvailableEntities{};

    //用于管理Entity和对应的Signature
    std::array<Signature,MAX_ENTITIES>mSignatures;

    //当前可用的EntityID的数量
    uint32_t mLivingEntityCount{};
};
#pragma once

#include "System.hpp"
#include "Types.hpp"
#include <cassert>
#include <memory>
#include <unordered_map>

//当新的system被注册时，它将通过与ComponentManager相同的小trick（typeid(T).name()）被添加到map中。
//每个System都需要有一个签名集合，以便SystemManager可以将合适的Entity添加到每个System的Entity列表中。
//当Entity的签名发生更改或销毁时，需要更新该System正在追踪的Entity列表
class SystemManager{
public:
    template<typename T>
    std::shared_ptr<T> RegisterSystem(){
        const char* typeName = typeid(T).name();

        assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

        auto system = std::make_shared<T>();
        mSystems.insert({typeName,system});
        return system;
    }

    template<typename T>
    void SetSignature(Signature signature){
        const char* typeName = typeid(T).name();

        assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");

        mSignatures.insert({typeName,signature});
    }

    void EntityDestroyed(Entity entity){
        for(auto const& pair : mSystems){
            auto const& system = pair.second;

            system->mEntities.erase(entity);
        }
    }

    void EntitySignatureChanged(Entity entity,Signature entitySignature){
        for(auto const& pair : mSystems){
            auto const& type = pair.first;
            auto const& system = pair.second;
            auto const& systemSignature = mSignatures[type];

            if((entitySignature & systemSignature) == systemSignature){
                system->mEntities.insert(entity);
            }
            else system->mEntities.erase(entity);
        }
    }

private:
    std::unordered_map<const char*,Signature> mSignatures{};
    std::unordered_map<const char*,std::shared_ptr<System>> mSystems{};
};


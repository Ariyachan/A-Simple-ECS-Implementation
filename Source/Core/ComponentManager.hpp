#pragma once

#include "ComponentArray.hpp"
#include "Types.hpp"
#include <any>
#include <memory>
#include <unordered_map>

//管理器负责在 Component 需要添加或删除时与所有不同的 ComponentArray 进行通信。
//需要为每种组件类型都分配一个唯一的ID，以便它可以在Entity的签名中用一个bit来表示
//最简单的方式是让Component Manager 拥有一个 ComponentType ​变量，它会随着每个新的ComponentType的注册而递增
class ComponentManager{
public:
    template<typename T>
    void RegisterComponent(){
        const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");

		mComponentTypes.insert({typeName, mNextComponentType});
		mComponentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});

		++mNextComponentType;
    }

    template<typename T>
    ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

		return mComponentTypes[typeName];
	}

    template<typename T>
    void AddComponent(Entity entity,T component){
        GetComponentArray<T>()->InsertData(entity,component);
    }

    template<typename T>
    void RemoveComponent(Entity entity){
        GetComponentArray<T>()->GetData(entity);
    }

    void EntityDestroyed(Entity entity){
        for(auto const& pair : mComponentArrays){
            auto const& component = pair.second;

            component->EntityDestroyed(entity);
        }
    }

private:
    //使用unordered_map，key为不同类型的T的Component一定对应不同的名字，因此它们的名字就可以当作区分它们的key
    //考虑到存储空间问题，使用指向名字的指针来表示Key，这个Key也可以用来映射IComponentArray
    //value为ComponentType
    std::unordered_map<const char*,ComponentType> mComponentTypes{};    //管理所有的ComponentType
    std::unordered_map<const char*,std::shared_ptr<IComponentArray>> mComponentArrays{};   //管理所有的ComponentArray
    ComponentType mNextComponentType{};     //指向下一个要注册的ComponentType

    template<typename T>    //根据类型获取ComponentArray
    std::shared_ptr<ComponentArray<T>> GetComponentArray(){
        const char* typeName = typeid(T).name();

        assert(mComponentTypes.find(typeName)!= mComponentTypes.end() && "Component not registered before use.");

        return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
    }
};
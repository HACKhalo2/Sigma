#pragma  once

#include "../IFactory.h"
#include "../ISystem.h"
#include "../components/ViewMover.h"

#include <string>
#include <vector>
#include <map>
#include "../AABBTree.h"

class Property;
class IMoverComponent;

class SimplePhysics
    : public Sigma::IFactory, public ISystem<IMoverComponent> {
public:
	SimplePhysics() { }
	~SimplePhysics() {
		for (auto treeitr = this->colliders.begin(); treeitr != this->colliders.end(); ++treeitr) {
			delete treeitr->second;
		}
	};
	/**
	 * \brief Starts the Simple Physics system.
	 *
	 * \return bool Returns false on startup failure.
	 */
	bool Start() { }

	/**
	 * \brief Causes an update in the system based on the change in time.
	 *
	 * Updates the state of the system based off how much time has elapsed since the last update.
	 * \param[in] const float delta The change in time since the last update
	 * \return bool Returns true if we had an update interval passed.
	 */
	bool Update(const double delta);

    std::map<std::string,FactoryFunction> getFactoryFunctions();

	void createPhysicsMover(const unsigned int entityID, std::vector<Property> &properties) ;
	void createViewMover(const unsigned int entityID, std::vector<Property> &properties) ;
	void createAABBTree(const unsigned int entityID, std::vector<Property> &properties) ;

	Sigma::AABBTree* GetCollider(const unsigned int entityID) {
		if (this->colliders.find(entityID) != this->colliders.end()) {
			return this->colliders[entityID];
		}
		return nullptr;
	}

private:
	std::map<unsigned int, Sigma::AABBTree*> colliders;
};

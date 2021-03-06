#pragma once
#include "../IBulletShape.h"

namespace Sigma{
	class GLMesh;
	class BulletShapeMesh : public IBulletShape {
	public:
		SET_COMPONENT_ID("BulletShapeMesh");
		BulletShapeMesh(const int entityID = 0) : IBulletShape(entityID) { }
		~BulletShapeMesh() {
			if (this->btmesh != nullptr) {
				delete this->btmesh;
			}
		}

		void SetMesh(const GLMesh* mesh);
	private:
		btTriangleMesh* btmesh;
	};
}
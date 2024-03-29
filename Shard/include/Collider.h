#pragma once

#include <string>
#include <typeinfo>
#include <optional>
#include <memory>
#include <vector>
#include <vector>

#include "CollisionHandler.h"
#include "Model.h"

namespace Shard {

	typedef struct Ray {
		glm::vec3 origin{0.0f};
		glm::vec3 dir{0.0f};
	} Ray;

    class ColliderBox;

    class Collider : public std::enable_shared_from_this<Collider> {
    public:
        
        bool m_rotateAtOffset;
        std::shared_ptr<Model> m_model;
        std::shared_ptr<CollisionHandler> m_gameObject;

        Collider();
        Collider(std::shared_ptr<CollisionHandler> gameObj, std::shared_ptr<Model> model);

        virtual void recalculateBoundingBox() = 0;
        virtual	std::optional<glm::vec3> checkCollision(Ray& ray) = 0;
        virtual std::vector<glm::vec2> getMinMaxDims() = 0;
        virtual std::vector<glm::vec2> getTransformedMinMaxDims() = 0;
        
    protected:
        bool m_fromModel;
    };

}

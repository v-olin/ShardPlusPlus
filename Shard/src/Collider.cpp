#include "Transform.h"
#include "Collider.h"
#include "Bootstrap.h"

namespace Shard {

	typedef struct Ray {
		glm::vec3 origin;
		glm::vec3 dir;
	} Ray;

    Collider::Collider(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform)
        : transform(transform)
        , game_object(game_obj)
    {}

}
#include "Collider.h"
#include "Bootstrap.h"

namespace Shard {

	typedef struct Ray {
		glm::vec3 origin;
		glm::vec3 dir;
	} Ray;

	Collider::Collider()
		: m_rotateAtOffset(false)
		, m_model(nullptr)
		, m_gameObject(nullptr)
		, m_fromModel(false)
	{ }

	Collider::Collider(std::shared_ptr<CollisionHandler> gameObj, std::shared_ptr<Model> model)
		: m_rotateAtOffset(false)
		, m_model(model)
		, m_gameObject(gameObj)
		, m_fromModel(true) // TODO, is this false or true? it was false previously...
	{ }

    //Collider::Collider(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform)
    //    : transform(transform)
    //    , game_object(game_obj)
    //{}

}
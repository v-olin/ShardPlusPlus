#include "Collider.h"
#include "Bootstrap.h"

namespace Shard {


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
}
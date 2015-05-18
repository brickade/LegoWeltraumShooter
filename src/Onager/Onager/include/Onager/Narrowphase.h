#pragma once

#include "myMath.h"
#include <vector>
#include "Contact.h"

namespace ong
{
	
	class Body;
	class Collider;
	struct BVTree;


	struct Pair;


	// todo persistent contacts
	class ContactManager
	{
	public:
		void generateContacts(Pair* pairs, int numPairs, int maxContacts);

		Contact* getContacts(int* numContacts);

	private:
		void collide(Body* a, Body* b);

		void collide(BVTree* tree1, BVTree* tree2, BVTree* a, BVTree* b, const vec3& t, const mat3x3& rot);
		void collide(BVTree* tree, BVTree* a, Collider* b, const vec3& t, const mat3x3& rot);
		void collide(Collider* c1, Collider* c2);


		std::vector<Contact> m_contacts;
		std::vector<ContactIter> m_contactIters;
		std::vector<Contact> m_oldContacts;
		// used for detecting ending of contacts
		std::vector<bool> m_contactEnded;
		std::vector<bool> m_oldContactEnded;
	};


	inline Contact* ContactManager::getContacts(int* numContacts)
	{
		if (numContacts)
			*numContacts = m_contacts.size();
		return m_contacts.data();

	}

}
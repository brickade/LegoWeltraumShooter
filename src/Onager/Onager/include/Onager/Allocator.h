#pragma once

namespace ong
{



	template <typename T>
	class Allocator
	{
	public:
		Allocator(int numElements)
			: pNextFree(nullptr),
			pSlabs(nullptr),
			numSlabs(0),
			NUM_ELEMENTS(numElements)
		{
		};

		~Allocator()
		{
			for (uint32 i = 0; i < numSlabs; ++i)
			{
				free(pSlabs[i]);
			}
		}

		T* sNew(const T& t)
		{
			void* p;

			p = pNextFree;

			if (p != nullptr)
			{
				pNextFree = *(void**)pNextFree;
			}
			else
			{
				p = malloc(NUM_ELEMENTS * sizeof(T));


				T* pChunkIter = (T*)p + (NUM_ELEMENTS - 1);
				void* pPrevious = nullptr;

				for (size_t i = 0; i < NUM_ELEMENTS - 1; ++i)
				{
					*(void**)pChunkIter = pPrevious;
					pPrevious = pChunkIter;
					pChunkIter--;
				}

				pSlabs = (void**)realloc(pSlabs, (++numSlabs) * sizeof(void*));
				pSlabs[numSlabs - 1] = p;
				pNextFree = pPrevious;
			}

			return new(p)T(t);
		}

		void sDelete(T* p)
		{
			p->~T();
			*(void**)p = pNextFree;
			pNextFree = p;
		}


		T* operator()(const T& t = T())
		{
			return sNew(t);
		}

	private:
		const size_t NUM_ELEMENTS;
		void* pNextFree;
		void** pSlabs;
		size_t numSlabs;

	};

}
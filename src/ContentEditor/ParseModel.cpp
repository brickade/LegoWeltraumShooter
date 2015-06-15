#include "ParseModel.h"
#include <stdio.h>
#include <string>


namespace Content
{
	void parseModel(const char* a_pModelPath, std::vector<ong::vec3>& a_Vertices)
	{
		FILE* f = fopen(a_pModelPath, "r");
		
		char buffer[1024];
		while (fgets(buffer, 1024, f))
		{

			char* c = strtok(buffer, " \n");
			if (c == nullptr || *c != 'v' || *(c+1) != '\0')
				continue;

			ong::vec3 v;
			for (int i = 0; i < 3; ++i)
			{
				c = strtok(nullptr, " \n");
				assert(c != nullptr);
				v[i] = atof(c);
			}

			a_Vertices.push_back(v);
		}

	}
}
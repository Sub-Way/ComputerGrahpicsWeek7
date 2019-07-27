#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <glm\glm.hpp>

class OBJRead
{
public:
	std::vector<glm::vec3> vertex_stack_;
	std::vector<glm::tvec3<unsigned int>> index_stack_;

public:
	void readOBJ(const char* filename)
	{
		using namespace std;

		cout << "Start Reading OBJ file: " << filename << "\n";

		// to check if this obj file contains vt or vn data
		bool read_vt(false), read_vn(false);

		ifstream file(filename);
		if (file.is_open() == false) {
			cout << filename << " does not exist. Program terminated." << std::endl; exit(-1);
		}

		char c[255];
		while (true)
		{
			file >> c;

			// finish reading if file is ended
			if (file.eof() != 0) break; 

			if (strcmp(c, "#") == 0) file.getline(c, 255); // comments (less than 255 characters)
			else if (strcmp(c, "vt") == 0)
			{
				read_vt = true;
				
				float u, v;
				file >> u >> v;
				
				// TODO: save texture coordinates
			}
			else if (strcmp(c, "vn") == 0)
			{
				read_vn = true;
				
				float nx, ny, nz;
				file >> nx >> ny >> nz;

				// TODO: save vertex normals
			}
			else if (strcmp(c, "v") == 0)
			{
				// TODO: vertices
				float x, y, z;
				file >> x >> y >> z;

				vertex_stack_.push_back(glm::vec3(x, y, z));
			}
			else if (strcmp(c, "f") == 0)
			{
				unsigned int v[3], vt[3], vn[3];
				// TODO: face
				if (read_vn == true && read_vt == true){
					for (int i = 0; i < 3; i++)
					{
						file >> v[i]; file.get(c, 2);
						file >> vt[i]; file.get(c, 2);
						file >> vn[i];

						v[i]--;
						vt[i]--;
						vn[i]--;
					}
				}
				else if (read_vn == true && read_vt == false){
					for (int i = 0; i < 3; i++)
					{
						file >> v[i]; file.get(c, 2); file.get(c, 2);
						file >> vn[i];
						v[i]--;
					}
				}
				else if (read_vn == false && read_vt == true){
					for (int i = 0; i < 3; i++)
					{
						file >> v[i]; file.get(c, 2);
						file >> vt[i];

						v[i]--;
						vt[i]--;
					}
				}
				else if (read_vn == false && read_vt == false){
					for (int i = 0; i < 3; i++)
					{
						file >> v[i];
						v[i]--;
					}
				}

				index_stack_.push_back(glm::tvec3<unsigned int>(v[0], v[1], v[2]));
			}
		}

		file.clear();
		file.close();

		std::cout << "Reading complete." << std::endl;
		std::cout << "# of vertices " << vertex_stack_.size() << std::endl;
		std::cout << "# of triangles " << index_stack_.size() << std::endl;

		const float min = std::numeric_limits<float>::min();
		const float max = std::numeric_limits<float>::max();

		glm::vec3 min_limit(max, max, max);
		glm::vec3 max_limit(min, min, min);
		// TODO: move center to origin
		for (int i = 0; i < vertex_stack_.size(); i++)
		{
			// find min, max
			max_limit.x = std::max(vertex_stack_[i].x, max_limit.x);
			max_limit.y = std::max(vertex_stack_[i].y, max_limit.y);
			max_limit.z = std::max(vertex_stack_[i].z, max_limit.z);

			min_limit.x = std::min(vertex_stack_[i].x, min_limit.x);
			min_limit.y = std::min(vertex_stack_[i].y, min_limit.y);
			min_limit.z = std::min(vertex_stack_[i].z, min_limit.z);
		}

		float middle_x = max_limit.x + min_limit.x;
		float middle_y = max_limit.y + min_limit.y;
		float middle_z = max_limit.z + min_limit.z;

		for (int i = 0; i < vertex_stack_.size(); i++)
		{
			vertex_stack_[i].x -= middle_x * 0.5;
			vertex_stack_[i].y -= middle_y * 0.5;
			vertex_stack_[i].z -= middle_z * 0.5;
		}

		// TODO: scale down the BB to a unit cube
		float dx = max_limit.x - min_limit.x;
		float dy = max_limit.y - min_limit.y;
		float dz = max_limit.z - min_limit.z;

		float dl = std::max(std::max(dx, dy), dz);

		for (int i = 0; i < vertex_stack_.size(); i++)
		{
			vertex_stack_[i].x /= dl;
			vertex_stack_[i].y /= dl;
			vertex_stack_[i].z /= dl;
		}
	}

	void dump()
	{
		for (int i = 0; i < vertex_stack_.size(); i++)
		{
			std::cout << vertex_stack_[i].x << " " << vertex_stack_[i].y << " " << vertex_stack_[i].z;
			std::cout << std::endl;
		}

		for (int i = 0; i < index_stack_.size(); i++)
		{
			std::cout << index_stack_[i].x << " " << index_stack_[i].y << " " << index_stack_[i].z;
			std::cout << std::endl;
		}

	}

	OBJRead();
	~OBJRead();
};


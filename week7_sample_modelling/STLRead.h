#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <glm\glm.hpp>

class STLRead
{
public:
	std::vector<glm::vec3> vertex_stack_;
	std::vector<glm::tvec3<unsigned int>> index_stack_;
	std::vector<glm::vec3> normal_stack_;

public:
	void readSTL(const char* filename)
	{
		using namespace std;

		cout << "Start Reading STL file: " << filename << "\n";

		ifstream file(filename, std::ifstream::binary);
		if (file.is_open() == false) {
			std::cout << filename << " does not exist. Program terminated." << std::endl; exit(-1);
		}

		// stl file format
		char header[80];
		file.read(header, sizeof(header));
		//cout << "[header]\n" << header << endl;

		unsigned int num_tri;
		file.read((char*)&num_tri, sizeof(num_tri));//ĳ���ͷ� ĳ�����ؼ� �Ѱ���� �޾ƿ� �� �ִ�.
		//std::cout << "# of triangles: " << num_tri << std::endl;

		vertex_stack_.clear();
		index_stack_.clear();
		normal_stack_.clear();

		vertex_stack_.resize(num_tri * 3);
		index_stack_.resize(num_tri);
		normal_stack_.resize(num_tri);

		for (int t = 0; t < (int)num_tri; t++)
		{
			// TODO: Read normal
			float n[3];
			file.read((char*)&n, sizeof(n));
			// TODO: Read each vertex
			for (int c = 0; c < 3; c++) {
				index_stack_[t][c] = 3 * t + c;
				normal_stack_[t][c] = n[c];
				float v[3];
				file.read((char*)&v, sizeof(v));
				vertex_stack_[3 * t + c][0] = v[1];
				vertex_stack_[3 * t + c][1] = v[2];
				vertex_stack_[3 * t + c][2] = v[0];
			}

			// Read attribute size
			unsigned short att_count;
			if (!file.read((char*)&att_count, sizeof(att_count))) {
				cerr << "IOError: bad format (3)." << endl;
			}
		}

		file.clear();
		file.close();

		std::cout << "Reading complete." << std::endl;
		std::cout << "# of vertices " << vertex_stack_.size() << std::endl;
		std::cout << "# of triangles " << index_stack_.size() << std::endl;


		const float min = std::numeric_limits<float>::min(); //float�� ���� �� �ִ� �ּҰ��� min�� ����.
		const float max = std::numeric_limits<float>::max(); //�ִ밪�� max�� ����

		glm::vec3 min_limit(max, max,max);
		glm::vec3 max_limit(min, min, min); // �ּڰ��� ã�� ���°�
		// TODO: move center to origin
		for (int i = 0; i < vertex_stack_.size(); i++)
		{
			// find min, max
			max_limit.x = std::max(vertex_stack_[i].x, max_limit.x); // ���ؼ� �� ū ���� ����
			max_limit.y = std::max(vertex_stack_[i].y, max_limit.y); // ���ؼ� �� ū ���� ����
			max_limit.z = std::max(vertex_stack_[i].z, max_limit.z); // ���ؼ� �� ū ���� ����

			min_limit.x = std::min(vertex_stack_[i].x, min_limit.x);
			min_limit.y = std::min(vertex_stack_[i].y, min_limit.y);
			min_limit.z = std::min(vertex_stack_[i].z, min_limit.z);
		}
		//bounding box�� ã�� ����.

		float middle_x = max_limit.x + min_limit.x; //�� ���� ������~~~
		float middle_y = max_limit.y + min_limit.y;
		float middle_z = max_limit.z + min_limit.z;

		for (int i = 0; i < vertex_stack_.size(); i++)//������ ���缭 ���ְ� �ɰ̴ϴ�.
		{
			vertex_stack_[i].x -= middle_x * 0.5;
			vertex_stack_[i].y -= middle_y * 0.5;
			vertex_stack_[i].z -= middle_z * 0.5;
		}

		// TODO: scale down the BB to a unit cube
		float dx = max_limit.x - min_limit.x;
		float dy = max_limit.y - min_limit.y;
		float dz = max_limit.z - min_limit.z;

		float dl = std::max(std::max(dx, dy), dz); // ���� ū ���� dl�� ����.

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
	STLRead();
	~STLRead();
};


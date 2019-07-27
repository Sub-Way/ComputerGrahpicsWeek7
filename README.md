# ComputerGrahpicsWeek7

## 7주차. 모델링

### 1.	이진코드의 STL 파일을 읽는 코드를 완성하여 OpenGL을 통해 출력하시오. (70점)


|||
|:------------------------|:--------|
|vertex와 index의 정보를 저장 |+20|
|Bounding Box 의 최소, 최대점을 찾기 |+10 |
|BB를 이용해서 3D 모델을 원점으로 이동|+10 |
|BB를 이용해서 3D 모델을 unit으로 변경|+10|
|OpenGL을 통해 출력|+20|



### 2.	자신이 맘에 드는 .obj 파일과 .stl 파일을 인터넷에서 찾아 OpenGL을 통해 출력하시오. (30점)

|||
|:------------------------|:--------|
|한 윈도우에 모두 출력하고 모델들을 Translation 해서 배치한 경우|+30|
|한 윈도우에 두 모델을 출력한 경우| +20 |
|두 윈도우에 각각 출력한 경우| +10 |

### 1. 주요 소스 코드
~~~
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


		const float min = std::numeric_limits<float>::min(); //float가 가질 수 있는 최소값을 min에 넣음.
		const float max = std::numeric_limits<float>::max(); //최대값을 max에 넣음

		glm::vec3 min_limit(max, max,max);
		glm::vec3 max_limit(min, min, min); // 최솟값을 찾아 가는것
		// TODO: move center to origin
		for (int i = 0; i < vertex_stack_.size(); i++)
		{
			// find min, max
			max_limit.x = std::max(vertex_stack_[i].x, max_limit.x); // 비교해서 더 큰 값을 넣음
			max_limit.y = std::max(vertex_stack_[i].y, max_limit.y); // 비교해서 더 큰 값을 넣음
			max_limit.z = std::max(vertex_stack_[i].z, max_limit.z); // 비교해서 더 큰 값을 넣음

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
~~~
전반적으로 OBJRead.h와 비슷하다. for문 안에서 읽어온 STL 파일을 읽어와서 index_stack, normal_stack, vertex_stack에 저장한다. 여기에서 vertex_statck에 저장하는 vertext의 순서를 다르게 하여 원본 그대로의 STL파일을 출력할 수 있도록 했다. 뒷 부분은 OBJRead.h와 같다. Bounding box를 찾고, 최솟값과 최대값을 넣고, 모델의 중점을 찾는 과정, Bounding box에 맞춰 scale을 주정하는 부분이 코드에 나타나 있다.

### 결과화면

![image](https://user-images.githubusercontent.com/22046757/61995638-00815580-b0c6-11e9-9bfd-6d11b14f0830.png)

### 2. 주요 소스 코드
~~~
//read 3D model from obj file
	OBJRead obj_reader;
	obj_reader.readOBJ("./earth.obj");
	//obj_reader.readObj("../../test-result-sphere_f80.obj");
	//obj_reader.dump();
	
	STLRead stl_reader;
	stl_reader.readSTL("./Cat.stl");
	//stl_reader.dump();
	
	// TODO: copy geometry to buffer object
	GLuint o_vbo[3];
	glGenBuffers(3, o_vbo);

	GLuint s_vbo[3];
	glGenBuffers(3, s_vbo);


	// glm::vec3 == vertex_stack_  (float, float, float) 
	//stl
	glBindBuffer(GL_ARRAY_BUFFER, o_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, obj_reader.vertex_stack_.size() * sizeof(float) * 3,
		&obj_reader.vertex_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, o_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, obj_reader.vertex_stack_.size() * sizeof(float) * 3, &obj_reader.vertex_stack_[0], GL_STATIC_DRAW);

	// glm::tvec3<unsigned int> (unsigned int, unsigned int, unsigned int)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o_vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj_reader.index_stack_.size() * sizeof(unsigned int) * 3 , &obj_reader.index_stack_[0], GL_STATIC_DRAW);

	//obj
	glBindBuffer(GL_ARRAY_BUFFER, s_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, stl_reader.vertex_stack_.size() * sizeof(float) * 3,
		&stl_reader.vertex_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, s_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, stl_reader.vertex_stack_.size() * sizeof(float) * 3, &stl_reader.vertex_stack_[0], GL_STATIC_DRAW);

	// glm::tvec3<unsigned int> (unsigned int, unsigned int, unsigned int)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, stl_reader.index_stack_.size() * sizeof(unsigned int) * 3, &stl_reader.index_stack_[0], GL_STATIC_DRAW);

/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // <- depth test
		
		glTranslatef(-1, 0, 0);
		glRotatef(1, 0, 1, 0);
		glBindBuffer(GL_ARRAY_BUFFER, o_vbo[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);		// Vector 3

		glBindBuffer(GL_ARRAY_BUFFER, o_vbo[1]);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, 0);		// Vector 3

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o_vbo[2]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, stl_reader.index_stack_.size() * sizeof(unsigned int) * 3, GL_UNSIGNED_INT, 0);
		glEnd();

		glTranslatef(1, 0, 0);
		glRotatef(1, 0, 1, 0);
		glBindBuffer(GL_ARRAY_BUFFER, s_vbo[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);		// Vector 3

		glBindBuffer(GL_ARRAY_BUFFER, s_vbo[1]);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, 0);		// Vector 3

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_vbo[2]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, stl_reader.index_stack_.size() * sizeof(unsigned int) * 3, GL_UNSIGNED_INT, 0);
		glEnd();

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
~~~

읽어올 3D 모델을 정의해준다. obj파일과 stl 파일을 읽어오기 위해 OBJRead obj_Reader, STLRead stl_reader로 읽어올 각각의 obj파일과 stl의 경로를 지정했다. 그리고 obj파일과 stl 파일에 대한 버퍼를 생성했다. Obj 파일과 stl 파일에 대해 각각의 버퍼에 target 값을 할당한다. 그리고 bind된 버퍼에 데이터를 넣는다.
While문 안에서 한 윈도우 안에 두개의 모델이 출력될 수 있도록 glTranslatef와 glRotatef를 이용한다. 그리고 모델의 각각의 vector에 대해 glVertexPointer와 glColorPointer, glDrawElements를 이용해 모델을 출력한다.
### 결과화면

![image](https://user-images.githubusercontent.com/22046757/61995640-037c4600-b0c6-11e9-8206-b3d0a407b3e5.png)
 
한 윈도우 안에 두개의 모델이 출력되었다.

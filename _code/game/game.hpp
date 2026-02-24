#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<vector>

#ifndef pidor
#define pidor

struct VENOM 
{
private:
	//unsigned int ID = 0;

public:
#pragma pack(push,1)
	struct OBJ
	{
	
		// 40 байт 
		glm::vec2 set_pos =     glm::vec2(0.f, 0.f);
		glm::vec2 set_scale =   glm::vec2(150.f,150.f);
		glm::vec2 set_rotate =  glm::vec2(0.f, 0.f);
		glm::vec3 set_color =   glm::vec3(0.f, 0.f, 0.f);
		// Texture* tex = 0;
		unsigned int texture_ = 0; 
		//40 байт
		unsigned int ID = 0;
    
	}; 
#pragma pack(pop)
public:

	~VENOM()
	{

	}
	VENOM()
	{
		//skok_obj = nullptr; obj_param = nullptr;
	}
	 void __fastcall add()
	 {
		  
		 objects.push_back(this->attrib);
		 objects[obj_count].ID = obj_count;
		
		 
		 obj_count++;
	 }
	
	
	int obj_count = 0;
	//char = 40;
	

	OBJ attrib;
	std::vector<OBJ> objects;
}; VENOM venom;

#endif
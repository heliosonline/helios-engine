#pragma once


namespace Helios {


	struct ModelVertexData
	{
		glm::vec3 pos;
		glm::vec4 color;
	};


	class Model
	{
	public:
		static Ref<Model> Create();

		Model() { LOG_CORE_DEBUG("Model::Model()"); }
		~Model() { LOG_CORE_DEBUG("Model::~Model()"); }

		void Load(const std::string& filename, const std::string& arcname = "");

//		void Transform(...);
//		void Scale(float scale);
//		void Pos(glm::vec3& pos);

		// Signal the renderer to draw the model
		virtual void Draw() = 0;

	private:
		std::vector<ModelVertexData> m_vertices;
	};


} // namespace Helios

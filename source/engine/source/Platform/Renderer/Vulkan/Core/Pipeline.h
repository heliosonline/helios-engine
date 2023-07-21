#pragma once

#include <vulkan/vulkan.hpp>

namespace Helios::Vulkan {


	struct PipelineConfigInfo {
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		vk::PipelineViewportStateCreateInfo viewportInfo;
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
		vk::PipelineMultisampleStateCreateInfo multisampleInfo;
		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
		vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
		vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<vk::DynamicState> dynamicStateEnables;
		vk::PipelineDynamicStateCreateInfo dynamicStateInfo;

		vk::PipelineLayout pipelineLayout = nullptr;
		vk::RenderPass renderPass = nullptr;
		uint32_t subpass = 0;

//		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
//		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
	};


	class Pipeline
	{
	public:
		Pipeline(const std::string &vertShader, const std::string &fragShader, const PipelineConfigInfo &configInfo);
		~Pipeline();

		void Create(const std::string &vertShader, const std::string &fragShader, const PipelineConfigInfo &configInfo);
		void Destroy();

	public:
		void Bind(vk::CommandBuffer commandBuffer);

	// Getter for vulkan objects
	public:
		vk::Pipeline& GetGraphicsPipeline() { return m_vkGraphicsPipeline; }

		static void DefaultConfigInfo(PipelineConfigInfo& confifInfo);

	// Vulkan objects
	private:
		vk::Pipeline m_vkGraphicsPipeline;
		vk::ShaderModule m_vkVertShaderModule;
		vk::ShaderModule m_vkFragShaderModule;

	// Internal helper
	private:
		vk::ShaderModule CreateShaderModule(const std::vector<char> &code);

	// Internal data
	private:
	};


} // namespace Helios::Vulkan

#include "pch.h"
#include "Pipeline.h"

#include "HeliosEngine/Renderer/Renderer.h"
#include "Platform/Renderer/Vulkan/VKRendererAPI.h"

#include "Platform/Renderer/Vulkan/VKModel.h"

#include "HeliosEngine/Core/Assets.h"


namespace Helios::Vulkan {


	Pipeline::Pipeline(const std::string &vertShader, const std::string &fragShader, const PipelineConfigInfo &configInfo)
	{
		Create(vertShader, fragShader, configInfo);
	}


	Pipeline::~Pipeline()
	{
		Destroy();
	}


	void Pipeline::Create(const std::string &vertShader, const std::string &fragShader, const PipelineConfigInfo &configInfo)
	{
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		LOG_RENDER_TRACE("Creating pipeline objects...");

		auto vertCode = Assets::Load(vertShader, "RendererVulkan");
		auto fragCode = Assets::Load(fragShader, "RendererVulkan");
		m_vkVertShaderModule = CreateShaderModule(vertCode);
		m_vkFragShaderModule = CreateShaderModule(fragCode);

		vk::PipelineShaderStageCreateInfo shaderStages[2];
		shaderStages[0] = vk::PipelineShaderStageCreateInfo();
		{
			shaderStages[0].stage = vk::ShaderStageFlagBits::eVertex;
			shaderStages[0].module = m_vkVertShaderModule;
			shaderStages[0].pName = "main";
		}
		shaderStages[1] = vk::PipelineShaderStageCreateInfo();
		{
			shaderStages[1].stage = vk::ShaderStageFlagBits::eFragment;
			shaderStages[1].module = m_vkFragShaderModule;
			shaderStages[1].pName = "main";
		}

		auto bindingDescriptions = VKModel::Vertex::GetBindingDescriptions();
		auto attributeDescriptions = VKModel::Vertex::GetAttributeDescriptions();
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = vk::PipelineVertexInputStateCreateInfo();
		{
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
			vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
		}

		vk::GraphicsPipelineCreateInfo pipelineInfo = vk::GraphicsPipelineCreateInfo();
		{
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderStages;
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
			pipelineInfo.pViewportState = &configInfo.viewportInfo;
			pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
			pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
			pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
			pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
			pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;

			pipelineInfo.layout = configInfo.pipelineLayout;
			pipelineInfo.renderPass = configInfo.renderPass;
			pipelineInfo.subpass = configInfo.subpass;

			pipelineInfo.basePipelineIndex = -1;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		}

		try {
			LOG_RENDER_TRACE("Creating graphics pipeline...");
			m_vkGraphicsPipeline = device->GetLogicalDevice().createGraphicsPipeline(nullptr, pipelineInfo).value;
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to create graphics pipeline!");
		}
	}


	void Pipeline::Destroy()
	{
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		LOG_RENDER_TRACE("Destroying pipeline objects...");

		if (m_vkVertShaderModule)
			device->GetLogicalDevice().destroyShaderModule(m_vkVertShaderModule);
		if (m_vkFragShaderModule)
			device->GetLogicalDevice().destroyShaderModule(m_vkFragShaderModule);
		if (m_vkGraphicsPipeline)
			device->GetLogicalDevice().destroyPipeline(m_vkGraphicsPipeline);
	}


	void Pipeline::Bind(vk::CommandBuffer commandBuffer)
	{
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_vkGraphicsPipeline);
	}


	void Pipeline::DefaultConfigInfo(PipelineConfigInfo &configInfo)
	{
		Ref<Swapchain> &swapchain = static_cast<VKRendererAPI*>(Renderer::Get())->GetSwapchain();

//		PipelineConfigInfo configInfo;

		configInfo.viewportInfo = vk::PipelineViewportStateCreateInfo();
		{
			configInfo.viewportInfo.viewportCount = 1;
			configInfo.viewportInfo.pViewports = nullptr;
			configInfo.viewportInfo.scissorCount = 1;
			configInfo.viewportInfo.pScissors = nullptr;
		}
		configInfo.inputAssemblyInfo = vk::PipelineInputAssemblyStateCreateInfo();
		{
			configInfo.inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
			configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
		}
		configInfo.rasterizationInfo = vk::PipelineRasterizationStateCreateInfo();
		{
			configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
			configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
			configInfo.rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
			configInfo.rasterizationInfo.lineWidth = 1.0f;
			configInfo.rasterizationInfo.cullMode = vk::CullModeFlagBits::eNone;
//			configInfo.rasterizationInfo.cullMode = vk::CullModeFlagBits::eBack;
			configInfo.rasterizationInfo.frontFace = vk::FrontFace::eClockwise;
			configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
			configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // optional
			configInfo.rasterizationInfo.depthBiasClamp = 0.0;           // optional
			configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;    // optional
		}
		configInfo.multisampleInfo = vk::PipelineMultisampleStateCreateInfo();
		{
			configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
			configInfo.multisampleInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
			configInfo.multisampleInfo.minSampleShading = 1.0f;          // optional
			configInfo.multisampleInfo.pSampleMask = nullptr;            // optional
			configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE; // optional
			configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;      // optional
		}
		configInfo.colorBlendAttachment = vk::PipelineColorBlendAttachmentState();
		{
			configInfo.colorBlendAttachment.colorWriteMask =
				vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
			configInfo.colorBlendAttachment.blendEnable = VK_FALSE;

			configInfo.colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;  // optional
			configInfo.colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // optional
			configInfo.colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;             // optional
			configInfo.colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;  // optional
			configInfo.colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // optional
			configInfo.colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;             // optional
		}
		configInfo.colorBlendInfo = vk::PipelineColorBlendStateCreateInfo();
		{
			configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
			configInfo.colorBlendInfo.logicOp = vk::LogicOp::eCopy; // optional
			configInfo.colorBlendInfo.attachmentCount = 1;
			configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
			configInfo.colorBlendInfo.blendConstants[0] = 0.0f;     // optional
			configInfo.colorBlendInfo.blendConstants[1] = 0.0f;     // optional
			configInfo.colorBlendInfo.blendConstants[2] = 0.0f;     // optional
			configInfo.colorBlendInfo.blendConstants[3] = 0.0f;     // optional
		}
		configInfo.depthStencilInfo = vk::PipelineDepthStencilStateCreateInfo();
		{
			configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
			configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
			configInfo.depthStencilInfo.depthCompareOp = vk::CompareOp::eLess;
			configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
			configInfo.depthStencilInfo.minDepthBounds = 0.0f;        // optional
			configInfo.depthStencilInfo.maxDepthBounds = 1.0f;        // optional
			configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE; // optional
//			configInfo.depthStencilInfo.front = ...;                  // optional
//			configInfo.depthStencilInfo.back = ...;                   // optional
		}
		configInfo.dynamicStateEnables = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
		configInfo.dynamicStateInfo = vk::PipelineDynamicStateCreateInfo();
		{
			configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
			configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
		}
	}


	vk::ShaderModule Pipeline::CreateShaderModule(const std::vector<char> &code)
	{
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		vk::ShaderModuleCreateInfo moduleInfo = vk::ShaderModuleCreateInfo();
		{
			moduleInfo.codeSize = code.size();
			moduleInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		}

		try {
			LOG_RENDER_TRACE("Creating shader module...");
			return device->GetLogicalDevice().createShaderModule(moduleInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to create shader module!");
		}
	}


} // namespace Helios::Vulkan

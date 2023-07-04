#include "pch.h"

#include "Platform/Renderer/Vulkan/Core/Pipeline.h"
#include "HeliosEngine/Renderer/Renderer.h"

#include "Platform/Renderer/Vulkan/Core/Devices.h"
#include "Platform/Renderer/Vulkan/Core/Swapchain.h"

#include "Platform/Renderer/Vulkan/VKShader.h"


namespace Helios::Vulkan {


	Pipeline::Pipeline()
	{
		m_Devices = ((VKRendererAPI*)Renderer::Get())->GetDevices();
		m_Swapchain = ((VKRendererAPI*)Renderer::Get())->GetSwapchain();
	}


	void Pipeline::Create()
	{
		LOG_RENDER_DEBUG("Creating vulkan pipeline...");

		vk::GraphicsPipelineCreateInfo PipelineInfo = vk::GraphicsPipelineCreateInfo();
		std::vector<vk::PipelineShaderStageCreateInfo> ShaderStages;

		// Vertex input
		vk::PipelineVertexInputStateCreateInfo VertexInfo = vk::PipelineVertexInputStateCreateInfo();
		{
			VertexInfo.vertexAttributeDescriptionCount = 0;
			VertexInfo.vertexBindingDescriptionCount = 0;
		}
		PipelineInfo.pVertexInputState = &VertexInfo;

		// Input assembly
		vk::PipelineInputAssemblyStateCreateInfo AssemblyInfo = vk::PipelineInputAssemblyStateCreateInfo();
		{
			AssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
		}
		PipelineInfo.pInputAssemblyState = &AssemblyInfo;

		// Vertex shader
		VKShader VertexShader("test.vert.spv");
		vk::PipelineShaderStageCreateInfo VertShaderInfo = vk::PipelineShaderStageCreateInfo();
		{
			VertShaderInfo.stage = vk::ShaderStageFlagBits::eVertex;
			VertShaderInfo.module = VertexShader.CreateModule();
			VertShaderInfo.pName = "main";
		}
		ShaderStages.push_back(VertShaderInfo);

		// Viewport and scissor
		vk::Viewport ViewPort = vk::Viewport();;
		{
			ViewPort.x = 0;
			ViewPort.y = 0;
			ViewPort.width = m_Swapchain->GetExtent().width;
			ViewPort.height = m_Swapchain->GetExtent().height;
			ViewPort.minDepth = 0.0f;
			ViewPort.maxDepth = 1.0f;
		}
		vk::Rect2D Scissor;
		{
			Scissor.offset.x = 0;
			Scissor.offset.y = 0;
			Scissor.extent = m_Swapchain->GetExtent();
		}
		vk::PipelineViewportStateCreateInfo ViewportState = vk::PipelineViewportStateCreateInfo();
		{
			ViewportState.viewportCount = 1;
			ViewportState.pViewports = &ViewPort;
			ViewportState.scissorCount = 1;
			ViewportState.pScissors = &Scissor;
		}
		PipelineInfo.pViewportState = &ViewportState;

		// Rasterizer
		vk::PipelineRasterizationStateCreateInfo RasterizerState = vk::PipelineRasterizationStateCreateInfo();
		{
			RasterizerState.depthClampEnable = VK_FALSE;
			RasterizerState.rasterizerDiscardEnable = VK_FALSE;
			RasterizerState.polygonMode = vk::PolygonMode::eFill;
			RasterizerState.lineWidth = 1.0f;
			RasterizerState.cullMode = vk::CullModeFlagBits::eBack;
			RasterizerState.frontFace = vk::FrontFace::eClockwise;
			RasterizerState.depthBiasEnable = VK_FALSE;
		}
		PipelineInfo.pRasterizationState = &RasterizerState;

		// Fragment shader
		VKShader FragmentShader("test.frag.spv");
		vk::PipelineShaderStageCreateInfo FragShaderInfo = vk::PipelineShaderStageCreateInfo();
		{
			FragShaderInfo.stage = vk::ShaderStageFlagBits::eFragment;
			FragShaderInfo.module = FragmentShader.CreateModule();
			FragShaderInfo.pName = "main";
		}
		ShaderStages.push_back(FragShaderInfo);
		PipelineInfo.stageCount = static_cast<uint32_t>(ShaderStages.size());
		PipelineInfo.pStages = ShaderStages.data();

		// Multisampling
		vk::PipelineMultisampleStateCreateInfo MultisamplingState = vk::PipelineMultisampleStateCreateInfo();
		{
			MultisamplingState.sampleShadingEnable = VK_FALSE;
			MultisamplingState.rasterizationSamples = vk::SampleCountFlagBits::e1;
		}
		PipelineInfo.pMultisampleState = &MultisamplingState;

		// Color blend
		vk::PipelineColorBlendAttachmentState ColorBlendAttachment = vk::PipelineColorBlendAttachmentState();
		{
			ColorBlendAttachment.colorWriteMask =
				vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
			ColorBlendAttachment.blendEnable = VK_FALSE;
		}
		vk::PipelineColorBlendStateCreateInfo ColorBlendState = vk::PipelineColorBlendStateCreateInfo();
		{
			ColorBlendState.logicOpEnable = VK_FALSE;
			ColorBlendState.logicOp = vk::LogicOp::eCopy;
			ColorBlendState.attachmentCount = 1;
			ColorBlendState.pAttachments = &ColorBlendAttachment;
			ColorBlendState.blendConstants[0] = 0.0f;
			ColorBlendState.blendConstants[1] = 0.0f;
			ColorBlendState.blendConstants[2] = 0.0f;
			ColorBlendState.blendConstants[3] = 0.0f;
		}
		PipelineInfo.pColorBlendState = &ColorBlendState;

		// Layout and Renderpass
		PipelineInfo.layout = CreateLayout();
		PipelineInfo.renderPass = CreateRenderPass();

		// Extra stuff
		PipelineInfo.basePipelineHandle = nullptr;

		// Create the pipeline
		try {
			m_vkPipeline = m_Devices->GetLogicalDevice().createGraphicsPipeline(nullptr, PipelineInfo).value;
		}
		catch (vk::SystemError err) {
			LOG_RENDER_FATAL("Failed to create graphics pipeline!");
		}

		// Cleanup
		VertexShader.Destroy();
		FragmentShader.Destroy();
	}


	void Pipeline::Destroy()
	{
		if (m_vkLayout)
			m_Devices->GetLogicalDevice().destroyPipelineLayout(m_vkLayout);

		if (m_vkRenderPass)
			m_Devices->GetLogicalDevice().destroyRenderPass(m_vkRenderPass);

		if (m_vkPipeline)
			m_Devices->GetLogicalDevice().destroyPipeline(m_vkPipeline);
	}


	vk::PipelineLayout Pipeline::CreateLayout()
	{
		vk::PipelineLayoutCreateInfo CreateInfo = vk::PipelineLayoutCreateInfo();
		{
			CreateInfo.setLayoutCount = 0;
			CreateInfo.pushConstantRangeCount = 0;
		}

		try {
			m_vkLayout = m_Devices->GetLogicalDevice().createPipelineLayout(CreateInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_FATAL("Failed to create pipeline layout!");
			return nullptr;
		}

		return m_vkLayout;
	}


	vk::RenderPass Pipeline::CreateRenderPass()
	{
		vk::AttachmentDescription ColorAttachment = vk::AttachmentDescription();
		{
			ColorAttachment.format = m_Swapchain->GetFormat();
			ColorAttachment.samples = vk::SampleCountFlagBits::e1;
			ColorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
			ColorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
			ColorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
			ColorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
			ColorAttachment.initialLayout = vk::ImageLayout::eUndefined;
			ColorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
		}
		vk::AttachmentReference ColorAttachmentRef = vk::AttachmentReference();
		{
			ColorAttachmentRef.attachment = 0;
			ColorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
		}
		vk::SubpassDescription Subpass = vk::SubpassDescription();
		{
			Subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
			Subpass.colorAttachmentCount = 1;
			Subpass.pColorAttachments = &ColorAttachmentRef;
		}
		vk::RenderPassCreateInfo RenderPass = vk::RenderPassCreateInfo();
		{
			RenderPass.attachmentCount = 1;
			RenderPass.pAttachments = &ColorAttachment;
			RenderPass.subpassCount = 1;
			RenderPass.pSubpasses = &Subpass;
		}

		try {
			m_vkRenderPass = m_Devices->GetLogicalDevice().createRenderPass(RenderPass);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_FATAL("Failed to create render pass!");
			return nullptr;
		}
		return m_vkRenderPass;
	}


} // namespace Helios::Vulkan

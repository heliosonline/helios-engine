#include "pch.h"

#include "Platform/Renderer/Vulkan/VKRendererAPI.h"

#include "HeliosEngine/Core/Assets.h"


namespace Helios {


	struct SimplePushConstantData
	{
		glm::mat2 transform{1.f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};


	void VKRendererAPI::Init()
	{
		LOG_RENDER_DEBUG("Initializing vulkan renderer...");

		m_Instance = CreateScope<Vulkan::Instance>();
		m_Device = CreateScope<Vulkan::Device>();

		CreatePipelineLayout();
		RecreateSwapchain();

		m_model = CreateRef<VKModel>();
	}


	void VKRendererAPI::Shutdown()
	{
		LOG_RENDER_DEBUG("Shutting down vulkan renderer...");

		m_Device->GetLogicalDevice().waitIdle();

		m_model.reset();

		m_Pipeline.reset();

		if (m_vkPipelineLayout)
			m_Device->GetLogicalDevice().destroyPipelineLayout(m_vkPipelineLayout);

		m_Swapchain.reset();
		m_Device.reset();
		m_Instance.reset();
	}


	void VKRendererAPI::Render()
	{
		uint32_t imageIndex;
		auto result = m_Swapchain->AcquireNextFrameIndex(&imageIndex);
		if (result == vk::Result::eErrorOutOfDateKHR)
		{
//			RecreateSwapchain();
			return;
		}
		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
			LOG_RENDER_EXCEPT("Failed to aquire swap chain image!");

		RecordDrawCommands(imageIndex);

		result = m_Swapchain->SubmitCommandBuffer(imageIndex);
		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
		{
//			RecreateSwapchain();
			return;
		}
		if (result != vk::Result::eSuccess)
			LOG_RENDER_EXCEPT("Failed to present swapchain image!");
	}


	void VKRendererAPI::OnWindowResize(uint32_t width, uint32_t height)
	{
		LOG_RENDER_TRACE("OnWindowResize - width:{} height:{}", width, height);
	}


	void VKRendererAPI::OnFramebufferResize(uint32_t width, uint32_t height)
	{
		LOG_RENDER_TRACE("OnFramebufferResize - width:{} height:{}", width, height);

		if (width == 0 or height == 0)
			return;

		RecreateSwapchain();
	}


	void VKRendererAPI::CreatePipelineLayout()
	{
		vk::PushConstantRange pushConstantRange = vk::PushConstantRange();
		{
			pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
			pushConstantRange.offset = 0;
			pushConstantRange.size = sizeof(SimplePushConstantData);
		}
		vk::PipelineLayoutCreateInfo layoutInfo = vk::PipelineLayoutCreateInfo();
		{
			layoutInfo.setLayoutCount = 0;
			layoutInfo.pSetLayouts = nullptr;
			layoutInfo.pushConstantRangeCount = 1;
			layoutInfo.pPushConstantRanges = &pushConstantRange;
		}
		try {
			LOG_RENDER_TRACE("Creating pipeline layout...");
			m_vkPipelineLayout = m_Device->GetLogicalDevice().createPipelineLayout(layoutInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to create framebuffer!");
		}
	}


	void VKRendererAPI::CreatePipeline()
	{
		m_Pipeline.reset();

		Vulkan::PipelineConfigInfo pipelineConfig{};
		Vulkan::Pipeline::DefaultConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = m_Swapchain->GetRenderPass();
		pipelineConfig.pipelineLayout = m_vkPipelineLayout;

		m_Pipeline = CreateScope<Vulkan::Pipeline>(
			"Shader/test.vert.spv",
			"Shader/test.frag.spv",
			pipelineConfig);
	}


	void VKRendererAPI::RecordDrawCommands(uint32_t imageIndex)
	{
		vk::CommandBuffer& commandBuffer = m_Swapchain->GetCommandBuffer(imageIndex);
		commandBuffer.reset();

		vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
		{
		}
		try {
			commandBuffer.begin(beginInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to begin recording command buffer!");
		}

		vk::RenderPassBeginInfo renderPassInfo = vk::RenderPassBeginInfo();
		{
			std::array<vk::ClearValue, 2> clearValues{};
			clearValues[0].color = vk::ClearColorValue{ 0.01f, 0.01f, 0.01f, 1.0f };
			clearValues[1].depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };
			renderPassInfo.renderPass = m_Swapchain->GetRenderPass();
			renderPassInfo.framebuffer = m_Swapchain->GetFrameBuffer(imageIndex);
			renderPassInfo.renderArea.offset.x = 0;
			renderPassInfo.renderArea.offset.y = 0;
			renderPassInfo.renderArea.extent = m_Swapchain->GetExtent();
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();
		}
		commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

		vk::Viewport viewport = vk::Viewport();
		{
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(m_Swapchain->GetExtent().width);
			viewport.height = static_cast<float>(m_Swapchain->GetExtent().height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
		}
		vk::Rect2D scissor({ 0, 0 }, m_Swapchain->GetExtent());
		commandBuffer.setViewport(0, 1, &viewport);
		commandBuffer.setScissor(0, 1, &scissor);

		m_Pipeline->Bind(commandBuffer);

		static int frame = 0;
		frame = (frame + 1) % 1000;

		m_model->vkBind(commandBuffer);
		for (int i = 0; i < 4; i++)
		{
			SimplePushConstantData push{};
			push.offset = { -0.5f + frame * 0.002f, -0.4f + i * 0.25f };
			push.color = { 0.0f, 0.0f, 0.2f + i * 0.2f };
			commandBuffer.pushConstants(m_vkPipelineLayout, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, sizeof(SimplePushConstantData), &push);
			m_model->vkDraw(commandBuffer);
		}

		commandBuffer.endRenderPass();

		try {
			commandBuffer.end();
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to end recording command buffer!");
		}
	}


	void VKRendererAPI::RecreateSwapchain()
	{
		m_Device->GetLogicalDevice().waitIdle();

//		m_Swapchain.reset();
		if (m_Swapchain)
			m_Swapchain = CreateRef<Vulkan::Swapchain>(std::move(m_Swapchain));
		else
			m_Swapchain = CreateRef<Vulkan::Swapchain>();

		CreatePipeline();
	}


} // namespace Helios

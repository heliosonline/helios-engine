#pragma once


namespace Helios {

	// unique pointers (type)
	template<typename T>
	using Scope = std::unique_ptr<T>;
	// unique pointers (create)
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}


	// shared pointers (type)
	template<typename T>
	using Ref = std::shared_ptr<T>;
	// shared pointers (create)
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}


	std::string GetExePath();


	uint32_t FastRnd(uint32_t seed);
	float RandomFloat(uint32_t& seed);


} // namespace Helios

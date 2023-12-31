#pragma once


namespace Helios {


	enum class EventType
	{
		// EventCategory::None
		None = 0,
		// EventCategory::EventCategoryApplication
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		FramebufferResize,
		AppTick, AppUpdate, AppRender,
		// EventCategory::EventCategoryKeyboard
		KeyPressed, KeyReleased, KeyTyped,
		// EventCategory::EventCategoryMouse
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};


	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = (1 << 0),
		EventCategoryInput = (1 << 1),
		EventCategoryKeyboard = (1 << 2),
		EventCategoryMouse = (1 << 3),
		EventCategoryMouseButton = (1 << 4)
	};


#define HE_EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
                               EventType GetEventType() const override { return GetStaticType(); }\
                               const char* GetName() const override { return #type; }

#define HE_EVENT_CLASS_CATEGORY(category) int GetCategoryFlags() const override { return category; }


	class Event
	{
	public:
		virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	};


	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		// F will be deduced by the compiler
		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};


	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}


} // namespace Helios


#define HE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

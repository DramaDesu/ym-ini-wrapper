#pragma once
#include <memory>
#include <string_view>
#include <vector>

namespace ym::ini
{
	struct handler
	{
		virtual ~handler() = default;

		template<typename T>
		T& get_impl() const
		{
			return *static_cast<T*>(impl);
		}

	protected:
		void* impl = nullptr;
	};

	using handler_t = std::unique_ptr<handler>;

	using value_t = std::string_view;
	using values_t = std::vector<value_t>;

	handler_t load(const char* path);

	bool has_section(const handler& in_handler, const char* in_section);

	values_t section_keys(const handler& in_handler, const char* in_section);
	values_t search_values(const handler& in_handler, const char* in_section, const char* in_path);

	bool has_value(const handler& in_handler, const char* in_section, const char* in_key);
	value_t get_value(const handler& in_handler, const char* in_section, const char* in_key);
	bool get_bool(const handler& in_handler, const char* in_section, const char* in_key, bool in_default = false);
	long get_long(const handler& in_handler, const char* in_section, const char* in_key, long in_default = 0);

	values_t get_values(const handler& in_handler, const char* in_section, const char* in_key);
	std::vector<bool> get_booleans(const handler& in_handler, const char* in_section, const char* in_key);
}

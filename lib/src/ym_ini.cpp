#include "include/ym_ini.h"

#include <format>
#include <iostream>
#include <ranges>
#include <stack>
#include <unordered_map>

#include "SimpleIni.h"

namespace 
{
	struct handler_impl : ym::ini::handler
	{
		handler_impl(std::unique_ptr<CSimpleIniA>&& in_ini) : ini(std::move(in_ini))
		{
			impl = ini.get();
		}
	private:
		std::unique_ptr<CSimpleIniA> ini;
	};

	std::vector<std::string_view> split_strings(const char* data, size_t size) {
		std::vector<std::string_view> result;

		const char* start = data;
		const char* end = data + size;

		while (start < end) {
			const char* next = std::find(start, end, '\0');
			if (start != next) {
				result.emplace_back(start, next - start);
			}
			start = next + 1;
		}

		return result;
	}

	struct path_t
	{
		path_t(const char* in_path, const char delimiter = '/')
		{
			const auto size = std::max<size_t>(strlen(in_path) + 1, 1);
			data.resize(size, 0);

			memcpy(data.data(), in_path, size - 1);

			std::ranges::replace(data, delimiter, 0);

			entries = split_strings(data.data(), data.size());
		}

		bool is_value() const
		{
			return !entries.empty() ? current_index == entries.size() - 1 : false;
		}

		const char* current() const
		{
			return entries[current_index].data();
		}

		path_t& push()
		{
			current_index = current_index < entries.size() - 1 ? current_index + 1 : current_index;
			return *this;
		}

		path_t& pop()
		{
			current_index = current_index > 0 ? current_index - 1 : current_index;
			return *this;
		}

	private:
		size_t current_index = 0;
		std::vector<std::string_view> entries;
		std::vector<char> data;
	};

	template<typename T>
	struct iterator_guard
	{
		iterator_guard(T& in_it) : it(in_it) {}

		~iterator_guard() { ++it; }

		std::string_view key() const { return it.key(); }
		std::string_view operator*() const { return *it; }

		operator bool() const { return it; }

		T& it;
	};

	struct ini_values_iterator
	{
		ini_values_iterator(const CSimpleIniA::TKeyVal& in_section, std::string_view in_key) : section_(in_section), key_(in_key)
		{
			forward();
		}

		std::string_view key() const { return key_; }

		std::string_view operator*() const { return out_value; }

		bool get_boolean() const {
			switch (out_value[0]) {
			case 't': case 'T': // true
			case 'y': case 'Y': // yes
			case '1':           // 1 (one)
				return true;

			case 'f': case 'F': // false
			case 'n': case 'N': // no
			case '0':           // 0 (zero)
				return false;

			case 'o': case 'O':
				if (out_value[1] == 'n' || out_value[1] == 'N') return true;  // on
				if (out_value[1] == 'f' || out_value[1] == 'F') return false; // off
				break;
			default: return false;
			}

			return false;
		}

		ini_values_iterator& operator++() {
			forward();
			return *this;
		}

		operator bool() const {
			return has_keys;
		}

	private:
		void forward()
		{
			has_keys = false;

			if (!found_keys)
			{
				if (auto target_key = section_.find(key_.data()); target_key != section_.cend())
				{
					has_keys = true;
					found_keys = true;
					out_key = key_;
					out_value = target_key->second;
					return;
				}
				found_keys = true;
			}
			if (found_keys)
			{
				const auto next_key = std::format("{}[{}]", key_, key_index++);
				if (auto target_key = section_.find(next_key.c_str()); target_key != section_.cend())
				{
					has_keys = true;
					out_key = target_key->first.pItem;
					out_value = target_key->second;
				}
			}
		}

		const CSimpleIniA::TKeyVal& section_;
		const std::string_view key_;

		std::string_view out_key;
		std::string_view out_value;

		bool found_keys = false;
		size_t key_index = 0;

		bool has_keys = true;
	};


	void search_values_impl(const CSimpleIniA& in_ini, std::string_view in_section, path_t& in_path, std::vector<std::string_view>& out_values)
	{
		if (const auto* section = in_ini.GetSection(in_section.data()))
		{
			if (in_path.is_value())
			{
				for (auto it = ini_values_iterator(*section, in_path.current()); it; ++it)
				{
					out_values.emplace_back(*it);
				}
			}
			else
			{
				const auto* key = in_path.current();
				in_path.push();

				for (auto it = ini_values_iterator(*section, key); it; ++it)
				{
					search_values_impl(in_ini, *it, in_path, out_values);
				}

				in_path.pop();
			}
		}
	}

	struct it_impl : ym::ini::path_iterator::impl
	{
		it_impl(CSimpleIniA& in_ini, const char* in_section, const char* in_path) : ini_(in_ini), section_(in_section), path_(in_path)
		{
			if (const auto* section = in_ini.GetSection(in_section))
			{
				values_it.emplace(*section , path_.current());
			}
		}

		void forward() override
		{
			has_next_values = false;
			while (!values_it.empty())
			{
				if (auto it = iterator_guard(values_it.top()))
				{
					if (path_.is_value())
					{
						has_next_values = true;
						current_value = *it;
						break;
					}

					const auto* section_name = (*it).data();
					if (const auto* section = ini_.GetSection(section_name))
					{
						values_mapping.insert_or_assign(it.key(), *it);

						path_.push();
						values_it.emplace(*section, path_.current());
					}
				}
				else
				{
					values_it.pop();
					path_.pop();
				}
			}
		}

		bool get_has_next_values() const override { return has_next_values; }
		std::string_view get_current_value() const override { return current_value; }

		std::string_view get_value(const char* in_path_part) const override
		{
			const auto it = values_mapping.find(in_path_part);
			return it != values_mapping.cend() ? it->second : "";
		}

	private:
		bool has_next_values = false;
		std::string_view current_value;

		CSimpleIniA& ini_;
		std::string_view section_;
		path_t path_;

		std::stack<ini_values_iterator> values_it;
		std::unordered_map<std::string_view, std::string_view> values_mapping;
	};
}

namespace ym::ini
{
	path_iterator::path_iterator(const handler& in_handler, const char* in_section, const char* in_path)
	{
		impl_ = std::make_unique<it_impl>(in_handler.get_impl<CSimpleIniA>(), in_section, in_path);
		impl_->forward();
	}

	value_t path_iterator::operator*() const
	{
		return impl_->get_current_value();
	}

	path_iterator& path_iterator::operator++()
	{
		impl_->forward();
		return *this;
	}

	path_iterator::operator bool() const
	{
		return impl_->get_has_next_values();
	}

	value_t path_iterator::get_value(const char* in_path_part) const
	{
		return impl_->get_value(in_path_part);
	}

	std::unique_ptr<handler> load(const char* path)
	{
		auto ini = std::make_unique<CSimpleIniA>();
		if (ini->LoadFile(path) == SI_OK)
		{
			return std::make_unique<handler_impl>(std::move(ini));
		}
		return nullptr;
	}

	bool has_section(const handler& in_handler, const char* in_section)
	{
		const auto& ini = in_handler.get_impl<CSimpleIniA>();
		return ini.GetSection(in_section) != nullptr;
	}

	std::vector<std::string_view> section_keys(const handler& in_handler, const char* in_section)
	{
		std::vector<std::string_view> out_keys;

		const auto& ini = in_handler.get_impl<CSimpleIniA>();
		if (const auto* section = ini.GetSection(in_section))
		{
			out_keys.reserve(section->size());
			for (const auto& value : *section | std::views::keys)
			{
				out_keys.emplace_back(value.pItem);
			}
		}

		return out_keys;
	}

	std::vector<std::string_view> search_values(const handler& in_handler, const char* in_section, const char* in_path)
	{
		std::vector<std::string_view> out_values;
		const auto& ini = in_handler.get_impl<CSimpleIniA>();
		path_t path(in_path);
		search_values_impl(ini, in_section, path, out_values);
		return out_values;
	}

	value_t get_value(const handler& in_handler, const char* in_section, const char* in_key)
	{
		const auto& ini = in_handler.get_impl<CSimpleIniA>();
		return ini.GetValue(in_section, in_key);
	}

	bool get_bool(const handler& in_handler, const char* in_section, const char* in_key, bool in_default)
	{
		const auto& ini = in_handler.get_impl<CSimpleIniA>();
		return ini.GetBoolValue(in_section, in_key, in_default);
	}

	long get_long(const handler& in_handler, const char* in_section, const char* in_key, long in_default)
	{
		const auto& ini = in_handler.get_impl<CSimpleIniA>();
		return ini.GetLongValue(in_section, in_key, in_default);
	}

	values_t get_values(const handler& in_handler, const char* in_section, const char* in_key)
	{
		std::vector<std::string_view> out_values;

		const auto& ini = in_handler.get_impl<CSimpleIniA>();
		if (const auto* section = ini.GetSection(in_section))
		{
			out_values.reserve(section->size());

			for (auto it = ini_values_iterator(*section, in_key); it; ++it)
			{
				out_values.emplace_back(*it);
			}

			out_values.shrink_to_fit();
		}

		return out_values;
	}

	std::vector<bool> get_booleans(const handler& in_handler, const char* in_section, const char* in_key)
	{
		std::vector<bool> out_values;

		const auto& ini = in_handler.get_impl<CSimpleIniA>();

		if (const auto* section = ini.GetSection(in_section))
		{
			out_values.reserve(section->size());

			for (auto it = ini_values_iterator(*section, in_key); it; ++it)
			{
				out_values.emplace_back(it.get_boolean());
			}

			out_values.shrink_to_fit();
		}

		return out_values;
	}

	bool has_value(const handler& in_handler, const char* in_section, const char* in_key)
	{
		const auto& ini = in_handler.get_impl<CSimpleIniA>();
		if (const auto* section = ini.GetSection(in_section))
		{
			for (auto it = ini_values_iterator(*section, in_key); it;)
			{
				return true;
			}
		}
		return false;
	}
}


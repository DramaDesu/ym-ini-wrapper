#include "ym_ini.h"

#include <format>
#include <iostream>

int main()
{
	if (const auto& handle = ym::ini::load("Data.ini"))
	{
		std::cout << "\n----------------rom_en_offsets--------------" << "\n\n";
		{
			auto&& keys = ym::ini::section_keys(*handle, "rom_en_offsets");
			for (auto key : keys)
			{
				std::cout << key << "\n";
			}
		}

		std::cout << "\n---------------rom/gems/bank-----------------" << "\n\n";
		{
			auto&& values = ym::ini::search_values(*handle, "Resources", "rom/gems/bank");
			for (auto key : values)
			{
				std::cout << key << "\n";
			}
		}

		std::cout << "\n----------------rom/path------------------" << "\n\n";
		{
			auto&& values = ym::ini::search_values(*handle, "Resources", "rom/path");
			for (auto key : values)
			{
				std::cout << key << "\n";
			}
		}

		std::cout << "\n------------------rom---------------------" << "\n\n";
		{
			auto&& values = ym::ini::get_values(*handle, "Resources", "rom");
			for (auto key : values)
			{
				std::cout << key << "\n";
			}
		}

		std::cout << "\n---------------data-----------------------" << "\n\n";
		{
			auto&& values = ym::ini::get_values(*handle, "Resources", "data");
			for (auto key : values)
			{
				std::cout << key << "\n";
			}
		}

		std::cout << "\n-------------------mask--------------------" << "\n\n";
		{
			auto&& values = ym::ini::get_booleans(*handle, "table_data", "mask");
			for (auto key : values)
			{
				std::cout << key << "\n";
			}
		}

		std::cout << "\n-------------------rom/gems/bank/instruments---------------------" << "\n\n";
		{
			auto&& values = ym::ini::search_values(*handle, "Resources", "rom/gems/bank/instruments");
			for (auto key : values)
			{
				std::cout << key << "\n";
			}
		}

		std::cout << "\n-------------------PATH---------------------" << "\n\n";
		for (auto it = ym::ini::path_iterator(*handle, "Resources", "rom/gems/bank"); it; ++it)
		{
			auto&& rom = it.get_value("rom");
			auto&& rom_path = ym::ini::get_value(*handle, rom.data(), "path");

			std::cout << std::format("bank={}, rom={}, rom_path={}\n", *it, rom, rom_path);
		}
	}

	std::cout << "\n---------------load---Data2.ini--------------" << "\n\n";
	if (const auto& handle = ym::ini::load("Data2.ini"))
	{
	}
	else
	{
		std::cout << "\n\nNo Data2.ini" << "\n";
	}

	return 0;
}

# ym-ini-wrapper
Yet More Ini Wrapper

Wrapper for sampleini (might support another backend) https://github.com/brofield/simpleini

Motivation:
- unified API, which is independent of sampleini (It is possible to substitute any backend to work with ini)
- Support find path for ini files

Find path example:
```
if (auto ini_handle = ym::ini::load(RESOURCE_PATH"/Settings/Data.ini")) {
  auto&& banks_names = ym::ini::search_values(*ini_handle, resources_section_name, "rom/gems/bank");
  for (auto&& bank_name : banks_names) {
    const auto instruments = ym::ini::get_long(*ini_handle, bank_name.data(), "instruments");
    const auto envelopes = ym::ini::get_long(*ini_handle, bank_name.data(), "envelopes");
    const auto sequences = ym::ini::get_long(*ini_handle, bank_name.data(), "sequences");
    const auto samples = ym::ini::get_long(*ini_handle, bank_name.data(), "samples");

    banks.emplace_back(bank_name.data(), instruments, envelopes, sequences, samples);
  }
}
```

API (WIP)

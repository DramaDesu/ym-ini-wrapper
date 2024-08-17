# ym-ini-wrapper
Yet More Ini Wrapper

Wrapper for sampleini https://github.com/brofield/simpleini

Motivation:
- unified API, which is independent of sampleini (It is possible to substitute the backend to work with ini)
- Support find path for ini files

Find path example:
auto&& banks = ym::ini::search_values(*handle, "Resources", "rom/gems/bank"); // will find all values along this path

API (WIP)

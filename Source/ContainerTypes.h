#pragma once

#include <functional>
#include <map>
#include <vector>

namespace Kurveball
{
	template<typename ContainedT>
	using Vector = std::vector<ContainedT>;
	
	template<typename KeyT, typename ValueT>
	using Map = std::map<KeyT, ValueT>;
	
	template<typename KeyT, typename ValueT>
	using Pair = std::pair<KeyT, ValueT>;

    template<typename T>
    using Optional = std::optional<T>;

    template<typename ReturnT, typename... Args>
    using Function = std::function<ReturnT(Args...)>;
}
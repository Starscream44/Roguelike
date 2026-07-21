#pragma once

namespace Roguelike
{
	class Block;

	class IBlockObserver
	{
	public:
		virtual ~IBlockObserver() = default;

		virtual void OnBlockDestroyed(const Block& block) = 0;
	};
}

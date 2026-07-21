#pragma once

#include "BlockObserver.h"

namespace Arkanoid
{
	class Block;

	class BlocksDestroyObserver final : public IBlockObserver
	{
	public:
		void Reset();
		void Observe(Block& block);

		void OnBlockDestroyed(const Block& block) override;

		bool AreAllBlocksDestroyed() const;
		int GetObservedBlocksCount() const;
		int GetDestroyedBlocksCount() const;

	private:
		int observedBlocksCount = 0;
		int destroyedBlocksCount = 0;
	};
}

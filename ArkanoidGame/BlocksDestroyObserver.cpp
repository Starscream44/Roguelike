#include "BlocksDestroyObserver.h"

#include "Block.h"

namespace Arkanoid
{
	void BlocksDestroyObserver::Reset()
	{
		observedBlocksCount = 0;
		destroyedBlocksCount = 0;
	}

	void BlocksDestroyObserver::Observe(Block& block)
	{
		block.AddObserver(this);
		++observedBlocksCount;
	}

	void BlocksDestroyObserver::OnBlockDestroyed(const Block&)
	{
		++destroyedBlocksCount;
	}

	bool BlocksDestroyObserver::AreAllBlocksDestroyed() const
	{
		return observedBlocksCount > 0 && destroyedBlocksCount >= observedBlocksCount;
	}

	int BlocksDestroyObserver::GetObservedBlocksCount() const
	{
		return observedBlocksCount;
	}

	int BlocksDestroyObserver::GetDestroyedBlocksCount() const
	{
		return destroyedBlocksCount;
	}
}

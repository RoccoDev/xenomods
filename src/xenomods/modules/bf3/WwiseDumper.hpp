#pragma once

#include <atomic>
#include <memory>

#include "../UpdatableModule.hpp"
#include "xenomods/engine/mm/mtl/FixStr.hpp"

namespace bf3 {
	struct [[gnu::packed]] WwiseDict {
		bool dirty;
		INSERT_PADDING_BYTES(3);
		mm::mtl::FixStr<32> current;
		mm::mtl::FixStr<32> previous;
		u32 currentFnvHash;
	};

	struct [[gnu::packed]] WwiseSheet {
		INSERT_PADDING_BYTES(0x5c);
		u32 flags;
		INSERT_PADDING_BYTES(4);
		WwiseDict dicts[7];
	};

	static std::atomic<WwiseSheet *> SHEET_PTR(nullptr);
} // namespace bf3

namespace xenomods {
	struct LockData {
		mm::mtl::FixStr<32> value;
		u32 fnvHash;
		bool locked;
		bool saved;
	};

	struct WwiseDumper : public xenomods::UpdatableModule {
		void Initialize() override;

		static void Render();
	};
} // namespace xenomods

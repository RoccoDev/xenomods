#pragma once

#include <atomic>
#include <memory>

#include "../UpdatableModule.hpp"
#include "xenomods/engine/mm/mtl/FixStr.hpp"

namespace bf3 {
	struct [[gnu::packed]] WwiseDict {
		mm::mtl::FixStr<32> value;
		INSERT_PADDING_BYTES(44);
	};

	struct [[gnu::packed]] WwiseSheet {
		INSERT_PADDING_BYTES(0x68);
		WwiseDict timeZone;
		WwiseDict map;
		WwiseDict field;
		WwiseDict battle;
		WwiseDict bossBgmPart;
		WwiseDict bgmEndPart;
		WwiseDict chainAttack;
	};

	static std::atomic<WwiseSheet *> SHEET_PTR(nullptr);
} // namespace bf3

namespace xenomods {
	struct WwiseDumper : public xenomods::UpdatableModule {
		void Initialize() override;

		static void Render();
	};
} // namespace xenomods

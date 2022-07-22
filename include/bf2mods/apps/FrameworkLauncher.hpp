//
// Created by block on 6/28/2022.
//

#pragma once

#include "../ml/camera.hpp"
#include "../utils.hpp"

namespace apps {

	class FrameworkLauncher {
	   public:
		INSERT_PADDING_BYTES(0x2b8); // travesty
		ml::Scn* ScnPtr;

		static FrameworkLauncher* s_instance;
	};

} // namespace apps
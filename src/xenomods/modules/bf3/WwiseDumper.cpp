#include <utility>

#include "WwiseDumper.hpp"
#include "xenomods/ImGuiExtensions.hpp"

namespace {
	struct GrabPtrHook : skylaunch::hook::Trampoline<GrabPtrHook> {
		static void Hook(bf3::WwiseSheet *ptr) {
			bf3::SHEET_PTR.store(ptr, std::memory_order_seq_cst);
			Orig(ptr);
		}
	};
}

namespace xenomods {
	void WwiseDumper::Initialize() {
		UpdatableModule::Initialize();
		g_Menu->RegisterRenderCallback(&Render);

		if (version::RuntimeVersion() == version::SemVer::v2_0_0) {
			GrabPtrHook::HookFromBase(0x7100d6776c);
		}
		else if (version::RuntimeVersion() == version::SemVer::v2_1_0) {
			GrabPtrHook::HookFromBase(0x7100d67a9c);
		}
		else if (version::RuntimeVersion() == version::SemVer::v2_1_1) {
			GrabPtrHook::HookFromBase(0x7100d67adc);
		}
		else if (version::RuntimeVersion() == version::SemVer::v2_2_0) {
			GrabPtrHook::HookFromBase(0x7100d6864c);
		}
	}

	void WwiseDumper::Render() {
		bf3::WwiseSheet *sheet = bf3::SHEET_PTR.load(std::memory_order_seq_cst);
		if (sheet == nullptr) {
			return;
		}

		if (!ImGui::Begin("Wwise Status")) {
			return;
		}

		std::pair<const char *, bf3::WwiseDict &> cells[] = {
			{"time_zone", sheet->timeZone},
			{"map", sheet->map},
			{"field", sheet->field},
			{"boss_bgm_part", sheet->bossBgmPart},
			{"bgm_endpart", sheet->bgmEndPart},
			{"chain_attack", sheet->chainAttack}
		};

		char buf[33];
		buf[32] = 0;

		ImGui::BeginTable("fields", 2);
		for (auto kv : cells) {
			memcpy(buf, kv.second.value.buffer, 32);
			buf[kv.second.value.m_nLen] = 0;

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text(kv.first);
			ImGui::TableNextColumn();
			ImGui::Text(buf);
		}
		ImGui::EndTable();
		ImGui::End();
	}
#if XENOMODS_CODENAME(bf3)
	XENOMODS_REGISTER_MODULE(WwiseDumper);
#endif

} // namespace xenomods

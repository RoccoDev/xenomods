#include <utility>

#include "WwiseDumper.hpp"
#include "xenomods/ImGuiExtensions.hpp"

static xenomods::LockData lockData[7] = {};
static std::array names = {
	"time_zone",
	"map",
	"field",
	"battle",
	"boss_bgm_part",
	"bgm_endpart",
	"chain_attack"
};

namespace {
	struct GrabPtrHook : skylaunch::hook::Trampoline<GrabPtrHook> {
		static void Hook(bf3::WwiseSheet *ptr) {
			bf3::SHEET_PTR.store(ptr, std::memory_order_seq_cst);
			Orig(ptr);
		}
	};

	struct PreTransitionHook : skylaunch::hook::Trampoline<PreTransitionHook> {
		static int Hook(bf3::WwiseSheet *sheet) {
			for (int i = 0; i < names.size(); i++) {
				if (lockData[i].locked) {
					sheet->dicts[i].currentFnvHash = lockData[i].fnvHash;
					memcpy(&sheet->dicts[i].current, &lockData[i].value, sizeof(sheet->dicts[i].current));
					sheet->dicts[i].dirty = false;
				}
			}

			return Orig(sheet);
		}
	};
}

namespace xenomods {
	void WwiseDumper::Initialize() {
		UpdatableModule::Initialize();
		g_Menu->RegisterRenderCallback(&Render);

		if (version::RuntimeVersion() == version::SemVer::v2_0_0) {
			GrabPtrHook::HookFromBase(0x7100d6776c);
			// TODO
		}
		else if (version::RuntimeVersion() == version::SemVer::v2_1_0) {
			GrabPtrHook::HookFromBase(0x7100d67a9c);
		}
		else if (version::RuntimeVersion() == version::SemVer::v2_1_1) {
			GrabPtrHook::HookFromBase(0x7100d67adc);
		}
		else if (version::RuntimeVersion() == version::SemVer::v2_2_0) {
			GrabPtrHook::HookFromBase(0x7100d6864c);
			PreTransitionHook::HookFromBase(0x7100d6b128);
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

		char buf[33];
		buf[32] = 0;

		if (!ImGui::BeginTable("fields", 4)) {
			ImGui::End();
			return;
		}

		std::array headers = {"Field", "Current", "Previous", "Freeze?"};
		ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
		for (auto s : headers) {
			ImGui::TableNextColumn();
			ImGui::Text(s);
		}

		for (int i = 0; i < names.size(); i++) {
			auto& dict = sheet->dicts[i];

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text(names[i]);
			ImGui::TableNextColumn();

			memcpy(buf, dict.current.buffer, 32);
			buf[dict.current.m_nLen] = 0;
			ImGui::Text(buf);

			ImGui::TableNextColumn();
			memcpy(buf, dict.previous.buffer, 32);
			buf[dict.previous.m_nLen] = 0;
			ImGui::Text(buf);

			ImGui::TableNextColumn();

			ImGui::PushID(i);
			ImGui::Checkbox("", &lockData[i].locked);
			ImGui::PopID();

			if (lockData[i].locked) {
				if (!lockData[i].saved) {
					lockData[i].fnvHash = dict.currentFnvHash;
					memcpy(&lockData[i].value, &dict.current, sizeof(dict.current));
					lockData[i].saved = true;
				}
			} else {
				lockData[i].saved = false;
			}
		}
		ImGui::EndTable();
		ImGui::End();
	}
#if XENOMODS_CODENAME(bf3)
	XENOMODS_REGISTER_MODULE(WwiseDumper);
#endif

} // namespace xenomods

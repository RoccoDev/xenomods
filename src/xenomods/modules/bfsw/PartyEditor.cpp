// Created by block on 6/18/23.

#include "PartyEditor.hpp"

#include "xenomods/engine/game/DocAccessor.hpp"
#include "xenomods/engine/game/Scripts.hpp"
#include "xenomods/engine/game/Utils.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

#if XENOMODS_CODENAME(bfsw) && _DEBUG
namespace xenomods {

	game::PcID PartyEditor::PartySetup[7] = {};

	void MenuGetParty() {
		game::DataParty* party = game::DataUtil::getDataParty(*DocumentPtr);
		if (party == nullptr)
			return;

		// clears and sets to party
		memset(&PartyEditor::PartySetup[0], 0, sizeof(PartyEditor::PartySetup));
		memcpy(&PartyEditor::PartySetup[0], &party->members[0], sizeof(game::PcID) * party->count);
	}

	void MenuApplyParty() {
		game::DataParty* party = game::DataUtil::getDataParty(*DocumentPtr);
		if (party == nullptr)
			return;

		std::vector<game::PcID> holder {};
		for(int i = 0; i < 7; ++i) {
			auto pcid = PartyEditor::PartySetup[i];
			if (pcid > game::PcID::None && pcid < game::PcID::Count)
				holder.push_back(pcid);
		}

		if (holder.size() <= 0) {
			g_Logger->ToastError(STRINGIFY(PartyEditor), "Party size is 0! Please choose some party members.");
			return;
		}

		for(int i = 0; i < holder.size(); ++i) {
			party->members[i] = holder[i];
		}
		party->count = holder.size();

		auto seqManager = reinterpret_cast<game::DocAccessor*>(&DocumentPtr)->getSeqManager();
		if(seqManager == nullptr)
			return;

		auto seqStack = seqManager->getField();
		if(seqStack == nullptr)
			return;

		// Refreshes party
		seqStack->addCommandImpl<game::SeqPartyMake>();

		// Open the party change dialog
		game::ScriptUtil::s_document = DocumentPtr;
		game::ScriptUI::openPartyChange();
	}

	/*void MenuLoadCharaStatus() {
		game::DataPc* member = game::DataUtil::getDataPc(*xenomods::DocumentPtr, static_cast<unsigned short>(game::PcID::Shulk));
		dbgutil::loadMemory(&member->status, "sd:/config/xenomods/memdump/DataCharaStatus - Dummy.dump");
	}

	void MenuDumpAllCharaStatuses() {
		for(unsigned short i = 1; i <= 15; i++) {
			game::DataPc* member = game::DataUtil::getDataPc(*DocumentPtr, i);
			game::PcID pcid = static_cast<game::PcID>(i);
			std::string filename = fmt::format("DataCharaStatus - {}.dump", pcid);
			dbgutil::dumpMemory(&member->status, sizeof(game::DataCharaStatus), filename.c_str());
		}
	}*/

	void PartyEditor::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up party editor...");

		auto modules = g_Menu->FindSection("modules");
		if(modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(PartyEditor), "Party Editor");
			//section->RegisterOption<void>("Dump all chara statuses", &MenuDumpAllCharaStatuses);
			//section->RegisterOption<void>("Load chara status", &MenuLoadCharaStatus);
			section->RegisterOption<game::PcID>(PartySetup[0], "PC 1");
			section->RegisterOption<game::PcID>(PartySetup[1], "PC 2");
			section->RegisterOption<game::PcID>(PartySetup[2], "PC 3");
			section->RegisterOption<game::PcID>(PartySetup[3], "PC 4");
			section->RegisterOption<game::PcID>(PartySetup[4], "PC 5");
			section->RegisterOption<game::PcID>(PartySetup[5], "PC 6");
			section->RegisterOption<game::PcID>(PartySetup[6], "PC 7");
			section->RegisterOption<void>("Apply Party", &MenuApplyParty);
			section->RegisterOption<void>("Get Party", &MenuGetParty);
		}
	}

	XENOMODS_REGISTER_MODULE(PartyEditor);

} // namespace xenomods
#endif
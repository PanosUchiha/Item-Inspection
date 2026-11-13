#include "Inspect.h"
#include "Lang.h"
#include "Menu.h"
#include "QuickLoot.h"
#include "Seen.h"
#include "Settings.h"
#include "SmoothCam.h"

namespace
{
	void OnMessage(SKSE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type) {
		case SKSE::MessagingInterface::kPostLoad:
			SmoothCam::Listen();
			QuickLoot::Connect();
			break;
		case SKSE::MessagingInterface::kPostPostLoad:
			SmoothCam::Request();
			break;
		case SKSE::MessagingInterface::kInputLoaded:
			Inspect::RegisterInput();
			break;
		case SKSE::MessagingInterface::kDataLoaded:
			Inspect::InstallPickUpHook();
			Settings::Load();
			Lang::Set(Settings::Get().language);
			Menu::Register();
			break;
		case SKSE::MessagingInterface::kPreLoadGame:
		case SKSE::MessagingInterface::kNewGame:
			Inspect::Reset();
			break;
		default:
			break;
		}
	}

	// Papyrus wrapper
	bool OfferItem(RE::StaticFunctionTag*, RE::TESForm* form, std::int32_t count)
	{
		const auto bound = form ? form->As<RE::TESBoundObject>() : nullptr;
		if (!bound) {
			logs::warn("ItemInspection.OfferItem: not a bound object");
			return false;
		}
		return Inspect::OfferItem(bound, count);
	}

	bool RegisterPapyrus(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("OfferItem", "ItemInspection", OfferItem);
		logs::info("Registered ItemInspection Papyrus functions");
		return true;
	}
}

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse, { .trampoline = true, .trampolineSize = 256 });
	logs::info("Item Inspection loading");

	Seen::Register();
	Inspect::InstallHooks();
	SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
	SKSE::GetPapyrusInterface()->Register(RegisterPapyrus);

	return true;
}

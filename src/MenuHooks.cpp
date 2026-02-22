#include "MenuHooks.h"
#include "InventoryCacheManager.h"
#include "ItemDataExtractor.h"
#include <cmath>
#include <format>
#include <spdlog/spdlog.h>

namespace {
    using ProcessMessageFn = RE::UI_MESSAGE_RESULTS (*)(RE::IMenu*, RE::UIMessage&);

    ProcessMessageFn _InventoryMenu_ProcessMessage;
    ProcessMessageFn _ContainerMenu_ProcessMessage;
    ProcessMessageFn _BarterMenu_ProcessMessage;

    // AS2: Math.round(x*100)/100
    inline double RoundTo2Decimals(double x)
    {
        return std::round(x * 100.0) / 100.0;
    }

    int32_t GetFilterType(RE::TESBoundObject* obj)
    {
        switch (obj->GetFormType()) {
            case RE::FormType::Weapon:      return 2;
            case RE::FormType::Armor:       return 1;
            case RE::FormType::Ammo:        return 2;
            case RE::FormType::Book:        return 4;
            case RE::FormType::Ingredient:  return 8;
            case RE::FormType::Light:       return 3;
            case RE::FormType::KeyMaster:   return 9;
            case RE::FormType::SoulGem:     return 12;
            case RE::FormType::Scroll:      return 7;
            case RE::FormType::Shout:       return 10;
            case RE::FormType::AlchemyItem: {
                auto* alch = obj->As<RE::AlchemyItem>();
                return (alch && alch->IsFood()) ? 5 : 6;
            }
            default: return 3;
        }
    }

    void PopulateInventory(bool bIsPlayer, RE::TESObjectREFR* refr)
    {
        auto* changes = refr->GetInventoryChanges(false);
        if (!changes || !changes->entryList)
            return;

        auto* cache    = InventoryCacheManager::GetSingleton();
        int32_t stored = 0;

        for (auto* entry : *changes->entryList) {
            if (!entry || !entry->object)
                continue;

            const char*       name        = entry->GetDisplayName();
            const int32_t     filterType  = GetFilterType(entry->object);
            const float       rawWeight   = entry->GetWeight();
            const int32_t     rawValue    = entry->GetValue();
            const bool        isEquipped  = entry->IsWorn();
            const RE::FormID  formID      = entry->object->GetFormID();
            // AS2: a_entryObject.baseId = a_entryObject.formId & 0x00FFFFFF (lower 24 bits; upper 8 = mod index)
            const std::uint32_t baseFormID = static_cast<std::uint32_t>(formID) & 0x00FFFFFFu;
            const bool        baseEnchant = entry->IsEnchanted();

            // AS2: infoValue = (value>0)? (Math.round(value*100)/100) : null; same for weight
            const float roundedWeight = static_cast<float>(RoundTo2Decimals(static_cast<double>(rawWeight)));
            const int32_t roundedValue = static_cast<int32_t>(RoundTo2Decimals(static_cast<double>(rawValue)));
            // AS2: infoValueWeight = (weight>0 && value>0)? Math.round(value/weight) : null
            // Use roundedWeight > 0 so we never divide by zero when rawWeight rounds down to 0 (e.g. < 0.005)
            const float vwr = (roundedWeight > 0.0f && rawValue > 0)
                                  ? static_cast<float>(std::round(static_cast<double>(roundedValue) / roundedWeight))
                                  : 0.0f;

            // Tempering (plan §6): Use base damage/armor here. If CommonLibSSE-NG exposes
            // GetAttackDamage/GetArmorRating(ExtraDataList*) or improved-item API, call it per stack and round.
            float rawInfoStat = 0.0f;
            if (auto* weap = entry->object->As<RE::TESObjectWEAP>())
                rawInfoStat = static_cast<float>(weap->GetAttackDamage());
            else if (auto* armo = entry->object->As<RE::TESObjectARMO>())
                rawInfoStat = armo->GetArmorRating();
            // AS2: infoArmor/infoDamage = (x>0)? (Math.round(x*100)/100) : null
            const float infoStat = static_cast<float>(RoundTo2Decimals(static_cast<double>(rawInfoStat)));

            // Pre-formatted strings for SkyUI textField assignment without AS conversion
            const std::string displayWeight =
                rawWeight > 0.0f ? std::format("{:.2f}", roundedWeight) : "";
            const std::string displayValue =
                rawValue > 0 ? std::format("{}", roundedValue) : "";
            const std::string displayValueWeight =
                (roundedWeight > 0.0f && rawValue > 0) ? std::format("{}", static_cast<int>(vwr)) : "";
            const std::string displayInfoStat =
                rawInfoStat > 0.0f ? std::format("{:.2f}", infoStat) : "";

            if (!entry->extraLists) {
                ExtendedItemData extended = ItemDataExtractor::Extract(entry->object, nullptr, baseFormID);
                cache->StoreItem(bIsPlayer, name, filterType, roundedWeight, roundedValue, vwr,
                                 false, baseEnchant, isEquipped, formID, baseFormID,
                                 entry->countDelta, infoStat,
                                 displayWeight, displayValue, displayValueWeight, displayInfoStat,
                                 extended, entry, nullptr);
                ++stored;
                continue;
            }

            int32_t stackSum = 0;
            for (auto* xList : *entry->extraLists) {
                if (!xList)
                    continue;

                ExtendedItemData extended = ItemDataExtractor::Extract(entry->object, xList, baseFormID);
                auto*         xCount     = xList->GetByType<RE::ExtraCount>();
                const int32_t count      = xCount ? static_cast<int32_t>(xCount->count) : 1;
                const bool    isStolen   = xList->HasType<RE::ExtraOwnership>();
                const bool    isEnchanted = baseEnchant || xList->HasType<RE::ExtraEnchantment>();

                stackSum += count;
                cache->StoreItem(bIsPlayer, name, filterType, roundedWeight, roundedValue, vwr,
                                 isStolen, isEnchanted, isEquipped, formID, baseFormID,
                                 count, infoStat,
                                 displayWeight, displayValue, displayValueWeight, displayInfoStat,
                                 extended, entry, xList);
                ++stored;
            }

            const int32_t pristine = entry->countDelta - stackSum;
            if (pristine > 0) {
                ExtendedItemData extended = ItemDataExtractor::Extract(entry->object, nullptr, baseFormID);
                cache->StoreItem(bIsPlayer, name, filterType, roundedWeight, roundedValue, vwr,
                                 false, baseEnchant, isEquipped, formID, baseFormID,
                                 pristine, infoStat,
                                 displayWeight, displayValue, displayValueWeight, displayInfoStat,
                                 extended, entry, nullptr);
                ++stored;
            }
        }

        spdlog::info("SCION: populated {} {} items", stored, bIsPlayer ? "player" : "target");
    }

    class RequestItemsHandler : public RE::GFxFunctionHandler {
    public:
        void Call(Params& a_params) override
        {
            if (a_params.argCount < 7)
                return;

            const bool        bIsPlayer = a_params.args[0].GetBool();
            const int32_t     columnId  = static_cast<int32_t>(a_params.args[1].GetNumber());
            const int32_t     stateId   = static_cast<int32_t>(a_params.args[2].GetNumber());
            const int32_t     catFilter = static_cast<int32_t>(a_params.args[3].GetNumber());
            const std::string search    = a_params.args[4].IsString()
                                              ? std::string(a_params.args[4].GetString()) : "";
            const int pageStart = static_cast<int>(a_params.args[5].GetNumber());
            const int pageSize  = static_cast<int>(a_params.args[6].GetNumber());

            auto* cache = InventoryCacheManager::GetSingleton();
            cache->UpdateActiveView(bIsPlayer, catFilter, columnId, stateId, search);

            const int total = cache->GetActiveViewCount(bIsPlayer);
            auto      page  = cache->GetPage(bIsPlayer, pageStart, pageSize);

            RE::GFxValue itemsArr;
            a_params.movie->CreateArray(&itemsArr);

            for (const auto& item : page) {
                const auto& e = item.Extended;
                RE::GFxValue entry;
                a_params.movie->CreateObject(&entry);
                entry.SetMember("text",            RE::GFxValue(item.Name.c_str()));
                entry.SetMember("count",           RE::GFxValue(static_cast<double>(item.Count)));
                entry.SetMember("filterFlag",      RE::GFxValue(static_cast<double>(item.FilterType)));
                entry.SetMember("infoWeight",      RE::GFxValue(static_cast<double>(item.Weight)));
                entry.SetMember("infoValue",       RE::GFxValue(static_cast<double>(item.Value)));
                entry.SetMember("infoValueWeight", RE::GFxValue(static_cast<double>(item.ValueWeightRatio)));
                entry.SetMember("catSort",         RE::GFxValue(static_cast<double>(item.FilterType)));
                entry.SetMember("typeSort",        RE::GFxValue(static_cast<double>(item.FilterType)));
                entry.SetMember("isStolen",        RE::GFxValue(item.IsStolen));
                entry.SetMember("isEnchanted",     RE::GFxValue(item.IsEnchanted));
                entry.SetMember("isEquipped",      RE::GFxValue(item.IsEquipped));
                entry.SetMember("formId",          RE::GFxValue(static_cast<double>(item.FormID)));
                entry.SetMember("baseId",          RE::GFxValue(static_cast<double>(item.BaseFormID)));
                entry.SetMember("infoStat",        RE::GFxValue(static_cast<double>(item.InfoStat)));
                entry.SetMember("sessionId",       RE::GFxValue(static_cast<double>(item.SessionID)));
                entry.SetMember("enabled",         RE::GFxValue(true));
                entry.SetMember("displayWeight",   RE::GFxValue(item.DisplayWeight.c_str()));
                entry.SetMember("displayValue",    RE::GFxValue(item.DisplayValue.c_str()));
                entry.SetMember("displayValueWeight", RE::GFxValue(item.DisplayValueWeight.c_str()));
                entry.SetMember("displayInfoStat", RE::GFxValue(item.DisplayInfoStat.c_str()));
                entry.SetMember("formType",       RE::GFxValue(static_cast<double>(e.FormType)));
                entry.SetMember("subType",        RE::GFxValue(static_cast<double>(e.SubType)));
                entry.SetMember("subTypeDisplay", RE::GFxValue(e.SubTypeDisplayKey.c_str()));
                entry.SetMember("weightClass",     RE::GFxValue(static_cast<double>(e.WeightClass)));
                entry.SetMember("weightClassDisplay", RE::GFxValue(e.WeightClassDisplayKey.c_str()));
                entry.SetMember("partMask",       RE::GFxValue(static_cast<double>(e.PartMask)));
                entry.SetMember("mainPartMask",    RE::GFxValue(static_cast<double>(e.MainPartMask)));
                entry.SetMember("material",       RE::GFxValue(static_cast<double>(e.Material)));
                entry.SetMember("materialDisplay", RE::GFxValue(e.MaterialDisplayKey.c_str()));
                entry.SetMember("isPoisoned",     RE::GFxValue(e.IsPoisoned));
                entry.SetMember("isRead",         RE::GFxValue(e.IsRead));
                entry.SetMember("duration",       RE::GFxValue(static_cast<double>(e.Duration)));
                entry.SetMember("magnitude",      RE::GFxValue(static_cast<double>(e.Magnitude)));
                entry.SetMember("status",         RE::GFxValue(static_cast<double>(e.SoulGemStatus)));
                itemsArr.PushBack(entry);
            }

            RE::GFxValue result;
            a_params.movie->CreateObject(&result);
            result.SetMember("items", itemsArr);
            result.SetMember("total", RE::GFxValue(static_cast<double>(total)));

            if (a_params.retVal)
                *a_params.retVal = result;
        }
    };

    template <class TMenu>
    void InjectRequestFunction(TMenu* menu)
    {
        auto& rtd = menu->GetRuntimeData();
        if (!menu->uiMovie || !rtd.root.IsObject())
            return;
        RE::GFxValue fn;
        menu->uiMovie->CreateFunction(&fn, new RequestItemsHandler());
        rtd.root.SetMember("SCION_RequestItems", fn);
    }

    template <class TMenu>
    void SignalReady(TMenu* menu)
    {
        auto& rtd = menu->GetRuntimeData();
        if (!menu->uiMovie || !rtd.root.IsObject())
            return;
        rtd.root.Invoke("InvalidateItemList", nullptr, nullptr, 0);
    }

    bool IsTriggerMessage(const RE::UIMessage& a_msg)
    {
        return a_msg.type == RE::UI_MESSAGE_TYPE::kShow ||
               a_msg.type == static_cast<RE::UI_MESSAGE_TYPE>(8);
    }

    RE::UI_MESSAGE_RESULTS InventoryMenu_ProcessMessage(RE::IMenu* a_this, RE::UIMessage& a_msg)
    {
        const bool trigger = IsTriggerMessage(a_msg);

        if (trigger) {
            InventoryCacheManager::GetSingleton()->ClearAll();
            if (auto* player = RE::PlayerCharacter::GetSingleton())
                PopulateInventory(true, player);
        }

        const auto result = _InventoryMenu_ProcessMessage(a_this, a_msg);

        if (trigger) {
            auto* typed = static_cast<RE::InventoryMenu*>(a_this);
            InjectRequestFunction(typed);
            SignalReady(typed);
        }

        return result;
    }

    RE::UI_MESSAGE_RESULTS ContainerMenu_ProcessMessage(RE::IMenu* a_this, RE::UIMessage& a_msg)
    {
        const bool trigger = IsTriggerMessage(a_msg);

        if (trigger) {
            InventoryCacheManager::GetSingleton()->ClearAll();
            if (auto* player = RE::PlayerCharacter::GetSingleton())
                PopulateInventory(true, player);

            auto handle = RE::ContainerMenu::GetTargetRefHandle();
            RE::NiPointer<RE::TESObjectREFR> targetRefr;
            if (RE::TESObjectREFR::LookupByHandle(handle, targetRefr) && targetRefr)
                PopulateInventory(false, targetRefr.get());
        }

        const auto result = _ContainerMenu_ProcessMessage(a_this, a_msg);

        if (trigger) {
            auto* typed = static_cast<RE::ContainerMenu*>(a_this);
            InjectRequestFunction(typed);
            SignalReady(typed);
        }

        return result;
    }

    RE::UI_MESSAGE_RESULTS BarterMenu_ProcessMessage(RE::IMenu* a_this, RE::UIMessage& a_msg)
    {
        const bool trigger = IsTriggerMessage(a_msg);

        if (trigger) {
            InventoryCacheManager::GetSingleton()->ClearAll();
            if (auto* player = RE::PlayerCharacter::GetSingleton())
                PopulateInventory(true, player);

            auto handle = RE::BarterMenu::GetTargetRefHandle();
            RE::NiPointer<RE::TESObjectREFR> targetRefr;
            if (RE::TESObjectREFR::LookupByHandle(handle, targetRefr) && targetRefr)
                PopulateInventory(false, targetRefr.get());
        }

        const auto result = _BarterMenu_ProcessMessage(a_this, a_msg);

        if (trigger) {
            auto* typed = static_cast<RE::BarterMenu*>(a_this);
            InjectRequestFunction(typed);
            SignalReady(typed);
        }

        return result;
    }
}

namespace MenuHooks {
    void InstallHooks()
    {
        REL::Relocation<std::uintptr_t> ivtbl{ RE::VTABLE_InventoryMenu[0] };
        _InventoryMenu_ProcessMessage = reinterpret_cast<ProcessMessageFn>(ivtbl.write_vfunc(4, &InventoryMenu_ProcessMessage));

        REL::Relocation<std::uintptr_t> cvtbl{ RE::VTABLE_ContainerMenu[0] };
        _ContainerMenu_ProcessMessage = reinterpret_cast<ProcessMessageFn>(cvtbl.write_vfunc(4, &ContainerMenu_ProcessMessage));

        REL::Relocation<std::uintptr_t> bvtbl{ RE::VTABLE_BarterMenu[0] };
        _BarterMenu_ProcessMessage = reinterpret_cast<ProcessMessageFn>(bvtbl.write_vfunc(4, &BarterMenu_ProcessMessage));

        spdlog::info("SCION: MenuHooks installed");
    }
}

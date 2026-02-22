#include "MenuHooks.h"
#include "InventoryCacheManager.h"
#include <spdlog/spdlog.h>

namespace {
    using ProcessMessageFn = RE::UI_MESSAGE_RESULTS (*)(RE::IMenu*, RE::UIMessage&);

    ProcessMessageFn _InventoryMenu_ProcessMessage;
    ProcessMessageFn _ContainerMenu_ProcessMessage;
    ProcessMessageFn _BarterMenu_ProcessMessage;

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

            const char*    name        = entry->GetDisplayName();
            const int32_t  filterType  = GetFilterType(entry->object);
            const float    weight      = entry->GetWeight();
            const int32_t  value       = entry->GetValue();
            const bool     isEquipped  = entry->IsWorn();
            const RE::FormID formID    = entry->object->GetFormID();
            const bool     baseEnchant = entry->IsEnchanted();
            const float    vwr         = weight > 0.0f ? static_cast<float>(value) / weight : 0.0f;

            float infoStat = 0.0f;
            if (auto* weap = entry->object->As<RE::TESObjectWEAP>())
                infoStat = static_cast<float>(weap->GetAttackDamage());
            else if (auto* armo = entry->object->As<RE::TESObjectARMO>())
                infoStat = armo->GetArmorRating();

            if (!entry->extraLists) {
                cache->StoreItem(bIsPlayer, name, filterType, weight, value, vwr,
                                 false, baseEnchant, isEquipped, formID,
                                 entry->countDelta, infoStat, entry, nullptr);
                ++stored;
                continue;
            }

            int32_t stackSum = 0;
            for (auto* xList : *entry->extraLists) {
                if (!xList)
                    continue;

                auto*         xCount     = xList->GetByType<RE::ExtraCount>();
                const int32_t count      = xCount ? static_cast<int32_t>(xCount->count) : 1;
                const bool    isStolen   = xList->HasType<RE::ExtraOwnership>();
                const bool    isEnchanted = baseEnchant || xList->HasType<RE::ExtraEnchantment>();

                stackSum += count;
                cache->StoreItem(bIsPlayer, name, filterType, weight, value, vwr,
                                 isStolen, isEnchanted, isEquipped, formID,
                                 count, infoStat, entry, xList);
                ++stored;
            }

            const int32_t pristine = entry->countDelta - stackSum;
            if (pristine > 0) {
                cache->StoreItem(bIsPlayer, name, filterType, weight, value, vwr,
                                 false, baseEnchant, isEquipped, formID,
                                 pristine, infoStat, entry, nullptr);
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
                entry.SetMember("infoStat",        RE::GFxValue(static_cast<double>(item.InfoStat)));
                entry.SetMember("sessionId",       RE::GFxValue(static_cast<double>(item.SessionID)));
                entry.SetMember("enabled",         RE::GFxValue(true));
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

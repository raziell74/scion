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

            if (!entry->extraLists) {
                cache->StoreItem(bIsPlayer, name, filterType, weight, value, vwr,
                                 false, baseEnchant, isEquipped, formID,
                                 entry->countDelta, entry, nullptr);
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
                                 count, entry, xList);
                ++stored;
            }

            const int32_t pristine = entry->countDelta - stackSum;
            if (pristine > 0) {
                cache->StoreItem(bIsPlayer, name, filterType, weight, value, vwr,
                                 false, baseEnchant, isEquipped, formID,
                                 pristine, entry, nullptr);
                ++stored;
            }
        }

        spdlog::info("SCION: populated {} {} items", stored, bIsPlayer ? "player" : "target");
    }

    template <class TMenu>
    void BypassGFxItemList(TMenu* menu)
    {
        auto& rtd = menu->GetRuntimeData();
        if (!menu->uiMovie || !rtd.root.IsObject())
            return;

        RE::GFxValue emptyArr;
        menu->uiMovie->CreateArray(&emptyArr);
        // Placeholder pending AS2 contract — update once scionui/src/ItemMenus/ defines the signal function
        rtd.root.Invoke("InvalidateItemList", nullptr, &emptyArr, 1);
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

        if (trigger)
            BypassGFxItemList(static_cast<RE::InventoryMenu*>(a_this));

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

        if (trigger)
            BypassGFxItemList(static_cast<RE::ContainerMenu*>(a_this));

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

        if (trigger)
            BypassGFxItemList(static_cast<RE::BarterMenu*>(a_this));

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

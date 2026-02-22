#include "InventoryCacheManager.h"
#include "util.h"

InventoryCacheManager* InventoryCacheManager::GetSingleton()
{
    static InventoryCacheManager instance;
    return &instance;
}

void InventoryCacheManager::ClearAll()
{
    std::unique_lock lock{ _mutex };
    _playerMasterList.clear();
    _targetMasterList.clear();
    _playerActiveView.clear();
    _targetActiveView.clear();
    _pointerMap.clear();
    _idCounter = 0;
}

uint64_t InventoryCacheManager::StoreItem(bool bIsPlayer,
                                          std::string_view name, int32_t filterType,
                                          float weight, int32_t value, float valueWeightRatio,
                                          bool isStolen, bool isEnchanted, bool isEquipped,
                                          RE::FormID formID, std::uint32_t baseFormID, int32_t count, float infoStat,
                                          std::string_view displayWeight, std::string_view displayValue,
                                          std::string_view displayValueWeight, std::string_view displayInfoStat,
                                          const ExtendedItemData& extended,
                                          RE::InventoryEntryData* entryData,
                                          RE::ExtraDataList* extraData)
{
    const uint64_t id = _idCounter.fetch_add(1, std::memory_order_relaxed);

    std::unique_lock lock{ _mutex };

    auto& masterList = bIsPlayer ? _playerMasterList : _targetMasterList;
    auto& activeView = bIsPlayer ? _playerActiveView : _targetActiveView;

    activeView.clear();

    CachedItem item{
        .SessionID          = id,
        .Name               = std::string(name),
        .FilterType         = filterType,
        .Weight             = weight,
        .Value              = value,
        .ValueWeightRatio   = valueWeightRatio,
        .IsStolen           = isStolen,
        .IsEnchanted        = isEnchanted,
        .IsEquipped         = isEquipped,
        .FormID             = formID,
        .BaseFormID         = baseFormID,
        .Count              = count,
        .InfoStat           = infoStat,
        .DisplayWeight      = std::string(displayWeight),
        .DisplayValue       = std::string(displayValue),
        .DisplayValueWeight = std::string(displayValueWeight),
        .DisplayInfoStat    = std::string(displayInfoStat),
        .Extended           = extended,
    };
    masterList.push_back(std::move(item));

    _pointerMap.emplace(id, ItemPointers{ entryData, extraData });

    return id;
}

void InventoryCacheManager::UpdateActiveView(bool bIsPlayer, int32_t categoryFilter,
                                             int32_t sortColumn, int32_t sortState,
                                             const std::string& searchString)
{
    std::unique_lock lock{ _mutex };

    auto& masterList = bIsPlayer ? _playerMasterList : _targetMasterList;
    auto& activeView = bIsPlayer ? _playerActiveView : _targetActiveView;

    activeView.clear();

    for (auto& item : masterList) {
        if (categoryFilter != -1 && item.FilterType != categoryFilter)
            continue;
        if (!searchString.empty() && !Util::String::iContains(item.Name, searchString))
            continue;
        activeView.push_back(&item);
    }

    switch (static_cast<SortColumn>(sortColumn)) {
    case SortColumn::Name:
        std::ranges::sort(activeView, [sortState](const CachedItem* a, const CachedItem* b) {
            if (sortState == 3 || sortState == 4) {
                if (a->IsStolen != b->IsStolen)
                    return sortState == 3 ? a->IsStolen > b->IsStolen : a->IsStolen < b->IsStolen;
            }
            const bool desc = sortState == 2;
            auto la = Util::String::ToLower(a->Name);
            auto lb = Util::String::ToLower(b->Name);
            if (la != lb)
                return desc ? la > lb : la < lb;
            return a->FormID < b->FormID;
        });
        break;
    case SortColumn::Value:
        std::ranges::sort(activeView, [sortState](const CachedItem* a, const CachedItem* b) {
            const bool desc = sortState == 1;
            if (a->Value != b->Value)
                return desc ? a->Value > b->Value : a->Value < b->Value;
            auto la = Util::String::ToLower(a->Name);
            auto lb = Util::String::ToLower(b->Name);
            if (la != lb)
                return la < lb;
            return a->FormID < b->FormID;
        });
        break;
    case SortColumn::Weight:
        std::ranges::sort(activeView, [sortState](const CachedItem* a, const CachedItem* b) {
            const bool desc = sortState == 1;
            if (a->Weight != b->Weight)
                return desc ? a->Weight > b->Weight : a->Weight < b->Weight;
            auto la = Util::String::ToLower(a->Name);
            auto lb = Util::String::ToLower(b->Name);
            if (la != lb)
                return la < lb;
            return a->FormID < b->FormID;
        });
        break;
    case SortColumn::ValueWeight:
        std::ranges::sort(activeView, [sortState](const CachedItem* a, const CachedItem* b) {
            const bool desc = sortState == 1;
            if (a->ValueWeightRatio != b->ValueWeightRatio)
                return desc ? a->ValueWeightRatio > b->ValueWeightRatio : a->ValueWeightRatio < b->ValueWeightRatio;
            auto la = Util::String::ToLower(a->Name);
            auto lb = Util::String::ToLower(b->Name);
            if (la != lb)
                return la < lb;
            return a->FormID < b->FormID;
        });
        break;
    case SortColumn::Damage:
    case SortColumn::Armor:
        std::ranges::sort(activeView, [sortState](const CachedItem* a, const CachedItem* b) {
            const bool desc = sortState == 1;
            if (a->InfoStat != b->InfoStat)
                return desc ? a->InfoStat > b->InfoStat : a->InfoStat < b->InfoStat;
            auto la = Util::String::ToLower(a->Name);
            auto lb = Util::String::ToLower(b->Name);
            if (la != lb)
                return la < lb;
            return a->FormID < b->FormID;
        });
        break;
    case SortColumn::Icon:
        std::ranges::sort(activeView, [sortState](const CachedItem* a, const CachedItem* b) {
            const bool desc = sortState == 2;
            if (a->FilterType != b->FilterType)
                return desc ? a->FilterType > b->FilterType : a->FilterType < b->FilterType;
            auto la = Util::String::ToLower(a->Name);
            auto lb = Util::String::ToLower(b->Name);
            if (la != lb)
                return desc ? la > lb : la < lb;
            return a->FormID < b->FormID;
        });
        break;
    }
}

std::vector<CachedItem> InventoryCacheManager::GetPage(bool bIsPlayer, int startIndex, int count)
{
    std::shared_lock lock{ _mutex };

    const auto& view = bIsPlayer ? _playerActiveView : _targetActiveView;

    const int end = std::min(startIndex + count, static_cast<int>(view.size()));
    if (startIndex >= end)
        return {};

    std::vector<CachedItem> page;
    page.reserve(static_cast<size_t>(end - startIndex));
    for (int i = startIndex; i < end; ++i)
        page.push_back(*view[i]);
    return page;
}

int InventoryCacheManager::GetActiveViewCount(bool bIsPlayer)
{
    std::shared_lock lock{ _mutex };
    const auto& view = bIsPlayer ? _playerActiveView : _targetActiveView;
    return static_cast<int>(view.size());
}

std::optional<ItemPointers> InventoryCacheManager::GetPointersByID(uint64_t sessionID)
{
    std::shared_lock lock{ _mutex };

    const auto it = _pointerMap.find(sessionID);
    if (it == _pointerMap.end())
        return std::nullopt;
    return it->second;
}

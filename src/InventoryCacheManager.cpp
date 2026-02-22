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
                                          RE::FormID formID, int32_t count,
                                          RE::InventoryEntryData* entryData,
                                          RE::ExtraDataList* extraData)
{
    const uint64_t id = _idCounter.fetch_add(1, std::memory_order_relaxed);

    std::unique_lock lock{ _mutex };

    auto& masterList = bIsPlayer ? _playerMasterList : _targetMasterList;
    auto& activeView = bIsPlayer ? _playerActiveView : _targetActiveView;

    activeView.clear();

    masterList.push_back(CachedItem{
        .SessionID       = id,
        .Name            = std::string(name),
        .FilterType      = filterType,
        .Weight          = weight,
        .Value           = value,
        .ValueWeightRatio = valueWeightRatio,
        .IsStolen        = isStolen,
        .IsEnchanted     = isEnchanted,
        .IsEquipped      = isEquipped,
        .FormID          = formID,
        .Count           = count,
    });

    _pointerMap.emplace(id, ItemPointers{ entryData, extraData });

    return id;
}

void InventoryCacheManager::UpdateActiveView(bool bIsPlayer, int32_t categoryFilter,
                                             int32_t sortColumn, bool bSortAscending,
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

    const bool asc = bSortAscending;
    switch (static_cast<SortColumn>(sortColumn)) {
    case SortColumn::Name:
        std::ranges::sort(activeView, [asc](const CachedItem* a, const CachedItem* b) {
            auto la = Util::String::ToLower(a->Name);
            auto lb = Util::String::ToLower(b->Name);
            return asc ? la < lb : la > lb;
        });
        break;
    case SortColumn::Weight:
        std::ranges::sort(activeView, [asc](const CachedItem* a, const CachedItem* b) {
            return asc ? a->Weight < b->Weight : a->Weight > b->Weight;
        });
        break;
    case SortColumn::Value:
        std::ranges::sort(activeView, [asc](const CachedItem* a, const CachedItem* b) {
            return asc ? a->Value < b->Value : a->Value > b->Value;
        });
        break;
    case SortColumn::ValueWeightRatio:
        std::ranges::sort(activeView, [asc](const CachedItem* a, const CachedItem* b) {
            return asc ? a->ValueWeightRatio < b->ValueWeightRatio
                       : a->ValueWeightRatio > b->ValueWeightRatio;
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

std::optional<ItemPointers> InventoryCacheManager::GetPointersByID(uint64_t sessionID)
{
    std::shared_lock lock{ _mutex };

    const auto it = _pointerMap.find(sessionID);
    if (it == _pointerMap.end())
        return std::nullopt;
    return it->second;
}

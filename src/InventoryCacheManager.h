#pragma once
#include <atomic>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

struct CachedItem {
    uint64_t    SessionID;
    std::string Name;
    int32_t     FilterType;
    float       Weight;
    int32_t     Value;
    float       ValueWeightRatio;
    bool        IsStolen;
    bool        IsEnchanted;
    bool        IsEquipped;
    RE::FormID  FormID;
    int32_t     Count;
};

struct ItemPointers {
    RE::InventoryEntryData* EntryData{ nullptr };
    RE::ExtraDataList*      ExtraData{ nullptr };
};

enum class SortColumn : int32_t {
    Name             = 0,
    Weight           = 1,
    Value            = 2,
    ValueWeightRatio = 3
};

class InventoryCacheManager {
public:
    static InventoryCacheManager* GetSingleton();

    void     ClearAll();

    uint64_t StoreItem(bool bIsPlayer,
                       std::string_view name, int32_t filterType,
                       float weight, int32_t value, float valueWeightRatio,
                       bool isStolen, bool isEnchanted, bool isEquipped,
                       RE::FormID formID, int32_t count,
                       RE::InventoryEntryData* entryData,
                       RE::ExtraDataList* extraData);

    void UpdateActiveView(bool bIsPlayer, int32_t categoryFilter,
                          int32_t sortColumn, bool bSortAscending,
                          const std::string& searchString);

    std::vector<CachedItem>      GetPage(bool bIsPlayer, int startIndex, int count);
    std::optional<ItemPointers>  GetPointersByID(uint64_t sessionID);

private:
    InventoryCacheManager() = default;

    std::vector<CachedItem>  _playerMasterList;
    std::vector<CachedItem>  _targetMasterList;
    std::vector<CachedItem*> _playerActiveView;
    std::vector<CachedItem*> _targetActiveView;
    std::unordered_map<uint64_t, ItemPointers> _pointerMap;
    std::atomic<uint64_t>    _idCounter{ 0 };
    mutable std::shared_mutex _mutex;
};

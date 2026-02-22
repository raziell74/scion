#pragma once
#include <atomic>
#include <cstdint>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

// Extended item data from ItemDataExtractor (formType, subType, material, etc.) for SkyUI
struct ExtendedItemData {
    int32_t     FormType{ 0 };
    int32_t     SubType{ -1 };           // SkyUI Item/Weapon/Armor subtype; -1 = null/OTHER
    std::string SubTypeDisplayKey;       // e.g. "$Weapon", "$Sword"
    int32_t     WeightClass{ -1 };       // Armor weight class; -1 = null
    std::string WeightClassDisplayKey;   // e.g. "$Light", "$Clothing"
    std::uint32_t PartMask{ 0 };
    std::uint32_t MainPartMask{ 0 };
    int32_t     Material{ -1 };          // SkyUI Material enum; -1 = other
    std::string MaterialDisplayKey;     // e.g. "$Daedric"
    bool        IsPoisoned{ false };
    bool        IsRead{ false };
    int32_t     BookType{ -1 };
    std::uint32_t Flags{ 0 };            // Book/ammo/alchemy flags
    float       Duration{ 0.0f };
    float       Magnitude{ 0.0f };
    int32_t     SoulGemStatus{ -1 };     // 0=empty, 1=partial, 2=full; -1 = N/A
};

struct CachedItem {
    uint64_t    SessionID;
    std::string Name;
    int32_t     FilterType;
    float       Weight;           // AS2: Math.round(weight*100)/100
    int32_t     Value;            // AS2: Math.round(value*100)/100
    float       ValueWeightRatio; // AS2: (weight>0&&value>0)? Math.round(value/weight) : 0
    bool        IsStolen;
    bool        IsEnchanted;
    bool        IsEquipped;
    RE::FormID  FormID;
    std::uint32_t BaseFormID;     // AS2: formId & 0x00FFFFFF (for icon mapping)
    int32_t     Count;
    float       InfoStat;         // AS2: Math.round(armor|damage * 100)/100
    // Pre-formatted display strings so SkyUI can assign textField.text without conversion
    std::string DisplayWeight;
    std::string DisplayValue;
    std::string DisplayValueWeight;
    std::string DisplayInfoStat;
    ExtendedItemData Extended;
};

struct ItemPointers {
    RE::InventoryEntryData* EntryData{ nullptr };
    RE::ExtraDataList*      ExtraData{ nullptr };
};

enum class SortColumn : int32_t {
    Icon        = 0,
    Name        = 1,
    Value       = 2,
    ValueWeight = 3,
    Weight      = 4,
    Damage      = 5,
    Armor       = 6,
};

class InventoryCacheManager {
public:
    static InventoryCacheManager* GetSingleton();

    void     ClearAll();

    uint64_t StoreItem(bool bIsPlayer,
                       std::string_view name, int32_t filterType,
                       float weight, int32_t value, float valueWeightRatio,
                       bool isStolen, bool isEnchanted, bool isEquipped,
                       RE::FormID formID, std::uint32_t baseFormID, int32_t count, float infoStat,
                       std::string_view displayWeight, std::string_view displayValue,
                       std::string_view displayValueWeight, std::string_view displayInfoStat,
                       const ExtendedItemData& extended,
                       RE::InventoryEntryData* entryData,
                       RE::ExtraDataList* extraData);

    void UpdateActiveView(bool bIsPlayer, int32_t categoryFilter,
                          int32_t sortColumn, int32_t sortState,
                          const std::string& searchString);

    std::vector<CachedItem>      GetPage(bool bIsPlayer, int startIndex, int count);
    int                          GetActiveViewCount(bool bIsPlayer);
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

#pragma once
#include "InventoryCacheManager.h"
#include "SkyUIDefines.h"

namespace ItemDataExtractor {
// Fills ExtendedItemData from obj (and optional extraList). baseFormId = formId & 0x00FFFFFF.
// Mirrors SkyUI InventoryDataSetter.as form-type and keyword logic.
ExtendedItemData Extract(RE::TESBoundObject* obj, RE::ExtraDataList* extraList, std::uint32_t baseFormId);
}

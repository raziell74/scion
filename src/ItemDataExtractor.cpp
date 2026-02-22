#include "ItemDataExtractor.h"
#include "SkyUIDefines.h"
#include <cstdint>
#include <string_view>

namespace {
using namespace SkyUI;

int MapFormType(RE::FormType ft)
{
    switch (ft) {
        case RE::FormType::Scroll:      return Form::TYPE_SCROLLITEM;
        case RE::FormType::Armor:       return Form::TYPE_ARMOR;
        case RE::FormType::Book:        return Form::TYPE_BOOK;
        case RE::FormType::Ingredient:  return Form::TYPE_INGREDIENT;
        case RE::FormType::Light:       return Form::TYPE_LIGHT;
        case RE::FormType::Misc:        return Form::TYPE_MISC;
        case RE::FormType::Weapon:      return Form::TYPE_WEAPON;
        case RE::FormType::Ammo:        return Form::TYPE_AMMO;
        case RE::FormType::KeyMaster:   return Form::TYPE_KEY;
        case RE::FormType::AlchemyItem: return Form::TYPE_POTION;
        case RE::FormType::SoulGem:     return Form::TYPE_SOULGEM;
        case RE::FormType::Shout:       return Form::TYPE_SHOUT;
        default:                        return Form::TYPE_NONE;
    }
}

bool HasKeyword(RE::TESForm* form, std::string_view editorID)
{
    auto* kw = form ? form->As<RE::BGSKeywordForm>() : nullptr;
    return kw && kw->HasKeywordString(editorID);
}

// AS2 processMaterialKeywords: ordered keyword -> (Material enum, display key)
void ResolveMaterial(RE::TESForm* form, int& material, std::string& materialDisplayKey)
{
    material = Material::OTHER;
    materialDisplayKey = "$Other";
    if (!form)
        return;
    // Order must match AS2 (e.g. Vampire before Leather)
    if (HasKeyword(form, "ArmorMaterialDaedric") || HasKeyword(form, "WeapMaterialDaedric")) {
        material = Material::DAEDRIC;
        materialDisplayKey = "$Daedric";
    } else if (HasKeyword(form, "ArmorMaterialDragonplate")) {
        material = Material::DRAGONPLATE;
        materialDisplayKey = "$Dragonplate";
    } else if (HasKeyword(form, "ArmorMaterialDragonscale")) {
        material = Material::DRAGONSCALE;
        materialDisplayKey = "$Dragonscale";
    } else if (HasKeyword(form, "ArmorMaterialDwarven") || HasKeyword(form, "WeapMaterialDwarven")) {
        material = Material::DWARVEN;
        materialDisplayKey = "$Dwarven";
    } else if (HasKeyword(form, "ArmorMaterialEbony") || HasKeyword(form, "WeapMaterialEbony")) {
        material = Material::EBONY;
        materialDisplayKey = "$Ebony";
    } else if (HasKeyword(form, "ArmorMaterialElven") || HasKeyword(form, "WeapMaterialElven")) {
        material = Material::ELVEN;
        materialDisplayKey = "$Elven";
    } else if (HasKeyword(form, "ArmorMaterialElvenGilded")) {
        material = Material::ELVENGILDED;
        materialDisplayKey = "$Elven Gilded";
    } else if (HasKeyword(form, "ArmorMaterialGlass") || HasKeyword(form, "WeapMaterialGlass")) {
        material = Material::GLASS;
        materialDisplayKey = "$Glass";
    } else if (HasKeyword(form, "ArmorMaterialHide")) {
        material = Material::HIDE;
        materialDisplayKey = "$Hide";
    } else if (HasKeyword(form, "ArmorMaterialImperialHeavy") || HasKeyword(form, "ArmorMaterialImperialLight") || HasKeyword(form, "WeapMaterialImperial")) {
        material = Material::IMPERIAL;
        materialDisplayKey = "$Imperial";
    } else if (HasKeyword(form, "ArmorMaterialImperialStudded")) {
        material = Material::IMPERIALSTUDDED;
        materialDisplayKey = "$Studded";
    } else if (HasKeyword(form, "ArmorMaterialIron") || HasKeyword(form, "WeapMaterialIron")) {
        material = Material::IRON;
        materialDisplayKey = "$Iron";
    } else if (HasKeyword(form, "ArmorMaterialIronBanded")) {
        material = Material::IRONBANDED;
        materialDisplayKey = "$Iron Banded";
    } else if (HasKeyword(form, "DLC1ArmorMaterialVampire")) {
        material = Material::VAMPIRE;
        materialDisplayKey = "$Vampire";
    } else if (HasKeyword(form, "ArmorMaterialLeather")) {
        material = Material::LEATHER;
        materialDisplayKey = "$Leather";
    } else if (HasKeyword(form, "ArmorMaterialOrcish") || HasKeyword(form, "WeapMaterialOrcish")) {
        material = Material::ORCISH;
        materialDisplayKey = "$Orcish";
    } else if (HasKeyword(form, "ArmorMaterialScaled")) {
        material = Material::SCALED;
        materialDisplayKey = "$Scaled";
    } else if (HasKeyword(form, "ArmorMaterialSteel") || HasKeyword(form, "WeapMaterialSteel")) {
        material = Material::STEEL;
        materialDisplayKey = "$Steel";
    } else if (HasKeyword(form, "ArmorMaterialSteelPlate")) {
        material = Material::STEELPLATE;
        materialDisplayKey = "$Steel Plate";
    } else if (HasKeyword(form, "ArmorMaterialStormcloak")) {
        material = Material::STORMCLOAK;
        materialDisplayKey = "$Stormcloak";
    } else if (HasKeyword(form, "ArmorMaterialStudded")) {
        material = Material::STUDDED;
        materialDisplayKey = "$Studded";
    } else if (HasKeyword(form, "DLC1ArmorMaterialDawnguard")) {
        material = Material::DAWNGUARD;
        materialDisplayKey = "$Dawnguard";
    } else if (HasKeyword(form, "DLC1ArmorMaterialFalmerHardened") || HasKeyword(form, "DLC1ArmorMaterialFalmerHeavy")) {
        material = Material::FALMERHARDENED;
        materialDisplayKey = "$Falmer Hardened";
    } else if (HasKeyword(form, "DLC1ArmorMaterialHunter")) {
        material = Material::HUNTER;
        materialDisplayKey = "$Hunter";
    } else if (HasKeyword(form, "DLC1LD_CraftingMaterialAetherium")) {
        material = Material::AETHERIUM;
        materialDisplayKey = "$Aetherium";
    } else if (HasKeyword(form, "DLC1WeapMaterialDragonbone")) {
        material = Material::DRAGONBONE;
        materialDisplayKey = "$Dragonbone";
    } else if (HasKeyword(form, "DLC2ArmorMaterialBonemoldHeavy") || HasKeyword(form, "DLC2ArmorMaterialBonemoldLight")) {
        material = Material::BONEMOLD;
        materialDisplayKey = "$Bonemold";
    } else if (HasKeyword(form, "DLC2ArmorMaterialChitinHeavy") || HasKeyword(form, "DLC2ArmorMaterialChitinLight")) {
        material = Material::CHITIN;
        materialDisplayKey = "$Chitin";
    } else if (HasKeyword(form, "DLC2ArmorMaterialMoragTong")) {
        material = Material::MORAGTONG;
        materialDisplayKey = "$Morag Tong";
    } else if (HasKeyword(form, "DLC2ArmorMaterialNordicHeavy") || HasKeyword(form, "DLC2ArmorMaterialNordicLight") || HasKeyword(form, "DLC2WeaponMaterialNordic")) {
        material = Material::NORDIC;
        materialDisplayKey = "$Nordic";
    } else if (HasKeyword(form, "DLC2ArmorMaterialStalhrimHeavy") || HasKeyword(form, "DLC2ArmorMaterialStalhrimLight") || HasKeyword(form, "DLC2WeaponMaterialStalhrim")) {
        material = Material::STALHRIM;
        materialDisplayKey = "$Stalhrim";
        if (HasKeyword(form, "DLC2dunHaknirArmor")) {
            material = Material::DEATHBRAND;
            materialDisplayKey = "$Deathbrand";
        }
    } else if (HasKeyword(form, "WeapMaterialDraugr")) {
        material = Material::DRAUGR;
        materialDisplayKey = "$Draugr";
    } else if (HasKeyword(form, "WeapMaterialDraugrHoned")) {
        material = Material::DRAUGRHONED;
        materialDisplayKey = "$Draugr Honed";
    } else if (HasKeyword(form, "WeapMaterialFalmer")) {
        material = Material::FALMER;
        materialDisplayKey = "$Falmer";
    } else if (HasKeyword(form, "WeapMaterialFalmerHoned")) {
        material = Material::FALMERHONED;
        materialDisplayKey = "$Falmer Honed";
    } else if (HasKeyword(form, "WeapMaterialSilver")) {
        material = Material::SILVER;
        materialDisplayKey = "$Silver";
    } else if (HasKeyword(form, "WeapMaterialWood")) {
        material = Material::WOOD;
        materialDisplayKey = "$Wood";
    }
}

void ProcessArmor(RE::TESObjectARMO* armor, std::uint32_t baseFormId, ExtendedItemData& out)
{
    ResolveMaterial(armor, out.Material, out.MaterialDisplayKey);
    // Weight class: CommonLibSSE-NG TESObjectARMO has no GetWeightClass(); infer from keywords and part mask only.
    out.WeightClass = -1;
    out.WeightClassDisplayKey = "$Other";
    if (HasKeyword(armor, "VendorItemClothing") || HasKeyword(armor, "VendorItemJewelry")) {
        out.WeightClass = HasKeyword(armor, "VendorItemClothing") ? Armor::WEIGHT_CLOTHING : Armor::WEIGHT_JEWELRY;
        out.WeightClassDisplayKey = (out.WeightClass == Armor::WEIGHT_CLOTHING) ? "$Clothing" : "$Jewelry";
    }
    // Part mask and mainPartMask -> subType (processArmorPartMask)
    auto* biped = armor->As<RE::BGSBipedObjectForm>();
    if (biped) {
        out.PartMask = static_cast<std::uint32_t>(biped->GetSlotMask());
        for (std::size_t i = 0; i < Armor::PARTMASK_PRECEDENCE_COUNT; ++i) {
            if (out.PartMask & Armor::PARTMASK_PRECEDENCE[i]) {
                out.MainPartMask = Armor::PARTMASK_PRECEDENCE[i];
                break;
            }
        }
        if (out.MainPartMask != 0) {
            switch (out.MainPartMask) {
                case Armor::PARTMASK_HEAD:
                case Armor::PARTMASK_HAIR:
                case Armor::PARTMASK_LONGHAIR:
                    out.SubType = (out.MainPartMask == Armor::PARTMASK_HEAD) ? Armor::EQUIP_HEAD : (out.MainPartMask == Armor::PARTMASK_HAIR ? Armor::EQUIP_HAIR : Armor::EQUIP_LONGHAIR);
                    out.SubTypeDisplayKey = "$Head";
                    break;
                case Armor::PARTMASK_BODY:  out.SubType = Armor::EQUIP_BODY;  out.SubTypeDisplayKey = "$Body"; break;
                case Armor::PARTMASK_HANDS: out.SubType = Armor::EQUIP_HANDS; out.SubTypeDisplayKey = "$Hands"; break;
                case Armor::PARTMASK_FOREARMS: out.SubType = Armor::EQUIP_FOREARMS; out.SubTypeDisplayKey = "$Forearms"; break;
                case Armor::PARTMASK_AMULET: out.SubType = Armor::EQUIP_AMULET; out.SubTypeDisplayKey = "$Amulet"; break;
                case Armor::PARTMASK_RING:   out.SubType = Armor::EQUIP_RING;   out.SubTypeDisplayKey = "$Ring"; break;
                case Armor::PARTMASK_FEET:   out.SubType = Armor::EQUIP_FEET;   out.SubTypeDisplayKey = "$Feet"; break;
                case Armor::PARTMASK_CALVES: out.SubType = Armor::EQUIP_CALVES; out.SubTypeDisplayKey = "$Calves"; break;
                case Armor::PARTMASK_SHIELD: out.SubType = Armor::EQUIP_SHIELD; out.SubTypeDisplayKey = "$Shield"; break;
                case Armor::PARTMASK_CIRCLET: out.SubType = Armor::EQUIP_CIRCLET; out.SubTypeDisplayKey = "$Circlet"; break;
                case Armor::PARTMASK_EARS:  out.SubType = Armor::EQUIP_EARS;  out.SubTypeDisplayKey = "$Ears"; break;
                case Armor::PARTMASK_TAIL:  out.SubType = Armor::EQUIP_TAIL;  out.SubTypeDisplayKey = "$Tail"; break;
                default: out.SubType = static_cast<int32_t>(out.MainPartMask); break;
            }
        }
    }
    // processArmorOther: infer weight class from part if still null
    if (out.WeightClass == -1 && out.MainPartMask != 0) {
        switch (out.MainPartMask) {
            case Armor::PARTMASK_HEAD:
            case Armor::PARTMASK_HAIR:
            case Armor::PARTMASK_LONGHAIR:
            case Armor::PARTMASK_BODY:
            case Armor::PARTMASK_HANDS:
            case Armor::PARTMASK_FOREARMS:
            case Armor::PARTMASK_FEET:
            case Armor::PARTMASK_CALVES:
            case Armor::PARTMASK_SHIELD:
            case Armor::PARTMASK_TAIL:
                out.WeightClass = Armor::WEIGHT_CLOTHING;
                out.WeightClassDisplayKey = "$Clothing";
                break;
            case Armor::PARTMASK_AMULET:
            case Armor::PARTMASK_RING:
            case Armor::PARTMASK_CIRCLET:
            case Armor::PARTMASK_EARS:
                out.WeightClass = Armor::WEIGHT_JEWELRY;
                out.WeightClassDisplayKey = "$Jewelry";
                break;
            default: break;
        }
    }
    // processArmorBaseId
    if (baseFormId == Form::BASEID_CLOTHESWEDDINGWREATH) {
        out.WeightClass = Armor::WEIGHT_JEWELRY;
        out.WeightClassDisplayKey = "$Jewelry";
    } else if (baseFormId == Form::BASEID_DLC1CLOTHESVAMPIRELORDARMOR) {
        out.SubType = Armor::EQUIP_BODY;
        out.SubTypeDisplayKey = "$Body";
    }
}

void ProcessWeapon(RE::TESObjectWEAP* weap, std::uint32_t baseFormId, RE::ExtraDataList* extraList, ExtendedItemData& out)
{
    ResolveMaterial(weap, out.Material, out.MaterialDisplayKey);
    out.IsPoisoned = extraList && extraList->HasType<RE::ExtraPoison>();
    out.SubType = Weapon::TYPE_MELEE;  // default
    out.SubTypeDisplayKey = "$Weapon";
    const auto wtype = weap->GetWeaponType();
    switch (wtype) {
        case RE::WEAPON_TYPE::kHandToHandMelee:
            out.SubType = Weapon::TYPE_MELEE; out.SubTypeDisplayKey = "$Melee"; break;
        case RE::WEAPON_TYPE::kOneHandSword:
            out.SubType = Weapon::TYPE_SWORD; out.SubTypeDisplayKey = "$Sword"; break;
        case RE::WEAPON_TYPE::kOneHandDagger:
            out.SubType = Weapon::TYPE_DAGGER; out.SubTypeDisplayKey = "$Dagger"; break;
        case RE::WEAPON_TYPE::kOneHandAxe:
            out.SubType = Weapon::TYPE_WARAXE; out.SubTypeDisplayKey = "$War Axe"; break;
        case RE::WEAPON_TYPE::kOneHandMace:
            out.SubType = Weapon::TYPE_MACE; out.SubTypeDisplayKey = "$Mace"; break;
        case RE::WEAPON_TYPE::kTwoHandSword:
            out.SubType = Weapon::TYPE_GREATSWORD; out.SubTypeDisplayKey = "$Greatsword"; break;
        case RE::WEAPON_TYPE::kTwoHandAxe: {
            out.SubType = Weapon::TYPE_BATTLEAXE;
            out.SubTypeDisplayKey = "$Battleaxe";
            if (HasKeyword(weap, "WeapTypeWarhammer")) {
                out.SubType = Weapon::TYPE_WARHAMMER;
                out.SubTypeDisplayKey = "$Warhammer";
            }
            break;
        }
        case RE::WEAPON_TYPE::kBow:
            out.SubType = Weapon::TYPE_BOW; out.SubTypeDisplayKey = "$Bow"; break;
        case RE::WEAPON_TYPE::kStaff:
            out.SubType = Weapon::TYPE_STAFF; out.SubTypeDisplayKey = "$Staff"; break;
        case RE::WEAPON_TYPE::kCrossbow:
            out.SubType = Weapon::TYPE_CROSSBOW; out.SubTypeDisplayKey = "$Crossbow"; break;
        default: break;
    }
    if (baseFormId == Form::BASEID_WEAPPICKAXE || baseFormId == Form::BASEID_SSDROCKSPLINTERPICKAXE || baseFormId == Form::BASEID_DUNVOLUNRUUDPICKAXE) {
        out.SubType = Weapon::TYPE_PICKAXE; out.SubTypeDisplayKey = "$Pickaxe";
    } else if (baseFormId == Form::BASEID_AXE01 || baseFormId == Form::BASEID_DUNHALTEDSTREAMPOACHERSAXE) {
        out.SubType = Weapon::TYPE_WOODAXE; out.SubTypeDisplayKey = "$Wood Axe";
    }
}

void ProcessAmmo(RE::TESAmmo* ammo, std::uint32_t baseFormId, ExtendedItemData& out)
{
    ResolveMaterial(ammo, out.Material, out.MaterialDisplayKey);
    const auto& rt = ammo->GetRuntimeData();
    if (ammo->IsBolt()) {
        out.SubType = Weapon::AMMO_BOLT;
        out.SubTypeDisplayKey = "$Bolt";
    } else {
        out.SubType = Weapon::AMMO_ARROW;
        out.SubTypeDisplayKey = "$Arrow";
    }
    out.Flags = static_cast<std::uint32_t>(rt.data.flags.underlying());
    // processAmmoBaseId: material overrides by base ID (subset)
    switch (baseFormId) {
        case Form::BASEID_DAEDRICARROW: out.Material = Material::DAEDRIC; out.MaterialDisplayKey = "$Daedric"; break;
        case Form::BASEID_EBONYARROW:   out.Material = Material::EBONY;   out.MaterialDisplayKey = "$Ebony"; break;
        case Form::BASEID_GLASSARROW:   out.Material = Material::GLASS;   out.MaterialDisplayKey = "$Glass"; break;
        case Form::BASEID_ELVENARROW:  out.Material = Material::ELVEN;   out.MaterialDisplayKey = "$Elven"; break;
        case Form::BASEID_DWARVENARROW: out.Material = Material::DWARVEN; out.MaterialDisplayKey = "$Dwarven"; break;
        case Form::BASEID_ORCISHARROW:  out.Material = Material::ORCISH;  out.MaterialDisplayKey = "$Orcish"; break;
        case Form::BASEID_NORDHEROARROW: out.Material = Material::NORDIC; out.MaterialDisplayKey = "$Nordic"; break;
        case Form::BASEID_DRAUGRARROW:  out.Material = Material::DRAUGR;  out.MaterialDisplayKey = "$Draugr"; break;
        case Form::BASEID_FALMERARROW:  out.Material = Material::FALMER;  out.MaterialDisplayKey = "$Falmer"; break;
        case Form::BASEID_STEELARROW:   out.Material = Material::STEEL;  out.MaterialDisplayKey = "$Steel"; break;
        case Form::BASEID_IRONARROW:    out.Material = Material::IRON;   out.MaterialDisplayKey = "$Iron"; break;
        case Form::BASEID_FORSWORNARROW: out.Material = Material::HIDE;  out.MaterialDisplayKey = "$Forsworn"; break;
        case Form::BASEID_DLC2RIEKLINGSPEARTHROWN: out.Material = Material::WOOD; out.MaterialDisplayKey = "$Wood"; out.SubTypeDisplayKey = "$Spear"; break;
        default: break;
    }
}

void ProcessBook(RE::TESObjectBOOK* book, ExtendedItemData& out)
{
    out.SubType = Item::OTHER;
    out.SubTypeDisplayKey = "$Book";
    out.Flags = static_cast<std::uint32_t>(book->data.flags.underlying());
    out.BookType = static_cast<int32_t>(book->data.type.underlying());
    out.IsRead = book->IsRead();
    if (book->IsNoteScroll() || out.BookType == Item::BOOKTYPE_NOTE) {
        out.SubType = Item::BOOK_NOTE;
        out.SubTypeDisplayKey = "$Note";
    }
    if (HasKeyword(book, "VendorItemRecipe")) {
        out.SubType = Item::BOOK_RECIPE;
        out.SubTypeDisplayKey = "$Recipe";
    } else if (HasKeyword(book, "VendorItemSpellTome")) {
        out.SubType = Item::BOOK_SPELLTOME;
        out.SubTypeDisplayKey = "$Spell Tome";
    }
}

void ProcessPotion(RE::AlchemyItem* alch, ExtendedItemData& out)
{
    out.SubType = Item::POTION_POTION;
    out.SubTypeDisplayKey = "$Potion";
    const auto* data = alch->GetData();
    if (!data) return;
    out.Flags = data->flags;
    if (alch->IsFood()) {
        out.SubType = Item::POTION_FOOD;
        out.SubTypeDisplayKey = "$Food";
        // consumptionSound not on MagicItem::Data in CommonLibSSE-NG; drink detection skipped
    } else if (alch->IsPoison()) {
        out.SubType = Item::POTION_POISON;
        out.SubTypeDisplayKey = "$Poison";
    } else {
        const auto av = alch->GetAssociatedSkill();
        if (static_cast<int>(av) == Actor::AV_HEALTH) {
            out.SubType = Item::POTION_HEALTH;
            out.SubTypeDisplayKey = "$Health";
        } else if (static_cast<int>(av) == Actor::AV_MAGICKA) {
            out.SubType = Item::POTION_MAGICKA;
            out.SubTypeDisplayKey = "$Magicka";
        } else if (static_cast<int>(av) == Actor::AV_STAMINA) {
            out.SubType = Item::POTION_STAMINA;
            out.SubTypeDisplayKey = "$Stamina";
        }
    }
}

void ProcessSoulGem(RE::TESSoulGem* soulgem, std::uint32_t baseFormId, ExtendedItemData& out)
{
    out.SubType = Item::OTHER;
    out.SubTypeDisplayKey = "$Soul Gem";
    const auto capacity = soulgem->GetMaximumCapacity();
    if (capacity != RE::SOUL_LEVEL::kNone) {
        out.SubType = static_cast<int32_t>(capacity);
    }
    if (baseFormId == Form::BASEID_DA01SOULGEMBLACKSTAR || baseFormId == Form::BASEID_DA01SOULGEMAZURASSTAR) {
        out.SubType = Item::SOULGEM_AZURA;
    }
    const auto contained = soulgem->GetContainedSoul();
    if (contained == RE::SOUL_LEVEL::kNone) {
        out.SoulGemStatus = 0;  // empty
    } else if (static_cast<int>(contained) >= static_cast<int>(capacity)) {
        out.SoulGemStatus = 2;  // full
    } else {
        out.SoulGemStatus = 1;  // partial
    }
}

void ProcessMisc(RE::TESObjectMISC* misc, std::uint32_t baseFormId, ExtendedItemData& out)
{
    out.SubType = Item::OTHER;
    out.SubTypeDisplayKey = "$Misc";
    if (HasKeyword(misc, "BYOHAdoptionClothesKeyword")) {
        out.SubType = Item::MISC_CHILDRENSCLOTHES; out.SubTypeDisplayKey = "$Clothing";
    } else if (HasKeyword(misc, "BYOHAdoptionToyKeyword")) {
        out.SubType = Item::MISC_TOY; out.SubTypeDisplayKey = "$Toy";
    } else if (HasKeyword(misc, "BYOHHouseCraftingCategoryWeaponRacks") || HasKeyword(misc, "BYOHHouseCraftingCategoryShelf") ||
               HasKeyword(misc, "BYOHHouseCraftingCategoryFurniture") || HasKeyword(misc, "BYOHHouseCraftingCategoryExterior") ||
               HasKeyword(misc, "BYOHHouseCraftingCategoryContainers") || HasKeyword(misc, "BYOHHouseCraftingCategoryBuilding") ||
               HasKeyword(misc, "BYOHHouseCraftingCategorySmithing")) {
        out.SubType = Item::MISC_HOUSEPART; out.SubTypeDisplayKey = "$House Part";
    } else if (HasKeyword(misc, "VendorItemDaedricArtifact")) {
        out.SubType = Item::MISC_ARTIFACT; out.SubTypeDisplayKey = "$Artifact";
    } else if (HasKeyword(misc, "VendorItemGem")) {
        out.SubType = Item::MISC_GEM; out.SubTypeDisplayKey = "$Gem";
    } else if (HasKeyword(misc, "VendorItemAnimalHide")) {
        out.SubType = Item::MISC_HIDE; out.SubTypeDisplayKey = "$Hide";
    } else if (HasKeyword(misc, "VendorItemTool")) {
        out.SubType = Item::MISC_TOOL; out.SubTypeDisplayKey = "$Tool";
    } else if (HasKeyword(misc, "VendorItemAnimalPart")) {
        out.SubType = Item::MISC_REMAINS; out.SubTypeDisplayKey = "$Remains";
    } else if (HasKeyword(misc, "VendorItemOreIngot")) {
        out.SubType = Item::MISC_INGOT; out.SubTypeDisplayKey = "$Ingot";
    } else if (HasKeyword(misc, "VendorItemClutter")) {
        out.SubType = Item::MISC_CLUTTER; out.SubTypeDisplayKey = "$Clutter";
    } else if (HasKeyword(misc, "VendorItemFirewood")) {
        out.SubType = Item::MISC_FIREWOOD; out.SubTypeDisplayKey = "$Firewood";
    }
    if (baseFormId == Form::BASEID_GEMAMETHYSTFLAWLESS) {
        out.SubType = Item::MISC_GEM; out.SubTypeDisplayKey = "$Gem";
    } else if (baseFormId == Form::BASEID_RUBYDRAGONCLAW || baseFormId == Form::BASEID_IVORYDRAGONCLAW || baseFormId == Form::BASEID_GLASSCLAW ||
               baseFormId == Form::BASEID_EBONYCLAW || baseFormId == Form::BASEID_EMERALDDRAGONCLAW || baseFormId == Form::BASEID_DIAMONDCLAW ||
               baseFormId == Form::BASEID_IRONCLAW || baseFormId == Form::BASEID_CORALDRAGONCLAW || baseFormId == Form::BASEID_E3GOLDENCLAW ||
               baseFormId == Form::BASEID_SAPPHIREDRAGONCLAW || baseFormId == Form::BASEID_MS13GOLDENCLAW) {
        out.SubType = Item::MISC_DRAGONCLAW; out.SubTypeDisplayKey = "$Claw";
    } else if (baseFormId == Form::BASEID_LOCKPICK) {
        out.SubType = Item::MISC_LOCKPICK; out.SubTypeDisplayKey = "$Lockpick";
    } else if (baseFormId == Form::BASEID_GOLD001) {
        out.SubType = Item::MISC_GOLD; out.SubTypeDisplayKey = "$Gold";
    } else if (baseFormId == Form::BASEID_LEATHER01) {
        out.SubTypeDisplayKey = "$Leather"; out.SubType = Item::MISC_LEATHER;
    } else if (baseFormId == Form::BASEID_LEATHERSTRIPS) {
        out.SubTypeDisplayKey = "$Strips"; out.SubType = Item::MISC_LEATHERSTRIPS;
    }
}
}

namespace ItemDataExtractor {
ExtendedItemData Extract(RE::TESBoundObject* obj, RE::ExtraDataList* extraList, std::uint32_t baseFormId)
{
    ExtendedItemData out;
    if (!obj)
        return out;
    out.FormType = MapFormType(obj->GetFormType());
    switch (obj->GetFormType()) {
        case RE::FormType::Scroll:
            out.SubTypeDisplayKey = "$Scroll";
            break;
        case RE::FormType::Armor: {
            auto* armo = obj->As<RE::TESObjectARMO>();
            if (armo) ProcessArmor(armo, baseFormId, out);
            break;
        }
        case RE::FormType::Book: {
            auto* book = obj->As<RE::TESObjectBOOK>();
            if (book) ProcessBook(book, out);
            break;
        }
        case RE::FormType::Ingredient:
            out.SubTypeDisplayKey = "$Ingredient";
            break;
        case RE::FormType::Light:
            out.SubTypeDisplayKey = "$Torch";
            break;
        case RE::FormType::Misc: {
            auto* misc = obj->As<RE::TESObjectMISC>();
            if (misc) ProcessMisc(misc, baseFormId, out);
            break;
        }
        case RE::FormType::Weapon: {
            auto* weap = obj->As<RE::TESObjectWEAP>();
            if (weap) ProcessWeapon(weap, baseFormId, extraList, out);
            break;
        }
        case RE::FormType::Ammo: {
            auto* ammo = obj->As<RE::TESAmmo>();
            if (ammo) ProcessAmmo(ammo, baseFormId, out);
            break;
        }
        case RE::FormType::KeyMaster:
            out.SubTypeDisplayKey = "$Key";
            break;
        case RE::FormType::AlchemyItem: {
            auto* alch = obj->As<RE::AlchemyItem>();
            if (alch) ProcessPotion(alch, out);
            break;
        }
        case RE::FormType::SoulGem: {
            auto* sg = obj->As<RE::TESSoulGem>();
            if (sg) ProcessSoulGem(sg, baseFormId, out);
            break;
        }
        default:
            break;
    }
    return out;
}
}

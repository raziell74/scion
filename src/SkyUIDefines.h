#pragma once

// SkyUI numeric constants matching scionui/src/Common/skyui/defines/*.as
// Used so C++ and AS stay in sync for formType, subType, material, etc.

namespace SkyUI {
namespace Form {
    constexpr int TYPE_NONE         = 0;
    constexpr int TYPE_SCROLLITEM  = 23;
    constexpr int TYPE_ARMOR       = 26;
    constexpr int TYPE_BOOK        = 27;
    constexpr int TYPE_INGREDIENT  = 30;
    constexpr int TYPE_LIGHT       = 31;
    constexpr int TYPE_MISC        = 32;
    constexpr int TYPE_WEAPON     = 41;
    constexpr int TYPE_AMMO       = 42;
    constexpr int TYPE_KEY        = 45;
    constexpr int TYPE_POTION     = 46;
    constexpr int TYPE_SOULGEM    = 52;
    constexpr int TYPE_SHOUT      = 119;
    constexpr int BOOKTYPE_NOTE   = 0xFF;
    constexpr std::uint32_t FORMID_ITMPotionUse = 0x000B6435;
    // Base IDs (formId & 0x00FFFFFF)
    constexpr std::uint32_t BASEID_CLOTHESWEDDINGWREATH     = 0x08895A;
    constexpr std::uint32_t BASEID_DLC1CLOTHESVAMPIRELORDARMOR = 0x011A84;
    constexpr std::uint32_t BASEID_WEAPPICKAXE    = 0x0E3C16;
    constexpr std::uint32_t BASEID_SSDROCKSPLINTERPICKAXE = 0x06A707;
    constexpr std::uint32_t BASEID_DUNVOLUNRUUDPICKAXE = 0x1019D4;
    constexpr std::uint32_t BASEID_AXE01         = 0x02F2F4;
    constexpr std::uint32_t BASEID_DUNHALTEDSTREAMPOACHERSAXE = 0x0AE086;
    constexpr std::uint32_t BASEID_DA01SOULGEMBLACKSTAR = 0x063B29;
    constexpr std::uint32_t BASEID_DA01SOULGEMAZURASSTAR = 0x063B27;
    constexpr std::uint32_t BASEID_DAEDRICARROW  = 0x0139C0;
    constexpr std::uint32_t BASEID_EBONYARROW    = 0x0139BF;
    constexpr std::uint32_t BASEID_GLASSARROW    = 0x0139BE;
    constexpr std::uint32_t BASEID_ELVENARROW    = 0x0139BD;
    constexpr std::uint32_t BASEID_DWARVENARROW  = 0x0139BC;
    constexpr std::uint32_t BASEID_ORCISHARROW   = 0x0139BB;
    constexpr std::uint32_t BASEID_NORDHEROARROW = 0x0EAFDF;
    constexpr std::uint32_t BASEID_DRAUGRARROW   = 0x034182;
    constexpr std::uint32_t BASEID_FALMERARROW   = 0x038341;
    constexpr std::uint32_t BASEID_STEELARROW    = 0x01397F;
    constexpr std::uint32_t BASEID_IRONARROW    = 0x01397D;
    constexpr std::uint32_t BASEID_FORSWORNARROW = 0x0CEE9E;
    constexpr std::uint32_t BASEID_DLC2RIEKLINGSPEARTHROWN = 0x017720;
    constexpr std::uint32_t BASEID_LOCKPICK      = 0x00000A;
    constexpr std::uint32_t BASEID_GOLD001       = 0x00000F;
    constexpr std::uint32_t BASEID_LEATHER01    = 0x0DB5D2;
    constexpr std::uint32_t BASEID_LEATHERSTRIPS = 0x0800E4;
    constexpr std::uint32_t BASEID_GEMAMETHYSTFLAWLESS = 0x06851E;
    constexpr std::uint32_t BASEID_RUBYDRAGONCLAW = 0x04B56C;
    constexpr std::uint32_t BASEID_IVORYDRAGONCLAW = 0x0AB7BB;
    constexpr std::uint32_t BASEID_GLASSCLAW     = 0x07C260;
    constexpr std::uint32_t BASEID_EBONYCLAW    = 0x05AF48;
    constexpr std::uint32_t BASEID_EMERALDDRAGONCLAW = 0x0ED417;
    constexpr std::uint32_t BASEID_DIAMONDCLAW  = 0x0AB375;
    constexpr std::uint32_t BASEID_IRONCLAW     = 0x08CDFA;
    constexpr std::uint32_t BASEID_CORALDRAGONCLAW = 0x0B634C;
    constexpr std::uint32_t BASEID_E3GOLDENCLAW = 0x0999E7;
    constexpr std::uint32_t BASEID_SAPPHIREDRAGONCLAW = 0x0663D7;
    constexpr std::uint32_t BASEID_MS13GOLDENCLAW = 0x039647;
}

namespace Armor {
    constexpr int WEIGHT_LIGHT    = 0;
    constexpr int WEIGHT_HEAVY    = 1;
    constexpr int WEIGHT_NONE     = 2;
    constexpr int WEIGHT_CLOTHING = 3;
    constexpr int WEIGHT_JEWELRY  = 4;
    constexpr std::uint32_t PARTMASK_HEAD     = 0x00000001;
    constexpr std::uint32_t PARTMASK_HAIR     = 0x00000002;
    constexpr std::uint32_t PARTMASK_BODY     = 0x00000004;
    constexpr std::uint32_t PARTMASK_HANDS    = 0x00000008;
    constexpr std::uint32_t PARTMASK_FOREARMS = 0x00000010;
    constexpr std::uint32_t PARTMASK_AMULET  = 0x00000020;
    constexpr std::uint32_t PARTMASK_RING    = 0x00000040;
    constexpr std::uint32_t PARTMASK_FEET    = 0x00000080;
    constexpr std::uint32_t PARTMASK_CALVES  = 0x00000100;
    constexpr std::uint32_t PARTMASK_SHIELD  = 0x00000200;
    constexpr std::uint32_t PARTMASK_TAIL    = 0x00000400;
    constexpr std::uint32_t PARTMASK_LONGHAIR = 0x00000800;
    constexpr std::uint32_t PARTMASK_CIRCLET = 0x00001000;
    constexpr std::uint32_t PARTMASK_EARS    = 0x00002000;
    constexpr int EQUIP_HEAD     = 0;
    constexpr int EQUIP_HAIR     = 1;
    constexpr int EQUIP_LONGHAIR = 2;
    constexpr int EQUIP_BODY     = 3;
    constexpr int EQUIP_FOREARMS = 4;
    constexpr int EQUIP_HANDS    = 5;
    constexpr int EQUIP_SHIELD   = 6;
    constexpr int EQUIP_CALVES   = 7;
    constexpr int EQUIP_FEET     = 8;
    constexpr int EQUIP_CIRCLET  = 9;
    constexpr int EQUIP_AMULET   = 10;
    constexpr int EQUIP_EARS     = 11;
    constexpr int EQUIP_RING     = 12;
    constexpr int EQUIP_TAIL     = 13;
    // First set bit in this order gives mainPartMask -> subType
    inline constexpr std::uint32_t PARTMASK_PRECEDENCE[] = {
        PARTMASK_BODY, PARTMASK_HAIR, PARTMASK_HANDS, PARTMASK_FOREARMS,
        PARTMASK_FEET, PARTMASK_CALVES, PARTMASK_SHIELD, PARTMASK_AMULET,
        PARTMASK_RING, PARTMASK_LONGHAIR, PARTMASK_EARS, PARTMASK_HEAD,
        PARTMASK_CIRCLET, PARTMASK_TAIL
    };
    constexpr std::size_t PARTMASK_PRECEDENCE_COUNT = 14;
}

namespace Weapon {
    constexpr int ANIM_HANDTOHANDMELEE = 0;
    constexpr int ANIM_ONEHANDSWORD    = 1;
    constexpr int ANIM_ONEHANDDAGGER   = 2;
    constexpr int ANIM_ONEHANDAXE      = 3;
    constexpr int ANIM_ONEHANDMACE     = 4;
    constexpr int ANIM_TWOHANDSWORD    = 5;
    constexpr int ANIM_TWOHANDAXE      = 6;
    constexpr int ANIM_BOW             = 7;
    constexpr int ANIM_STAFF           = 8;
    constexpr int ANIM_CROSSBOW        = 9;
    constexpr int ANIM_H2H             = 10;
    constexpr int ANIM_1HS             = 11;
    constexpr int ANIM_1HD             = 12;
    constexpr int ANIM_1HA             = 13;
    constexpr int ANIM_1HM             = 14;
    constexpr int ANIM_2HS             = 15;
    constexpr int ANIM_2HA             = 16;
    constexpr int ANIM_BOW2            = 17;
    constexpr int ANIM_STAFF2          = 18;
    constexpr int ANIM_CBOW            = 19;
    constexpr int TYPE_MELEE      = 0;
    constexpr int TYPE_SWORD      = 1;
    constexpr int TYPE_DAGGER     = 2;
    constexpr int TYPE_WARAXE     = 3;
    constexpr int TYPE_MACE       = 4;
    constexpr int TYPE_GREATSWORD = 5;
    constexpr int TYPE_BATTLEAXE  = 6;
    constexpr int TYPE_WARHAMMER  = 7;
    constexpr int TYPE_BOW        = 8;
    constexpr int TYPE_CROSSBOW   = 9;
    constexpr int TYPE_STAFF      = 10;
    constexpr int TYPE_PICKAXE    = 11;
    constexpr int TYPE_WOODAXE    = 12;
    constexpr int AMMO_ARROW      = 0;
    constexpr int AMMO_BOLT       = 1;
    constexpr std::uint32_t AMMOFLAG_NONBOLT = 0x0004;
}

namespace Item {
    constexpr int OTHER = -1;
    constexpr int BOOK_SPELLTOME = 0;
    constexpr int BOOK_NOTE      = 1;
    constexpr int BOOK_RECIPE    = 2;
    constexpr int BOOKTYPE_NOTE  = 0xFF;  // OBJ_BOOK::Type::kNoteScroll
    constexpr int BOOKFLAG_READ  = 0x08;
    constexpr int POTION_POTION  = 12;
    constexpr int POTION_DRINK   = 13;
    constexpr int POTION_FOOD    = 14;
    constexpr int POTION_POISON  = 15;
    constexpr int POTION_HEALTH  = 0;
    constexpr int POTION_MAGICKA = 3;
    constexpr int POTION_STAMINA = 6;
    constexpr std::uint32_t ALCHFLAG_FOOD   = 0x00002;
    constexpr std::uint32_t ALCHFLAG_POISON = 0x20000;
    constexpr int SOULGEMSTATUS_EMPTY   = 0;
    constexpr int SOULGEMSTATUS_PARTIAL = 1;
    constexpr int SOULGEMSTATUS_FULL    = 2;
    constexpr int SOULGEM_NONE  = 0;
    constexpr int SOULGEM_PETTY = 1;
    constexpr int SOULGEM_LESSER = 2;
    constexpr int SOULGEM_COMMON = 3;
    constexpr int SOULGEM_GREATER = 4;
    constexpr int SOULGEM_GRAND  = 5;
    constexpr int SOULGEM_AZURA  = 6;
    constexpr int MISC_GEM = 0;
    constexpr int MISC_DRAGONCLAW = 1;
    constexpr int MISC_ARTIFACT = 2;
    constexpr int MISC_LEATHER = 3;
    constexpr int MISC_LEATHERSTRIPS = 4;
    constexpr int MISC_HIDE = 5;
    constexpr int MISC_REMAINS = 6;
    constexpr int MISC_INGOT = 7;
    constexpr int MISC_TOOL = 8;
    constexpr int MISC_CHILDRENSCLOTHES = 9;
    constexpr int MISC_TOY = 10;
    constexpr int MISC_FIREWOOD = 11;
    constexpr int MISC_HOUSEPART = 18;
    constexpr int MISC_CLUTTER = 19;
    constexpr int MISC_LOCKPICK = 20;
    constexpr int MISC_GOLD = 21;
}

namespace Actor {
    constexpr int AV_HEALTH  = 24;
    constexpr int AV_MAGICKA = 25;
    constexpr int AV_STAMINA = 26;
}

namespace Material {
    constexpr int OTHER = -1;
    constexpr int DAEDRIC = 6;
    constexpr int DRAGONPLATE = 9;
    constexpr int DRAGONSCALE = 10;
    constexpr int DWARVEN = 14;
    constexpr int EBONY = 15;
    constexpr int ELVEN = 16;
    constexpr int ELVENGILDED = 17;
    constexpr int GLASS = 22;
    constexpr int HIDE = 23;
    constexpr int IMPERIAL = 25;
    constexpr int IMPERIALSTUDDED = 26;
    constexpr int IRON = 27;
    constexpr int IRONBANDED = 28;
    constexpr int LEATHER = 29;
    constexpr int ORCISH = 34;
    constexpr int SCALED = 35;
    constexpr int STEEL = 38;
    constexpr int STEELPLATE = 39;
    constexpr int STORMCLOAK = 40;
    constexpr int STUDDED = 41;
    constexpr int VAMPIRE = 42;
    constexpr int WOOD = 43;
    constexpr int DAWNGUARD = 7;
    constexpr int FALMERHARDENED = 19;
    constexpr int HUNTER = 24;
    constexpr int AETHERIUM = 0;
    constexpr int DRAGONBONE = 8;
    constexpr int BONEMOLD = 2;
    constexpr int CHITIN = 4;
    constexpr int MORAGTONG = 31;
    constexpr int NORDIC = 33;
    constexpr int STALHRIM = 37;
    constexpr int DEATHBRAND = 13;
    constexpr int DRAUGR = 11;
    constexpr int DRAUGRHONED = 12;
    constexpr int FALMER = 18;
    constexpr int FALMERHONED = 20;
    constexpr int SILVER = 36;
}
}

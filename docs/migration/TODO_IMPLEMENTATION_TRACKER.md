# TODO Implementation Tracker

**Created:** 2026-01-06
**Total TODOs:** 283
**Status:** In Progress

---

## Priority 1: Core Combat System (Critical Path)

### AnimNotify States (Block gameplay if not implemented)
| File | Line | TODO | Status |
|------|------|------|--------|
| SLFAnimNotifyStateInputBuffer.cpp | 17 | ToggleBuffer(true) on NotifyBegin | ⬜ |
| SLFAnimNotifyStateInputBuffer.cpp | 28 | ToggleBuffer(false) on NotifyEnd | ⬜ |
| SLFAnimNotifyStateInvincibility.cpp | 14 | Set IsInvincible = true | ⬜ |
| SLFAnimNotifyStateInvincibility.cpp | 22 | Set IsInvincible = false | ⬜ |
| SLFAnimNotifyStateHyperArmor.cpp | 14 | Set HyperArmor = true | ⬜ |
| SLFAnimNotifyStateHyperArmor.cpp | 22 | Set HyperArmor = false | ⬜ |
| SLFAnimNotifyStateWeaponTrace.cpp | 19 | Enable weapon trace | ⬜ |
| SLFAnimNotifyStateWeaponTrace.cpp | 32 | Perform sphere trace | ⬜ |
| SLFAnimNotifyStateWeaponTrace.cpp | 43 | Disable weapon trace | ⬜ |
| SLFAnimNotifyStateFistTrace.cpp | 22 | Sphere trace at fist | ⬜ |

### Combat Manager (4 TODOs)
| File | Line | TODO | Status |
|------|------|------|--------|
| AC_CombatManager.cpp | 730 | Play hit reaction animation | ⬜ |
| AC_CombatManager.cpp | 813 | Apply status effects | ⬜ |
| AC_CombatManager.cpp | 859 | Spawn currency pickup | ⬜ |
| AC_CombatManager.cpp | 1270 | Play poise break animation | ⬜ |

---

## Priority 2: Equipment System (7 TODOs in AC_EquipmentManager)

| File | Line | TODO | Status |
|------|------|------|--------|
| AC_EquipmentManager.cpp | 265 | Look up spawned actor from map | ⬜ |
| AC_EquipmentManager.cpp | 375 | Convert tag to item data | ⬜ |
| AC_EquipmentManager.cpp | 431 | Convert items to FInstancedStruct | ⬜ |
| AC_EquipmentManager.cpp | 644 | Get guard sequence from shield | ⬜ |
| AC_EquipmentManager.cpp | 726 | Track stance state | ⬜ |
| AC_EquipmentManager.cpp | 742 | Adjust movement based on weight | ⬜ |
| AC_EquipmentManager.cpp | 846 | Get from equipped shield data | ⬜ |

---

## Priority 3: Stat System (10 TODOs in StatManagerComponent)

| File | Line | TODO | Status |
|------|------|------|--------|
| StatManagerComponent.cpp | 47 | Load stat definitions from table | ⬜ |
| StatManagerComponent.cpp | 57 | Apply override values | ⬜ |
| StatManagerComponent.cpp | 76 | Extract FStatInfo from B_Stat | ⬜ |
| StatManagerComponent.cpp | 124 | Call Reset on B_Stat | ⬜ |
| StatManagerComponent.cpp | 138 | Adjust stat value on B_Stat | ⬜ |
| StatManagerComponent.cpp | 147 | Trigger regen for stat | ⬜ |
| StatManagerComponent.cpp | 168 | Get StatsToAffect from B_Stat | ⬜ |
| StatManagerComponent.cpp | 211 | Create FInstancedStruct for save | ⬜ |
| StatManagerComponent.cpp | 221 | Load stat data | ⬜ |
| StatManagerComponent.cpp | 237 | Call ToggleRegen on B_Stat | ⬜ |

---

## Priority 4: Status Effect System (7 TODOs)

| File | Line | TODO | Status |
|------|------|------|--------|
| StatusEffectManagerComponent.cpp | 39 | Get tag from data asset | ⬜ |
| StatusEffectManagerComponent.cpp | 52 | Start continuous buildup | ⬜ |
| StatusEffectManagerComponent.cpp | 63 | Stop continuous buildup | ⬜ |
| StatusEffectManagerComponent.cpp | 79 | Get effect tag from class | ⬜ |
| StatusEffectManagerComponent.cpp | 91 | Spawn UObject from class | ⬜ |
| StatusEffectManagerComponent.cpp | 120 | Get source data asset | ⬜ |
| StatusEffectManagerComponent.cpp | 146 | Create instance and init | ⬜ |

---

## Priority 5: Character Base (13 TODOs)

| File | Line | TODO | Status |
|------|------|------|--------|
| SLFBaseCharacter.cpp | Multiple | Interface calls and component getters | ⬜ |

---

## Priority 6: Ladder System (12 TODOs)

| File | Line | TODO | Status |
|------|------|------|--------|
| LadderManagerComponent.cpp | 178-268 | All ladder montages and movement | ⬜ |

---

## Priority 7: Inventory System (8 TODOs)

| File | Line | TODO | Status |
|------|------|------|--------|
| InventoryManagerComponent.cpp | Multiple | Item use, dropping, filtering | ⬜ |

---

## Priority 8: Save/Load System (4 TODOs)

| File | Line | TODO | Status |
|------|------|------|--------|
| SaveLoadManagerComponent.cpp | Multiple | Serialize inventory/equipment | ⬜ |

---

## Priority 9: Widgets (~100 TODOs)

Most are "Cache widget references" placeholders - low priority.

---

## Implementation Progress

| Priority | Category | Total | Done | Remaining |
|----------|----------|-------|------|-----------|
| 1 | Combat AnimNotifies | 10 | 0 | 10 |
| 2 | Equipment Manager | 7 | 0 | 7 |
| 3 | Stat Manager | 10 | 0 | 10 |
| 4 | Status Effects | 7 | 0 | 7 |
| 5 | Base Character | 13 | 0 | 13 |
| 6 | Ladder System | 12 | 0 | 12 |
| 7 | Inventory | 8 | 0 | 8 |
| 8 | Save/Load | 4 | 0 | 4 |
| 9 | Widgets | ~100 | 0 | ~100 |
| - | Other | ~112 | 0 | ~112 |
| **TOTAL** | | **283** | **0** | **283** |

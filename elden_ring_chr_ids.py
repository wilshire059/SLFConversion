"""
Elden Ring Character ID Reference
==================================
Maps cXXXX chr IDs to enemy/NPC names and body types.

Sources:
  - https://github.com/vawser/ER-Documentation/blob/main/Info%20-%20Chr%20IDs.txt
  - https://soulsmodding.com/doku.php?id=er-refmat:character-list
  - https://gist.github.com/gracenotes/745ee2d07878f81067fc23f43670a808
  - https://www.nexusmods.com/eldenring/articles/539

Types:
  humanoid        - Human-sized bipedal (soldiers, knights, NPCs)
  humanoid_large  - Oversized bipedal (trolls, fire prelates, golems)
  quadruped       - Four-legged (dogs, wolves, bears, horses)
  flying          - Airborne (hawks, bats, dragonflies, avionettes)
  large_creature  - Giant non-humanoid (dragons, erdtree avatars, worms)
  boss            - Major/demigod boss with unique moveset
  npc             - Non-hostile or story NPC
  ambient         - Passive wildlife
  object          - Siege weapon, vehicle, or intractable object
  unknown         - Unidentified or dummy/placeholder
"""

ELDEN_RING_CHR_IDS = {
    # =========================================================================
    # SYSTEM / DUMMY (c0000 - c1000)
    # =========================================================================
    "c0000": {"name": "Player Character", "type": "humanoid"},
    "c0100": {"name": "Dummy (Generic)", "type": "unknown"},
    "c0110": {"name": "Caravan Dummy", "type": "unknown"},
    "c0120": {"name": "Bullet Dummy", "type": "unknown"},
    "c0130": {"name": "Cutscene Dummy", "type": "unknown"},
    "c1000": {"name": "Talk Dummy", "type": "unknown"},

    # =========================================================================
    # NAMED NPCs & DEMIGODS (c2000 - c2299)
    # =========================================================================
    "c2010": {"name": "Blaidd", "type": "npc"},
    "c2030": {"name": "Rennala, Queen of the Full Moon", "type": "boss"},
    "c2031": {"name": "Rennala (Phase 2)", "type": "boss"},
    "c2040": {"name": "Juvenile Scholar / Rennala Student", "type": "humanoid"},
    "c2041": {"name": "Larva of Rot / Small Pest", "type": "large_creature"},
    "c2050": {"name": "Ranni the Witch", "type": "npc"},
    "c2060": {"name": "The Two Fingers", "type": "npc"},
    "c2110": {"name": "Beast Clergyman / Maliketh", "type": "boss"},
    "c2120": {"name": "Malenia, Blade of Miquella", "type": "boss"},
    "c2130": {"name": "Morgott, the Omen King", "type": "boss"},
    "c2131": {"name": "Morgott (Dead)", "type": "boss"},
    "c2140": {"name": "Fell Twin / Omen", "type": "humanoid_large"},
    "c2150": {"name": "Lightning Ball (Empty/Placeholder)", "type": "unknown"},
    "c2160": {"name": "Finger Reader Crone (Wooden Chair)", "type": "npc"},
    "c2170": {"name": "Finger Reader Crone (No Chair)", "type": "npc"},
    "c2180": {"name": "Melina", "type": "npc"},
    "c2190": {"name": "Radagon of the Golden Order", "type": "boss"},
    "c2200": {"name": "Elden Beast", "type": "boss"},

    # Crabs (c2270 - c2277)
    "c2270": {"name": "Giant Crab", "type": "large_creature"},
    "c2271": {"name": "Crab", "type": "quadruped"},
    "c2272": {"name": "Giant Crab (Damaged/Black)", "type": "large_creature"},
    "c2273": {"name": "Crab (Maggots/Black)", "type": "quadruped"},
    "c2274": {"name": "Giant Crab (Clean/Albinauric)", "type": "large_creature"},
    "c2275": {"name": "Crab (Clean/Albinauric)", "type": "quadruped"},
    "c2276": {"name": "Giant Crab (Golden Tufts/Death)", "type": "large_creature"},
    "c2277": {"name": "Crab (Golden Tufts/Death)", "type": "quadruped"},

    # =========================================================================
    # HUMANOID ENEMIES (c2500 - c3999)
    # =========================================================================
    "c2500": {"name": "Crucible Knight", "type": "humanoid"},
    "c3000": {"name": "Exile Soldier (Castle)", "type": "humanoid"},
    "c3010": {"name": "Banished Knight", "type": "humanoid"},
    "c3020": {"name": "Large Castle Exile", "type": "humanoid_large"},
    "c3060": {"name": "Undead Mariner", "type": "humanoid"},
    "c3061": {"name": "Undead Azula Beastman", "type": "humanoid"},
    "c3070": {"name": "Dominula Celebrant", "type": "humanoid"},
    "c3080": {"name": "Imp", "type": "humanoid"},
    "c3100": {"name": "Elemer of the Briar / Crucible Knight (variant)", "type": "humanoid"},
    "c3160": {"name": "Night's Cavalry Horse", "type": "quadruped"},
    "c3170": {"name": "Albinauric Archer", "type": "humanoid"},
    "c3171": {"name": "Giant Albinauric Archer", "type": "humanoid_large"},
    "c3180": {"name": "Dog / Red Wolf of Radagon (base)", "type": "quadruped"},
    "c3181": {"name": "Red Wolf of Radagon", "type": "quadruped"},
    "c3200": {"name": "Nomad Trader", "type": "npc"},
    "c3201": {"name": "Nomad Trader (variant)", "type": "npc"},
    "c3210": {"name": "Nomad Mule", "type": "quadruped"},
    "c3250": {"name": "Draconic Tree Sentinel", "type": "humanoid_large"},
    "c3251": {"name": "Tree Sentinel", "type": "humanoid_large"},
    "c3252": {"name": "Nox Swordstress", "type": "humanoid"},
    "c3300": {"name": "Nox Priest / Nox Monk", "type": "humanoid"},
    "c3320": {"name": "Silver Tear", "type": "humanoid"},
    "c3330": {"name": "Silver Tear Orb", "type": "large_creature"},
    "c3350": {"name": "Crystalian", "type": "humanoid"},
    "c3360": {"name": "Ancestral Follower", "type": "humanoid"},
    "c3361": {"name": "Undead Ancestral Follower", "type": "humanoid"},
    "c3370": {"name": "Ancestral Follower Shaman", "type": "humanoid"},
    "c3371": {"name": "Undead Ancestral Follower Shaman", "type": "humanoid"},
    "c3400": {"name": "Grave Warden Duelist", "type": "humanoid"},
    "c3450": {"name": "Misbegotten", "type": "humanoid"},
    "c3451": {"name": "Misbegotten (variant)", "type": "humanoid"},
    "c3460": {"name": "Leonine Misbegotten", "type": "humanoid"},
    "c3470": {"name": "Albinauric", "type": "humanoid"},
    "c3471": {"name": "Albinauric (variant)", "type": "humanoid"},
    "c3510": {"name": "Skeleton (Scimitar)", "type": "humanoid"},
    "c3570": {"name": "Godskin Noble", "type": "humanoid_large"},
    "c3600": {"name": "Alabaster Lord", "type": "humanoid"},
    "c3630": {"name": "Giant Oracle Envoy", "type": "humanoid_large"},
    "c3650": {"name": "Erdtree Guardian", "type": "humanoid"},
    "c3660": {"name": "Commoner", "type": "humanoid"},
    "c3661": {"name": "Putrid Corpse", "type": "humanoid"},
    "c3662": {"name": "Scarlet Putrid Corpse", "type": "humanoid"},
    "c3663": {"name": "Commoner (variant)", "type": "humanoid"},
    "c3664": {"name": "Cemetery Shade", "type": "humanoid"},
    "c3665": {"name": "Devious Commoner", "type": "humanoid"},
    "c3666": {"name": "Sunflower Commoner", "type": "humanoid"},
    "c3670": {"name": "Albinauric Lookout", "type": "humanoid"},
    "c3701": {"name": "White Veil Perfumer", "type": "humanoid"},
    "c3702": {"name": "Raya Lucaria Scholar", "type": "humanoid"},
    "c3703": {"name": "Noble's Page", "type": "humanoid"},
    "c3704": {"name": "Battlemage", "type": "humanoid"},
    "c3710": {"name": "Master Azur", "type": "npc"},
    "c3720": {"name": "Master Lusat", "type": "npc"},
    "c3730": {"name": "Orb of Many Faces", "type": "large_creature"},
    "c3750": {"name": "Clayman", "type": "humanoid"},
    "c3810": {"name": "Kindred of Rot", "type": "humanoid"},
    "c3850": {"name": "Marionette Soldier", "type": "humanoid"},
    "c3860": {"name": "Avionette Soldier", "type": "flying"},
    "c3901": {"name": "Godskin Monk / Fire Monk (variant)", "type": "humanoid"},

    # =========================================================================
    # CREATURES & BEASTS (c4000 - c4999)
    # =========================================================================
    "c4000": {"name": "Royal Rider / Night Rider", "type": "humanoid"},
    "c4020": {"name": "Royal Revenant", "type": "large_creature"},
    "c4040": {"name": "Slug", "type": "quadruped"},
    "c4060": {"name": "Horse (Thick Mane)", "type": "quadruped"},
    "c4070": {"name": "Grey Wolf", "type": "quadruped"},
    "c4071": {"name": "White Wolf", "type": "quadruped"},
    "c4100": {"name": "Demi-Human", "type": "humanoid"},
    "c4101": {"name": "Large Demi-Human", "type": "humanoid_large"},
    "c4120": {"name": "Demi-Human Beastman", "type": "humanoid"},
    "c4130": {"name": "Demi-Human Queen", "type": "humanoid_large"},
    "c4140": {"name": "Snail (Spirit-Caller)", "type": "large_creature"},
    "c4150": {"name": "Basilisk", "type": "quadruped"},
    "c4160": {"name": "Large Starved Dog", "type": "quadruped"},
    "c4161": {"name": "Starved Dog", "type": "quadruped"},
    "c4162": {"name": "Large Braided Dog", "type": "quadruped"},
    "c4163": {"name": "Braided Dog", "type": "quadruped"},
    "c4164": {"name": "Large Rotten Dog", "type": "quadruped"},
    "c4165": {"name": "Rotten Dog", "type": "quadruped"},
    "c4166": {"name": "Large Mushroom Dog", "type": "quadruped"},
    "c4167": {"name": "Mushroom Dog", "type": "quadruped"},
    "c4170": {"name": "Small Living Mass", "type": "large_creature"},
    "c4171": {"name": "Giant Living Mass", "type": "large_creature"},
    "c4180": {"name": "Jellyfish", "type": "flying"},
    "c4190": {"name": "Large Tear Scarab", "type": "quadruped"},
    "c4191": {"name": "Tear Scarab", "type": "quadruped"},
    "c4192": {"name": "Ash Scarab", "type": "quadruped"},
    "c4200": {"name": "Giant Bat", "type": "flying"},
    "c4201": {"name": "Bat Harpy", "type": "flying"},
    "c4210": {"name": "Warhawk", "type": "flying"},
    "c4220": {"name": "Giant Land Octopus", "type": "large_creature"},
    "c4230": {"name": "Small Land Octopus", "type": "large_creature"},
    "c4240": {"name": "Large Fingercreeper", "type": "large_creature"},
    "c4241": {"name": "Giant Fingercreeper", "type": "large_creature"},
    "c4250": {"name": "Small Fingercreeper", "type": "large_creature"},
    "c4260": {"name": "Burial Watchdog", "type": "humanoid_large"},
    "c4280": {"name": "Giant Ant", "type": "quadruped"},
    "c4281": {"name": "Giant Ant (Egg Sac)", "type": "quadruped"},

    # Soldiers & Knights (c4310 - c4385)
    "c4311": {"name": "Godrick Soldier", "type": "humanoid"},
    "c4312": {"name": "Raya Lucaria Soldier", "type": "humanoid"},
    "c4313": {"name": "Leyndell Soldier", "type": "humanoid"},
    "c4314": {"name": "Radahn Soldier", "type": "humanoid"},
    "c4315": {"name": "Mausoleum Soldier (Headless)", "type": "humanoid"},
    "c4316": {"name": "Haligtree Soldier", "type": "humanoid"},
    "c4321": {"name": "Vulgar Militia (variant)", "type": "humanoid"},
    "c4341": {"name": "Grinning Torturer / Mad Pumpkin Head (variant)", "type": "humanoid_large"},
    "c4351": {"name": "Godrick Knight", "type": "humanoid"},
    "c4352": {"name": "Cuckoo Knight", "type": "humanoid"},
    "c4353": {"name": "Leyndell Knight", "type": "humanoid"},
    "c4354": {"name": "Redmane Knight", "type": "humanoid"},
    "c4355": {"name": "Mausoleum Knight (Headless)", "type": "humanoid"},
    "c4356": {"name": "Haligtree Knight", "type": "humanoid"},

    # Knight Horses (c4360 - c4365)
    "c4360": {"name": "Knight's Horse (Generic)", "type": "quadruped"},
    "c4361": {"name": "Godrick Knight's Horse", "type": "quadruped"},
    "c4362": {"name": "Cuckoo Knight's Horse", "type": "quadruped"},
    "c4363": {"name": "Leyndell Knight's Horse", "type": "quadruped"},
    "c4364": {"name": "Redmane Knight's Horse", "type": "quadruped"},
    "c4365": {"name": "Mausoleum Knight's Horse", "type": "quadruped"},

    # Foot Soldiers (c4371 - c4376)
    "c4371": {"name": "Godrick Foot Soldier", "type": "humanoid"},
    "c4372": {"name": "Raya Lucaria Foot Soldier", "type": "humanoid"},
    "c4373": {"name": "Leyndell Foot Soldier", "type": "humanoid"},
    "c4374": {"name": "Radahn Foot Soldier", "type": "humanoid"},
    "c4375": {"name": "Mausoleum Foot Soldier", "type": "humanoid"},
    "c4376": {"name": "Haligtree Foot Soldier", "type": "humanoid"},

    # Miners & Misc (c4381 - c4385)
    "c4381": {"name": "Bloodthorn Exile / Starcaller (variant)", "type": "humanoid"},
    "c4382": {"name": "Tunnel Miner (Big Sack)", "type": "humanoid"},
    "c4383": {"name": "Glintstone Miner", "type": "humanoid"},
    "c4384": {"name": "Glintstone Miner (Small Sack)", "type": "humanoid"},
    "c4385": {"name": "Fungal Sorcerer", "type": "humanoid"},

    # More Creatures
    "c4400": {"name": "Fungal Pod / Land Squirt", "type": "large_creature"},
    "c4420": {"name": "Giant Lobster", "type": "large_creature"},
    "c4430": {"name": "Abnormal Stone Cluster", "type": "large_creature"},
    "c4440": {"name": "Fungal Pod", "type": "large_creature"},
    "c4441": {"name": "Giant Fungal Pod", "type": "large_creature"},
    "c4442": {"name": "Giant Rotten Pod", "type": "large_creature"},
    "c4450": {"name": "Walking Mausoleum", "type": "large_creature"},
    "c4460": {"name": "Maneuverable Flamethrower Tank", "type": "object"},
    "c4480": {"name": "Giant Miranda Sprout", "type": "large_creature"},
    "c4481": {"name": "Miranda Sprout", "type": "large_creature"},
    "c4482": {"name": "Fading Giant Miranda Sprout", "type": "large_creature"},
    "c4483": {"name": "Fading Miranda Sprout", "type": "large_creature"},
    "c4490": {"name": "Living Pot (Large)", "type": "humanoid"},
    "c4491": {"name": "Living Pot (Small)", "type": "humanoid"},
    "c4492": {"name": "Living Pot (Huge)", "type": "humanoid_large"},

    # Dragons (c4500 - c4520)
    "c4500": {"name": "Dragon", "type": "large_creature"},
    "c4501": {"name": "Dragon (variant)", "type": "large_creature"},
    "c4502": {"name": "Dragon (Blade in Mouth)", "type": "large_creature"},
    "c4503": {"name": "Dragon (variant 2)", "type": "large_creature"},
    "c4504": {"name": "Elder Dragon Greyoll", "type": "boss"},
    "c4505": {"name": "Lesser Elder Dragon", "type": "large_creature"},
    "c4510": {"name": "Ancient Dragon (Lansseax/etc)", "type": "large_creature"},
    "c4511": {"name": "Lichdragon Fortissax", "type": "boss"},
    "c4520": {"name": "Dragonlord Placidusax", "type": "boss"},

    # Large Beasts
    "c4550": {"name": "Monstrous Dog", "type": "quadruped"},
    "c4560": {"name": "Monstrous Crow", "type": "quadruped"},
    "c4561": {"name": "Monstrous Rotten Crow", "type": "quadruped"},
    "c4570": {"name": "Large Wormface", "type": "humanoid_large"},
    "c4580": {"name": "Giant Wormface", "type": "humanoid_large"},

    # Trolls (c4600 - c4604)
    "c4600": {"name": "Troll Knight (Blade)", "type": "humanoid_large"},
    "c4601": {"name": "Armored Troll Knight (Blade)", "type": "humanoid_large"},
    "c4602": {"name": "Troll Knight", "type": "humanoid_large"},
    "c4603": {"name": "Troll Knight (Club)", "type": "humanoid_large"},
    "c4604": {"name": "Troll Knight (Covered Face, Club & Book)", "type": "humanoid_large"},

    # Major Enemies & Bosses
    "c4620": {"name": "Astel, Stars of Darkness", "type": "boss"},
    "c4630": {"name": "Runebear", "type": "quadruped"},
    "c4640": {"name": "Ulcerated Tree Spirit", "type": "large_creature"},
    "c4650": {"name": "Dragonkin Soldier", "type": "humanoid_large"},
    "c4660": {"name": "Golem", "type": "humanoid_large"},
    "c4670": {"name": "Ancestor Spirit", "type": "boss"},
    "c4680": {"name": "Fallingstar Beast", "type": "large_creature"},
    "c4690": {"name": "Grafted Scion", "type": "large_creature"},
    "c4710": {"name": "God-Devouring Serpent / Rykard", "type": "boss"},
    "c4711": {"name": "Writhing Mass (Rykard arena)", "type": "large_creature"},
    "c4720": {"name": "Godfrey, First Elden Lord", "type": "boss"},
    "c4721": {"name": "Hoarah Loux, Warrior", "type": "boss"},
    "c4730": {"name": "Starscourge Radahn", "type": "boss"},
    "c4750": {"name": "Godrick the Grafted", "type": "boss"},
    "c4751": {"name": "Godrick (Head and Torso)", "type": "boss"},
    "c4760": {"name": "Fire Giant", "type": "boss"},
    "c4770": {"name": "Gargoyle (Black Blade / Valiant)", "type": "humanoid_large"},
    "c4800": {"name": "Mohg, The Omen", "type": "boss"},
    "c4810": {"name": "Erdtree Avatar", "type": "humanoid_large"},
    "c4811": {"name": "Erdtree Avatar (variant)", "type": "humanoid_large"},
    "c4910": {"name": "Magma Wyrm", "type": "large_creature"},
    "c4911": {"name": "Magma Wyrm (Standing)", "type": "large_creature"},
    "c4950": {"name": "Tibia Mariner", "type": "humanoid"},
    "c4960": {"name": "Giant Skeleton Torso", "type": "large_creature"},
    "c4980": {"name": "Death Rite Bird", "type": "flying"},

    # =========================================================================
    # DLC — SHADOW OF THE ERDTREE (c5000 - c5999)
    # =========================================================================
    "c5040": {"name": "Curseblade Labirith", "type": "humanoid"},
    "c5080": {"name": "Bloodfiend", "type": "humanoid"},
    "c5090": {"name": "Gravebird", "type": "flying"},
    "c5160": {"name": "Fire Knight", "type": "humanoid"},
    "c5190": {"name": "Spider Scorpion", "type": "quadruped"},
    "c5194": {"name": "Spider Scorpion (Small)", "type": "quadruped"},
    "c5250": {"name": "Horned Warrior", "type": "humanoid"},
    "c5260": {"name": "Golem Smith", "type": "humanoid_large"},
    "c5261": {"name": "Smith Golem (variant)", "type": "humanoid_large"},
    "c5310": {"name": "Inquisitor", "type": "humanoid"},
    "c5311": {"name": "Inquisitor (Candles)", "type": "humanoid"},
    "c5550": {"name": "Fingercreeper (DLC)", "type": "large_creature"},
    "c5560": {"name": "Fingercreeper Small (DLC)", "type": "large_creature"},
    "c5810": {"name": "Demi-Human Swordmaster Onze", "type": "humanoid"},
    "c5830": {"name": "Messmer Soldier", "type": "humanoid"},
    "c5840": {"name": "Black Knight", "type": "humanoid"},
    "c5870": {"name": "Imp (Lion Head / DLC)", "type": "humanoid"},
    "c5900": {"name": "Man-Fly", "type": "flying"},

    # =========================================================================
    # WILDLIFE / AMBIENT (c6000 - c6100)
    # =========================================================================
    "c6000": {"name": "Hawk", "type": "ambient"},
    "c6001": {"name": "Hawk (variant)", "type": "ambient"},
    "c6010": {"name": "Deer", "type": "ambient"},
    "c6020": {"name": "Eagle", "type": "ambient"},
    "c6030": {"name": "Bear", "type": "ambient"},
    "c6031": {"name": "Bear (variant)", "type": "ambient"},
    "c6040": {"name": "Owl", "type": "ambient"},
    "c6050": {"name": "Boar", "type": "ambient"},
    "c6060": {"name": "Goat", "type": "ambient"},
    "c6070": {"name": "Gull", "type": "ambient"},
    "c6071": {"name": "Gull (variant)", "type": "ambient"},
    "c6072": {"name": "Gull (variant 2)", "type": "ambient"},
    "c6080": {"name": "Dragonfly", "type": "ambient"},
    "c6081": {"name": "Dragonfly (variant)", "type": "ambient"},
    "c6082": {"name": "Dragonfly (variant 2)", "type": "ambient"},
    "c6090": {"name": "Turtle", "type": "ambient"},
    "c6091": {"name": "Turtle Pope (Miriel)", "type": "npc"},
    "c6100": {"name": "Rabbitgaroo", "type": "ambient"},

    # =========================================================================
    # SPECIAL / MISCELLANEOUS (c7000 - c9999)
    # =========================================================================
    "c7000": {"name": "Fallen Hawk Soldier", "type": "humanoid"},
    "c7100": {"name": "White Mask Assailant (Varre's quest)", "type": "humanoid"},
    "c8000": {"name": "Torrent (Player Mount)", "type": "quadruped"},
    "c8100": {"name": "Ballista (Siege)", "type": "object"},
    "c8101": {"name": "Giant Ballista", "type": "object"},
    "c8110": {"name": "Dragon-faced Flamethrower", "type": "object"},
    "c8120": {"name": "Merciless Chariot", "type": "object"},
    "c9001": {"name": "Debug / Test Entity", "type": "unknown"},
}


# ============================================================================
# Convenience lookups
# ============================================================================

def get_humanoid_enemies():
    """Return all humanoid chr IDs suitable for animation retargeting."""
    return {
        k: v for k, v in ELDEN_RING_CHR_IDS.items()
        if v["type"] in ("humanoid", "humanoid_large")
    }

def get_by_type(enemy_type: str):
    """Filter by type: humanoid, quadruped, flying, large_creature, boss, npc, ambient, object, unknown."""
    return {
        k: v for k, v in ELDEN_RING_CHR_IDS.items()
        if v["type"] == enemy_type
    }

def search_by_name(keyword: str):
    """Case-insensitive name search."""
    keyword_lower = keyword.lower()
    return {
        k: v for k, v in ELDEN_RING_CHR_IDS.items()
        if keyword_lower in v["name"].lower()
    }


if __name__ == "__main__":
    print(f"Total entries: {len(ELDEN_RING_CHR_IDS)}")
    for t in ("humanoid", "humanoid_large", "quadruped", "flying",
              "large_creature", "boss", "npc", "ambient", "object", "unknown"):
        count = len(get_by_type(t))
        print(f"  {t}: {count}")

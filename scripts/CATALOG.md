# Script & Source Catalog

Auto-generated index of all project scripts and C++ source files.
**Check this before writing new scripts or C++ classes.**
Last generated: 2026-03-22 15:08

---

## Migration Scripts (`scripts/migration`)
**371 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| add_crouch_mapping.py | Add IA_Crouch mapping to IMC_Gameplay Input Mapping Context | log, add_crouch_mapping | 2026-01-07 | 2026-03-22 |
| add_debug_service_to_bt.py | add_debug_service_to_bt.py | add_debug_service, check_bt_structure | 2026-01-26 | 2026-03-22 |
| add_default_scene_root.py | Add DefaultSceneRoot to SLFBaseCharacter |  | 2026-01-03 | 2026-03-22 |
| add_dodge_logging.py | Add logging to the POST-MIGRATION: Dodge Montages section |  | 2026-01-07 | 2026-03-22 |
| add_dsr_to_interactable.py | Add DefaultSceneRoot to B_Interactable |  | 2026-01-03 | 2026-03-22 |
| add_initialize_actions.py | Add InitializeDefaultActions to AC_ActionManager |  | 2026-01-03 | 2026-03-22 |
| add_pda_action_migration.py | Add PDA_Action to migration map so DA_Action_* assets will be UPDA_Action instan |  | 2026-01-03 | 2026-03-22 |
| add_pda_to_migration.py | Add PDA_Action to migration map |  | 2026-01-03 | 2026-03-22 |
| add_property_redirects.py | Add property redirectors for the ? suffix issue |  | 2026-01-03 | 2026-03-22 |
| add_skymanager_components.py | Add DirectionalLight components to B_SkyManager Blueprint | main | 2026-01-26 | 2026-03-22 |
| add_spell_item.py | add_spell_item.py | create_spell_data_asset, main | 2026-03-09 | 2026-03-22 |
| add_tab_key.py | add_tab_key.py | add_tab_key | 2026-01-07 | 2026-03-22 |
| add_validation_phase.py | Add validation phase to run_migration.py |  | 2026-01-03 | 2026-03-22 |
| apply_action_data.py | apply_action_data.py | apply_action_data | 2026-01-07 | 2026-03-22 |
| apply_action_stamina_costs.py | apply_action_stamina_costs.py | apply_stamina_costs | 2026-01-26 | 2026-03-22 |
| apply_ai_weapon_transforms.py | apply_ai_weapon_transforms.py | log, apply_ai_weapon_transforms | 2026-01-12 | 2026-03-22 |
| apply_armor_data.py | apply_armor_data.py | debug_log, apply_armor_data | 2026-01-09 | 2026-03-22 |
| apply_base_stats.py | Apply BaseStats to character class data assets (DA_Manny, DA_Quinn). | apply_base_stats | 2026-01-26 | 2026-03-22 |
| apply_base_stats_blueprint_keys.py | apply_base_stats_blueprint_keys.py | log, apply_base_stats | 2026-01-26 | 2026-03-22 |
| apply_base_stats_defaults.py | apply_base_stats_defaults.py | log, apply_base_stats | 2026-01-26 | 2026-03-22 |
| apply_boss_weapon_mesh.py | Apply boss weapon mesh to B_Item_AI_Weapon_BossMace. | apply_boss_weapon_mesh | 2026-03-09 | 2026-03-22 |
| apply_campfire_mesh.py | Apply SM_Campfire mesh to B_RestingPoint's DefaultInteractableMesh property |  | 2026-01-26 | 2026-03-22 |
| apply_charinfo_data.py | Apply CharacterClassName and other data to DA_Quinn and DA_Manny after reparenti |  | 2026-03-09 | 2026-03-22 |
| apply_container_data.py | apply_container_data.py | apply_container_data | 2026-01-26 | 2026-03-22 |
| apply_container_loot.py | apply_container_loot.py | main | 2026-01-26 | 2026-03-22 |
| apply_container_loot_cpp.py | apply_container_loot_cpp.py | main | 2026-01-26 | 2026-03-22 |
| apply_container_meshes.py | apply_container_meshes.py | main | 2026-01-26 | 2026-03-22 |
| apply_core_redirect.py | apply_core_redirect.py |  | 2026-01-07 | 2026-03-22 |
| apply_daynight_entries.py | Apply correct bp_only DayNight entries to DA_ExampleDayNightInfo | main | 2026-01-26 | 2026-03-22 |
| apply_death_montages.py | apply_death_montages.py | apply_death_montages | 2026-01-26 | 2026-03-22 |
| apply_default_mesh_data.py | apply_default_mesh_data.py | log, apply_mesh_data, set_gameinstance_selected_class, main | 2026-01-26 | 2026-03-22 |
| apply_dialog_requirements.py | apply_dialog_requirements.py | log | 2026-03-09 | 2026-03-22 |
| apply_dialog_requirements_v2.py | apply_dialog_requirements_v2.py | log | 2026-03-09 | 2026-03-22 |
| apply_dialog_requirements_v3.py | apply_dialog_requirements_v3.py | log | 2026-03-09 | 2026-03-22 |
| apply_dialog_requirements_v4.py | apply_dialog_requirements_v4.py | log | 2026-03-09 | 2026-03-22 |
| apply_dialog_requirements_v5.py | apply_dialog_requirements_v5.py | log | 2026-03-09 | 2026-03-22 |
| apply_dialog_requirements_v6.py | apply_dialog_requirements_v6.py | log | 2026-03-09 | 2026-03-22 |
| apply_dodge_montages.py | apply_dodge_montages.py - Set dodge montages on B_Action_Dodge Blueprint | apply_dodge_montages | 2026-01-09 | 2026-03-22 |
| apply_door_default_mesh.py | Apply DefaultDoorMesh to B_Door Blueprint CDO |  | 2026-01-26 | 2026-03-22 |
| apply_door_mesh.py | Apply door mesh to B_Door Blueprint CDO |  | 2026-01-26 | 2026-03-22 |
| apply_enemy_abilities.py | apply_enemy_abilities.py | log, apply_abilities, main | 2026-01-26 | 2026-03-22 |
| apply_enemy_bt.py | Apply BehaviorTree to enemy AIBehaviorManagerComponent | log, apply_bt_to_enemy, main | 2026-01-26 | 2026-03-22 |
| apply_enemy_status_effects.py | Apply default attack status effects to enemy Blueprints. | log | 2026-03-09 | 2026-03-22 |
| apply_equip_slots_cpp.py | apply_equip_slots_cpp.py | log, apply_equip_slots | 2026-01-26 | 2026-03-22 |
| apply_exact_bp_only_transforms.py | apply_exact_bp_only_transforms.py | log, make_rotator, apply_transforms | 2026-01-26 | 2026-03-22 |
| apply_execution_data.py | Apply execution animation data to DA_Execute_Generic using C++ SLFAutomationLibr | apply_execution_data | 2026-01-26 | 2026-03-22 |
| apply_execution_tags.py | Apply execution tags to DA_Execute_Generic data asset. | apply_execution_tags | 2026-01-26 | 2026-03-22 |
| apply_flask_data.py | apply_flask_data.py | apply_flask_data | 2026-03-09 | 2026-03-22 |
| apply_fogdensity_curve.py | Apply FogDensityTrack curve to B_SkyManager Blueprint CDO | main | 2026-01-26 | 2026-03-22 |
| apply_icon_paths.py | apply_icon_paths.py | apply_icon_paths | 2026-01-09 | 2026-03-22 |
| apply_icons_fixed.py | (no description) | apply_icons | 2026-01-09 | 2026-03-22 |
| apply_item_categories.py | Apply item categories and usable flags to migrated item data assets. | get_category_name, log, apply_categories, list_current_values | 2026-01-09 | 2026-03-22 |
| apply_item_data.py | Apply Item Data Migration Script | log, apply_item_data | 2026-01-08 | 2026-03-22 |
| apply_item_icons.py | apply_item_icons.py - Apply cached icon data to migrated assets | apply_item_data | 2026-01-09 | 2026-03-22 |
| apply_item_info_data.py | apply_item_info_data.py | apply_item_info | 2026-01-09 | 2026-03-22 |
| apply_item_subcategories.py | Apply item subcategories to migrated item data assets. | get_subcategory_name, log, apply_subcategories, list_current_values | 2026-01-09 | 2026-03-22 |
| apply_item_tags.py | apply_item_tags.py | log, main | 2026-01-26 | 2026-03-22 |
| apply_loot_to_level_containers.py | apply_loot_to_level_containers.py | main | 2026-01-26 | 2026-03-22 |
| apply_niagara_and_icons.py | apply_niagara_and_icons.py - Apply cached icon AND niagara data to migrated asse | apply_item_data | 2026-01-09 | 2026-03-22 |
| apply_niagara_paths.py | apply_niagara_paths.py | apply_niagara_paths | 2026-01-04 | 2026-03-22 |
| apply_override_item_to_containers.py | apply_override_item_to_containers.py | main | 2026-01-26 | 2026-03-22 |
| apply_player_weapon_transforms.py | apply_player_weapon_transforms.py | log, apply_player_weapon_transforms | 2026-01-12 | 2026-03-22 |
| apply_player_weapon_transforms_v2.py | apply_player_weapon_transforms_v2.py | log, apply_transforms | 2026-01-26 | 2026-03-22 |
| apply_projectile_montage.py | Check and apply throw montage data to DA_Action_Projectile. | check_and_fix | 2026-03-09 | 2026-03-22 |
| apply_reduced_poison_damage.py | Apply reduced poison tick damage to DA_StatusEffect_Poison. | main | 2026-01-26 | 2026-03-22 |
| apply_remaining_data.py | (no description) | apply_niagara, apply_dodge_montages | 2026-01-09 | 2026-03-22 |
| apply_sequence_actor_data.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| apply_settings_colors.py | Apply bp_only color values to settings widgets. | main | 2026-03-09 | 2026-03-22 |
| apply_settings_embedded.py | Apply embedded settings widget data from bp_only to current project. | main | 2026-03-09 | 2026-03-22 |
| apply_skeletal_mesh_info.py | Apply skeletal mesh info to armor items using C++ automation library. | log, apply_skeletal_mesh_info | 2026-01-09 | 2026-03-22 |
| apply_skymanager_timeinfo.py | Apply TimeInfoAsset to B_SkyManager Blueprint CDO | main | 2026-01-26 | 2026-03-22 |
| apply_spell_categories.py | apply_spell_categories.py | log, apply_spell_categories, main | 2026-03-09 | 2026-03-22 |
| apply_status_effect_icons.py | Apply status effect icon data to SLFConversion project. | apply_status_effect_icons | 2026-03-09 | 2026-03-22 |
| apply_status_effect_vfx.py | Apply Status Effect VFX to RankInfo | apply_status_effect_vfx | 2026-01-26 | 2026-03-22 |
| apply_status_effect_vfx_v2.py | Apply Status Effect VFX to RankInfo v2 | log, apply_status_effect_vfx | 2026-01-26 | 2026-03-22 |
| apply_statuseffect_config.py | Apply StatusEffectToApply configuration to B_StatusEffectArea instances in L_Dem | log, apply_statuseffect_config | 2026-01-26 | 2026-03-22 |
| apply_statuseffect_damage.py | Apply Status Effect Damage Configuration | create_stat_change, create_stat_change_array, apply_status_effect_config, main | 2026-01-26 | 2026-03-22 |
| apply_statuseffect_rankinfo.py | apply_statuseffect_rankinfo.py | create_stat_change, create_status_effect_tick, create_status_effect_stat_chan... | 2026-01-26 | 2026-03-22 |
| apply_throwable_data.py | Apply data needed for throwing knives to work: | check_and_apply | 2026-03-09 | 2026-03-22 |
| apply_throwable_data_v2.py | Apply data needed for throwing knives to work: | log, check_and_apply | 2026-03-09 | 2026-03-22 |
| apply_twohanded_anims.py | Apply Two-Handed Pose Animations to PDA_AnimData | apply_twohanded_anims | 2026-03-09 | 2026-03-22 |
| apply_vendor_items.py | Apply vendor items to DA_ExampleVendor. | apply_vendor_items | 2026-03-09 | 2026-03-22 |
| apply_vendor_items_v2.py | Apply vendor items to DA_ExampleVendor v2 - with explicit package marking. | apply_vendor_items | 2026-03-09 | 2026-03-22 |
| apply_verify_settings.py | Apply and verify Settings widget data using C++ Automation |  | 2026-03-09 | 2026-03-22 |
| apply_weapon_abilities.py | Apply WeaponAbility property to weapon data assets. | log, apply_weapon_abilities | 2026-03-09 | 2026-03-22 |
| apply_weapon_categories.py | Apply Weapon Categories | main | 2026-01-26 | 2026-03-22 |
| apply_weapon_categories_simple.py | Apply Weapon Categories (Simplified) | main | 2026-01-26 | 2026-03-22 |
| apply_weapon_categories_v2.py | Apply Weapon Categories and SubCategories | main | 2026-01-26 | 2026-03-22 |
| apply_weapon_equip_slots.py | Apply EquipSlots tags to weapon data assets. | apply_weapon_equip_slots | 2026-01-26 | 2026-03-22 |
| apply_weapon_meshes.py | apply_weapon_meshes.py | log, apply_weapon_meshes | 2026-01-09 | 2026-03-22 |
| apply_weapon_movesets.py | apply_weapon_movesets.py | log, apply_movesets | 2026-01-26 | 2026-03-22 |
| apply_weapon_overlay.py | Apply WeaponOverlay tags to item data assets from cached data. | log, apply_weapon_overlay_data | 2026-01-26 | 2026-03-22 |
| apply_weapon_slots_and_sockets.py | apply_weapon_slots_and_sockets.py | log, apply_weapon_data | 2026-01-13 | 2026-03-22 |
| apply_weapon_stats.py | Apply weapon stat data from cached JSON to migrated weapon data assets. | log, log_warn, log_error, write_output, apply_weapon_stats | 2026-01-26 | 2026-03-22 |
| apply_weapon_statuseffects.py | Apply WeaponStatusEffectInfo data to weapon data assets. | log, load_weapon_status_effects, apply_weapon_status_effects | 2026-01-26 | 2026-03-22 |
| apply_weapon_statuseffects_v2.py | Apply WeaponStatusEffectInfo data to weapon data assets. | log, apply_weapon_status_effects | 2026-01-26 | 2026-03-22 |
| apply_weapon_statuseffects_v3.py | Apply WeaponStatusEffectInfo data to weapon data assets. | log, apply_weapon_status_effects | 2026-01-26 | 2026-03-22 |
| apply_weapon_transforms.py | apply_weapon_transforms.py | log, make_rotator, apply_transforms | 2026-01-26 | 2026-03-22 |
| apply_weapon_transforms_v3.py | apply_weapon_transforms_v3.py | log, main | 2026-01-26 | 2026-03-22 |
| apply_weapon_transforms_v4.py | apply_weapon_transforms_v4.py | log, main | 2026-01-26 | 2026-03-22 |
| apply_weapon_transforms_v5.py | apply_weapon_transforms_v5.py | log, main | 2026-01-26 | 2026-03-22 |
| apply_widget_instance_data.py | Apply widget instance property data after migration. | log, apply_widget_instance_data, apply_inventory_category_data | 2026-01-09 | 2026-03-22 |
| configure_boss.py | Configure boss using C++ SLFAutomationLibrary functions. | configure_boss | 2026-03-09 | 2026-03-22 |
| configure_container_loot.py | configure_container_loot.py | main | 2026-01-26 | 2026-03-22 |
| configure_container_loot_proper.py | configure_container_loot_proper.py | validate_loot_table_exists, find_containers_in_level, find_loot_manager_compo... | 2026-01-26 | 2026-03-22 |
| configure_containers_final.py | configure_containers_final.py | main | 2026-01-26 | 2026-03-22 |
| configure_enemy_status_effects.py | Configure demo enemies with default attack status effects. | log | 2026-03-09 | 2026-03-22 |
| configure_enemy_status_effects_v2.py | Configure demo enemies with default attack status effects. | log | 2026-01-29 | 2026-03-22 |
| configure_execution_widget.py | Configure ExecutionWidget in B_BaseCharacter to use W_TargetExecutionIndicator. | configure_execution_widget | 2026-01-26 | 2026-03-22 |
| configure_input_actions.py | configure_input_actions.py | run | 2026-01-03 | 2026-03-22 |
| configure_level_pawn.py | Configure GM_SoulslikeFramework to use B_Soulslike_Character as DefaultPawnClass | main | 2026-01-03 | 2026-03-22 |
| copy_action_tag_to_spells.py | copy_action_tag_to_spells.py | log, copy_action_tag, main | 2026-03-09 | 2026-03-22 |
| copy_niagara_to_cpp.py | copy_niagara_to_cpp.py | copy_niagara_data | 2026-01-07 | 2026-03-22 |
| create_all_spells.py | create_all_spells.py | log, create_spell_data_assets, add_spells_to_starting_inventory, main | 2026-03-09 | 2026-03-22 |
| create_execution_indicator_widget.py | Create W_TargetExecutionIndicator widget Blueprint. | create_execution_indicator_widget | 2026-01-26 | 2026-03-22 |
| create_fresh_skymanager.py | Delete B_SkyManager and create a fresh one inheriting from SLFSkyManager | main | 2026-01-26 | 2026-03-22 |
| delete_blueprint_functions.py | delete_blueprint_functions.py | delete_function_from_blueprint, process_blueprint, main | 2026-01-03 | 2026-03-22 |
| fix_action_loading.py | Fix InitializeDefaultActions to use correct tag names and asset loading |  | 2026-01-03 | 2026-03-22 |
| fix_action_montage.py | Set the ActionMontage on DA_Action_PickupItemMontage |  | 2026-03-09 | 2026-03-22 |
| fix_all_broken_structs.py | fix_all_broken_structs.py | check_prerequisites, load_blueprint, fix_b_statuseffect, fix_w_hud, fix_w_lev... | 2026-01-03 | 2026-03-22 |
| fix_and_resave_chaos_montages.py | Fix ANS_ToggleChaosField reparenting and resave montages. | reparent_if_needed, resave_montages, main | 2026-01-26 | 2026-03-22 |
| fix_animbp_animgraph_connections.py | fix_animbp_animgraph_connections.py | log, main | 2026-01-26 | 2026-03-22 |
| fix_animbp_boolean_connections.py | fix_animbp_boolean_connections.py | log, main | 2026-01-26 | 2026-03-22 |
| fix_animbp_complete.py | fix_animbp_complete.py | log, main | 2026-01-26 | 2026-03-22 |
| fix_animbp_defaults.py | fix_animbp_defaults.py | main | 2026-01-26 | 2026-03-22 |
| fix_animbp_eventgraphs.py | fix_animbp_eventgraphs.py | main | 2026-01-26 | 2026-03-22 |
| fix_animbp_guard.py | fix_animbp_guard.py |  | 2026-01-26 | 2026-03-22 |
| fix_animbp_overlay_bindings.py | Fix AnimBP BlendListByEnum node bindings for overlay states. | log, find_anim_graph_nodes, check_animbp_properties, check_cpp_class_properti... | 2026-01-26 | 2026-03-22 |
| fix_animbp_remaining.py | Fix remaining AnimBP issues: | log, main | 2026-01-26 | 2026-03-22 |
| fix_animbp_remove_shadowing_vars.py | fix_animbp_remove_shadowing_vars.py | log, main | 2026-01-26 | 2026-03-22 |
| fix_animbp_stale_state.py | fix_animbp_stale_state.py | main | 2026-01-26 | 2026-03-22 |
| fix_animbp_transitions_targeted.py | Fix AnimBP Transitions - Targeted approach | log, main | 2026-01-26 | 2026-03-22 |
| fix_animbp_variable_names.py | fix_animbp_variable_names.py | log, main | 2026-01-26 | 2026-03-22 |
| fix_animbp_variables.py | fix_animbp_variables.py | main | 2026-01-26 | 2026-03-22 |
| fix_animbps_nocompile.py | fix_animbps_nocompile.py | main | 2026-01-26 | 2026-03-22 |
| fix_animgraph_property_access.py | Fix AnimGraph Property Access nodes that reference "?" suffix properties. | fix_animgraph_property_access | 2026-01-09 | 2026-03-22 |
| fix_ans_inputbuffer.py | fix_ans_inputbuffer.py |  | 2026-01-03 | 2026-03-22 |
| fix_ans_register_attack.py | fix_ans_register_attack.py | log, main | 2026-01-11 | 2026-03-22 |
| fix_ans_toggle_parent.py | Fix ANS_ToggleChaosField parent using C++ ForceReparentAndValidate. | main | 2026-01-26 | 2026-03-22 |
| fix_ans_trail.py | Fix ANS_Trail compile error by reparenting to C++ UANS_Trail. | fix_ans_trail | 2026-01-26 | 2026-03-22 |
| fix_ans_trail_fresh.py | Fix ANS_Trail by restoring from backup and reparenting fresh. | fix_ans_trail_fresh | 2026-01-26 | 2026-03-22 |
| fix_b_door_remove_all_scs.py | Remove ALL SCS components from B_Door to fully inherit from C++ | log_step | 2026-01-26 | 2026-03-22 |
| fix_b_door_scs_conflict.py | Fix B_Door by removing conflicting SCS components before reparenting | log_step | 2026-01-26 | 2026-03-22 |
| fix_b_door_with_automation.py | Fix B_Door using SLFAutomationLibrary functions | log_step | 2026-01-26 | 2026-03-22 |
| fix_b_sequenceactor.py | Fix B_SequenceActor Blueprint after reparenting. | log, main | 2026-03-09 | 2026-03-22 |
| fix_b_weight_final.py | fix_b_weight_final.py | fix_b_weight | 2026-01-26 | 2026-03-22 |
| fix_b_weight_reparent.py | fix_b_weight_reparent.py | fix_b_weight | 2026-01-26 | 2026-03-22 |
| fix_b_weight_statinfo.py | fix_b_weight_statinfo.py | fix_b_weight_statinfo | 2026-01-26 | 2026-03-22 |
| fix_b_weight_v2.py | fix_b_weight_v2.py | fix_b_weight | 2026-01-26 | 2026-03-22 |
| fix_bb_enum_type.py | Fix BB_Standard State key to use C++ ESLFAIStates enum instead of Blueprint E_AI | log, main | 2026-01-26 | 2026-03-22 |
| fix_blend_bindings.py | fix_blend_bindings.py | log, main | 2026-01-10 | 2026-03-22 |
| fix_blend_by_enum_binding.py | Fix BlendListByEnum ActiveEnumValue binding - connect to OverlayState variable. | log, main | 2026-01-26 | 2026-03-22 |
| fix_blend_by_enum_binding_v2.py | Fix BlendListByEnum ActiveEnumValue binding using the correct function. | log, main | 2026-01-26 | 2026-03-22 |
| fix_blend_by_enum_binding_v3.py | Fix BlendListByEnum ActiveEnumValue binding - call with correct signature. | log, main | 2026-01-26 | 2026-03-22 |
| fix_blendlist_bindings.py | Fix BlendListByEnum property bindings to use C++ overlay state properties | log, main | 2026-03-09 | 2026-03-22 |
| fix_blocationactor.py | Fix B_LocationActor compile error by reparenting to AB_LocationActor. | fix_location_actor | 2026-01-26 | 2026-03-22 |
| fix_boss_abilities.py | fix_boss_abilities.py | fix_boss_abilities | 2026-01-26 | 2026-03-22 |
| fix_boss_animset.py | fix_boss_animset.py | fix_boss_animset | 2026-01-26 | 2026-03-22 |
| fix_boss_config.py | Fix boss configuration in SLFConversion project. | log, write_output, fix_boss_config | 2026-03-09 | 2026-03-22 |
| fix_boss_door_complete.py | Complete fix for B_BossDoor: | fix_boss_door | 2026-01-26 | 2026-03-22 |
| fix_boss_door_direct.py | Direct fix for B_BossDoor - bypass cascade loading issues. | main | 2026-01-26 | 2026-03-22 |
| fix_boss_weapon_and_status.py | Fix boss weapon mesh and check status effect icons from backup. | extract_boss_weapon_data | 2026-03-09 | 2026-03-22 |
| fix_bt_task_parent.py | fix_bt_task_parent.py | log, main | 2026-01-26 | 2026-03-22 |
| fix_bts_trygetability.py | fix_bts_trygetability.py | log, main | 2026-01-26 | 2026-03-22 |
| fix_centered_text.py | Clear and reparent W_Settings_CenteredText to C++ UW_Settings_CenteredText | main | 2026-03-09 | 2026-03-22 |
| fix_chaos_forcefield_complete.py | Comprehensive fix for B_Chaos_ForceField to use C++ implementations. | main | 2026-01-26 | 2026-03-22 |
| fix_chaos_interface.py | Fix B_Chaos_ForceField by removing the Blueprint interface BPI_DestructibleHelpe | main | 2026-01-26 | 2026-03-22 |
| fix_chaos_reparents.py | Fix ANS_ToggleChaosField and B_Destructible Blueprint reparenting. | reparent_blueprint, clear_blueprint_logic, main | 2026-01-26 | 2026-03-22 |
| fix_character_bindings.py | fix_character_bindings.py |  | 2026-01-03 | 2026-03-22 |
| fix_character_parent.py | Check and fix character Blueprint parent classes. | log, reparent_blueprint, main | 2026-01-26 | 2026-03-22 |
| fix_container_loot.py | fix_container_loot.py | main | 2026-01-26 | 2026-03-22 |
| fix_container_parent.py | fix_container_parent.py | fix_parent | 2026-01-26 | 2026-03-22 |
| fix_container_positions.py | fix_container_positions.py | main | 2026-01-26 | 2026-03-22 |
| fix_crafting_only_throwingknife.py | fix_crafting_only_throwingknife.py | log, fix_crafting_state | 2026-01-26 | 2026-03-22 |
| fix_crouch_mapping.py | Fix IA_Crouch key mapping in IMC_Gameplay | log_to_file, fix_crouch_mapping | 2026-01-07 | 2026-03-22 |
| fix_debug_window.py | (no description) |  | 2026-01-03 | 2026-03-22 |
| fix_demo_blueprints.py | Fix Demo Blueprints - Clear graphs in demo Blueprints that are blocking PIE | log, clear_blueprint_graphs, clear_animbp_eventgraph, run | 2026-01-03 | 2026-03-22 |
| fix_demo_timeslider.py | Fix B_Demo_TimeSlider compile error by reparenting to C++ class. | fix_demo_timeslider | 2026-01-26 | 2026-03-22 |
| fix_destructible.py | Fix B_Destructible Blueprint. | fix_destructible | 2026-01-26 | 2026-03-22 |
| fix_destructible_complete.py | Complete fix for B_Destructible Blueprint. | fix_destructible_complete | 2026-01-26 | 2026-03-22 |
| fix_dialog_asset.py | Fix DA_ExampleDialog by setting the correct DefaultDialogTable |  | 2026-03-09 | 2026-03-22 |
| fix_dialog_tables.py | Fix DA_ExampleDialog and DA_ExampleDialog_Vendor DefaultDialogTable properties. | main | 2026-03-09 | 2026-03-22 |
| fix_dialog_widget_parent.py | Ensure W_Dialog widget is reparented to C++ UW_Dialog class. |  | 2026-03-09 | 2026-03-22 |
| fix_display_names.py | Quick script to fix DisplayName meta in C++ headers |  | 2026-01-03 | 2026-03-22 |
| fix_door_cdo_mesh.py | Directly set the mesh on B_Door CDO components. |  | 2026-01-26 | 2026-03-22 |
| fix_door_complete.py | Complete fix for B_Door: |  | 2026-01-26 | 2026-03-22 |
| fix_door_in_demo_level.py | Fix B_Door instances in the Demo level. |  | 2026-01-26 | 2026-03-22 |
| fix_door_mesh.py | Fix B_Door mesh - add SM_PrisonDoor to the Blueprint's SCS component | fix_door_mesh | 2026-01-26 | 2026-03-22 |
| fix_door_prison_in_level.py | Fix B_Door_Prison instances in L_Demo level. |  | 2026-01-26 | 2026-03-22 |
| fix_doors_all_levels.py | Fix doors in all available levels and check B_Door_Prison Blueprint. |  | 2026-01-26 | 2026-03-22 |
| fix_duplicate_types.py | Fix duplicate type definitions in component headers. | fix_header, main | 2026-01-03 | 2026-03-22 |
| fix_enemy_animbps.py | fix_enemy_animbps.py | log, fix_animbp, main | 2026-01-26 | 2026-03-22 |
| fix_enum_both.py | fix_enum_both.py | main | 2026-01-03 | 2026-03-22 |
| fix_enum_by_value.py | fix_enum_by_value.py | main | 2026-01-03 | 2026-03-22 |
| fix_enum_comprehensive.py | fix_enum_comprehensive.py | fix_blueprint, main | 2026-01-03 | 2026-03-22 |
| fix_enum_cpp.py | fix_enum_cpp.py | main | 2026-01-03 | 2026-03-22 |
| fix_enum_step1.py | fix_enum_step1.py | fix_blueprint, main | 2026-01-03 | 2026-03-22 |
| fix_enum_v2.py | fix_enum_v2.py | get_all_graphs_from_blueprint, fix_blueprint, main | 2026-01-03 | 2026-03-22 |
| fix_firsttime_flag.py | Correct path | log | 2026-03-09 | 2026-03-22 |
| fix_gi_and_level.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| fix_gi_interface_conflicts.py | Fix GI_SoulslikeFramework interface conflicts by removing Blueprint function gra | log, main | 2026-03-09 | 2026-03-22 |
| fix_guard_sequence.py | fix_guard_sequence.py |  | 2026-01-26 | 2026-03-22 |
| fix_guard_toggle.py | fix_guard_toggle.py |  | 2026-01-26 | 2026-03-22 |
| fix_key_correlation_table.py | Fix KeyCorrelationTable property on W_Inventory widget. | log, fix_key_correlation_table, list_data_table_rows | 2026-01-09 | 2026-03-22 |
| fix_level_blueprint.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| fix_level_blueprint_nodes.py | Fix Level Blueprint stale nodes by force-reconstructing them. | log, get_blueprint_graphs, reconstruct_node, find_problematic_nodes, fix_leve... | 2026-03-09 | 2026-03-22 |
| fix_level_blueprint_spawns.py | Diagnose and fix Level Blueprint SpawnActor node that lost its class reference. | log, main | 2026-03-09 | 2026-03-22 |
| fix_level_door_instances.py | Find and fix B_Door instances in the level that have null meshes. |  | 2026-01-26 | 2026-03-22 |
| fix_levelup_categories.py | Fix W_LevelUp stat block category tags. | fix_levelup_categories | 2026-01-26 | 2026-03-22 |
| fix_montage_notifies.py | fix_montage_notifies.py | log, main | 2026-01-11 | 2026-03-22 |
| fix_npc_dialog_assets.py | Fix NPC Dialog Assets - Restore DialogAsset references that were lost during mig | fix_npc_dialog_assets | 2026-03-09 | 2026-03-22 |
| fix_npc_interfaces.py | Remove implemented interfaces and clear all event nodes from NPC Blueprints. | main | 2026-03-09 | 2026-03-22 |
| fix_overlay_bindings_simple.py | fix_overlay_bindings_simple.py | log, main | 2026-01-26 | 2026-03-22 |
| fix_overlay_enum.py | fix_overlay_enum.py | fix_file, main | 2026-01-26 | 2026-03-22 |
| fix_pda_daynight.py | Fix PDA_DayNight and B_SkyManager after migration | clear_blueprint_eventgraph, set_skymanager_defaults, main | 2026-01-26 | 2026-03-22 |
| fix_phase1_issues.py | fix_phase1_issues.py |  | 2026-01-03 | 2026-03-22 |
| fix_pickup_montage.py | Set the pickup montage on DA_Action_PickupItemMontage |  | 2026-03-09 | 2026-03-22 |
| fix_pickup_relevant_data.py | Populate RelevantData on DA_Action_PickupItemMontage with an FMontage struct |  | 2026-03-09 | 2026-03-22 |
| fix_pie_errors.py | Fix PIE Blueprint compile errors by clearing ALL Blueprint logic and reparenting | fix_blueprint, main | 2026-01-26 | 2026-03-22 |
| fix_property_access.py | Fix Property Access nodes in AnimBP that reference "?" suffix properties. | fix_animblueprint_property_access, fix_property_bindings | 2026-01-09 | 2026-03-22 |
| fix_property_access_paths.py | fix_property_access_paths.py | log, main | 2026-01-26 | 2026-03-22 |
| fix_remaining_dependents.py | fix_remaining_dependents.py |  | 2026-01-03 | 2026-03-22 |
| fix_restingpoint_parent.py | fix_restingpoint_parent.py | main | 2026-01-26 | 2026-03-22 |
| fix_run_migration.py | Fix the corrupted line in run_migration.py |  | 2026-01-07 | 2026-03-22 |
| fix_sequence_actor.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| fix_sequence_actor_v2.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| fix_settings_entry.py | Fix W_Settings_Entry reparenting and W_Settings parent class. | fix_widget, main | 2026-01-26 | 2026-03-22 |
| fix_settings_entry_bindings.py | Fix W_Settings_Entry - Clear stale widget delegate bindings. | fix_w_settings_entry, check_automation_functions, export_widget_blueprint_tex... | 2026-01-26 | 2026-03-22 |
| fix_settings_entry_reparent.py | Clear event graphs and reparent W_Settings_Entry Blueprint to C++ class. | clear_and_reparent, main | 2026-01-26 | 2026-03-22 |
| fix_shield_rotation.py | fix_shield_rotation.py | log, make_rotator, fix_shield | 2026-01-26 | 2026-03-22 |
| fix_showcase_blueprints.py | Fix Showcase Demo Blueprint compile errors by clearing stale Blueprint logic. | fix_showcase_blueprint, main | 2026-01-26 | 2026-03-22 |
| fix_skymanager_all_settings.py | Fix ALL SkyManager settings to match bp_only exactly | log, main | 2026-01-26 | 2026-03-22 |
| fix_skymanager_complete.py | Complete fix for B_SkyManager Blueprint after C++ migration. | log, main | 2026-01-26 | 2026-03-22 |
| fix_skymanager_in_level.py | Fix B_SkyManager in L_Demo_Showcase Level | log, fix_skymanager_in_level | 2026-01-26 | 2026-03-22 |
| fix_skymanager_level.py | Fix B_SkyManager level instance - scale, mesh, material, and light priorities | log, main | 2026-01-26 | 2026-03-22 |
| fix_skymanager_level_instance.py | Fix B_SkyManager Level Instance | log, get_world, fix_skymanager_instance | 2026-01-26 | 2026-03-22 |
| fix_skymanager_mesh.py | Fix B_SkyManager instance - set the sky sphere mesh and material | log, main | 2026-01-26 | 2026-03-22 |
| fix_spell_class.py | fix_spell_class.py | log, fix_spell_item_class, main | 2026-03-09 | 2026-03-22 |
| fix_statuseffect_simple.py | Simple fix for B_StatusEffectArea - just check properties and apply config. | log, fix | 2026-01-26 | 2026-03-22 |
| fix_statuseffect_triggeredtext.py | Fix TriggeredText on status effect data assets. | log, fix_triggered_text | 2026-01-26 | 2026-03-22 |
| fix_statuseffectarea_blueprint.py | Fix B_StatusEffectArea: Remove Blueprint variables that shadow C++ properties. | log, fix_blueprint | 2026-01-26 | 2026-03-22 |
| fix_valuetype_only.py | fix_valuetype_only.py | main | 2026-01-03 | 2026-03-22 |
| fix_variable_references.py | fix_variable_references.py | log, main | 2026-01-26 | 2026-03-22 |
| fix_vendor_data.py | Fix DA_ExampleVendor - extract Items data and ensure it loads properly | fix_vendor | 2026-03-09 | 2026-03-22 |
| fix_vendor_npc.py | Fix Vendor NPC - Set VendorAsset on AIInteractionManager component | log, fix_vendor_npc | 2026-03-09 | 2026-03-22 |
| fix_w_dialog_reparent.py | Fix W_Dialog widget reparenting to C++ UW_Dialog class. | fix_w_dialog | 2026-03-09 | 2026-03-22 |
| fix_w_settings_reparent.py | Clear event graphs and reparent W_Settings Blueprint to C++ class. | clear_and_reparent, main | 2026-01-26 | 2026-03-22 |
| fix_weapon_scale_rule.py | Fix ScaleRule from KeepWorld to SnapToTarget to match original Blueprint | main | 2026-01-26 | 2026-03-22 |
| fix_weapon_trace_notify.py | fix_weapon_trace_notify.py | fix_anim_notify_state, fix_component_blueprint | 2026-01-26 | 2026-03-22 |
| fix_weapon_transforms.py | Remove IsZero() guards from weapon mesh transform application in BeginPlay | main | 2026-01-26 | 2026-03-22 |
| generate_ai.py | Generate C++ headers and stubs for AI Blueprint classes. | type_to_cpp, sanitize_name, get_ai_parent_info, generate_ai_class, main | 2026-01-03 | 2026-03-22 |
| generate_animblueprints.py | Generate C++ AnimInstance classes from AnimBlueprint JSON exports. | type_to_cpp, sanitize_name, generate_animblueprint, main | 2026-01-03 | 2026-03-22 |
| generate_animnotifies.py | Generate C++ headers and stubs for Animation Notify classes. | type_to_cpp, sanitize_name, generate_animnotify, main | 2026-01-03 | 2026-03-22 |
| generate_blueprints.py | Generate C++ headers and stubs for Blueprint classes. | build_parent_hierarchy, type_to_cpp, sanitize_name, get_parent_info, generate... | 2026-01-03 | 2026-03-22 |
| generate_components.py | Generate C++ Component headers and stubs from Blueprint JSON exports. | build_parent_hierarchy, type_to_cpp, sanitize_name, extract_types_from_cpp, g... | 2026-01-03 | 2026-03-22 |
| generate_comprehensive_inventory.py | We will scan ALL subdirectories. | find_description, is_likely_blueprint, get_blueprint_info, main | 2026-01-09 | 2026-03-22 |
| generate_dungeon_layout.py | Generate dungeon layout JSON from cell graph data. | normalize_2d, compute_corridor_walls, compute_room_perimeter_walls, main | 2026-03-09 | 2026-03-22 |
| generate_gameframework.py | Generate C++ headers and stubs for Game Framework classes. | type_to_cpp, sanitize_name, get_parent_info, generate_gameframework, main | 2026-01-03 | 2026-03-22 |
| generate_interfaces.py | Generate C++ Interface headers from Blueprint JSON exports. | sanitize_name, type_to_cpp, sanitize_param_name, generate_interface, main | 2026-01-03 | 2026-03-22 |
| generate_inventory.py | Directories to look for blueprints | find_description, get_blueprint_info, main | 2026-01-09 | 2026-03-22 |
| generate_inventory_v2.py | (no description) | get_description_from_metadata, get_blueprint_details, main | 2026-01-09 | 2026-03-22 |
| generate_logic_from_graphs.py | Graph Logic Parser and C++ Generator | **Pin, Node, GraphParser**; process_component, main | 2026-01-03 | 2026-03-22 |
| generate_migration_checklist.py | generate_migration_checklist.py | get_asset_game_path, get_asset_type, extract_graph_name, process_function_sig... | 2026-01-03 | 2026-03-22 |
| generate_migration_plan.py | generate_migration_plan.py | load_json, check_type_needs_migration, analyze_variables, analyze_node, analy... | 2026-01-03 | 2026-03-22 |
| generate_widgets.py | Generate C++ headers and stubs for Widget Blueprint classes. | type_to_cpp, sanitize_name, get_parent_info, generate_widget, main | 2026-01-03 | 2026-03-22 |
| migrate_actionmanager_automated.py | migrate_actionmanager_automated.py | log_section, log_step, check_prerequisites, get_pre_migration_diagnostics, cl... | 2026-01-03 | 2026-03-22 |
| migrate_actionmanager_to_cpp.py | migrate_actionmanager_to_cpp.py |  | 2026-01-03 | 2026-03-22 |
| migrate_all_components.py | migrate_all_components.py | log_banner, check_all_prerequisites, run_migration_script, verify_all_migrati... | 2026-01-03 | 2026-03-22 |
| migrate_animbp_aaa.py | migrate_animbp_aaa.py | log_separator, get_parent_class_name, migrate_animbp, main | 2026-01-26 | 2026-03-22 |
| migrate_animbp_complete.py | migrate_animbp_complete.py | main | 2026-01-26 | 2026-03-22 |
| migrate_animbp_preserve_wiring.py | AnimBP Migration - Preserve Wiring Approach | log, main | 2026-01-26 | 2026-03-22 |
| migrate_animbp_proper.py | migrate_animbp_proper.py | main | 2026-01-26 | 2026-03-22 |
| migrate_armor_only.py | migrate_armor_only.py | log, reparent_armor_assets, apply_armor_mesh_data, main | 2026-01-09 | 2026-03-22 |
| migrate_b_container_only.py | migrate_b_container_only.py | main | 2026-01-26 | 2026-03-22 |
| migrate_b_interactable_only.py | migrate_b_interactable_only.py | main | 2026-01-26 | 2026-03-22 |
| migrate_bb_enum.py | Migrate BB_Standard State key to use C++ ESLFAIStates enum | log, main | 2026-01-26 | 2026-03-22 |
| migrate_bb_enum_to_cpp.py | Migrate BB_Standard State key from Blueprint enum E_AI_States to C++ enum ESLFAI | main | 2026-01-26 | 2026-03-22 |
| migrate_bb_to_cpp.py | Migrate BB_Standard State key from E_AI_States to ESLFAIStates | log, main | 2026-01-26 | 2026-03-22 |
| migrate_boss_door_two_phase.py | Two-phase migration for B_BossDoor: | get_current_phase, set_phase, phase_1_clear, phase_2_reparent, main | 2026-01-26 | 2026-03-22 |
| migrate_bstat_full_cpp.py | migrate_bstat_full_cpp.py | reparent_blueprint, delete_function, main | 2026-01-03 | 2026-03-22 |
| migrate_bstat_to_cpp.py | migrate_bstat_to_cpp.py | migrate_bstat | 2026-01-03 | 2026-03-22 |
| migrate_bt_enum_to_cpp.py | Migrate BT decorators from Blueprint E_AI_States enum to C++ ESLFAIStates int va | log, fix_bt_decorator_intvalues, main | 2026-01-26 | 2026-03-22 |
| migrate_buffmanager_automated.py | migrate_buffmanager_automated.py | log_section, log_step, check_prerequisites, get_pre_migration_diagnostics, cl... | 2026-01-03 | 2026-03-22 |
| migrate_chest_tier_datatable.py | migrate_chest_tier_datatable.py | main | 2026-01-26 | 2026-03-22 |
| migrate_container_full.py | migrate_container_full.py | main | 2026-01-26 | 2026-03-22 |
| migrate_container_only.py | migrate_container_only.py | find_and_load_bp, clear_blueprint_nodes, migrate_container | 2026-01-26 | 2026-03-22 |
| migrate_container_v3.py | migrate_container_v3.py | clear_all_blueprint_content, migrate_container | 2026-01-26 | 2026-03-22 |
| migrate_container_v4.py | migrate_container_v4.py | get_blueprint_from_generated_class, clear_blueprint_content, migrate_container | 2026-01-26 | 2026-03-22 |
| migrate_destructibles.py | migrate_destructibles.py | migrate_blueprint, main | 2026-01-26 | 2026-03-22 |
| migrate_dialog_datatables.py | Migrate Dialog DataTables - Delete old Blueprint-struct tables and create fresh  | delete_old_datatables, create_datatable_with_cpp_struct, run_migration | 2026-03-09 | 2026-03-22 |
| migrate_dialog_datatables_cpp.py | Migrate Dialog DataTables using C++ SLFAutomationLibrary. | main | 2026-03-09 | 2026-03-22 |
| migrate_dodge_montages.py | migrate_dodge_montages.py | migrate_dodge_montages | 2026-01-04 | 2026-03-22 |
| migrate_enemy_animbps.py | migrate_enemy_animbps.py | main | 2026-01-26 | 2026-03-22 |
| migrate_enemy_animbps_aaa.py | migrate_enemy_animbps_aaa.py | log, get_blueprint_variables, rename_variable, clear_event_graphs, get_bluepr... | 2026-01-26 | 2026-03-22 |
| migrate_enum_coreredirects.py | migrate_enum_coreredirects.py |  | 2026-01-03 | 2026-03-22 |
| migrate_enum_direct.py | migrate_enum_direct.py |  | 2026-01-03 | 2026-03-22 |
| migrate_enum_full.py | migrate_enum_full.py |  | 2026-01-03 | 2026-03-22 |
| migrate_enum_to_cpp.py | migrate_enum_to_cpp.py |  | 2026-01-03 | 2026-03-22 |
| migrate_enum_values.py | migrate_enum_values.py | get_all_blueprints, find_enum_references_in_blueprint, main | 2026-01-03 | 2026-03-22 |
| migrate_enum_with_fixer.py | migrate_enum_with_fixer.py |  | 2026-01-03 | 2026-03-22 |
| migrate_execution_widget.py | Migrate W_TargetExecutionIndicator to C++ - clear function graphs then reparent. | migrate_single_widget, migrate_execution_widget | 2026-01-26 | 2026-03-22 |
| migrate_fregen_fstatoverride.py | migrate_fregen_fstatoverride.py | check_prerequisites, load_blueprint, fix_asset, main | 2026-01-03 | 2026-03-22 |
| migrate_fstatinfo.py | migrate_fstatinfo.py | check_prerequisites, load_blueprint, fix_asset, main | 2026-01-03 | 2026-03-22 |
| migrate_inputbuffer.py | migrate_inputbuffer.py | check_cpp_class_exists, load_blueprint, delete_blueprint_functions, delete_bl... | 2026-01-03 | 2026-03-22 |
| migrate_inputbuffer_automated.py | migrate_inputbuffer_automated.py | log_section, log_step, check_prerequisites, get_pre_migration_diagnostics, cl... | 2026-01-03 | 2026-03-22 |
| migrate_inputbuffer_continue.py | migrate_inputbuffer_continue.py | log_section, log_step | 2026-01-03 | 2026-03-22 |
| migrate_inputbuffer_full.py | migrate_inputbuffer_full.py | refresh_and_compile_blueprint, check_blueprint_subsystem, run_migration | 2026-01-03 | 2026-03-22 |
| migrate_inputbuffer_with_fixer.py | migrate_inputbuffer_with_fixer.py | step1_reparent, step2_analyze_dependents, step3_fix_dependents, run_migration | 2026-01-03 | 2026-03-22 |
| migrate_interactables.py | migrate_interactables.py | clear_blueprint_content, migrate_blueprint, migrate_all | 2026-01-26 | 2026-03-22 |
| migrate_interactables_fix.py | migrate_interactables_fix.py | clear_eventgraph_only, migrate_blueprint, main | 2026-01-26 | 2026-03-22 |
| migrate_interactables_sequential.py | Sequential migration for B_Interactable and B_RestingPoint | migrate_single, migrate_interactables | 2026-01-26 | 2026-03-22 |
| migrate_interactables_targeted.py | Targeted migration for B_Interactable and B_RestingPoint | migrate_interactables | 2026-01-26 | 2026-03-22 |
| migrate_interactables_v2.py | migrate_interactables_v2.py | clear_all_blueprint_content, migrate_blueprint, migrate_all | 2026-01-26 | 2026-03-22 |
| migrate_interactables_v3.py | migrate_interactables_v3.py | migrate_keep_vars, migrate_regular, main | 2026-01-26 | 2026-03-22 |
| migrate_interactables_v4.py | migrate_interactables_v4.py | migrate_blueprint, main | 2026-01-26 | 2026-03-22 |
| migrate_item_data.py | Full Item Data Migration Script | extract_nsloctext, extract_texture_path, extract_int, extract_bool, extract_enum | 2026-01-08 | 2026-03-22 |
| migrate_ladder_system.py | Migrate ladder system Blueprints to C++ using SLFAutomationLibrary. | migrate_and_validate | 2026-01-26 | 2026-03-22 |
| migrate_niagara_from_backup.py | migrate_niagara_from_backup.py | extract_niagara_from_backup, apply_niagara_to_cpp_property, main | 2026-01-07 | 2026-03-22 |
| migrate_npcs.py | Migrate NPC Blueprints to C++ parent classes using SLFAutomationLibrary. | migrate_npcs | 2026-03-09 | 2026-03-22 |
| migrate_npcs_preserve_scs.py | Migrate NPC Blueprints to C++ - Reparent and clear EventGraphs. | migrate_npcs | 2026-03-09 | 2026-03-22 |
| migrate_overlay_enum_to_cpp.py | Migrate AnimBP BlendListByEnum nodes from Blueprint E_OverlayState to C++ ESLFOv | log, main | 2026-01-26 | 2026-03-22 |
| migrate_pda_daynight.py | Migrate PDA_DayNight to C++ UPDA_DayNight and populate entries | migrate_pda_daynight | 2026-01-26 | 2026-03-22 |
| migrate_pda_default_mesh.py | Option B Migration: PDA_DefaultMeshData | log | 2026-01-07 | 2026-03-22 |
| migrate_pda_default_mesh_v2.py | Option B Migration v2: PDA_DefaultMeshData | log | 2026-01-07 | 2026-03-22 |
| migrate_settings_only.py | Migrate Settings Widgets and PDA_CustomSettings to C++ | reparent_and_clear, main | 2026-03-09 | 2026-03-22 |
| migrate_settings_step1_entry.py | Step 1: Clear and reparent W_Settings_Entry ONLY. |  | 2026-03-09 | 2026-03-22 |
| migrate_settings_step2_category.py | Step 2: Clear and reparent W_Settings_CategoryEntry ONLY. |  | 2026-03-09 | 2026-03-22 |
| migrate_settings_step3_pda.py | Step 3: Clear and reparent PDA_CustomSettings ONLY. |  | 2026-03-09 | 2026-03-22 |
| migrate_settings_step4_settings.py | Step 4: Clear and reparent W_Settings. |  | 2026-03-09 | 2026-03-22 |
| migrate_settings_widgets.py | Migrate Settings menu widgets to C++ using the standard pattern: | migrate_widget, main | 2026-01-26 | 2026-03-22 |
| migrate_settings_widgets_v2.py | Migrate Settings menu widgets to C++ using smarter clearing: | migrate_simple_widget, migrate_complex_widget, main | 2026-01-26 | 2026-03-22 |
| migrate_skymanager_proper.py | Proper two-pass clear-and-reparent for B_SkyManager | main | 2026-01-26 | 2026-03-22 |
| migrate_skymanager_v2.py | Two-pass SkyManager migration using BlueprintEditorLibrary | main | 2026-01-26 | 2026-03-22 |
| migrate_statmanager_automated.py | migrate_statmanager_automated.py | log_section, log_step, check_prerequisites, get_pre_migration_diagnostics, cl... | 2026-01-03 | 2026-03-22 |
| migrate_statmanager_full.py | migrate_statmanager_full.py |  | 2026-01-03 | 2026-03-22 |
| migrate_statmanager_to_cpp.py | migrate_statmanager_to_cpp.py |  | 2026-01-03 | 2026-03-22 |
| migrate_to_full_cpp.py | migrate_to_full_cpp.py | reparent_blueprint, delete_function, process_blueprint, main | 2026-01-03 | 2026-03-22 |
| migrate_w_resources.py | Migrate W_Resources - Clear FIRST, then reparent |  | 2026-01-26 | 2026-03-22 |
| migrate_w_resources_clean.py | Migrate W_Resources - Clear graphs, keep variables, then reparent |  | 2026-01-26 | 2026-03-22 |
| migrate_w_resources_fixed.py | Migrate W_Resources - Proper widget migration |  | 2026-01-26 | 2026-03-22 |
| migrate_w_resources_full_clear.py | Migrate W_Resources using FULL CLEAR like migrate_execution_widget.py |  | 2026-01-26 | 2026-03-22 |
| migrate_w_resources_no_clear.py | Migrate W_Resources - REPARENT ONLY with NO clearing at all |  | 2026-01-26 | 2026-03-22 |
| migrate_w_resources_preserve_widget_tree.py | Migrate W_Resources: Clear graphs (removes problematic Create Widget calls) |  | 2026-01-26 | 2026-03-22 |
| migrate_w_resources_reparent_only.py | Migrate W_Resources - REPARENT ONLY (no clearing) |  | 2026-01-26 | 2026-03-22 |
| patch_weapon_logging.py | Patch SLFWeaponBase.cpp to add world position logging |  | 2026-01-26 | 2026-03-22 |
| remove_duplicate_bm_component.py | Remove duplicate AC_AI_BehaviorManager SCS component from enemies | log, remove_scs_behavior_manager, main | 2026-01-26 | 2026-03-22 |
| remove_gi_interface.py | Remove interface implementation from GI_SoulslikeFramework. | log, main | 2026-03-09 | 2026-03-22 |
| remove_npc_functions.py | Remove specific conflicting functions from NPC Blueprints. | main | 2026-03-09 | 2026-03-22 |
| remove_overlay_blueprint_vars.py | Remove Blueprint overlay state variables that shadow C++ properties | log, main | 2026-03-09 | 2026-03-22 |
| remove_question_mark_vars.py | Remove Blueprint variables with '?' suffix from AnimBP. | log, main | 2026-01-26 | 2026-03-22 |
| rename_overlay_vars.py | rename_overlay_vars.py | log, main | 2026-01-10 | 2026-03-22 |
| rename_question_mark_vars.py | Rename '?' suffix variables to match C++ property names. | log, main | 2026-01-26 | 2026-03-22 |
| restore_animbp_assets.py | restore_animbp_assets.py | main | 2026-01-26 | 2026-03-22 |
| restore_animbp_surgical.py | Surgical AnimBP restore - keeps AnimGraph, clears EventGraph only. | log, restore_from_backup, clear_event_graph_only, reparent_to_cpp, compile_an... | 2026-01-26 | 2026-03-22 |
| restore_blueprint_values.py | restore_blueprint_values.py | load_asset_reference, set_property_from_value, restore_blueprint_values, run | 2026-01-03 | 2026-03-22 |
| restore_bt_assets.py | restore_bt_assets.py | main | 2026-01-26 | 2026-03-22 |
| restore_from_bp_only.py | Files to restore from bp_only |  | 2026-01-03 | 2026-03-22 |
| restore_level_bp_dependencies.py | Restore B_SequenceActor and GI_SoulslikeFramework from bp_only WITHOUT reparenti | log, restore_assets, main | 2026-03-09 | 2026-03-22 |
| restore_weapon_blueprints.py | Restore weapon Blueprints from backup to recover SCS component hierarchy. | restore_files | 2026-01-03 | 2026-03-22 |
| set_default_meshes.py | (no description) | set_default_meshes | 2026-01-09 | 2026-03-22 |
| set_execution_widget_class.py | Set ExecutionWidget WidgetClass using SubobjectDataSubsystem. | set_execution_widget_class | 2026-01-26 | 2026-03-22 |
| set_poise_break_default.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| update_bt_decorators.py | update_bt_decorators.py | get_bt_info, diagnose_all_bts, check_cpp_decorator_class, create_state_decora... | 2026-01-26 | 2026-03-22 |
| update_mesh_loading.py | Update SLFSoulslikeCharacter.cpp to use direct property access instead of reflec |  | 2026-01-07 | 2026-03-22 |
| update_w_equipment.py | Script to add all event declarations to W_Equipment.h |  | 2026-01-03 | 2026-03-22 |
| update_w_equipment_cpp.py | Script to add all event implementations to W_Equipment.cpp |  | 2026-01-03 | 2026-03-22 |
| update_w_hud_cpp.py | Script to add all event implementations to W_HUD.cpp |  | 2026-01-03 | 2026-03-22 |
| update_w_hud_header.py | Script to add all event declarations to W_HUD.h |  | 2026-01-03 | 2026-03-22 |
| update_w_inventory.py | Script to add all event declarations to W_Inventory.h |  | 2026-01-03 | 2026-03-22 |
| update_w_inventory_cpp.py | Script to add all event implementations to W_Inventory.cpp |  | 2026-01-03 | 2026-03-22 |
| update_weapon_cpp.py | Update SLFWeaponBase.cpp with interface implementations | main | 2026-01-26 | 2026-03-22 |

## Extraction Scripts (`scripts/extraction`)
**171 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| dump_weapon_properties.py | dump_weapon_properties.py | log, dump_props | 2026-01-26 | 2026-03-22 |
| export_ai_system_full.py | export_ai_system_full.py | ensure_dir, export_blueprint_full, export_behavior_tree, export_blackboard, e... | 2026-01-26 | 2026-03-22 |
| export_all_bt_subtrees.py | export_all_bt_subtrees.py | log, export_subtrees | 2026-01-26 | 2026-03-22 |
| export_all_transitions.py | export_all_transitions.py | log, main | 2026-01-26 | 2026-03-22 |
| export_all_with_keytypes.py | Export All Blueprints with TMap KeyTypes - Using Enhanced PythonBridge | get_category, export_all, export_single, validate_keytype_export | 2026-01-03 | 2026-03-22 |
| export_and_check_animgraph.py | Export AnimGraph and check for broken bIsBlocking/bIsFalling connections | log, main | 2026-01-26 | 2026-03-22 |
| export_animbp_bponly.py | export_animbp_bponly.py | main | 2026-01-26 | 2026-03-22 |
| export_animbp_complete.py | export_animbp_complete.py | main | 2026-01-26 | 2026-03-22 |
| export_animbp_detailed.py | export_animbp_detailed.py | log, get_pin_info, get_node_info, export_graph, main | 2026-01-26 | 2026-03-22 |
| export_animbp_from_bponly.py | Export AnimGraph from bp_only project to see working LL_TwoHanded implementation | log, main | 2026-03-09 | 2026-03-22 |
| export_animbp_vars.py | (no description) |  | 2026-01-26 | 2026-03-22 |
| export_animbp_via_cpp.py | export_animbp_via_cpp.py | log, main | 2026-01-26 | 2026-03-22 |
| export_blueprint_state.py | Export comprehensive Blueprint state for comparison between projects. | get_class_hierarchy, get_component_info, export_blueprint, main | 2026-01-26 | 2026-03-22 |
| export_boss_door_complete.py | Export COMPLETE B_BossDoor configuration from bp_only project. | export_boss_door_complete | 2026-01-26 | 2026-03-22 |
| export_boss_door_text.py | Export B_BossDoor text to analyze SCS components | main | 2026-01-26 | 2026-03-22 |
| export_bp_only_config.py | COMPREHENSIVE CONFIG EXPORT FOR BP_ONLY | log, ensure_dir, export_skeletal_mesh_sockets, export_character_config, expor... | 2026-01-26 | 2026-03-22 |
| export_bp_only_door.py | Export complete door state from bp_only project. | export_door_complete | 2026-01-26 | 2026-03-22 |
| export_bt_decorators.py | export_bt_decorators.py | log, export_bt_decorators | 2026-01-26 | 2026-03-22 |
| export_bt_state.py | Export BT text and analyze State decorators | log, main | 2026-01-26 | 2026-03-22 |
| export_character_state.py | Export B_Soulslike_Character current state to check for BeginPlay override. | log, main | 2026-01-26 | 2026-03-22 |
| export_complete_animbp.py | export_complete_animbp.py | log, export_pin_full, export_node_full, export_graph_full, main | 2026-01-26 | 2026-03-22 |
| export_container_instance_data.py | export_container_instance_data.py | main | 2026-01-26 | 2026-03-22 |
| export_container_t3d.py | export_container_t3d.py | main | 2026-01-26 | 2026-03-22 |
| export_current_animbp.py | Export complete AnimGraph from current SLFConversion project | log, main | 2026-03-09 | 2026-03-22 |
| export_daynight_raw.py | Export DayNight asset as raw text to find the time values | main | 2026-01-26 | 2026-03-22 |
| export_dialog_comparison.py | Export dialog system configuration from both bp_only and SLFConversion for compa | get_struct_info, export_datatable_info, export_data_asset_info, export_widget... | 2026-03-09 | 2026-03-22 |
| export_door_state.py | Export complete door state from both projects for comparison. | export_blueprint_graphs, export_animbp_state, main | 2026-01-26 | 2026-03-22 |
| export_enemy_scs.py | Export enemy weapon and mesh info via CDO inspection | main | 2026-01-26 | 2026-03-22 |
| export_enemy_weapon_info.py | Export enemy weapon ChildActorClass and skeleton mesh info | main | 2026-01-26 | 2026-03-22 |
| export_enemy_weapon_text.py | Export enemy Blueprint text to analyze weapon configurations. | main | 2026-01-26 | 2026-03-22 |
| export_full_config.py | COMPREHENSIVE CONFIG EXPORT | log, ensure_dir, export_skeleton_sockets, export_skeletal_mesh_sockets, expor... | 2026-01-26 | 2026-03-22 |
| export_hud_t3d.py | Export W_HUD to T3D format to see widget configuration. | export_hud | 2026-03-09 | 2026-03-22 |
| export_item_wheel_config.py | Export item wheel slot configuration from W_HUD to understand slot organization. | export_config | 2026-03-09 | 2026-03-22 |
| export_level_text.py | Export level to text format to find B_StatusEffectArea instances. | log, export_level | 2026-01-26 | 2026-03-22 |
| export_levelup_widget.py | export_levelup_widget.py | export_widget_hierarchy, main | 2026-01-26 | 2026-03-22 |
| export_levelup_widgets.py | Export all level-up related widgets from bp_only | export_widget_blueprint, main | 2026-01-26 | 2026-03-22 |
| export_npc_yaml_bp_only.py | Export NPC System YAML documentation from bp_only project |  | 2026-03-09 | 2026-03-22 |
| export_npc_yaml_bponly.py | Export ALL NPC system assets from bp_only to YAML. | get_class_hierarchy, get_all_properties, export_blueprint_details, export_com... | 2026-03-09 | 2026-03-22 |
| export_pda_daynight.py | Export PDA_DayNight structure and DA_ExampleDayNightInfo entries from bp_only | export_daynight | 2026-01-26 | 2026-03-22 |
| export_projectile_scs.py | Export SCS components from B_BaseProjectile and B_Projectile_ThrowingKnife | get_scs_components, export_blueprint_info, main | 2026-03-09 | 2026-03-22 |
| export_property_access_paths.py | export_property_access_paths.py | log, main | 2026-01-26 | 2026-03-22 |
| export_restingpoint_full.py | (no description) | export_restingpoint_details | 2026-01-26 | 2026-03-22 |
| export_settings_current.py | Export W_Settings widget configuration from CURRENT project for comparison to bp | get_widget_property, color_to_dict, tag_to_string, export_settings_widget, main | 2026-03-09 | 2026-03-22 |
| export_skeleton_raw.py | Export skeleton raw text to find socket data | log, extract_sockets_from_text, main | 2026-01-26 | 2026-03-22 |
| export_skeleton_sockets.py | Export skeleton sockets directly from skeleton assets | log, export_skeleton, main | 2026-01-26 | 2026-03-22 |
| export_skeleton_text.py | Export skeleton assets as text to find socket data | log, main | 2026-01-26 | 2026-03-22 |
| export_slfconv_door.py | Export complete door state from SLFConversion project. | export_door_complete | 2026-01-26 | 2026-03-22 |
| export_via_cpp_comprehensive.py | export_via_cpp_comprehensive.py | log, main | 2026-01-26 | 2026-03-22 |
| export_w_resources_t3d.py | Export W_Resources to T3D format to see raw widget properties |  | 2026-01-26 | 2026-03-22 |
| export_w_settings_entries.py | Export W_Settings to see how entry instances are configured. | export_w_settings, export_w_settings_entry_blueprint, check_entry_default_val... | 2026-01-26 | 2026-03-22 |
| export_weapon_scs_data.py | export_weapon_scs_data.py | log, parse_vector, parse_rotator, export_asset_to_text, export_weapons | 2026-01-26 | 2026-03-22 |
| export_weapon_socket_data.py | export_weapon_socket_data.py | log, main | 2026-01-26 | 2026-03-22 |
| export_widget_t3d.py | Export W_Settings_CategoryEntry widget as text to compare between projects. |  | 2026-01-26 | 2026-03-22 |
| extract_abilities_v2.py | extract_abilities_v2.py | log, extract_abilities, main | 2026-01-26 | 2026-03-22 |
| extract_action_data.py | extract_action_data.py | extract_action_data | 2026-01-07 | 2026-03-22 |
| extract_ai_weapon_itemdata.py | Extract Item Data references from AI weapon Blueprints. | log | 2026-03-09 | 2026-03-22 |
| extract_ai_weapon_meshes.py | Extract AI weapon mesh data from backup (bp_only). | extract_ai_weapon_meshes | 2026-03-09 | 2026-03-22 |
| extract_all_stat_defaults.py | Extract ALL stat CDO defaults from bp_only project. | extract_stat_defaults | 2026-01-26 | 2026-03-22 |
| extract_and_compare_settings.py | Extract embedded settings from both bp_only and current W_Settings, | main | 2026-03-09 | 2026-03-22 |
| extract_armor_data.py | extract_armor_data.py | debug_log, get_property_value, list_struct_properties, extract_via_copy_expor... | 2026-01-09 | 2026-03-22 |
| extract_armor_stat_changes.py | extract_armor_stat_changes.py | extract_armor_stat_changes | 2026-01-26 | 2026-03-22 |
| extract_base_stats_bponly.py | extract_base_stats_bponly.py | extract_base_stats | 2026-01-26 | 2026-03-22 |
| extract_blueprint_values.py | extract_blueprint_values.py | get_property_value_as_string, extract_cdo_properties, extract_component_defau... | 2026-01-03 | 2026-03-22 |
| extract_boss_door_components.py | Extract B_BossDoor component data via export_text | parse_scs_components, extract_blueprint_data, main | 2026-01-26 | 2026-03-22 |
| extract_boss_door_config.py | Extract complete B_BossDoor configuration from bp_only project. | vector_to_dict, rotator_to_dict, transform_to_dict, get_safe_property, extrac... | 2026-01-26 | 2026-03-22 |
| extract_boss_door_from_level.py | Extract B_BossDoor configuration by loading the demo level and examining instanc | vector_to_dict, rotator_to_dict, main | 2026-01-26 | 2026-03-22 |
| extract_boss_door_full.py | Extract complete B_BossDoor configuration from bp_only project. | vector_to_dict, rotator_to_dict, get_safe_property, extract_component_from_sc... | 2026-01-26 | 2026-03-22 |
| extract_boss_door_scale.py | Extract B_BossDoor exact scale and location from bp_only backup | main | 2026-01-26 | 2026-03-22 |
| extract_boss_door_text.py | Extract B_BossDoor configuration using asset text export | export_bp_text, parse_component_from_text, parse_scale_from_text, parse_locat... | 2026-01-26 | 2026-03-22 |
| extract_boss_door_transforms.py | Extract exact component transforms from B_BossDoor in bp_only | vector_to_dict, rotator_to_dict, main | 2026-01-26 | 2026-03-22 |
| extract_bp_only_icons.py | Export W_Settings to text to see per-instance values |  | 2026-01-26 | 2026-03-22 |
| extract_bp_only_mesh_transform.py | extract_bp_only_mesh_transform.py | log, main | 2026-01-26 | 2026-03-22 |
| extract_bp_only_mesh_v2.py | extract_bp_only_mesh_v2.py | log, main | 2026-01-26 | 2026-03-22 |
| extract_bp_only_scs.py | extract_bp_only_scs.py | log, main | 2026-01-26 | 2026-03-22 |
| extract_bp_only_transforms.py | extract_bp_only_transforms.py | log, extract_transforms | 2026-01-26 | 2026-03-22 |
| extract_bponly_container.py | extract_bponly_container.py | main | 2026-01-26 | 2026-03-22 |
| extract_bponly_door.py | Extract B_Door data from bp_only project. | extract_door_data | 2026-01-26 | 2026-03-22 |
| extract_bponly_door_simple.py | Extract B_Door data from bp_only project. |  | 2026-01-26 | 2026-03-22 |
| extract_button_border_brush.py | (no description) |  | 2026-01-26 | 2026-03-22 |
| extract_button_cdo.py | Load W_GenericButton and examine its widget tree and CDO |  | 2026-01-26 | 2026-03-22 |
| extract_button_t3d.py | Export W_GenericButton as T3D to see CDO values |  | 2026-01-26 | 2026-03-22 |
| extract_category_icons.py | Load W_Settings widget blueprint |  | 2026-01-26 | 2026-03-22 |
| extract_character_basestats.py | Extract BaseStats from bp_only character class data assets. | extract_basestats | 2026-01-26 | 2026-03-22 |
| extract_chest_tier_data.py | extract_chest_tier_data.py | main | 2026-01-26 | 2026-03-22 |
| extract_chest_tier_from_backup.py | extract_chest_tier_from_backup.py | main | 2026-01-26 | 2026-03-22 |
| extract_container_data.py | extract_container_data.py | extract_container_data | 2026-01-26 | 2026-03-22 |
| extract_container_loot.py | extract_container_loot.py | main | 2026-01-26 | 2026-03-22 |
| extract_container_loot_config.py | extract_container_loot_config.py | main | 2026-01-26 | 2026-03-22 |
| extract_container_loot_cpp.py | extract_container_loot_cpp.py | main | 2026-01-26 | 2026-03-22 |
| extract_container_timeline.py | extract_container_timeline.py | main | 2026-01-26 | 2026-03-22 |
| extract_daynight_detailed.py | Extract DayNight entries from bp_only with all struct details | main | 2026-01-26 | 2026-03-22 |
| extract_daynight_entries.py | Extract DayNight entries from bp_only PDA_DayNight | main | 2026-01-26 | 2026-03-22 |
| extract_daynight_from_backup.py | Extract DayNight entries from bp_only to see correct time values | main | 2026-01-26 | 2026-03-22 |
| extract_death_montages.py | extract_death_montages.py | extract_death_montages | 2026-01-26 | 2026-03-22 |
| extract_dialog_data.py | Extract dialog data from bp_only DataTables. |  | 2026-03-09 | 2026-03-22 |
| extract_dialog_data_v2.py | Extract dialog data from bp_only DataTables using export. |  | 2026-03-09 | 2026-03-22 |
| extract_embedded_settings.py | Extract embedded W_Settings_Entry and W_Settings_CategoryEntry widget instances | extract_widget_properties, get_all_children_recursive, main | 2026-03-09 | 2026-03-22 |
| extract_enemy_abilities.py | extract_enemy_abilities.py | log, extract_abilities, main | 2026-01-26 | 2026-03-22 |
| extract_enemy_bt.py | Extract BehaviorTree assignments from backup enemies | log, check_enemy, main | 2026-01-26 | 2026-03-22 |
| extract_enemy_bt_v2.py | Extract BehaviorTree assignments from backup enemies - v2 | log, check_enemy, main | 2026-01-26 | 2026-03-22 |
| extract_enemy_bt_v3.py | Extract BehaviorTree from Blueprint SCS (SimpleConstructionScript) | log, check_blueprint_scs, main | 2026-01-26 | 2026-03-22 |
| extract_enemy_weapon_config.py | Extract enemy weapon configuration from bp_only to understand how enemies apply  | log | 2026-03-09 | 2026-03-22 |
| extract_execution_data.py | Extract execution animation data from bp_only project. | extract_execution_data | 2026-01-26 | 2026-03-22 |
| extract_from_backup.py | Extract widget instance properties from BACKUP W_Inventory Blueprint | log, extract_from_backup | 2026-01-09 | 2026-03-22 |
| extract_generic_button_config.py | (no description) | extract_widget_config, extract_widget_hierarchy | 2026-01-26 | 2026-03-22 |
| extract_icon_paths.py | extract_icon_paths.py | extract_icon_paths | 2026-01-09 | 2026-03-22 |
| extract_ikrig_details.py | Extract detailed IK Rig node configuration from AnimBP assets. | get_animgraph_ik_details | 2026-01-26 | 2026-03-22 |
| extract_item_data_full.py | !/usr/bin/env python | extract_item_data | 2026-01-26 | 2026-03-22 |
| extract_item_full_data.py | Extract full item data from backup Blueprint assets. | extract_item_data, extract_from_text_export, parse_item_info, main | 2026-01-09 | 2026-03-22 |
| extract_item_info.py | extract_item_info.py | extract_item_info | 2026-01-09 | 2026-03-22 |
| extract_item_info_backup.py | extract_item_info_backup.py | extract_item_info | 2026-01-09 | 2026-03-22 |
| extract_item_info_from_backup.py | extract_item_info_from_backup.py | extract_from_json_exports | 2026-01-09 | 2026-03-22 |
| extract_level_instance_data.py | Extract B_BossDoor instance data from level maps to see scale/transform values | vector_to_dict, rotator_to_dict, main | 2026-01-26 | 2026-03-22 |
| extract_level_t3d.py | Export level actors to text format and search for StatusEffectArea data. | log, extract_level_data | 2026-01-26 | 2026-03-22 |
| extract_mesh_data_bp.py | extract_mesh_data_bp.py | log, extract_mesh_paths_from_export, main | 2026-01-26 | 2026-03-22 |
| extract_niagara_paths.py | extract_niagara_paths.py | extract_niagara_paths | 2026-01-04 | 2026-03-22 |
| extract_npc_config.py | Extract NPC configuration data from bp_only project. | extract_npc_data, main | 2026-03-09 | 2026-03-22 |
| extract_npc_config_complete.py | Extract complete NPC configuration from bp_only Blueprints. | main | 2026-03-09 | 2026-03-22 |
| extract_npc_data.py | extract_npc_data.py | debug_log, get_property_value, list_all_properties, extract_component_data, e... | 2026-03-09 | 2026-03-22 |
| extract_npc_level_config.py | Extract NPC level instance configuration from bp_only. | main | 2026-03-09 | 2026-03-22 |
| extract_npc_scs.py | Extract NPC SCS component configuration from bp_only Blueprints. | main | 2026-03-09 | 2026-03-22 |
| extract_npc_text.py | Export NPC Blueprint text to find component configuration. | main | 2026-03-09 | 2026-03-22 |
| extract_npc_via_text.py | Extract NPC configuration from bp_only using text export. | main | 2026-03-09 | 2026-03-22 |
| extract_player_transforms.py | extract_player_transforms.py | log, extract_transforms | 2026-01-26 | 2026-03-22 |
| extract_player_weapon_transforms.py | extract_player_weapon_transforms.py | log, extract_transforms | 2026-01-26 | 2026-03-22 |
| extract_radar_config.py | Load the AC_RadarManager Blueprint |  | 2026-01-26 | 2026-03-22 |
| extract_scs_weapon.py | Extract Weapon ChildActorComponent configuration from Blueprint SCS. | main, extract_scs_node | 2026-01-26 | 2026-03-22 |
| extract_settings_data_cpp.py | Extract settings widget data using C++ SLFAutomationLibrary. |  | 2026-03-09 | 2026-03-22 |
| extract_settings_data_python.py | Extract Settings Widget data from bp_only using pure Python | color_to_dict, extract_settings_data | 2026-03-09 | 2026-03-22 |
| extract_settings_entries.py | Extract all W_Settings_Entry instance properties from the W_Settings widget. | extract_entry_properties, extract_category_properties, main | 2026-03-09 | 2026-03-22 |
| extract_settings_from_backup.py | Extract settings widget data from bp_only backup. | get_linear_color, get_gameplay_tag | 2026-03-09 | 2026-03-22 |
| extract_settings_icons.py | Load W_Settings widget blueprint |  | 2026-01-26 | 2026-03-22 |
| extract_settings_icons_v2.py | Load W_Settings widget blueprint | get_export_text | 2026-01-26 | 2026-03-22 |
| extract_settings_simple.py | Simple settings data extraction using Python reflection. | extract_all_properties, main | 2026-03-09 | 2026-03-22 |
| extract_stat_cdo_detailed.py | Extract detailed stat CDO values from bp_only including FStatInfo struct. | extract_stat_info, main | 2026-01-26 | 2026-03-22 |
| extract_stat_defaults_full.py | extract_stat_defaults_full.py | extract_stat_info, main | 2026-01-26 | 2026-03-22 |
| extract_stat_defaults_reflect.py | extract_stat_defaults_reflect.py | list_all_properties, extract_stat_info, main | 2026-01-26 | 2026-03-22 |
| extract_stat_defaults_t3d.py | extract_stat_defaults_t3d.py | parse_stat_from_text, extract_stat_info, main | 2026-01-26 | 2026-03-22 |
| extract_status_effect_icons.py | Extract status effect icon data from backup (bp_only) for migration. | extract_status_effect_icons | 2026-03-09 | 2026-03-22 |
| extract_statuseffect_cdo.py | Extract B_StatusEffectArea Blueprint CDO defaults. | log, extract_cdo | 2026-01-26 | 2026-03-22 |
| extract_statuseffectarea_instances.py | Extract B_StatusEffectArea instance data from bp_only level. | log, extract_statuseffectarea_instances | 2026-01-26 | 2026-03-22 |
| extract_strings.py | Extract readable strings from uasset files to find mesh references. | extract_strings, main | 2026-01-07 | 2026-03-22 |
| extract_tracking_json.py | extract_tracking_json.py | map_type, extract_variable, extract_function, extract_event_dispatcher, analy... | 2026-01-03 | 2026-03-22 |
| extract_vendor_data.py | Extract vendor item data from bp_only DA_ExampleVendor for migration to C++ UPDA | extract_vendor_data | 2026-03-09 | 2026-03-22 |
| extract_vendor_mesh.py | Extract vendor NPC mesh configuration from bp_only | extract_vendor_mesh | 2026-03-09 | 2026-03-22 |
| extract_vendor_mesh_v2.py | Extract vendor NPC mesh configuration from bp_only - v2 | extract_vendor_mesh | 2026-03-09 | 2026-03-22 |
| extract_vendor_meshes.py | Extract mesh assignments from B_Soulslike_NPC_ShowcaseVendor's SCS components. | extract_vendor_meshes | 2026-03-09 | 2026-03-22 |
| extract_vendor_meshes_v2.py | Extract mesh assignments from B_Soulslike_NPC_ShowcaseVendor using text export. | extract_vendor_meshes | 2026-03-09 | 2026-03-22 |
| extract_w_resources_basewidth.py | Extract W_Resources BaseWidth values from bp_only |  | 2026-01-26 | 2026-03-22 |
| extract_w_resources_basewidth_v2.py | Extract BaseWidth values from W_Resources in bp_only |  | 2026-01-26 | 2026-03-22 |
| extract_w_resources_sizebox.py | Extract W_Resources SizeBox dimensions from bp_only |  | 2026-01-26 | 2026-03-22 |
| extract_w_resources_sizebox_v2.py | Extract W_Resources SizeBox dimensions from bp_only |  | 2026-01-26 | 2026-03-22 |
| extract_w_resources_widget_styling.py | Extract W_Resources widget tree properties |  | 2026-01-26 | 2026-03-22 |
| extract_weapon_abilities.py | Extract WeaponAbility property from bp_only weapon data assets. | extract_weapon_abilities | 2026-03-09 | 2026-03-22 |
| extract_weapon_config.py | Extract weapon configuration from enemy Blueprints. | main | 2026-01-26 | 2026-03-22 |
| extract_weapon_data_raw.py | extract_weapon_data_raw.py | log, export_asset_text, extract_weapon_data | 2026-01-26 | 2026-03-22 |
| extract_weapon_iteminfo.py | extract_weapon_iteminfo.py | log, extract_iteminfo | 2026-01-26 | 2026-03-22 |
| extract_weapon_mesh_info.py | Extract weapon mesh and component configuration from weapon Blueprints. | main | 2026-01-26 | 2026-03-22 |
| extract_weapon_movesets.py | extract_weapon_movesets.py | log, extract_movesets | 2026-01-26 | 2026-03-22 |
| extract_weapon_pda_data.py | extract_weapon_pda_data.py | log, extract_pda_data | 2026-01-26 | 2026-03-22 |
| extract_weapon_stats.py | Extract weapon stat data from bp_only project | extract_weapon_stats | 2026-01-26 | 2026-03-22 |
| extract_weapon_stats_t3d.py | Extract weapon stat data from bp_only using T3D export | extract_weapon_stats_t3d | 2026-01-26 | 2026-03-22 |
| extract_weapon_statuseffects.py | Extract WeaponStatusEffectInfo from bp_only backup. | log, extract | 2026-01-26 | 2026-03-22 |
| extract_weapon_statuseffects_bponly.py | (no description) |  | 2026-01-03 | 2026-03-22 |
| extract_weapon_transforms.py | Extract weapon transforms from bp_only project | log, extract_weapon, main | 2026-01-26 | 2026-03-22 |
| extract_widget_instance_data.py | Extract widget instance property data from BACKUP W_Inventory | log, extract_widget_instance_data, find_all_widgets | 2026-01-09 | 2026-03-22 |
| extract_widget_properties.py | Extract widget instance properties from W_Inventory Blueprint | log, extract_widget_properties | 2026-01-09 | 2026-03-22 |
| get_daynight_data.py | Get DayNight entries from DA_ExampleDayNightInfo instance |  | 2026-01-26 | 2026-03-22 |
| get_widget_dirs.py | (no description) |  | 2026-01-03 | 2026-03-22 |
| parse_backup_widget_data.py | Parse backup .uasset files to extract widget instance property values. | extract_texture_refs, extract_category_data, analyze_widget_uasset, main | 2026-01-09 | 2026-03-22 |
| parse_weapon_t3d.py | Parse weapon T3D files and extract stat data | parse_t3d_file, main | 2026-01-26 | 2026-03-22 |

## Validation Scripts (`scripts/validation`)
**457 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| check_abp_parent.py | (no description) |  | 2026-03-09 | 2026-03-22 |
| check_ac_ai_combat_manager.py | Check the AC_AI_CombatManager Blueprint component | log | 2026-03-09 | 2026-03-22 |
| check_action_montage.py | Check current state of DA_Action_PickupItemMontage |  | 2026-03-09 | 2026-03-22 |
| check_action_stamina.py | Check stamina costs on action data assets |  | 2026-01-26 | 2026-03-22 |
| check_action_struct_types.py | check_action_struct_types.py | check_action_struct_types | 2026-01-07 | 2026-03-22 |
| check_aic_parent.py | Check AIC_SoulslikeFramework Blueprint parent class | log, main | 2026-01-26 | 2026-03-22 |
| check_all_lighting.py | Comprehensive check of ALL lighting actors and components in L_Demo_Showcase | log, main | 2026-01-26 | 2026-03-22 |
| check_all_sockets.py | check_all_sockets.py | log, main | 2026-01-26 | 2026-03-22 |
| check_all_weapon_status.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_all_weapon_status_v2.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_animbp_animdata.py | Check AnimDataAsset in AnimBP - Blueprint var vs C++ property | log, main | 2026-03-09 | 2026-03-22 |
| check_animbp_compile.py | Quick check to verify AnimBP loads and compiles without enum errors. | main | 2026-01-10 | 2026-03-22 |
| check_animbp_errormsg.py | check_animbp_errormsg.py | log, main | 2026-01-26 | 2026-03-22 |
| check_animbp_eventgraph.py | Check and clear EventGraphs from ABP_SoulslikeEnemy and ABP_SoulslikeBossNew | log, main | 2026-03-09 | 2026-03-22 |
| check_animbp_graphs.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_animbp_parent.py | check_animbp_parent.py | log, main | 2026-01-26 | 2026-03-22 |
| check_animbp_parent_class.py | Check AnimBP parent class to verify C++ reparenting. | log, main | 2026-01-26 | 2026-03-22 |
| check_animbp_parent_simple.py | Simple check of AnimBP parent class using C++ automation. | log, main | 2026-01-26 | 2026-03-22 |
| check_animbp_parents.py | check_animbp_parents.py | main | 2026-01-26 | 2026-03-22 |
| check_animbp_vars.py | Check AnimBP variables - both Blueprint and inherited C++ properties | log, main | 2026-01-26 | 2026-03-22 |
| check_ans_toggle_chaos.py | Check ANS_ToggleChaosField Blueprint parent class. | check_notify | 2026-01-26 | 2026-03-22 |
| check_armor_current_state.py | check_armor_current_state.py | check_armor | 2026-01-26 | 2026-03-22 |
| check_armor_stat_changes.py | check_armor_stat_changes.py | tag_to_string, check_armor_stat_changes | 2026-01-26 | 2026-03-22 |
| check_b_weight.py | check_b_weight.py | check_b_weight | 2026-01-26 | 2026-03-22 |
| check_backup_bt_services.py | check_backup_bt_services.py | log, check_services_recursive, main | 2026-01-26 | 2026-03-22 |
| check_backup_input.py | check_backup_input.py | log | 2026-03-09 | 2026-03-22 |
| check_backup_tags.py | Check what item tags exist in the BACKUP project | log | 2026-03-09 | 2026-03-22 |
| check_base_stats_map.py | check_base_stats_map.py | check_base_stats | 2026-01-26 | 2026-03-22 |
| check_base_weapon_components.py | Check B_Item_Weapon base class component setup | log, check_base_weapon | 2026-01-26 | 2026-03-22 |
| check_behaviormanager_parent.py | check_behaviormanager_parent.py | main | 2026-01-26 | 2026-03-22 |
| check_boss_ability_montages.py | Check Boss Ability Montage Assignments | check_boss_abilities | 2026-01-26 | 2026-03-22 |
| check_boss_animbp_state.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_boss_backup.py | Check boss configuration in bp_only (backup) project. | log, write_output, check_boss_backup | 2026-03-09 | 2026-03-22 |
| check_boss_components.py | Check what components the boss character has | log | 2026-03-09 | 2026-03-22 |
| check_boss_components_full.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_boss_config.py | Check boss configuration in SLFConversion project. | log, write_output, check_boss_config | 2026-03-09 | 2026-03-22 |
| check_boss_door_montage.py | Check AM_SLF_OpenDoor_Boss montage length |  | 2026-01-26 | 2026-03-22 |
| check_boss_door_only.py | Check B_BossDoor only - since it works, understand what makes it work | get_bp_info | 2026-01-26 | 2026-03-22 |
| check_boss_door_parent_bponly.py | Check B_BossDoor parent class in bp_only backup | main | 2026-01-26 | 2026-03-22 |
| check_boss_door_scs.py | Check B_BossDoor SCS node details in bp_only | main | 2026-01-26 | 2026-03-22 |
| check_boss_final.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_boss_parent.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_both_doors_fixed.py | Check both B_Door and B_BossDoor - after restoring B_Door from git | get_bp_info | 2026-01-26 | 2026-03-22 |
| check_bp_only_niagara.py | Load the Blueprint asset | check_resting_point | 2026-01-26 | 2026-03-22 |
| check_bp_only_sockets.py | Check socket names in bp_only backup | log, check_weapons | 2026-01-26 | 2026-03-22 |
| check_bp_parent.py | check_bp_parent.py | log, check | 2026-01-26 | 2026-03-22 |
| check_bp_parent_class.py | check_bp_parent_class.py | main | 2026-01-26 | 2026-03-22 |
| check_bp_parent_v2.py | check_bp_parent_v2.py | main | 2026-01-26 | 2026-03-22 |
| check_bp_parent_v3.py | check_bp_parent_v3.py | main | 2026-01-26 | 2026-03-22 |
| check_bponly_animbp.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_bponly_boss.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_bponly_container_delay.py | check_bponly_container_delay.py | main | 2026-01-26 | 2026-03-22 |
| check_bponly_equip_slots.py | Check weapon EquipSlots tags in bp_only backup. | check_bponly_equip_slots | 2026-01-26 | 2026-03-22 |
| check_bponly_poise.py | Run this on bp_only project to check AC_AI_CombatManager defaults | log | 2026-03-09 | 2026-03-22 |
| check_bponly_rankinfo.py | Check RankInfo configuration on status effect data assets in bp_only backup. | log, check_rankinfo | 2026-01-26 | 2026-03-22 |
| check_bponly_skymanager.py | Check bp_only B_SkyManager for SCS components | main | 2026-01-26 | 2026-03-22 |
| check_bt_decorators.py | Check BT_Enemy decorator IntValues | log, check_bt_decorators, main | 2026-01-26 | 2026-03-22 |
| check_bt_decorators_v2.py | Check BT_Enemy decorator configuration by iterating nodes | log, check_decorators, main | 2026-01-26 | 2026-03-22 |
| check_bt_enemy_decorators.py | Check BT_Enemy main tree decorators for State key comparison. | main | 2026-01-26 | 2026-03-22 |
| check_bt_services.py | check_bt_services.py | main | 2026-01-26 | 2026-03-22 |
| check_bt_structure.py | Check BT_Enemy structure and decorator setup | log, check_node_recursive, main | 2026-01-26 | 2026-03-22 |
| check_bt_task_parents.py | check_bt_task_parents.py | log, check_parents | 2026-01-26 | 2026-03-22 |
| check_bt_task_reparent.py | check_bt_task_reparent.py | log, check_reparent | 2026-01-26 | 2026-03-22 |
| check_bts_parent.py | check_bts_parent.py | main | 2026-01-26 | 2026-03-22 |
| check_button_cdo.py | (no description) |  | 2026-01-26 | 2026-03-22 |
| check_button_complete.py | (no description) |  | 2026-01-26 | 2026-03-22 |
| check_button_parent.py | (no description) |  | 2026-01-26 | 2026-03-22 |
| check_camera_settingtag.py | Check if Camera Settings entries have their SettingTag set. | check_camera_settings | 2026-01-26 | 2026-03-22 |
| check_category_data.py | Check if InventoryCategoryData is preserved after migration | log, check_category_data | 2026-01-09 | 2026-03-22 |
| check_category_icons.py | Load W_Settings widget blueprint |  | 2026-01-26 | 2026-03-22 |
| check_category_parent.py | Check parent class of W_Settings_CategoryEntry |  | 2026-03-09 | 2026-03-22 |
| check_cdo_transforms.py | check_cdo_transforms.py | log, main | 2026-01-26 | 2026-03-22 |
| check_centered_text.py | Verify W_Settings_CenteredText is reparented to C++ | main | 2026-03-09 | 2026-03-22 |
| check_chaos_forcefield_state.py | Check B_Chaos_ForceField Blueprint state using C++ validation. | main | 2026-01-26 | 2026-03-22 |
| check_chaos_interfaces.py | Check what interfaces B_Chaos_ForceField implements. | main | 2026-01-26 | 2026-03-22 |
| check_character_mesh.py | check_character_mesh.py | log, main | 2026-01-26 | 2026-03-22 |
| check_character_parent.py | Check the parent class of B_Soulslike_Character | main | 2026-01-26 | 2026-03-22 |
| check_character_parent_only.py | CHECK ONLY - Does not modify anything. | log, main | 2026-01-26 | 2026-03-22 |
| check_chest_tier_table.py | check_chest_tier_table.py | main | 2026-01-26 | 2026-03-22 |
| check_class_exists.py | Check if AB_SequenceActor class exists and can be loaded. | log, main | 2026-03-09 | 2026-03-22 |
| check_compile_errors.py | Check Blueprint Compilation Errors | check_blueprint_compile_errors | 2026-01-26 | 2026-03-22 |
| check_container_blueprint_loot.py | check_container_blueprint_loot.py | main | 2026-01-26 | 2026-03-22 |
| check_container_cdo.py | check_container_cdo.py | check_container | 2026-01-26 | 2026-03-22 |
| check_container_components.py | check_container_components.py | main | 2026-01-26 | 2026-03-22 |
| check_container_loot_table.py | check_container_loot_table.py | main | 2026-01-26 | 2026-03-22 |
| check_container_parent.py | check_container_parent.py | walk_bp_hierarchy, main | 2026-01-26 | 2026-03-22 |
| check_container_properties.py | check_container_properties.py | check_container | 2026-01-26 | 2026-03-22 |
| check_container_scs.py | check_container_scs.py | main | 2026-01-26 | 2026-03-22 |
| check_copy9.py | check_copy9.py | main | 2026-01-26 | 2026-03-22 |
| check_copy9_container.py | check_copy9_container.py | main | 2026-01-26 | 2026-03-22 |
| check_core_fixes.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_da_to_bp.py | check_da_to_bp.py | log, main | 2026-01-26 | 2026-03-22 |
| check_daynight_bponly.py | Check PDA_DayNight entries in bp_only |  | 2026-01-26 | 2026-03-22 |
| check_death_currency.py | Check B_DeathCurrency stat defaults in bp_only |  | 2026-01-26 | 2026-03-22 |
| check_deathcurrency_parent.py | Check and fix B_DeathCurrency parent class. | test | 2026-01-26 | 2026-03-22 |
| check_default_mesh_data.py | check_default_mesh_data.py | log, main | 2026-01-26 | 2026-03-22 |
| check_demo_level.py | Check demo level status | run | 2026-01-07 | 2026-03-22 |
| check_demo_room.py | Check B_DemoRoom and level loading | run | 2026-01-07 | 2026-03-22 |
| check_destructible_parent.py | Check B_Destructible's actual parent class to ensure it's reparented to C++. | check_parent | 2026-01-26 | 2026-03-22 |
| check_destructible_physics.py | Check B_Destructible physics settings to diagnose why barrels don't break. | check_destructible_physics | 2026-01-26 | 2026-03-22 |
| check_dialog_input.py | check_dialog_input.py | log | 2026-03-09 | 2026-03-22 |
| check_dialog_requirements.py | check_dialog_requirements.py | log | 2026-03-09 | 2026-03-22 |
| check_dialog_widget_parent.py | Check if W_Dialog widget is reparented to C++ class. |  | 2026-03-09 | 2026-03-22 |
| check_dialog_widget_parent_v2.py | Check if W_Dialog widget is reparented to C++ class - simple version. |  | 2026-03-09 | 2026-03-22 |
| check_dodge_data.py | (no description) |  | 2026-01-07 | 2026-03-22 |
| check_dodge_struct.py | check_dodge_struct.py |  | 2026-01-07 | 2026-03-22 |
| check_door_class_hierarchy.py | Check the actual class hierarchy of boss doors in the level. | check_door_hierarchy | 2026-01-26 | 2026-03-22 |
| check_door_demo_parent.py | Check B_Door_Demo parent class. |  | 2026-01-26 | 2026-03-22 |
| check_door_demo_scs.py | Check B_Door_Demo SCS (SimpleConstructionScript) for component definitions. |  | 2026-01-26 | 2026-03-22 |
| check_enemy_abilities.py | check_enemy_abilities.py | log, check_enemy, main | 2026-01-26 | 2026-03-22 |
| check_enemy_bt.py | Check what BehaviorTree is assigned to enemy Blueprints | log, check_enemy, main | 2026-01-26 | 2026-03-22 |
| check_enemy_components.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_enemy_equipment.py | Check what equipment/weapons enemies have that would apply status effects. | log | 2026-03-09 | 2026-03-22 |
| check_enemy_weapon_status.py | Check what weapons enemies use and if they have status effects configured. | log | 2026-03-09 | 2026-03-22 |
| check_enemy_weapons.py | Check enemy weapon ChildActorClass and skeleton mesh assignments | check_enemy_weapon, main | 2026-01-26 | 2026-03-22 |
| check_entry_parent_v2.py | Check W_Settings_Entry parent class hierarchy | main | 2026-03-09 | 2026-03-22 |
| check_enum_types.py | Check available enum types and their values | log, check_enum_types | 2026-01-09 | 2026-03-22 |
| check_enum_values.py | Check E_AI_States enum values in the backup project | log, main | 2026-01-26 | 2026-03-22 |
| check_execution_widget.py | Check ExecutionWidget component in character hierarchy. | check_execution_widget | 2026-01-26 | 2026-03-22 |
| check_flask_data.py | check_flask_data.py | check_flask_data | 2026-03-09 | 2026-03-22 |
| check_funcs.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| check_gc_barrel_settings.py | Check GC_Barrel GeometryCollection asset physics settings. | check_gc_barrel | 2026-01-26 | 2026-03-22 |
| check_genericbutton_parent.py | (no description) |  | 2026-03-09 | 2026-03-22 |
| check_greatsword_status.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_ik_rig.py | Check IK Rig configuration for hand IK | log, check_ik_rig, main | 2026-01-26 | 2026-03-22 |
| check_imc.py | Check IMC_Gameplay mappings |  | 2026-01-07 | 2026-03-22 |
| check_imc_dialog.py | Check what input actions are mapped in IMC_Dialog. | check_imc_dialog | 2026-03-09 | 2026-03-22 |
| check_interactable_components.py | check_interactable_components.py | check_blueprint, main | 2026-01-26 | 2026-03-22 |
| check_interactable_parent.py | check_interactable_parent.py | check_parent | 2026-01-26 | 2026-03-22 |
| check_interactable_scs.py | Check B_Interactable SCS components |  | 2026-01-26 | 2026-03-22 |
| check_inventory_items.py | Check what items are in the player's inventory. | check_inventory_items | 2026-01-26 | 2026-03-22 |
| check_item_classes.py | check_item_classes.py | log | 2026-01-26 | 2026-03-22 |
| check_item_data.py | Check PDA_Item data to see if ItemInformation has valid icons and descriptions | log, check_item_data | 2026-01-09 | 2026-03-22 |
| check_item_tags.py | Check what item tags exist in the project | log | 2026-03-09 | 2026-03-22 |
| check_katana_stats.py | Check DA_Katana stats from bp_only project | check_katana_stats | 2026-01-26 | 2026-03-22 |
| check_ladder_meshes.py | Check B_Ladder ISM component mesh assignments | check_ladder_meshes | 2026-01-26 | 2026-03-22 |
| check_level_actors.py | Load the demo level |  | 2026-03-09 | 2026-03-22 |
| check_level_bp.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_level_lights.py | Check for directional lights in L_Demo_Showcase level | log, main | 2026-01-26 | 2026-03-22 |
| check_levelup_categories.py | Check the Category tag settings on W_StatBlock_LevelUp widgets in W_LevelUp. | test | 2026-01-26 | 2026-03-22 |
| check_loot_manager_parent.py | check_loot_manager_parent.py | main | 2026-01-26 | 2026-03-22 |
| check_loot_manager_parent_v2.py | check_loot_manager_parent_v2.py | main | 2026-01-26 | 2026-03-22 |
| check_loot_manager_props.py | check_loot_manager_props.py | main | 2026-01-26 | 2026-03-22 |
| check_loot_manager_reparent.py | check_loot_manager_reparent.py | main | 2026-01-26 | 2026-03-22 |
| check_mesh_data_detailed.py | check_mesh_data_detailed.py | log, check_asset_properties, main | 2026-01-26 | 2026-03-22 |
| check_mesh_inheritance.py | check_mesh_inheritance.py | log, print_class_hierarchy, main | 2026-01-26 | 2026-03-22 |
| check_mesh_simple.py | check_mesh_simple.py | log, main | 2026-01-26 | 2026-03-22 |
| check_mesh_sockets.py | Check sockets on character skeletal meshes at runtime | log, check_socket_on_mesh, spawn_and_check_runtime, main | 2026-01-26 | 2026-03-22 |
| check_montage_notifies.py | check_montage_notifies.py | main | 2026-01-26 | 2026-03-22 |
| check_niagara_assets.py | Check Niagara assets in B_RestingPoint by spawning instance |  | 2026-01-26 | 2026-03-22 |
| check_niagara_detail.py | Check Niagara component details in B_RestingPoint SCS |  | 2026-01-26 | 2026-03-22 |
| check_niagara_effect.py | Load the Blueprint asset | check_resting_point_niagara | 2026-01-26 | 2026-03-22 |
| check_notify_parent.py | (no description) |  | 2026-01-26 | 2026-03-22 |
| check_notify_status.py | Check notify Blueprint status WITHOUT modifying anything |  | 2026-01-26 | 2026-03-22 |
| check_npc_compile.py | Check compile errors on NPC system Blueprints. | main | 2026-03-09 | 2026-03-22 |
| check_npc_components.py | Check NPC Blueprint components to verify AC_AI_InteractionManager exists. | check_npc_components | 2026-03-09 | 2026-03-22 |
| check_npc_parent.py | Check actual parent class of NPC Blueprints using export_text. | check_parent_classes | 2026-03-09 | 2026-03-22 |
| check_overlay_blueprint_vars.py | Check overlay state Blueprint variables vs C++ properties | log, main | 2026-03-09 | 2026-03-22 |
| check_parent.py | (no description) | log | 2026-01-07 | 2026-03-22 |
| check_parent_classes.py | check_parent_classes.py | main | 2026-01-26 | 2026-03-22 |
| check_parent_components.py | Check component hierarchy across parent Blueprints | log, check_bp, run | 2026-01-03 | 2026-03-22 |
| check_parent_path.py | check_parent_path.py |  | 2026-01-07 | 2026-03-22 |
| check_parent_quick.py | Quick parent class check after migration |  | 2026-01-03 | 2026-03-22 |
| check_parent_simple.py | check_parent_simple.py | main | 2026-01-26 | 2026-03-22 |
| check_pc_blueprint.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_pc_components.py | Get parent via cast |  | 2026-03-09 | 2026-03-22 |
| check_pc_cutscene.py | Check PC_SoulslikeFramework and cutscene setup | log, main | 2026-03-09 | 2026-03-22 |
| check_pc_parent.py | (no description) |  | 2026-03-09 | 2026-03-22 |
| check_pda_classes.py | check_pda_classes.py |  | 2026-01-07 | 2026-03-22 |
| check_pda_parent.py | Check PDA_Item Blueprint parent class. | check_pda_parent | 2026-01-26 | 2026-03-22 |
| check_pda_reparent.py | Check PDA_DayNight parent class |  | 2026-01-26 | 2026-03-22 |
| check_pickup_action.py | Check the pickup action data asset | log | 2026-03-09 | 2026-03-22 |
| check_pickup_collision.py | Check B_PickupItem collision settings in the backup project | check_pickup_collision | 2026-01-26 | 2026-03-22 |
| check_pickup_collision_settings.py | Check pickup item collision settings. | check_pickup_collision | 2026-01-26 | 2026-03-22 |
| check_pickup_parent.py | Check parent class of B_Action_PickupItemMontage |  | 2026-03-09 | 2026-03-22 |
| check_pickup_simple.py | Simple check of B_PickupItem collision |  | 2026-01-26 | 2026-03-22 |
| check_playercard_parent.py | (no description) |  | 2026-03-09 | 2026-03-22 |
| check_poise_asset.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_poise_persist.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_property_bindings.py | Check AnimBP property bindings and variable names. | check_animbp_bindings | 2026-01-26 | 2026-03-22 |
| check_reparent_status.py | check_reparent_status.py |  | 2026-01-07 | 2026-03-22 |
| check_restingpoint_components.py | Check B_RestingPoint component structure |  | 2026-01-26 | 2026-03-22 |
| check_restingpoint_current.py | (no description) | check_restingpoint | 2026-01-26 | 2026-03-22 |
| check_restingpoint_niagara.py | Check B_RestingPoint Niagara component assets |  | 2026-01-26 | 2026-03-22 |
| check_scs_templates.py | Check SCS component templates directly in both projects | log, check_scs_templates | 2026-01-26 | 2026-03-22 |
| check_scs_transform.py | check_scs_transform.py | log, main | 2026-01-26 | 2026-03-22 |
| check_sequence_actor_final.py | (no description) |  | 2026-03-09 | 2026-03-22 |
| check_sequence_actor_final2.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_sequence_actor_parent.py | Get parent class |  | 2026-03-09 | 2026-03-22 |
| check_sequenceactor_state.py | Check B_SequenceActor Blueprint state - does it have EventGraph logic? | log, main | 2026-03-09 | 2026-03-22 |
| check_settings_compile.py | Check W_Settings compile errors. | check_compile, main | 2026-01-26 | 2026-03-22 |
| check_settings_entry_parent.py | Check W_Settings_Entry parent class. | check_parent, main | 2026-01-26 | 2026-03-22 |
| check_settings_entry_reparent.py | Check if W_Settings_Entry is properly reparented to C++. | check_blueprint_reparenting, main | 2026-01-26 | 2026-03-22 |
| check_settings_parent.py | (no description) |  | 2026-01-26 | 2026-03-22 |
| check_settings_parent_final.py | (no description) | get_class_hierarchy | 2026-03-09 | 2026-03-22 |
| check_settings_parent_v2.py | (no description) |  | 2026-03-09 | 2026-03-22 |
| check_settings_parent_v3.py | (no description) |  | 2026-03-09 | 2026-03-22 |
| check_settings_parents.py | Check parent classes of W_Settings_Entry and W_Settings_CategoryEntry. | check_parent, main | 2026-03-09 | 2026-03-22 |
| check_shield_data.py | Check shield item data - EquipSlots and AttachmentSockets | main | 2026-01-26 | 2026-03-22 |
| check_shield_props.py | Check shield item properties - list all available properties | main | 2026-01-26 | 2026-03-22 |
| check_single_item.py | Simple item class check | log | 2026-03-09 | 2026-03-22 |
| check_skeleton_sockets.py | check_skeleton_sockets.py | log, main | 2026-01-26 | 2026-03-22 |
| check_skymanager.py | Check for SkyManager in the level and verify PDA_DayNight data |  | 2026-01-26 | 2026-03-22 |
| check_skymanager_components.py | Check if B_SkyManager has SCS components after migration | main | 2026-01-26 | 2026-03-22 |
| check_skymanager_components_v2.py | Check B_SkyManager components using alternative methods | main | 2026-01-26 | 2026-03-22 |
| check_skymanager_in_level.py | Check B_SkyManager instance in the demo level | main | 2026-01-26 | 2026-03-22 |
| check_skymanager_instance.py | Check B_SkyManager instance in level for mesh/material data | log, main | 2026-01-26 | 2026-03-22 |
| check_skymanager_level.py | Check B_SkyManager instance in the level | main | 2026-01-26 | 2026-03-22 |
| check_skymanager_scs.py | Check B_SkyManager SCS components in detail | main | 2026-01-26 | 2026-03-22 |
| check_skysphere_transform.py | Check sky sphere transform and light priority settings | log, check_level, main | 2026-01-26 | 2026-03-22 |
| check_slf_classes.py | check_slf_classes.py |  | 2026-01-07 | 2026-03-22 |
| check_stat_derivation.py | Check if primary stats have StatsToAffect configured for derivation. | check_stat_derivation | 2026-01-26 | 2026-03-22 |
| check_stat_hierarchy.py | check_stat_hierarchy.py | check_stat_hierarchy | 2026-01-26 | 2026-03-22 |
| check_stat_table.py | check_stat_table.py | check_stat_table | 2026-01-26 | 2026-03-22 |
| check_status_effect_data.py | Check status effect data assets to verify icons and configurations. | check_status_effects | 2026-03-09 | 2026-03-22 |
| check_statuseffect_blueprint.py | Check if B_StatusEffectArea Blueprint is properly reparented to C++ class. | log, check_blueprint | 2026-01-26 | 2026-03-22 |
| check_statuseffect_rankinfo.py | Check RankInfo configuration on status effect data assets. | log, check_rankinfo | 2026-01-26 | 2026-03-22 |
| check_statuseffect_statchanges.py | Check what stat change data is configured on status effect data assets. | log, check_statchanges | 2026-01-26 | 2026-03-22 |
| check_statuseffect_triggeredtext.py | Check TriggeredText on status effect data assets in both projects. | log | 2026-03-09 | 2026-03-22 |
| check_struct_registration.py | Check how UE5 actually registers struct names. |  | 2026-01-07 | 2026-03-22 |
| check_struct_types.py | Check what struct types are available in Python for status effects. | log, check_structs | 2026-01-26 | 2026-03-22 |
| check_subtrees.py | Check which subtrees contain ToggleFocus task | log, main | 2026-01-26 | 2026-03-22 |
| check_template_meshes.py | Check mesh assignments on SCS component templates | log, check_template_meshes | 2026-01-26 | 2026-03-22 |
| check_throwing_knife.py | check_throwing_knife.py | log | 2026-03-09 | 2026-03-22 |
| check_throwingknife.py | Check throwing knife data asset for category and subcategory. | check_throwing_knife | 2026-03-09 | 2026-03-22 |
| check_upda_item.py | check_upda_item.py |  | 2026-01-07 | 2026-03-22 |
| check_vendor_components.py | Check Vendor Blueprint components - look for Head, Body, Arms, Legs mesh parts. | check_vendor | 2026-03-09 | 2026-03-22 |
| check_vendor_npc.py | Check Vendor NPC - Verify VendorAsset is set on AIInteractionManager component | log, check_vendor_npc | 2026-03-09 | 2026-03-22 |
| check_vendor_parents.py | Check parent classes of vendor widgets. |  | 2026-03-09 | 2026-03-22 |
| check_venom_sword_statuseffect.py | Check if venom sword has WeaponStatusEffectInfo configured. | log, check | 2026-01-26 | 2026-03-22 |
| check_w_resources.py | Check W_Resources parent and properties |  | 2026-01-26 | 2026-03-22 |
| check_weapon_ability_montage.py | Check if DA_ExampleWeaponAbility has a montage assigned. | check_weapon_ability | 2026-03-09 | 2026-03-22 |
| check_weapon_blueprint_scs.py | check_weapon_blueprint_scs.py | log, main | 2026-01-26 | 2026-03-22 |
| check_weapon_categories.py | Check weapon item categories to diagnose why weapons aren't showing in equipment | check_weapon_categories | 2026-01-26 | 2026-03-22 |
| check_weapon_cdo.py | Check weapon Blueprint CDO values for mesh transforms | log, check_weapons | 2026-01-26 | 2026-03-22 |
| check_weapon_components.py | Check weapon Blueprints for components |  | 2026-01-03 | 2026-03-22 |
| check_weapon_equip_slots.py | Check weapon EquipSlots tags to diagnose equipment slot matching. | check_weapon_equip_slots | 2026-01-26 | 2026-03-22 |
| check_weapon_hierarchy.py | check_weapon_hierarchy.py | log, check_hierarchy | 2026-01-26 | 2026-03-22 |
| check_weapon_inheritance.py | check_weapon_inheritance.py | log, check_inheritance | 2026-01-26 | 2026-03-22 |
| check_weapon_instance.py | Check spawned weapon instance component transforms | log, check_weapons | 2026-01-26 | 2026-03-22 |
| check_weapon_overlay_tags.py | check_weapon_overlay_tags.py | log, main | 2026-01-26 | 2026-03-22 |
| check_weapon_parents.py | check_weapon_parents.py | log, check_parents | 2026-01-26 | 2026-03-22 |
| check_weapon_scs_details.py | Extract detailed StaticMeshComponent settings from weapon Blueprints | log, check_weapons | 2026-01-26 | 2026-03-22 |
| check_weapon_sockets.py | check_weapon_sockets.py | log, main | 2026-01-26 | 2026-03-22 |
| check_weapon_stats_bponly.py | check_weapon_stats_bponly.py | check_weapons | 2026-01-26 | 2026-03-22 |
| check_weapon_stats_v2.py | check_weapon_stats_v2.py | check_weapons | 2026-01-26 | 2026-03-22 |
| check_weapon_stats_v3.py | check_weapon_stats_v3.py | check_weapons | 2026-01-26 | 2026-03-22 |
| check_weapon_status_data.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_weapon_status_effects.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_weapon_statuseffects.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_weapon_statuseffects_current.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| check_weapon_weight.py | check_weapon_weight.py | check_weapons | 2026-01-26 | 2026-03-22 |
| check_weight_stat.py | check_weight_stat.py | check_weight_stat | 2026-01-26 | 2026-03-22 |
| check_widget_parent.py | Check parent class of settings widgets. | main | 2026-01-26 | 2026-03-22 |
| check_widget_parent_v2.py | Check widget parent class using load_blueprint_class. | check_parent, main | 2026-01-26 | 2026-03-22 |
| check_widget_parents.py | check_widget_parents.py | check_widget_parents | 2026-01-26 | 2026-03-22 |
| compare_animbp_complete.py | (no description) |  | 2026-01-26 | 2026-03-22 |
| compare_boss_door.py | Compare Boss Door configuration between bp_only and SLFConversion. | compare_boss_door | 2026-01-26 | 2026-03-22 |
| compare_both_doors.py | Compare B_Door and B_BossDoor SCS and CDO components to understand | get_bp_info | 2026-01-26 | 2026-03-22 |
| compare_bp_cpp.py | (no description) | normalize_name, parse_json_blueprint, parse_cpp_header, compare | 2026-01-09 | 2026-03-22 |
| compare_components.py | Compare components between original and migrated B_PickupItem | check_blueprint | 2026-01-07 | 2026-03-22 |
| compare_container_both.py | compare_container_both.py | main | 2026-01-26 | 2026-03-22 |
| compare_destructible_detailed.py | Detailed comparison of B_Destructible between SLFConversion and bp_only. | compare_destructible | 2026-01-26 | 2026-03-22 |
| compare_destructible_scs.py | Compare B_Destructible SCS component settings between bp_only and SLFConversion. | compare_destructible | 2026-01-26 | 2026-03-22 |
| compare_dialog_systems.py | Compare Dialog Systems - Extract dialog data for comparison between projects. | compare_dialog_systems | 2026-03-09 | 2026-03-22 |
| compare_door_blueprints_deep.py | Deep comparison of B_Door vs B_BossDoor to understand why B_BossDoor inherits C+ | analyze_blueprint | 2026-01-26 | 2026-03-22 |
| compare_door_blueprints_to_file.py | Deep comparison of B_Door vs B_BossDoor - saves results to JSON file | analyze_blueprint | 2026-01-26 | 2026-03-22 |
| compare_door_components.py | Compare B_Door components between bp_only and SLFConversion projects | export_door_data | 2026-01-26 | 2026-03-22 |
| compare_door_demo_to_bponly.py | Check B_Door_Demo in bp_only to see what it's supposed to look like. |  | 2026-01-26 | 2026-03-22 |
| compare_door_to_bponly.py | Compare B_Door between SLFConversion and bp_only to find mesh/component differen | extract_door_data | 2026-01-26 | 2026-03-22 |
| compare_door_transforms.py | Compare B_Door_Demo component transforms between bp_only and SLFConversion. |  | 2026-01-26 | 2026-03-22 |
| compare_door_transforms_bponly.py | Compare B_Door_Demo component transforms - bp_only version |  | 2026-01-26 | 2026-03-22 |
| compare_doors_fixed.py | Compare B_Door between SLFConversion - extract full component and mesh data. | extract_door_data | 2026-01-26 | 2026-03-22 |
| compare_doors_simple.py | Simple comparison of B_Door mesh data. |  | 2026-01-26 | 2026-03-22 |
| compare_enemy_weapons.py | Compare enemy weapon configurations between bp_only and SLFConversion. | get_component_details, export_enemy_config, main | 2026-01-26 | 2026-03-22 |
| compare_guard_vs_demo.py | compare_guard_vs_demo.py | log, get_component_details, extract_enemy_weapon_setup | 2026-01-26 | 2026-03-22 |
| compare_hand_r_socket.py | compare_hand_r_socket.py | log, main | 2026-01-26 | 2026-03-22 |
| compare_ik_setup.py | Compare IK setup between SLFConversion and bp_only AnimBP assets. | export_asset_text, search_text_for_patterns, main | 2026-01-26 | 2026-03-22 |
| compare_niagara_bponly.py | Compare Niagara settings between bp_only and SLFConversion B_RestingPoint |  | 2026-01-26 | 2026-03-22 |
| compare_npc_blueprints.py | Compare NPC Blueprints between bp_only and SLFConversion. | ensure_dir, export_npc_data, main | 2026-03-09 | 2026-03-22 |
| compare_overlay_anims.py | Compare overlay animation sequences between projects | log, get_anim_instance_info, check_animbp_directly, check_overlay_animations_... | 2026-01-26 | 2026-03-22 |
| compare_rest_system.py | Compare Rest System assets between bp_only and SLFConversion | export_actor_details, export_data_asset_details, export_widget_details, main | 2026-01-26 | 2026-03-22 |
| compare_restingpoint_full.py | (no description) | export_restingpoint_full | 2026-01-26 | 2026-03-22 |
| compare_settings_bponly.py | Export W_Settings related blueprints from bp_only for comparison | export_widget_info, main | 2026-03-09 | 2026-03-22 |
| compare_settings_tabs.py | Compare W_Settings tabs between bp_only and current implementation. | get_widget_children_by_type, extract_settings_structure, main | 2026-01-26 | 2026-03-22 |
| compare_settings_widgets.py | Comprehensive comparison of W_Settings and W_Settings_CategoryEntry between bp_o | safe_str, get_class_hierarchy, analyze_category_entry_blueprint, create_runti... | 2026-01-26 | 2026-03-22 |
| compare_skymanager_both.py | Compare B_SkyManager between bp_only and SLFConversion | get_scs_components, get_cdo_properties, main | 2026-01-26 | 2026-03-22 |
| compare_skymanager_complete.py | COMPLETE comparison of B_SkyManager between bp_only and SLFConversion | log, get_all_properties, check_actor_instance, main | 2026-01-26 | 2026-03-22 |
| compare_skymanager_full.py | THOROUGH comparison of B_SkyManager between bp_only and SLFConversion | get_all_properties, get_scs_components, analyze_skymanager, main | 2026-01-26 | 2026-03-22 |
| compare_skymanager_mesh.py | Compare B_SkyManager sky sphere mesh between SLFConversion and bp_only | log, check_skymanager_components, main | 2026-01-26 | 2026-03-22 |
| compare_skymanager_scs.py | Compare B_SkyManager SCS between bp_only and SLFConversion | main | 2026-01-26 | 2026-03-22 |
| compare_skymanager_v2.py | Compare B_SkyManager - comprehensive check | main | 2026-01-26 | 2026-03-22 |
| compare_socket_positions.py | compare_socket_positions.py | log, main | 2026-01-26 | 2026-03-22 |
| compare_w_resources_binary.py | Compare W_Resources widget dimensions between projects |  | 2026-01-26 | 2026-03-22 |
| compare_weapon_components.py | compare_weapon_components.py | log, compare | 2026-01-26 | 2026-03-22 |
| compare_weapon_configs.py | compare_weapon_configs.py | log, get_rotator_dict, get_vector_dict, export_component_data, export_weapon_... | 2026-01-26 | 2026-03-22 |
| compare_weapon_positions.py | compare_weapon_positions.py | log, get_vector_dict, get_rotator_dict, test_weapon, main | 2026-01-26 | 2026-03-22 |
| compare_weapon_scs.py | compare_weapon_scs.py | log, get_vector_dict, get_rotator_dict, main | 2026-01-26 | 2026-03-22 |
| compare_weapon_transforms.py | compare_weapon_transforms.py | log, get_weapon_info, main | 2026-01-26 | 2026-03-22 |
| compare_weapon_transforms_live.py | compare_weapon_transforms_live.py | log, get_mesh_transform, main | 2026-01-26 | 2026-03-22 |
| pie_test.py | PIE Test Script - Verifies camera and movement for B_Soulslike_Character | log, run_pie_test | 2026-01-03 | 2026-03-22 |
| run_pie_test.py | Get the editor subsystem | run_pie_test | 2026-01-03 | 2026-03-22 |
| test_action_cast.py | test_action_cast.py |  | 2026-01-07 | 2026-03-22 |
| test_action_comprehensive.py | Comprehensive Action System Test | write_log, test_comprehensive | 2026-01-03 | 2026-03-22 |
| test_action_direct.py | Direct Action Manager Test - Test the Blueprint class loading | write_log, test_action_lookup | 2026-01-03 | 2026-03-22 |
| test_action_manager.py | test_action_manager.py | test_action_manager | 2026-01-03 | 2026-03-22 |
| test_action_manager_v2.py | test_action_manager_v2.py | test_action_manager | 2026-01-03 | 2026-03-22 |
| test_action_setup.py | Action System Setup Verification Test | log, run_test | 2026-01-03 | 2026-03-22 |
| test_action_system.py | Test Action System - Verify AvailableActions map stores UClass* correctly | test_action_manager | 2026-01-03 | 2026-03-22 |
| test_ai_diagnostic.py | test_ai_diagnostic.py | log, main | 2026-01-26 | 2026-03-22 |
| test_animbp_native.py | test_animbp_native.py | log, main | 2026-01-10 | 2026-03-22 |
| test_animbp_pie.py | test_animbp_pie.py | log, main | 2026-01-10 | 2026-03-22 |
| test_automation.py | test_automation.py - Test if SLFAutomationLibrary is accessible |  | 2026-01-26 | 2026-03-22 |
| test_automation2.py | test_automation2.py - Test with UE logging |  | 2026-01-26 | 2026-03-22 |
| test_automation_binding.py | Test if SLFAutomationLibrary functions are available. |  | 2026-03-09 | 2026-03-22 |
| test_automation_lib.py | Test if SLFAutomationLibrary functions are available |  | 2026-01-26 | 2026-03-22 |
| test_automation_lib_v2.py | Test if SLFAutomationLibrary functions are available - write to file |  | 2026-01-26 | 2026-03-22 |
| test_backup_extraction.py | Test extracting struct data from BACKUP assets (before any C++ migration). | log | 2026-01-07 | 2026-03-22 |
| test_boss_animbp.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| test_cast_simulation.py | test_cast_simulation.py |  | 2026-01-07 | 2026-03-22 |
| test_category_icons.py | Test loading the category entry textures |  | 2026-01-26 | 2026-03-22 |
| test_character_functions.py | Headless functional test for SoulslikeFramework character systems. | log, test_character_spawn, test_component_managers, test_action_base, test_bu... | 2026-01-03 | 2026-03-22 |
| test_class.py | (no description) |  | 2026-01-26 | 2026-03-22 |
| test_clear_function.py | test_clear_function.py | run | 2026-01-03 | 2026-03-22 |
| test_compile_character.py | test_compile_character.py | run | 2026-01-03 | 2026-03-22 |
| test_component_cache.py | Simple test - just load the editor and print component info |  | 2026-01-03 | 2026-03-22 |
| test_cpp_access.py | test_cpp_access.py |  | 2026-01-07 | 2026-03-22 |
| test_demo_full.py | Full demo level test | run | 2026-01-07 | 2026-03-22 |
| test_dialog_data_load.py | Test that DA_ExampleDialog loads with correct Requirement data | test_dialog_data | 2026-03-09 | 2026-03-22 |
| test_dodge_action.py | test_dodge_action.py |  | 2026-01-07 | 2026-03-22 |
| test_dodge_save.py | Step 1: Load and check initial state |  | 2026-01-07 | 2026-03-22 |
| test_door_onconstruction.py | Test that B_Door OnConstruction properly applies the DefaultDoorMesh. | check_door_cdo | 2026-01-26 | 2026-03-22 |
| test_door_spawn.py | Test spawning B_Door and verifying mesh is applied in BeginPlay | test_spawn_door | 2026-01-26 | 2026-03-22 |
| test_enemy_anim.py | test_enemy_anim.py | log, main | 2026-01-26 | 2026-03-22 |
| test_enemy_system.py | Enemy System Test - Verify enemy Blueprints and components load correctly | write_log, test_enemy_system | 2026-01-03 | 2026-03-22 |
| test_equipment_filter.py | Test Equipment Filter Logic | get_item_category_info, test_slot_matching, main | 2026-01-26 | 2026-03-22 |
| test_execution_widget.py | Test ExecutionWidget on spawned enemy. | test_execution_widget | 2026-01-26 | 2026-03-22 |
| test_export_import.py | (no description) |  | 2026-01-07 | 2026-03-22 |
| test_gameplay_systems.py | Headless functional test for SoulslikeFramework gameplay systems. | log, test_character_class, test_component_managers, test_action_system, test_... | 2026-01-03 | 2026-03-22 |
| test_input_simulation.py | PIE Input Simulation Test - Automated | run_pie_input_test | 2026-01-03 | 2026-03-22 |
| test_interface_impl.py | test_interface_impl.py | main | 2026-01-26 | 2026-03-22 |
| test_iterate.py | test_iterate.py | run | 2026-01-03 | 2026-03-22 |
| test_mesh_data.py | Test if mesh data is accessible after redirect fix. | log | 2026-01-07 | 2026-03-22 |
| test_npc_dialog.py | Test NPC dialog system in PIE. | test_dialog_datatable | 2026-03-09 | 2026-03-22 |
| test_pickup_visibility.py | Test pickup item visibility - verify Niagara component is active | test_pickup | 2026-01-07 | 2026-03-22 |
| test_pie.py | PIE Test Script - Verify Character Spawns Correctly | run | 2026-01-03 | 2026-03-22 |
| test_pie_actions.py | PIE Test for Action System | run_pie_test | 2026-01-03 | 2026-03-22 |
| test_recreate_switch.py | test_recreate_switch.py |  | 2026-01-03 | 2026-03-22 |
| test_rest_point.py | Test script to verify resting point configuration after migration | test_resting_point | 2026-01-26 | 2026-03-22 |
| test_set_struct.py | Test setting struct properties with correct types | log, test_set_struct | 2026-01-09 | 2026-03-22 |
| test_settings_cast.py | Test that W_Settings can Cast children to UW_Settings_CategoryEntry. | main | 2026-01-26 | 2026-03-22 |
| test_settings_click_debug.py | Test W_Settings tab clicking - diagnose Cast and button binding issues. | check_blueprint_parent, check_widget_tree, main | 2026-01-26 | 2026-03-22 |
| test_settings_compile_fresh.py | Fresh test of W_Settings compilation to verify DisplayName fixes. | main | 2026-01-26 | 2026-03-22 |
| test_settings_config.py | Test settings widget configuration. | test_settings_widgets | 2026-03-09 | 2026-03-22 |
| test_settings_construct.py | Test W_Settings construction by creating a widget instance. | main | 2026-03-09 | 2026-03-22 |
| test_settings_runtime.py | Create and show the W_Settings widget to trigger icon loading |  | 2026-01-26 | 2026-03-22 |
| test_sky_times.py | Test sky rotation at different times to verify the formula matches bp_only | log, main | 2026-01-26 | 2026-03-22 |
| test_soft_ref.py | (no description) |  | 2026-01-07 | 2026-03-22 |
| test_stat_categories.py | Test stat categories to verify proper grouping in level-up widget. | test | 2026-01-26 | 2026-03-22 |
| test_stat_derivation.py | Test stat derivation by checking the StatsToAffect configuration in C++ stat cla | test | 2026-01-26 | 2026-03-22 |
| test_stat_equipment.py | test_stat_equipment.py | log, main | 2026-01-10 | 2026-03-22 |
| test_statuseffect_component.py | Test that StatusEffectManager component exists on player character. | log, test | 2026-01-26 | 2026-03-22 |
| test_struct_migration.py | Test Option A: Extract struct data from backup, migrate, re-apply. |  | 2026-01-07 | 2026-03-22 |
| test_struct_names.py | Test to understand UE5 struct naming behavior. |  | 2026-01-07 | 2026-03-22 |
| test_twohanded_anim.py | Test that the AnimBP correctly loads TwoHanded animations from DA_ExampleAnimSet | log, main | 2026-03-09 | 2026-03-22 |
| test_twohanded_pose.py | Test two-handed pose after binding fix by checking overlay state at runtime | log, main | 2026-03-09 | 2026-03-22 |
| test_w_settings_compile.py | Test W_Settings Blueprint compilation after DisplayName fixes. | test_w_settings_compile, test_related_widgets | 2026-01-26 | 2026-03-22 |
| test_weapon_attach_direct.py | test_weapon_attach_direct.py | log, main | 2026-01-26 | 2026-03-22 |
| test_weapon_attachment.py | test_weapon_attachment.py | log, get_vector_dict, get_rotator_dict, main | 2026-01-26 | 2026-03-22 |
| test_weapon_components.py | test_weapon_components.py | log, test_weapon_components | 2026-01-09 | 2026-03-22 |
| test_weapon_equip_visual.py | test_weapon_equip_visual.py | log, main | 2026-01-26 | 2026-03-22 |
| test_weapon_offset.py | test_weapon_offset.py | log, main | 2026-01-26 | 2026-03-22 |
| validate_chaos_migration.py | Comprehensive validation of chaos destruction migration using C++ functions. | main | 2026-01-26 | 2026-03-22 |
| validate_ladder_components.py | Validate ladder components are preserved after migration. | validate_ladder_components | 2026-01-26 | 2026-03-22 |
| validate_ladder_cpp.py | Ladder System Validation using SLFAutomationLibrary (C++) | validate_ladder | 2026-01-26 | 2026-03-22 |
| validate_ladder_interface.py | Validate B_Ladder interface implementations and C++ base class setup. | validate_ladder_interface | 2026-01-26 | 2026-03-22 |
| validate_ladder_system.py | Comprehensive Ladder System Validation Script | get_blueprint_parent_class, get_blueprint_components, get_blueprint_functions... | 2026-01-26 | 2026-03-22 |
| validate_migration.py | (no description) | load_json_files, extract_variables, extract_functions, extract_dispatchers, l... | 2026-01-03 | 2026-03-22 |
| validate_migration_fixed.py | (no description) | load_json_files, extract_variables, extract_functions, extract_dispatchers, l... | 2026-01-07 | 2026-03-22 |
| validate_npc_compile.py | Validate NPC system Blueprint compilation status. | validate_npc_compile | 2026-03-09 | 2026-03-22 |
| validate_npc_restoration.py | Validate NPC System Restoration Against YAML Source of Truth | log_result, validate_data_asset, validate_dialog_asset, validate_datatable, v... | 2026-03-09 | 2026-03-22 |
| validate_settings_complete.py | Complete Settings Widget Validation | main | 2026-03-09 | 2026-03-22 |
| validate_settings_pie.py | PIE test to validate settings widgets show correct values. | main | 2026-03-09 | 2026-03-22 |
| validate_settings_simple.py | Simple Settings Widget Validation using C++ functions | main | 2026-03-09 | 2026-03-22 |
| verify_action_cast.py | verify_action_cast.py |  | 2026-01-07 | 2026-03-22 |
| verify_ai_vs_player_weapon.py | verify_ai_vs_player_weapon.py | log, compare | 2026-01-26 | 2026-03-22 |
| verify_all_fixes.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| verify_and_fix_assets.py | Verify B_SequenceActor parent and force re-clear ABP_SoulslikeBossNew with expli | log, main | 2026-03-09 | 2026-03-22 |
| verify_animbp_fix.py | verify_animbp_fix.py | log, main | 2026-01-26 | 2026-03-22 |
| verify_animbp_hierarchy.py | Verify AnimBP class hierarchy using C++. | log, main | 2026-01-26 | 2026-03-22 |
| verify_animbp_pins.py | Verify AnimBP pins - comprehensive check for: | log, main | 2026-01-26 | 2026-03-22 |
| verify_animbp_state.py | Verify AnimBP migration state - check for any remaining issues. | log, main | 2026-01-26 | 2026-03-22 |
| verify_animbps.py | verify_animbps.py | main | 2026-01-26 | 2026-03-22 |
| verify_base_stats.py | Verify BaseStats were applied to character class data assets. | verify | 2026-01-26 | 2026-03-22 |
| verify_base_stats_to_file.py | Verify BaseStats were applied to character class data assets. | verify | 2026-01-26 | 2026-03-22 |
| verify_boss_door_complete.py | Complete verification of B_BossDoor after migration | main | 2026-01-26 | 2026-03-22 |
| verify_boss_door_parent.py | Verify B_BossDoor parent class after migration | main | 2026-01-26 | 2026-03-22 |
| verify_both_doors.py | Verify both B_Door and B_BossDoor states after fix | check_door | 2026-01-26 | 2026-03-22 |
| verify_campfire_fix.py | Verify B_RestingPoint now has the campfire mesh |  | 2026-01-26 | 2026-03-22 |
| verify_cdo_mesh.py | Verify the CDO has DefaultInteractableMesh set correctly |  | 2026-01-26 | 2026-03-22 |
| verify_character_parent.py | Verify B_Soulslike_Character's current parent class. | log, main | 2026-01-26 | 2026-03-22 |
| verify_charinfo_reparent.py | Verify PDA_BaseCharacterInfo reparenting worked. |  | 2026-03-09 | 2026-03-22 |
| verify_compile.py | Get all Blueprint assets | run | 2026-01-03 | 2026-03-22 |
| verify_container.py | verify_container.py | verify_container | 2026-01-26 | 2026-03-22 |
| verify_container_settings.py | verify_container_settings.py | find_containers_in_level, verify_and_fix_container, main | 2026-01-26 | 2026-03-22 |
| verify_core_redirect.py | verify_core_redirect.py |  | 2026-01-07 | 2026-03-22 |
| verify_cutscene_setup.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| verify_data_migration.py | Check Dodge Montages |  | 2026-01-07 | 2026-03-22 |
| verify_destructible_physics.py | Verify B_Destructible physics is now working after the fix. | verify_destructible_physics | 2026-01-26 | 2026-03-22 |
| verify_destructibles.py | verify_destructibles.py | main | 2026-01-26 | 2026-03-22 |
| verify_dialog_content.py | Verify dialog content and GameplayEvents. |  | 2026-03-09 | 2026-03-22 |
| verify_dialog_fix.py | Verify the dialog system fix: | main | 2026-03-09 | 2026-03-22 |
| verify_dialog_migration.py | Verify dialog DataTables use C++ struct. |  | 2026-03-09 | 2026-03-22 |
| verify_dialog_requirements.py | verify_dialog_requirements.py | log | 2026-03-09 | 2026-03-22 |
| verify_door_softptr.py | Verify that DefaultDoorMesh TSoftObjectPtr was saved to B_Door CDO |  | 2026-01-26 | 2026-03-22 |
| verify_embedded_data.py | Verify embedded W_Settings_Entry data in W_Settings | main | 2026-03-09 | 2026-03-22 |
| verify_enemy_components.py | Verify enemy component hierarchy | log, check_enemy_components, main | 2026-01-26 | 2026-03-22 |
| verify_enum_migration.py | Verify that BlendListByEnum nodes were migrated to C++ enum. | main | 2026-01-26 | 2026-03-22 |
| verify_execution_tags.py | Verify execution tags were applied to DA_Execute_Generic data asset. | verify_execution_tags | 2026-01-26 | 2026-03-22 |
| verify_guard_sequences.py | verify_guard_sequences.py | log, verify_guards | 2026-01-26 | 2026-03-22 |
| verify_hud_setup.py | Verify HUD Setup for SLFPlayerController | log, verify_hud_setup | 2026-01-07 | 2026-03-22 |
| verify_input_mappings.py | verify_input_mappings.py | log | 2026-03-09 | 2026-03-22 |
| verify_interactables.py | verify_interactables.py | main | 2026-01-26 | 2026-03-22 |
| verify_item_class_link.py | verify_item_class_link.py | log, verify_links | 2026-01-26 | 2026-03-22 |
| verify_level_bp.py | Verify that the Level Blueprint for L_Demo_Showcase compiles. | log, main | 2026-03-09 | 2026-03-22 |
| verify_mesh_migration.py | Verify Option B Migration: Check if PDA_DefaultMeshData has mesh data | log | 2026-01-07 | 2026-03-22 |
| verify_mesh_migration_v2.py | Verify Option B Migration: Check if PDA_DefaultMeshData has mesh data | log | 2026-01-07 | 2026-03-22 |
| verify_migration.py | Verify migration results | log | 2026-01-07 | 2026-03-22 |
| verify_niagara_assets.py | Verify Niagara assets are assigned in B_RestingPoint |  | 2026-01-26 | 2026-03-22 |
| verify_npc_final.py | Verify all NPC Blueprints are ready for PIE testing. | main | 2026-03-09 | 2026-03-22 |
| verify_npc_migration.py | Verify NPC Blueprints are reparented to C++ classes. | verify_npcs | 2026-03-09 | 2026-03-22 |
| verify_post_migration.py | verify_post_migration.py | verify | 2026-01-07 | 2026-03-22 |
| verify_reparent.py | verify_reparent.py |  | 2026-01-07 | 2026-03-22 |
| verify_reparent_simple.py | verify_reparent_simple.py |  | 2026-01-07 | 2026-03-22 |
| verify_rest_menu_widgets.py | Verify W_RestMenu widget names between bp_only and SLFConversion | main | 2026-01-26 | 2026-03-22 |
| verify_restingpoint.py | verify_restingpoint.py | main | 2026-01-26 | 2026-03-22 |
| verify_sequence_actor.py | (no description) |  | 2026-03-09 | 2026-03-22 |
| verify_settings_compile.py | Verify Settings widgets compile without errors |  | 2026-03-09 | 2026-03-22 |
| verify_settings_cpp_props.py | Verify Settings widgets have C++ properties (proves reparent worked) |  | 2026-03-09 | 2026-03-22 |
| verify_settings_fix.py | Verify that settings embedded data was applied correctly. | main | 2026-01-26 | 2026-03-22 |
| verify_settings_migration.py | Verify all Settings widgets compile correctly after migration. |  | 2026-03-09 | 2026-03-22 |
| verify_settings_parents.py | Verify Settings widgets are properly reparented. | check_widget, main | 2026-01-26 | 2026-03-22 |
| verify_settings_reparent.py | Verify W_Settings widgets are reparented to C++. | get_parent_class_name, check_widget, main | 2026-01-26 | 2026-03-22 |
| verify_settings_widgets.py | Verify W_Settings widget bindings after C++ rebuild. | verify_w_settings_entry, verify_w_settings, verify_w_settings_category_entry,... | 2026-01-26 | 2026-03-22 |
| verify_skymanager_parent.py | Verify B_SkyManager is properly reparented to SLFSkyManager | main | 2026-01-26 | 2026-03-22 |
| verify_spells.py | (no description) | log | 2026-01-29 | 2026-03-22 |
| verify_stat_tags_runtime.py | Verify stat tags are being set correctly from C++ parent classes. | get_tag_string, test | 2026-01-26 | 2026-03-22 |
| verify_statuseffect_config.py | Verify B_StatusEffectArea configuration persisted after save. | log, verify | 2026-01-26 | 2026-03-22 |
| verify_sword01_cdo.py | verify_sword01_cdo.py | log, main | 2026-01-26 | 2026-03-22 |
| verify_values.py | verify_values.py | run | 2026-01-03 | 2026-03-22 |
| verify_vendor_parent.py | Verify Vendor NPC parent class - writes to file | log, verify_vendor | 2026-03-09 | 2026-03-22 |
| verify_weapon_cdos.py | verify_weapon_cdos.py | log, verify_weapons | 2026-01-26 | 2026-03-22 |
| verify_weapon_data.py | verify_weapon_data.py | log, verify_pdas, verify_blueprints | 2026-01-26 | 2026-03-22 |
| verify_weapon_runtime.py | Verify weapon runtime behavior by spawning with BeginPlay call | log, verify_weapon | 2026-01-26 | 2026-03-22 |
| verify_weapon_spawn.py | verify_weapon_spawn.py | log, verify | 2026-01-26 | 2026-03-22 |
| verify_weapon_statuseffects.py | Verify WeaponStatusEffectInfo was applied to weapon data assets. | log, verify | 2026-01-26 | 2026-03-22 |
| verify_widget_fix.py | Verify the widget instance data fix | log, verify_widget_fix | 2026-01-09 | 2026-03-22 |

## Debug Scripts (`scripts/debug`)
**29 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| analyze_blueprints.py | Analyze all Blueprint JSON exports and generate comprehensive migration tracker. | count_items, analyze_category, generate_tracker | 2026-01-03 | 2026-03-22 |
| analyze_hit_reaction.py | Check Logic section |  | 2026-01-26 | 2026-03-22 |
| analyze_struct_impact.py | Analyze the impact of struct migration - what assets use which structs. |  | 2026-01-07 | 2026-03-22 |
| debug_centered_text.py | Debug W_Settings_CenteredText hierarchy | main | 2026-03-09 | 2026-03-22 |
| debug_dodge_migration.py | (no description) |  | 2026-01-07 | 2026-03-22 |
| debug_gameplay_tag.py | Debug script to discover GameplayTag API. |  | 2026-01-26 | 2026-03-22 |
| debug_imc.py | debug_imc.py | debug | 2026-01-07 | 2026-03-22 |
| debug_imc2.py | debug_imc2.py | debug | 2026-01-07 | 2026-03-22 |
| debug_inputbuffer.py | debug_inputbuffer.py |  | 2026-01-03 | 2026-03-22 |
| debug_item_class.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| debug_item_properties.py | debug_item_properties.py | log, debug_item_properties | 2026-01-07 | 2026-03-22 |
| debug_item_props.py | debug_item_props.py | debug_item_properties | 2026-01-09 | 2026-03-22 |
| debug_json.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| debug_key.py | debug_key.py | debug | 2026-01-07 | 2026-03-22 |
| debug_key_row_data.py | Debug the actual row data in DT_KeyMappingCorrelation to see property values. | log, debug_row_data | 2026-01-09 | 2026-03-22 |
| debug_key_table.py | Debug the DT_KeyMappingCorrelation data table to see actual key values. | log, debug_key_table | 2026-01-09 | 2026-03-22 |
| debug_mesh_data.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| debug_mesh_data2.py | Load as Blueprint and get its generated class |  | 2026-01-09 | 2026-03-22 |
| debug_npc_interaction.py | Debug NPC interaction flow - check all required components and settings. | check_npc_interaction | 2026-03-09 | 2026-03-22 |
| debug_player_weapon_cdo.py | debug_player_weapon_cdo.py | log, debug_cdos | 2026-01-26 | 2026-03-22 |
| debug_single_item.py | debug_single_item.py | debug_single_item | 2026-01-07 | 2026-03-22 |
| debug_statuseffect_props.py | Debug: List all accessible properties on B_StatusEffectArea instances. | log, debug_props | 2026-01-26 | 2026-03-22 |
| debug_w_resources_sizebox.py | Debug W_Resources SizeBox dimensions - compare bp_only vs migrated | check_sizebox_properties | 2026-01-26 | 2026-03-22 |
| debug_w_resources_widgets.py | Debug W_Resources widgets - check if widget tree is intact |  | 2026-01-26 | 2026-03-22 |
| debug_w_resources_widgets_v2.py | Debug W_Resources widgets - check if widget tree is intact |  | 2026-01-26 | 2026-03-22 |
| debug_weapon_cdo.py | debug_weapon_cdo.py | log, debug_weapon_cdos | 2026-01-26 | 2026-03-22 |
| debug_weapon_components.py | debug_weapon_components.py | log, get_vector_str, get_rotator_str, analyze_blueprint, run_debug | 2026-01-26 | 2026-03-22 |
| debug_weapon_transform.py | debug_weapon_transform.py | log, main | 2026-01-26 | 2026-03-22 |
| diagnose_character.py | Diagnose Character - Inspect B_Soulslike_Character to find why it's not visible | log, run | 2026-01-03 | 2026-03-22 |

## Misc Scripts (`scripts/misc`)
**275 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| attach_bts_service.py | attach_bts_service.py | log, main | 2026-01-26 | 2026-03-22 |
| automated_struct_migration.py | automated_struct_migration.py | find_affected_blueprints, delete_blueprint_assets, refresh_struct_nodes_in_bl... | 2026-01-03 | 2026-03-22 |
| batch_update_all_widgets.py | Batch script to add events to ALL remaining widgets | extract_events, type_to_cpp, clean_param_name, generate_event_declarations, g... | 2026-01-03 | 2026-03-22 |
| batch_update_widgets.py | Batch script to add events to multiple widgets | extract_events, type_to_cpp, clean_param_name, generate_event_declarations, g... | 2026-01-03 | 2026-03-22 |
| bulk_struct_migration.py | Bulk Struct Migration Script - FIXED for PrimaryDataAssets | log, log_section, get_asset_object, get_property_safe, set_property_safe | 2026-01-07 | 2026-03-22 |
| bulk_struct_migration_v2.py | Bulk Struct Migration Script v2 - Uses export_text() for proper struct data extr | log, log_section, get_asset_object, get_property_safe, set_property_safe | 2026-01-07 | 2026-03-22 |
| bulk_struct_migration_v3.py | Bulk Struct Migration Script v3 - Uses pre-extracted JSON data | log, log_section, get_property_safe, set_property_safe, get_asset_path | 2026-01-07 | 2026-03-22 |
| call_cpp_statuseffect_fix.py | Call C++ ApplyAllStatusEffectRankInfo to fix poison damage values. | main | 2026-01-26 | 2026-03-22 |
| cleanup_migration_script.py | Remove the non-working POST-MIGRATION: Dodge Montages section |  | 2026-01-07 | 2026-03-22 |
| cleanup_skymanager.py | Cleanup B_SkyManager - Remove extra instances | log, cleanup_skymanager | 2026-01-26 | 2026-03-22 |
| clear_ai_enum_blueprints.py | clear_ai_enum_blueprints.py | log, main | 2026-01-26 | 2026-03-22 |
| clear_and_reparent_boss_door.py | Aggressively clear B_BossDoor and reparent to SLFBossDoor. | clear_and_reparent | 2026-01-26 | 2026-03-22 |
| clear_and_reparent_door.py | Clear B_Door's Blueprint logic and SCS, then reparent to C++ | clear_and_reparent_door | 2026-01-26 | 2026-03-22 |
| clear_and_reparent_interactables.py | (no description) | clear_blueprint_graphs, clear_and_reparent | 2026-01-26 | 2026-03-22 |
| clear_and_reparent_skymanager.py | Clear B_SkyManager Blueprint components/graphs and reparent to SLFSkyManager C++ | main | 2026-01-26 | 2026-03-22 |
| clear_animbp_eventgraphs.py | Clear EventGraphs from AnimBlueprints that have stale property accesses. | log, clear_eventgraph_only, main | 2026-03-09 | 2026-03-22 |
| clear_animbp_functions.py | clear_animbp_functions.py | log, main | 2026-01-10 | 2026-03-22 |
| clear_animbp_using_cpp.py | Clear EventGraphs from AnimBlueprints using C++ SLFAutomationLibrary. | log, clear_animbp_graphs, main | 2026-03-09 | 2026-03-22 |
| clear_boss_animbp_eventgraph.py | Clear the EventGraph of ABP_SoulslikeBossNew to fix PoiseBreakAsset errors. | log, main | 2026-03-09 | 2026-03-22 |
| clear_bts_eventgraph.py | clear_bts_eventgraph.py | log, main | 2026-01-26 | 2026-03-22 |
| clear_chaos_eventgraphs.py | Clear EventGraphs from chaos-related Blueprints after C++ migration. | clear_eventgraph, main | 2026-01-26 | 2026-03-22 |
| clear_character_eventgraph.py | Clear the EventGraph of B_Soulslike_Character so C++ input handling takes over. | log, main | 2026-01-26 | 2026-03-22 |
| clear_combobox_bindings.py | Clear ComboBoxKey delegate bindings from W_Settings_Entry. | clear_widget_delegate_bindings, main | 2026-01-26 | 2026-03-22 |
| clear_construction_script.py | Clear ConstructionScript from B_DemoRoom so C++ OnConstruction runs instead | log, clear_construction_script | 2026-01-07 | 2026-03-22 |
| clear_container_eventgraph.py | clear_container_eventgraph.py | clear_function_graphs, clear_uber_graphs, main | 2026-01-26 | 2026-03-22 |
| clear_container_lid_var.py | clear_container_lid_var.py | main | 2026-01-26 | 2026-03-22 |
| clear_container_scs.py | clear_container_scs.py | clear_container_scs | 2026-01-26 | 2026-03-22 |
| clear_destructible_logic.py | Clear B_Destructible Blueprint logic after C++ migration. | clear_destructible | 2026-01-26 | 2026-03-22 |
| clear_door_scs.py | Clear B_Door's SCS to force it to use C++ components | clear_door_scs | 2026-01-26 | 2026-03-22 |
| clear_door_scs_and_reparent.py | Clear B_Door's SCS (SimpleConstructionScript) completely, then reparent to C++. | clear_scs_and_reparent_door | 2026-01-26 | 2026-03-22 |
| clear_enemy_animbp_eventgraphs.py | Clear EventGraphs from ABP_SoulslikeEnemy and ABP_SoulslikeBossNew to fix PIE ex | log, main | 2026-03-09 | 2026-03-22 |
| clear_gi_all_graphs.py | Clear ALL graphs (including function graphs) from GI_SoulslikeFramework. | log, main | 2026-03-09 | 2026-03-22 |
| clear_level_blueprint.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| clear_levelup_widget_functions.py | Clear Blueprint function graphs for LevelUp widgets. | clear_widget_functions | 2026-01-26 | 2026-03-22 |
| clear_npc_eventgraphs.py | Clear event graphs on NPC system Blueprints after reparenting to C++. | main | 2026-03-09 | 2026-03-22 |
| clear_npc_logic.py | Clear ALL Blueprint logic (variables, functions, graphs) from NPC Blueprints. | main | 2026-03-09 | 2026-03-22 |
| clear_pda_daynight.py | Clear PDA_DayNight function graphs to resolve struct mismatch | main | 2026-01-26 | 2026-03-22 |
| clear_pickup_action_bp.py | Clear the Blueprint's EventGraph so C++ implementation runs |  | 2026-03-09 | 2026-03-22 |
| clear_pickup_action_eventgraph.py | Clear the EventGraph of B_Action_PickupItemMontage so C++ _Implementation runs |  | 2026-03-09 | 2026-03-22 |
| clear_playercard_eventgraph.py | Check for event graph logic |  | 2026-03-09 | 2026-03-22 |
| clear_quitconfirmation_eventgraph.py | Clear event graphs (removes all EventGraph logic) |  | 2026-03-09 | 2026-03-22 |
| clear_settings_entry_graphs.py | Clear W_Settings_Entry function graphs while preserving widget tree. | clear_settings_entry_graphs, verify_widget_tree, check_compile | 2026-01-26 | 2026-03-22 |
| clear_vendor_eventgraph.py | Clear Vendor NPC EventGraph - Remove Blueprint BeginPlay so C++ BeginPlay runs | log, clear_vendor_eventgraph | 2026-03-09 | 2026-03-22 |
| clear_vendor_eventgraphs.py | Clear EventGraphs from vendor widgets and reparent to C++. | clear_and_reparent, main | 2026-03-09 | 2026-03-22 |
| clear_vendor_logic.py | Clear ALL Blueprint logic from vendor widgets that still have compile errors. | clear_vendor_logic | 2026-03-09 | 2026-03-22 |
| clear_vendor_via_cpp.py | Clear Vendor NPC EventGraph using C++ SLFAutomationLibrary | clear_vendor | 2026-03-09 | 2026-03-22 |
| clear_w_settings_eventgraph.py | Clear W_Settings Blueprint EventGraph to fix compile errors. | clear_blueprint_event_graph, main | 2026-01-26 | 2026-03-22 |
| clear_weapon_graphs.py | clear_weapon_graphs.py | run | 2026-01-03 | 2026-03-22 |
| complete_animbp_export.py | Complete AnimBP Export - Exports ALL graphs, nodes, variables, and connections | log, export_pin_info, export_node_full, export_graph_full, main | 2026-01-26 | 2026-03-22 |
| complete_animbp_migration.py | Complete AnimBP Migration Script | log, main | 2026-01-26 | 2026-03-22 |
| comprehensive_migration.py | comprehensive_migration.py | extract_item_data, extract_action_data, extract_default_mesh_data, extract_ca... | 2026-01-09 | 2026-03-22 |
| comprehensive_struct_fix.py | comprehensive_struct_fix.py | check_cpp_types, load_blueprint, reconstruct_all_nodes, fix_pin_values, get_b... | 2026-01-03 | 2026-03-22 |
| count_bp_errors.py | Count Blueprint compilation errors |  | 2026-01-03 | 2026-03-22 |
| count_categories.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| diagnose_ai_enum.py | Diagnose E_AI_States enum and BB_Standard State key | log, main | 2026-01-26 | 2026-03-22 |
| diagnose_and_fix_animbp_overlay.py | Diagnose and fix AnimBP overlay state variable issues | log, main | 2026-03-09 | 2026-03-22 |
| diagnose_and_fix_structs.py | diagnose_and_fix_structs.py | check_cpp_structs_exist, diagnose_blueprint, main | 2026-01-03 | 2026-03-22 |
| diagnose_and_reparent_categoryentry.py | Diagnose and reparent W_Settings_CategoryEntry to C++ class. | main | 2026-01-26 | 2026-03-22 |
| diagnose_animbp_full.py | Full AnimBP diagnostic using C++ SLFAutomationLibrary. | log, main | 2026-01-26 | 2026-03-22 |
| diagnose_animbp_overlay.py | Diagnose AnimBP overlay state mismatch. | diagnose | 2026-01-26 | 2026-03-22 |
| diagnose_animbp_vars.py | Diagnose AnimBP variable state - check if Blueprint vars shadow C++ properties. | log, diagnose_animbp_variables, check_runtime_values, check_enum_values, main | 2026-01-26 | 2026-03-22 |
| diagnose_bb_full.py | diagnose_bb_full.py | log, diagnose | 2026-01-26 | 2026-03-22 |
| diagnose_bb_state.py | diagnose_bb_state.py | main | 2026-01-26 | 2026-03-22 |
| diagnose_bb_state_key.py | diagnose_bb_state_key.py | log, diagnose | 2026-01-26 | 2026-03-22 |
| diagnose_blend_nodes.py | Diagnose BlendListByEnum nodes in AnimBP. | log, main | 2026-01-26 | 2026-03-22 |
| diagnose_blueprint_api.py | diagnose_blueprint_api.py |  | 2026-01-03 | 2026-03-22 |
| diagnose_boss_animbp_deep.py | Check bp_only version for comparison | log | 2026-03-09 | 2026-03-22 |
| diagnose_boss_combat_manager.py | (no description) | log | 2026-03-09 | 2026-03-22 |
| diagnose_broken_nodes.py | diagnose_broken_nodes.py | main | 2026-01-03 | 2026-03-22 |
| diagnose_bt_combat.py | diagnose_bt_combat.py | log, inspect_bt_node, main | 2026-01-26 | 2026-03-22 |
| diagnose_bt_decorators.py | diagnose_bt_decorators.py | diagnose_bt_enemy, diagnose_blackboard, export_bt_via_text | 2026-01-26 | 2026-03-22 |
| diagnose_bt_intvalues.py | Diagnose BT Decorator IntValues for State key comparison | log, check_bt, main | 2026-01-26 | 2026-03-22 |
| diagnose_bt_structure.py | diagnose_bt_structure.py | log, diagnose_bt_node, main | 2026-01-26 | 2026-03-22 |
| diagnose_chaos_interface.py | Diagnose interface implementation for B_Chaos_ForceField. | main | 2026-01-26 | 2026-03-22 |
| diagnose_cinematic.py | Diagnose why cinematic isn't playing. | log, main | 2026-03-09 | 2026-03-22 |
| diagnose_container_components.py | diagnose_container_components.py | main | 2026-01-26 | 2026-03-22 |
| diagnose_container_scs.py | diagnose_container_scs.py | diagnose_container | 2026-01-26 | 2026-03-22 |
| diagnose_datatable_struct.py | diagnose_datatable_struct.py | main | 2026-01-26 | 2026-03-22 |
| diagnose_destructible_cdo.py | Diagnose B_Destructible CDO - check if GeometryCollection is set properly. | diagnose_destructible | 2026-01-26 | 2026-03-22 |
| diagnose_destructibles.py | Diagnose and fix B_Destructible visibility issues. | diagnose_destructible | 2026-01-26 | 2026-03-22 |
| diagnose_destructibles_bponly.py | Diagnose B_Destructible in bp_only for comparison. | diagnose_destructible_bponly | 2026-01-26 | 2026-03-22 |
| diagnose_dialog_widget.py | Diagnose W_Dialog widget reparenting and W_HUD widget tree. | diagnose_w_dialog | 2026-03-09 | 2026-03-22 |
| diagnose_door_complete.py | Complete diagnostic of B_Door - check everything that could cause invisibility. |  | 2026-01-26 | 2026-03-22 |
| diagnose_door_demo.py | Diagnose B_Door_Demo Blueprint specifically. |  | 2026-01-26 | 2026-03-22 |
| diagnose_embedded_widgets.py | Diagnose Embedded Settings Widgets - Check actual embedded widget property value | main | 2026-03-09 | 2026-03-22 |
| diagnose_enemy_animbp.py | diagnose_enemy_animbp.py | log, main | 2026-01-26 | 2026-03-22 |
| diagnose_enum.py | diagnose_enum.py | main | 2026-01-03 | 2026-03-22 |
| diagnose_enum_detailed.py | diagnose_enum_detailed.py | main | 2026-01-03 | 2026-03-22 |
| diagnose_enum_pins.py | diagnose_enum_pins.py |  | 2026-01-03 | 2026-03-22 |
| diagnose_enum_usage.py | diagnose_enum_usage.py | find_enum_asset, find_referencers, main | 2026-01-03 | 2026-03-22 |
| diagnose_graph_access.py | diagnose_graph_access.py |  | 2026-01-03 | 2026-03-22 |
| diagnose_interactables_complete.py | diagnose_interactables_complete.py | check_blueprint, main | 2026-01-26 | 2026-03-22 |
| diagnose_interactables_simple.py | diagnose_interactables_simple.py | check_blueprint, main | 2026-01-26 | 2026-03-22 |
| diagnose_ladder_bponly.py | Diagnostic for bp_only B_Ladder ISM components | log, diagnose_ladder_visibility | 2026-01-26 | 2026-03-22 |
| diagnose_ladder_visibility.py | Comprehensive diagnostic for B_Ladder ISM component visibility | log, diagnose_ladder_visibility | 2026-01-26 | 2026-03-22 |
| diagnose_level_actors.py | Diagnose level actors - find all actors and look for status effect related ones. | log, diagnose_level | 2026-01-26 | 2026-03-22 |
| diagnose_linkedanimlayer.py | diagnose_linkedanimlayer.py | log, main | 2026-01-10 | 2026-03-22 |
| diagnose_npc_dialog.py | Diagnose NPC dialog configuration in SLFConversion. | main | 2026-03-09 | 2026-03-22 |
| diagnose_npc_final.py | Diagnose remaining compile errors in NPC Blueprints. | main | 2026-03-09 | 2026-03-22 |
| diagnose_overlay_connections.py | diagnose_overlay_connections.py | log, main | 2026-01-26 | 2026-03-22 |
| diagnose_property_access.py | diagnose_property_access.py | log, main | 2026-01-10 | 2026-03-22 |
| diagnose_rest_menu.py | Diagnose Rest Menu time entries issue - simplified | diagnose | 2026-01-26 | 2026-03-22 |
| diagnose_settings.py | (no description) |  | 2026-03-09 | 2026-03-22 |
| diagnose_settings_compile.py | Diagnose Settings widget compilation errors in detail |  | 2026-03-09 | 2026-03-22 |
| diagnose_settings_display.py | Diagnose W_Settings Display tab - export actual widget state. | diagnose_w_settings_entry, diagnose_w_settings, check_blueprint_compile_statu... | 2026-01-26 | 2026-03-22 |
| diagnose_settings_entry.py | Diagnose W_Settings_Entry widget issues. | diagnose_w_settings_entry, check_w_settings_instances, main | 2026-01-26 | 2026-03-22 |
| diagnose_settings_errors.py | Diagnose Settings widget compilation errors |  | 2026-03-09 | 2026-03-22 |
| diagnose_settings_icons.py | Diagnose W_Settings_CategoryEntry icons at runtime. | analyze_widget_tree | 2026-01-26 | 2026-03-22 |
| diagnose_settings_runtime.py | Diagnose Settings Widgets at Runtime - Check DropDown binding and content | main | 2026-03-09 | 2026-03-22 |
| diagnose_settings_state.py | Diagnose current state of Settings widgets after backup restore. |  | 2026-03-09 | 2026-03-22 |
| diagnose_settings_tabs.py | Diagnose W_Settings tab clicking issues. | log, inspect_widget_hierarchy, diagnose_category_entry, main | 2026-01-26 | 2026-03-22 |
| diagnose_skymanager_full.py | Full diagnosis of B_SkyManager Blueprint state after migration. | log, diagnose_skymanager | 2026-01-26 | 2026-03-22 |
| diagnose_skymanager_instance.py | Diagnose B_SkyManager: Compare Blueprint CDO vs Level Instance | log, diagnose_skymanager | 2026-01-26 | 2026-03-22 |
| diagnose_stats_runtime.py | Diagnose what stats are in the stat table (DT_ExampleStatTable). | diagnose | 2026-01-26 | 2026-03-22 |
| diagnose_status_effect_assets.py | Diagnose Status Effect Data Assets | log, diagnose | 2026-01-26 | 2026-03-22 |
| diagnose_statuseffect_complete.py | Complete diagnosis and fix for B_StatusEffectArea. | log, diagnose_and_fix | 2026-01-26 | 2026-03-22 |
| diagnose_vendor_npc.py | Diagnose NPC Vendor configuration - mesh, animation, position | diagnose_vendor_npc | 2026-03-09 | 2026-03-22 |
| direct_struct_migration.py | direct_struct_migration.py | find_affected_blueprints, verify_cpp_structs, verify_bp_structs, migrate_stru... | 2026-01-03 | 2026-03-22 |
| do_reparent_pda_action.py | do_reparent_pda_action.py |  | 2026-01-07 | 2026-03-22 |
| enable_crafting_test.py | !/usr/bin/env python | log, enable_crafting | 2026-01-26 | 2026-03-22 |
| execute_migration.py | execute_migration.py | load_yaml_simple, get_blueprint, find_node_by_guid, fix_pin_value, fix_variab... | 2026-01-03 | 2026-03-22 |
| find_all_category_entries.py | Find all W_Inventory_CategoryEntry instances in W_Inventory widget tree | log, find_all_category_entries | 2026-01-09 | 2026-03-22 |
| find_enum_references.py | find_enum_references.py | search_dict, process_json_file, main | 2026-01-03 | 2026-03-22 |
| find_enumerator_pins.py | find_enumerator_pins.py | main | 2026-01-03 | 2026-03-22 |
| find_levels_and_doors.py | Find all level files and door-related assets in the project. |  | 2026-01-26 | 2026-03-22 |
| find_reparent_method.py | find_reparent_method.py |  | 2026-01-07 | 2026-03-22 |
| find_sockets_v2.py | find_sockets_v2.py | log, main | 2026-01-26 | 2026-03-22 |
| find_weapon_meshes.py | find_weapon_meshes.py | log, find_meshes | 2026-01-26 | 2026-03-22 |
| find_widget_subobjects.py | Find widget subobjects in WidgetBlueprint to access widget instances | log, find_widget_subobjects | 2026-01-09 | 2026-03-22 |
| force_clear_boss_animbp.py | Force clear ALL Blueprint logic from ABP_SoulslikeBossNew (except AnimGraph). | log, main | 2026-03-09 | 2026-03-22 |
| force_recompile_animbp.py | Force full recompilation of ABP_SoulslikeBossNew by: | log, main | 2026-03-09 | 2026-03-22 |
| force_recompile_container.py | force_recompile_container.py | main | 2026-01-26 | 2026-03-22 |
| force_recompile_interactables.py | (no description) | force_recompile | 2026-01-26 | 2026-03-22 |
| force_recompile_settings.py | Force recompile Settings widgets and verify they compile |  | 2026-03-09 | 2026-03-22 |
| force_reparent_npc.py | Force reparent problematic NPC Blueprints using ForceReparentAndValidate. | main | 2026-03-09 | 2026-03-22 |
| force_reparent_pda_action.py | force_reparent_pda_action.py |  | 2026-01-07 | 2026-03-22 |
| full_migration.py | full_migration.py - Complete migration workflow | extract_item_data, run_migration, apply_item_data, apply_dodge_montages, main | 2026-01-09 | 2026-03-22 |
| full_settings_migration.py | Full Settings Migration - Single Process |  | 2026-03-09 | 2026-03-22 |
| full_type_migration.py | full_type_migration.py | ue_path_to_file_path, check_prerequisites, print_required_redirects, find_aff... | 2026-01-03 | 2026-03-22 |
| healthcheck_migration.py | SoulslikeFramework Migration Healthcheck Script | **JsonItem, CppItem, HealthcheckResult**; parse_json_export, find_cpp_files, ... | 2026-01-03 | 2026-03-22 |
| inspect_all_transitions.py | inspect_all_transitions.py | log, main | 2026-01-26 | 2026-03-22 |
| inspect_bp_only_animbp.py | inspect_bp_only_animbp.py | log, main | 2026-01-26 | 2026-03-22 |
| inspect_bp_variables.py | inspect_bp_variables.py | log, main | 2026-01-26 | 2026-03-22 |
| inspect_components.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| inspect_details.py | Pick a complex blueprint that likely has documented functions | print_recursive_search | 2026-01-09 | 2026-03-22 |
| inspect_final.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| inspect_hierarchy.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| inspect_json.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| inspect_json_v2.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| inspect_json_v3.py | (no description) | find_key | 2026-01-09 | 2026-03-22 |
| inspect_logic.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| inspect_transitions.py | inspect_transitions.py | log, main | 2026-01-26 | 2026-03-22 |
| inspect_widget_bp_api.py | Inspect WidgetBlueprint API to find correct way to access widget tree | log, inspect_widget_bp | 2026-01-09 | 2026-03-22 |
| investigate_player_weapons.py | investigate_player_weapons.py | log, get_class_hierarchy, investigate_weapons | 2026-01-26 | 2026-03-22 |
| list_cdo_properties.py | List all CDO properties available on the AnimBP instance | list_cdo_properties | 2026-01-26 | 2026-03-22 |
| list_hand_sockets.py | list_hand_sockets.py | log, main | 2026-01-26 | 2026-03-22 |
| list_items.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| list_pda_action_path.py | list_pda_action_path.py |  | 2026-01-07 | 2026-03-22 |
| mesh_test_with_output.py | Test mesh data and write results to file | log | 2026-01-07 | 2026-03-22 |
| pie_test_hud.py | PIE Test for W_HUD and Game Menu | run_pie_test | 2026-01-07 | 2026-03-22 |
| populate_daynight_entries.py | Populate DA_ExampleDayNightInfo with standard day/night entries | main | 2026-01-26 | 2026-03-22 |
| read_aic_bp.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| read_char_bp.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| read_enemy_bp.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| read_gamemenu_bp.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| read_hud_bp.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| read_interactable_bp.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| read_navigable_bp.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| read_pickup_bp.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| read_raw.py | (no description) |  | 2026-01-09 | 2026-03-22 |
| recompile_and_check_door.py | Force recompile B_Door and check components |  | 2026-01-26 | 2026-03-22 |
| reconstruct_nodes.py | reconstruct_nodes.py | main | 2026-01-03 | 2026-03-22 |
| recreate_spells_from_blueprint.py | recreate_spells_from_blueprint.py | log, main | 2026-01-29 | 2026-03-22 |
| redirect_overlay_bindings.py | Redirect overlay state property bindings from _0 suffixed vars to C++ properties | log, main | 2026-03-09 | 2026-03-22 |
| reduce_poison_sword_damage.py | Reduce poison sword damage from testing values. | main | 2026-01-26 | 2026-03-22 |
| refresh_level_blueprint.py | Refresh Level Blueprint nodes to fix stale pin connections after C++ migration. | log, refresh_level_blueprint, main | 2026-03-09 | 2026-03-22 |
| refresh_level_containers.py | refresh_level_containers.py | main | 2026-01-26 | 2026-03-22 |
| refresh_skymanager.py | Refresh B_SkyManager from bp_only and reparent to C++ | main | 2026-01-26 | 2026-03-22 |
| repair_transition_wiring.py | repair_transition_wiring.py | log, main | 2026-01-26 | 2026-03-22 |
| reparent_action_assets.py | reparent_action_assets.py |  | 2026-01-07 | 2026-03-22 |
| reparent_ai_components.py | reparent_ai_components.py | reparent_blueprint, main | 2026-01-26 | 2026-03-22 |
| reparent_animbp_only.py | Reparent ABP_SoulslikeCharacter_Additive to C++ AnimInstance | reparent_animbp | 2026-03-09 | 2026-03-22 |
| reparent_animbp_simple.py | Simple reparenting of ABP_SoulslikeCharacter_Additive to C++ class. | log, simple_reparent | 2026-01-26 | 2026-03-22 |
| reparent_animbp_surgical.py | Surgical reparenting of ABP_SoulslikeCharacter_Additive to C++ class. | surgical_reparent | 2026-01-26 | 2026-03-22 |
| reparent_animbps.py | reparent_animbps.py | main | 2026-01-26 | 2026-03-22 |
| reparent_b_stat.py | reparent_b_stat.py - Phase A: Reparent B_Stat to USLFStatObject | reparent_b_stat | 2026-01-03 | 2026-03-22 |
| reparent_boss_door.py | Reparent B_BossDoor to ASLFBossDoor. | reparent_boss_door | 2026-01-26 | 2026-03-22 |
| reparent_categoryentry_cpp.py | Reparent W_Settings_CategoryEntry to C++ using SLFAutomationLibrary. | main | 2026-01-26 | 2026-03-22 |
| reparent_centered_text.py | Reparent W_Settings_CenteredText to C++ UW_Settings_CenteredText | main | 2026-03-09 | 2026-03-22 |
| reparent_centered_text_surgical.py | Surgical reparent of W_Settings_CenteredText to C++ UW_Settings_CenteredText | main | 2026-03-09 | 2026-03-22 |
| reparent_container.py | reparent_container.py | reparent_container | 2026-01-26 | 2026-03-22 |
| reparent_container_to_base.py | reparent_container_to_base.py | main | 2026-01-26 | 2026-03-22 |
| reparent_container_to_interactable.py | reparent_container_to_interactable.py | main | 2026-01-26 | 2026-03-22 |
| reparent_deathcurrency.py | Reparent B_DeathCurrency Blueprint to AB_DeathCurrency C++ class. | reparent_deathcurrency | 2026-01-26 | 2026-03-22 |
| reparent_destructible.py | Reparent B_Destructible to the correct C++ parent class AB_Destructible. | reparent_destructible | 2026-01-26 | 2026-03-22 |
| reparent_dialog_assets.py | Reparent individual dialog data assets to C++ UPDA_Dialog. | main | 2026-03-09 | 2026-03-22 |
| reparent_door_only.py | Reparent only B_Door to C++ AB_Door class | reparent_door | 2026-01-26 | 2026-03-22 |
| reparent_execution_widget.py | Reparent W_TargetExecutionIndicator to C++ class. | reparent_execution_widget | 2026-01-26 | 2026-03-22 |
| reparent_genericbutton.py | Reparent W_GenericButton to UW_GenericButton C++ class |  | 2026-03-09 | 2026-03-22 |
| reparent_inputbuffer.py | reparent_inputbuffer.py | run_reparent | 2026-01-03 | 2026-03-22 |
| reparent_interactables.py | B_Interactable needs to be reparented to AB_Interactable | reparent_interactables | 2026-01-26 | 2026-03-22 |
| reparent_interactables_v2.py | (no description) | reparent_interactables | 2026-01-26 | 2026-03-22 |
| reparent_level_bp_deps.py | Reparent B_SequenceActor and GI_SoulslikeFramework to their C++ parent classes. | log, reparent_and_clear, main | 2026-03-09 | 2026-03-22 |
| reparent_notifies.py | Map Blueprint notify paths to C++ parent classes |  | 2026-01-26 | 2026-03-22 |
| reparent_npc_system.py | Reparent NPC system Blueprints to C++ classes. | main | 2026-03-09 | 2026-03-22 |
| reparent_npc_window.py | Reparent W_NPC_Window widget Blueprint to C++ UW_NPC_Window class | reparent_widget | 2026-03-09 | 2026-03-22 |
| reparent_only.py | Reparent AnimBP to C++ class WITHOUT removing variables. | log, main | 2026-01-26 | 2026-03-22 |
| reparent_pda_action.py | reparent_pda_action.py |  | 2026-01-07 | 2026-03-22 |
| reparent_pda_basecharinfo.py | Surgical reparent script: PDA_BaseCharacterInfo -> UPDA_BaseCharacterInfo | reparent | 2026-03-09 | 2026-03-22 |
| reparent_pda_dialog.py | Reparent PDA_Dialog Blueprint to UPDA_Dialog C++ class. | main | 2026-03-09 | 2026-03-22 |
| reparent_pda_item.py | reparent_pda_item.py |  | 2026-01-04 | 2026-03-22 |
| reparent_pda_vendor.py | Reparent PDA_Vendor Blueprint to C++ UPDA_Vendor | reparent_vendor | 2026-03-09 | 2026-03-22 |
| reparent_player_weapons.py | reparent_player_weapons.py | log, reparent_and_apply | 2026-01-26 | 2026-03-22 |
| reparent_playercard.py | Force save after reparent (reparent_blueprint returns false for widgets but CDO  |  | 2026-03-09 | 2026-03-22 |
| reparent_playercard_v2.py | Reparent W_Settings_PlayerCard and W_Settings_QuitConfirmation to their C++ clas |  | 2026-03-09 | 2026-03-22 |
| reparent_restingpoint_only.py | reparent_restingpoint_only.py | main | 2026-01-26 | 2026-03-22 |
| reparent_settings.py | Assets to reparent |  | 2026-01-26 | 2026-03-22 |
| reparent_settings_categoryentry.py | Reparent W_Settings_CategoryEntry to C++ class. | main | 2026-01-26 | 2026-03-22 |
| reparent_settings_entry.py | Reparent W_Settings_Entry Blueprint to C++ UW_Settings_Entry class. | reparent_widget, main | 2026-01-26 | 2026-03-22 |
| reparent_settings_entry_preserve.py | Reparent W_Settings_Entry to C++ while PRESERVING widget tree bindings. | reparent_w_settings_entry, verify_reparent, check_compile_status | 2026-01-26 | 2026-03-22 |
| reparent_settings_widgets.py | Reparent W_Settings and W_Settings_CategoryEntry to C++ classes. | reparent_widget, main | 2026-01-26 | 2026-03-22 |
| reparent_skymanager_v2.py | Reparent B_SkyManager to SLFSkyManager C++ class | main | 2026-01-26 | 2026-03-22 |
| reparent_spawn_notify.py | reparent_spawn_notify.py | log, reparent_notify, main | 2026-03-09 | 2026-03-22 |
| reparent_vendor_assets.py | Reparent all vendor-related assets to C++ | reparent_asset, main | 2026-03-09 | 2026-03-22 |
| reparent_vendor_to_cpp.py | Reparent vendor widgets to C++ classes. | reparent_widget, main | 2026-03-09 | 2026-03-22 |
| reparent_vendor_widget.py | Reparent W_NPC_Window_Vendor Blueprint to C++ class UW_NPC_Window_Vendor | reparent_vendor_widget | 2026-03-09 | 2026-03-22 |
| reparent_vendor_widgets.py | Reparent ALL vendor widgets to their C++ classes. | reparent_widget, main | 2026-03-09 | 2026-03-22 |
| reparent_w_resources.py | Reparent W_Resources to C++ class |  | 2026-01-26 | 2026-03-22 |
| reparent_w_resources_only.py | Reparent W_Resources to C++ without clearing anything |  | 2026-01-26 | 2026-03-22 |
| resave_action_assets.py | resave_action_assets.py | resave_action_assets | 2026-01-07 | 2026-03-22 |
| resave_attack_montages.py | Resave attack montages to pick up the reparented ANS_ToggleChaosField class. | resave_montages | 2026-01-26 | 2026-03-22 |
| resave_data_assets.py | Resave data assets to update serialized struct type names. |  | 2026-01-07 | 2026-03-22 |
| resave_daynight_data.py | Resave DA_ExampleDayNightInfo to apply struct redirect | main | 2026-01-26 | 2026-03-22 |
| resave_dodge_action.py | resave_dodge_action.py |  | 2026-01-07 | 2026-03-22 |
| resave_interactables.py | List of blueprints to resave | resave_interactables | 2026-01-26 | 2026-03-22 |
| resave_skymanager.py | Resave B_SkyManager to ensure SCS components are properly linked after reparenti | main | 2026-01-26 | 2026-03-22 |
| reset_cutscene_flag.py | Reset the FirstTimeOnDemoLevel flag to true so the cutscene plays on next PIE | log, main | 2026-03-09 | 2026-03-22 |
| resilient_migration.py | resilient_migration.py | extract_all_data_from_backup, extract_item_data, extract_default_mesh_data, e... | 2026-01-09 | 2026-03-22 |
| run_apply_embedded_settings.py | Apply Embedded Settings Widgets from JSON cache | main | 2026-03-09 | 2026-03-22 |
| run_apply_flask_data.py | run_apply_flask_data.py | run_apply_flask_data | 2026-03-09 | 2026-03-22 |
| run_container_extract.py | run_container_extract.py | main | 2026-01-26 | 2026-03-22 |
| run_destructible_diag.py | Run C++ destructible diagnostics via USLFAutomationLibrary. | run_diag | 2026-01-26 | 2026-03-22 |
| run_dialog_migration.py | Run Dialog DataTable migration using C++ automation. |  | 2026-03-09 | 2026-03-22 |
| run_dialog_validation.py | Run asset validation to compare dialog assets between bp_only and SLFConversion |  | 2026-03-09 | 2026-03-22 |
| run_dialog_verify.py | Run Dialog DataTable verification using C++ automation. |  | 2026-03-09 | 2026-03-22 |
| run_extract_settings.py | Run C++ ExtractEmbeddedSettingsWidgets on current project |  | 2026-03-09 | 2026-03-22 |
| run_fix.py | 1. THE LIST OF ZOMBIE ASSETS | run_global_migration | 2026-01-03 | 2026-03-22 |
| run_fix_all.py | run_fix_all.py - Complete Variable Migration Script v13 | verify_prerequisites, run_full_migration | 2026-01-03 | 2026-03-22 |
| run_migration.py | run_migration.py | find_bp, run | 2026-01-03 | 2026-03-22 |
| run_migration_backup.py | run_migration.py | find_bp, run | 2026-01-03 | 2026-03-22 |
| run_migration_interfaces_only.py | run_migration_interfaces_only.py | find_interface, run | 2026-01-03 | 2026-03-22 |
| run_npc_dialog_test.py | Run NPC Dialog PIE test. |  | 2026-03-09 | 2026-03-22 |
| run_pie_npc_test.py | Run PIE NPC Dialog Test - loads level and starts PIE with test command. | run_pie_npc_test | 2026-03-09 | 2026-03-22 |
| run_pie_test.py | PIE Input Simulation Test |  | 2026-01-03 | 2026-03-22 |
| run_settings_cpp_migration.py | Run Settings Widget Migration using C++ Automation on SLFConversion |  | 2026-03-09 | 2026-03-22 |
| run_settings_fix.py | Run settings fix and validation: | main | 2026-03-09 | 2026-03-22 |
| run_settings_migration.py | Full Settings Widget Migration using C++ Automation |  | 2026-03-09 | 2026-03-22 |
| run_statuseffect_fix.py | run_statuseffect_fix.py |  | 2026-01-26 | 2026-03-22 |
| run_weapon_debug.py | (no description) |  | 2026-01-26 | 2026-03-22 |
| save_animbps.py | save_animbps.py | main | 2026-01-26 | 2026-03-22 |
| scan_backup_item_data.py | Scan backup item data assets to extract ItemInformation values | extract_strings, main | 2026-01-09 | 2026-03-22 |
| scan_widgets_for_data.py | Scan backup widgets for texture/data that needs migration | extract_textures, main | 2026-01-09 | 2026-03-22 |
| shell_migration_complete.py | shell_migration_complete.py | find_bp, clear_bp, reparent_bp, compile_and_save_bp, migrate | 2026-01-03 | 2026-03-22 |
| show_blend_connections.py | show_blend_connections.py | log, main | 2026-01-26 | 2026-03-22 |
| simple_mesh_test.py | Simple test to check if mesh data loads |  | 2026-01-07 | 2026-03-22 |
| slf_type_mapping.py | Shared type mapping module for SoulslikeFramework Blueprint-to-C++ migration. | get_enum_cpp_name, get_struct_cpp_name, get_class_prefix, get_class_cpp_name,... | 2026-01-03 | 2026-03-22 |
| snapshot_blueprint_state.py | snapshot_blueprint_state.py | get_property_value_safe, serialize_value, snapshot_blueprint, capture_full_sn... | 2026-01-03 | 2026-03-22 |
| spawn_and_check_weapon.py | spawn_and_check_weapon.py | log, main | 2026-01-26 | 2026-03-22 |
| spawn_boss_door_check.py | Spawn B_BossDoor and check its component values | main | 2026-01-26 | 2026-03-22 |
| spawn_bponly_container.py | spawn_bponly_container.py | main | 2026-01-26 | 2026-03-22 |
| spawn_check_container.py | spawn_check_container.py | main | 2026-01-26 | 2026-03-22 |
| spawn_check_restingpoint.py | (no description) | spawn_and_check_restingpoint | 2026-01-26 | 2026-03-22 |
| spawn_test_destructible.py | Spawn test for B_Destructible to verify visibility fix. | spawn_test_destructible | 2026-01-26 | 2026-03-22 |
| surgical_restore_npc.py | Surgical NPC System Restore | surgical_restore | 2026-03-09 | 2026-03-22 |
| tae_parser_outline.py | TAE (TimeAct Event) Parser for FromSoftware Animation Files | **EventType, JumpTableFlag, Event**; find_invulnerability_frames, find_hyper_... | 2026-03-09 | 2026-03-22 |

## Blender Pipeline (`scripts/blender`)
**6 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| build_proper_blend.py | Build proper .blend files: HKX armature + FLVER mesh + HKX animations. | _to_blender_vector, _to_blender_matrix3, game_bone_to_bl_matrix, clean_scene,... | 2026-02-23 | 2026-03-22 |
| extract_animations.py | Elden Ring Animation Extraction - Headless Blender Script | **HeadlessLogger**; _to_blender_vector, _to_blender_matrix3, _game_bone_trans... | 2026-02-23 | 2026-03-22 |
| mesh_animation_pipeline.py | mesh_animation_pipeline.py - Reusable Blender-only pipeline for custom enemy mes | get_action_fcurves, get_fcurves_collection, clean_orphans, import_fbx, get_ar... | 2026-02-23 | 2026-03-22 |
| reexport_anim_only.py | reexport_anim_only.py - Re-export Phase 3 forensic FBXes as animation-only (no m | clean_scene, process_animation, main | 2026-02-23 | 2026-03-22 |
| tae_parser.py | TAE (TimeAct Event) Parser for FromSoftware Games (Elden Ring, DS3, Sekiro) | **EventType, JumpTableFlag, TAEEvent**; export_all_animations, print_animatio... | 2026-02-23 | 2026-03-22 |
| validate_reskin.py | Validate reskinned mesh against original FLVER mesh. | log, compute_bone_weight_stats, compute_deformation_quality | 2026-02-23 | 2026-03-22 |

## Test Scripts (`scripts/tests`)
**44 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| char_final_check.py | (no description) |  | 2026-01-03 | 2026-03-22 |
| char_simple_check.py | (no description) |  | 2026-01-03 | 2026-03-22 |
| check_char_mesh.py | (no description) |  | 2026-01-03 | 2026-03-22 |
| check_char_state.py | (no description) |  | 2026-01-03 | 2026-03-22 |
| check_cpp_class.py | Load the C++ class directly and check its CDO |  | 2026-01-03 | 2026-03-22 |
| check_parent.py | Check B_Soulslike_Character parent |  | 2026-01-03 | 2026-03-22 |
| check_pawn.py | (no description) |  | 2026-01-03 | 2026-03-22 |
| check_scs_detail.py | (no description) |  | 2026-01-03 | 2026-03-22 |
| check_weapon_childactor.py | check_weapon_childactor.py | run | 2026-01-03 | 2026-03-22 |
| check_weapon_scs.py | check_weapon_scs.py | run | 2026-01-03 | 2026-03-22 |
| check_weapon_scs_detailed.py | check_weapon_scs_detailed.py | run | 2026-01-03 | 2026-03-22 |
| check_weapon_spawn.py | check_weapon_spawn.py | run | 2026-01-03 | 2026-03-22 |
| compare_components.py | (no description) | check_blueprint | 2026-01-03 | 2026-03-22 |
| compile_blueprints.py | compile_blueprints.py | run | 2026-01-03 | 2026-03-22 |
| deep_scs_check.py | Try different ways to access SCS |  | 2026-01-03 | 2026-03-22 |
| detailed_character_diagnostic.py | detailed_character_diagnostic.py | run | 2026-01-03 | 2026-03-22 |
| detailed_enemy_diagnostic.py | detailed_enemy_diagnostic.py | run | 2026-01-03 | 2026-03-22 |
| enemy_weapon_diagnostic.py | enemy_weapon_diagnostic.py | run | 2026-01-03 | 2026-03-22 |
| gamemode_check.py | Check GameMode DefaultPawnClass setting |  | 2026-01-03 | 2026-03-22 |
| load_level_test.py | load_level_test.py | run | 2026-01-03 | 2026-03-22 |
| parent_check.py | Check B_Soulslike_Character |  | 2026-01-03 | 2026-03-22 |
| pie_simulation_test.py | pie_simulation_test.py | run | 2026-01-03 | 2026-03-22 |
| pie_visibility_test.py | Get editor subsystem |  | 2026-01-03 | 2026-03-22 |
| pie_weapon_test.py | PIE Test - Verify enemy weapons are spawned correctly |  | 2026-01-03 | 2026-03-22 |
| player_visibility_check.py | Check player character visibility and component settings |  | 2026-01-03 | 2026-03-22 |
| run_all_tests.py | =============================================================================== | print_banner, print_test_plan, initialize_context, run_all_tests, generate_de... | 2026-01-03 | 2026-03-22 |
| simple_check.py | Check B_Soulslike_Character |  | 2026-01-03 | 2026-03-22 |
| simple_check2.py | Check B_Soulslike_Character |  | 2026-01-03 | 2026-03-22 |
| simple_pie_test.py | simple_pie_test.py | run | 2026-01-03 | 2026-03-22 |
| slf_test_framework.py | ═══════════════════════════════════════════════════════════════════════════════ | **TestConfig, LogLevel, TestLogger**; safe_call, load_class_safe, load_asset_... | 2026-01-03 | 2026-03-22 |
| test_action_system.py | test_action_system.py | run | 2026-01-03 | 2026-03-22 |
| test_ai_systems.py | =============================================================================== | test_ai_component_classes, test_ai_behavior_manager, test_ai_behavior_paramet... | 2026-01-03 | 2026-03-22 |
| test_animation_systems.py | =============================================================================== | test_anim_blueprint_classes, test_player_abp_states, test_anim_notify_classes... | 2026-01-03 | 2026-03-22 |
| test_character_movement.py | test_character_movement.py | run | 2026-01-03 | 2026-03-22 |
| test_core_systems.py | ═══════════════════════════════════════════════════════════════════════════════ | test_game_mode_class, test_player_controller_class, test_game_state_class, te... | 2026-01-03 | 2026-03-22 |
| test_demo_level.py | test_demo_level.py | run | 2026-01-03 | 2026-03-22 |
| test_gameplay_systems.py | ═══════════════════════════════════════════════════════════════════════════════ | test_stat_manager_exists, test_stat_manager_properties, test_stat_classes, te... | 2026-01-03 | 2026-03-22 |
| test_interfaces.py | =============================================================================== | test_interface_classes, test_interactable_interface, test_interactable_functi... | 2026-01-03 | 2026-03-22 |
| test_player_spawn.py | Test player spawning and game framework in L_Demo_Showcase | log, test_game_framework, test_level_info, test_blueprint_game_mode, test_pla... | 2026-01-03 | 2026-03-22 |
| test_slf_systems.py | ═══════════════════════════════════════════════════════════════════════════════ | **TestResult, TestSuite**; log, load_class_safe, get_cdo_safe, has_function, ... | 2026-01-03 | 2026-03-22 |
| test_ui_widgets.py | ═══════════════════════════════════════════════════════════════════════════════ | test_hud_widgets, test_menu_widgets, test_npc_widgets, test_debug_window, tes... | 2026-01-03 | 2026-03-22 |
| test_utility_systems.py | =============================================================================== | test_save_load_manager_class, test_save_slot_properties, test_save_data_struc... | 2026-01-03 | 2026-03-22 |
| test_world_actors.py | =============================================================================== | test_interactable_base, test_interactable_interface, test_container_base, tes... | 2026-01-03 | 2026-03-22 |
| verify_mesh.py | Check C++ SLFSoulslikeCharacter class |  | 2026-01-03 | 2026-03-22 |

## C++ Source (root) (`Source/SLFConversion`)
**37 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| SetupBatchEnemyCommandlet.cpp | SetupBatchEnemyCommandlet.cpp | ToPascalCase, CategorizeAnimID, Main, Printf, StaticClass | 2026-03-15 | 2026-03-22 |
| SetupBatchEnemyCommandlet.h | SetupBatchEnemyCommandlet.h | **USetupBatchEnemyCommandlet**; CreateMontages | 2026-03-15 | 2026-03-20 |
| IKRetargetMultiSourceCommandlet.cpp | IKRig headers | Main, ImportMeshFBX, ImportAnimFBX, CreateIKRigForERSkeleton, CreateRetargeter | 2026-03-18 | 2026-03-18 |
| IKRetargetMultiSourceCommandlet.h | Import multi-source enemy animations and retarget to c3100 via UE5 IK Retargeter | **UIKRetargetMultiSourceCommandlet, UIKRigDefinition, UIKRetargeter**; Create... | 2026-03-18 | 2026-03-18 |
| AutoBuildDungeonCommandlet.cpp | AutoBuildDungeonCommandlet.cpp | GetBiomePreset, Main, SaveAsset, SavePackage, SaveLevel | 2026-03-09 | 2026-03-16 |
| AutoBuildDungeonCommandlet.h | AutoBuildDungeonCommandlet.h | **UCellFlowAsset, UDungeonThemeAsset, UAutoBuildDungeonCommandlet**; Generate... | 2026-03-09 | 2026-03-16 |
| SLFAutomationLibrary.cpp | SLFAutomationLibrary.cpp | GetBlueprintVariables, GetBlueprintFunctions, GetBlueprintEventDispatchers, G... | 2026-01-03 | 2026-03-16 |
| SLFAutomationLibrary.h | SLFAutomationLibrary.h | **USLFAutomationLibrary**; ApplyWeaponStatData, ApplyArmorStatChanges, Valida... | 2026-01-03 | 2026-03-16 |
| SLFConversion.cpp | Fill out your copyright notice in the Description page of Project Settings. | StaticClass | 2026-01-03 | 2026-03-16 |
| SLFEnums.h | SLFEnums.h |  | 2026-01-03 | 2026-03-09 |
| SLFGameTypes.h | SLFGameTypes.h | **UPDA_ActionBase_C, UPDA_Item_C, UPDA_WeaponAbility_C**; GetTypeHash | 2026-01-03 | 2026-03-09 |
| SetupGameAnimCommandlet.cpp | SetupGameAnimCommandlet.cpp | Main, CopyAnimAsset, CreateMontageFromSequence, SetupRestingPointAnims, Setup... | 2026-03-09 | 2026-03-09 |
| SetupGameAnimCommandlet.h | SetupGameAnimCommandlet.h | **USetupGameAnimCommandlet** | 2026-03-09 | 2026-03-09 |
| SetupOpenWorldCommandlet.cpp | SetupOpenWorldCommandlet.cpp | Main, GenerateHeightmap, CarveFlats, CreateLandscape, DiscoverWastelandAssets | 2026-02-27 | 2026-03-09 |
| SetupOpenWorldCommandlet.h | SetupOpenWorldCommandlet.h | **ADungeon, UCellFlowAsset, UDungeonThemeAsset**; ScatterCategory | 2026-02-27 | 2026-03-09 |
| SetupSentinelCommandlet.cpp | (no description) | Main, SavePackage | 2026-02-21 | 2026-03-09 |
| SetupTitanWorldCommandlet.cpp | SetupTitanWorldCommandlet.cpp | Main, InitializeZoneRegions, SetupZoneTriggers, SetupRestingPoints, SetupEnem... | 2026-02-27 | 2026-03-09 |
| SetupTitanWorldCommandlet.h | SetupTitanWorldCommandlet.h | **USetupTitanWorldCommandlet** | 2026-02-27 | 2026-03-09 |
| SetupSentinelCommandlet.h | (no description) | **USetupSentinelCommandlet** | 2026-02-18 | 2026-02-21 |
| BlueprintFixerLibrary.cpp | BlueprintFixerLibrary.cpp | RefreshFunctionCallsForClass, ReconstructAllNodes, GetFunctionCallInfo, FixEv... | 2026-01-03 | 2026-02-18 |
| BlueprintFixerLibrary.h | BlueprintFixerLibrary.h | **UBlueprintFixerLibrary** | 2026-01-03 | 2026-02-18 |
| PythonBridge.cpp | PythonBridge.cpp - Comprehensive Blueprint Export and Migration Utilities | ExtractMapKeyValueTypes, SerializePropertyType, SerializeVariable, SerializeP... | 2026-01-03 | 2026-02-18 |
| PythonBridge.h | PythonBridge.h - Comprehensive Blueprint Export and Migration Utilities | **UBlueprint, UAnimBlueprint, UUserDefinedEnum** | 2026-01-03 | 2026-02-18 |
| SLFAssetValidator.cpp | SLFAssetValidator.cpp | ExportObjectProperties, ExportStructProperties, ExportBlueprintProperties, Ex... | 2026-02-07 | 2026-02-18 |
| SLFAssetValidator.h | SLFAssetValidator.h | **USLFAssetValidator** | 2026-02-07 | 2026-02-18 |
| SLFYAMLExporter.cpp | SLFYAMLExporter.cpp | GetIndent, ChrN, EscapeYAMLString, Printf, FTextToString | 2026-02-07 | 2026-02-18 |
| SLFYAMLExporter.h | SLFYAMLExporter.h | **USLFYAMLExporter** | 2026-02-07 | 2026-02-18 |
| SetupC3100Commandlet.cpp | (no description) | Main, SavePackage | 2026-02-18 | 2026-02-18 |
| SetupC3100Commandlet.h | (no description) | **USetupC3100Commandlet** | 2026-02-18 | 2026-02-18 |
| SLFPrimaryDataAssets.cpp | SLFPrimaryDataAssets.cpp | GetDialogTableBasedOnProgress, GetCompletionProgressTag, ReduceItemStock, Get... | 2026-01-07 | 2026-02-07 |
| SLFPrimaryDataAssets.h | SLFPrimaryDataAssets.h | **UNiagaraSystem, USoundCue, UAnimMontage**; Guard_R, Guard_R_Hit, Guard_L, G... | 2026-01-03 | 2026-02-07 |
| WeaponDebugLibrary.cpp | WeaponDebugLibrary.cpp | LogAllWeaponsInPIE, SaveWeaponDataToJSON | 2026-01-16 | 2026-01-16 |
| WeaponDebugLibrary.h | WeaponDebugLibrary.h | **UWeaponDebugLibrary** | 2026-01-16 | 2026-01-16 |
| MyActor.cpp | Fill out your copyright notice in the Description page of Project Settings. | BeginPlay, Tick | 2026-01-03 | 2026-01-03 |
| MyActor.h | Fill out your copyright notice in the Description page of Project Settings. | **AMyActor** | 2026-01-03 | 2026-01-03 |
| SLFConversion.h | Fill out your copyright notice in the Description page of Project Settings. |  | 2026-01-03 | 2026-01-03 |
| SLFStatTypes.h | SLFStatTypes.h |  | 2026-01-03 | 2026-01-03 |

## C++ AI (`Source/SLFConversion/AI`)
**54 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| BTD_StateEquals.cpp | BTD_StateEquals.cpp | CalculateRawConditionValue, GetStaticDescription, Printf | 2026-01-12 | 2026-01-12 |
| BTD_StateEquals.h | BTD_StateEquals.h | **UBTD_StateEquals** | 2026-01-12 | 2026-01-12 |
| BTS_ChaseBounds.cpp | BTS_ChaseBounds.cpp | TickNode | 2026-01-03 | 2026-01-12 |
| BTS_DebugLog.cpp | BTS_DebugLog.cpp | TickNode | 2026-01-12 | 2026-01-12 |
| BTS_DebugLog.h | BTS_DebugLog.h | **UBTS_DebugLog** | 2026-01-12 | 2026-01-12 |
| BTS_IsTargetDead.cpp | BTS_IsTargetDead.cpp | TickNode | 2026-01-03 | 2026-01-12 |
| BTS_TryGetAbility.cpp | BTS_TryGetAbility.cpp | TickNode | 2026-01-03 | 2026-01-12 |
| BTT_PatrolPath.cpp | BTT_PatrolPath.cpp | ExecuteTask, AbortTask | 2026-01-03 | 2026-01-12 |
| BTT_SimpleMoveTo.cpp | BTT_SimpleMoveTo.cpp | ExecuteTask, OnMoveCompleted, CleanupMoveDelegate, AbortTask | 2026-01-03 | 2026-01-12 |
| BTT_SwitchState.cpp | BTT_SwitchState.cpp | ExecuteTask, AbortTask | 2026-01-03 | 2026-01-12 |
| BTT_SwitchToPreviousState.cpp | BTT_SwitchToPreviousState.cpp | ExecuteTask, AbortTask | 2026-01-03 | 2026-01-12 |
| BTT_TryExecuteAbility.cpp | BTT_TryExecuteAbility.cpp | ExecuteTask, OnAttackEndCallback, CleanupDelegateBinding, AbortTask | 2026-01-03 | 2026-01-12 |
| BTT_TryExecuteAbility.h | BTT_TryExecuteAbility.h | **AB_Soulslike_Enemy, ASLFSoulslikeEnemy, UBTT_TryExecuteAbility** | 2026-01-03 | 2026-01-12 |
| SLFBTTaskSwitchState.cpp | SLFBTTaskSwitchState.cpp | ExecuteTask, GetStaticDescription, Printf | 2026-01-03 | 2026-01-12 |
| BTS_ChaseBounds.h | BTS_ChaseBounds.h | **UBTS_ChaseBounds** | 2026-01-03 | 2026-01-03 |
| BTS_DistanceCheck.cpp | BTS_DistanceCheck.cpp | TickNode | 2026-01-03 | 2026-01-03 |
| BTS_DistanceCheck.h | BTS_DistanceCheck.h | **UBTS_DistanceCheck** | 2026-01-03 | 2026-01-03 |
| BTS_IsTargetDead.h | BTS_IsTargetDead.h | **UBTS_IsTargetDead** | 2026-01-03 | 2026-01-03 |
| BTS_SetMovementModeBasedOnDistance.cpp | BTS_SetMovementModeBasedOnDistance.cpp | TickNode | 2026-01-03 | 2026-01-03 |
| BTS_SetMovementModeBasedOnDistance.h | BTS_SetMovementModeBasedOnDistance.h | **UBTS_SetMovementModeBasedOnDistance** | 2026-01-03 | 2026-01-03 |
| BTS_TryGetAbility.h | BTS_TryGetAbility.h | **UBTS_TryGetAbility** | 2026-01-03 | 2026-01-03 |
| BTT_ClearKey.cpp | BTT_ClearKey.cpp | ExecuteTask, AbortTask | 2026-01-03 | 2026-01-03 |
| BTT_ClearKey.h | BTT_ClearKey.h | **UBTT_ClearKey** | 2026-01-03 | 2026-01-03 |
| BTT_GetCurrentLocation.cpp | BTT_GetCurrentLocation.cpp | ExecuteTask, AbortTask | 2026-01-03 | 2026-01-03 |
| BTT_GetCurrentLocation.h | BTT_GetCurrentLocation.h | **UBTT_GetCurrentLocation** | 2026-01-03 | 2026-01-03 |
| BTT_GetRandomPoint.cpp | BTT_GetRandomPoint.cpp | ExecuteTask, AbortTask | 2026-01-03 | 2026-01-03 |
| BTT_GetRandomPoint.h | BTT_GetRandomPoint.h | **UBTT_GetRandomPoint** | 2026-01-03 | 2026-01-03 |
| BTT_GetRandomPointNearStartPosition.cpp | BTT_GetRandomPointNearStartPosition.cpp | ExecuteTask, AbortTask | 2026-01-03 | 2026-01-03 |
| BTT_GetRandomPointNearStartPosition.h | BTT_GetRandomPointNearStartPosition.h | **UBTT_GetRandomPointNearStartPosition** | 2026-01-03 | 2026-01-03 |
| BTT_GetStrafePointAroundTarget.cpp | BTT_GetStrafePointAroundTarget.cpp | ExecuteTask, AbortTask, SetStrafeLocations, PickStrafeMethodBasedOnScore | 2026-01-03 | 2026-01-03 |
| BTT_GetStrafePointAroundTarget.h | BTT_GetStrafePointAroundTarget.h | **UBTT_GetStrafePointAroundTarget** | 2026-01-03 | 2026-01-03 |
| BTT_PatrolPath.h | BTT_PatrolPath.h | **UBTT_PatrolPath** | 2026-01-03 | 2026-01-03 |
| BTT_SetKey.cpp | BTT_SetKey.cpp | ExecuteTask, AbortTask | 2026-01-03 | 2026-01-03 |
| BTT_SetKey.h | BTT_SetKey.h | **UBTT_SetKey** | 2026-01-03 | 2026-01-03 |
| BTT_SetMovementMode.cpp | BTT_SetMovementMode.cpp | ExecuteTask, AbortTask | 2026-01-03 | 2026-01-03 |
| BTT_SetMovementMode.h | BTT_SetMovementMode.h | **UBTT_SetMovementMode** | 2026-01-03 | 2026-01-03 |
| BTT_SimpleMoveTo.h | BTT_SimpleMoveTo.h | **UBTT_SimpleMoveTo** | 2026-01-03 | 2026-01-03 |
| BTT_SwitchState.h | BTT_SwitchState.h | **UBTT_SwitchState** | 2026-01-03 | 2026-01-03 |
| BTT_SwitchToPreviousState.h | BTT_SwitchToPreviousState.h | **UBTT_SwitchToPreviousState** | 2026-01-03 | 2026-01-03 |
| BTT_ToggleFocus.cpp | BTT_ToggleFocus.cpp | ExecuteTask, AbortTask | 2026-01-03 | 2026-01-03 |
| BTT_ToggleFocus.h | BTT_ToggleFocus.h | **UBTT_ToggleFocus** | 2026-01-03 | 2026-01-03 |
| SLFBTServiceDistanceCheck.cpp | SLFBTServiceDistanceCheck.cpp | TickNode | 2026-01-03 | 2026-01-03 |
| SLFBTServiceDistanceCheck.h | SLFBTServiceDistanceCheck.h | **USLFBTServiceDistanceCheck** | 2026-01-03 | 2026-01-03 |
| SLFBTServiceIsTargetDead.cpp | SLFBTServiceIsTargetDead.cpp | TickNode | 2026-01-03 | 2026-01-03 |
| SLFBTServiceIsTargetDead.h | SLFBTServiceIsTargetDead.h | **USLFBTServiceIsTargetDead** | 2026-01-03 | 2026-01-03 |
| SLFBTTaskGetRandomPoint.cpp | SLFBTTaskGetRandomPoint.cpp | ExecuteTask | 2026-01-03 | 2026-01-03 |
| SLFBTTaskGetRandomPoint.h | SLFBTTaskGetRandomPoint.h | **USLFBTTaskGetRandomPoint** | 2026-01-03 | 2026-01-03 |
| SLFBTTaskSimpleMoveTo.cpp | SLFBTTaskSimpleMoveTo.cpp | ExecuteTask, GetStaticDescription, Printf | 2026-01-03 | 2026-01-03 |
| SLFBTTaskSimpleMoveTo.h | SLFBTTaskSimpleMoveTo.h | **USLFBTTaskSimpleMoveTo** | 2026-01-03 | 2026-01-03 |
| SLFBTTaskSwitchState.h | SLFBTTaskSwitchState.h | **USLFBTTaskSwitchState** | 2026-01-03 | 2026-01-03 |
| SLFBTTaskToggleFocus.cpp | SLFBTTaskToggleFocus.cpp | ExecuteTask | 2026-01-03 | 2026-01-03 |
| SLFBTTaskToggleFocus.h | SLFBTTaskToggleFocus.h | **USLFBTTaskToggleFocus** | 2026-01-03 | 2026-01-03 |
| SLFBTTaskTryExecuteAbility.cpp | SLFBTTaskTryExecuteAbility.cpp | ExecuteTask | 2026-01-03 | 2026-01-03 |
| SLFBTTaskTryExecuteAbility.h | SLFBTTaskTryExecuteAbility.h | **USLFBTTaskTryExecuteAbility** | 2026-01-03 | 2026-01-03 |

## C++ AnimNotifies (`Source/SLFConversion/AnimNotifies`)
**50 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| AN_SpawnProjectile.cpp | AN_SpawnProjectile.cpp | Notify, GetNotifyName_Implementation, Printf | 2026-01-03 | 2026-01-29 |
| ANS_WeaponTrace.cpp | ANS_WeaponTrace.cpp | NotifyBegin, NotifyTick, NotifyEnd, GetNotifyName_Implementation | 2026-01-03 | 2026-01-12 |
| ANS_WeaponTrace.h | ANS_WeaponTrace.h | **UANS_WeaponTrace** | 2026-01-03 | 2026-01-12 |
| AN_SetMovementMode.cpp | AN_SetMovementMode.cpp | Notify, GetNotifyName_Implementation | 2026-01-03 | 2026-01-12 |
| ANS_RegisterAttackSequence.cpp | ANS_RegisterAttackSequence.cpp | GetCombatManager, NotifyBegin, NotifyTick, NotifyEnd, GetNotifyName_Implement... | 2026-01-03 | 2026-01-11 |
| ANS_RegisterAttackSequence.h | ANS_RegisterAttackSequence.h | **UAC_CombatManager, UANS_RegisterAttackSequence** | 2026-01-03 | 2026-01-11 |
| ANS_AI_FistTrace.cpp | ANS_AI_FistTrace.cpp | NotifyBegin, NotifyTick, NotifyEnd, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| ANS_AI_FistTrace.h | ANS_AI_FistTrace.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| ANS_AI_RotateTowardsTarget.cpp | ANS_AI_RotateTowardsTarget.cpp | NotifyBegin, NotifyTick, NotifyEnd, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| ANS_AI_RotateTowardsTarget.h | ANS_AI_RotateTowardsTarget.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| ANS_AI_Trail.cpp | ANS_AI_Trail.cpp | NotifyBegin, NotifyTick, NotifyEnd, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| ANS_AI_Trail.h | ANS_AI_Trail.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| ANS_AI_WeaponTrace.cpp | ANS_AI_WeaponTrace.cpp | NotifyBegin, NotifyTick, NotifyEnd, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| ANS_AI_WeaponTrace.h | ANS_AI_WeaponTrace.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| ANS_FistTrace.cpp | ANS_FistTrace.cpp | NotifyBegin, NotifyTick, NotifyEnd, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| ANS_FistTrace.h | ANS_FistTrace.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| ANS_HyperArmor.cpp | ANS_HyperArmor.cpp | NotifyBegin, NotifyTick, NotifyEnd, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| ANS_HyperArmor.h | ANS_HyperArmor.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| ANS_InputBuffer.cpp | ANS_InputBuffer.cpp | NotifyBegin, NotifyTick, NotifyEnd, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| ANS_InputBuffer.h | ANS_InputBuffer.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| ANS_InvincibilityFrame.cpp | ANS_InvincibilityFrame.cpp | NotifyBegin, NotifyTick, NotifyEnd, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| ANS_InvincibilityFrame.h | ANS_InvincibilityFrame.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| ANS_ToggleChaosField.cpp | ANS_ToggleChaosField.cpp | NotifyBegin, NotifyTick, NotifyEnd, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| ANS_ToggleChaosField.h | ANS_ToggleChaosField.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| ANS_Trail.cpp | ANS_Trail.cpp | NotifyBegin, NotifyTick, NotifyEnd, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| ANS_Trail.h | ANS_Trail.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| AN_AI_SpawnProjectile.cpp | AN_AI_SpawnProjectile.cpp | Notify, GetNotifyName_Implementation, Printf | 2026-01-03 | 2026-01-03 |
| AN_AI_SpawnProjectile.h | AN_AI_SpawnProjectile.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| AN_AdjustStat.cpp | AN_AdjustStat.cpp | Notify, GetNotifyName_Implementation, Printf | 2026-01-03 | 2026-01-03 |
| AN_AdjustStat.h | AN_AdjustStat.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| AN_AoeDamage.cpp | AN_AoeDamage.cpp | Notify, GetNotifyName_Implementation, Printf | 2026-01-03 | 2026-01-03 |
| AN_AoeDamage.h | AN_AoeDamage.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| AN_CameraShake.cpp | AN_CameraShake.cpp | Notify, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| AN_CameraShake.h | AN_CameraShake.h | **UAN_CameraShake** | 2026-01-03 | 2026-01-03 |
| AN_FootstepTrace.cpp | AN_FootstepTrace.cpp | Notify, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| AN_FootstepTrace.h | AN_FootstepTrace.h | **UAN_FootstepTrace** | 2026-01-03 | 2026-01-03 |
| AN_InterruptMontage.cpp | AN_InterruptMontage.cpp | Notify, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| AN_InterruptMontage.h | AN_InterruptMontage.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| AN_LaunchField.cpp | AN_LaunchField.cpp | Notify, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| AN_LaunchField.h | AN_LaunchField.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| AN_PlayCameraSequence.cpp | AN_PlayCameraSequence.cpp | Notify, GetNotifyName_Implementation, Printf | 2026-01-03 | 2026-01-03 |
| AN_PlayCameraSequence.h | AN_PlayCameraSequence.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| AN_SetAiState.cpp | AN_SetAiState.cpp | Notify, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| AN_SetAiState.h | AN_SetAiState.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| AN_SetMovementMode.h | AN_SetMovementMode.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| AN_SpawnProjectile.h | AN_SpawnProjectile.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| AN_TryGuard.cpp | AN_TryGuard.cpp | Notify, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| AN_TryGuard.h | AN_TryGuard.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |
| AN_WorldCameraShake.cpp | AN_WorldCameraShake.cpp | Notify, GetNotifyName_Implementation | 2026-01-03 | 2026-01-03 |
| AN_WorldCameraShake.h | AN_WorldCameraShake.h | **AB_BaseProjectile, AB_Interactable, UB_Action** | 2026-01-03 | 2026-01-03 |

## C++ Animation (`Source/SLFConversion/Animation`)
**52 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| SLFAnimNotifyStateTelegraph.cpp | SLFAnimNotifyStateTelegraph.cpp | GetNotifyName_Implementation, NotifyBegin, NotifyEnd | 2026-03-16 | 2026-03-16 |
| SLFAnimNotifyStateTelegraph.h | SLFAnimNotifyStateTelegraph.h | **USLFAnimNotifyStateTelegraph, UNiagaraSystem, UNiagaraComponent** | 2026-03-16 | 2026-03-16 |
| SLFAnimNotifyStateWeaponTrace.cpp | SLFAnimNotifyStateWeaponTrace.cpp | GetNotifyName_Implementation, NotifyBegin, NotifyTick, NotifyEnd | 2026-01-03 | 2026-03-16 |
| SLFAnimNotifyStateWeaponTrace.h | SLFAnimNotifyStateWeaponTrace.h | **USLFAnimNotifyStateWeaponTrace, UNiagaraSystem, UNiagaraComponent** | 2026-01-03 | 2026-03-16 |
| ABP_SoulslikeEnemy.cpp | ABP_SoulslikeEnemy.cpp | NativeInitializeAnimation, NativeUpdateAnimation, GetOwnerVelocity, GetOwnerR... | 2026-01-03 | 2026-02-07 |
| ABP_SoulslikeEnemy.h | ABP_SoulslikeEnemy.h | **UAnimMontage, ACharacter, UCharacterMovementComponent** | 2026-01-03 | 2026-02-07 |
| ABP_SoulslikeBossNew.cpp | ABP_SoulslikeBossNew.cpp | NativeInitializeAnimation, NativeUpdateAnimation, GetOwnerVelocity, GetOwnerR... | 2026-01-03 | 2026-01-30 |
| ABP_SoulslikeBossNew.h | ABP_SoulslikeBossNew.h | **UAnimMontage, ACharacter, UCharacterMovementComponent** | 2026-01-03 | 2026-01-30 |
| ABP_SoulslikeCharacter_Additive.cpp | C++ AnimInstance implementation for ABP_SoulslikeCharacter_Additive | NativeInitializeAnimation, NativeUpdateAnimation, GetOwnerVelocity, GetOwnerR... | 2026-01-03 | 2026-01-29 |
| ABP_SoulslikeCharacter_Additive.h | C++ AnimInstance for ABP_SoulslikeCharacter_Additive | **UAnimMontage, ACharacter, UAC_ActionManager** | 2026-01-03 | 2026-01-13 |
| ABP_SoulslikeNPC.cpp | ABP_SoulslikeNPC.cpp | NativeInitializeAnimation, NativeUpdateAnimation, GetOwnerVelocity, GetOwnerR... | 2026-01-03 | 2026-01-11 |
| ABP_SoulslikeNPC.h | ABP_SoulslikeNPC.h | **UAnimMontage, ACharacter, UCharacterMovementComponent** | 2026-01-03 | 2026-01-11 |
| ABP_Manny_PostProcess.cpp | ABP_Manny_PostProcess.cpp | NativeInitializeAnimation, NativeUpdateAnimation, GetOwnerVelocity, GetOwnerR... | 2026-01-03 | 2026-01-07 |
| ABP_Quinn_PostProcess.cpp | ABP_Quinn_PostProcess.cpp | NativeInitializeAnimation, NativeUpdateAnimation, GetOwnerVelocity, GetOwnerR... | 2026-01-03 | 2026-01-07 |
| SLFAnimNotifyAdjustStat.cpp | SLFAnimNotifyAdjustStat.cpp | GetNotifyName_Implementation, Printf, Notify | 2026-01-03 | 2026-01-07 |
| SLFAnimNotifyAoeDamage.cpp | SLFAnimNotifyAoeDamage.cpp | GetNotifyName_Implementation, Printf, Notify | 2026-01-03 | 2026-01-07 |
| SLFAnimNotifyFootstepTrace.cpp | SLFAnimNotifyFootstepTrace.cpp | GetNotifyName_Implementation, Printf, Notify | 2026-01-03 | 2026-01-07 |
| SLFAnimNotifySetAIState.cpp | SLFAnimNotifySetAIState.cpp | GetNotifyName_Implementation, Printf, Notify | 2026-01-03 | 2026-01-07 |
| SLFAnimNotifyStateAIRotateToTarget.cpp | SLFAnimNotifyStateAIRotateToTarget.cpp | GetNotifyName_Implementation, NotifyTick | 2026-01-03 | 2026-01-07 |
| SLFAnimNotifyStateFistTrace.cpp | SLFAnimNotifyStateFistTrace.cpp | GetNotifyName_Implementation, Printf, NotifyBegin, NotifyTick, NotifyEnd | 2026-01-03 | 2026-01-07 |
| SLFAnimNotifyStateFistTrace.h | SLFAnimNotifyStateFistTrace.h | **USLFAnimNotifyStateFistTrace** | 2026-01-03 | 2026-01-07 |
| SLFAnimNotifyStateHyperArmor.cpp | SLFAnimNotifyStateHyperArmor.cpp | GetNotifyName_Implementation, NotifyBegin, NotifyEnd | 2026-01-03 | 2026-01-07 |
| SLFAnimNotifyStateInputBuffer.cpp | SLFAnimNotifyStateInputBuffer.cpp | GetNotifyName_Implementation, NotifyBegin, NotifyEnd | 2026-01-03 | 2026-01-07 |
| SLFAnimNotifyStateInvincibility.cpp | SLFAnimNotifyStateInvincibility.cpp | GetNotifyName_Implementation, NotifyBegin, NotifyEnd | 2026-01-03 | 2026-01-07 |
| SLFAnimNotifyStateTrail.cpp | SLFAnimNotifyStateTrail.cpp | GetNotifyName_Implementation, NotifyBegin, NotifyEnd | 2026-01-03 | 2026-01-07 |
| SLFAnimNotifyTryGuard.cpp | SLFAnimNotifyTryGuard.cpp | GetNotifyName_Implementation, Notify | 2026-01-03 | 2026-01-07 |
| SLFBossAnimInstance.cpp | SLFBossAnimInstance.cpp | NativeInitializeAnimation, NativeUpdateAnimation, CacheReferences, UpdateAnim... | 2026-01-03 | 2026-01-07 |
| SLFBossAnimInstance.h | SLFBossAnimInstance.h | **UCharacterMovementComponent, UAICombatManagerComponent, UDataAsset** | 2026-01-03 | 2026-01-07 |
| SLFEnemyAnimInstance.cpp | SLFEnemyAnimInstance.cpp | NativeInitializeAnimation, NativeUpdateAnimation, CacheReferences, UpdateAnim... | 2026-01-03 | 2026-01-07 |
| SLFEnemyAnimInstance.h | SLFEnemyAnimInstance.h | **UCharacterMovementComponent, UAICombatManagerComponent, UEquipmentManagerCo... | 2026-01-03 | 2026-01-07 |
| ABP_Manny_PostProcess.h | ABP_Manny_PostProcess.h | **UAnimMontage, ACharacter, UABP_Manny_PostProcess** | 2026-01-03 | 2026-01-03 |
| ABP_Quinn_PostProcess.h | ABP_Quinn_PostProcess.h | **UAnimMontage, ACharacter, UABP_Quinn_PostProcess** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifyAdjustStat.h | SLFAnimNotifyAdjustStat.h | **USLFAnimNotifyAdjustStat** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifyAoeDamage.h | SLFAnimNotifyAoeDamage.h | **USLFAnimNotifyAoeDamage** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifyCameraShake.cpp | SLFAnimNotifyCameraShake.cpp | GetNotifyName_Implementation, Notify | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifyCameraShake.h | SLFAnimNotifyCameraShake.h | **USLFAnimNotifyCameraShake** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifyFootstepTrace.h | SLFAnimNotifyFootstepTrace.h | **USLFAnimNotifyFootstepTrace** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifyInterruptMontage.cpp | SLFAnimNotifyInterruptMontage.cpp | GetNotifyName_Implementation, Notify | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifyInterruptMontage.h | SLFAnimNotifyInterruptMontage.h | **USLFAnimNotifyInterruptMontage** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifySetAIState.h | SLFAnimNotifySetAIState.h | **USLFAnimNotifySetAIState** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifySetMovementMode.cpp | SLFAnimNotifySetMovementMode.cpp | GetNotifyName_Implementation, Notify | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifySetMovementMode.h | SLFAnimNotifySetMovementMode.h | **USLFAnimNotifySetMovementMode** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifySpawnProjectile.cpp | SLFAnimNotifySpawnProjectile.cpp | GetNotifyName_Implementation, Notify | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifySpawnProjectile.h | SLFAnimNotifySpawnProjectile.h | **USLFAnimNotifySpawnProjectile** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifyStateAIRotateToTarget.h | SLFAnimNotifyStateAIRotateToTarget.h | **USLFAnimNotifyStateAIRotateToTarget** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifyStateHyperArmor.h | SLFAnimNotifyStateHyperArmor.h | **USLFAnimNotifyStateHyperArmor** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifyStateInputBuffer.h | SLFAnimNotifyStateInputBuffer.h | **USLFAnimNotifyStateInputBuffer** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifyStateInvincibility.h | SLFAnimNotifyStateInvincibility.h | **USLFAnimNotifyStateInvincibility** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifyStateTrail.h | SLFAnimNotifyStateTrail.h | **USLFAnimNotifyStateTrail** | 2026-01-03 | 2026-01-03 |
| SLFAnimNotifyTryGuard.h | SLFAnimNotifyTryGuard.h | **USLFAnimNotifyTryGuard** | 2026-01-03 | 2026-01-03 |
| SLFNPCAnimInstance.cpp | SLFNPCAnimInstance.cpp | NativeInitializeAnimation, NativeUpdateAnimation, CacheReferences, UpdateAnim... | 2026-01-03 | 2026-01-03 |
| SLFNPCAnimInstance.h | SLFNPCAnimInstance.h | **UCharacterMovementComponent, UAICombatManagerComponent, UEquipmentManagerCo... | 2026-01-03 | 2026-01-03 |

## C++ Blueprints (`Source/SLFConversion/Blueprints`)
**570 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| SLFEnemyGeneric.cpp | SLFEnemyGeneric.cpp | BeginPlay, ApplyEnemyConfig, Tick, ConfigureAbilities, TrySpawnBodyVFX | 2026-03-15 | 2026-03-16 |
| SLFEnemySentinel.cpp | SLFEnemySentinel.cpp | BeginPlay, ApplySentinelConfig, Tick, ConfigureAbilities, FixMontageDurations | 2026-02-21 | 2026-03-16 |
| SLFEnemyGeneric.h | SLFEnemyGeneric.h | **UNavigationInvokerComponent, UNiagaraComponent, UNiagaraSystem** | 2026-03-15 | 2026-03-15 |
| SLFEnemySentinel.h | SLFEnemySentinel.h | **UNavigationInvokerComponent, ASLFEnemySentinel** | 2026-01-03 | 2026-03-09 |
| SLFRestingPointBase.cpp | SLFRestingPointBase.cpp | BeginPlay, OnConstruction, GetReplenishData_Implementation, OnInteract_Implem... | 2026-01-03 | 2026-03-09 |
| SLFRestingPointBase.h | SLFRestingPointBase.h | **UAnimMontage, UDataAsset, ASLFRestingPointBase** | 2026-01-03 | 2026-03-09 |
| SLFSoulslikeCharacter.cpp | SLFSoulslikeCharacter.cpp | BeginPlay, Tick, OnStartCrouch, OnEndCrouch, Landed | 2026-01-03 | 2026-03-09 |
| SLFSoulslikeCharacter.h | SLFSoulslikeCharacter.h | **UInputMappingContext, UInputAction, UDataAsset** | 2026-01-03 | 2026-03-09 |
| Actions/SLFActionBackstab.cpp | SLFActionBackstab.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-03-09 |
| Actions/SLFActionDoubleJump.cpp | SLFActionDoubleJump.cpp | CanExecuteAction_Implementation, ExecuteAction_Implementation | 2026-03-09 | 2026-03-09 |
| Actions/SLFActionDoubleJump.h | SLFActionDoubleJump.h | **USLFActionDoubleJump** | 2026-03-09 | 2026-03-09 |
| Actions/SLFActionGrapple.cpp | SLFActionGrapple.cpp | FindBestGrapplePoint, CanExecuteAction_Implementation, ExecuteAction_Implemen... | 2026-03-09 | 2026-03-09 |
| Actions/SLFActionGrapple.h | SLFActionGrapple.h | **ASLFGrapplePoint, USLFActionGrapple** | 2026-03-09 | 2026-03-09 |
| Actions/SLFActionGuardCounter.cpp | SLFActionGuardCounter.cpp | OpenCounterWindow, CloseCounterWindow, CanExecuteAction_Implementation, Execu... | 2026-03-09 | 2026-03-09 |
| Actions/SLFActionGuardCounter.h | SLFActionGuardCounter.h | **USLFActionGuardCounter** | 2026-03-09 | 2026-03-09 |
| Actions/SLFActionJump.cpp | SLFActionJump.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-03-09 |
| Actions/SLFActionMantle.cpp | SLFActionMantle.cpp | DetectLedge, CanExecuteAction_Implementation, ExecuteAction_Implementation | 2026-03-09 | 2026-03-09 |
| Actions/SLFActionMantle.h | SLFActionMantle.h | **USLFActionMantle**; UMETA | 2026-03-09 | 2026-03-09 |
| Actions/SLFActionSlide.cpp | SLFActionSlide.cpp | CanExecuteAction_Implementation, ExecuteAction_Implementation, EndSlide, Clea... | 2026-03-09 | 2026-03-09 |
| Actions/SLFActionSlide.h | SLFActionSlide.h | **USLFActionSlide** | 2026-03-09 | 2026-03-09 |
| Actions/SLFActionSwim.cpp | SLFActionSwim.cpp | CanExecuteAction_Implementation, ExecuteAction_Implementation, EnterWater, Ex... | 2026-03-09 | 2026-03-09 |
| Actions/SLFActionSwim.h | SLFActionSwim.h | **USLFActionSwim** | 2026-03-09 | 2026-03-09 |
| Actors/SLFBossDoor.cpp | SLFBossDoor.cpp | BeginPlay, SetFogGateCollision, OnTraced_Implementation, OnInteract_Implement... | 2026-01-03 | 2026-03-09 |
| Actors/SLFBossDoor.h | SLFBossDoor.h | **ASLFBossDoor** | 2026-01-03 | 2026-03-09 |
| Actors/SLFBossPhaseObjective.cpp | SLFBossPhaseObjective.cpp | BeginPlay, TakeDamage, DestroyObjective | 2026-03-09 | 2026-03-09 |
| Actors/SLFBossPhaseObjective.h | SLFBossPhaseObjective.h | **ASLFBossPhaseObjective**; TakeDamage | 2026-03-09 | 2026-03-09 |
| Actors/SLFGrapplePoint.cpp | SLFGrapplePoint.cpp | BeginPlay, GetLandingLocation, IsInRange, Dist | 2026-03-09 | 2026-03-09 |
| Actors/SLFGrapplePoint.h | SLFGrapplePoint.h | **ASLFGrapplePoint** | 2026-03-09 | 2026-03-09 |
| Actors/SLFPuzzleMarker.cpp | SLFPuzzleMarker.cpp | BeginPlay, CompleteObjective | 2026-03-09 | 2026-03-09 |
| Actors/SLFPuzzleMarker.h | SLFPuzzleMarker.h | **ASLFPuzzleMarker** | 2026-03-09 | 2026-03-09 |
| Actors/SLFShortcutGate.cpp | SLFShortcutGate.cpp | BeginPlay, IsOnOpenSide, OpenGate, OnInteract_Implementation | 2026-03-09 | 2026-03-09 |
| Actors/SLFShortcutGate.h | SLFShortcutGate.h | **ASLFShortcutGate** | 2026-03-09 | 2026-03-09 |
| Actors/SLFTrapBase.cpp | SLFTrapBase.cpp | BeginPlay, OnTriggerOverlap, ActivateTrap_Implementation, ResetTrap_Implement... | 2026-03-09 | 2026-03-09 |
| Actors/SLFTrapBase.h | SLFTrapBase.h | **ASLFTrapBase**; OnTriggerOverlap | 2026-03-09 | 2026-03-09 |
| B_RestingPoint.cpp | B_RestingPoint.cpp | BeginPlay, GetReplenishData_Implementation, GetRestingPointSpawnPosition_Impl... | 2026-01-03 | 2026-02-27 |
| SLFNPCKael.cpp | SLFNPCKael.cpp | BeginPlay, GetUpgradeCost, CanUpgradeWeapon, OnInteract_Implementation, OnTra... | 2026-02-27 | 2026-02-27 |
| SLFNPCKael.h | SLFNPCKael.h | **ASLFNPCKael** | 2026-02-27 | 2026-02-27 |
| SLFNPCVara.cpp | SLFNPCVara.cpp | BeginPlay, UpdateLocationFromProgress, GetContextualDialog_Implementation, On... | 2026-02-27 | 2026-02-27 |
| SLFNPCVara.h | SLFNPCVara.h | **ASLFNPCVara** | 2026-02-27 | 2026-02-27 |
| Actors/SLFBossEncounter.cpp | SLFBossEncounter.cpp | BeginPlay, Tick, StartEncounter, OnBossDefeated, TransitionToPhase | 2026-02-27 | 2026-02-27 |
| Actors/SLFBossEncounter.h | SLFBossEncounter.h | **UBoxComponent, UStaticMeshComponent, ASLFBossEncounter**; OnFogGateOverlap | 2026-02-27 | 2026-02-27 |
| Actors/SLFEnemySpawnPoint.cpp | SLFEnemySpawnPoint.cpp | BeginPlay, SpawnEnemy, DespawnEnemy, OnPlayerRested, IsEnemyAlive | 2026-02-27 | 2026-02-27 |
| Actors/SLFEnemySpawnPoint.h | SLFEnemySpawnPoint.h | **USphereComponent, UBillboardComponent, ASLFSoulslikeEnemy** | 2026-02-27 | 2026-02-27 |
| Actors/SLFLevelStreamManager.cpp | SLFLevelStreamManager.cpp | BeginPlay, Tick, OnTriggerOverlap, EnterDungeon, ExitDungeon | 2026-02-27 | 2026-02-27 |
| Actors/SLFLevelStreamManager.h | SLFLevelStreamManager.h | **UBoxComponent, ULevelStreamingDynamic, ASLFLevelStreamManager**; OnTriggerO... | 2026-02-27 | 2026-02-27 |
| Actors/SLFZoneTrigger.cpp | SLFZoneTrigger.cpp | BeginPlay, OnPlayerEnterZone | 2026-02-27 | 2026-02-27 |
| Actors/SLFZoneTrigger.h | SLFZoneTrigger.h | **UBoxComponent, ASLFZoneTrigger**; OnPlayerEnterZone | 2026-02-27 | 2026-02-27 |
| SLFEnemyGuard.cpp | SLFEnemyGuard.cpp | BeginPlay, ApplyC3100Config, Tick, ConfigureAbilities, ApplyMontageLocomotion... | 2026-01-03 | 2026-02-18 |
| SLFEnemyGuard.h | SLFEnemyGuard.h | **ASLFEnemyGuard** | 2026-01-03 | 2026-02-18 |
| B_PickupItem.cpp | B_PickupItem.cpp | TryGetItemInfo_Implementation, OnInteract_Implementation, EndPlay | 2026-01-03 | 2026-02-07 |
| B_PickupItem.h | B_PickupItem.h | **UAnimMontage, UDataTable, AB_PickupItem** | 2026-01-03 | 2026-02-07 |
| B_SequenceActor.cpp | B_SequenceActor.cpp | BeginPlay, EndPlay, SetupInputBindings, OnAnyKeyInput, OnGameMenuInput | 2026-01-03 | 2026-02-07 |
| B_SequenceActor.h | B_SequenceActor.h | **UInputAction, UInputMappingContext, AB_SequenceActor** | 2026-01-03 | 2026-02-07 |
| B_Soulslike_Character.cpp | B_Soulslike_Character.cpp | HandleTargetLock_Implementation, GetAxisValue_Implementation, MakeModularMesh... | 2026-01-03 | 2026-02-07 |
| B_Soulslike_Character.h | B_Soulslike_Character.h | **UAnimMontage, UDataTable, UAIInteractionManagerComponent** | 2026-01-03 | 2026-02-07 |
| B_StatusEffect.cpp | B_StatusEffect.cpp | GetOwnerStatManager_Implementation, TryAdjustOwnerStat, GetBuildupPercent_Imp... | 2026-01-03 | 2026-02-07 |
| SLFNPCShowcaseGuide.cpp | SLFNPCShowcaseGuide.cpp | BeginPlay | 2026-01-03 | 2026-02-07 |
| SLFNPCShowcaseGuide.h | SLFNPCShowcaseGuide.h | **ASLFNPCShowcaseGuide** | 2026-01-03 | 2026-02-07 |
| SLFPickupItemBase.cpp | SLFPickupItemBase.cpp | BeginPlay, SetupWorldNiagara, TryGetItemInfo_Implementation, TriggerOnItemLoo... | 2026-01-03 | 2026-02-07 |
| SLFSoulslikeNPC.cpp | SLFSoulslikeNPC.cpp | BeginPlay, GetLookAtLocation_Implementation, TeleportToLocation_Implementatio... | 2026-01-03 | 2026-02-07 |
| SLFSoulslikeNPC.h | SLFSoulslikeNPC.h | **UPrimitiveComponent, USphereComponent, UAIInteractionManagerComponent** | 2026-01-03 | 2026-02-07 |
| SLFNPCShowcaseVendor.cpp | SLFNPCShowcaseVendor.cpp | BeginPlay | 2026-01-03 | 2026-02-01 |
| SLFNPCShowcaseVendor.h | SLFNPCShowcaseVendor.h | **ASLFNPCShowcaseVendor** | 2026-01-03 | 2026-02-01 |
| SLFSoulslikeBoss.cpp | SLFSoulslikeBoss.cpp | BeginPlay, OnTriggerBeginOverlap | 2026-01-03 | 2026-01-30 |
| SLFSoulslikeBoss.h | SLFSoulslikeBoss.h | **UAIBossComponent, UAC_AI_CombatManager, ASLFSoulslikeBoss**; OnTriggerBegin... | 2026-01-03 | 2026-01-30 |
| B_Action_ScrollWheel_Tools.cpp | B_Action_ScrollWheel_Tools.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-29 |
| B_Action_WeaponAbility.cpp | B_Action_WeaponAbility.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-29 |
| SLFBaseCharacter.cpp | SLFBaseCharacter.cpp | BeginPlay, Tick, SetupPlayerInputComponent, PlaySoftSoundAtLocation_Implement... | 2026-01-03 | 2026-01-29 |
| SLFSpellProjectile.cpp | SLFSpellProjectile.cpp | BeginPlay, Tick, SetupBlackholeEffect, UpdatePulseEffect, GetElementColor | 2026-01-29 | 2026-01-29 |
| SLFSpellProjectile.h | SLFSpellProjectile.h | **UStaticMeshComponent, UMaterialInstanceDynamic, UPointLightComponent**; UMETA | 2026-01-29 | 2026-01-29 |
| SLFWeaponBase.cpp | SLFWeaponBase.cpp | BeginPlay, OnWeaponEquip_Implementation, OnWeaponUnequip_Implementation, OnUs... | 2026-01-03 | 2026-01-29 |
| Actions/SLFActionScrollWheelTools.cpp | SLFActionScrollWheelTools.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-29 |
| Actions/SLFActionThrowProjectile.cpp | SLFActionThrowProjectile.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-29 |
| Actions/SLFActionUseEquippedTool.cpp | SLFActionUseEquippedTool.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-29 |
| Actions/SLFActionWeaponAbility.cpp | SLFActionWeaponAbility.cpp | ExecuteAction_Implementation, BeginSpecialAttack | 2026-01-03 | 2026-01-29 |
| Actions/SLFActionWeaponAbility.h | SLFActionWeaponAbility.h | **UPDA_WeaponAbility, USLFActionWeaponAbility** | 2026-01-03 | 2026-01-29 |
| Spells/SLFAOESpell.cpp | SLFAOESpell.cpp | BeginPlay, Tick, SetupAOEEffect, UpdateExpansion | 2026-01-29 | 2026-01-29 |
| Spells/SLFAOESpell.h | SLFAOESpell.h | **UStaticMeshComponent, UMaterialInstanceDynamic, UPointLightComponent** | 2026-01-29 | 2026-01-29 |
| Spells/SLFArcaneMissileProjectile.cpp | SLFArcaneMissileProjectile.cpp | BeginPlay, Tick, SetupArcaneEffect, UpdateMagicalEffect | 2026-01-29 | 2026-01-29 |
| Spells/SLFArcaneMissileProjectile.h | SLFArcaneMissileProjectile.h | **UStaticMeshComponent, UMaterialInstanceDynamic, UPointLightComponent** | 2026-01-29 | 2026-01-29 |
| Spells/SLFBeamSpell.cpp | SLFBeamSpell.cpp | BeginPlay, Tick, SetupBeamEffect, UpdateBeamEffect | 2026-01-29 | 2026-01-29 |
| Spells/SLFBeamSpell.h | SLFBeamSpell.h | **UStaticMeshComponent, UMaterialInstanceDynamic, UPointLightComponent** | 2026-01-29 | 2026-01-29 |
| Spells/SLFFireballProjectile.cpp | SLFFireballProjectile.cpp | BeginPlay, Tick, SetupFireEffect, UpdateFlickerEffect | 2026-01-29 | 2026-01-29 |
| Spells/SLFFireballProjectile.h | SLFFireballProjectile.h | **UStaticMeshComponent, UMaterialInstanceDynamic, UPointLightComponent** | 2026-01-29 | 2026-01-29 |
| Spells/SLFHealingSpell.cpp | SLFHealingSpell.cpp | BeginPlay, Tick, SetupHealingEffect, UpdateHealingEffect | 2026-01-29 | 2026-01-29 |
| Spells/SLFHealingSpell.h | SLFHealingSpell.h | **UStaticMeshComponent, UMaterialInstanceDynamic, UPointLightComponent** | 2026-01-29 | 2026-01-29 |
| Spells/SLFHolyOrbProjectile.cpp | SLFHolyOrbProjectile.cpp | BeginPlay, Tick, SetupHolyEffect, UpdateRadianceEffect | 2026-01-29 | 2026-01-29 |
| Spells/SLFHolyOrbProjectile.h | SLFHolyOrbProjectile.h | **UStaticMeshComponent, UMaterialInstanceDynamic, UPointLightComponent** | 2026-01-29 | 2026-01-29 |
| Spells/SLFIceShardProjectile.cpp | SLFIceShardProjectile.cpp | BeginPlay, Tick, SetupIceEffect, UpdateShimmerEffect | 2026-01-29 | 2026-01-29 |
| Spells/SLFIceShardProjectile.h | SLFIceShardProjectile.h | **UStaticMeshComponent, UMaterialInstanceDynamic, UPointLightComponent** | 2026-01-29 | 2026-01-29 |
| Spells/SLFLightningBoltProjectile.cpp | SLFLightningBoltProjectile.cpp | BeginPlay, Tick, SetupLightningEffect, UpdateElectricEffect | 2026-01-29 | 2026-01-29 |
| Spells/SLFLightningBoltProjectile.h | SLFLightningBoltProjectile.h | **UStaticMeshComponent, UMaterialInstanceDynamic, UPointLightComponent** | 2026-01-29 | 2026-01-29 |
| Spells/SLFPoisonBlobProjectile.cpp | SLFPoisonBlobProjectile.cpp | BeginPlay, Tick, SetupPoisonEffect, UpdateBubblingEffect | 2026-01-29 | 2026-01-29 |
| Spells/SLFPoisonBlobProjectile.h | SLFPoisonBlobProjectile.h | **UStaticMeshComponent, UMaterialInstanceDynamic, UPointLightComponent** | 2026-01-29 | 2026-01-29 |
| Spells/SLFShieldSpell.cpp | SLFShieldSpell.cpp | BeginPlay, Tick, SetupShieldEffect, UpdateShieldEffect | 2026-01-29 | 2026-01-29 |
| Spells/SLFShieldSpell.h | SLFShieldSpell.h | **UStaticMeshComponent, UMaterialInstanceDynamic, UPointLightComponent** | 2026-01-29 | 2026-01-29 |
| Spells/SLFSummoningCircle.cpp | SLFSummoningCircle.cpp | BeginPlay, Tick, SetupCircleEffect, UpdateCircleEffect | 2026-01-29 | 2026-01-29 |
| Spells/SLFSummoningCircle.h | SLFSummoningCircle.h | **UStaticMeshComponent, UMaterialInstanceDynamic, UPointLightComponent** | 2026-01-29 | 2026-01-29 |
| B_BaseProjectile.cpp | B_BaseProjectile.cpp | BeginPlay, CacheComponentReferences, OnTriggerOverlap, InitializeProjectile_I... | 2026-01-03 | 2026-01-28 |
| B_BaseProjectile.h | B_BaseProjectile.h | **UAnimMontage, UDataTable, UProjectileMovementComponent**; OnTriggerOverlap | 2026-01-03 | 2026-01-28 |
| SLFProjectileBase.cpp | SLFProjectileBase.cpp | BeginPlay, Tick, SetupProjectile_Implementation, OnProjectileHit_Implementati... | 2026-01-03 | 2026-01-28 |
| SLFProjectileBase.h | SLFProjectileBase.h | **UNiagaraSystem, UNiagaraComponent, UProjectileMovementComponent**; OnTrigge... | 2026-01-03 | 2026-01-28 |
| B_Action.cpp | B_Action.cpp | GetEquipmentManager_Implementation, GetInteractionManager_Implementation, Get... | 2026-01-03 | 2026-01-27 |
| B_Action.h | B_Action.h | **UAnimMontage, UDataTable, UAC_EquipmentManager** | 2026-01-03 | 2026-01-27 |
| B_Action_DrinkFlask_HP.cpp | B_Action_DrinkFlask_HP.cpp | ExecuteAction_Implementation, GetChangeAmountFromPercent_Implementation | 2026-01-03 | 2026-01-27 |
| B_Action_DrinkFlask_HP.h | B_Action_DrinkFlask_HP.h | **UAnimMontage, UDataTable, UB_Action_DrinkFlask_HP** | 2026-01-03 | 2026-01-27 |
| B_Action_ScrollWheel_LeftHand.cpp | B_Action_ScrollWheel_LeftHand.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-27 |
| B_Action_ScrollWheel_RightHand.cpp | B_Action_ScrollWheel_RightHand.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-27 |
| SLFActionBase.cpp | SLFActionBase.cpp | GetEquipmentManager_Implementation, GetInteractionManager_Implementation, Get... | 2026-01-03 | 2026-01-27 |
| SLFActionBase.h | SLFActionBase.h | **UDataAsset, UAnimInstance, UStatManagerComponent** | 2026-01-03 | 2026-01-27 |
| Actions/SLFActionDrinkFlaskHP.cpp | SLFActionDrinkFlaskHP.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-27 |
| Actions/SLFActionScrollWheelLeftHand.cpp | SLFActionScrollWheelLeftHand.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-27 |
| Actions/SLFActionScrollWheelRightHand.cpp | SLFActionScrollWheelRightHand.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-27 |
| AIC_SoulslikeFramework.cpp | AIC_SoulslikeFramework.cpp | BeginPlay, OnPossess, Tick, InitializeBehavior_Implementation | 2026-01-03 | 2026-01-26 |
| B_Action_PickupItemMontage.cpp | B_Action_PickupItemMontage.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-26 |
| B_Chaos_ForceField.h | B_Chaos_ForceField.h | **UStaticMeshComponent, USkeletalMeshComponent, USphereComponent** | 2026-01-03 | 2026-01-26 |
| B_Container.cpp | B_Container.cpp | BeginPlay, OnConstruction, OnInteract_Implementation, OpenContainer_Implement... | 2026-01-03 | 2026-01-26 |
| B_Container.h | B_Container.h | **UAC_LootDropManager, AB_PickupItem, AB_Container** | 2026-01-03 | 2026-01-26 |
| B_DeathCurrency.cpp | B_DeathCurrency.cpp | BeginPlay, OnInteract_Implementation, OnSpawnedFromSave_Implementation | 2026-01-03 | 2026-01-26 |
| B_DeathCurrency.h | B_DeathCurrency.h | **UAnimMontage, UDataTable, AB_DeathCurrency** | 2026-01-03 | 2026-01-26 |
| B_Destructible.cpp | B_Destructible.cpp | BeginPlay, OnConstruction, OnChaosBreakEvent | 2026-01-03 | 2026-01-26 |
| B_Interactable.cpp | B_Interactable.cpp | BeginPlay, AddInteractableStateToSaveData_Implementation, AddSpawnedInteracta... | 2026-01-03 | 2026-01-26 |
| B_Interactable.h | B_Interactable.h | **UAnimMontage, UDataTable, AB_Interactable** | 2026-01-03 | 2026-01-26 |
| B_Ladder.cpp | B_Ladder.cpp | OnConstruction, GetPoleHeight_Implementation, CreateLadder_Implementation, Be... | 2026-01-03 | 2026-01-26 |
| B_RestingPoint.h | B_RestingPoint.h | **UAnimMontage, UDataTable, AB_RestingPoint** | 2026-01-03 | 2026-01-26 |
| B_StatusEffect.h | B_StatusEffect.h | **UAC_StatManager, UPrimaryDataAsset, UNiagaraComponent** | 2026-01-03 | 2026-01-26 |
| B_StatusEffectArea.cpp | B_StatusEffectArea.cpp | BeginPlay, OnBoxBeginOverlap, OnBoxEndOverlap | 2026-01-03 | 2026-01-26 |
| B_StatusEffectArea.h | B_StatusEffectArea.h | **UPrimaryDataAsset, UAC_StatusEffectManager, AB_StatusEffectArea**; OnBoxBeg... | 2026-01-03 | 2026-01-26 |
| SLFBaseCharacter.h | SLFBaseCharacter.h | **UStatManagerComponent, UStatusEffectManagerComponent, UAC_StatusEffectManag... | 2026-01-03 | 2026-01-26 |
| SLFLadderBase.cpp | SLFLadderBase.cpp | OnConstruction, BeginPlay, GetPoleHeight_Implementation, CreateLadder_Impleme... | 2026-01-03 | 2026-01-26 |
| SLFLadderBase.h | SLFLadderBase.h | **ULadderManagerComponent, ASLFLadderBase**; OnClimbingCollisionEndOverlap, O... | 2026-01-03 | 2026-01-26 |
| SLFSoulslikeEnemy.cpp | SLFSoulslikeEnemy.cpp | BeginPlay, Tick, UpdateAnimationVariables, SetupPerceptionCallbacks, OnPercep... | 2026-01-03 | 2026-01-26 |
| SLFSoulslikeEnemy.h | SLFSoulslikeEnemy.h | **UAICombatManagerComponent, UAIBehaviorManagerComponent, ULootDropManagerCom... | 2026-01-03 | 2026-01-26 |
| SLFWeaponBase.h | SLFWeaponBase.h | **UNiagaraSystem, UNiagaraComponent, USoundBase** | 2026-01-03 | 2026-01-26 |
| Actions/SLFActionPickupItemMontage.cpp | SLFActionPickupItemMontage.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-26 |
| Actors/SLFInteractableBase.cpp | SLFInteractableBase.cpp | BeginPlay, Interact_Implementation, CanBeInteractedWith_Implementation, Enabl... | 2026-01-03 | 2026-01-26 |
| Actors/SLFInteractableBase.h | SLFInteractableBase.h | **ASLFInteractableBase** | 2026-01-03 | 2026-01-26 |
| Stats/SLFAttackPowerFire.cpp | SLFAttackPowerFire.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFAttackPowerFrost.cpp | SLFAttackPowerFrost.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFAttackPowerHoly.cpp | SLFAttackPowerHoly.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFAttackPowerLightning.cpp | SLFAttackPowerLightning.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFAttackPowerMagic.cpp | SLFAttackPowerMagic.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFAttackPowerPhysical.cpp | SLFAttackPowerPhysical.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFDamageNegationFire.cpp | SLFDamageNegationFire.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFDamageNegationFrost.cpp | SLFDamageNegationFrost.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFDamageNegationHoly.cpp | SLFDamageNegationHoly.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFDamageNegationLightning.cpp | SLFDamageNegationLightning.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFDamageNegationMagic.cpp | SLFDamageNegationMagic.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFDamageNegationPhysical.cpp | SLFDamageNegationPhysical.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFResistanceFocus.cpp | SLFResistanceFocus.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFResistanceImmunity.cpp | SLFResistanceImmunity.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFResistanceRobustness.cpp | SLFResistanceRobustness.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFResistanceVitality.cpp | SLFResistanceVitality.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFStatArcane.cpp | SLFStatArcane.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFStatDexterity.cpp | SLFStatDexterity.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFStatEndurance.cpp | SLFStatEndurance.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFStatFaith.cpp | SLFStatFaith.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFStatIntelligence.cpp | SLFStatIntelligence.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFStatMind.cpp | SLFStatMind.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFStatStrength.cpp | SLFStatStrength.cpp |  | 2026-01-03 | 2026-01-26 |
| Stats/SLFStatVigor.cpp | SLFStatVigor.cpp |  | 2026-01-03 | 2026-01-26 |
| B_Chaos_ForceField.cpp | B_Chaos_ForceField.cpp | BeginPlay, Tick, EnableChaosDestroy_Implementation, DisableChaosDestroy_Imple... | 2026-01-03 | 2026-01-19 |
| B_Destructible.h | B_Destructible.h | **AB_Destructible** | 2026-01-03 | 2026-01-19 |
| B_Door.cpp | B_Door.cpp | OnConstruction, BeginPlay, OnInteract_Implementation, ValidateUnlockRequireme... | 2026-01-03 | 2026-01-19 |
| B_Door.h | B_Door.h | **UAnimMontage, UDataTable, UStaticMeshComponent** | 2026-01-03 | 2026-01-19 |
| SLFContainerBase.cpp | SLFContainerBase.cpp | BeginPlay, OnInteract_Implementation, DelayedOpenLidAndSpawnItems, Tick, OpenLid | 2026-01-03 | 2026-01-17 |
| SLFContainerBase.h | SLFContainerBase.h | **UAnimMontage, UNiagaraSystem, ULootDropManagerComponent** | 2026-01-03 | 2026-01-17 |
| Actors/SLFSkyManager.cpp | SLFSkyManager.cpp | BeginPlay, Tick, SetTimeOfDay_Implementation, AdvanceTime_Implementation, IsD... | 2026-01-03 | 2026-01-17 |
| Actors/SLFSkyManager.h | SLFSkyManager.h | **UPDA_DayNight, ASLFSkyManager, ADirectionalLight** | 2026-01-03 | 2026-01-17 |
| B_BossDoor.cpp | B_BossDoor.cpp | BeginPlay, OnInteract_Implementation, GetDeathCurrencySpawnPoint_Implementati... | 2026-01-03 | 2026-01-16 |
| B_BossDoor.h | B_BossDoor.h | **AB_BossDoor** | 2026-01-03 | 2026-01-16 |
| B_Item.h | B_Item.h | **UAnimMontage, UDataTable, AB_Item** | 2026-01-03 | 2026-01-16 |
| B_Ladder.h | B_Ladder.h | **AB_Ladder**; OnClimbingCollisionEndOverlap, OnBottomCollisionBeginOverlap, ... | 2026-01-03 | 2026-01-16 |
| B_Soulslike_Boss.cpp | B_Soulslike_Boss.cpp | BeginPlay, OnTriggerCollisionBeginOverlap, OnControllerPerceptionUpdated, Try... | 2026-01-03 | 2026-01-16 |
| B_Soulslike_Boss.h | B_Soulslike_Boss.h | **UAIBossComponent, AB_Soulslike_Boss**; OnTriggerCollisionBeginOverlap | 2026-01-03 | 2026-01-16 |
| B_Soulslike_Enemy.cpp | B_Soulslike_Enemy.cpp | BeginPlay, CheckSense_Implementation, GetExecutionMoveToTransform_Implementat... | 2026-01-03 | 2026-01-16 |
| B_Soulslike_Enemy.h | B_Soulslike_Enemy.h | **UAnimMontage, UAIPerceptionComponent, UWidgetComponent** | 2026-01-03 | 2026-01-16 |
| B_Weight.cpp | B_Weight.cpp |  | 2026-01-03 | 2026-01-16 |
| CS_Boss_Roar.cpp | CS_Boss_Roar.cpp |  | 2026-01-03 | 2026-01-16 |
| CS_Boss_Roar.h | CS_Boss_Roar.h | **UCS_Boss_Roar** | 2026-01-03 | 2026-01-16 |
| SLFStatBase.cpp | SLFStatBase.cpp | GetWorld, AdjustValue_Implementation, AdjustAffectedValue_Implementation, Cal... | 2026-01-03 | 2026-01-16 |
| Actions/SLFActionExecute.cpp | SLFActionExecute.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-16 |
| Stats/SLFStatPoise.cpp | SLFStatPoise.cpp |  | 2026-01-03 | 2026-01-16 |
| Stats/SLFStatWeight.cpp | SLFStatWeight.cpp |  | 2026-01-03 | 2026-01-16 |
| B_Item_AI_Weapon.cpp | B_Item_AI_Weapon.cpp | BeginPlay | 2026-01-03 | 2026-01-12 |
| B_Item_AI_Weapon.h | B_Item_AI_Weapon.h | **UAnimMontage, UDataTable, AB_Item_AI_Weapon** | 2026-01-03 | 2026-01-12 |
| B_Item_AI_Weapon_Greatsword.cpp | B_Item_AI_Weapon_Greatsword.cpp |  | 2026-01-03 | 2026-01-12 |
| B_Item_AI_Weapon_Sword.cpp | B_Item_AI_Weapon_Sword.cpp |  | 2026-01-03 | 2026-01-12 |
| Actions/SLFActionStopSprinting.cpp | SLFActionStopSprinting.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-12 |
| Stats/SLFStatStamina.cpp | SLFStatStamina.cpp |  | 2026-01-03 | 2026-01-12 |
| AIC_SoulslikeFramework.h | AIC_SoulslikeFramework.h | **UBehaviorTree, UAIPerceptionComponent, AAIC_SoulslikeFramework** | 2026-01-03 | 2026-01-11 |
| Actions/SLFActionComboHeavy.cpp | SLFActionComboHeavy.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-11 |
| Actions/SLFActionComboLightL.cpp | SLFActionComboLightL.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-11 |
| Actions/SLFActionComboLightR.cpp | SLFActionComboLightR.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-11 |
| Actions/SLFActionStartSprinting.cpp | SLFActionStartSprinting.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-11 |
| Actions/SLFActionTwoHandedStanceL.cpp | SLFActionTwoHandedStanceL.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-11 |
| Actions/SLFActionTwoHandedStanceR.cpp | SLFActionTwoHandedStanceR.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-11 |
| BFL_Helper.cpp | BFL_Helper.cpp | GetKeysForIA, GetInputUserSettings, GetStatCurrentValue, GetStatMaxValue, Get... | 2026-01-03 | 2026-01-10 |
| SLFPickupItemBase.h | SLFPickupItemBase.h | **UDataAsset, ASLFPickupItemBase** | 2026-01-03 | 2026-01-09 |
| BFL_Helper.h | BFL_Helper.h | **UAnimMontage, UDataTable, UPrimaryDataAsset** | 2026-01-03 | 2026-01-07 |
| BFL_StructConversion.cpp | BFL_StructConversion.cpp | ConvertToWorldSaveInfoStruct, ConvertToClassSaveInfoStruct, ConvertToStatInfo... | 2026-01-07 | 2026-01-07 |
| BFL_StructConversion.h | BFL_StructConversion.h | **UBFL_StructConversion**; ExtractFromInstancedStruct, ExtractArrayFromSaveData | 2026-01-07 | 2026-01-07 |
| B_AP_Fire.cpp | B_AP_Fire.cpp |  | 2026-01-03 | 2026-01-07 |
| B_AP_Frost.cpp | B_AP_Frost.cpp |  | 2026-01-03 | 2026-01-07 |
| B_AP_Holy.cpp | B_AP_Holy.cpp |  | 2026-01-03 | 2026-01-07 |
| B_AP_Lightning.cpp | B_AP_Lightning.cpp |  | 2026-01-03 | 2026-01-07 |
| B_AP_Magic.cpp | B_AP_Magic.cpp |  | 2026-01-03 | 2026-01-07 |
| B_AP_Physical.cpp | B_AP_Physical.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Action_Backstab.cpp | B_Action_Backstab.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_Backstab.h | B_Action_Backstab.h | **UAnimMontage, UDataTable, UB_Action_Backstab** | 2026-01-03 | 2026-01-07 |
| B_Action_ComboHeavy.cpp | B_Action_ComboHeavy.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_ComboHeavy.h | B_Action_ComboHeavy.h | **UAnimMontage, UDataTable, UB_Action_ComboHeavy** | 2026-01-03 | 2026-01-07 |
| B_Action_ComboLight_L.cpp | B_Action_ComboLight_L.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_ComboLight_L.h | B_Action_ComboLight_L.h | **UAnimMontage, UDataTable, UB_Action_ComboLight_L** | 2026-01-03 | 2026-01-07 |
| B_Action_ComboLight_R.cpp | B_Action_ComboLight_R.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_ComboLight_R.h | B_Action_ComboLight_R.h | **UAnimMontage, UDataTable, UB_Action_ComboLight_R** | 2026-01-03 | 2026-01-07 |
| B_Action_Crouch.cpp | B_Action_Crouch.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_Crouch.h | B_Action_Crouch.h | **UAnimMontage, UDataTable, UB_Action_Crouch** | 2026-01-03 | 2026-01-07 |
| B_Action_Dodge.cpp | B_Action_Dodge.cpp | ExecuteAction_Implementation, GetDirectionalDodgeMontage | 2026-01-03 | 2026-01-07 |
| B_Action_DualWieldAttack.cpp | B_Action_DualWieldAttack.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_DualWieldAttack.h | B_Action_DualWieldAttack.h | **UAnimMontage, UDataTable, UB_Action_DualWieldAttack** | 2026-01-03 | 2026-01-07 |
| B_Action_Execute.cpp | B_Action_Execute.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_Execute.h | B_Action_Execute.h | **UAnimMontage, UDataTable, UB_Action_Execute** | 2026-01-03 | 2026-01-07 |
| B_Action_GuardCancel.cpp | B_Action_GuardCancel.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_GuardCancel.h | B_Action_GuardCancel.h | **UAnimMontage, UDataTable, UB_Action_GuardCancel** | 2026-01-03 | 2026-01-07 |
| B_Action_GuardEnd.cpp | B_Action_GuardEnd.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_GuardEnd.h | B_Action_GuardEnd.h | **UAnimMontage, UDataTable, UB_Action_GuardEnd** | 2026-01-03 | 2026-01-07 |
| B_Action_GuardStart.cpp | B_Action_GuardStart.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_GuardStart.h | B_Action_GuardStart.h | **UAnimMontage, UDataTable, UB_Action_GuardStart** | 2026-01-03 | 2026-01-07 |
| B_Action_Jump.cpp | B_Action_Jump.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_Jump.h | B_Action_Jump.h | **UAnimMontage, UDataTable, UB_Action_Jump** | 2026-01-03 | 2026-01-07 |
| B_Action_JumpAttack.cpp | B_Action_JumpAttack.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_JumpAttack.h | B_Action_JumpAttack.h | **UAnimMontage, UDataTable, UB_Action_JumpAttack** | 2026-01-03 | 2026-01-07 |
| B_Action_PickupItemMontage.h | B_Action_PickupItemMontage.h | **UAnimMontage, UDataTable, UB_Action_PickupItemMontage** | 2026-01-03 | 2026-01-07 |
| B_Action_ScrollWheel_LeftHand.h | B_Action_ScrollWheel_LeftHand.h | **UAnimMontage, UDataTable, UB_Action_ScrollWheel_LeftHand** | 2026-01-03 | 2026-01-07 |
| B_Action_ScrollWheel_RightHand.h | B_Action_ScrollWheel_RightHand.h | **UAnimMontage, UDataTable, UB_Action_ScrollWheel_RightHand** | 2026-01-03 | 2026-01-07 |
| B_Action_ScrollWheel_Tools.h | B_Action_ScrollWheel_Tools.h | **UAnimMontage, UDataTable, UB_Action_ScrollWheel_Tools** | 2026-01-03 | 2026-01-07 |
| B_Action_SprintAttack.cpp | B_Action_SprintAttack.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_SprintAttack.h | B_Action_SprintAttack.h | **UAnimMontage, UDataTable, UB_Action_SprintAttack** | 2026-01-03 | 2026-01-07 |
| B_Action_StartSprinting.cpp | B_Action_StartSprinting.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_StartSprinting.h | B_Action_StartSprinting.h | **UAnimMontage, UDataTable, UB_Action_StartSprinting** | 2026-01-03 | 2026-01-07 |
| B_Action_StopSprinting.cpp | B_Action_StopSprinting.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_StopSprinting.h | B_Action_StopSprinting.h | **UAnimMontage, UDataTable, UB_Action_StopSprinting** | 2026-01-03 | 2026-01-07 |
| B_Action_ThrowProjectile.cpp | B_Action_ThrowProjectile.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_ThrowProjectile.h | B_Action_ThrowProjectile.h | **UAnimMontage, UDataTable, UB_Action_ThrowProjectile** | 2026-01-03 | 2026-01-07 |
| B_Action_TwoHandedStance_L.cpp | B_Action_TwoHandedStance_L.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_TwoHandedStance_L.h | B_Action_TwoHandedStance_L.h | **UAnimMontage, UDataTable, UB_Action_TwoHandedStance_L** | 2026-01-03 | 2026-01-07 |
| B_Action_TwoHandedStance_R.cpp | B_Action_TwoHandedStance_R.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_TwoHandedStance_R.h | B_Action_TwoHandedStance_R.h | **UAnimMontage, UDataTable, UB_Action_TwoHandedStance_R** | 2026-01-03 | 2026-01-07 |
| B_Action_UseEquippedTool.cpp | B_Action_UseEquippedTool.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| B_Action_UseEquippedTool.h | B_Action_UseEquippedTool.h | **UAnimMontage, UDataTable, UB_Action_UseEquippedTool** | 2026-01-03 | 2026-01-07 |
| B_Action_WeaponAbility.h | B_Action_WeaponAbility.h | **UAnimMontage, UDataTable, UB_Action_WeaponAbility** | 2026-01-03 | 2026-01-07 |
| B_Arcane.cpp | B_Arcane.cpp |  | 2026-01-03 | 2026-01-07 |
| B_DN_Fire.cpp | B_DN_Fire.cpp |  | 2026-01-03 | 2026-01-07 |
| B_DN_Frost.cpp | B_DN_Frost.cpp |  | 2026-01-03 | 2026-01-07 |
| B_DN_Holy.cpp | B_DN_Holy.cpp |  | 2026-01-03 | 2026-01-07 |
| B_DN_Lightning.cpp | B_DN_Lightning.cpp |  | 2026-01-03 | 2026-01-07 |
| B_DN_Magic.cpp | B_DN_Magic.cpp |  | 2026-01-03 | 2026-01-07 |
| B_DN_Physical.cpp | B_DN_Physical.cpp |  | 2026-01-03 | 2026-01-07 |
| B_DemoRoom.cpp | B_DemoRoom.cpp | OnConstruction, BeginPlay, CreateInstances_Implementation, Redraw_Implementat... | 2026-01-03 | 2026-01-07 |
| B_DemoRoom.h | B_DemoRoom.h | **UAnimMontage, UDataTable, AB_DemoRoom** | 2026-01-03 | 2026-01-07 |
| B_Dexterity.cpp | B_Dexterity.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Endurance.cpp | B_Endurance.cpp |  | 2026-01-03 | 2026-01-07 |
| B_FP.cpp | B_FP.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Faith.cpp | B_Faith.cpp |  | 2026-01-03 | 2026-01-07 |
| B_HP.cpp | B_HP.cpp |  | 2026-01-03 | 2026-01-07 |
| B_IncantationPower.cpp | B_IncantationPower.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Intelligence.cpp | B_Intelligence.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Item_Weapon.cpp | B_Item_Weapon.cpp | GetAttackPowerStats_Implementation, GetWeaponStat_Implementation, GetWeaponSt... | 2026-01-03 | 2026-01-07 |
| B_Mind.cpp | B_Mind.cpp |  | 2026-01-03 | 2026-01-07 |
| B_PatrolPath.cpp | B_PatrolPath.cpp | GetWorldPositionForCurrentPoint_Implementation | 2026-01-03 | 2026-01-07 |
| B_Poise.cpp | B_Poise.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Resistance_Focus.cpp | B_Resistance_Focus.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Resistance_Immunity.cpp | B_Resistance_Immunity.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Resistance_Robustness.cpp | B_Resistance_Robustness.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Resistance_Vitality.cpp | B_Resistance_Vitality.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Soulslike_NPC.cpp | B_Soulslike_NPC.cpp | BeginPlay, GetLookAtLocation, TryGetItemInfo_Implementation, OnSpawnedFromSav... | 2026-01-03 | 2026-01-07 |
| B_Soulslike_NPC.h | B_Soulslike_NPC.h | **UAIInteractionManagerComponent, USphereComponent, AB_Soulslike_NPC** | 2026-01-03 | 2026-01-07 |
| B_Stamina.cpp | B_Stamina.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Stance.cpp | B_Stance.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Stat.cpp | B_Stat.cpp | GetWorld, AdjustValue_Implementation, AdjustAffectedValue_Implementation, Cal... | 2026-01-03 | 2026-01-07 |
| B_Stat.h | B_Stat.h | **UAnimMontage, UDataTable, UB_Stat** | 2026-01-03 | 2026-01-07 |
| B_StatusEffectBuildup.cpp | B_StatusEffectBuildup.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Strength.cpp | B_Strength.cpp |  | 2026-01-03 | 2026-01-07 |
| B_Vigor.cpp | B_Vigor.cpp |  | 2026-01-03 | 2026-01-07 |
| SG_SaveSlots.cpp | SG_SaveSlots.cpp | AddSlot_Implementation, GetLastSaveSlot_Implementation, GetAllSlots_Implement... | 2026-01-03 | 2026-01-07 |
| SG_SaveSlots.h | SG_SaveSlots.h | **UAnimMontage, UDataTable, USG_SaveSlots** | 2026-01-03 | 2026-01-07 |
| SG_SoulslikeFramework.cpp | SG_SoulslikeFramework.cpp | SetSavedData_Implementation, GetSavedData_Implementation | 2026-01-03 | 2026-01-07 |
| SG_SoulslikeFramework.h | SG_SoulslikeFramework.h | **UAnimMontage, UDataTable, USG_SoulslikeFramework** | 2026-01-03 | 2026-01-07 |
| SLFItemBase.h | SLFItemBase.h | **UDataAsset, ASLFItemBase** | 2026-01-03 | 2026-01-07 |
| SLFStatBase.h | SLFStatBase.h | **USLFStatBase, USLFStatBase** | 2026-01-03 | 2026-01-07 |
| SLFStatusEffectBase.cpp | SLFStatusEffectBase.cpp | GetOwnerStatManager_Implementation, GetBuildupPercent_Implementation, GetResi... | 2026-01-03 | 2026-01-07 |
| Actions/SLFActionDualWieldAttack.cpp | SLFActionDualWieldAttack.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| Actions/SLFActionJumpAttack.cpp | SLFActionJumpAttack.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| Actions/SLFActionSprintAttack.cpp | SLFActionSprintAttack.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-07 |
| Actors/SLFBonfire.cpp | SLFBonfire.cpp | BeginPlay, LightBonfire_Implementation, Rest_Implementation, Kindle_Implement... | 2026-01-03 | 2026-01-07 |
| Actors/SLFContainer.cpp | SLFContainer.cpp | BeginPlay, Interact_Implementation, OpenContainer_Implementation, LootContain... | 2026-01-03 | 2026-01-07 |
| Actors/SLFContainer.h | SLFContainer.h | **ASLFContainer, UBoxComponent** | 2026-01-03 | 2026-01-07 |
| Actors/SLFDestructible.cpp | SLFDestructible.cpp | BeginPlay, ApplyDamage_Implementation, DestroyObject_Implementation, SpawnDeb... | 2026-01-03 | 2026-01-07 |
| Actors/SLFDiscovery.cpp | SLFDiscovery.cpp | BeginPlay, OnPlayerEnter_Implementation, TriggerDiscovery_Implementation, Sho... | 2026-01-03 | 2026-01-07 |
| Actors/SLFSequenceActor.cpp | SLFSequenceActor.cpp | BeginPlay, PlaySequence_Implementation, StopSequence_Implementation, OnSequen... | 2026-01-03 | 2026-01-07 |
| Actors/SLFSequenceActor.h | SLFSequenceActor.h | **ASLFSequenceActor, UBoxComponent, ULevelSequence** | 2026-01-03 | 2026-01-07 |
| Actors/SLFStatusEffectArea.cpp | SLFStatusEffectArea.cpp | BeginPlay, Tick, OnActorEnter_Implementation, OnActorExit_Implementation, App... | 2026-01-03 | 2026-01-07 |
| Actors/SLFStatusEffectBuildup.cpp | SLFStatusEffectBuildup.cpp | BeginPlay, ApplyBuildup_Implementation, SetTarget_Implementation, OnStatusEff... | 2026-01-03 | 2026-01-07 |
| Actors/SLFStatusEffectOneShot.cpp | SLFStatusEffectOneShot.cpp | BeginPlay, OnActorOverlap_Implementation, TriggerEffect_Implementation | 2026-01-03 | 2026-01-07 |
| Buffs/SLFBuffAttackPower.cpp | SLFBuffAttackPower.cpp | OnGranted_Implementation, OnRemoved_Implementation | 2026-01-03 | 2026-01-07 |
| Actions/SLFActionDodge.cpp | SLFActionDodge.cpp | ExecuteAction_Implementation, GetDirectionalDodgeMontage | 2026-01-03 | 2026-01-04 |
| Actions/SLFActionGuardCancel.cpp | SLFActionGuardCancel.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-04 |
| Actions/SLFActionGuardEnd.cpp | SLFActionGuardEnd.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-04 |
| Actions/SLFActionGuardStart.cpp | SLFActionGuardStart.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-04 |
| Actors/SLFDoorBase.h | SLFDoorBase.h | **ASLFDoorBase, UBoxComponent** | 2026-01-03 | 2026-01-04 |
| BML_HelperMacros.cpp | BML_HelperMacros.cpp |  | 2026-01-03 | 2026-01-03 |
| BML_HelperMacros.h | BML_HelperMacros.h | **UAnimMontage, UDataTable, UBML_HelperMacros** | 2026-01-03 | 2026-01-03 |
| BML_StructConversion.cpp | BML_StructConversion.cpp |  | 2026-01-03 | 2026-01-03 |
| BML_StructConversion.h | BML_StructConversion.h | **UAnimMontage, UDataTable, UBML_StructConversion** | 2026-01-03 | 2026-01-03 |
| B_AP_Fire.h | B_AP_Fire.h | **UAnimMontage, UDataTable, UB_AP_Fire** | 2026-01-03 | 2026-01-03 |
| B_AP_Frost.h | B_AP_Frost.h | **UAnimMontage, UDataTable, UB_AP_Frost** | 2026-01-03 | 2026-01-03 |
| B_AP_Holy.h | B_AP_Holy.h | **UAnimMontage, UDataTable, UB_AP_Holy** | 2026-01-03 | 2026-01-03 |
| B_AP_Lightning.h | B_AP_Lightning.h | **UAnimMontage, UDataTable, UB_AP_Lightning** | 2026-01-03 | 2026-01-03 |
| B_AP_Magic.h | B_AP_Magic.h | **UAnimMontage, UDataTable, UB_AP_Magic** | 2026-01-03 | 2026-01-03 |
| B_AP_Physical.h | B_AP_Physical.h | **UAnimMontage, UDataTable, UB_AP_Physical** | 2026-01-03 | 2026-01-03 |
| B_AbilityEffectBase.cpp | B_AbilityEffectBase.cpp |  | 2026-01-03 | 2026-01-03 |
| B_AbilityEffectBase.h | B_AbilityEffectBase.h | **UAnimMontage, UDataTable, AB_AbilityEffectBase** | 2026-01-03 | 2026-01-03 |
| B_Action_Dodge.h | B_Action_Dodge.h | **UAnimMontage, UDataTable, UB_Action_Dodge** | 2026-01-03 | 2026-01-03 |
| B_Arcane.h | B_Arcane.h | **UAnimMontage, UDataTable, UB_Arcane** | 2026-01-03 | 2026-01-03 |
| B_BaseCharacter.cpp | B_BaseCharacter.cpp |  | 2026-01-03 | 2026-01-03 |
| B_BaseCharacter.h | B_BaseCharacter.h | **UAnimMontage, UDataTable, AB_BaseCharacter** | 2026-01-03 | 2026-01-03 |
| B_Buff.cpp | B_Buff.cpp | GetOwnerStatManager_Implementation, GetMultiplierForCurrentRank_Implementatio... | 2026-01-03 | 2026-01-03 |
| B_Buff.h | B_Buff.h | **UAnimMontage, UDataTable, UB_Buff** | 2026-01-03 | 2026-01-03 |
| B_Buff_AttackPower.cpp | B_Buff_AttackPower.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Buff_AttackPower.h | B_Buff_AttackPower.h | **UAnimMontage, UDataTable, UB_Buff_AttackPower** | 2026-01-03 | 2026-01-03 |
| B_DN_Fire.h | B_DN_Fire.h | **UAnimMontage, UDataTable, UB_DN_Fire** | 2026-01-03 | 2026-01-03 |
| B_DN_Frost.h | B_DN_Frost.h | **UAnimMontage, UDataTable, UB_DN_Frost** | 2026-01-03 | 2026-01-03 |
| B_DN_Holy.h | B_DN_Holy.h | **UAnimMontage, UDataTable, UB_DN_Holy** | 2026-01-03 | 2026-01-03 |
| B_DN_Lightning.h | B_DN_Lightning.h | **UAnimMontage, UDataTable, UB_DN_Lightning** | 2026-01-03 | 2026-01-03 |
| B_DN_Magic.h | B_DN_Magic.h | **UAnimMontage, UDataTable, UB_DN_Magic** | 2026-01-03 | 2026-01-03 |
| B_DN_Physical.h | B_DN_Physical.h | **UAnimMontage, UDataTable, UB_DN_Physical** | 2026-01-03 | 2026-01-03 |
| B_DeathTrigger.cpp | B_DeathTrigger.cpp |  | 2026-01-03 | 2026-01-03 |
| B_DeathTrigger.h | B_DeathTrigger.h | **UAnimMontage, UDataTable, AB_DeathTrigger** | 2026-01-03 | 2026-01-03 |
| B_DemoDisplay.cpp | B_DemoDisplay.cpp | OnConstruction, CreateInstances_Implementation, CreateDisplay_Implementation,... | 2026-01-03 | 2026-01-03 |
| B_DemoDisplay.h | B_DemoDisplay.h | **UAnimMontage, UDataTable, AB_DemoDisplay** | 2026-01-03 | 2026-01-03 |
| B_Demo_TimeSlider.cpp | B_Demo_TimeSlider.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Demo_TimeSlider.h | B_Demo_TimeSlider.h | **UAnimMontage, UDataTable, AB_SkyManager** | 2026-01-03 | 2026-01-03 |
| B_Dexterity.h | B_Dexterity.h | **UAnimMontage, UDataTable, UB_Dexterity** | 2026-01-03 | 2026-01-03 |
| B_Discovery.cpp | B_Discovery.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Discovery.h | B_Discovery.h | **UAnimMontage, UDataTable, UB_Discovery** | 2026-01-03 | 2026-01-03 |
| B_Door_Demo.cpp | B_Door_Demo.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Door_Demo.h | B_Door_Demo.h | **UAnimMontage, UDataTable, AB_Door_Demo** | 2026-01-03 | 2026-01-03 |
| B_Endurance.h | B_Endurance.h | **UAnimMontage, UDataTable, UB_Endurance** | 2026-01-03 | 2026-01-03 |
| B_FP.h | B_FP.h | **UAnimMontage, UDataTable, UB_FP** | 2026-01-03 | 2026-01-03 |
| B_Faith.h | B_Faith.h | **UAnimMontage, UDataTable, UB_Faith** | 2026-01-03 | 2026-01-03 |
| B_HP.h | B_HP.h | **UAnimMontage, UDataTable, UB_HP** | 2026-01-03 | 2026-01-03 |
| B_IncantationPower.h | B_IncantationPower.h | **UAnimMontage, UDataTable, UB_IncantationPower** | 2026-01-03 | 2026-01-03 |
| B_Intelligence.h | B_Intelligence.h | **UAnimMontage, UDataTable, UB_Intelligence** | 2026-01-03 | 2026-01-03 |
| B_Item.cpp | B_Item.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Item_AI_Weapon_BossMace.cpp | B_Item_AI_Weapon_BossMace.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Item_AI_Weapon_BossMace.h | B_Item_AI_Weapon_BossMace.h | **UAnimMontage, UDataTable, AB_Item_AI_Weapon_BossMace** | 2026-01-03 | 2026-01-03 |
| B_Item_AI_Weapon_Greatsword.h | B_Item_AI_Weapon_Greatsword.h | **UAnimMontage, UDataTable, AB_Item_AI_Weapon_Greatsword** | 2026-01-03 | 2026-01-03 |
| B_Item_AI_Weapon_Sword.h | B_Item_AI_Weapon_Sword.h | **UAnimMontage, UDataTable, AB_Item_AI_Weapon_Sword** | 2026-01-03 | 2026-01-03 |
| B_Item_Lantern.cpp | B_Item_Lantern.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Item_Lantern.h | B_Item_Lantern.h | **UAnimMontage, UDataTable, AB_Item_Lantern** | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon.h | B_Item_Weapon.h | **UAnimMontage, UDataTable, AB_Item_Weapon** | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_BossMace.cpp | B_Item_Weapon_BossMace.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_BossMace.h | B_Item_Weapon_BossMace.h | **UAnimMontage, UDataTable, AB_Item_Weapon_BossMace** | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_Greatsword.cpp | B_Item_Weapon_Greatsword.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_Greatsword.h | B_Item_Weapon_Greatsword.h | **UAnimMontage, UDataTable, AB_Item_Weapon_Greatsword** | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_Katana.cpp | B_Item_Weapon_Katana.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_Katana.h | B_Item_Weapon_Katana.h | **UAnimMontage, UDataTable, AB_Item_Weapon_Katana** | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_PoisonSword.cpp | B_Item_Weapon_PoisonSword.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_PoisonSword.h | B_Item_Weapon_PoisonSword.h | **UAnimMontage, UDataTable, AB_Item_Weapon_PoisonSword** | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_Shield.cpp | B_Item_Weapon_Shield.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_Shield.h | B_Item_Weapon_Shield.h | **UAnimMontage, UDataTable, AB_Item_Weapon_Shield** | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_SwordExample01.cpp | B_Item_Weapon_SwordExample01.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_SwordExample01.h | B_Item_Weapon_SwordExample01.h | **UAnimMontage, UDataTable, AB_Item_Weapon_SwordExample01** | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_SwordExample02.cpp | B_Item_Weapon_SwordExample02.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Item_Weapon_SwordExample02.h | B_Item_Weapon_SwordExample02.h | **UAnimMontage, UDataTable, AB_Item_Weapon_SwordExample02** | 2026-01-03 | 2026-01-03 |
| B_LocationActor.cpp | B_LocationActor.cpp |  | 2026-01-03 | 2026-01-03 |
| B_LocationActor.h | B_LocationActor.h | **UAnimMontage, UDataTable, AB_LocationActor** | 2026-01-03 | 2026-01-03 |
| B_Mind.h | B_Mind.h | **UAnimMontage, UDataTable, UB_Mind** | 2026-01-03 | 2026-01-03 |
| B_PatrolPath.h | B_PatrolPath.h | **UAnimMontage, UDataTable, AB_PatrolPath** | 2026-01-03 | 2026-01-03 |
| B_Poise.h | B_Poise.h | **UAnimMontage, UDataTable, UB_Poise** | 2026-01-03 | 2026-01-03 |
| B_Projectile_Boss_Fireball.cpp | B_Projectile_Boss_Fireball.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Projectile_Boss_Fireball.h | B_Projectile_Boss_Fireball.h | **UAnimMontage, UDataTable, AB_Projectile_Boss_Fireball** | 2026-01-03 | 2026-01-03 |
| B_Projectile_ThrowingKnife.cpp | B_Projectile_ThrowingKnife.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Projectile_ThrowingKnife.h | B_Projectile_ThrowingKnife.h | **UAnimMontage, UDataTable, AB_Projectile_ThrowingKnife** | 2026-01-03 | 2026-01-03 |
| B_Resistance_Focus.h | B_Resistance_Focus.h | **UAnimMontage, UDataTable, UB_Resistance_Focus** | 2026-01-03 | 2026-01-03 |
| B_Resistance_Immunity.h | B_Resistance_Immunity.h | **UAnimMontage, UDataTable, UB_Resistance_Immunity** | 2026-01-03 | 2026-01-03 |
| B_Resistance_Robustness.h | B_Resistance_Robustness.h | **UAnimMontage, UDataTable, UB_Resistance_Robustness** | 2026-01-03 | 2026-01-03 |
| B_Resistance_Vitality.h | B_Resistance_Vitality.h | **UAnimMontage, UDataTable, UB_Resistance_Vitality** | 2026-01-03 | 2026-01-03 |
| B_ShowcaseEnemy_ExitCombat.cpp | B_ShowcaseEnemy_ExitCombat.cpp |  | 2026-01-03 | 2026-01-03 |
| B_ShowcaseEnemy_ExitCombat.h | B_ShowcaseEnemy_ExitCombat.h | **UAnimMontage, UDataTable, AB_Soulslike_Enemy** | 2026-01-03 | 2026-01-03 |
| B_ShowcaseEnemy_StatDisplay.cpp | B_ShowcaseEnemy_StatDisplay.cpp |  | 2026-01-03 | 2026-01-03 |
| B_ShowcaseEnemy_StatDisplay.h | B_ShowcaseEnemy_StatDisplay.h | **UAnimMontage, UDataTable, AB_Soulslike_Enemy** | 2026-01-03 | 2026-01-03 |
| B_SkyManager.cpp | B_SkyManager.cpp |  | 2026-01-03 | 2026-01-03 |
| B_SkyManager.h | B_SkyManager.h | **UAnimMontage, UDataTable, AB_SkyManager** | 2026-01-03 | 2026-01-03 |
| B_Soulslike_Boss_Malgareth.cpp | B_Soulslike_Boss_Malgareth.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Soulslike_Boss_Malgareth.h | B_Soulslike_Boss_Malgareth.h | **UAnimMontage, UDataTable, AB_Soulslike_Boss_Malgareth** | 2026-01-03 | 2026-01-03 |
| B_Soulslike_Enemy_Guard.cpp | B_Soulslike_Enemy_Guard.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Soulslike_Enemy_Guard.h | B_Soulslike_Enemy_Guard.h | **UAnimMontage, UDataTable, AB_Soulslike_Enemy_Guard** | 2026-01-03 | 2026-01-03 |
| B_Soulslike_Enemy_Showcase.cpp | B_Soulslike_Enemy_Showcase.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Soulslike_Enemy_Showcase.h | B_Soulslike_Enemy_Showcase.h | **UAnimMontage, UDataTable, AB_Soulslike_Enemy_Showcase** | 2026-01-03 | 2026-01-03 |
| B_Soulslike_NPC_ShowcaseGuide.cpp | B_Soulslike_NPC_ShowcaseGuide.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Soulslike_NPC_ShowcaseGuide.h | B_Soulslike_NPC_ShowcaseGuide.h | **UAnimMontage, UDataTable, AB_Soulslike_NPC_ShowcaseGuide** | 2026-01-03 | 2026-01-03 |
| B_Soulslike_NPC_ShowcaseVendor.cpp | B_Soulslike_NPC_ShowcaseVendor.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Soulslike_NPC_ShowcaseVendor.h | B_Soulslike_NPC_ShowcaseVendor.h | **UAnimMontage, UDataTable, AB_Soulslike_NPC_ShowcaseVendor** | 2026-01-03 | 2026-01-03 |
| B_Stamina.h | B_Stamina.h | **UAnimMontage, UDataTable, UB_Stamina** | 2026-01-03 | 2026-01-03 |
| B_Stance.h | B_Stance.h | **UAnimMontage, UDataTable, UB_Stance** | 2026-01-03 | 2026-01-03 |
| B_StatusEffectBuildup.h | B_StatusEffectBuildup.h | **UAnimMontage, UDataTable, UB_StatusEffectBuildup** | 2026-01-03 | 2026-01-03 |
| B_StatusEffectOneShot.cpp | B_StatusEffectOneShot.cpp |  | 2026-01-03 | 2026-01-03 |
| B_StatusEffectOneShot.h | B_StatusEffectOneShot.h | **UAnimMontage, UDataTable, AB_BaseCharacter** | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Bleed.cpp | B_StatusEffect_Bleed.cpp |  | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Bleed.h | B_StatusEffect_Bleed.h | **UAnimMontage, UDataTable, UB_StatusEffect_Bleed** | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Burn.cpp | B_StatusEffect_Burn.cpp |  | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Burn.h | B_StatusEffect_Burn.h | **UAnimMontage, UDataTable, UB_StatusEffect_Burn** | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Corruption.cpp | B_StatusEffect_Corruption.cpp |  | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Corruption.h | B_StatusEffect_Corruption.h | **UAnimMontage, UDataTable, UB_StatusEffect_Corruption** | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Frostbite.cpp | B_StatusEffect_Frostbite.cpp |  | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Frostbite.h | B_StatusEffect_Frostbite.h | **UAnimMontage, UDataTable, UB_StatusEffect_Frostbite** | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Madness.cpp | B_StatusEffect_Madness.cpp |  | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Madness.h | B_StatusEffect_Madness.h | **UAnimMontage, UDataTable, UB_StatusEffect_Madness** | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Plague.cpp | B_StatusEffect_Plague.cpp |  | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Plague.h | B_StatusEffect_Plague.h | **UAnimMontage, UDataTable, UB_StatusEffect_Plague** | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Poison.cpp | B_StatusEffect_Poison.cpp |  | 2026-01-03 | 2026-01-03 |
| B_StatusEffect_Poison.h | B_StatusEffect_Poison.h | **UAnimMontage, UDataTable, UB_StatusEffect_Poison** | 2026-01-03 | 2026-01-03 |
| B_Strength.h | B_Strength.h | **UAnimMontage, UDataTable, UB_Strength** | 2026-01-03 | 2026-01-03 |
| B_Torch.cpp | B_Torch.cpp |  | 2026-01-03 | 2026-01-03 |
| B_Torch.h | B_Torch.h | **UAnimMontage, UDataTable, AB_Torch** | 2026-01-03 | 2026-01-03 |
| B_Vigor.h | B_Vigor.h | **UAnimMontage, UDataTable, UB_Vigor** | 2026-01-03 | 2026-01-03 |
| B_Weight.h | B_Weight.h | **UAnimMontage, UDataTable, UB_Weight** | 2026-01-03 | 2026-01-03 |
| CS_Guard.cpp | CS_Guard.cpp |  | 2026-01-03 | 2026-01-03 |
| CS_Guard.h | CS_Guard.h | **UAnimMontage, UDataTable, UCS_Guard** | 2026-01-03 | 2026-01-03 |
| CS_JumpHit.cpp | CS_JumpHit.cpp |  | 2026-01-03 | 2026-01-03 |
| CS_JumpHit.h | CS_JumpHit.h | **UAnimMontage, UDataTable, UCS_JumpHit** | 2026-01-03 | 2026-01-03 |
| CS_Seq_Handheld.cpp | CS_Seq_Handheld.cpp |  | 2026-01-03 | 2026-01-03 |
| CS_Seq_Handheld.h | CS_Seq_Handheld.h | **UAnimMontage, UDataTable, UCS_Seq_Handheld** | 2026-01-03 | 2026-01-03 |
| CS_StrongHit.cpp | CS_StrongHit.cpp |  | 2026-01-03 | 2026-01-03 |
| CS_StrongHit.h | CS_StrongHit.h | **UAnimMontage, UDataTable, UCS_StrongHit** | 2026-01-03 | 2026-01-03 |
| CS_SubtleHit.cpp | CS_SubtleHit.cpp |  | 2026-01-03 | 2026-01-03 |
| CS_SubtleHit.h | CS_SubtleHit.h | **UAnimMontage, UDataTable, UCS_SubtleHit** | 2026-01-03 | 2026-01-03 |
| GS_SoulslikeFramework.cpp | GS_SoulslikeFramework.cpp |  | 2026-01-03 | 2026-01-03 |
| GS_SoulslikeFramework.h | GS_SoulslikeFramework.h | **UAnimMontage, UDataTable, AGS_SoulslikeFramework** | 2026-01-03 | 2026-01-03 |
| PS_SoulslikeFramework.cpp | PS_SoulslikeFramework.cpp |  | 2026-01-03 | 2026-01-03 |
| PS_SoulslikeFramework.h | PS_SoulslikeFramework.h | **UAnimMontage, UDataTable, APS_SoulslikeFramework** | 2026-01-03 | 2026-01-03 |
| SLFBossMalgareth.cpp | SLFBossMalgareth.cpp | BeginPlay | 2026-01-03 | 2026-01-03 |
| SLFBossMalgareth.h | SLFBossMalgareth.h | **ASLFBossMalgareth** | 2026-01-03 | 2026-01-03 |
| SLFBuffBase.cpp | SLFBuffBase.cpp | GetOwnerStatManager_Implementation, GetMultiplierForCurrentRank_Implementatio... | 2026-01-03 | 2026-01-03 |
| SLFBuffBase.h | SLFBuffBase.h | **UDataAsset, UStatManagerComponent, UPDA_Buff** | 2026-01-03 | 2026-01-03 |
| SLFEnemyShowcase.cpp | SLFEnemyShowcase.cpp | BeginPlay | 2026-01-03 | 2026-01-03 |
| SLFEnemyShowcase.h | SLFEnemyShowcase.h | **UChildActorComponent, ASLFEnemyShowcase** | 2026-01-03 | 2026-01-03 |
| SLFItemBase.cpp | SLFItemBase.cpp | BeginPlay, OnConstruction, SetupItem_Implementation | 2026-01-03 | 2026-01-03 |
| SLFStatusEffectBase.h | SLFStatusEffectBase.h | **UDataAsset, UStatManagerComponent, UNiagaraSystem** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionBackstab.h | SLFActionBackstab.h | **USLFActionBackstab** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionComboHeavy.h | SLFActionComboHeavy.h | **USLFActionComboHeavy** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionComboLightL.h | SLFActionComboLightL.h | **USLFActionComboLightL** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionComboLightR.h | SLFActionComboLightR.h | **USLFActionComboLightR** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionCrouch.cpp | SLFActionCrouch.cpp | ExecuteAction_Implementation | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionCrouch.h | SLFActionCrouch.h | **USLFActionCrouch** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionDodge.h | SLFActionDodge.h | **USLFActionDodge** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionDrinkFlaskHP.h | SLFActionDrinkFlaskHP.h | **USLFActionDrinkFlaskHP** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionDualWieldAttack.h | SLFActionDualWieldAttack.h | **USLFActionDualWieldAttack** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionExecute.h | SLFActionExecute.h | **USLFActionExecute** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionGuardCancel.h | SLFActionGuardCancel.h | **USLFActionGuardCancel** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionGuardEnd.h | SLFActionGuardEnd.h | **USLFActionGuardEnd** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionGuardStart.h | SLFActionGuardStart.h | **USLFActionGuardStart** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionJump.h | SLFActionJump.h | **USLFActionJump** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionJumpAttack.h | SLFActionJumpAttack.h | **USLFActionJumpAttack** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionPickupItemMontage.h | SLFActionPickupItemMontage.h | **USLFActionPickupItemMontage** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionScrollWheelLeftHand.h | SLFActionScrollWheelLeftHand.h | **USLFActionScrollWheelLeftHand** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionScrollWheelRightHand.h | SLFActionScrollWheelRightHand.h | **USLFActionScrollWheelRightHand** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionScrollWheelTools.h | SLFActionScrollWheelTools.h | **USLFActionScrollWheelTools** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionSprintAttack.h | SLFActionSprintAttack.h | **USLFActionSprintAttack** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionStartSprinting.h | SLFActionStartSprinting.h | **USLFActionStartSprinting** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionStopSprinting.h | SLFActionStopSprinting.h | **USLFActionStopSprinting** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionThrowProjectile.h | SLFActionThrowProjectile.h | **USLFActionThrowProjectile** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionTwoHandedStanceL.h | SLFActionTwoHandedStanceL.h | **USLFActionTwoHandedStanceL** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionTwoHandedStanceR.h | SLFActionTwoHandedStanceR.h | **USLFActionTwoHandedStanceR** | 2026-01-03 | 2026-01-03 |
| Actions/SLFActionUseEquippedTool.h | SLFActionUseEquippedTool.h | **USLFActionUseEquippedTool** | 2026-01-03 | 2026-01-03 |
| Actors/SLFAbilityEffectBase.cpp | SLFAbilityEffectBase.cpp | BeginPlay, Tick, ActivateEffect_Implementation, DeactivateEffect_Implementati... | 2026-01-03 | 2026-01-03 |
| Actors/SLFAbilityEffectBase.h | SLFAbilityEffectBase.h | **ASLFAbilityEffectBase, UNiagaraComponent** | 2026-01-03 | 2026-01-03 |
| Actors/SLFBonfire.h | SLFBonfire.h | **ASLFBonfire, UPointLightComponent, UNiagaraComponent** | 2026-01-03 | 2026-01-03 |
| Actors/SLFChaosForceField.cpp | SLFChaosForceField.cpp | BeginPlay, OnEffectTick_Implementation, OnActorEnterField_Implementation, Pus... | 2026-01-03 | 2026-01-03 |
| Actors/SLFChaosForceField.h | SLFChaosForceField.h | **ASLFChaosForceField, USphereComponent** | 2026-01-03 | 2026-01-03 |
| Actors/SLFDeathTrigger.cpp | SLFDeathTrigger.cpp | BeginPlay, OnActorEnter_Implementation, KillActor_Implementation | 2026-01-03 | 2026-01-03 |
| Actors/SLFDeathTrigger.h | SLFDeathTrigger.h | **ASLFDeathTrigger, UBoxComponent** | 2026-01-03 | 2026-01-03 |
| Actors/SLFDestructible.h | SLFDestructible.h | **ASLFDestructible, UNiagaraSystem** | 2026-01-03 | 2026-01-03 |
| Actors/SLFDiscovery.h | SLFDiscovery.h | **ASLFDiscovery, UBoxComponent** | 2026-01-03 | 2026-01-03 |
| Actors/SLFDoorBase.cpp | SLFDoorBase.cpp | BeginPlay, Tick, Interact_Implementation, OpenDoor_Implementation, CloseDoor_... | 2026-01-03 | 2026-01-03 |
| Actors/SLFDoorDemo.cpp | SLFDoorDemo.cpp | BeginPlay, Interact_Implementation, ShowHint_Implementation, HideHint_Impleme... | 2026-01-03 | 2026-01-03 |
| Actors/SLFDoorDemo.h | SLFDoorDemo.h | **ASLFDoorDemo** | 2026-01-03 | 2026-01-03 |
| Actors/SLFLocationActor.cpp | SLFLocationActor.cpp | BeginPlay, GetSpawnTransform, UnlockLocation_Implementation, TeleportActorHer... | 2026-01-03 | 2026-01-03 |
| Actors/SLFLocationActor.h | SLFLocationActor.h | **ASLFLocationActor, UBillboardComponent, UArrowComponent** | 2026-01-03 | 2026-01-03 |
| Actors/SLFPatrolPath.cpp | SLFPatrolPath.cpp | BeginPlay, OnConstruction, GetPatrolPoint, GetNextPointIndex, GetWaitTimeAtPoint | 2026-01-03 | 2026-01-03 |
| Actors/SLFPatrolPath.h | SLFPatrolPath.h | **ASLFPatrolPath, USplineComponent** | 2026-01-03 | 2026-01-03 |
| Actors/SLFStatusEffectArea.h | SLFStatusEffectArea.h | **ASLFStatusEffectArea, UBoxComponent, UNiagaraComponent** | 2026-01-03 | 2026-01-03 |
| Actors/SLFStatusEffectBuildup.h | SLFStatusEffectBuildup.h | **ASLFStatusEffectBuildup** | 2026-01-03 | 2026-01-03 |
| Actors/SLFStatusEffectOneShot.h | SLFStatusEffectOneShot.h | **ASLFStatusEffectOneShot, USphereComponent** | 2026-01-03 | 2026-01-03 |
| Actors/SLFTorch.cpp | SLFTorch.cpp | BeginPlay, LightTorch_Implementation, ExtinguishTorch_Implementation, ToggleT... | 2026-01-03 | 2026-01-03 |
| Actors/SLFTorch.h | SLFTorch.h | **ASLFTorch, UPointLightComponent, UNiagaraComponent** | 2026-01-03 | 2026-01-03 |
| Buffs/SLFBuffAttackPower.h | SLFBuffAttackPower.h | **USLFBuffAttackPower** | 2026-01-03 | 2026-01-03 |
| Items/SLFItemAIWeapon.cpp | SLFItemAIWeapon.cpp |  | 2026-01-03 | 2026-01-03 |
| Items/SLFItemAIWeapon.h | SLFItemAIWeapon.h | **ASLFItemAIWeapon** | 2026-01-03 | 2026-01-03 |
| Items/SLFItemLantern.cpp | SLFItemLantern.cpp | ToggleLight | 2026-01-03 | 2026-01-03 |
| Items/SLFItemLantern.h | SLFItemLantern.h | **ASLFItemLantern, UPointLightComponent** | 2026-01-03 | 2026-01-03 |
| Items/SLFItemWeapon.cpp | SLFItemWeapon.cpp |  | 2026-01-03 | 2026-01-03 |
| Items/SLFItemWeapon.h | SLFItemWeapon.h | **ASLFItemWeapon** | 2026-01-03 | 2026-01-03 |
| Items/SLFItemWeaponBossMace.cpp | SLFItemWeaponBossMace.cpp |  | 2026-01-03 | 2026-01-03 |
| Items/SLFItemWeaponBossMace.h | SLFItemWeaponBossMace.h | **ASLFItemWeaponBossMace** | 2026-01-03 | 2026-01-03 |
| Items/SLFItemWeaponGreatsword.cpp | SLFItemWeaponGreatsword.cpp |  | 2026-01-03 | 2026-01-03 |
| Items/SLFItemWeaponGreatsword.h | SLFItemWeaponGreatsword.h | **ASLFItemWeaponGreatsword** | 2026-01-03 | 2026-01-03 |
| Items/SLFItemWeaponKatana.cpp | SLFItemWeaponKatana.cpp |  | 2026-01-03 | 2026-01-03 |
| Items/SLFItemWeaponKatana.h | SLFItemWeaponKatana.h | **ASLFItemWeaponKatana** | 2026-01-03 | 2026-01-03 |
| Items/SLFItemWeaponPoisonSword.cpp | SLFItemWeaponPoisonSword.cpp |  | 2026-01-03 | 2026-01-03 |
| Items/SLFItemWeaponPoisonSword.h | SLFItemWeaponPoisonSword.h | **ASLFItemWeaponPoisonSword** | 2026-01-03 | 2026-01-03 |
| Items/SLFItemWeaponShield.cpp | SLFItemWeaponShield.cpp |  | 2026-01-03 | 2026-01-03 |
| Items/SLFItemWeaponShield.h | SLFItemWeaponShield.h | **ASLFItemWeaponShield** | 2026-01-03 | 2026-01-03 |
| Projectiles/SLFProjectileBossFireball.cpp | SLFProjectileBossFireball.cpp |  | 2026-01-03 | 2026-01-03 |
| Projectiles/SLFProjectileBossFireball.h | SLFProjectileBossFireball.h | **ASLFProjectileBossFireball** | 2026-01-03 | 2026-01-03 |
| Projectiles/SLFProjectileThrowingKnife.cpp | SLFProjectileThrowingKnife.cpp |  | 2026-01-03 | 2026-01-03 |
| Projectiles/SLFProjectileThrowingKnife.h | SLFProjectileThrowingKnife.h | **ASLFProjectileThrowingKnife** | 2026-01-03 | 2026-01-03 |
| Stats/SLFAttackPowerFire.h | SLFAttackPowerFire.h | **USLFAttackPowerFire** | 2026-01-03 | 2026-01-03 |
| Stats/SLFAttackPowerFrost.h | SLFAttackPowerFrost.h | **USLFAttackPowerFrost** | 2026-01-03 | 2026-01-03 |
| Stats/SLFAttackPowerHoly.h | SLFAttackPowerHoly.h | **USLFAttackPowerHoly** | 2026-01-03 | 2026-01-03 |
| Stats/SLFAttackPowerLightning.h | SLFAttackPowerLightning.h | **USLFAttackPowerLightning** | 2026-01-03 | 2026-01-03 |
| Stats/SLFAttackPowerMagic.h | SLFAttackPowerMagic.h | **USLFAttackPowerMagic** | 2026-01-03 | 2026-01-03 |
| Stats/SLFAttackPowerPhysical.h | SLFAttackPowerPhysical.h | **USLFAttackPowerPhysical** | 2026-01-03 | 2026-01-03 |
| Stats/SLFDamageNegationFire.h | SLFDamageNegationFire.h | **USLFDamageNegationFire** | 2026-01-03 | 2026-01-03 |
| Stats/SLFDamageNegationFrost.h | SLFDamageNegationFrost.h | **USLFDamageNegationFrost** | 2026-01-03 | 2026-01-03 |
| Stats/SLFDamageNegationHoly.h | SLFDamageNegationHoly.h | **USLFDamageNegationHoly** | 2026-01-03 | 2026-01-03 |
| Stats/SLFDamageNegationLightning.h | SLFDamageNegationLightning.h | **USLFDamageNegationLightning** | 2026-01-03 | 2026-01-03 |
| Stats/SLFDamageNegationMagic.h | SLFDamageNegationMagic.h | **USLFDamageNegationMagic** | 2026-01-03 | 2026-01-03 |
| Stats/SLFDamageNegationPhysical.h | SLFDamageNegationPhysical.h | **USLFDamageNegationPhysical** | 2026-01-03 | 2026-01-03 |
| Stats/SLFResistanceFocus.h | SLFResistanceFocus.h | **USLFResistanceFocus** | 2026-01-03 | 2026-01-03 |
| Stats/SLFResistanceImmunity.h | SLFResistanceImmunity.h | **USLFResistanceImmunity** | 2026-01-03 | 2026-01-03 |
| Stats/SLFResistanceRobustness.h | SLFResistanceRobustness.h | **USLFResistanceRobustness** | 2026-01-03 | 2026-01-03 |
| Stats/SLFResistanceVitality.h | SLFResistanceVitality.h | **USLFResistanceVitality** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatArcane.h | SLFStatArcane.h | **USLFStatArcane** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatDeathCurrency.cpp | SLFStatDeathCurrency.cpp |  | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatDeathCurrency.h | SLFStatDeathCurrency.h | **USLFStatDeathCurrency** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatDexterity.h | SLFStatDexterity.h | **USLFStatDexterity** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatEndurance.h | SLFStatEndurance.h | **USLFStatEndurance** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatFP.cpp | SLFStatFP.cpp |  | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatFP.h | SLFStatFP.h | **USLFStatFP** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatFaith.h | SLFStatFaith.h | **USLFStatFaith** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatHP.cpp | SLFStatHP.cpp |  | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatHP.h | SLFStatHP.h | **USLFStatHP** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatIncantationPower.cpp | SLFStatIncantationPower.cpp |  | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatIncantationPower.h | SLFStatIncantationPower.h | **USLFStatIncantationPower** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatIntelligence.h | SLFStatIntelligence.h | **USLFStatIntelligence** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatMind.h | SLFStatMind.h | **USLFStatMind** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatPoise.h | SLFStatPoise.h | **USLFStatPoise** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatStamina.h | SLFStatStamina.h | **USLFStatStamina** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatStance.cpp | SLFStatStance.cpp |  | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatStance.h | SLFStatStance.h | **USLFStatStance** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatStrength.h | SLFStatStrength.h | **USLFStatStrength** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatVigor.h | SLFStatVigor.h | **USLFStatVigor** | 2026-01-03 | 2026-01-03 |
| Stats/SLFStatWeight.h | SLFStatWeight.h | **USLFStatWeight** | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectBleed.cpp | SLFStatusEffectBleed.cpp |  | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectBleed.h | SLFStatusEffectBleed.h | **USLFStatusEffectBleed** | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectBurn.cpp | SLFStatusEffectBurn.cpp |  | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectBurn.h | SLFStatusEffectBurn.h | **USLFStatusEffectBurn** | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectCorruption.cpp | SLFStatusEffectCorruption.cpp |  | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectCorruption.h | SLFStatusEffectCorruption.h | **USLFStatusEffectCorruption** | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectFrostbite.cpp | SLFStatusEffectFrostbite.cpp |  | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectFrostbite.h | SLFStatusEffectFrostbite.h | **USLFStatusEffectFrostbite** | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectMadness.cpp | SLFStatusEffectMadness.cpp |  | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectMadness.h | SLFStatusEffectMadness.h | **USLFStatusEffectMadness** | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectPlague.cpp | SLFStatusEffectPlague.cpp |  | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectPlague.h | SLFStatusEffectPlague.h | **USLFStatusEffectPlague** | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectPoison.cpp | SLFStatusEffectPoison.cpp |  | 2026-01-03 | 2026-01-03 |
| StatusEffects/SLFStatusEffectPoison.h | SLFStatusEffectPoison.h | **USLFStatusEffectPoison** | 2026-01-03 | 2026-01-03 |

## C++ Components (`Source/SLFConversion/Components`)
**91 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| SLFAIStateMachineComponent.cpp | SLFAIStateMachineComponent.cpp | BeginPlay, CacheReferences, RefreshCachedAnimInstance, TickComponent, SetState | 2026-01-11 | 2026-03-16 |
| AC_ActionManager.cpp | AC_ActionManager.cpp | BeginPlay, TickComponent, GetStatManager_Implementation, GetInteractionManage... | 2026-01-03 | 2026-03-09 |
| AC_ActionManager.h | AC_ActionManager.h | **UAC_CombatManager, UAC_InteractionManager, UStatManagerComponent** | 2026-01-03 | 2026-03-09 |
| AC_CombatManager.cpp | AC_CombatManager.cpp | BeginPlay, TickComponent, SetGuardState_Implementation, GetIsGuarding_Impleme... | 2026-01-03 | 2026-03-09 |
| AC_CombatManager.h | AC_CombatManager.h | **UStatManagerComponent, UAnimMontage, UDataTable** | 2026-01-03 | 2026-03-09 |
| SLFCraftingManager.cpp | SLFCraftingManager.cpp | BeginPlay, DiscoverRecipe, GetDiscoveredRecipes, CanCraft, CraftItem | 2026-03-09 | 2026-03-09 |
| SLFCraftingManager.h | SLFCraftingManager.h | **USLFCraftingManager** | 2026-03-09 | 2026-03-09 |
| SLFZoneManagerComponent.cpp | SLFZoneManagerComponent.cpp | BeginPlay, SetCurrentZone, DiscoverZone, DefeatBoss, IsZoneDiscovered | 2026-02-27 | 2026-03-09 |
| SaveLoadManagerComponent.cpp | SaveLoadManagerComponent.cpp | BeginPlay, SaveToSlot_Implementation, SaveToCheckpoint_Implementation, AutoSa... | 2026-01-03 | 2026-03-09 |
| SLFZoneManagerComponent.h | SLFZoneManagerComponent.h | **USLFZoneManagerComponent** | 2026-02-27 | 2026-02-27 |
| SaveLoadManagerComponent.h | SaveLoadManagerComponent.h | **UStatManagerComponent, UInventoryManagerComponent, UEquipmentManagerCompone... | 2026-01-03 | 2026-02-27 |
| AIBossComponent.cpp | AIBossComponent.cpp | BeginPlay, SetFightActive_Implementation, EndFight_Implementation, SetPhase_I... | 2026-01-03 | 2026-02-18 |
| AIBossComponent.h | AIBossComponent.h | **UAnimMontage, UAudioComponent, ULevelSequencePlayer** | 2026-01-03 | 2026-02-18 |
| AICombatManagerComponent.cpp | AICombatManagerComponent.cpp | BeginPlay, HandleIncomingWeaponDamage_AI_Implementation, HandleProjectileDama... | 2026-01-03 | 2026-02-18 |
| CollisionManagerComponent.cpp | CollisionManagerComponent.cpp | BeginPlay, TickComponent, SetMultipliers_Implementation, GetMultipliers_Imple... | 2026-01-03 | 2026-02-18 |
| SLFAIStateMachineComponent.h | SLFAIStateMachineComponent.h | **AAIController, APawn, UAnimMontage** | 2026-01-11 | 2026-02-18 |
| AC_AI_InteractionManager.cpp | AC_AI_InteractionManager.cpp | BeginPlay, TickComponent, GetCurrentDialogEntry_Implementation, EventBeginDia... | 2026-01-03 | 2026-02-07 |
| AC_AI_InteractionManager.h | AC_AI_InteractionManager.h | **UAC_ProgressManager, UAnimMontage, UDataTable** | 2026-01-03 | 2026-02-07 |
| AC_SaveLoadManager.cpp | AC_SaveLoadManager.cpp | BeginPlay, TickComponent, GetOwnerPlayerController, GetOwnerPawn, OnAsyncSave... | 2026-01-03 | 2026-02-07 |
| AIInteractionManagerComponent.cpp | AIInteractionManagerComponent.cpp | GetCurrentDialogEntry, GetCurrentDialogEntryOutParams, BeginDialog_Implementa... | 2026-01-03 | 2026-02-07 |
| AIInteractionManagerComponent.h | AIInteractionManagerComponent.h | **UDataAsset, UProgressManagerComponent, UUserWidget** | 2026-01-03 | 2026-02-07 |
| InventoryManagerComponent.cpp | InventoryManagerComponent.cpp | BeginPlay, EquipStartingFlask, EquipStartingSpell, AddItem_Implementation, Re... | 2026-01-03 | 2026-02-07 |
| ProgressManagerComponent.cpp | ProgressManagerComponent.cpp | BeginPlay, GetProgress_Implementation, SetProgress_Implementation, GetRequire... | 2026-01-03 | 2026-02-07 |
| ProgressManagerComponent.h | ProgressManagerComponent.h | **UProgressManagerComponent** | 2026-01-03 | 2026-02-07 |
| StatusEffectManagerComponent.cpp | StatusEffectManagerComponent.cpp | GetTagFromStatusEffectAsset, GetOwnerResistanceStatValue, TriggerStatusEffect... | 2026-01-03 | 2026-02-07 |
| AC_AI_CombatManager.cpp | AC_AI_CombatManager.cpp | OnRegister, BeginPlay, TickComponent, TryGetAbility_Implementation, EvaluateA... | 2026-01-03 | 2026-01-30 |
| AC_AI_CombatManager.h | AC_AI_CombatManager.h | **UAC_StatManager, UAnimMontage, UDataTable** | 2026-01-03 | 2026-01-30 |
| AC_AI_Boss.cpp | AC_AI_Boss.cpp | BeginPlay, TickComponent, SetFightActive_Implementation, SetPhase_Implementat... | 2026-01-03 | 2026-01-29 |
| AC_EquipmentManager.cpp | AC_EquipmentManager.cpp | BeginPlay, TickComponent, GetLifetimeReplicatedProps, IsSlotOccupied_Implemen... | 2026-01-03 | 2026-01-29 |
| AC_EquipmentManager.h | AC_EquipmentManager.h | **UAC_BuffManager, UAnimMontage, UDataTable** | 2026-01-03 | 2026-01-29 |
| AC_InventoryManager.cpp | AC_InventoryManager.cpp | BeginPlay, TickComponent, GetInventoryWidget_Implementation, GetAmountOfItem_... | 2026-01-03 | 2026-01-29 |
| AC_StatusEffectManager.cpp | AC_StatusEffectManager.cpp | BeginPlay, TickComponent, StartBuildup_Implementation, StopBuildup_Implementa... | 2026-01-03 | 2026-01-29 |
| AICombatManagerComponent.h | AICombatManagerComponent.h | **UDataAsset, UPrimaryDataAsset, UAnimMontage** | 2026-01-03 | 2026-01-29 |
| InventoryManagerComponent.h | InventoryManagerComponent.h | **UDataAsset, UUserWidget, UInventoryManagerComponent** | 2026-01-03 | 2026-01-29 |
| AC_InteractionManager.cpp | AC_InteractionManager.cpp | BeginPlay, TickComponent, GetLifetimeReplicatedProps, EventOnRest, OnRestingP... | 2026-01-03 | 2026-01-27 |
| EquipmentManagerComponent.cpp | EquipmentManagerComponent.cpp | BeginPlay, GetLifetimeReplicatedProps, EquipWeaponToSlot_Implementation, Uneq... | 2026-01-03 | 2026-01-27 |
| AC_InteractionManager.h | AC_InteractionManager.h | **UAnimMontage, UDataTable, UPrimaryDataAsset** | 2026-01-03 | 2026-01-26 |
| AC_InventoryManager.h | AC_InventoryManager.h | **UAnimMontage, UDataTable, UPrimaryDataAsset** | 2026-01-03 | 2026-01-26 |
| AC_StatManager.cpp | AC_StatManager.cpp | BeginPlay, TickComponent, GetStat_Implementation, GetAllStats_Implementation,... | 2026-01-03 | 2026-01-26 |
| InteractionManagerComponent.cpp | InteractionManagerComponent.cpp | BeginPlay, TickComponent, GetLifetimeReplicatedProps, TraceForInteractables_I... | 2026-01-03 | 2026-01-26 |
| LadderManagerComponent.h | LadderManagerComponent.h | **UDataAsset, UAnimMontage, UAnimInstance** | 2026-01-03 | 2026-01-26 |
| LootDropManagerComponent.cpp | LootDropManagerComponent.cpp | IsOverrideItemValid, GetRandomItemFromTable_Implementation, PickItem_Implemen... | 2026-01-03 | 2026-01-26 |
| RadarElementComponent.cpp | RadarElementComponent.cpp | EndPlay, AddTrackedElement_Implementation, RemoveTrackedElement_Implementation | 2026-01-03 | 2026-01-26 |
| RadarElementComponent.h | RadarElementComponent.h | **UUserWidget, UW_Radar_TrackedElement, URadarElementComponent** | 2026-01-03 | 2026-01-26 |
| RadarManagerComponent.cpp | RadarManagerComponent.cpp | BeginPlay, PopulateDefaultCardinalData, ShowRadar_Implementation, HideRadar_I... | 2026-01-03 | 2026-01-26 |
| RadarManagerComponent.h | RadarManagerComponent.h | **UCameraComponent, UUserWidget, URadarElementComponent** | 2026-01-03 | 2026-01-26 |
| StatManagerComponent.cpp | StatManagerComponent.cpp | BeginPlay, GetStat_Implementation, GetAllStats_Implementation, GetStatsForCat... | 2026-01-03 | 2026-01-26 |
| StatusEffectManagerComponent.h | StatusEffectManagerComponent.h | **UDataAsset, UPDA_StatusEffect, UStatusEffectManagerComponent** | 2026-01-03 | 2026-01-26 |
| LadderManagerComponent.cpp | LadderManagerComponent.cpp | BeginPlay, TickComponent, OnMoveInput, HandleClimbingInput, SetCurrentLadder_... | 2026-01-03 | 2026-01-19 |
| AC_CollisionManager.cpp | AC_CollisionManager.cpp | BeginPlay, TickComponent, GetTraceLocations_Implementation, SubsteppedTrace_I... | 2026-01-03 | 2026-01-16 |
| AIBehaviorManagerComponent.cpp | AIBehaviorManagerComponent.cpp | BeginPlay, SetState_Implementation, SetTarget_Implementation, SetPatrolPath_I... | 2026-01-03 | 2026-01-16 |
| StatManagerComponent.h | StatManagerComponent.h | **UDataAsset, UStatManagerComponent** | 2026-01-03 | 2026-01-16 |
| AC_AI_BehaviorManager.cpp | AC_AI_BehaviorManager.cpp | BeginPlay, TickComponent, GetBlackboard_Implementation, SetKeyValue_Implement... | 2026-01-03 | 2026-01-12 |
| CombatManagerComponent.cpp | CombatManagerComponent.cpp | BeginPlay, HandleIncomingWeaponDamage_Implementation, HandleProjectileDamage_... | 2026-01-03 | 2026-01-12 |
| AC_StatManager.h | AC_StatManager.h | **UAnimMontage, UDataTable, UPrimaryDataAsset** | 2026-01-03 | 2026-01-10 |
| ActionManagerComponent.cpp | ActionManagerComponent.cpp | BeginPlay, GetDirectionalDodge, GetCombatManager, GetInteractionManager, GetS... | 2026-01-03 | 2026-01-08 |
| AC_ProgressManager.cpp | AC_ProgressManager.cpp | BeginPlay, TickComponent, SetProgress_Implementation, GetProgress_Implementat... | 2026-01-03 | 2026-01-07 |
| AC_ProgressManager.h | AC_ProgressManager.h | **UAnimMontage, UDataTable, UPrimaryDataAsset** | 2026-01-03 | 2026-01-07 |
| AC_SaveLoadManager.h | AC_SaveLoadManager.h | **UAC_EquipmentManager, UAC_InventoryManager, UAC_ProgressManager** | 2026-01-03 | 2026-01-07 |
| AC_StatusEffectManager.h | AC_StatusEffectManager.h | **UAnimMontage, UDataTable, UPrimaryDataAsset** | 2026-01-03 | 2026-01-07 |
| EquipmentManagerComponent.h | EquipmentManagerComponent.h | **UDataTable, UDataAsset, UAnimSequenceBase** | 2026-01-03 | 2026-01-07 |
| AC_AI_BehaviorManager.h | AC_AI_BehaviorManager.h | **AB_PatrolPath, UAnimMontage, UDataTable** | 2026-01-03 | 2026-01-03 |
| AC_AI_Boss.h | AC_AI_Boss.h | **UAnimMontage, UDataTable, UPrimaryDataAsset** | 2026-01-03 | 2026-01-03 |
| AC_BuffManager.cpp | AC_BuffManager.cpp | BeginPlay, TickComponent, TryAddBuff_Implementation, DelayedRemoveBuff_Implem... | 2026-01-03 | 2026-01-03 |
| AC_BuffManager.h | AC_BuffManager.h | **UAnimMontage, UDataTable, UPrimaryDataAsset** | 2026-01-03 | 2026-01-03 |
| AC_CollisionManager.h | AC_CollisionManager.h | **UAnimMontage, UDataTable, UPrimaryDataAsset** | 2026-01-03 | 2026-01-03 |
| AC_DebugCentral.cpp | AC_DebugCentral.cpp | BeginPlay, TickComponent, LateInitialize, CacheComponentReferences, PopulateC... | 2026-01-03 | 2026-01-03 |
| AC_DebugCentral.h | AC_DebugCentral.h | **UAC_ActionManager, UAC_BuffManager, UAC_CombatManager** | 2026-01-03 | 2026-01-03 |
| AC_InputBuffer.cpp | AC_InputBuffer.cpp | BeginPlay, TickComponent, QueueAction_Implementation, ConsumeInputBuffer_Impl... | 2026-01-03 | 2026-01-03 |
| AC_InputBuffer.h | AC_InputBuffer.h | **UAnimMontage, UDataTable, UPrimaryDataAsset** | 2026-01-03 | 2026-01-03 |
| AC_LadderManager.cpp | AC_LadderManager.cpp | BeginPlay, TickComponent, SetIsOnGround_Implementation, SetIsClimbing_Impleme... | 2026-01-03 | 2026-01-03 |
| AC_LadderManager.h | AC_LadderManager.h | **UAnimMontage, UDataTable, UPrimaryDataAsset** | 2026-01-03 | 2026-01-03 |
| AC_LootDropManager.cpp | AC_LootDropManager.cpp | BeginPlay, TickComponent, GetRandomItemFromTable_Implementation, IsOverrideIt... | 2026-01-03 | 2026-01-03 |
| AC_LootDropManager.h | AC_LootDropManager.h | **UAnimMontage, UDataTable, UPrimaryDataAsset** | 2026-01-03 | 2026-01-03 |
| AC_RadarElement.cpp | AC_RadarElement.cpp | BeginPlay, EndPlay, TickComponent, RegisterWithRadarManager, UnregisterFromRa... | 2026-01-03 | 2026-01-03 |
| AC_RadarElement.h | AC_RadarElement.h | **UAnimMontage, UDataTable, UPrimaryDataAsset** | 2026-01-03 | 2026-01-03 |
| AC_RadarManager.cpp | AC_RadarManager.cpp | BeginPlay, TickComponent, SetupPlayerIcon_Implementation, SetupCardinals_Impl... | 2026-01-03 | 2026-01-03 |
| AC_RadarManager.h | AC_RadarManager.h | **UAC_RadarElement, UAnimMontage, UDataTable** | 2026-01-03 | 2026-01-03 |
| AIBehaviorManagerComponent.h | AIBehaviorManagerComponent.h | **UBehaviorTree, UBlackboardComponent, UAIBehaviorManagerComponent**; UMETA | 2026-01-03 | 2026-01-03 |
| ActionManagerComponent.h | ActionManagerComponent.h | **UDataAsset, UCombatManagerComponent, UInteractionManagerComponent** | 2026-01-03 | 2026-01-03 |
| BuffManagerComponent.cpp | BuffManagerComponent.cpp | GetBuffsWithTag_Implementation, RemoveBuffWithTag_Implementation, RemoveAllBu... | 2026-01-03 | 2026-01-03 |
| BuffManagerComponent.h | BuffManagerComponent.h | **UDataAsset, UBuffManagerComponent** | 2026-01-03 | 2026-01-03 |
| CollisionManagerComponent.h | CollisionManagerComponent.h | **UCollisionManagerComponent** | 2026-01-03 | 2026-01-03 |
| CombatManagerComponent.h | CombatManagerComponent.h | **UDataAsset, UAnimMontage, UCameraShakeBase** | 2026-01-03 | 2026-01-03 |
| DebugCentralComponent.cpp | DebugCentralComponent.cpp | BeginPlay, TickComponent, LateInitialize_Implementation | 2026-01-03 | 2026-01-03 |
| DebugCentralComponent.h | DebugCentralComponent.h | **UUserWidget, UDebugCentralComponent** | 2026-01-03 | 2026-01-03 |
| InputBufferComponent.cpp | InputBufferComponent.cpp | BeginPlay, QueueAction_Implementation, ToggleBuffer_Implementation, ConsumeIn... | 2026-01-03 | 2026-01-03 |
| InputBufferComponent.h | InputBufferComponent.h | **UInputBufferComponent** | 2026-01-03 | 2026-01-03 |
| InteractionManagerComponent.h | InteractionManagerComponent.h | **UInteractionManagerComponent** | 2026-01-03 | 2026-01-03 |
| LootDropManagerComponent.h | LootDropManagerComponent.h | **ULootDropManagerComponent** | 2026-01-03 | 2026-01-03 |
| SLFComponents.h | SLFComponents.h |  | 2026-01-03 | 2026-01-03 |

## C++ Dungeon (`Source/SLFConversion/Dungeon`)
**15 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| SLFCaveEventListener.cpp | SLFCaveEventListener.cpp | OnPostDungeonBuild_Implementation | 2026-03-09 | 2026-03-09 |
| SLFCaveEventListener.h | SLFCaveEventListener.h | **USLFCaveEventListener** | 2026-03-09 | 2026-03-09 |
| SLFCaveMarkerEmitter.cpp | SLFCaveMarkerEmitter.cpp | EmitMarkers_Implementation | 2026-03-09 | 2026-03-09 |
| SLFCaveMarkerEmitter.h | SLFCaveMarkerEmitter.h | **USLFCaveMarkerEmitter**; EmitMarkers_Implementation | 2026-03-09 | 2026-03-09 |
| SLFCaveMazeBuilder.cpp | SLFCaveMazeBuilder.cpp | CaptureCaveFloor, ExportCellGraphToJson, GenerateTemplateLayout, GenerateOver... | 2026-03-09 | 2026-03-09 |
| SLFCaveMazeBuilder.h | SLFCaveMazeBuilder.h | **UWorld, ADungeon, FSLFCaveMazeBuilder** | 2026-03-09 | 2026-03-09 |
| SLFCaveMazeTypes.h | SLFCaveMazeTypes.h |  | 2026-03-09 | 2026-03-09 |
| SLFCaveRandomTransform.cpp | SLFCaveRandomTransform.cpp | Execute | 2026-03-09 | 2026-03-09 |
| SLFCaveRandomTransform.h | SLFCaveRandomTransform.h | **USLFCaveRandomTransform**; Execute | 2026-03-09 | 2026-03-09 |
| SLFCaveRoomSelector.cpp | SLFCaveRoomSelector.cpp | SelectNode_Implementation | 2026-03-09 | 2026-03-09 |
| SLFCaveRoomSelector.h | SLFCaveRoomSelector.h | **USLFCaveRoomSelector**; SelectNode_Implementation | 2026-03-09 | 2026-03-09 |
| SLFCaveSpawnLogic.cpp | SLFCaveSpawnLogic.cpp | OnItemSpawn_Implementation, GetLightColor | 2026-03-09 | 2026-03-09 |
| SLFCaveSpawnLogic.h | SLFCaveSpawnLogic.h | **USLFCaveSpawnLogic** | 2026-03-09 | 2026-03-09 |
| SLFProceduralCaveManager.cpp | SLFProceduralCaveManager.cpp | BeginPlay, BuildCaveDungeon, ConfigureDungeon, ForceMaterialOnVoxelMeshes, On... | 2026-03-09 | 2026-03-09 |
| SLFProceduralCaveManager.h | SLFProceduralCaveManager.h | **ADungeon, UCellFlowAsset, UDungeonThemeAsset**; QueryPathPositions | 2026-03-09 | 2026-03-09 |

## C++ Framework (`Source/SLFConversion/Framework`)
**16 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| SLFPlayerController.cpp | SLFPlayerController.cpp | BeginPlay, SetupInputComponent, HandleGameMenuInput, Native_InitializeHUD, Ev... | 2026-01-03 | 2026-03-09 |
| SLFPlayerController.h | SLFPlayerController.h | **UInputAction, UInputMappingContext, UW_HUD** | 2026-01-03 | 2026-03-09 |
| SLFGameMode.cpp | SLFGameMode.cpp | BeginPlay | 2026-01-03 | 2026-02-27 |
| SLFGameInstance.cpp | SLFGameInstance.cpp | Init, GetAllSaveSlots_Implementation, SetLastSlotNameToActive_Implementation,... | 2026-01-03 | 2026-02-07 |
| SLFGameInstance.h | SLFGameInstance.h | **USG_SaveSlots, UPrimaryDataAsset, USLFGameInstance** | 2026-01-03 | 2026-01-29 |
| SLFAIController.cpp | SLFAIController.cpp | BeginPlay, OnPossess, Tick, InitializeBehavior_Implementation | 2026-01-03 | 2026-01-26 |
| SLFAIController.h | SLFAIController.h | **ASLFAIController** | 2026-01-03 | 2026-01-12 |
| SLFGameMode.h | SLFGameMode.h | **ASLFGameMode** | 2026-01-03 | 2026-01-03 |
| SLFGameState.cpp | SLFGameState.cpp | BeginPlay | 2026-01-03 | 2026-01-03 |
| SLFGameState.h | SLFGameState.h | **ASLFGameState** | 2026-01-03 | 2026-01-03 |
| SLFPlayerState.cpp | SLFPlayerState.cpp | BeginPlay | 2026-01-03 | 2026-01-03 |
| SLFPlayerState.h | SLFPlayerState.h | **ASLFPlayerState** | 2026-01-03 | 2026-01-03 |
| SLFSaveGame.cpp | SLFSaveGame.cpp |  | 2026-01-03 | 2026-01-03 |
| SLFSaveGame.h | SLFSaveGame.h | **USLFSaveGame** | 2026-01-03 | 2026-01-03 |
| SLFSaveSlots.cpp | SLFSaveSlots.cpp |  | 2026-01-03 | 2026-01-03 |
| SLFSaveSlots.h | SLFSaveSlots.h | **USLFSaveSlots** | 2026-01-03 | 2026-01-03 |

## C++ GameFramework (`Source/SLFConversion/GameFramework`)
**10 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| GI_SoulslikeFramework.cpp | GI_SoulslikeFramework.cpp |  | 2026-01-03 | 2026-02-07 |
| GI_SoulslikeFramework.h | GI_SoulslikeFramework.h | **UGI_SoulslikeFramework** | 2026-01-03 | 2026-02-07 |
| PC_Menu_SoulslikeFramework.cpp | PC_Menu_SoulslikeFramework.cpp | BeginPlay, CreateMainMenuWidget, SetupMenuInput, CheckSaveGameExists, SetActi... | 2026-01-03 | 2026-02-07 |
| PC_Menu_SoulslikeFramework.h | PC_Menu_SoulslikeFramework.h | **UW_MainMenu, UInputMappingContext, UInputAction** | 2026-01-03 | 2026-02-07 |
| PC_SoulslikeFramework.cpp | PC_SoulslikeFramework.cpp | BeginPlay, SetupInputComponent, HandleNavigateUp, HandleNavigateDown, HandleN... | 2026-01-03 | 2026-02-07 |
| PC_SoulslikeFramework.h | PC_SoulslikeFramework.h | **UW_HUD, ULevelSequencePlayer, UInventoryManagerComponent** | 2026-01-03 | 2026-01-29 |
| GM_Menu_SoulslikeFramework.cpp | GM_Menu_SoulslikeFramework.cpp |  | 2026-01-03 | 2026-01-03 |
| GM_Menu_SoulslikeFramework.h | GM_Menu_SoulslikeFramework.h | **AGM_Menu_SoulslikeFramework** | 2026-01-03 | 2026-01-03 |
| GM_SoulslikeFramework.cpp | GM_SoulslikeFramework.cpp |  | 2026-01-03 | 2026-01-03 |
| GM_SoulslikeFramework.h | GM_SoulslikeFramework.h | **AGM_SoulslikeFramework** | 2026-01-03 | 2026-01-03 |

## C++ Interfaces (`Source/SLFConversion/Interfaces`)
**37 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| SLFPlayerInterface.h | SLFPlayerInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-02-07 |
| BPI_AIC.h | BPI_AIC.h | **UBPI_AIC, IBPI_AIC** | 2026-01-03 | 2026-01-03 |
| BPI_BossDoor.h | BPI_BossDoor.h | **UBPI_BossDoor, IBPI_BossDoor** | 2026-01-03 | 2026-01-03 |
| BPI_Controller.h | BPI_Controller.h | **UActorComponent, UUserWidget, UMaterialInterface** | 2026-01-03 | 2026-01-03 |
| BPI_DestructibleHelper.h | BPI_DestructibleHelper.h | **UBPI_DestructibleHelper, IBPI_DestructibleHelper** | 2026-01-03 | 2026-01-03 |
| BPI_Enemy.h | BPI_Enemy.h | **AB_PatrolPath, UBPI_Enemy, IBPI_Enemy** | 2026-01-03 | 2026-01-03 |
| BPI_EnemyHealthbar.h | BPI_EnemyHealthbar.h | **UBPI_EnemyHealthbar, IBPI_EnemyHealthbar** | 2026-01-03 | 2026-01-03 |
| BPI_Executable.h | BPI_Executable.h | **UBPI_Executable, IBPI_Executable** | 2026-01-03 | 2026-01-03 |
| BPI_ExecutionIndicator.h | BPI_ExecutionIndicator.h | **UBPI_ExecutionIndicator, IBPI_ExecutionIndicator** | 2026-01-03 | 2026-01-03 |
| BPI_GameInstance.h | BPI_GameInstance.h | **UPrimaryDataAsset, UBPI_GameInstance, IBPI_GameInstance** | 2026-01-03 | 2026-01-03 |
| BPI_GenericCharacter.h | BPI_GenericCharacter.h | **USceneComponent, UAnimInstance, UAnimMontage**; PlaySoftSoundAtLocation, Sp... | 2026-01-03 | 2026-01-03 |
| BPI_Interactable.h | BPI_Interactable.h | **UBPI_Interactable, IBPI_Interactable** | 2026-01-03 | 2026-01-03 |
| BPI_Item.h | BPI_Item.h | **UBPI_Item, IBPI_Item** | 2026-01-03 | 2026-01-03 |
| BPI_MainMenu.h | BPI_MainMenu.h | **UBPI_MainMenu, IBPI_MainMenu** | 2026-01-03 | 2026-01-03 |
| BPI_NPC.h | BPI_NPC.h | **UBPI_NPC, IBPI_NPC** | 2026-01-03 | 2026-01-03 |
| BPI_Player.h | BPI_Player.h | **ULevelSequence, UAnimMontage, UActorComponent** | 2026-01-03 | 2026-01-03 |
| BPI_Projectile.h | BPI_Projectile.h | **UBPI_Projectile, IBPI_Projectile** | 2026-01-03 | 2026-01-03 |
| BPI_RestingPoint.h | BPI_RestingPoint.h | **UBPI_RestingPoint, IBPI_RestingPoint** | 2026-01-03 | 2026-01-03 |
| BPI_StatEntry.h | BPI_StatEntry.h | **UUserWidget, UBPI_StatEntry, IBPI_StatEntry** | 2026-01-03 | 2026-01-03 |
| SLFAICInterface.h | SLFAICInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFBossDoorInterface.h | SLFBossDoorInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFControllerInterface.h | SLFControllerInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFDestructibleHelperInterface.h | SLFDestructibleHelperInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFEnemyHealthbarInterface.h | SLFEnemyHealthbarInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFEnemyInterface.h | SLFEnemyInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFExecutableInterface.h | SLFExecutableInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFExecutionIndicatorInterface.h | SLFExecutionIndicatorInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFGameInstanceInterface.h | SLFGameInstanceInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFGenericCharacterInterface.h | SLFGenericCharacterInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFInteractableInterface.h | SLFInteractableInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFInterfaces.h | SLFInterfaces.h |  | 2026-01-03 | 2026-01-03 |
| SLFItemInterface.h | SLFItemInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFMainMenuInterface.h | SLFMainMenuInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFNPCInterface.h | SLFNPCInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFProjectileInterface.h | SLFProjectileInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFRestingPointInterface.h | SLFRestingPointInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |
| SLFStatEntryInterface.h | SLFStatEntryInterface.h | **UAnimMontage, UAnimInstance, USoundBase** | 2026-01-03 | 2026-01-03 |

## C++ Testing (`Source/SLFConversion/Testing`)
**6 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| SLFPIETestRunner.cpp | SLFPIETestRunner.cpp | Initialize, Deinitialize, RunAllTests, RunTest, RunActionSystemTest | 2026-01-03 | 2026-03-22 |
| SLFTestManager.cpp | SLFTestManager.cpp | BeginPlay, EndPlay, RunTest, SpawnTestEnemy, CaptureScreenshot | 2026-02-18 | 2026-02-18 |
| SLFTestManager.h | SLFTestManager.h | **ASLFTestManager** | 2026-02-18 | 2026-02-18 |
| SLFPIETestRunner.h | SLFPIETestRunner.h | **USLFPIETestRunner, FSLFTestCommands** | 2026-01-03 | 2026-02-07 |
| SLFInputSimulator.cpp | SLFInputSimulator.cpp | Initialize, Deinitialize, GetLocalPlayerController, InjectKeyEvent, SimulateK... | 2026-01-03 | 2026-01-03 |
| SLFInputSimulator.h | SLFInputSimulator.h | **USLFInputSimulator, USLFInputSimulatorLibrary** | 2026-01-03 | 2026-01-03 |

## C++ Tests (`Source/SLFConversion/Tests`)
**8 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| SLFSpawnTests.cpp | SLFSpawnTests.cpp | RunTest | 2026-01-12 | 2026-02-27 |
| SLFAITests.cpp | SLFAITests.cpp | RunTest | 2026-01-12 | 2026-01-12 |
| SLFAnimNotifyTests.cpp | SLFAnimNotifyTests.cpp | RunTest | 2026-01-12 | 2026-01-12 |
| SLFCombatTests.cpp | SLFCombatTests.cpp | RunTest, Clamp | 2026-01-12 | 2026-01-12 |
| SLFComponentTests.cpp | SLFComponentTests.cpp | RunTest | 2026-01-12 | 2026-01-12 |
| SLFFunctionalTests.cpp | SLFFunctionalTests.cpp | RunTest | 2026-01-12 | 2026-01-12 |
| SLFStatActionTests.cpp | SLFStatActionTests.cpp | RunTest | 2026-01-12 | 2026-01-12 |
| SLFWorldTests.cpp | SLFWorldTests.cpp | RunTest | 2026-01-12 | 2026-01-12 |

## C++ Utilities (`Source/SLFConversion/Utilities`)
**2 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| SLFNiagaraFactory.cpp | SLFNiagaraFactory.cpp | CreateMagicProjectileEffect, CreateImpactBurstEffect, CreateFireSpellEffect, ... | 2026-02-07 | 2026-02-07 |
| SLFNiagaraFactory.h | SLFNiagaraFactory.h | **USLFNiagaraFactory**; CreateMagicProjectileEffect, CreateImpactBurstEffect | 2026-02-07 | 2026-02-07 |

## C++ Widgets (`Source/SLFConversion/Widgets`)
**246 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| W_WorldMap.cpp | W_WorldMap.cpp | RebuildWidget, NativeConstruct, NativeOnKeyDown, Handled, NativeOnMouseButton... | 2026-02-27 | 2026-03-09 |
| W_WorldMap.h | W_WorldMap.h | **UImage, UCanvasPanel, UTextBlock** | 2026-02-27 | 2026-03-09 |
| W_Crafting.cpp | W_Crafting.cpp | NativeConstruct, NativeDestruct, NativeOnKeyDown, Handled, CacheWidgetReferences | 2026-01-03 | 2026-02-27 |
| W_GameMenu.cpp | W_GameMenu.cpp | NativePreConstruct, NativeConstruct, NativeDestruct, NativeOnKeyDown, Handled | 2026-01-03 | 2026-02-27 |
| W_GameMenu.h | W_GameMenu.h | **UW_GameMenu_Button, UWidgetAnimation, UPanelWidget** | 2026-01-03 | 2026-02-27 |
| W_HUD.cpp | W_HUD.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetTargetWidgetVisibi... | 2026-01-03 | 2026-02-27 |
| W_HUD.h | W_HUD.h | **UW_LoadingScreen, UW_Inventory, UW_Equipment** | 2026-01-03 | 2026-02-27 |
| W_MainMenu.cpp | W_MainMenu.cpp | NativeConstruct, NativeDestruct, NativeOnKeyDown, Handled, InitializeButtons | 2026-01-03 | 2026-02-27 |
| W_MainMenu.h | W_MainMenu.h | **UW_LoadingScreen, UW_MainMenu_Button, UW_CharacterSelection** | 2026-01-03 | 2026-02-27 |
| W_CharacterSelection.cpp | W_CharacterSelection.cpp | NativeConstruct, NativeDestruct, InitializeAndStoreClassAssets_Implementation... | 2026-01-03 | 2026-02-07 |
| W_CharacterSelection.h | W_CharacterSelection.h | **UW_CharacterSelectionCard, UScrollBox, UW_CharacterSelection** | 2026-01-03 | 2026-02-07 |
| W_CharacterSelectionCard.cpp | W_CharacterSelectionCard.cpp | NativeConstruct, NativeDestruct, PopulateCardDisplay, SetCardSelected_Impleme... | 2026-01-03 | 2026-02-07 |
| W_CharacterSelectionCard.h | W_CharacterSelectionCard.h | **UTextBlock, UImage, UBorder** | 2026-01-03 | 2026-02-07 |
| W_GenericButton.cpp | W_GenericButton.cpp | NativePreConstruct, NativeConstruct, NativeDestruct, CacheWidgetReferences, S... | 2026-01-03 | 2026-02-07 |
| W_GenericButton.h | W_GenericButton.h | **UW_GenericButton, UButton** | 2026-01-03 | 2026-02-07 |
| W_LevelUp.cpp | W_LevelUp.cpp | NativeConstruct, NativeDestruct, NativeOnKeyDown, Handled, OnStatsInitialized | 2026-01-03 | 2026-02-07 |
| W_LoadGame.cpp | W_LoadGame.cpp | NativeConstruct, NativeDestruct, InitializeSaveSlots_Implementation, EventNav... | 2026-01-03 | 2026-02-07 |
| W_LoadGame.h | W_LoadGame.h | **UW_LoadGame_Entry, UW_LoadGame** | 2026-01-03 | 2026-02-07 |
| W_LoadGame_Entry.cpp | W_LoadGame_Entry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, ApplySaveDataToWidget... | 2026-01-03 | 2026-02-07 |
| W_LoadGame_Entry.h | W_LoadGame_Entry.h | **UW_LoadingScreen, USG_SoulslikeFramework, UW_LoadGame_Entry** | 2026-01-03 | 2026-02-07 |
| W_MainMenu_Button.cpp | W_MainMenu_Button.cpp | NativePreConstruct, NativeConstruct, NativeDestruct, SetMenuButtonSelected_Im... | 2026-01-03 | 2026-02-07 |
| W_NPC_Window.h | W_NPC_Window.h | **UW_RestMenu_Button, UW_NPC_Window_Vendor, UAIInteractionManagerComponent** | 2026-01-03 | 2026-02-07 |
| W_Settings.cpp | W_Settings.cpp | NativeConstruct, NativeDestruct, NativeOnPreviewKeyDown, Unhandled, Handled | 2026-01-03 | 2026-02-07 |
| W_Settings.h | W_Settings.h | **UW_GenericButton, UW_Settings_CategoryEntry, UW_Settings_Entry** | 2026-01-03 | 2026-02-07 |
| W_Settings_CategoryEntry.cpp | W_Settings_CategoryEntry.cpp | NativePreConstruct, NativeConstruct, NativeDestruct, CacheWidgetReferences, B... | 2026-01-03 | 2026-02-07 |
| W_Settings_CategoryEntry.h | W_Settings_CategoryEntry.h | **UW_Settings, UButton, UImage** | 2026-01-03 | 2026-02-07 |
| W_Settings_CenteredText.cpp | W_Settings_CenteredText.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventSetText_Implemen... | 2026-01-03 | 2026-02-07 |
| W_Settings_CenteredText.h | W_Settings_CenteredText.h | **UW_Settings_CenteredText, UTextBlock** | 2026-01-03 | 2026-02-07 |
| W_Settings_Entry.cpp | W_Settings_Entry.cpp | NativePreConstruct, NativeConstruct, NativeDestruct, ApplyVisualConfig, SetEn... | 2026-01-03 | 2026-02-07 |
| W_Settings_Entry.h | W_Settings_Entry.h | **UBorder, UTextBlock, USlider** | 2026-01-03 | 2026-02-07 |
| W_Settings_PlayerCard.cpp | W_Settings_PlayerCard.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventOnLevelUpdated_I... | 2026-01-03 | 2026-02-07 |
| W_Settings_PlayerCard.h | W_Settings_PlayerCard.h | **UW_Settings_PlayerCard, UTextBlock, UTextBlock** | 2026-01-03 | 2026-02-07 |
| W_Settings_QuitConfirmation.cpp | W_Settings_QuitConfirmation.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, OnYesButtonPressed, O... | 2026-01-03 | 2026-02-07 |
| W_Settings_QuitConfirmation.h | W_Settings_QuitConfirmation.h | **UW_GenericButton, UTextBlock, UW_Settings_QuitConfirmation** | 2026-01-03 | 2026-02-07 |
| W_StatBlock_LevelUp.cpp | W_StatBlock_LevelUp.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, OnStatsInitialized, I... | 2026-01-03 | 2026-02-07 |
| W_InventoryActionAmount.cpp | W_InventoryActionAmount.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, BindButtonEvents, Upd... | 2026-01-03 | 2026-02-01 |
| W_InventoryActionAmount.h | W_InventoryActionAmount.h | **UW_InventorySlot, UW_GenericButton, UButton** | 2026-01-03 | 2026-02-01 |
| W_NPC_Window.cpp | W_NPC_Window.cpp | NativeConstruct, NativeDestruct, NativeOnKeyDown, Handled, CacheWidgetReferences | 2026-01-03 | 2026-02-01 |
| W_NPC_Window_Vendor.cpp | W_NPC_Window_Vendor.cpp | NativeConstruct, NativeDestruct, NativeOnKeyDown, Handled, CacheWidgetReferences | 2026-01-03 | 2026-02-01 |
| W_NPC_Window_Vendor.h | W_NPC_Window_Vendor.h | **UW_Inventory_CategoryEntry, UW_VendorSlot, UW_VendorAction** | 2026-01-03 | 2026-02-01 |
| W_VendorAction.cpp | W_VendorAction.cpp | NativeConstruct, NativeDestruct, NativeOnKeyDown, Handled, CacheWidgetReferences | 2026-01-03 | 2026-02-01 |
| W_VendorAction.h | W_VendorAction.h | **UW_VendorSlot, UTextBlock, UW_GenericButton** | 2026-01-03 | 2026-02-01 |
| W_VendorSlot.cpp | W_VendorSlot.cpp | NativeConstruct, NativeDestruct, NativeOnMouseButtonDown, Handled, NativeOnMo... | 2026-01-03 | 2026-02-01 |
| W_VendorSlot.h | W_VendorSlot.h | **UButton, UW_VendorSlot** | 2026-01-03 | 2026-02-01 |
| W_StatusEffectBar.cpp | W_StatusEffectBar.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, SetupEffect, OnBuildu... | 2026-01-03 | 2026-01-30 |
| W_StatusEffectBar.h | W_StatusEffectBar.h | **UB_StatusEffect, UProgressBar, UImage** | 2026-01-03 | 2026-01-30 |
| W_Equipment.cpp | W_Equipment.cpp | NativeConstruct, NativeDestruct, NativeOnKeyDown, Handled, CacheWidgetReferences | 2026-01-03 | 2026-01-29 |
| W_ItemWheelSlot.cpp | W_ItemWheelSlot.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, HandleOnItemEquippedT... | 2026-01-03 | 2026-01-29 |
| W_ItemWheelSlot.h | W_ItemWheelSlot.h | **UW_ItemWheel_NextSlot, UAC_EquipmentManager, UImage** | 2026-01-03 | 2026-01-29 |
| W_StatusEffectNotification.cpp | W_StatusEffectNotification.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventFinishNotificati... | 2026-01-03 | 2026-01-29 |
| W_AbilityDisplay.cpp | W_AbilityDisplay.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventOnAbilityInfoRec... | 2026-01-03 | 2026-01-26 |
| W_BigScreenMessage.cpp | W_BigScreenMessage.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventShowMessage_Impl... | 2026-01-03 | 2026-01-26 |
| W_Boss_Healthbar.cpp | W_Boss_Healthbar.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventHideBossBar_Impl... | 2026-01-03 | 2026-01-26 |
| W_Boss_Healthbar.h | W_Boss_Healthbar.h | **USLFStatBase, UW_Boss_Healthbar, UProgressBar** | 2026-01-03 | 2026-01-26 |
| W_BrowserFilterEntry.cpp | W_BrowserFilterEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventSetup_Implementa... | 2026-01-03 | 2026-01-26 |
| W_Browser_Action.cpp | W_Browser_Action.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetToolTipWidget_Impl... | 2026-01-03 | 2026-01-26 |
| W_Browser_Action_Tooltip.cpp | W_Browser_Action_Tooltip.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Browser_Animset.cpp | W_Browser_Animset.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetToolTipWidget_Impl... | 2026-01-03 | 2026-01-26 |
| W_Browser_AnimsetTooltip.cpp | W_Browser_AnimsetTooltip.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Browser_Animset_OwnerEntry.cpp | W_Browser_Animset_OwnerEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Browser_Categories.cpp | W_Browser_Categories.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventOnCategoryClicke... | 2026-01-03 | 2026-01-26 |
| W_Browser_CategoryEntry.cpp | W_Browser_CategoryEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Browser_Item.cpp | W_Browser_Item.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetToolTipWidget_Impl... | 2026-01-03 | 2026-01-26 |
| W_Browser_StatusEffect.cpp | W_Browser_StatusEffect.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetToolTipWidget_Impl... | 2026-01-03 | 2026-01-26 |
| W_Browser_StatusEffect_Tooltip.cpp | W_Browser_StatusEffect_Tooltip.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Browser_Tooltip.cpp | W_Browser_Tooltip.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Browser_WeaponAbility.cpp | W_Browser_WeaponAbility.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetToolTipWidget_Impl... | 2026-01-03 | 2026-01-26 |
| W_Browser_WeaponAbility_Tooltip.cpp | W_Browser_WeaponAbility_Tooltip.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Buff.cpp | W_Buff.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_CategoryEntry.cpp | W_CategoryEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_CharacterSelection_StatEntry.cpp | W_CharacterSelection_StatEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_CraftingEntry.cpp | W_CraftingEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetOwnedAmount_Implem... | 2026-01-03 | 2026-01-26 |
| W_CraftingEntrySimple.cpp | W_CraftingEntrySimple.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetOwnedAmount_Implem... | 2026-01-03 | 2026-01-26 |
| W_CreditEntry.cpp | W_CreditEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventInitializeSubEnt... | 2026-01-03 | 2026-01-26 |
| W_CreditEntry_Extra.cpp | W_CreditEntry_Extra.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_CreditEntry_Sub.cpp | W_CreditEntry_Sub.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventInitializeNames_... | 2026-01-03 | 2026-01-26 |
| W_CreditEntry_Sub_NameEntry.cpp | W_CreditEntry_Sub_NameEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Credits.cpp | W_Credits.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, SetupEntries_Implemen... | 2026-01-03 | 2026-01-26 |
| W_CurrencyContainer.cpp | W_CurrencyContainer.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventOnCurrencyIncrea... | 2026-01-03 | 2026-01-26 |
| W_DebugWindow.cpp | W_DebugWindow.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, SetMovePositionClickD... | 2026-01-03 | 2026-01-26 |
| W_Debug_ComponentSlot.cpp | W_Debug_ComponentSlot.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Debug_ComponentTooltip.cpp | W_Debug_ComponentTooltip.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Debug_HUD.cpp | W_Debug_HUD.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, FindWindowForComponen... | 2026-01-03 | 2026-01-26 |
| W_Equipment_Item_ItemEffect.cpp | W_Equipment_Item_ItemEffect.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventSetupItemEffect_... | 2026-01-03 | 2026-01-26 |
| W_Equipment_Item_OnUseEffect.cpp | W_Equipment_Item_OnUseEffect.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventSetupOnUseText_I... | 2026-01-03 | 2026-01-26 |
| W_Error.cpp | W_Error.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventSetMessage_Imple... | 2026-01-03 | 2026-01-26 |
| W_FirstLootNotification.cpp | W_FirstLootNotification.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventOnFinish_Impleme... | 2026-01-03 | 2026-01-26 |
| W_InteractionError.cpp | W_InteractionError.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventRemoveMessage_Im... | 2026-01-03 | 2026-01-26 |
| W_InventoryAction.cpp | W_InventoryAction.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, OnActionButtonPressed... | 2026-01-03 | 2026-01-26 |
| W_InventorySlot.cpp | W_InventorySlot.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, BindButtonEvents, OnS... | 2026-01-03 | 2026-01-26 |
| W_InventorySlot.h | W_InventorySlot.h | **UButton, UImage, UTextBlock** | 2026-01-03 | 2026-01-26 |
| W_Inventory_ActionButton.cpp | W_Inventory_ActionButton.cpp | NativePreConstruct, NativeConstruct, NativeDestruct, CacheWidgetReferences, S... | 2026-01-03 | 2026-01-26 |
| W_Inventory_ActionButton.h | W_Inventory_ActionButton.h | **UW_Inventory_ActionButton, UButton** | 2026-01-03 | 2026-01-26 |
| W_ItemWheel_NextSlot.cpp | W_ItemWheel_NextSlot.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, HandleChildItem_Imple... | 2026-01-03 | 2026-01-26 |
| W_ItemWheel_NextSlot.h | W_ItemWheel_NextSlot.h | **UW_ItemWheelSlot, UImage, UW_ItemWheel_NextSlot** | 2026-01-03 | 2026-01-26 |
| W_LevelCurrencyBlock_LevelUp.cpp | W_LevelCurrencyBlock_LevelUp.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventInitializeLevelU... | 2026-01-03 | 2026-01-26 |
| W_LevelUpCost.cpp | W_LevelUpCost.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventInitializeLevelU... | 2026-01-03 | 2026-01-26 |
| W_LoadingScreen.cpp | W_LoadingScreen.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventFadeOutAndNotify... | 2026-01-03 | 2026-01-26 |
| W_LoadingScreen.h | W_LoadingScreen.h | **UImage, UTextBlock, UW_LoadingScreen** | 2026-01-03 | 2026-01-26 |
| W_Navigable.cpp | W_Navigable.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, CanNavigate_Implement... | 2026-01-03 | 2026-01-26 |
| W_Navigable_InputReader.cpp | W_Navigable_InputReader.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetInputIconForKey_Im... | 2026-01-03 | 2026-01-26 |
| W_Radar.cpp | W_Radar.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, AddCardinal_Implement... | 2026-01-03 | 2026-01-26 |
| W_Radar.h | W_Radar.h | **UW_Radar_Cardinal, UW_Radar_TrackedElement, UOverlay** | 2026-01-03 | 2026-01-26 |
| W_Radar_Cardinal.cpp | W_Radar_Cardinal.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, UpdateCardinalDisplay | 2026-01-03 | 2026-01-26 |
| W_Radar_Cardinal.h | W_Radar_Cardinal.h | **UW_Radar_Cardinal, UTextBlock** | 2026-01-03 | 2026-01-26 |
| W_Radar_TrackedElement.cpp | W_Radar_TrackedElement.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Resources.cpp | W_Resources.cpp | NativePreConstruct, NativeConstruct, HideBarSeparators, NativeDestruct, Cache... | 2026-01-03 | 2026-01-26 |
| W_RestMenu.cpp | W_RestMenu.cpp | NativeConstruct, NativeDestruct, NativeOnKeyDown, Handled, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_RestMenu.h | W_RestMenu.h | **UW_RestMenu_Button, UW_RestMenu_TimeEntry, UW_TimePass** | 2026-01-03 | 2026-01-26 |
| W_RestMenu_Button.cpp | W_RestMenu_Button.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, SetRestMenuButtonSele... | 2026-01-03 | 2026-01-26 |
| W_RestMenu_Button.h | W_RestMenu_Button.h | **UW_RestMenu_Button, UBorder, UTextBlock** | 2026-01-03 | 2026-01-26 |
| W_RestMenu_TimeEntry.cpp | W_RestMenu_TimeEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, SetTimeEntrySelected_... | 2026-01-03 | 2026-01-26 |
| W_RestMenu_TimeEntry.h | W_RestMenu_TimeEntry.h | **UW_RestMenu_TimeEntry, UTextBlock, UBorder** | 2026-01-03 | 2026-01-26 |
| W_Settings_ControlEntry.cpp | W_Settings_ControlEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Settings_ControlsDisplay.cpp | W_Settings_ControlsDisplay.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Settings_InputKeySelector.cpp | W_Settings_InputKeySelector.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, TryGetKeyIcon_Impleme... | 2026-01-03 | 2026-01-26 |
| W_Settings_KeyMapping.cpp | W_Settings_KeyMapping.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, OnInputDeviceChanged_... | 2026-01-03 | 2026-01-26 |
| W_Settings_KeyMapping.h | W_Settings_KeyMapping.h | **UW_Settings_KeyMapping_Entry, UW_Settings_KeyMapping** | 2026-01-03 | 2026-01-26 |
| W_Settings_KeyMapping_Category.cpp | W_Settings_KeyMapping_Category.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Settings_KeyMapping_Entry.cpp | W_Settings_KeyMapping_Entry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, SelectMappingForSlot_... | 2026-01-03 | 2026-01-26 |
| W_StatBlock.cpp | W_StatBlock.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, OnStatsInitialized, S... | 2026-01-03 | 2026-01-26 |
| W_StatEntry_StatName.cpp | W_StatEntry_StatName.cpp | NativePreConstruct, NativeConstruct, NativeDestruct, CacheWidgetReferences, S... | 2026-01-03 | 2026-01-26 |
| W_StatEntry_StatName.h | W_StatEntry_StatName.h | **UW_StatEntry_StatName, UTextBlock** | 2026-01-03 | 2026-01-26 |
| W_StatusEffectNotification.h | W_StatusEffectNotification.h | **UTextBlock, UAC_StatusEffectManager, UStatusEffectManagerComponent** | 2026-01-03 | 2026-01-26 |
| W_Status_StatBlock.cpp | W_Status_StatBlock.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, SetupCurrentStats_Imp... | 2026-01-03 | 2026-01-26 |
| W_TargetLock.cpp | W_TargetLock.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Utility_Asset.cpp | W_Utility_Asset.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetToolTipWidget_Impl... | 2026-01-03 | 2026-01-26 |
| W_Utility_AssetTooltip.cpp | W_Utility_AssetTooltip.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-26 |
| W_Utility_Component.cpp | W_Utility_Component.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetToolTipWidget_Impl... | 2026-01-03 | 2026-01-26 |
| W_Crafting.h | W_Crafting.h | **UW_EquipmentSlot, UW_InventorySlot, UW_CraftingAction** | 2026-01-03 | 2026-01-22 |
| W_CraftingAction.cpp | W_CraftingAction.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, BindButtonEvents, Han... | 2026-01-03 | 2026-01-22 |
| W_CraftingAction.h | W_CraftingAction.h | **UW_InventorySlot, UW_CraftingAction, UW_GenericButton** | 2026-01-03 | 2026-01-22 |
| W_Inventory.cpp | W_Inventory.cpp | IsActionMenuBlockingNavigation, NativeConstruct, NativeDestruct, NativeOnKeyD... | 2026-01-03 | 2026-01-19 |
| W_Inventory.h | W_Inventory.h | **UW_InventorySlot, UW_Inventory_CategoryEntry, UW_InventoryAction** | 2026-01-03 | 2026-01-19 |
| W_InventoryAction.h | W_InventoryAction.h | **UW_InventorySlot, UW_Inventory_ActionButton, UW_InventoryActionAmount** | 2026-01-03 | 2026-01-19 |
| W_LevelUp.h | W_LevelUp.h | **UW_StatEntry_LevelUp, UW_StatBlock_LevelUp, UAC_InventoryManager** | 2026-01-03 | 2026-01-18 |
| W_Resources.h | W_Resources.h | **UW_Buff, UProgressBar, USizeBox** | 2026-01-03 | 2026-01-18 |
| W_StatBlock_LevelUp.h | W_StatBlock_LevelUp.h | **UW_StatEntry_LevelUp, UVerticalBox, UStatManagerComponent** | 2026-01-03 | 2026-01-18 |
| W_StatEntry_LevelUp.cpp | W_StatEntry_LevelUp.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, InitStatEntry_Impleme... | 2026-01-03 | 2026-01-18 |
| W_StatEntry_LevelUp.h | W_StatEntry_LevelUp.h | **USLFStatBase, UW_StatEntry_LevelUp** | 2026-01-03 | 2026-01-18 |
| W_Status.cpp | W_Status.cpp | NativeConstruct, NativeDestruct, NativeOnKeyDown, Handled, InitializeLevelCur... | 2026-01-03 | 2026-01-18 |
| W_TimePass.cpp | W_TimePass.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventAnimate_Implemen... | 2026-01-03 | 2026-01-17 |
| W_TimePass.h | W_TimePass.h | **UW_TimePass, UWidget** | 2026-01-03 | 2026-01-17 |
| W_Equipment.h | W_Equipment.h | **UW_EquipmentSlot, UW_InventorySlot, UW_GenericError** | 2026-01-03 | 2026-01-16 |
| W_Equipment_Item_AttackPower.cpp | W_Equipment_Item_AttackPower.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetStatDisplayName, F... | 2026-01-03 | 2026-01-16 |
| W_Equipment_Item_AttackPower.h | W_Equipment_Item_AttackPower.h | **UW_ItemInfoEntry, UAC_StatManager, UStatManagerComponent** | 2026-01-03 | 2026-01-16 |
| W_Equipment_Item_DamageNegation.cpp | W_Equipment_Item_DamageNegation.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetDisplayNameFromTag... | 2026-01-03 | 2026-01-16 |
| W_Equipment_Item_DamageNegation.h | W_Equipment_Item_DamageNegation.h | **UW_ItemInfoEntry, UW_Equipment_Item_DamageNegation** | 2026-01-03 | 2026-01-16 |
| W_Equipment_Item_RequiredStats.cpp | W_Equipment_Item_RequiredStats.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetStatDisplayName, F... | 2026-01-03 | 2026-01-16 |
| W_Equipment_Item_RequiredStats.h | W_Equipment_Item_RequiredStats.h | **UW_ItemInfoEntry_RequiredStats, UAC_StatManager, UStatManagerComponent** | 2026-01-03 | 2026-01-16 |
| W_Equipment_Item_Resistance.cpp | W_Equipment_Item_Resistance.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetDisplayNameFromTag... | 2026-01-03 | 2026-01-16 |
| W_Equipment_Item_Resistance.h | W_Equipment_Item_Resistance.h | **UW_ItemInfoEntry, UW_Equipment_Item_Resistance** | 2026-01-03 | 2026-01-16 |
| W_Equipment_Item_StatScaling.cpp | W_Equipment_Item_StatScaling.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetStatDisplayName, F... | 2026-01-03 | 2026-01-16 |
| W_Equipment_Item_StatScaling.h | W_Equipment_Item_StatScaling.h | **UW_ItemInfoEntry_StatScaling, UAC_StatManager, UStatManagerComponent** | 2026-01-03 | 2026-01-16 |
| W_Equipment_Item_StatsGranted.cpp | W_Equipment_Item_StatsGranted.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, GetStatDisplayName, F... | 2026-01-03 | 2026-01-16 |
| W_Equipment_Item_StatsGranted.h | W_Equipment_Item_StatsGranted.h | **UW_ItemInfoEntry, UW_Equipment_Item_StatsGranted** | 2026-01-03 | 2026-01-16 |
| W_ItemInfoEntry.cpp | W_ItemInfoEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-16 |
| W_ItemInfoEntry.h | W_ItemInfoEntry.h | **UW_ItemInfoEntry** | 2026-01-03 | 2026-01-16 |
| W_ItemInfoEntry_RequiredStats.cpp | W_ItemInfoEntry_RequiredStats.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-16 |
| W_ItemInfoEntry_RequiredStats.h | W_ItemInfoEntry_RequiredStats.h | **UW_ItemInfoEntry_RequiredStats** | 2026-01-03 | 2026-01-16 |
| W_ItemInfoEntry_StatScaling.cpp | W_ItemInfoEntry_StatScaling.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences | 2026-01-03 | 2026-01-16 |
| W_ItemInfoEntry_StatScaling.h | W_ItemInfoEntry_StatScaling.h | **UW_ItemInfoEntry_StatScaling** | 2026-01-03 | 2026-01-16 |
| W_StatEntry_Status.cpp | W_StatEntry_Status.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, InitStatEntry_Impleme... | 2026-01-03 | 2026-01-16 |
| W_StatEntry_Status.h | W_StatEntry_Status.h | **UB_Stat, USLFStatBase, UW_StatEntry_Status** | 2026-01-03 | 2026-01-16 |
| W_Status.h | W_Status.h | **UW_Status_LevelCurrencyBlock, UW_Status_StatBlock, UInventoryManagerCompone... | 2026-01-03 | 2026-01-16 |
| W_Status_LevelCurrencyBlock.cpp | W_Status_LevelCurrencyBlock.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventInitializeLevelA... | 2026-01-03 | 2026-01-16 |
| W_Status_LevelCurrencyBlock.h | W_Status_LevelCurrencyBlock.h | **UTextBlock, UW_Status_LevelCurrencyBlock, UTextBlock** | 2026-01-03 | 2026-01-16 |
| W_Status_StatBlock.h | W_Status_StatBlock.h | **UW_StatEntry_Status, UVerticalBox, USLFStatBase** | 2026-01-03 | 2026-01-16 |
| W_TargetExecutionIndicator.cpp | W_TargetExecutionIndicator.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, ToggleExecutionIcon_I... | 2026-01-03 | 2026-01-16 |
| W_TargetExecutionIndicator.h | W_TargetExecutionIndicator.h | **UW_TargetExecutionIndicator** | 2026-01-03 | 2026-01-16 |
| W_BigScreenMessage.h | W_BigScreenMessage.h | **UW_BigScreenMessage, UTextBlock, UTextBlock** | 2026-01-03 | 2026-01-12 |
| W_EnemyHealthbar.cpp | W_EnemyHealthbar.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, UpdateEnemyHealthbar_... | 2026-01-03 | 2026-01-12 |
| W_EnemyHealthbar.h | W_EnemyHealthbar.h | **UW_EnemyHealthbar, UProgressBar, UProgressBar** | 2026-01-03 | 2026-01-12 |
| W_CurrencyEntry.cpp | W_CurrencyEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, UpdateCurrencyDisplay... | 2026-01-03 | 2026-01-09 |
| W_CurrencyEntry.h | W_CurrencyEntry.h | **UTextBlock, UInventoryManagerComponent, UW_StatEntry_StatName** | 2026-01-03 | 2026-01-09 |
| W_EquipmentSlot.cpp | W_EquipmentSlot.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventClearEquipmentSl... | 2026-01-03 | 2026-01-09 |
| W_FirstLootNotification.h | W_FirstLootNotification.h | **UTexture2D, UWidgetAnimation, UW_FirstLootNotification** | 2026-01-03 | 2026-01-09 |
| W_Interaction.cpp | W_Interaction.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventHide_Implementat... | 2026-01-03 | 2026-01-09 |
| W_Interaction.h | W_Interaction.h | **UTextBlock, UW_Interaction** | 2026-01-03 | 2026-01-09 |
| W_Inventory_CategoryEntry.cpp | W_Inventory_CategoryEntry.cpp | NativePreConstruct, NativeConstruct, NativeDestruct, CacheWidgetReferences, S... | 2026-01-03 | 2026-01-09 |
| W_Inventory_CategoryEntry.h | W_Inventory_CategoryEntry.h | **UImage, UButton, UW_Inventory_CategoryEntry** | 2026-01-03 | 2026-01-09 |
| W_LevelEntry.cpp | W_LevelEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, UpdateLevelDisplay, E... | 2026-01-03 | 2026-01-09 |
| W_LevelEntry.h | W_LevelEntry.h | **UTextBlock, UStatManagerComponent, UW_StatEntry_StatName** | 2026-01-03 | 2026-01-09 |
| W_LootNotification.cpp | W_LootNotification.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventFinished_Impleme... | 2026-01-03 | 2026-01-09 |
| W_LootNotification.h | W_LootNotification.h | **UWidgetAnimation, UW_LootNotification** | 2026-01-03 | 2026-01-09 |
| W_StatBlock.h | W_StatBlock.h | **UW_StatEntry, UVerticalBox, UStatManagerComponent** | 2026-01-03 | 2026-01-09 |
| W_StatEntry.cpp | W_StatEntry.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, InitStatEntry_Impleme... | 2026-01-03 | 2026-01-09 |
| W_StatEntry.h | W_StatEntry.h | **USLFStatBase, UW_StatEntry** | 2026-01-03 | 2026-01-09 |
| W_GenericError.cpp | W_GenericError.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventErrorOkPressed_I... | 2026-01-03 | 2026-01-08 |
| W_Dialog.cpp | W_Dialog.cpp | NativeConstruct, NativeDestruct, CacheWidgetReferences, EventCloseDialog_Impl... | 2026-01-03 | 2026-01-07 |
| W_GameMenu_Button.cpp | W_GameMenu_Button.cpp | NativePreConstruct, NativeConstruct, NativeDestruct, ApplyVisualConfig, SetGa... | 2026-01-03 | 2026-01-07 |
| W_GameMenu_Button.h | W_GameMenu_Button.h | **UButton, UBorder, UImage** | 2026-01-03 | 2026-01-07 |
| W_Navigable.h | W_Navigable.h | **UW_Navigable** | 2026-01-03 | 2026-01-07 |
| W_Navigable_InputReader.h | W_Navigable_InputReader.h | **UW_Navigable_InputReader** | 2026-01-03 | 2026-01-07 |
| W_Settings_KeyMapping_Entry.h | W_Settings_KeyMapping_Entry.h | **UW_Settings_KeyMapping_Entry** | 2026-01-03 | 2026-01-07 |
| W_SkipCinematic.cpp | W_SkipCinematic.cpp | NativePreConstruct, NativeConstruct, NativeDestruct, CacheWidgetReferences, E... | 2026-01-03 | 2026-01-07 |
| W_AbilityDisplay.h | W_AbilityDisplay.h | **UW_AbilityDisplay** | 2026-01-03 | 2026-01-03 |
| W_BrowserFilterEntry.h | W_BrowserFilterEntry.h | **UW_BrowserFilterEntry** | 2026-01-03 | 2026-01-03 |
| W_Browser_Action.h | W_Browser_Action.h | **UW_Browser_Action_Tooltip, UB_Action, UW_Browser_Action** | 2026-01-03 | 2026-01-03 |
| W_Browser_Action_Tooltip.h | W_Browser_Action_Tooltip.h | **UB_Action, UW_Browser_Action_Tooltip** | 2026-01-03 | 2026-01-03 |
| W_Browser_Animset.h | W_Browser_Animset.h | **UW_Browser_AnimsetTooltip, UW_Browser_Animset** | 2026-01-03 | 2026-01-03 |
| W_Browser_AnimsetTooltip.h | W_Browser_AnimsetTooltip.h | **UW_Browser_AnimsetTooltip** | 2026-01-03 | 2026-01-03 |
| W_Browser_Animset_OwnerEntry.h | W_Browser_Animset_OwnerEntry.h | **UW_Browser_Animset_OwnerEntry** | 2026-01-03 | 2026-01-03 |
| W_Browser_Categories.h | W_Browser_Categories.h | **UW_Browser_Categories** | 2026-01-03 | 2026-01-03 |
| W_Browser_CategoryEntry.h | W_Browser_CategoryEntry.h | **UW_Browser_CategoryEntry** | 2026-01-03 | 2026-01-03 |
| W_Browser_Item.h | W_Browser_Item.h | **UW_Browser_Tooltip, UW_Browser_Item** | 2026-01-03 | 2026-01-03 |
| W_Browser_StatusEffect.h | W_Browser_StatusEffect.h | **UW_Browser_StatusEffect_Tooltip, UB_StatusEffect, UW_Browser_StatusEffect** | 2026-01-03 | 2026-01-03 |
| W_Browser_StatusEffect_Tooltip.h | W_Browser_StatusEffect_Tooltip.h | **UB_StatusEffect, UW_Browser_StatusEffect_Tooltip** | 2026-01-03 | 2026-01-03 |
| W_Browser_Tooltip.h | W_Browser_Tooltip.h | **UW_Browser_Tooltip** | 2026-01-03 | 2026-01-03 |
| W_Browser_WeaponAbility.h | W_Browser_WeaponAbility.h | **UW_Browser_WeaponAbility_Tooltip, UW_Browser_WeaponAbility** | 2026-01-03 | 2026-01-03 |
| W_Browser_WeaponAbility_Tooltip.h | W_Browser_WeaponAbility_Tooltip.h | **UW_Browser_WeaponAbility_Tooltip** | 2026-01-03 | 2026-01-03 |
| W_Buff.h | W_Buff.h | **UW_Buff** | 2026-01-03 | 2026-01-03 |
| W_CategoryEntry.h | W_CategoryEntry.h | **UW_CategoryEntry** | 2026-01-03 | 2026-01-03 |
| W_CharacterSelection_StatEntry.h | W_CharacterSelection_StatEntry.h | **UW_CharacterSelection_StatEntry** | 2026-01-03 | 2026-01-03 |
| W_CraftingEntry.h | W_CraftingEntry.h | **UW_CraftingEntry** | 2026-01-03 | 2026-01-03 |
| W_CraftingEntrySimple.h | W_CraftingEntrySimple.h | **UW_CraftingEntrySimple** | 2026-01-03 | 2026-01-03 |
| W_CreditEntry.h | W_CreditEntry.h | **UW_CreditEntry** | 2026-01-03 | 2026-01-03 |
| W_CreditEntry_Extra.h | W_CreditEntry_Extra.h | **UW_CreditEntry_Extra** | 2026-01-03 | 2026-01-03 |
| W_CreditEntry_Sub.h | W_CreditEntry_Sub.h | **UW_CreditEntry_Sub** | 2026-01-03 | 2026-01-03 |
| W_CreditEntry_Sub_NameEntry.h | W_CreditEntry_Sub_NameEntry.h | **UW_CreditEntry_Sub_NameEntry** | 2026-01-03 | 2026-01-03 |
| W_Credits.h | W_Credits.h | **UW_Credits** | 2026-01-03 | 2026-01-03 |
| W_CurrencyContainer.h | W_CurrencyContainer.h | **UW_CurrencyContainer** | 2026-01-03 | 2026-01-03 |
| W_DebugWindow.h | W_DebugWindow.h | **UAC_DebugCentral, UW_DebugWindow** | 2026-01-03 | 2026-01-03 |
| W_Debug_ComponentSlot.h | W_Debug_ComponentSlot.h | **UW_Debug_ComponentSlot** | 2026-01-03 | 2026-01-03 |
| W_Debug_ComponentTooltip.h | W_Debug_ComponentTooltip.h | **UW_Debug_ComponentTooltip** | 2026-01-03 | 2026-01-03 |
| W_Debug_HUD.h | W_Debug_HUD.h | **UW_DebugWindow, UW_Debug_HUD** | 2026-01-03 | 2026-01-03 |
| W_Dialog.h | W_Dialog.h | **UW_Dialog** | 2026-01-03 | 2026-01-03 |
| W_EquipmentSlot.h | W_EquipmentSlot.h | **UW_EquipmentSlot** | 2026-01-03 | 2026-01-03 |
| W_Equipment_Item_ItemEffect.h | W_Equipment_Item_ItemEffect.h | **UW_Equipment_Item_ItemEffect** | 2026-01-03 | 2026-01-03 |
| W_Equipment_Item_OnUseEffect.h | W_Equipment_Item_OnUseEffect.h | **UW_Equipment_Item_OnUseEffect** | 2026-01-03 | 2026-01-03 |
| W_Error.h | W_Error.h | **UW_Error** | 2026-01-03 | 2026-01-03 |
| W_GenericError.h | W_GenericError.h | **UW_GenericError** | 2026-01-03 | 2026-01-03 |
| W_InteractionError.h | W_InteractionError.h | **UW_InteractionError** | 2026-01-03 | 2026-01-03 |
| W_LevelCurrencyBlock_LevelUp.h | W_LevelCurrencyBlock_LevelUp.h | **UW_LevelCurrencyBlock_LevelUp** | 2026-01-03 | 2026-01-03 |
| W_LevelUpCost.h | W_LevelUpCost.h | **UW_LevelUpCost** | 2026-01-03 | 2026-01-03 |
| W_MainMenu_Button.h | W_MainMenu_Button.h | **USizeBox, UBorder, UButton** | 2026-01-03 | 2026-01-03 |
| W_Radar_TrackedElement.h | W_Radar_TrackedElement.h | **UW_Radar_TrackedElement** | 2026-01-03 | 2026-01-03 |
| W_Settings_ControlEntry.h | W_Settings_ControlEntry.h | **UW_Settings_ControlEntry** | 2026-01-03 | 2026-01-03 |
| W_Settings_ControlsDisplay.h | W_Settings_ControlsDisplay.h | **UW_Settings_ControlsDisplay** | 2026-01-03 | 2026-01-03 |
| W_Settings_InputKeySelector.h | W_Settings_InputKeySelector.h | **UW_Settings_InputKeySelector** | 2026-01-03 | 2026-01-03 |
| W_Settings_KeyMapping_Category.h | W_Settings_KeyMapping_Category.h | **UW_Settings_KeyMapping_Category** | 2026-01-03 | 2026-01-03 |
| W_SkipCinematic.h | W_SkipCinematic.h | **UTextBlock, USizeBox, UImage** | 2026-01-03 | 2026-01-03 |
| W_TargetLock.h | W_TargetLock.h | **UW_TargetLock** | 2026-01-03 | 2026-01-03 |
| W_Utility_Asset.h | W_Utility_Asset.h | **UW_Utility_AssetTooltip, UW_Utility_Asset** | 2026-01-03 | 2026-01-03 |
| W_Utility_AssetTooltip.h | W_Utility_AssetTooltip.h | **UB_Action, UW_Utility_AssetTooltip** | 2026-01-03 | 2026-01-03 |
| W_Utility_Component.h | W_Utility_Component.h | **UW_Utility_AssetTooltip, UW_Utility_Component** | 2026-01-03 | 2026-01-03 |

## Elden Ring Tools (root) (`C:/scripts/elden_ring_tools/`)
**528 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| _gen_diag.py | (no description) |  | unknown | unknown |
| analyze_c3100_components.py | Analyze c3100 FLVER mesh for modular component swapping. | load_flver, get_bone_weights_from_array, classify_region, main | unknown | unknown |
| analyze_deformation.py | analyze_deformation.py - Analyze Sentinel mesh deformation during animation. | setup_render, position_camera_at_bone, analyze_vertex_groups, check_bone_infl... | unknown | unknown |
| apply_c2120_tae_notifies.py | Apply TAE (TimeAct Events) as UE5 AnimNotifies to imported c2120 AnimSequences. | get_notify_info, apply_notifies, dry_run | unknown | unknown |
| apply_tae_notifies.py | Apply TAE (TimeAct Events) as UE5 AnimNotifies to imported AnimSequences. | get_notify_info, apply_notifies, dry_run | unknown | unknown |
| arp_bind_warlord.py | arp_bind_warlord.py - Phase 2: Bind fresh Warlord mesh to ARP rig. |  | unknown | unknown |
| arp_retarget_comparison.py | ARP Retarget Comparison — Generate .blend files for every tuning variant. | enable_arp, get_action_fcurves, strip_non_root_translations, do_arp_retarget,... | unknown | unknown |
| arp_rig_warlord.py | arp_rig_warlord.py - Use Auto-Rig Pro to auto-rig the Ironbound Warlord mesh. | ensure_object_mode, find_largest_mesh, dump_objects | unknown | unknown |
| arp_rotfix_comparison.py | ARP Retarget — Rotation Fix Comparison. | enable_arp, get_action_fcurves, strip_non_root_translations, negate_rotation_... | unknown | unknown |
| assign_c3100_abilities.py | Assign c3100 (Crucible Knight) abilities to Enemy Guard via C++ automation. | log | unknown | unknown |
| assign_guard_mesh.py | Assign c3100 guard mesh + AnimBP to Enemy Guard Blueprint. | log, out, main | unknown | unknown |
| assign_mesh_to_boss.py | Assign imported c4810 mesh to boss Blueprint - STEP 2 of 2. | log, main | unknown | unknown |
| assign_mvp_to_boss.py | Assign MVP mesh to boss + retarget AnimBP. | log, main | unknown | unknown |
| assign_v11.py | Check v11 import results and assign to boss. | main | unknown | unknown |
| assign_v13.py | Check v13 import and assign to boss with AnimBP. | main | unknown | unknown |
| assign_v14b_own_to_boss.py | Assign v14b own-skeleton mesh to boss + Compatible Skeletons + AnimBP retarget. | log, main | unknown | unknown |
| assign_v15_to_boss.py | Verify v15 mesh import and assign to boss BP. | log, main | unknown | unknown |
| assign_v16_own_to_boss.py | Assign v16-own-skeleton mesh to boss + set up Compatible Skeletons. | log, main | unknown | unknown |
| assign_v16_to_boss.py | Verify v16 mesh import and assign to boss BP. | log, main | unknown | unknown |
| assign_v9_noanim.py | Assign v9 mesh to boss with no AnimBP. | main | unknown | unknown |
| bake_sentinel_animations.py | bake_sentinel_animations.py - Batch retarget c3100 animations onto Sentinel arma | enable_arp, delete_objects_except, clean_orphans, import_anim_fbx, get_action... | unknown | unknown |
| batch_import_anims.py | Batch animation importer - runs UE5 once per animation file. | check_asset_exists, run_ue5_import, import_animations, apply_notifies, main | unknown | unknown |
| batch_import_c3100_mannequin.py | Batch import retargeted c3100 animations to UE5 (one per invocation). | check_asset_exists, run_import, main | unknown | unknown |
| batch_import_mannequin.py | Batch import c4810 Mannequin-retargeted animations (1 per UE5 invocation). | import_single, main | unknown | unknown |
| batch_import_mq_anims.py | Batch import c4810 Mannequin-named animations into UE5. | check_already_imported, run_import, main | unknown | unknown |
| batch_import_onto_mannequin.py | Batch import c4810 Mannequin-named animations directly onto SK_Mannequin. | check_already_imported, run_import, main | unknown | unknown |
| batch_import_zup_anims.py | Batch import Z-up animations into UE5, one per invocation. | check_asset_exists, import_one, main | unknown | unknown |
| batch_reskin_c3100.py | Batch reskin c3100 animations onto Mannequin mesh. | run_blender, process_animation, main | unknown | unknown |
| batch_retarget_c4810.py | Batch retarget c4810 animations using existing IK Retargeter assets. | log | unknown | unknown |
| blender_retarget_c4810.py | Blender headless script to retarget Elden Ring c4810 animations to UE5 SK_Manneq | clear_scene, import_fbx, get_animation_frame_range, add_copy_rotation_constra... | unknown | unknown |
| build_custom_flver_transfer.py | Build custom mesh .blend files with FLVER weight transfer. | _to_blender_vector, _to_blender_matrix3, clean_scene, strip_root_motion, buil... | unknown | unknown |
| build_custom_flver_transfer_v2.py | Build custom mesh .blend files with FLVER weight transfer v2. | clean_scene, strip_root_motion, build_hkx_armature, import_manny_mesh, comput... | unknown | unknown |
| build_custom_flver_transfer_v3.py | Build custom mesh .blend files with FLVER weight transfer v3. | clean_scene, strip_root_motion, build_hkx_armature, compute_arm_angle_below_h... | unknown | unknown |
| build_custom_flver_transfer_v4.py | Build custom mesh .blend files with FLVER weight transfer v4. | clean_scene, strip_root_motion, build_hkx_armature, import_and_scale_manny, r... | unknown | unknown |
| build_custom_mesh_blend.py | Build .blend files: HKX armature + CUSTOM MESH (v8 reskin) + HKX animations. | clean_scene, strip_root_motion, build_hkx_armature_blender_space, load_v8_mes... | unknown | unknown |
| build_ikretarget_blends.py | Build .blend files from UE5 IK Retarget JSON dump. | ue5_quat_to_blender, ue5_pos_to_blender | unknown | unknown |
| build_mesh_on_anim_armature.py | MVP Test: Build FLVER mesh on the EXACT animation armature. | log, _get_vertex_positions, _get_vertex_uvs, _get_bone_weights, _destrip | unknown | unknown |
| build_mesh_v14b.py | build_mesh_v14b.py - Build mesh in Z-up space (UE5 convention) | log, swap_yz_vec, swap_yz_matrix3, load_hkx_skeleton, create_armature_zup | unknown | unknown |
| build_mesh_v15.py | build_mesh_v15.py - Build mesh compatible with animation skeleton | log, load_flver, _get_positions, _get_uvs, _get_bone_data | unknown | unknown |
| build_mesh_v16.py | build_mesh_v16.py - Build mesh with IDENTICAL armature code to extract_animation | log, load_hkx_skeleton, create_armature_from_skeleton_hkx, load_flver, _get_p... | unknown | unknown |
| build_proper_blend.py | Build proper .blend files: HKX armature + FLVER mesh + HKX animations. | _to_blender_vector, _to_blender_matrix3, game_bone_to_bl_matrix, clean_scene,... | unknown | unknown |
| build_raw_c3100_validation.py | Build validation .blend files from raw c3100 source FBXes (pre-retarget). | build_blend | unknown | unknown |
| build_raw_c3100_with_mesh.py | Build validation .blend files from raw c3100 FBXes WITH the FLVER mesh. |  | unknown | unknown |
| build_raw_c3100_with_mesh_v2.py | Build raw c3100 validation blends - v2: import mesh+anim from SAME FBX. |  | unknown | unknown |
| build_raw_c3100_with_mesh_v3.py | Build raw c3100 validation blends - v3: import mesh first, then apply anim actio |  | unknown | unknown |
| build_raw_c3100_with_mesh_v4.py | Build raw c3100 validation blends - v4: fix Blender 5.0 layered action slot bind | assign_action_to_armature | unknown | unknown |
| build_review_blends.py | Build .blend files for review with the original c3100 knight mesh | clean_scene, get_fcurves, setup_camera_and_lights, import_mesh, import_and_tr... | unknown | unknown |
| build_sentinel_review_blend.py | Build a single .blend file with the Sentinel rigged mesh + all animations for re |  | unknown | unknown |
| build_unified_mesh_v14.py | build_unified_mesh_v14.py - Build mesh directly on animation skeleton | log, load_hkx_skeleton, create_armature_from_hkx, load_flver, _get_vertex_pos... | unknown | unknown |
| build_vertex_snag_validation.py | Build per-animation validation .blend files from Phase 3 final FBXes. | clear_scene, build_one, main | unknown | unknown |
| build_walk_comparison_blend.py | Build visual .blend files for Sentinel walk/attack animations. | get_action_fcurves, build_sentinel_anim, build_c3100_walk, main | unknown | unknown |
| check_abp_c2120_details.py | Check ABP_c2120_Boss parent class, variables, and AnimGraph state. | check_abp, check_blendspace, main | unknown | unknown |
| check_abp_c2120_simple.py | Simple check of ABP_c2120_Boss parent class and variables. | check_abp, main | unknown | unknown |
| check_abp_parent.py | Minimal check - just parent class chain of ABP_c2120_Boss. | safe_check | unknown | unknown |
| check_abp_parent2.py | Check ABP parent via Blueprint property. | check | unknown | unknown |
| check_and_assign_v14.py | Check if v14 import succeeded (despite Slate crash) and assign to boss. | log, main | unknown | unknown |
| check_and_assign_v6.py | Check v6 mesh and assign to boss. Also try importing WITHOUT target skeleton as  | find_mesh, check_mesh, assign_to_boss, main | unknown | unknown |
| check_and_configure_v2.py | Check if v2 import succeeded and configure everything. | main | unknown | unknown |
| check_anibnd.py | Quick check of c2120 ANIBND contents. |  | unknown | unknown |
| check_anim_skeletons.py | Check what skeleton each imported c4810m AnimSequence uses, and what the boss me | log | unknown | unknown |
| check_animgraph_nodes.py | Check ABP_SoulslikeEnemy AnimGraph nodes, pins, and connections. | log, inspect_graph | unknown | unknown |
| check_armature_transform.py | Check if mesh and armature are in same world space after FBX import. |  | unknown | unknown |
| check_assign_v14b.py | Check v14b import, set up Compatible Skeletons, assign to boss. | log, main | unknown | unknown |
| check_bone_data.py | Bone Data Checker - Diagnose which bones have meaningful keyframes in each anima | clean_scene, get_fcurves_from_action, analyze_animation, compare_animations, ... | unknown | unknown |
| check_bone_maps.py | Check which source skeletons have verified .bmap files. |  | unknown | unknown |
| check_boss_abp.py | Check what skeleton ABP_SoulslikeEnemy uses and what the boss mesh skeleton is. | log | unknown | unknown |
| check_boss_skeleton.py | Check the current boss character's skeleton, mesh, and AnimBP assignment. | log | unknown | unknown |
| check_chrbnd.py | Quick check of c2120 CHRBND contents. |  | unknown | unknown |
| check_coordinate_spaces.py | Compare HKX bone positions vs FLVER bone positions to find the coordinate mismat |  | unknown | unknown |
| check_existing_malenia_meshes.py | Check ALL existing c2120/Malenia meshes in the project. | log | unknown | unknown |
| check_import_result.py | Check if mesh import succeeded (assets saved before Slate crash). |  | unknown | unknown |
| check_mannequin_bones.py | Quick diagnostic: check Mannequin FBX bone names and directions. |  | unknown | unknown |
| check_mannequin_imports.py | Check what got imported into c4810_mannequin directory. | log | unknown | unknown |
| check_mannequin_ref.py | Check Mannequin reference animation bone positions to understand expected values | log, check_anim_bones | unknown | unknown |
| check_mannequin_refpose.py | Check Mannequin skeleton reference pose local rotations. | log | unknown | unknown |
| check_mesh_bounds.py | Quick check: where is the FLVER mesh in game space? |  | unknown | unknown |
| check_mesh_vs_skeleton_scale.py | Check mesh bounding box vs skeleton bone scale to diagnose scale mismatch. | main | unknown | unknown |
| check_montage_data.py | Check montage internal structure - does it actually contain animation data? | log | unknown | unknown |
| check_mq_ref_local.py | Check Mannequin reference local rotations by reading ref skeleton via C++ automa | log | unknown | unknown |
| check_multisource_anims.py | Quick check: which animations exist for each source enemy. |  | unknown | unknown |
| check_preview_meshes.py | Check what preview meshes exist and what animations are using. | log | unknown | unknown |
| check_retarget_motion.py | Check retargeted animations for actual bone motion across frames. | log | unknown | unknown |
| check_rt_anim_data.py | Check if retargeted animations have actual bone data (not empty/zero). | log | unknown | unknown |
| check_sentinel_anim_data.py | check_sentinel_anim_data.py - Verify Sentinel FBX animations have actual bone mo |  | unknown | unknown |
| check_temp_imports.py | Check what was imported in Temp/c4810m_* directories. | log | unknown | unknown |
| check_v5_mesh.py | Check the v5 mesh import results and assign to boss if scale is correct. | main | unknown | unknown |
| check_v7_assign.py | Check v7 (own skeleton) import results and assign to boss. | main | unknown | unknown |
| check_v8_assign.py | Check v8 import and assign to boss. Run after import_c2120_v8.py. | main | unknown | unknown |
| check_v9_assign.py | Check v9 import and assign to boss. Run after import_c2120_v9.py. | main | unknown | unknown |
| check_zup_import.py | Check Z-up import results and complete assignment. | log, main | unknown | unknown |
| cleanup_and_reimport.py | Delete StaticMesh assets from failed import, prepare for reimport. | log | unknown | unknown |
| cleanup_c2120_meshes.py | Delete extra SkeletalMesh assets created during c2120 animation import. |  | unknown | unknown |
| cleanup_c3100_old_montages.py | Clean up old c3100 montages still on SK_Mannequin_Skeleton (UE4). | log | unknown | unknown |
| cleanup_c3100_physics.py | Delete the auto-generated physics asset from c3100 mesh import. | log | unknown | unknown |
| cleanup_extra_meshes.py | Delete extra SkeletalMesh assets created during animation import. |  | unknown | unknown |
| clear_boss_abp.py | Clear AnimBP from boss so it shows reference pose. | main | unknown | unknown |
| compare_armatures.py | Compare v14b mesh armature vs animation armature bone-by-bone. | log, import_fbx_armature, get_bone_data, matrix_diff, main | unknown | unknown |
| compare_bone_matrices.py | Compare the FULL bone.matrix_local between HKX armature and FLVER transforms. |  | unknown | unknown |
| compare_c3010_vs_c4350.py | Carefully compare c3010 vs c4350 skeleton bone names, | enable_arp, load_chr_data | unknown | unknown |
| compare_fbx_bones.py | Compare bones between v16 mesh FBX and an animation FBX. | log, get_armature_bones, main | unknown | unknown |
| compare_flver_hkx_bones.py | Compare FLVER bone positions vs HKX bone positions. | log, get_flver_bone_world_transforms, get_hkx_bone_world_transforms, main | unknown | unknown |
| compare_meshes.py | (no description) | clear_scene, import_fbx, mesh_stats, assign_solid_material, setup_render | unknown | unknown |
| compare_native_vs_retarget.py | Create a .blend with BOTH the native c4090 animation (on its own armature) | enable_arp, get_action_fcurves | unknown | unknown |
| compare_retarget_c3180.py | Compare retarget approaches: c3180 native vs c3180->c3100 retarget. | load_chr, retarget_rotation_only | unknown | unknown |
| compare_sentinel_vgs.py | compare_sentinel_vgs.py - Compare vertex groups between original .blend and FBX  |  | unknown | unknown |
| compress_retargeted_anims.py | Compress retargeted c4810 animations to reduce file size. | compress_animations, cleanup_temp_folders | unknown | unknown |
| configure_boss_malenia_anims.py | Configure boss with Malenia (c4810) animations on Mannequin. | log, setup_compatible_skeletons, configure_boss_abilities, main | unknown | unknown |
| configure_boss_malenia_v2.py | Configure boss with Malenia (c4810) abilities on the correct combat manager. | log, main | unknown | unknown |
| configure_boss_malenia_v3.py | Configure boss with Malenia abilities via SCS component template. | log, find_scs_combat_manager, find_combat_manager_alternative, main | unknown | unknown |
| configure_c2120_boss.py | Configure boss for c2120 (Malenia) - creates abilities, poise break, boss config | ensure_directory_exists, load_montage, load_anim_sequence, create_ability_dat... | unknown | unknown |
| configure_c3100_guard.py | Configure c3100 guard: BlendSpace samples, AnimGraph, mesh+AnimBP assignment. | log, out, cleanup_duplicates, configure_blendspace, setup_animgraph | unknown | unknown |
| configure_c4810_boss.py | Configure boss for c4810 (Malenia) - Step 8 of the pipeline. | ensure_directory_exists, load_montage, load_anim_sequence, create_ability_dat... | unknown | unknown |
| control_test_montage.py | Control test: Assign a KNOWN WORKING SLF montage to one boss ability. | log | unknown | unknown |
| create_comparison_blends.py | Create side-by-side .blend files for visual validation of c3100 knight animation | clean_scene, import_fbx, make_color_material, extract_hold_pose, transfer_ani... | unknown | unknown |
| create_mannequin_mesh.py | Create a simple but recognizable humanoid mesh in Blender for reskin testing. | log, add_part | unknown | unknown |
| create_mannequin_montages.py | Create montages from imported Mannequin animations and update ability data asset | log, main | unknown | unknown |
| create_mesh_for_anim_skeleton.py | Create a FLVER mesh FBX that matches the animation skeleton's Y-up convention. | log, find_nearest_bone, main | unknown | unknown |
| create_mq_montages.py | Create AnimMontages from c4810 mannequin animations and update boss abilities. | log | unknown | unknown |
| create_rt_montages.py | Create AnimMontages from retargeted c4810 animations for the Mannequin skeleton. | log | unknown | unknown |
| debug_action_api.py | Debug: what do bone fcurve data_paths look like in Blender 5.0 FBX import? |  | unknown | unknown |
| debug_action_paths.py | Debug: check what data paths the action uses vs what the armature expects. |  | unknown | unknown |
| debug_alignment.py | Diagnose why the right arm distorts: check bone positions vs mesh vertex positio | pt_seg_dist | unknown | unknown |
| debug_arm_bend.py | Diagnostic script: Arm bend direction analysis for reskinned Mannequin on c3100  | log, sep, find_armature, find_mesh, format_vec | unknown | unknown |
| debug_bone_chain.py | Debug script: dump bone parent chain for c3100 skeleton to diagnose R_Forearm is | is_ancestor_of | unknown | unknown |
| debug_bone_names.py | (no description) | get_fcurve_bones | unknown | unknown |
| debug_fbx_export.py | Debug: Check if FBX export introduces spurious Master bone translation. |  | unknown | unknown |
| debug_rfoot.py | Quick check: R_Foot bone position vs mesh vertex positions. |  | unknown | unknown |
| debug_rt_one.py | Re-run retarget on ONE animation to see debug bone transform output. | log | unknown | unknown |
| debug_strip.py | Debug: check FCurve data paths in source and retargeted actions. | get_fcurves | unknown | unknown |
| debug_weapon_parent.py | Debug weapon parenting in rigged.blend and a retargeted .blend. | inspect_file | unknown | unknown |
| debug_weapon_parent2.py | Check which bones in the arm chain have/miss keyframes, and test actual weapon w |  | unknown | unknown |
| debug_weapon_rotation.py | Check weapon rotation across full animation range and compare to native source. |  | unknown | unknown |
| deep_diag_apose.py | Deep diagnosis: Check exact object references in montage, skeleton identity, and | log | unknown | unknown |
| definitive_comparison.py | Definitive side-by-side: HKX skeleton (correct) vs FLVER skeleton (wrong rest po | clean_scene, setup_scene, get_fcurves, import_mesh_and_anim, import_from_blen... | unknown | unknown |
| delete_and_retarget_c4810.py | Delete existing retargeted animations and re-retarget with coordinate alignment  | log | unknown | unknown |
| diag_gen.py | generator |  | unknown | unknown |
| diag_gen2.py | (no description) |  | unknown | unknown |
| diag_sentinel_blend.py | Diagnose why animations don't play on the Sentinel rig. |  | unknown | unknown |
| diagnose_anim_bones.py | Diagnose why c3100 knight animations show A-pose in Blender. | clean_scene, get_fcurves, extract_bone_names_from_fcurves, test_anim_on_own_a... | unknown | unknown |
| diagnose_animgraph_cpp.py | Call C++ DiagnoseAnimGraphNodes to inspect ABP_SoulslikeEnemy AnimGraph. | log | unknown | unknown |
| diagnose_animgraph_nodes.py | Diagnose AnimGraph nodes for boss vs working enemy AnimBPs. | dump_abp, check_character_mesh, check_animation_skeleton_compat, main | unknown | unknown |
| diagnose_anims.py | Diagnose Attack02 flip and Walk one-leg issues. | analyze_action | unknown | unknown |
| diagnose_anims_v2.py | Diagnose Attack02 flip and Walk one-leg - v2 with auto bone name detection. | get_all_fcurves, analyze_action | unknown | unknown |
| diagnose_apose.py | Diagnose A-pose: Check if retargeted AnimSequences have compressed data. | log | unknown | unknown |
| diagnose_arm_detection.py | Quick diagnostic: check mesh vertex distribution for arm detection. |  | unknown | unknown |
| diagnose_bind_pose.py | Diagnose bind pose mismatch between animation skeleton and mesh skeleton. | clear_scene, import_fbx, find_armature, find_meshes, dump_bone_transforms | unknown | unknown |
| diagnose_blend_meshes.py | diagnose_blend_meshes.py - Inspect all blend files to find the actual weighted m |  | unknown | unknown |
| diagnose_boss_anim.py | Diagnose boss AnimBP skeleton mismatch. | main | unknown | unknown |
| diagnose_boss_apose.py | (no description) | log, skel_info, check_anim_bp, inspect_skel_mesh_comp, main | unknown | unknown |
| diagnose_boss_state.py | Diagnose why Malenia boss looks like a blob and isn't using animations. | log, main | unknown | unknown |
| diagnose_c2120_abp.py | Diagnose why ABP_c2120_Boss AnimInstance is NULL at runtime. | main | unknown | unknown |
| diagnose_c2120_anims.py | Diagnose c2120 animation data - check tracks, duration, bone names. | main | unknown | unknown |
| diagnose_c2120_skeleton.py | Diagnose c2120 skeleton compatibility and mesh assignment. | get_skeleton_info, main | unknown | unknown |
| diagnose_c3100_anim.py | Diagnose why animations don't play on the FLVER armature. | log, get_fcurves_from_action | unknown | unknown |
| diagnose_c3100_anims.py | Diagnose c3100 AnimSequence bone data to find skeleton mismatch. | log | unknown | unknown |
| diagnose_c3100_mesh.py | Diagnostic: Export c3100 mesh both WITH and WITHOUT armature. | log, load_flver, main | unknown | unknown |
| diagnose_c3100_tracks.py | Diagnose c3100 AnimSequence bone track data using C++ DiagnoseAnimSequence. | log | unknown | unknown |
| diagnose_fbx_bones.py | diagnose_fbx_bones.py - Inspect FBX bone names to find UE5 sanitization collisio | ue5_sanitize | unknown | unknown |
| diagnose_flver_arm.py | Diagnose FLVER mesh arm positions vs bone positions. |  | unknown | unknown |
| diagnose_left_weights.py | Diagnose left-side vertex weights on Sentinel mesh. |  | unknown | unknown |
| diagnose_retarget.py | Diagnose why retargeted FBXs have static animation. | check_animation | unknown | unknown |
| diagnose_retarget_data.py | Diagnostic: Check retargeted animation data for actual bone movement. | log | unknown | unknown |
| diagnose_retarget_positions.py | Diagnose retarget position issue. | log, check_anim_bones | unknown | unknown |
| diagnose_rt_output.py | Diagnose retargeted animation output - check if tracks have data. | log | unknown | unknown |
| diagnose_sentinel_bones.py | diagnose_sentinel_bones.py - Find which bones are disconnected after ARP retarge |  | unknown | unknown |
| diagnose_sentinel_mesh.py | diagnose_sentinel_mesh.py - Check mesh vertex groups vs armature bones. |  | unknown | unknown |
| diagnose_skeleton_approach.py | Diagnose the correct skeleton approach for c3100. | setup_camera_and_lights, setup_render, render_frame, analyze_vertex_groups, t... | unknown | unknown |
| diagnose_skeleton_bones.py | Diagnose skeleton bone mismatches between mesh and animation skeletons. | get_skeleton_bone_names, get_skeleton_bone_count, main | unknown | unknown |
| diagnose_v16_bones.py | Diagnose v16 bone mismatch with Z-up skeleton. | log, get_skeleton_bone_names, get_mesh_bone_names, main | unknown | unknown |
| diagnose_v9_skeleton.py | Diagnose v9 skeleton reference pose in UE5. | log, dump_ref_skeleton, check_mesh_bounds, compare_skeletons, main | unknown | unknown |
| diagnose_vertex_snag.py | Diagnose vertex snagging: find vertex groups with no matching bone in the armatu | main | unknown | unknown |
| diagnose_vertex_snag_v2.py | Diagnose vertex snagging v2: find unweighted/low-weight vertices | main | unknown | unknown |
| diagnose_weapon_verts.py | Diagnose weapon geometry on the original Warlord mesh. | main | unknown | unknown |
| diagnose_weapon_verts2.py | Diagnose bone positions and vertex distribution in the rigged blend. | main | unknown | unknown |
| diagnose_weapon_weights.py | diagnose_weapon_weights.py - Analyze vertex weights in weapon/handle region of r |  | unknown | unknown |
| diagnose_weights.py | Blender 5.0 headless script: Diagnose vertex weights on the reskinned Mannequin  | clear_scene, import_fbx, analyze_weights, check_armature_modifier, main | unknown | unknown |
| direct_vs_retarget_comparison.py | Create DIRECT (name-matched) and ARP retarget versions of each animation, | enable_arp, get_action_fcurves, strip_non_root_translations, setup_camera, ge... | unknown | unknown |
| dump_bone_names.py | Dump all bone names from source and target skeletons for bone mapping. | log | unknown | unknown |
| dump_c3100_bones.py | Dump c3100 bone names and hierarchy from an FBX file. |  | unknown | unknown |
| dump_mannequin_bones.py | Dump SK_Mannequin AND c4810_mq bone rest-pose transforms to JSON. | log | unknown | unknown |
| dump_mannequin_bones_v2.py | Dump bone names from SK_Mannequin via SkeletalMesh API. | log | unknown | unknown |
| dump_mannequin_bones_v3.py | Dump Mannequin bones by exporting mesh as FBX then reading bone names. | log | unknown | unknown |
| dump_manny_bones.py | Quick diagnostic: dump Mannequin bone names. |  | unknown | unknown |
| dump_sentinel_bones.py | Dump all 91 bone names from the Sentinel armature for rename mapping. |  | unknown | unknown |
| dump_skeleton_bones.py | Dump bone names from c4810 skeleton FBX using Blender headless. |  | unknown | unknown |
| enumerate_bones.py | Enumerate bones from c4810 skeleton and Mannequin skeleton. | enumerate_via_ikrig | unknown | unknown |
| export_c2120_malenia_mesh.py | Export c2120 (Malenia) FLVER mesh with ONLY the animation skeleton bones. | log, log_warn, log_error, _to_blender_vector, _to_blender_matrix3 | unknown | unknown |
| export_c3100_anim_bones.py | Export c3100 mesh using ANIMATION skeleton bone positions instead of FLVER bone  | log, load_flver, build_hierarchy, find_ancestor, main | unknown | unknown |
| export_c3100_batch.py | Export c3100 mesh + batch of animations as FBXs for UE5. | log, main | unknown | unknown |
| export_c3100_final.py | Export the working c3100 (FBX armature + FLVER mesh) as FBX for UE5 import. | log, main | unknown | unknown |
| export_c3100_intermediate_skeleton.py | Export the c3100 intermediate skeleton with Mannequin bone names but ER rest pos | should_remove_bone | unknown | unknown |
| export_c3100_mesh.py | Export c3100 FLVER mesh as FBX using build_proper_blend.py's proven method. | clean_scene, build_hkx_armature, add_flver_mesh, export_fbx, main | unknown | unknown |
| export_c3100_original.py | Export original c3100 FLVER mesh with animation as source-of-truth FBX. | log | unknown | unknown |
| export_c3100_refpose.py | Export c3100 ER reference pose as JSON for C++ retarget fix. | should_remove_bone | unknown | unknown |
| export_c3100_soulstruct.py | Use soulstruct-blender addon's FLVER import logic (not the operator) to properly | log, load_flver, main | unknown | unknown |
| export_c3100_static.py | Export c3100 as STATIC mesh only (no armature, no weights). | log, load_flver, main | unknown | unknown |
| export_c3100_test.py | Export the working c3100 Blender file as FBX (mesh + armature + animation). |  | unknown | unknown |
| export_c3100_test_mesh.py | Export c3100 (Crucible Knight) mesh - SIMPLE test of the pipeline. | log, load_flver, get_anim_bone_names, build_hierarchy, find_ancestor | unknown | unknown |
| export_c4810_anim_bones_mesh.py | Export c4810 (Malenia) FLVER mesh with ONLY the 176 animation skeleton bones. | log, log_warn, log_error, _to_blender_vector, _to_blender_matrix3 | unknown | unknown |
| export_mannequin_fbx.py | Export SKM_Manny_Simple as FBX for Blender rest-pose reference. | log | unknown | unknown |
| export_mannequin_skeleton.py | UE5 Python script to export SK_Mannequin skeleton as FBX reference. | export_skeleton_as_fbx, main | unknown | unknown |
| export_manny_cpp.py | Export SKM_Manny using C++ automation (avoids Python FBX exporter crash). | log | unknown | unknown |
| export_manny_fbx.py | Export SKM_Manny from UE5 as FBX for reskin testing. | log | unknown | unknown |
| export_manny_fbx_v2.py | Export SKM_Manny from UE5 as FBX - v2 using different approach. | log | unknown | unknown |
| export_manny_fbx_v3.py | Export SKM_Manny - try multiple approaches to avoid commandlet crash. | log | unknown | unknown |
| export_manny_fbx_v4.py | Export SKM_Manny from UE5 as FBX. | log | unknown | unknown |
| export_manny_fbx_v5.py | Export SKM_Manny using Interchange pipeline (avoids FBX exporter crash). | log | unknown | unknown |
| export_manny_obj.py | Export SKM_Manny as OBJ (simpler exporter, avoids FBX Slate crash). | log | unknown | unknown |
| export_multisource_raw_fbx.py | Export raw FBX per source enemy for UE5 IK Retargeter pipeline. | load_chr, create_dummy_mesh, export_fbx | unknown | unknown |
| export_skeleton_fbx.py | Export a reference skeleton FBX from extracted animation data. | create_placeholder_mesh, export_skeleton_fbx, main | unknown | unknown |
| extract_animations.py | Elden Ring Animation Extraction - Headless Blender Script | **HeadlessLogger**; _to_blender_vector, _to_blender_matrix3, _game_bone_trans... | unknown | unknown |
| extract_animations_zup.py | Extract animations with Z-up skeleton (matching v14b mesh). | _to_blender_vector_zup, _to_blender_matrix3_zup, _game_bone_transform_to_bl_b... | unknown | unknown |
| extract_character_mesh.py | Elden Ring Character Mesh Extraction - Headless Blender Script | _to_blender_vector, _to_blender_matrix3, _game_bone_to_bl_bone_matrix, create... | unknown | unknown |
| extract_manny_data.py | Extract SKM_Manny mesh data (verts, faces, weights, skeleton) as JSON. | log | unknown | unknown |
| extract_tae_hitboxes.py | Extract TAE AttackBehavior hitbox windows and apply forensic time transforms. | compute_speed_factor, forensic_time_transform, scan_fbx_directory, get_anim_d... | unknown | unknown |
| finalize_c2120_setup.py | Finalize c2120 boss setup - Compatible Skeletons, mesh assignment, abilities. | setup_compatible_skeletons, assign_mesh_to_boss, configure_combat_abilities, ... | unknown | unknown |
| finalize_retarget.py | Finalize Retarget: Save v8 mannequin mesh with validated c3100 animations. | clean_scene, import_fbx, import_action, main | unknown | unknown |
| find_compatible_skeletons.py | Find all Elden Ring enemies with skeletons compatible with c3100 (Crucible Knigh |  | unknown | unknown |
| find_hold_frame.py | Find the best frame in the attack animation for the 'hold pose' - | clean_scene, main | unknown | unknown |
| fix_abp_c2120_animgraph.py | Fix ABP_c2120_Boss AnimGraph and BlendSpace after recreation. | main | unknown | unknown |
| fix_abp_c2120_parent.py | Check and fix ABP_c2120_Boss parent class to UABP_SoulslikeBossNew. | main | unknown | unknown |
| fix_abp_c2120_parent_v2.py | Properly reparent ABP_c2120_Boss to UABP_SoulslikeBossNew with compile. | main | unknown | unknown |
| fix_abp_skeleton.py | Fix AnimBP target skeleton to match mesh skeleton. | log, main | unknown | unknown |
| fix_abp_skeleton_and_animgraph.py | Fix ABP_c2120_Boss: Recreate with correct mesh skeleton + AnimGraph. | main | unknown | unknown |
| fix_anim_skeleton_axis.py | Fix animation skeleton and ONE test animation to Z-up (matching FLVER mesh). | log, rotate_armature_y_to_z, process_skeleton, process_animation, main | unknown | unknown |
| fix_blendspace_skeleton.py | Fix BlendSpace skeleton mismatch - recreate with mesh skeleton. | retarget_anim_to_skeleton, recreate_blendspace, retarget_all_animations, rebu... | unknown | unknown |
| fix_c2120_abp_parent.py | Fix c2120 boss animations by reparenting ABP to C++ AnimInstance. | main | unknown | unknown |
| fix_c2120_combat_config.py | Fix c2120 boss combat configuration. | get_all_c2120_abilities, fix_combat_manager_abilities, remove_status_effect_a... | unknown | unknown |
| fix_c2120_compatible_skeletons.py | Set up Compatible Skeletons between c2120 animation and mesh skeletons. | add_compatible_skeleton, main | unknown | unknown |
| fix_c2120_mesh_root_bone.py | Fix root bone name mismatch - rename MESH root bone to match skeleton. | main | unknown | unknown |
| fix_c2120_mesh_scale.py | Fix c2120 mesh scale mismatch - re-import with convert_scene=True. | main | unknown | unknown |
| fix_c2120_mesh_v6.py | Fix c2120 mesh - import with EXACT same settings as skeleton import. | main | unknown | unknown |
| fix_c2120_montages.py | Fix empty c2120 montages by deleting and recreating with source_animation. | anim_id_to_asset_name, main | unknown | unknown |
| fix_c2120_own_skeleton.py | Import c2120 mesh with its OWN skeleton (no target), then set up retargeting. | main | unknown | unknown |
| fix_c2120_root_bone.py | Fix root bone name mismatch between c2120 skeleton and mesh. | main | unknown | unknown |
| fix_c3100_refpose.py | Fix c3100 animation reference poses. | log, out | unknown | unknown |
| fix_c3100_skeleton.py | Fix c3100 skeleton mismatch: retargeted anims use SK_Mannequin_Skeleton but | log | unknown | unknown |
| fix_c3100_skeleton_v2.py | Fix c3100 skeleton mismatch using C++ RetargetAnimationSkeleton. | log | unknown | unknown |
| fix_crossbody_weights.py | Fix cross-body vertex weights on Sentinel mesh. |  | unknown | unknown |
| fix_guard_sockets_and_mesh.py | Fix c3100 guard: add weapon sockets to skeleton, diagnose mesh visibility. | log, out, diagnose_skeleton, add_weapon_sockets, add_sockets_to_mesh | unknown | unknown |
| fix_ik_rigs_and_retarget.py | Fix existing IK Rig assets (add chains) and create retargeter + test retarget. | log | unknown | unknown |
| fix_original_mesh.py | Fix zero-weight vertices in original FLVER mesh and re-export. | log, find_nearest_bone, fix_zero_weights, main | unknown | unknown |
| fix_remove_twists.py | First restore clean weights from WW |  | unknown | unknown |
| fix_restore_weights.py | Clear everything |  | unknown | unknown |
| fix_root_motion.py | Disable root motion on c4810 Mannequin AnimSequences by setting ForceRootLock. | log | unknown | unknown |
| fix_sentinel_mesh.py | fix_sentinel_mesh.py - Fix Sentinel mesh skinning issues. |  | unknown | unknown |
| fix_sentinel_review.py | Fix Sentinel review blend: assign action slots properly for Blender 5.0. |  | unknown | unknown |
| fix_sentinel_weights.py | (no description) | analyze_weights | unknown | unknown |
| fix_sentinel_weights_final.py | fix_sentinel_weights_final.py - Fix missing vertex weights on Sentinel mesh. |  | unknown | unknown |
| fix_sentinel_weights_v2.py | fix_sentinel_weights_v2.py - Re-rig Sentinel mesh to c3100 FLVER armature. | count_weighted_verts | unknown | unknown |
| fix_sentinel_weights_v3.py | fix_sentinel_weights_v3.py - Rename bones from Manny to FLVER names + re-weight. | count_weighted_verts | unknown | unknown |
| fix_sentinel_weights_v4.py | fix_sentinel_weights_v4.py - Re-rig Ironbound Warlord mesh to FLVER armature. | count_weighted_verts | unknown | unknown |
| fix_skeleton_and_animgraph.py | Fix collapsed skeleton and AnimGraph for c2120 boss. | main, approach2 | unknown | unknown |
| fix_skeleton_mismatch_v2.py | Fix AnimGraph skeleton mismatch v2 - Use ReplaceSkeleton API. | try_retarget_anim, main | unknown | unknown |
| fix_vertex_snag.py | Fix vertex snagging: assign unweighted vertices to nearest bones. | clear_scene, import_fbx, find_nearest_structural_bones, fix_mesh_weights, bui... | unknown | unknown |
| fix_weapon_lock.py | (no description) |  | unknown | unknown |
| force_refpose_v11.py | Force reference pose on boss mesh and clear AnimClass completely. | main | unknown | unknown |
| forensic_audit_sentinel.py | Forensic Audit: Compare pre-forensic (source) vs final (forensic) Sentinel anima | clean_scene, import_fbx, get_bone_names, get_action_info, sample_bone_rotations | unknown | unknown |
| forensic_final_pipeline.py | forensic_final_pipeline.py - Apply ALL forensic changes and export clean FBXes. | get_fcurves, get_bone_noise_weight, smooth_noise, warp_time, lerp | unknown | unknown |
| forensic_isolate_attack02.py | Isolate bone transforms for Attack02 using the REAL Soulstruct pipeline. | strip_root_motion, strip_weapon_bone_fcurves, import_attack02 | unknown | unknown |
| forensic_mesh_export.py | Re-export the Sentinel mesh with forensic bone transforms applied. | perturb_rest_pose | unknown | unknown |
| forensic_noise_preview.py | forensic_noise_preview.py - Side-by-side comparison: original vs noise-injected  | get_bone_weight, smooth_noise, get_fcurves, apply_noise_to_action, duplicate_... | unknown | unknown |
| forensic_offset_preview.py | forensic_offset_preview.py - Side-by-side: original vs static rotation offset. | get_fcurves, apply_static_offset, duplicate_hierarchy | unknown | unknown |
| forensic_rename_preview.py | forensic_rename_preview.py - Side-by-side: original bones vs renamed bones. | get_fcurves, rename_bones_in_armature, rename_vertex_groups, rename_fcurve_pa... | unknown | unknown |
| forensic_resample_preview.py | forensic_resample_preview.py - Side-by-side: original (30fps) vs resampled (24fp | get_fcurves, slerp_quat, resample_action, duplicate_hierarchy | unknown | unknown |
| forensic_step0_baseline.py | Step 0: BASELINE -- No forensic transforms, WITH MESH. | get_action_frame_range | unknown | unknown |
| forensic_step0_decompose.py | Step 0 decomposed into sub-steps for Sentinel_Attack01. | get_action_frame_range | unknown | unknown |
| forensic_step0_decompose_a02.py | Step 0 decomposed into sub-steps for Sentinel_Attack02. | get_action_frame_range | unknown | unknown |
| forensic_step0_isolate.py | Isolate forensic bone transforms for Attack02. | get_action_frame_range, get_all_fcurves, load_rigged_with_anim, rename_bones,... | unknown | unknown |
| forensic_timewarp_preview.py | forensic_timewarp_preview.py - Side-by-side: original vs time-warped animation. | get_fcurves, warp_time, lerp, slerp_quat, apply_timewarp_to_action | unknown | unknown |
| generate_bone_maps.py | Generate .bmap files for all humanoid source skeletons → c3100 target. | generate_bmap, extract_missing_hierarchies | unknown | unknown |
| humanoid_full_comparison.py | Full humanoid comparison: load skeleton from base anibnd, find attack anims | enable_arp, get_action_fcurves, strip_non_root_translations, ensure_white_bac... | unknown | unknown |
| humanoid_skeleton_survey.py | Survey ALL unpacked humanoid enemy skeletons. | read_skeleton_from_binder, load_skeleton | unknown | unknown |
| humanoid_survey_blender.py | Survey ALL unpacked humanoid enemy skeletons via Blender + Soulstruct. | enable_arp, get_action_fcurves, strip_non_root_translations, ensure_white_bac... | unknown | unknown |
| import_assign_v14.py | Import c2120_v14 mesh into UE5 and assign to boss Blueprint. | log, import_fbx, assign_to_boss, main | unknown | unknown |
| import_assign_v14b.py | Import v14b mesh (Z-up) with own skeleton, set up Compatible Skeletons, assign t | log, import_fbx, setup_compatible_skeletons, assign_to_boss, main | unknown | unknown |
| import_c2120_matched_mesh.py | Import the reskinned c2120 mesh FBX that matches the animation skeleton. | main | unknown | unknown |
| import_c2120_matched_mesh_v2.py | Import reskinned c2120 mesh to a CLEAN temp directory (no target skeleton). | clean_dest_directory, import_mesh, find_imported_assets, setup_compatible_ske... | unknown | unknown |
| import_c2120_own_skeleton.py | Import c2120 mesh with its OWN skeleton to preserve reference pose. | main | unknown | unknown |
| import_c2120_skeleton.py | Import c2120 animation skeleton to UE5. | log, main | unknown | unknown |
| import_c2120_v10.py | Import zero-weight-fixed FLVER mesh with ANIMATION skeleton as target (v10). | main | unknown | unknown |
| import_c2120_v11.py | Import FLVER mesh with CoB-stripped bones (v11). | main | unknown | unknown |
| import_c2120_v12.py | Import noCob mesh WITH animation skeleton as target + AnimBP (v12). | main | unknown | unknown |
| import_c2120_v13.py | Import unified mesh (rigged to animation skeleton) + AnimBP (v13). | main | unknown | unknown |
| import_c2120_v8.py | Import c2120 v3 mesh (axis-fixed) with OWN skeleton into UE5. | main | unknown | unknown |
| import_c2120_v9.py | Import original FLVER mesh (zero-weight fixed) into UE5 with OWN skeleton. | main | unknown | unknown |
| import_c3100_anims.py | Import a few c3100 animations to test with the mesh. | log | unknown | unknown |
| import_c3100_batch.py | Import batch of c3100 animations to UE5 using existing skeleton. | log | unknown | unknown |
| import_c3100_comparison.py | Import both original FLVER and reskinned mesh FBXs into UE5 for comparison. | log, out | unknown | unknown |
| import_c3100_direct.py | Import c3100 direct FBX as a completely new asset in UE5. | log, out | unknown | unknown |
| import_c3100_final.py | Import c3100 final mesh + animations to UE5. | log | unknown | unknown |
| import_c3100_intermediate.py | Import c3100 intermediate skeleton to UE5 and set up for retargeting. | log, out | unknown | unknown |
| import_c3100_mannequin.py | Import retargeted c3100 animations (Mannequin bone names) into UE5. | log | unknown | unknown |
| import_c3100_mesh.py | Import c3100 (Crucible Knight) mesh to UE5 - simple pipeline test. | log | unknown | unknown |
| import_c3100_mesh_v2.py | Import c3100 mesh v2 - to fresh directory to avoid file locks. | log | unknown | unknown |
| import_c3100_new_skeleton.py | Import one c3100 renamed-bone FBX as a NEW skeleton (don't merge into Mannequin) | log, out | unknown | unknown |
| import_c3100_test.py | Import the test retargeted FBX to verify it works in UE5. | log, out | unknown | unknown |
| import_c3100_v3.py | Import c3100 mesh v3 - using animation bone positions. | log | unknown | unknown |
| import_c3100_v4.py | Import c3100 soulstruct mesh to UE5. | log | unknown | unknown |
| import_c4810_anim_mesh.py | Import c4810 (Malenia) mesh with 176 animation bones into UE5. | log | unknown | unknown |
| import_c4810_mannequin.py | Import c4810 Mannequin-named animations into UE5. | log, main | unknown | unknown |
| import_c4810_mesh.py | Import the ACTUAL c4810 (Malenia) mesh FBX into UE5. | log | unknown | unknown |
| import_character_mesh.py | Import FLVER character mesh FBX into UE5 and assign to boss Blueprint. | log, import_mesh_fbx, assign_mesh_to_boss, main | unknown | unknown |
| import_er_character.py | Import Elden Ring Character into UE5 (Option B - Direct Skeleton Import) | import_skeleton, import_single_animation, import_animations, get_notify_info,... | unknown | unknown |
| import_guard_anims_to_ue5.py | Import all c3100 guard reskinned animations to UE5. | log, out | unknown | unknown |
| import_guard_anims_to_ue5_v2.py | Import all c3100 guard reskinned animations to UE5. | log, out | unknown | unknown |
| import_hitreact_to_ue5.py | Import reskinned hit react animation FBX to UE5. | log, out | unknown | unknown |
| import_malenia_mesh.py | Import Malenia (c2120) mesh FBX into UE5 at /Game/Temp/malenia/ and set up previ | log | unknown | unknown |
| import_mvp_to_boss.py | Import MVP mesh and assign to boss with AnimBP. | log, main | unknown | unknown |
| import_reskinned.py | Import a reskinned mesh to UE5 using the existing c3100 skeleton. | log | unknown | unknown |
| import_single_anim_mannequin.py | Import a single FBX animation onto SK_Mannequin skeleton. | log | unknown | unknown |
| import_single_mq_anim.py | Import a single c4810 Mannequin-named animation onto the existing skeleton. | log, main | unknown | unknown |
| import_single_onto_mannequin.py | Import a single c4810 Mannequin-named animation directly onto SK_Mannequin. | log, main | unknown | unknown |
| import_single_zup_anim.py | Import a single Z-up animation FBX targeting the Z-up skeleton. | log, main | unknown | unknown |
| import_to_ue5.py | UE5 Animation Import & AnimNotify Automation | import_fbx_animations, apply_tae_notifies, get_notify_info, add_notify_state,... | unknown | unknown |
| import_v12_to_ue5.py | Import v12 reskinned animations to UE5. | log, out | unknown | unknown |
| import_via_soulstruct.py | Import Elden Ring character mesh + animations using the io_soulstruct addon. | **DummyOperator**; clean_scene, try_addon_import, import_flver_direct, try_bl... | unknown | unknown |
| import_zup_skeleton.py | Import the first Z-up animation FBX to create the Z-up skeleton in UE5. | log, import_first_anim, assign_skeleton_to_mesh, assign_to_boss, main | unknown | unknown |
| inspect_ai_mesh.py | Inspect Meshy AI Ironbound Warlord FBX - check mesh, armature, scale, bone count |  | unknown | unknown |
| inspect_blend.py | Inspect the existing c3100 .blend files to understand the correct setup. | dump_armature, dump_meshes, inspect_blend, inspect_fbx, compare_rest_poses | unknown | unknown |
| inspect_flver_meshes.py | Inspect FLVER meshes inside the c2120 CHRBND to identify humanoid body vs decora | main | unknown | unknown |
| inspect_flver_submeshes.py | Inspect FLVER Submesh Structure - c2120 | load_flver, get_face_count, get_vertex_count, get_referenced_bone_names, chec... | unknown | unknown |
| inspect_guard_anims.py | Blender 5.0 headless script: Visually inspect c3100 guard idle/walk animations. | clear_scene, import_fbx, get_fcurves_from_action, import_anim_only, apply_act... | unknown | unknown |
| inspect_ue5_meshes.py | Inspect what skeletal meshes UE5 actually has and their bone counts. | log | unknown | unknown |
| investigate_retarget_diff.py | Investigate why c3010 retargets well but c4350 doesn't. | load_skeleton, quat_angle_between, print_bone_info | unknown | unknown |
| investigate_weapon_disconnect.py | Investigate weapon disconnect: compare c3010 (good) vs c4350 (bad). | get_action_fcurves, analyze_blend | unknown | unknown |
| investigate_weapon_position.py | Investigate weapon position: compare c3010 (natural, no R_Sword keys) vs | get_action_fcurves | unknown | unknown |
| list_addons.py | List all available Blender addons. |  | unknown | unknown |
| list_blend_objects.py | List all objects in rest_pose_comparison.blend |  | unknown | unknown |
| list_c3100_animations.py | List all c3100 animations by category to find diverse attack/dodge/death options |  | unknown | unknown |
| list_c4810_assets.py | List all assets in c4810 directory to find correct animation paths. | log | unknown | unknown |
| list_fbx_bones.py | Clear scene |  | unknown | unknown |
| list_project_meshes.py | List SkeletalMesh and StaticMesh assets in the project for reskin testing. | log | unknown | unknown |
| match_sentinel_rest_pose.py | match_sentinel_rest_pose.py | clear_scene, import_fbx, bone_table, match_and_apply, verify_match | unknown | unknown |
| match_sentinel_v2.py | match_sentinel_v2.py - Match Sentinel mesh pose to FLVER, with screenshots. | clear_scene, import_fbx, bone_table, setup_render, render_view | unknown | unknown |
| match_sentinel_v3.py | match_sentinel_v3.py - Match Sentinel to FLVER MESH arm angle (not bones). | clear_scene, import_fbx, vertex_centroid, bone_head_world, bone_direction | unknown | unknown |
| match_sentinel_v4.py | match_sentinel_v4.py - Match Sentinel to FLVER A-pose (~36 deg below horizontal) | clear_scene, import_fbx, rotate_bone_direction, setup_render, render_view | unknown | unknown |
| match_sentinel_v5.py | match_sentinel_v5.py - Fix BOTH skeletons to match their meshes. | clear_scene, import_fbx, fix_arms_iterative, setup_render, render_view | unknown | unknown |
| match_sentinel_v6.py | match_sentinel_v6.py - Parent Sentinel mesh to FLVER armature directly. | clear_scene, import_fbx, fix_arms_iterative, setup_render, render_view | unknown | unknown |
| merge_c4810_skeleton.py | Merge c4810 mesh bones into the animation skeleton, then reassign the mesh. | log | unknown | unknown |
| mesh_animation_pipeline.py | mesh_animation_pipeline.py - Reusable Blender-only pipeline for custom enemy mes | get_action_fcurves, get_fcurves_collection, clean_orphans, import_fbx, get_ar... | unknown | unknown |
| multisource_sentinel_pipeline.py | Multi-source forensic pipeline for Sentinel enemy. | strip_root_motion, strip_weapon_bone_fcurves, enable_arp, get_action_fcurves,... | unknown | unknown |
| native_source_comparison.py | For each source enemy, build THEIR native skeleton from HKX, | enable_arp, get_action_fcurves, strip_non_root_translations, get_mesh_bounds,... | unknown | unknown |
| place_c2120_boss.py | Place c2120 boss in the demo level. | main | unknown | unknown |
| populate_montages.py | Populate empty AnimMontages with their source AnimSequence data. | anim_id_to_asset_name, delete_and_recreate_montages, try_set_montage_animatio... | unknown | unknown |
| populate_montages_v2.py | Populate AnimMontages v2 - uses export_text approach to set animation reference. | anim_id_to_asset_name, check_montage_populated, main | unknown | unknown |
| prepare_reskin_blend.py | Prepare a working Blender file for the reskin pipeline. | log, load_flver_from_chrbnd, build_flver_mesh, main | unknown | unknown |
| preview_final_sentinel.py | preview_final_sentinel.py - Generate preview .blend files from forensically clea | get_fcurves | unknown | unknown |
| preview_sentinel_anims.py | preview_sentinel_anims.py - Import Sentinel animation FBXes into .blend files fo |  | unknown | unknown |
| preview_sentinel_fixed.py | preview_sentinel_fixed.py - Re-import Sentinel animations with ignore_leaf_bones |  | unknown | unknown |
| probe_action_api.py | Probe Blender 5.0 Action API to find where FCurves live. |  | unknown | unknown |
| probe_anibnd_animations.py | Probe multiple ANIBNDs to list skeleton bone counts and animation IDs. |  | unknown | unknown |
| probe_anim_enum.py | Probe AnimationMode enum values in UE5.7 Python. | log | unknown | unknown |
| qremesh_gui.py | qremesh_gui.py - Phase 1: Quad Remesher on original AI mesh (GUI mode). | get_view3d_override, write_settings, check_progress, on_qr_complete, quit_ble... | unknown | unknown |
| quick_check_rt.py | Quick check retarget positions vs Mannequin reference. | log | unknown | unknown |
| quick_render_test.py | Quick test: render the HKX armature + FLVER mesh from multiple angles, no animat |  | unknown | unknown |
| rebake_sentinel_constraints.py | rebake_sentinel_constraints.py - Constraint-based animation bake (fixes glitchy  | clean_orphans, import_anim_fbx, constraint_bake_animation, export_sentinel_anim | unknown | unknown |
| rebake_sentinel_direct.py | rebake_sentinel_direct.py - Direct bone-by-bone animation copy (NO ARP). | clean_orphans, import_anim_fbx, direct_copy_animation, export_sentinel_anim | unknown | unknown |
| rebuild_c2120_animgraph.py | Rebuild c2120 boss AnimGraph with CORRECT animation paths. | main | unknown | unknown |
| recreate_abp_c2120.py | Delete and recreate ABP_c2120_Boss with correct parent class. | main | unknown | unknown |
| reexport_anim_only.py | reexport_anim_only.py - Re-export Phase 3 forensic FBXes as animation-only (no m | clean_scene, process_animation, main | unknown | unknown |
| reexport_sentinel_nomesh.py | Re-export ONE Sentinel animation FBX WITHOUT mesh data. |  | unknown | unknown |
| reimport_c2120_mesh_matched.py | Re-import c2120 mesh FBX targeting the ANIMATION skeleton. | main | unknown | unknown |
| reimport_character_mesh.py | Clean reimport of FLVER character mesh FBX into UE5 - STEP 1 of 2. | log, main | unknown | unknown |
| reimport_malenia_mesh.py | Reimport Malenia mesh using the existing c2120 animation skeleton directly. | log | unknown | unknown |
| reimport_malenia_own_skeleton.py | Reimport Malenia mesh with its OWN skeleton, then set up Compatible Skeletons fo | log | unknown | unknown |
| reimport_malenia_v3.py | Reimport Malenia mesh v3 - coordinates now match animation skeleton. | log | unknown | unknown |
| reimport_matched_mesh.py | Re-import the c2120 matched mesh after fixing FBX axis settings. | main | unknown | unknown |
| reimport_v14b_on_zup_skeleton.py | Reimport v14b mesh targeting the Z-up animation skeleton. | log, main | unknown | unknown |
| reimport_v14b_own_skeleton.py | Import v14b mesh with its OWN skeleton (no targeting). | log, main | unknown | unknown |
| reimport_v15_on_zup_skeleton.py | Reimport v15 mesh targeting the Z-up animation skeleton. | log, main | unknown | unknown |
| reimport_v16_on_zup_skeleton.py | Reimport v16 mesh targeting the Z-up animation skeleton. | log, main | unknown | unknown |
| reimport_v16_own_skeleton.py | Import v16 mesh with its OWN skeleton (no targeting). | log, main | unknown | unknown |
| remove_c2120_boss_weapons.py | Remove old demo boss weapon SCS components from B_Soulslike_Boss_Malgareth. | main | unknown | unknown |
| remove_status_effect_area.py | Remove B_StatusEffectArea actors from the demo level. | main | unknown | unknown |
| remove_status_effect_area_v2.py | Remove B_StatusEffectArea actors from the demo level using sublevel actor iterat | main | unknown | unknown |
| render_attack01_verify.py | Render Attack01 FBX from the NEW pipeline to verify no distortion. |  | unknown | unknown |
| render_blend_gallery.py | Render mid-frame screenshots from all saved blend files for visual validation. | setup_camera_and_render | unknown | unknown |
| render_bone_overlay.py | Render FLVER armature overlaid on custom mesh - front view, zoomed out, showing  | render_view | unknown | unknown |
| render_bone_overlay_v2.py | Render FLVER armature overlaid on custom mesh with VISIBLE bones. | setup_scene, make_camera, render_view | unknown | unknown |
| render_bone_overlay_v3.py | Render FLVER armature overlaid on Warlord mesh with HIGH CONTRAST bones. | render_view | unknown | unknown |
| render_c3100_frames.py | Render sample frames of c3100 animation for visual diagnosis. |  | unknown | unknown |
| render_diagnostic.py | Open saved comparison .blend files and render diagnostic frames. | dump_scene_state, render_frame, process_blend, main | unknown | unknown |
| render_existing_blend.py | Render frames from the existing c3100 .blend files to see what the | add_camera_and_lights, render_blend, main | unknown | unknown |
| render_knight_solo.py | Render the FLVER (Crucible Knight) mesh solo for each animation. | position_camera_for_mesh | unknown | unknown |
| render_mannequin_solo.py | Render the reskinned Mannequin mesh solo for each animation. | position_camera_for_mesh | unknown | unknown |
| render_mesh_comparison.py | render_mesh_comparison.py - Render base Meshy mesh and weighted Sentinel from mu | setup_scene, position_camera, get_mesh_bounds, render_angles, apply_texture_t... | unknown | unknown |
| render_pose_match_validation.py | Render validation images of the pose-matched rigged mesh + armature. | render_view | unknown | unknown |
| render_rest_pose_compare.py | render_rest_pose_compare.py — Side-by-side rest pose comparison of FLVER c3100 m | clear_scene, import_fbx, setup_lighting, add_camera, render_to | unknown | unknown |
| render_sentinel_attack.py | Render Sentinel Attack01 animation frames for review. |  | unknown | unknown |
| render_sentinel_attack_v2.py | Render Attack01 with tracking camera + bone diagnostics. |  | unknown | unknown |
| render_v5_check.py | Quick render check: Open the v5 blend file, apply idle anim, render with proper  | get_mesh_bounds, setup_ortho_camera, main | unknown | unknown |
| reskin_c2120_mesh.py | Re-skin c2120 mesh to use the animation skeleton bone hierarchy. | clear_scene, import_fbx, find_armature, find_meshes, main | unknown | unknown |
| reskin_c2120_mesh_v2.py | Re-skin c2120 mesh v2 - fixes zero-weight vertex bug. | clear_scene, import_fbx, find_nearest_bone, main | unknown | unknown |
| reskin_c2120_mesh_v3.py | Re-skin c2120 mesh v3 - fixes zero-weight vertices AND bone axis mismatch. | clear_scene, import_fbx, find_nearest_bone, rotate_bones_y_to_z, main | unknown | unknown |
| reskin_guard_flver_transfer.py | Blender 5.0 headless script: Re-skin the Mannequin mesh to the c3100 skeleton | clear_scene, import_fbx, build_kdtree, transfer_weights_kdtree, verify_weights | unknown | unknown |
| reskin_guard_v2.py | Blender 5.0 headless script: Re-skin Mannequin to c3100 skeleton using | clear_scene, import_fbx, pose_to_apose, get_deformed_positions, kdtree_transf... | unknown | unknown |
| reskin_guard_v3.py | Blender 5.0 headless script: Re-skin Mannequin to c3100 skeleton using | clear_scene, import_fbx, print_bone_directions, compute_bone_rotation, pose_t... | unknown | unknown |
| reskin_guard_v4.py | Blender 5.0 headless script: Re-skin Mannequin to c3100 skeleton using | clear_scene, import_fbx, classify_src_vertices_by_bone, classify_tgt_vertices... | unknown | unknown |
| reskin_guard_v5.py | Blender 5.0 headless script: Re-skin Mannequin to c3100 skeleton using | clear_scene, import_fbx, get_bone_local_matrix, to_bone_local, classify_and_t... | unknown | unknown |
| reskin_guard_v6.py | Blender 5.0 headless: Re-skin Mannequin to c3100 using bone-local-space | clear_scene, import_fbx, get_bone_local_matrix, classify_and_transform_src, c... | unknown | unknown |
| reskin_guard_v7.py | Reskin Guard v7: Auto-weights with A-pose duplicate armature. | clean_scene, import_fbx, import_action, apply_action, pose_arm_to_apose | unknown | unknown |
| reskin_guard_v8.py | Reskin Guard v8: Vertex-space arm correction + non-deform bone auto-weights. | clean_scene, import_fbx, import_action, apply_action, get_arm_vertex_weights | unknown | unknown |
| reskin_mesh.py | Reskin Mesh - Generalized wrapper for reskin_to_c3100.py |  | unknown | unknown |
| reskin_pipeline.py | Master Reskin Pipeline Orchestrator | log, find_anibnd, find_chrbnd, find_anim_fbx, list_available_anims | unknown | unknown |
| reskin_sentinel.py | Reskin Sentinel: Create forensically distinct mesh from Manny for the Sentinel e | clean_scene, import_fbx, get_arm_vertex_weights, rotate_vertices, compute_arm... | unknown | unknown |
| reskin_to_c3100.py | Reskin Pipeline: Replace a FromSoft character mesh with a custom mesh while | log, get_mesh_bounds, import_mesh, join_meshes, align_mesh_to_armature | unknown | unknown |
| rest_pose_fix.py | rest_pose_fix.py — Match Warlord rest pose to FLVER c3100, with before/after ren | clear_scene, import_fbx, bone_table, setup_render_scene, add_camera | unknown | unknown |
| retarget_all_c4810.py | Batch retarget all c4810 (Malenia) animations to Mannequin skeleton. | log | unknown | unknown |
| retarget_and_fix_animgraph.py | Retarget animations to mesh skeleton using C++ and rebuild AnimGraph. | main | unknown | unknown |
| retarget_c3100_proper.py | Properly retarget c3100 (Crucible Knight) animations to UE5 Mannequin skeleton. | should_remove_bone, get_armature_rest_pose_cs, extract_mannequin_rest_pose, p... | unknown | unknown |
| retarget_c3100_test_one.py | Test proper retarget with a single animation to verify approach. | should_remove_bone, build_cs, build_er_cs | unknown | unknown |
| retarget_c3100_to_mannequin.py | Retarget c3100 (Crucible Knight) animations to UE5 Mannequin skeleton by renamin | should_remove_bone, get_mannequin_name, process_animation, main | unknown | unknown |
| retarget_c4810_to_mannequin.py | Retarget c4810 (Malenia) animations to UE5 Mannequin skeleton by renaming bones. | should_remove_bone, get_mannequin_name, process_animation, main | unknown | unknown |
| retarget_compare.py | Compare retarget methods on a single test animation. | log | unknown | unknown |
| retarget_minimal.py | Minimal IK Retarget - just the retarget call. | log | unknown | unknown |
| retarget_native.py | Test UE5's built-in IKRetargetBatchOperation to retarget c4810 animations. | log | unknown | unknown |
| retarget_one_anim.py | Retarget a single c4810 animation using the already-created IK Retarget assets. |  | unknown | unknown |
| retarget_review_step1.py | Step 1: Compare c3100 and c3180 skeletons, then render visual comparison. | load_chr, create_bone_viz_material, create_bone_sphere, create_bone_cylinder,... | unknown | unknown |
| retarget_review_step2.py | Step 2: Cross-skeleton retarget with armature-space rest-pose correction. | load_chr, retarget_animation, create_mat, create_sphere, create_cylinder | unknown | unknown |
| retarget_review_step3.py | Step 3: Test retarget on actual Sentinel mesh. | load_chr, retarget_animation, setup_camera, render | unknown | unknown |
| retarget_review_step4.py | Step 4: Test retarget with multiple source enemies on Sentinel mesh. | load_chr, retarget_animation, setup_and_render | unknown | unknown |
| retarget_sentinel_arp.py | retarget_sentinel_arp.py - Retarget c3100 animations to ARP-rigged Sentinel mesh | enable_arp, clean_orphans, get_action_fcurves, import_anim_fbx, strip_spuriou... | unknown | unknown |
| retarget_sidebyside.py | Side-by-side comparison renders: c3180 native vs c3180->c3100 retarget. | get_bone_region, load_chr, create_bone_visualization, retarget_rotation_only,... | unknown | unknown |
| retarget_sidebyside_v2.py | Side-by-side retarget comparison v2. | get_bone_region, load_chr, add_bone_spheres, get_bone_positions, compute_boun... | unknown | unknown |
| retarget_sidebyside_v3.py | Side-by-side v3: Both armatures in ONE scene, large bright bone markers, | load_chr, make_emissive_mat, add_bone_markers, retarget_rotation_only | unknown | unknown |
| retarget_sidebyside_v4.py | Side-by-side v4: COMPONENT-SPACE rotation delta retarget. | load_chr, make_emissive_mat, add_bone_markers, retarget_component_space | unknown | unknown |
| retarget_sidebyside_v5.py | Side-by-side v5: WORLD-SPACE rotation delta retarget. | load_chr, make_emissive_mat, add_bone_markers, retarget_component_space | unknown | unknown |
| retarget_test_v3.py | Test IK Retarget with corrected duplicate_and_retarget signature. | log | unknown | unknown |
| retarget_test_v4.py | Test IK Retarget with AssetData array (not loaded UObjects). | log | unknown | unknown |
| retarget_test_v5.py | Test IK Retarget - safe version with try/except around each call. | log | unknown | unknown |
| retarget_validate_v1.py | Autonomous Retarget Validation - Iteration 1 | clean_scene, import_fbx, import_action, apply_action, get_mesh_bounds | unknown | unknown |
| retarget_validate_v2.py | Autonomous Retarget Validation - Iteration 2 | clean_scene, import_fbx, import_action, apply_action, get_armature_bounds | unknown | unknown |
| retarget_validate_v3.py | Autonomous Retarget Validation - Iteration 3 | clean_scene, import_fbx, import_action, apply_action, get_mesh_bounds | unknown | unknown |
| retarget_via_ik.py | Retarget c4810 animations to SK_Mannequin using UE5 IK Retargeter. | log, get_or_create_rig | unknown | unknown |
| revert_c4810_skeleton.py | Revert c4810_model mesh back to its original skeleton. | log | unknown | unknown |
| revert_malenia_skeleton.py | Revert c4810_malenia mesh to its original skeleton. |  | unknown | unknown |
| reweight_v8_to_hkx.py | Re-weight the v8 Mannequin mesh to the correct HKX Blender-space armature. | clean_scene, strip_root_motion, setup_viewport, main | unknown | unknown |
| rig_ai_mesh.py | rig_ai_mesh.py - Rig Meshy AI Ironbound Warlord mesh onto Sentinel skeleton. | enable_addon | unknown | unknown |
| rig_ai_mesh_final.py | rig_ai_mesh_final.py - Rig the AI mesh to the forensically-clean 68-bone skeleto |  | unknown | unknown |
| rig_ironbound_arp.py | rig_ironbound_arp.py — Rig the Ironbound Warlord mesh to the exact UE5 ARP skele | clean_scene, build_arp_armature, scale_armature_bones, import_warlord_mesh, g... | unknown | unknown |
| rig_ironbound_warlord.py | rig_ironbound_warlord.py — Rig the Meshy AI Ironbound Warlord mesh to the Sentin | clean_scene, import_arp_skeleton, scale_armature_bones, import_warlord_mesh, ... | unknown | unknown |
| rig_mesh_to_anim_skeleton.py | Rig FLVER mesh to the ANIMATION skeleton (single skeleton approach). | log, find_nearest_bone, main | unknown | unknown |
| rig_qr_result.py | rig_qr_result.py - Phase 2: Rig QR result mesh to FLVER armature. |  | unknown | unknown |
| rig_sentinel_quick.py | Quick pipeline: Voxel remesh AI mesh -> auto-weight to skeleton -> export FBX. |  | unknown | unknown |
| rig_sentinel_to_c3100.py | Re-rig the Sentinel mesh to the c3100 FLVER skeleton. |  | unknown | unknown |
| rig_sentinel_v2.py | rig_sentinel_v2.py - Rig AI mesh to Sentinel skeleton with robust weighting. | count_weighted_verts, clean_mesh, bone_distance_weight, try_auto_weight, try_... | unknown | unknown |
| rig_sentinel_v3.py | rig_sentinel_v3.py - Rig AI mesh using QuadRemesher for clean topology. | count_weighted_verts, mesh_health | unknown | unknown |
| rig_warlord_to_flver.py | rig_warlord_to_flver.py - Rig Ironbound Warlord mesh directly to FLVER armature. | count_weighted_verts | unknown | unknown |
| run_cpp_retarget.py | Call C++ BatchRetargetAnimations via thin Python wrapper. | log | unknown | unknown |
| save_c3100_blend.py | Save c3100 mesh as a .blend file so user can inspect in Blender GUI. | log, load_flver, main | unknown | unknown |
| save_c3100_with_anim.py | Build c3100 .blend with mesh + animation applied to the armature. | log, load_flver, build_flver_armature_and_mesh, assign_action_to_armature, main | unknown | unknown |
| save_c3100_with_anim_v2.py | Build c3100 .blend: Import anim FBX (keep its armature), build FLVER mesh, paren | log, load_flver, main | unknown | unknown |
| scan_all_skeletons.py | Scan ALL Elden Ring character skeletons from game archives (in-memory). |  | unknown | unknown |
| scan_all_skeletons_v2.py | Scan ALL Elden Ring character skeletons by extracting skeleton.hkx bytes |  | unknown | unknown |
| screenshot_rotfix_comparison.py | Create screenshots of rotfix comparison .blends. | enable_arp, get_action_fcurves, strip_non_root_translations, setup_viewport_c... | unknown | unknown |
| set_preview_mesh.py | Set SKM_Manny_Simple as the preview mesh for c4810_mannequin skeleton. | log | unknown | unknown |
| setup_animgraph_and_blendspace.py | Step 9: Set up AnimGraph and BlendSpace1D for c4810 boss. | anim_id_to_asset_name, ensure_blendspace_exists, main | unknown | unknown |
| setup_bone_mapping.py | Bone Mapping Setup — Interactive Blender Script | load_skeleton_and_sample_anim | unknown | unknown |
| setup_bone_mapping_batch.py | Batch Bone Mapping — Processes ALL humanoid source skeletons in one Blender sess | load_skeleton, process_skeleton, run_batch | unknown | unknown |
| setup_c2120_animgraph.py | Set up AnimGraph and BlendSpace1D for c2120 (Malenia) boss. | anim_id_to_asset_name, ensure_blendspace_exists, main | unknown | unknown |
| setup_c2120_boss.py | Set up boss AnimBlueprint and AnimMontages for c2120 (Malenia). | anim_id_to_asset_name, ensure_directory_exists, load_skeleton, create_montage... | unknown | unknown |
| setup_c2120_v4_compat.py | Set up Compatible Skeletons and assign c2120_v4 mesh to boss. | main | unknown | unknown |
| setup_c3100_enemy_guard.py | Set up c3100 (Crucible Knight) retargeted animations for Enemy Guard. | log, get_notify_info, anim_id_to_asset_name, find_anim_sequence, create_montages | unknown | unknown |
| setup_c3100_guard_ue5.py | Set up c3100 guard reskinned animations in UE5. | log, out, cleanup_duplicates, create_anim_blueprint, create_blendspace | unknown | unknown |
| setup_c3100_preview.py | Set preview mesh on c3100 skeleton using C++ automation. | log | unknown | unknown |
| setup_c4810_boss.py | Set up boss AnimBlueprint and AnimMontages for c4810 (Malenia). | anim_id_to_asset_name, ensure_directory_exists, load_skeleton, create_montage... | unknown | unknown |
| setup_c4810_preview.py | Set up c4810 Compatible Skeletons for previewing animations on the ER mesh. | log | unknown | unknown |
| setup_compat_skeletons.py | Set up Compatible Skeletons between c2120 mesh skeleton and animation skeleton. | add_compatible, main | unknown | unknown |
| setup_compatible_skeletons.py | Set up Compatible Skeletons between c4810_mannequin and SK_Mannequin. | log | unknown | unknown |
| setup_ik_retarget.py | Set up IK Retarget pipeline: c4810 (Elden Ring) -> UE5 Mannequin. |  | unknown | unknown |
| setup_ik_retarget_test.py | Set up IK Retarget from c4810 (Malenia) to SK_Mannequin and retarget one test an | log | unknown | unknown |
| setup_ik_retargeter.py | Set up IK Rigs and IK Retargeter for c4810 -> Mannequin retargeting. | log | unknown | unknown |
| setup_malenia_preview.py | Set up the actual Malenia mesh for animation preview. | log | unknown | unknown |
| setup_preview_cpp.py | Use C++ automation to set up Compatible Skeletons and preview mesh for c4810. | log | unknown | unknown |
| setup_v11_compat_and_abp.py | Set up Compatible Skeletons between v11 and animation skeleton, then assign Anim | main | unknown | unknown |
| strip_bone_rotation.py | Strip CoB rotation from FLVER mesh bones and re-export. | log, main | unknown | unknown |
| survey_humanoid_attacks.py | survey_humanoid_attacks.py - Survey ALL enemies in unpacked Elden Ring animation | parse_anim_id, categorize_anim, scan_binder, main | unknown | unknown |
| swap_boss_standin_mesh.py | Swap the Malenia boss to use a visible stand-in mesh for combat testing. | log, get_mesh_component, set_mesh_and_abp, main | unknown | unknown |
| swap_boss_to_mannequin.py | Swap boss to standard Mannequin mesh + enemy AnimBP. | log, main | unknown | unknown |
| swap_c3100_anim.py | Swap the animation on the reskinned c3100 mesh and re-export FBX. | log | unknown | unknown |
| swap_skeleton_cpp.py | Use C++ SetMeshSkeleton to swap the mesh to the animation skeleton. | main | unknown | unknown |
| swap_skeleton_simple.py | Swap the c2120_matched mesh's skeleton to the animation skeleton. | main | unknown | unknown |
| swap_to_v3_mesh.py | Swap boss to v3 mesh (uses animation skeleton directly, no Compatible Skeletons  | main | unknown | unknown |
| tae_parser.py | TAE (TimeAct Event) Parser for FromSoftware Games (Elden Ring, DS3, Sekiro) | **EventType, JumpTableFlag, TAEEvent**; export_all_animations, print_animatio... | unknown | unknown |
| test_decrypt.py | Quick test to find chr/ files across all ER archives. |  | unknown | unknown |
| test_direct_anim_play.py | Test direct animation playback bypassing AnimGraph. | main | unknown | unknown |
| test_direct_cross_skeleton.py | Test: Soulstruct direct import of c3180 animations onto c3100 armature+mesh. | load_chr | unknown | unknown |
| test_multisource_blend.py | Test multi-source animation mixing: import one animation from each candidate | strip_root_motion, strip_weapon_bone_fcurves | unknown | unknown |
| test_reskin_dummy.py | Generate a simple dummy humanoid mesh to test the reskin pipeline. | log | unknown | unknown |
| test_rest_pose.py | Temporarily remove AnimBP to show mesh in rest pose. | log, main | unknown | unknown |
| test_retarget_result.py | Quick test: import a retargeted FBX and check if animation data exists. |  | unknown | unknown |
| test_retarget_result2.py | Check if retargeted animation has actual varying keyframe data. |  | unknown | unknown |
| test_soulstruct_direct.py | test_soulstruct_direct.py - Test bypassing ARP retarget for Sentinel animations. | strip_root_motion, build_hkx_armature, import_and_rig_warlord_mesh, main | unknown | unknown |
| test_write.py | Diagnose boss A-pose issue. |  | unknown | unknown |
| test_zup_anim_direct.py | Test Z-up animation by playing it directly on the mesh (no AnimBP). | log, main | unknown | unknown |
| top5_blend_export.py | Export .blend files for the top 5 humanoid ARP retarget candidates. | enable_arp, get_action_fcurves, strip_non_root_translations, strip_weapon_bon... | unknown | unknown |
| unpack_er_archives.py | Selective unpacker for Elden Ring BHD/BDT archives. | **BHD5FileHeader**; compute_path_hash, decrypt_bhd_rsa, _parse_rsa_public_key... | unknown | unknown |
| validate_mesh_fbx.py | Comprehensive validation of c2120_matched.fbx in Blender. | log, clear_scene, import_fbx, validate_mesh | unknown | unknown |
| validate_original_mesh.py | Validate original FLVER mesh FBX - check bone-vertex consistency. | log, import_fbx, get_bone_world_positions, validate_mesh_bone_consistency, an... | unknown | unknown |
| validate_reskin.py | Validate reskinned mesh against original FLVER mesh. | log, compute_bone_weight_stats, compute_deformation_quality | unknown | unknown |
| validate_reskinned_mesh.py | Validate the reskinned mesh by testing deformation with an animation. | log, clear_scene, main, save_report | unknown | unknown |
| validate_retarget_renders.py | Render validation frames from retargeted Mannequin FBXs. | create_bone_sticks, clear_bone_sticks, setup_camera, render_frames, save_blen... | unknown | unknown |
| validate_sentinel_fbx.py | Validate Sentinel FBX animation files in Blender headless. |  | unknown | unknown |
| validate_sentinel_retarget.py | Blender visual validation for Sentinel animation retarget. | clear_scene, setup_render, render_frame, import_fbx, get_action | unknown | unknown |
| validate_sentinel_stickfig.py | Matplotlib stick-figure validation for Sentinel animation retarget. | quat_to_matrix, local_to_cs, compute_cs_positions, render_stick_figure, rende... | unknown | unknown |
| verify_abp_c2120_final.py | Final verification of ABP_c2120_Boss setup. | main | unknown | unknown |
| verify_and_assign_c2120_v3.py | Verify the re-imported c2120 mesh and assign to boss Blueprint. | main | unknown | unknown |
| verify_anim_tracks.py | Verify retargeted AnimSequences have bone track data by checking raw key counts. | log, check_anim | unknown | unknown |
| verify_c2120_boss_final.py | Final verification of c2120 boss setup before PIE. | check_asset_exists, count_assets_in_dir, main | unknown | unknown |
| verify_c2120_montages.py | Verify c2120 montages have source animations assigned. |  | unknown | unknown |
| verify_c3100_final.py | Final verification: c3100 abilities -> montages -> skeleton chain is correct. | log | unknown | unknown |
| verify_c3100_refpose_fix.py | Verify c3100 reference pose fix by checking animation bone transforms. | log, out | unknown | unknown |
| verify_c3100_setup.py | Verify c3100 (Crucible Knight) setup for Enemy Guard is complete. | log | unknown | unknown |
| verify_c4810_anim_mesh.py | Verify c4810 anim mesh import and set up Compatible Skeletons. | log | unknown | unknown |
| verify_malenia_assets.py | Verify state of Malenia mesh assets in /Game/Temp/malenia/. | log | unknown | unknown |
| verify_mesh_import.py | Verify the imported c4810_model SkeletalMesh. | log, main | unknown | unknown |
| verify_rt_anim.py | Verify retargeted animation has actual bone movement by evaluating compressed da | log | unknown | unknown |
| verify_rt_batch.py | Verify retargeted animations have movement and reasonable bone positions. | log | unknown | unknown |
| verify_rt_compressed.py | Quick check: compare frame 0 pelvis across animations to verify they're not all  | check_anim | unknown | unknown |
| verify_rt_quick.py | Quick verification of retargeted animations - checks bone movement. | log | unknown | unknown |
| verify_rt_setup.py | Verify retargeted montage setup: boss mesh, AnimBP, ability montages all on same | log | unknown | unknown |
| verify_rt_v2.py | Verify retargeted vs source animations using C++ evaluation. | log | unknown | unknown |
| verify_zup_anims.py | Verify Z-up animation import and set up for PIE test. | log, main | unknown | unknown |
| visual_compare_v6.py | Comprehensive visual comparison: Original c3100 knight vs v6 reskinned Mannequin | clean_scene, import_fbx, import_action, apply_action, get_combined_bounds | unknown | unknown |
| visual_compare_v6_quick.py | Quick visual comparison test: just rest pose + idle + walk from front view. | clean_scene, import_fbx, import_action, apply_action, get_mesh_height | unknown | unknown |
| visual_compare_v6_v2.py | Visual comparison v2: Import original at 100x scale to match cm-scale v6. | clean_scene, import_mesh, import_action, apply_action, clear_action | unknown | unknown |
| visual_compare_v7.py | Visual comparison: Original c3100 Knight vs V7 Reskinned Mannequin. | clean_scene, import_fbx, import_action, apply_action, get_mesh_bounds | unknown | unknown |

## Elden Ring Tools (test_meshes) (`C:/scripts/elden_ring_tools/test_meshes`)
**56 files**

| File | Description | Key Functions/Classes | Created | Modified |
|------|-------------|----------------------|---------|----------|
| _align_to_reference.py | Align weapon to match original model.fbx reference. |  | unknown | unknown |
| _apply_texture.py | Apply textures from GLB to rigged.blend and render textured screenshots. |  | unknown | unknown |
| _apply_texture_to_anims.py | Apply GLB textures + lighting to all animation .blend files. | add_lighting_setup, apply_glb_material | unknown | unknown |
| _check_bones.py | (no description) |  | unknown | unknown |
| _check_idle.py | Quick check: render idle animation at rest-like frames. |  | unknown | unknown |
| _check_rigged_baseline.py | Compare weapon position in rigged.blend (baseline) vs animation .blend. |  | unknown | unknown |
| _check_uvs.py | (no description) |  | unknown | unknown |
| _check_weapon_attach.py | Quick check: render attack animation at multiple frames to verify weapon attachm |  | unknown | unknown |
| _debug_texture.py | Debug: inspect GLB material nodes and texture images. |  | unknown | unknown |
| _diagnose_body.py | Diagnose: check body mesh modifiers, vertex groups, and armature link. |  | unknown | unknown |
| _diagnose_body_mod.py | Check body mesh modifiers in rigged.blend and animation blends. |  | unknown | unknown |
| _diagnose_deform.py | Check if body mesh ACTUALLY deforms using depsgraph evaluation. |  | unknown | unknown |
| _diagnose_vg_match.py | Check if vertex group names match bone names in anim .blend. |  | unknown | unknown |
| _diagnose_weapon.py | Diagnose: compare weapon and body positions at multiple frames. |  | unknown | unknown |
| _diagnose_weapon2.py | Deep diagnosis: check weapon bone position vs weapon mesh position at multiple f |  | unknown | unknown |
| _diagnose_weights.py | Diagnose weight painting issues in rigged.blend files. |  | unknown | unknown |
| _render_anim_check.py | Render attack animation frames from a .blend to check weapon attachment. |  | unknown | unknown |
| _render_preview.py | clean orphans |  | unknown | unknown |
| _save_textures.py | (no description) |  | unknown | unknown |
| _test_bright_material.py | Apply a bright red checkerboard to verify textures show in viewport. |  | unknown | unknown |
| _verify_all_anims.py | Render verification frames from multiple animation types to confirm weapon attac |  | unknown | unknown |
| _verify_texture.py | Open a saved animation blend |  | unknown | unknown |
| align_weapon_preview.py | align_weapon_preview.py — Position weapon in hand and render preview screenshots |  | unknown | unknown |
| batch_extract_all.py | batch_extract_all.py -- Extract ALL animations from ALL compatible Elden Ring | get_sources, print_plan, run_blender_source, organize_outputs, run_pipeline | unknown | unknown |
| batch_extract_c3100_loco.py | Extract c3100 walk/idle/run for ALL humanoid enemies with forensic transforms. |  | unknown | unknown |
| batch_extract_wanderer.py | batch_extract_wanderer.py -- Extract 100+ animations from 13 Elden Ring sources | print_plan, run_blender, run_pipeline | unknown | unknown |
| batch_full_production.py | batch_full_production.py — Full production pipeline for ALL 47 enemies. | load_plan, load_skel_map, load_progress, save_progress, get_faction_for_enemy | unknown | unknown |
| batch_rig_all_enemies.py | Batch rig all pose-first enemies using rig_enemy_production.py. | run_enemy, main | unknown | unknown |
| batch_sample_anims.py | batch_sample_anims.py -- Generate a small animation sample for each rigged enemy | get_rigged_enemies, load_skeleton_mapping, sample_enemy, run | unknown | unknown |
| batch_submit_all.py | batch_submit_all.py -- Submit ALL concept images to Meshy, poll, download, then  | load_tracker, save_tracker, get_all_concepts, submit_all, check_status | unknown | unknown |
| compare_hand_closeup.py | Render close-up of right hand area in rigged.blend vs animation blend at REST po | setup_scene, render_view, get_hand_pos | unknown | unknown |
| debug_weapon_pos.py | Debug weapon position: compare rigged.blend rest pose vs animation frame. |  | unknown | unknown |
| diagnose_weapon_drift.py | Diagnose weapon drift: compare bone positions at rest vs animated. |  | unknown | unknown |
| download_meshy_models.py | download_meshy_models.py - Download completed Meshy AI models (FBX + textures). | get_task_status, download_file, download_enemy, main | unknown | unknown |
| export_retargeted_anims.py | Export ARP-retargeted animations from other Elden Ring humanoids to an existing  | enable_arp, get_action_fcurves, strip_non_root_translations, strip_weapon_bon... | unknown | unknown |
| fix_flagellant_weights.py | Fix ashen_flagellant weights: |  | unknown | unknown |
| fix_root_keyframes.py | Fix missing root bone animation track in FBX exports. |  | unknown | unknown |
| generate_all_concepts.py | Generate ALL faction enemy concept art for Meshy AI submission. | glow, generate_image, main | unknown | unknown |
| generate_body_weapon.py | Generate separate body + weapon concepts for the two-mesh rig pipeline. | gemini_generate, generate_concepts, submit_to_meshy, check_status, download_m... | unknown | unknown |
| generate_enemy_concepts.py | Generate 3D enemy meshes for Ashborne using Meshy AI text-to-3D pipeline. | load_tasks, save_tasks, submit_previews, check_status, submit_refines | unknown | unknown |
| generate_faction_concepts.py | Generate faction concept art using Gemini Pro image model with Taken reference. | generate_image, main | unknown | unknown |
| regenerate_fixes.py | Regenerate specific enemy concepts that need fixes. | glow, generate_image | unknown | unknown |
| regenerate_shield_fix.py | Regenerate veilborn_sentry and withered_hulk — shield must be on LEFT arm. | glow, generate_image | unknown | unknown |
| render_anim_previews.py | Render animation preview snapshots for all rigged enemies. | setup_scene, frame_armature, find_blend_for_anim, render_enemy, main | unknown | unknown |
| render_rest_pose_mannequin.py | Render the c3100 FLVER skeleton rest pose as a mannequin for AI reskinning. | build_hkx_armature, should_skip_bone, get_bone_radius, get_child_bone_head, c... | unknown | unknown |
| reskin_mannequin.py | Reskin the FLVER rest pose mannequin for each enemy using Gemini image generatio | reskin_enemy, main | unknown | unknown |
| rig_enemy_production.py | rig_enemy_production.py — Production pipeline for Ashborne custom enemies. | enable_arp, build_hkx_armature, import_and_rig_mesh, import_weapon_mesh, stri... | unknown | unknown |
| rig_meshy_to_c3100.py | rig_meshy_to_c3100.py - Rig any Meshy AI mesh to c3100 (Crucible Knight) HKX ske | build_hkx_armature, strip_root_motion, import_and_rig_mesh, main | unknown | unknown |
| rig_multisource.py | rig_multisource.py - Rig a Meshy AI mesh to c3100 skeleton with animations | enable_arp, get_action_fcurves, strip_non_root_translations, strip_weapon_bon... | unknown | unknown |
| rig_weight_transfer.py | rig_weight_transfer.py - Rig AI meshes by transferring weights from the original |  | unknown | unknown |
| rig_with_weapon.py | rig_with_weapon.py - Rig body + separate weapon to c3100 skeleton with visual al | build_hkx_armature, setup_camera, render_screenshot, render_comparison_screen... | unknown | unknown |
| submit_concepts_to_meshy.py | Submit Gemini-reskinned concept art to Meshy Image-to-3D pipeline. | load_tasks, save_tasks, submit_tasks, check_status, download_meshes | unknown | unknown |
| submit_single_enemy.py | Submit a single enemy concept image to Meshy Image-to-3D with optimized settings | get_task_file, load_task, save_task, list_concepts, submit | unknown | unknown |
| verify_bone_overlay.py | Render with armature visible to verify bone-weapon alignment. |  | unknown | unknown |
| verify_rest_vs_anim.py | Compare weapon position at REST pose vs animation frame 0. |  | unknown | unknown |
| verify_weapon_tracking.py | Render screenshots at multiple frames to verify weapon tracks the hand. |  | unknown | unknown |


---
**Total: 3131 files cataloged**

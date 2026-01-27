/**
 * Migrate BlendListByEnum nodes from Blueprint enum to C++ enum
 * This properly replaces nodes: saves connections, maps enum entry names by VALUE, reconstructs, restores connections
 *
 * The key challenge is that Blueprint enums use internal names like "NewEnumerator0", "NewEnumerator1"
 * while C++ enums use proper names like "Unarmed", "OneHanded", etc.
 * We map by INTEGER VALUE, not by name.
 */
int32 USLFAutomationLibrary::MigrateBlendListByEnumToCpp(UObject* AnimBlueprintAsset, const FString& OldEnumPath, const FString& NewEnumPath)
{
	if (!AnimBlueprintAsset) return 0;

	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UBlueprint* BP = Cast<UBlueprint>(AnimBlueprintAsset);
		if (BP && BP->GeneratedClass)
		{
			AnimBP = Cast<UAnimBlueprint>(BP);
		}
	}
	if (!AnimBP) return 0;

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== MigrateBlendListByEnumToCpp ==="));
	UE_LOG(LogSLFAutomation, Warning, TEXT("  Old enum: %s"), *OldEnumPath);
	UE_LOG(LogSLFAutomation, Warning, TEXT("  New enum: %s"), *NewEnumPath);

	// Load the new C++ enum
	UEnum* NewEnum = FindObject<UEnum>(nullptr, *NewEnumPath);
	if (!NewEnum)
	{
		NewEnum = LoadObject<UEnum>(nullptr, *NewEnumPath);
	}
	if (!NewEnum)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Failed to find new enum: %s"), *NewEnumPath);
		return 0;
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("  Found new enum: %s with %d entries"), *NewEnum->GetName(), NewEnum->NumEnums());

	// Log C++ enum entries
	for (int32 i = 0; i < NewEnum->NumEnums(); i++)
	{
		FName EntryName = NewEnum->GetNameByIndex(i);
		int64 EntryValue = NewEnum->GetValueByIndex(i);
		UE_LOG(LogSLFAutomation, Warning, TEXT("    C++ Enum[%d]: %s = %lld"), i, *EntryName.ToString(), EntryValue);
	}

	// Load the old enum for value mapping
	UEnum* OldEnum = FindObject<UEnum>(nullptr, *OldEnumPath);
	if (!OldEnum)
	{
		OldEnum = LoadObject<UEnum>(nullptr, *OldEnumPath);
	}
	if (OldEnum)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Found old enum: %s with %d entries"), *OldEnum->GetName(), OldEnum->NumEnums());
		for (int32 i = 0; i < OldEnum->NumEnums(); i++)
		{
			FName EntryName = OldEnum->GetNameByIndex(i);
			int64 EntryValue = OldEnum->GetValueByIndex(i);
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Old Enum[%d]: %s = %lld"), i, *EntryName.ToString(), EntryValue);
		}
	}

	int32 MigratedCount = 0;

	// Get ALL graphs
	TArray<UEdGraph*> AllGraphs;
	AllGraphs.Append(AnimBP->FunctionGraphs);
	for (UEdGraph* Graph : AnimBP->UbergraphPages)
	{
		if (Graph) AllGraphs.Add(Graph);
	}

	// Find the VisibleEnumEntries property
	FArrayProperty* VisibleEntriesProp = CastField<FArrayProperty>(
		UAnimGraphNode_BlendListByEnum::StaticClass()->FindPropertyByName(TEXT("VisibleEnumEntries")));

	if (!VisibleEntriesProp)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Could not find VisibleEnumEntries property!"));
		return 0;
	}

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		UE_LOG(LogSLFAutomation, Warning, TEXT("  Checking graph: %s"), *Graph->GetName());

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;
			if (!Node->GetClass()->GetName().Contains(TEXT("BlendListByEnum"))) continue;

			UAnimGraphNode_BlendListByEnum* BlendNode = Cast<UAnimGraphNode_BlendListByEnum>(Node);
			if (!BlendNode) continue;

			UEnum* BoundEnum = BlendNode->GetEnum();
			if (!BoundEnum) continue;

			FString BoundEnumPath = BoundEnum->GetPathName();
			if (!BoundEnumPath.Equals(OldEnumPath, ESearchCase::IgnoreCase))
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("    Node %s uses different enum: %s"), *Node->GetName(), *BoundEnumPath);
				continue;
			}

			UE_LOG(LogSLFAutomation, Warning, TEXT("    Migrating node: %s at (%d, %d)"), *Node->GetName(), Node->NodePosX, Node->NodePosY);

			// Step 1: Save pin connections BY INDEX (BlendPose_0, BlendPose_1, etc.)
			struct FPinConnectionByIndex
			{
				int32 PinIndex;
				TArray<TPair<UEdGraphNode*, FName>> LinkedPins;
			};
			TArray<FPinConnectionByIndex> SavedPoseConnections;
			TArray<TPair<FName, TArray<TPair<UEdGraphNode*, FName>>>> OtherConnections;

			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (!Pin || Pin->LinkedTo.Num() == 0) continue;

				TArray<TPair<UEdGraphNode*, FName>> Links;
				for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
				{
					if (LinkedPin && LinkedPin->GetOwningNode())
					{
						Links.Add(TPair<UEdGraphNode*, FName>(LinkedPin->GetOwningNode(), LinkedPin->PinName));
					}
				}
				if (Links.Num() == 0) continue;

				FString PinStr = Pin->PinName.ToString();
				if (PinStr.StartsWith(TEXT("BlendPose_")))
				{
					int32 PinIndex = FCString::Atoi(*PinStr.RightChop(10));
					FPinConnectionByIndex Conn;
					Conn.PinIndex = PinIndex;
					Conn.LinkedPins = Links;
					SavedPoseConnections.Add(Conn);
					UE_LOG(LogSLFAutomation, Warning, TEXT("      Saved BlendPose_%d: %d links"), PinIndex, Links.Num());
				}
				else
				{
					OtherConnections.Add(TPair<FName, TArray<TPair<UEdGraphNode*, FName>>>(Pin->PinName, Links));
					UE_LOG(LogSLFAutomation, Warning, TEXT("      Saved %s: %d links"), *PinStr, Links.Num());
				}
			}

			// Step 2: Get visible entry VALUES before changing enum
			TArray<int64> VisibleEntryValues;
			{
				FScriptArrayHelper ArrayHelper(VisibleEntriesProp, VisibleEntriesProp->ContainerPtrToValuePtr<void>(BlendNode));
				for (int32 i = 0; i < ArrayHelper.Num(); i++)
				{
					FName* EntryNamePtr = (FName*)ArrayHelper.GetRawPtr(i);
					if (EntryNamePtr && OldEnum)
					{
						int64 EntryValue = OldEnum->GetValueByName(*EntryNamePtr);
						VisibleEntryValues.Add(EntryValue);
						UE_LOG(LogSLFAutomation, Warning, TEXT("      VisibleEntry[%d]: %s -> value %lld"), i, *EntryNamePtr->ToString(), EntryValue);
					}
				}
			}

			// Step 3: Call ReloadEnum to switch to new enum
			BlendNode->ReloadEnum(NewEnum);

			// Step 4: Fix VisibleEnumEntries to use correct C++ enum names
			{
				FScriptArrayHelper ArrayHelper(VisibleEntriesProp, VisibleEntriesProp->ContainerPtrToValuePtr<void>(BlendNode));
				ArrayHelper.EmptyValues();

				for (int64 Value : VisibleEntryValues)
				{
					int32 NewEnumIndex = NewEnum->GetIndexByValue(Value);
					if (NewEnumIndex != INDEX_NONE)
					{
						FName NewEntryName = NewEnum->GetNameByIndex(NewEnumIndex);
						int32 AddedIndex = ArrayHelper.AddValue();
						FName* NewEntryPtr = (FName*)ArrayHelper.GetRawPtr(AddedIndex);
						*NewEntryPtr = NewEntryName;
						UE_LOG(LogSLFAutomation, Warning, TEXT("      Set VisibleEntry[%d]: value %lld -> %s"), AddedIndex, Value, *NewEntryName.ToString());
					}
					else
					{
						UE_LOG(LogSLFAutomation, Warning, TEXT("      [WARN] No C++ enum entry for value %lld"), Value);
					}
				}
			}

			// Step 5: Reconstruct node with new pins
			BlendNode->ReconstructNode();

			// Step 6: Restore BlendPose connections by index
			for (const FPinConnectionByIndex& Conn : SavedPoseConnections)
			{
				FName NewPinName = FName(*FString::Printf(TEXT("BlendPose_%d"), Conn.PinIndex));
				UEdGraphPin* Pin = Node->FindPin(NewPinName);
				if (!Pin)
				{
					UE_LOG(LogSLFAutomation, Warning, TEXT("      Could not find BlendPose_%d after reconstruction"), Conn.PinIndex);
					continue;
				}

				for (const TPair<UEdGraphNode*, FName>& Link : Conn.LinkedPins)
				{
					UEdGraphPin* OtherPin = Link.Key ? Link.Key->FindPin(Link.Value) : nullptr;
					if (OtherPin)
					{
						Pin->MakeLinkTo(OtherPin);
						UE_LOG(LogSLFAutomation, Warning, TEXT("      Restored BlendPose_%d -> %s.%s"), Conn.PinIndex, *Link.Key->GetName(), *Link.Value.ToString());
					}
				}
			}

			// Step 7: Restore other connections
			for (const auto& Conn : OtherConnections)
			{
				UEdGraphPin* Pin = Node->FindPin(Conn.Key);
				if (!Pin)
				{
					UE_LOG(LogSLFAutomation, Warning, TEXT("      Could not find pin %s after reconstruction"), *Conn.Key.ToString());
					continue;
				}

				for (const TPair<UEdGraphNode*, FName>& Link : Conn.Value)
				{
					UEdGraphPin* OtherPin = Link.Key ? Link.Key->FindPin(Link.Value) : nullptr;
					if (OtherPin)
					{
						Pin->MakeLinkTo(OtherPin);
						UE_LOG(LogSLFAutomation, Warning, TEXT("      Restored %s -> %s.%s"), *Conn.Key.ToString(), *Link.Key->GetName(), *Link.Value.ToString());
					}
				}
			}

			MigratedCount++;
		}
	}

	if (MigratedCount > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(AnimBP);
		FKismetEditorUtilities::CompileBlueprint(AnimBP, EBlueprintCompileOptions::SkipGarbageCollection);

		// Save
		UPackage* Package = AnimBP->GetOutermost();
		FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Standalone;
		UPackage::SavePackage(Package, AnimBP, *PackageFileName, SaveArgs);
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== Migrated %d BlendListByEnum nodes ==="), MigratedCount);
	return MigratedCount;
}

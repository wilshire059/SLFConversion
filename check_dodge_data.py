import unreal

output = []
dodge_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
dodge_asset = unreal.EditorAssetLibrary.load_asset(dodge_path)

if dodge_asset:
    output.append("DA_Action_Dodge loaded")
    output.append("Class: " + str(dodge_asset.get_class().get_name()))
    
    try:
        dm = dodge_asset.get_editor_property('DodgeMontages')
        output.append("DodgeMontages struct found")
        
        props = ['forward', 'forward_left', 'forward_right', 'left', 'right', 
                 'backward', 'backward_left', 'backward_right', 'backstep']
        for p in props:
            try:
                val = getattr(dm, p)
                if val:
                    output.append("  " + p + ": " + str(val.get_name()))
                else:
                    output.append("  " + p + ": NULL")
            except Exception as e:
                output.append("  " + p + ": ERROR - " + str(e))
    except Exception as e:
        output.append("DodgeMontages ERROR: " + str(e))
else:
    output.append("Could not load DA_Action_Dodge")

# Write to file
with open("C:/scripts/SLFConversion/dodge_check_result.txt", "w") as f:
    f.write("\n".join(output))
print("Results written to dodge_check_result.txt")

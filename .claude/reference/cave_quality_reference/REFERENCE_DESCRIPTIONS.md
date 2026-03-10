# DA Voxel MultiRoomCave — Quality Reference from Demo Video

Source: https://www.youtube.com/watch?v=KVH-zzC8TV4
Content path shown: `Content > DA_Voxel_MultiRoomCave > Maps`

## Screenshot 1: Final Decorated Cave Interior (Inside Player-View)

**What to match:**
- Dense hanging vines/ivy cascading from ceiling and walls (multiple layers)
- Large organic rock formations with varied heights (floor-to-ceiling pillars)
- Bioluminescent elements: orange berry-like clusters climbing up rock surfaces
- Red/pink mushrooms scattered on floor level (multiple sizes)
- Purple/magenta atmospheric fog filling deep cave areas
- Warm point lights creating bright focal areas against dark cave walls
- Dense ground-level foliage/plants (red, green, brown tones)
- Sparkle/glitter particles near water features
- Overall mood: magical enchanted cave, warm vs cool color contrast
- Density: extremely high prop density, almost no bare rock visible

## Screenshot 2: Voxel Cave Geometry (Blue/Purple Lit)

**What to match:**
- Organic bulging cave walls with repeated bumpy texture pattern (displacement-mapped)
- Smooth walkable floor with gentle height variation (not flat, but walkable)
- Corridor visible between rock formations with pink/magenta backlighting
- Cave wall surface has consistent bump pattern (looks like bubble/polyp textures)
- Floor-to-wall transition is smooth (not a sharp angle)
- Ceiling is rough/organic with varied overhang depths
- Color: blue primary lighting with pink/purple accent lights behind formations
- Sense of depth: corridors lead to other chambers visible in background
- Thick walls create real sense of mass and enclosure

## Screenshot 3: GPU Voxel Engine Showcase (Stalactites + Fog)

**What to match:**
- Dramatic stalactite formations hanging from high ceiling (many, varying lengths)
- Some stalactites are thin/pointy, others are thick/rounded
- Stalactites cluster together naturally (not uniformly spaced)
- Floor-level fog bank (thick, purple/pink, obscures floor details)
- Cave ceiling is very high (tall chamber)
- Blue-cyan primary wall lighting with magenta/pink accent areas
- Bumpy polyp texture on walls (same as Screenshot 2)
- Pink bioluminescent growth clusters at floor level
- Icicle-like formations (some areas have vertical ridges on walls)
- Overall: grand, cathedral-like cave space

## Screenshot 4: Cell Flow Layout Graph (Editor View)

**What to match — Execution Graph:**
- `Create Cells (Voronoi)` → `Create Main Path` → `Create Path` → `Create Path` → `Finalize Graph` → `Result`

**Settings visible (left panel):**
- Cell Flow type
- Random Points: 100
- Room Relax Iterations: 2
- Handle Boundary Cells: checked
- Grid Size: 30 x 30
- Min Group Area: 10.0
- Border Bobbiness: 0.5
- Edge Correction Threshold: 0.3

**3D Layout Preview (center):**
- Voronoi cells forming organic cave layout (NOT rectangular rooms)
- Cells colored by path/type (pink, beige, gray tones)
- Green dots = room centers/nodes
- Yellow dots = path waypoints
- Blue lines = path connections between cells
- Overall shape: irregular, organic, no right angles
- Multiple distinct rooms connected by winding corridors

## Screenshot 5: Top-Down Voxel Mesh (Wireframe Overlay)

**What to match:**
- Organic cave mesh viewed from above showing overall structure
- Multiple connected chambers of varying size
- Cave openings visible as dark holes in the rock surface
- Wireframe cubes = voxel chunk boundaries (32x32x32 chunks)
- Rock surface texture applied (pink/beige rock)
- Mesh has organic rough edges (not straight lines anywhere)
- Some areas have overhangs creating dark shadows below
- Pink/warm point lights visible inside caves from above
- Overall: the Voronoi layout is visible in the organic mesh shape

## Quality Comparison Protocol

At each visual inspection checkpoint, compare against these criteria:
1. **Organic shape**: No rectangular or grid-aligned geometry
2. **Wall texture**: Consistent bump/displacement detail on surfaces
3. **Floor walkability**: Smooth enough to walk, but not flat
4. **Height variation**: High ceilings in chambers, lower in corridors
5. **Stalactites**: Dense, varied lengths, natural clustering
6. **Lighting**: Blue/cyan base + warm/pink accents, pools of light not uniform
7. **Fog**: Floor-level atmospheric haze
8. **Vegetation density**: Nearly every surface has some organic coverage
9. **Prop variety**: Multiple mesh types (vines, mushrooms, crystals, rocks, plants)
10. **Color contrast**: Cool blues vs warm oranges/pinks

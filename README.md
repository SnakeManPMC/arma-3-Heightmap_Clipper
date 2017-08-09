# arma-3-Heightmap_Clipper

Reads heightmap ASC file and clips its elevations to fit arma3 ocean 0 meter system, ie fixes oceans.

ArmA 3 oceans start below 0 meter elevations, basically exact 0 meters is sort of bugged or at least it looks really ugly, you should never use precise 0 meters.

Elevations between 0 to 0.25m should be clipped to 0.25m which is ground, water aka ocean is below 0m.

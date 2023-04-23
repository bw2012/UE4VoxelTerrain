# UE4VoxelTerrain
Unreal Engine 4 procedural voxel terrian example (partially based on Transvoxel™ Algorithm by Eric Lengyel http://transvoxel.org/)
This plugin provides tools to generate, visualize and in-game change volume terrain using voxels.

**Master branch tested with Unreal Engine 4.27.1 [Windows 10]**
**Work in progress**

![Unreal Engine 4 voxel terrian](https://github.com/bw2012/UE4VoxelTerrain/blob/master/terrain.gif?raw=true)

![Grass](https://github.com/bw2012/UE4VoxelTerrain/blob/master/grass.gif?raw=true)

![Cave](https://github.com/bw2012/UE4VoxelTerrain/blob/master/cave.gif?raw=true)

# Download playable demo
UE5 version: [UE5VoxelTerrain-0.0.37.zip](https://cutt.ly/z2yhLnA) 

UE4 version: [UE4VoxelTerrain-0.0.23.zip](https://bit.ly/3v4F13A) 

# UE5 version source code

https://github.com/bw2012/UE5VoxelTerrainDemo

# Features
* Runtime terrain modification
* Procedural landscape/caves generation
* Level of details (per chunk)
* Foliage
* Up to 65535 terrain materials

# Usage
1. Install MS Visual Studio 2022
2. Clone repository properly ```git clone --recursive https://github.com/bw2012/UE4VoxelTerrain.git```
3. This project uses git submodules. Make sure that project are cloned properly and folder UE4VoxelTerrain/Plugins is not empty.
4. Open directory UE4VoxelTerrain
5. Download [Content.zip](https://drive.google.com/file/d/1lvWXYaOzaiHsp0OgZyLBL7G_NSyvf9sT/view?usp=sharing) (~1.2Gb) and unzip it to ```Content``` folder
6. Open project file with Unreal Engine 4.27
7. Wait for compile UE4 KiteDemo shaders (first run may take long time)



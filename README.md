# UE4VoxelTerrain (WIP)
Unreal Engine 4 procedural voxel terrian example (partially based on Transvoxel™ Algorithm by Eric Lengyel http://transvoxel.org/)
This plugin provides tools to generate, visualize and in-game change volume terrain using voxels.

**Master branch tested with Unreal Engine 4.26.2 [Windows 10]**
**Work in progress**

![Unreal Engine 4 voxel terrian](https://github.com/bw2012/UE4VoxelTerrain/blob/master/terrain.gif?raw=true)

![Grass](https://github.com/bw2012/UE4VoxelTerrain/blob/master/grass.gif?raw=true)

![Cave](https://github.com/bw2012/UE4VoxelTerrain/blob/master/cave.gif?raw=true)

# Demo project
Download packaged demo project: [UE4VoxelTerrain-0.0.13.zip](https://bit.ly/3aTWMsD) 

# Features
* Runtime terrain modification
* Procedural landscape/caves generation
* Level of details (per chunk)
* Foliage
* Up to 65535 terrain materials
* Network multiplayer (experimental, WIP)

# Attention
* This project uses git submodules. Make sure that project are cloned properly and folder UE4VoxelTerrain/Plugins is not empty.
* Work in progress. Some features is not finished/buggy/has ugly code/do not meet your expectations.

# Usage
1. Install MS Visual Studio 2019
2. Download last stable project 
3. Or clone repository properly ```git clone --recursive https://github.com/bw2012/UE4VoxelTerrain.git```
4. Open directory UE4VoxelTerrain
5. Download [Content.zip](https://drive.google.com/file/d/1nBXoLTTpq92ENEfLSIoFxh2cTe1AVGJ3/view?usp=sharingi) (1.2Gb) and unzip it to ```Content``` folder
6. Open project file with Unreal Engine 4.26
7. Wait for compile UE4 KiteDemo shaders (first run may take long time)

# License
MIT license

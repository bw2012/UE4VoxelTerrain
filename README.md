# UE4VoxelTerrain
Unreal Engine 4 procedural voxel terrian example (partially based on Transvoxel™ Algorithm by Eric Lengyel http://transvoxel.org/)
This plugin provides tools to generate, visualize and in-game change volume terrain using voxels.

**Master branch tested with Unreal Engine 4.16.2 [Windows 7 x64]**

![Unreal Engine 4 voxel terrian](http://media.indiedb.com/images/games/1/51/50197/ezgif.com-video-to-gif_2.gif)

![Grass](http://www.unrealsandbox.com/grass4.gif)

# Features
* Runtime terrain modification
* Procedural landscape/caves generation
* Level of details (per chunk)
* Foliage
* Up to 65535 terrain materials

# Attention
* This project uses git submodules. Make sure that project are cloned properly and folder UE4VoxelTerrain/Plugins is not empty.
* If you have not idea that means 'git submodules' and 'clone recursive' just download last stable version (see link below). 
* Work in progress. Some features is not finished/buggy/has ugly code/do not meet your expectations.

# Usage
1. Install MS Visual Studio 2015
2. Download last stable project 
3. Or clone repository properly ```git clone --recursive https://github.com/bw2012/UE4VoxelTerrain.git```
4. Open directory UE4VoxelTerrain
5. Open project file with Unreal Engine 4.15

# Download 
**Last stable version 0.0.10-pre-alpha. Tested with Unreal Engine 4.16.2**

Download from my patreon: https://goo.gl/uLmgcx


# Controls
```V ``` - toggle view (top down / third person)

**Top down view:**

```Left mouse button  ``` - move character to selected point

```Right mouse button ``` - use tool 

```1 ``` ... ```7 ``` - change tool

**Third person view:**

```W ``` ```S ``` ```A ``` ```D ``` - move character

```Mouse ``` - camera look



# License
MIT license

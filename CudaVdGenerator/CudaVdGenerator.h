#pragma once


typedef struct TZoneData {
    float x;
    float y;
    float z;

    int c[10];

} TZoneData;

typedef struct TVdGenBlock {
    int size = 0;

    TZoneData* zoneData = nullptr;
    size_t zd_size = 0;

    TDensityVal* voxelData = nullptr;
    size_t vd_size = 0;

    TMaterialId* materialData = nullptr;
    size_t md_size = 0;

    int* cacheData = nullptr;
    size_t cd_size = 0;

} TVdGenBlock;
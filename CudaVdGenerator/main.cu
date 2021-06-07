
#include <stdio.h>
#include <chrono>

// For the CUDA runtime routines (prefixed with "cuda_")
#include <cuda_runtime.h>
//#include <helper_cuda.h>


#define USBT_ZONE_SIZE			1000.f
#define USBT_ZONE_DIMENSION		65

typedef unsigned char TDensityVal;
typedef unsigned short TMaterialId;

#include "CudaVdGenerator.h"

//====================================================================================
// DLL declarations
//====================================================================================

extern "C" __declspec(dllexport) int CudaGetInfo(void);
extern "C" __declspec(dllexport) int CudaGenerateVd(TVdGenBlock* vdGenBlock);


//====================================================================================
// cuda perlin noise
//====================================================================================

__device__ const int p[512] = { 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180, 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };

__device__ float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

__device__ float lerp(float t, float a, float b) { return a + t * (b - a); }

__device__ float grad(int hash, float x, float y, float z) {
    int h = hash & 15;
    float u = h < 8 ? x : y,
        v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
__device__ float noise(float x, float y, float z) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
    int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

    return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
        grad(p[BA], x - 1, y, z)),
        lerp(u, grad(p[AB], x, y - 1, z),
            grad(p[BB], x - 1, y - 1, z))),
        lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),
            grad(p[BA + 1], x - 1, y, z - 1)),
            lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
                grad(p[BB + 1], x - 1, y - 1, z - 1))));
}

//====================================================================================
// 
//====================================================================================

__device__ float3 operator+(const float3& a, const float3& b) {
    return make_float3(a.x + b.x, a.y + b.y, a.z + b.z);
}

__device__ float3 voxelIndexToRelPos(const int x, const int y, const int z, int vd_num, float vd_size) {
    const float step = vd_size / (vd_num - 1);
    const float s = -vd_size / 2;
    float3 v = make_float3(s, s, s);
    float3 a = make_float3(x * step, y * step, z * step);
    v = v + a;
    return v;
}

__device__ float clcGroundLevel(float3 v, int vd_num, float vd_size) {
    const float scale1 = 0.001f; // small
    const float scale2 = 0.0004f; // medium
    const float scale3 = 0.00009f; // big

    const float noise_small = noise(v.x * scale1, v.y * scale1, 0) * 0.5f;
    const float noise_medium = noise(v.x * scale2, v.y * scale2, 0) * 5;
    const float noise_big = noise(v.x * scale3, v.y * scale3, 0) * 10;
    const float gl = ((noise_small + noise_medium + noise_big) * 100) + 205.f;

    return gl;
}

__device__ float clcDensityByGroundLevel(float3 v, float gl) {
    const float Z = v.z;
    const float D = Z - gl;

    if (D > 500) {
        return 0.f;
    }

    if (D < -500) {
        return 1.f;
    }

    const float density = 1 - (1 / (1 + exp(-(Z-gl) / 20)));
    return density;
}

__device__ int clcLinearIndex(int x, int y, int z, int n) {
    return x * n * n + y * n + z;
};


__global__ void cudaGenerateZoneVd(TZoneData* devZoneData, TDensityVal* devVd, TMaterialId* devMd, int vd_num, float vd_size, int totalZoneCount) {
    int idx = blockDim.x * blockIdx.x + threadIdx.x;

    if (idx >= totalZoneCount) {
        return;
    }

    TZoneData zd = devZoneData[idx];
    auto t = vd_num * vd_num * vd_num * idx;

    TDensityVal* vd = devVd + t;
    TMaterialId* md = devMd + t;

    const float3 zoneOrigin = make_float3(zd.x, zd.y, zd.z);
    const float step = vd_size / (vd_num - 1);
    const float s = -vd_size / 2;

    int i = 0;
    for (int x = 0; x < vd_num; x++) {
        for (int y = 0; y < vd_num; y++) {
            float3 v0 = voxelIndexToRelPos(x, y, 0, vd_num, vd_size);
            v0 = v0 + zoneOrigin;
            const float gl = clcGroundLevel(v0, vd_num, vd_size);

            for (int z = 0; z < vd_num; z++) {              
                float3 v = voxelIndexToRelPos(x, y, z, vd_num, vd_size);
                v = v + zoneOrigin;

                float density = clcDensityByGroundLevel(v, gl);
                int d = 255 * density;

                vd[i] = d;
                md[i] = 1;

                i++;
            }

        }
    } 
}

#define LOD_ARRAY_SIZE 7

__global__ void cudaMakeCache(TZoneData* devZoneData, TDensityVal* devVd, int* devCd, int vd_num, float vd_size, int totalZoneCount) {
    int idx = blockDim.x * blockIdx.x + threadIdx.x;

    if (idx >= totalZoneCount) {
        return;
    }

    //TZoneData zd = devZoneData[idx];
    TZoneData* zd_p = &devZoneData[idx];
    auto t = vd_num * vd_num * vd_num * idx;

    TDensityVal* vd = devVd + t;
    int* cd = devCd + t;

    for (int lod = 0; lod < LOD_ARRAY_SIZE; lod++) {
        zd_p->c[lod] = 0;
    }

    for (int x = 0u; x < vd_num; x++) {
        for (int y = 0u; y < vd_num; y++) {
            for (int z = 0u; z < vd_num; z++) {

                int offset = 0;

                for (int lod = 0; lod < LOD_ARRAY_SIZE; lod++) {
                    int s = 1 << lod;

                    int n = (vd_num - 1) >> lod;

                    if (x >= s && y >= s && z >= s) {
                        if (x % s == 0 && y % s == 0 && z % s == 0) {
                            int li[8];
                            li[7] = clcLinearIndex(x, y - s, z, vd_num);
                            li[6] = clcLinearIndex(x, y, z, vd_num);
                            li[5] = clcLinearIndex(x - s, y - s, z, vd_num);
                            li[4] = clcLinearIndex(x - s, y, z, vd_num);
                            li[3] = clcLinearIndex(x, y - s, z - s, vd_num);
                            li[2] = clcLinearIndex(x, y, z - s, vd_num);
                            li[1] = clcLinearIndex(x - s, y - s, z - s, vd_num);
                            li[0] = clcLinearIndex(x - s, y, z - s, vd_num);


                            char corner[8];
                            for (auto i = 0; i < 8; i++) {
                                corner[i] = (vd[li[i]] <= 127) ? -127 : 0;
                            }

                            unsigned long caseCode = ((corner[0] >> 7) & 0x01)
                                | ((corner[1] >> 6) & 0x02)
                                | ((corner[2] >> 5) & 0x04)
                                | ((corner[3] >> 4) & 0x08)
                                | ((corner[4] >> 3) & 0x10)
                                | ((corner[5] >> 2) & 0x20)
                                | ((corner[6] >> 1) & 0x40)
                                | (corner[7] & 0x80);

                            if (caseCode != 0 && caseCode != 255) {
                                cd[offset + zd_p->c[lod]] = li[1];
                                zd_p->c[lod]++;
                            }
                        }
                    }

                    offset += n * n * n;
                }


            }
        }
    }
    

    
}

//====================================================================================
// 
//====================================================================================

typedef unsigned __int64 uint64;

uint64 time_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int CudaGetInfo(void) {
    return 0;
}

int CudaGenerateVd(TVdGenBlock* vdGenBlock) {
    cudaError_t err = cudaSuccess;

    if (vdGenBlock == nullptr) {
        return -1;
    }

    uint64 start0 = time_ms();

    int vd_num = USBT_ZONE_DIMENSION;
    float vd_size = USBT_ZONE_SIZE;

    printf("total_zone_count -> %d \n", vdGenBlock->size);

    auto start1 = time_ms();

    TDensityVal* d_vd = NULL;
    err = cudaMalloc((void**)&d_vd, vdGenBlock->vd_size);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device TDensityVal (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }

    TMaterialId* d_md = NULL;
    err = cudaMalloc((void**)&d_md, vdGenBlock->md_size);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device TMaterialId (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }

    TZoneData* d_zd = NULL;
    err = cudaMalloc((void**)&d_zd, vdGenBlock->zd_size);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device TZoneData (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }

    int* d_cd = NULL;
    err = cudaMalloc((void**)&d_cd, vdGenBlock->cd_size);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device cach data (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }

    printf("Copy input data from the host memory to the CUDA device\n");
    err = cudaMemcpy(d_zd, vdGenBlock->zoneData, vdGenBlock->zd_size, cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy zone data from host to device (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }

    auto end1 = time_ms();
    printf("%d ms\n", (int)(end1 - start1));

    int threadsPerBlock = 256;
    int blocksPerGrid = (vdGenBlock->size + threadsPerBlock - 1) / threadsPerBlock;

    printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);

    auto t1 = time_ms();
    cudaGenerateZoneVd << <blocksPerGrid, threadsPerBlock >> > (d_zd, d_vd, d_md, vd_num, vd_size, vdGenBlock->size);
    err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to launch cudaGenerateZoneVd kernel (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }


    cudaMakeCache << <blocksPerGrid, threadsPerBlock >> > (d_zd, d_vd, d_cd, vd_num, vd_size, vdGenBlock->size);
    err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to launch cudaMakeCache kernel (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }

    err = cudaDeviceSynchronize();
    auto t2 = time_ms();
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to cudaDeviceSynchronize: %s \n", cudaGetErrorString(err));
        return -1;
    } else {
        printf("%d ms\n", (int)(t2 - t1));
    }

    printf("Copy output data from the CUDA device to the host memory\n");
    printf("%d\n", vdGenBlock->vd_size);

    auto start2 = time_ms();

    err = cudaMemcpy(vdGenBlock->voxelData, d_vd, vdGenBlock->vd_size, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy vector C from device to host (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }

    err = cudaMemcpy(vdGenBlock->materialData, d_md, vdGenBlock->md_size, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy vector C from device to host (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }

    err = cudaMemcpy(vdGenBlock->zoneData, d_zd, vdGenBlock->zd_size, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy zone data from device to host (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }

    err = cudaMemcpy(vdGenBlock->cacheData, d_cd, vdGenBlock->cd_size, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy cache data from device to host (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }

    auto end2 = time_ms();
    printf("%d ms\n", (int)(end2 - start2));

    // Free device global memory
    err = cudaFree(d_vd);
    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to free device vector A (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }

    err = cudaFree(d_md);
    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to free device vector A (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }

    err = cudaFree(d_zd);
    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to free device vector A (error code %s)!\n", cudaGetErrorString(err));
        return -1;
    }

    auto end0 = time_ms();
    printf("Finished: %d ms\n", (int)(end0 - start0));

    return 0;
}


/*
 * Copyright (c) 2013-2014 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

#include <stdio.h>
#include <stdlib.h>
#include <VX/vx_khr_tiling.h>

/*! \file
 * \example vx_tiling_median.c
 */

/*! \brief A 3x3 to 1x1 median filter
 * This kernel uses this tiling definition.
 * \code
 * vx_block_size_t outSize = {1,1};
 * vx_neighborhood_size_t inNbhd = {-1,1,-1,1};
 * \endcode
 * \ingroup group_tiling
 */
//! [median_tiling_function]
static int vx_uint8_compare(const void *p1, const void *p2)
{
    vx_uint8 a = *(vx_uint8 *)p1;
    vx_uint8 b = *(vx_uint8 *)p2;
    if (a > b)
        return 1;
    else if (a == b)
        return 0;
    else
        return -1;
}

void median_image_tiling(void * VX_RESTRICT parameters[VX_RESTRICT],
                      void * VX_RESTRICT tile_memory,
                      vx_size tile_memory_size)
{
    vx_uint32 x, y;
    vx_uint32 m, n;
    vx_tile_t *in = (vx_tile_t *)parameters[0];
    vx_tile_t *out = (vx_tile_t *)parameters[1];

    printf("TileHeight: %d \n", vxTileHeight(out,0));
    printf("TileWidth: %d \n", vxTileWidth(out,0));
    printf("TileBlockHeight: %d \n",vxTileBlockHeight(out));
    printf("TileBlockWidth: %d \n",vxTileBlockWidth(out));

    for (y = 0; y < vxTileHeight(out, 0); y+=vxTileBlockHeight(out))
    {
        for (x = 0; x < vxTileWidth(out, 0); x+=vxTileBlockWidth(out))
        {
            for (n = 0u; n < vxTileBlockHeight(out); n++)
            {
                for (m = 0u; m < vxTileBlockWidth(out); m++)
                {
                    vx_int32 j, i;
                    vx_uint8 values[9];
                    vx_uint8 nitems;
                    nitems = sizeof(values)/sizeof(values[0]);
                    values[0] = vxImagePixel(vx_uint8, in, 0, x+m, y+n, -1, -1);
                    values[1] = vxImagePixel(vx_uint8, in, 0, x+m, y+n,  0, -1);
                    values[2] = vxImagePixel(vx_uint8, in, 0, x+m, y+n, +1, -1);
                    values[3] = vxImagePixel(vx_uint8, in, 0, x+m, y+n, -1,  0);
                    values[4] = vxImagePixel(vx_uint8, in, 0, x+m, y+n,  0,  0);
                    values[5] = vxImagePixel(vx_uint8, in, 0, x+m, y+n, +1,  0);
                    values[6] = vxImagePixel(vx_uint8, in, 0, x+m, y+n, -1, +1);
                    values[7] = vxImagePixel(vx_uint8, in, 0, x+m, y+n,  0, +1);
                    values[8] = vxImagePixel(vx_uint8, in, 0, x+m, y+n, +1, +1);
                    qsort(values, nitems, sizeof(vx_uint8), vx_uint8_compare);
                    vxImagePixel(vx_uint8, out, 0, x+m, y+n, 0, 0) = values[4];  /* pick the middle value */
                }
            }
        }
    }
}
//! [median_tiling_function]

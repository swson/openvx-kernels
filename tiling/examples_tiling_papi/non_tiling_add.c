#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <VX/vx.h>
#include <VX/vx_helper.h>
#include <VX/vx_lib_debug.h>
#include <VX/vx_compatibility.h>
#include <VX/vx_types.h>
#include <assert.h>
#define img_w 1024
#define img_h 1024
#include <papi.h>
int main(int argc, char *argv[])
{   int retval;
    retval = PAPI_hl_region_begin("computation");
    vx_status status = VX_SUCCESS;
    vx_context context = vxCreateContext();
    if (vxGetStatus((vx_reference)context) == VX_SUCCESS)
    {
        vx_rectangle_t rect = {1, 1, img_w+1, img_h+1}; // 512x512
        vx_uint32 i = 0;
        vx_image images[] = {
                vxCreateImage(context, img_w+2, img_h+2, VX_DF_IMAGE_U8), // 0:input
                vxCreateImageFromROI(images[0], &rect),       // 1:ROI input
                vxCreateImage(context, img_w, img_h, VX_DF_IMAGE_U8), // 2:alpha
                vxCreateImage(context, img_w, img_h, VX_DF_IMAGE_S16),// 3:add
        };
        vx_enum policy = VX_CONVERT_POLICY_SATURATE;
        status |= vxLoadKernels(context, "openvx-debug");
        if (status == VX_SUCCESS)
        {
            vx_graph graph = vxCreateGraph(context);
            if (vxGetStatus((vx_reference)graph) == VX_SUCCESS)
            {
                vx_node nodes[] = {
                    vxFReadImageNode(graph, "lena_1024x1024.pgm", images[1]),
                    vxFReadImageNode(graph, "non_tiling_alpha_lena_1024x1024.pgm", images[2]),
                    vxAddNode(graph, images[1], images[2], policy, images[3]),
                    vxFWriteImageNode(graph, images[3], "non_tiling_add_lena_1024x1024.pgm"),
                };
                for (i = 0; i < dimof(nodes); i++)
                {
                    if (nodes[i] == 0)
                    {
                        printf("Failed to create node[%u]\n", i);
                        status = VX_ERROR_INVALID_NODE;
                        break;
                    }
                }
                if (status == VX_SUCCESS)
                {
                    status = vxVerifyGraph(graph);
                    if (status == VX_SUCCESS)
                    {
                        status = vxProcessGraph(graph);
                    }
                }
                for (i = 0; i < dimof(nodes); i++)
                {
                    vxReleaseNode(&nodes[i]);
                }
                vxReleaseGraph(&graph);
            }
        }
        // status |= vxUnloadKernels(context, "openvx-debug");
        // status |= vxUnloadKernels(context, "openvx-tiling");
        for (i = 0; i < dimof(images); i++)
        {
            vxReleaseImage(&images[i]);
        }
        vxReleaseContext(&context);
    }
    retval =  PAPI_hl_region_end("computation");
    printf("%s::main() returns = %d\n", argv[0], status);
    return (int)status;
}

// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2017 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include "shufflechannel_arm.h"

#if __ARM_NEON
#include <arm_neon.h>
#endif // __ARM_NEON

namespace ncnn {

DEFINE_LAYER_CREATOR(ShuffleChannel_arm)

int ShuffleChannel_arm::forward(const Mat &bottom_blob, Mat &top_blob) const
{
    int c = bottom_blob.c;
    int w = bottom_blob.w;
    int h = bottom_blob.h;
    int size = w * h;
//    int cstep = bottom_blob.cstep;
    int chs_per_group = c / group;

    if (c != chs_per_group * group) {
//        cout << "Wrong group num";
//        error;
        // reject invalid group
        return -100;
    }
    top_blob.create(w, h, c);
    if (top_blob.empty())
        return -100;

    int dst_q;
    int src_q;
#if __ARM_NEON
            int nn = size >> 2;
//            int remain = size & 3;
            int remain_bytes = (size & 3) * sizeof(float);
#else
//            int remain = cstep;
            int remain_bytes = size * sizeof(float);
#endif // __ARM_NEON

    #pragma omp parallel for
    for (int i = 0; i != group; ++i) {
        for (int j = 0; j != chs_per_group; ++j) {
            src_q = chs_per_group * i + j;
            dst_q = chs_per_group * j + i;
#if __ARM_NEON
#if __aarch64__
            float *src_p = bottom_blob.channel(src_q);
            float *dst_p = top_blob.channel(dst_q);
            for (int _nn = nn; _nn > 0; --_nn) {
                float32x4_t _p = vld1q_f32(src_p);
                vst1q_f32(dst_p, _p);

                dst_p += 4;
                src_p += 4;
            }
#else
            int _nn = nn;
            asm volatile(
                        "veor       q1, q0, q0          \n"
                        "0:                             \n"
                        "pld        [%1, #128]          \n"
                        "vld1.f32   {d0-d1}, [%1 :128]  \n"
                        "vmax.f32   q0, q0, q1          \n"
                        "subs       %0, #1              \n"
                        "vst1.f32   {d0-d1}, [%1 :128]! \n"
                        "bne        0b                  \n"
                        : "=r"(_nn),     // %0
                          "=r"(ptr)      // %1
                        : "0"(_nn),
                          "1"(ptr)
                        : "cc", "memory", "q0", "q1"
                        );
#endif // __aarch64__
            if (remain_bytes > 0)
                memcpy(top_blob.channel(dst_q), bottom_blob.channel(src_q),
                       remain_bytes);
#else // no neon
            memcpy(top_blob.channel(dst_q), bottom_blob.channel(src_q),
                   remain_bytes);
#endif // __ARM_NEON
        }
    }
    return 0;
}

} // namespace ncnn



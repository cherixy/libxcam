/*
 * cl_defog_dcp_handler.h - CL defog dark channel prior handler
 *
 *  Copyright (c) 2016 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Wind Yuan <feng.yuan@intel.com>
 */

#ifndef XCAM_CL_DEFOG_DCP_HANLDER_H
#define XCAM_CL_DEFOG_DCP_HANLDER_H

#include "xcam_utils.h"
#include "cl_image_handler.h"
#include "base/xcam_3a_result.h"
#include "x3a_stats_pool.h"

#define XCAM_DEFOG_DC_ORIGINAL     0
#define XCAM_DEFOG_DC_MIN_FILTER_V   1
#define XCAM_DEFOG_DC_MIN_FILTER_H   2
#define XCAM_DEFOG_DC_REFINED      3
#define XCAM_DEFOG_DC_MAX_BUF      4

#define XCAM_DEFOG_R_CHANNEL    0
#define XCAM_DEFOG_G_CHANNEL    1
#define XCAM_DEFOG_B_CHANNEL    2
#define XCAM_DEFOG_MAX_CHANNELS 3

namespace XCam {

class CLDefogDcpImageHandler;

class CLDarkChannelKernel
    : public CLImageKernel
{
public:
    explicit CLDarkChannelKernel (SmartPtr<CLContext> &context, SmartPtr<CLDefogDcpImageHandler> &defog_handler);

protected:
    virtual XCamReturn prepare_arguments (
        SmartPtr<DrmBoBuffer> &input, SmartPtr<DrmBoBuffer> &output,
        CLArgument args[], uint32_t &arg_count,
        CLWorkSize &work_size);

    virtual XCamReturn post_execute (SmartPtr<DrmBoBuffer> &output);

private:
    XCAM_DEAD_COPY (CLDarkChannelKernel);

private:
    SmartPtr<CLImage>                  _image_in_y;
    SmartPtr<CLImage>                  _image_in_uv;
    SmartPtr<CLDefogDcpImageHandler>   _defog_handler;
};

class CLMinFilterKernel
    : public CLImageKernel
{
public:
    explicit CLMinFilterKernel (
        SmartPtr<CLContext> &context, SmartPtr<CLDefogDcpImageHandler> &defog_handler, int index);

protected:
    virtual XCamReturn prepare_arguments (
        SmartPtr<DrmBoBuffer> &input, SmartPtr<DrmBoBuffer> &output,
        CLArgument args[], uint32_t &arg_count,
        CLWorkSize &work_size);

private:
    XCAM_DEAD_COPY (CLMinFilterKernel);

    SmartPtr<CLDefogDcpImageHandler>   _defog_handler;
    uint32_t                           _buf_index;
};

class CLDefogRecoverKernel
    : public CLImageKernel
{
public:
    explicit CLDefogRecoverKernel (
        SmartPtr<CLContext> &context, SmartPtr<CLDefogDcpImageHandler> &defog_handler);

protected:
    virtual XCamReturn prepare_arguments (
        SmartPtr<DrmBoBuffer> &input, SmartPtr<DrmBoBuffer> &output,
        CLArgument args[], uint32_t &arg_count,
        CLWorkSize &work_size);
    virtual XCamReturn post_execute (SmartPtr<DrmBoBuffer> &output);

private:
    float get_max_value (SmartPtr<DrmBoBuffer> &buf);

    XCAM_DEAD_COPY (CLDefogRecoverKernel);

private:
    SmartPtr<CLDefogDcpImageHandler>   _defog_handler;
    SmartPtr<CLImage>                  _image_out_y;
    SmartPtr<CLImage>                  _image_out_uv;
    float                              _max_r;
    float                              _max_g;
    float                              _max_b;
    float                              _max_i;
};

class CLDefogDcpImageHandler
    : public CLImageHandler
{
public:
    explicit CLDefogDcpImageHandler (const char *name);

    SmartPtr<CLImage> &get_dark_map (uint index) {
        XCAM_ASSERT (index < XCAM_DEFOG_DC_MAX_BUF);
        return _dark_channel_buf[index];
    };
    SmartPtr<CLImage> &get_rgb_channel (uint index) {
        XCAM_ASSERT (index < XCAM_DEFOG_MAX_CHANNELS);
        return _rgb_buf[index];
    };

    void pre_stop ();

protected:
    virtual XCamReturn prepare_output_buf (SmartPtr<DrmBoBuffer> &input, SmartPtr<DrmBoBuffer> &output);
    XCamReturn allocate_transmit_bufs (const VideoBufferInfo &video_info);

private:
    XCAM_DEAD_COPY (CLDefogDcpImageHandler);

private:
    SmartPtr<CLImage>                 _dark_channel_buf[XCAM_DEFOG_DC_MAX_BUF];
    SmartPtr<CLImage>                 _rgb_buf[XCAM_DEFOG_MAX_CHANNELS];
};

SmartPtr<CLImageHandler>
create_cl_defog_dcp_image_handler (SmartPtr<CLContext> &context);

};

#endif //XCAM_CL_DEFOG_DCP_HANLDER_H

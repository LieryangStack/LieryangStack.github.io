/*
 * Copyright (c) 2022-2023, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "deepstream_common.h"
#include "deepstream_config_yaml.h"
#include <string>
#include <cstring>
#include <iostream>

using std::cout;
using std::endl;

gboolean
parse_tracker_yaml (NvDsTrackerConfig *config, gchar *cfg_file_path)
{
  gboolean ret = FALSE;
  YAML::Node configyml = YAML::LoadFile(cfg_file_path);

  config->display_tracking_id = TRUE;
  config->tracking_id_reset_mode = 0;
  config->input_tensor_meta = FALSE;
  config->input_tensor_gie_id = 0;
  config->compute_hw = 0;
  config->user_meta_pool_size = 16;
  config->sub_batches = {};

  for(YAML::const_iterator itr = configyml["tracker"].begin();
     itr != configyml["tracker"].end(); ++itr)
  {
    std::string paramKey = itr->first.as<std::string>();
    if (paramKey == "enable") {
      config->enable = itr->second.as<gboolean>();
    } else if (paramKey == "tracker-width") {
      config->width = itr->second.as<gint>();
    } else if (paramKey == "tracker-height") {
      config->height = itr->second.as<gint>();
    } else if (paramKey == "gpu-id") {
      config->gpu_id = itr->second.as<guint>();
    } else if (paramKey == "tracker-surface-type") {
      config->tracking_surf_type = itr->second.as<guint>();
    } else if (paramKey == "ll-config-file") {
      std::string llConfigString = itr->second.as<std::string>();
      std::stringstream ss(llConfigString);
      std::string temp;
      std::stringstream ssFinal;
      char* str = (char*) malloc(sizeof(char) * 1024);
      char* str_out = (char*) malloc(sizeof(char) * 1024);
      while(std::getline(ss, temp, ';')){
        std::strncpy (str, temp.c_str(), 1023);
        if (!get_absolute_file_path_yaml (cfg_file_path, str, str_out)){
          g_printerr ("Error: Could not parse ll-config-file in tracker.\n");
          g_free (str); g_free (str_out);
          goto done;
        }
        ssFinal << str_out << ";";
      }
      config->ll_config_file = g_strdup(ssFinal.str().c_str());
      g_free (str); g_free (str_out);
    } else if (paramKey == "ll-lib-file") {
      std::string temp = itr->second.as<std::string>();
      char* str = (char*) malloc(sizeof(char) * 1024);
      std::strncpy (str, temp.c_str(), 1023);
      config->ll_lib_file = (char*) malloc(sizeof(char) * 1024);
      if (!get_absolute_file_path_yaml (cfg_file_path, str,
              config->ll_lib_file)) {
        g_printerr ("Error: Could not parse ll-lib-file in tracker.\n");
        g_free (str);
        goto done;
      }
      g_free (str);
    } else if (paramKey == "tracking-surface-type") {
      //Diff b/w this and tracking_surf_type
      config->tracking_surface_type = itr->second.as<guint>();
    } else if(paramKey == "display-tracking-id"){
      config->display_tracking_id = itr->second.as<gboolean>();
    } else if(paramKey == "tracking-id-reset-mode"){
      config->tracking_id_reset_mode = itr->second.as<guint>();
    } else if(paramKey == "input-tensor-meta"){
      config->input_tensor_meta = itr->second.as<gboolean>();
    } else if(paramKey == "tensor-meta-gie-id"){
      config->input_tensor_gie_id =  itr->second.as<guint>();
    } else if(paramKey == "compute-hw"){
      config->compute_hw =  itr->second.as<guint>();
    } else if(paramKey == "user-meta-pool-size"){
      config->user_meta_pool_size =  itr->second.as<guint>();
    } else if(paramKey == "sub-batches"){
      config->sub_batches =  itr->second.as<std::string>().c_str();
    } else {
      cout << "Unknown key " << paramKey << " for tracker" << endl;
    }
  }

  ret = TRUE;
done:
  if (!ret) {
    cout <<  __func__ << " failed" << endl;
  }
  return ret;
}

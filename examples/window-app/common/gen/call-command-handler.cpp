/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

// THIS FILE IS GENERATED BY ZAP

#include <stdint.h>

#include "af-structs.h"
#include "app/util/util.h"
#include "call-command-handler.h"
#include "callback.h"
#include "cluster-id.h"
#include "command-id.h"

using namespace chip;

EmberAfStatus emberAfWindowCoveringClusterServerCommandParse(EmberAfClusterCommand * cmd);

static EmberAfStatus status(bool wasHandled, bool clusterExists, bool mfgSpecific)
{
    if (wasHandled)
    {
        return EMBER_ZCL_STATUS_SUCCESS;
    }
    else if (mfgSpecific)
    {
        return EMBER_ZCL_STATUS_UNSUP_MANUF_CLUSTER_COMMAND;
    }
    else if (clusterExists)
    {
        return EMBER_ZCL_STATUS_UNSUP_COMMAND;
    }
    else
    {
        return EMBER_ZCL_STATUS_UNSUPPORTED_CLUSTER;
    }
}

// Main command parsing controller.
EmberAfStatus emberAfClusterSpecificCommandParse(EmberAfClusterCommand * cmd)
{
    EmberAfStatus result = status(false, false, cmd->mfgSpecific);
    if (cmd->direction == (uint8_t) ZCL_DIRECTION_SERVER_TO_CLIENT &&
        emberAfContainsClientWithMfgCode(cmd->apsFrame->destinationEndpoint, cmd->apsFrame->clusterId, cmd->mfgCode))
    {
        switch (cmd->apsFrame->clusterId)
        {
        default:
            // Unrecognized cluster ID, error status will apply.
            break;
        }
    }
    else if (cmd->direction == (uint8_t) ZCL_DIRECTION_CLIENT_TO_SERVER &&
             emberAfContainsServerWithMfgCode(cmd->apsFrame->destinationEndpoint, cmd->apsFrame->clusterId, cmd->mfgCode))
    {
        switch (cmd->apsFrame->clusterId)
        {
        case ZCL_WINDOW_COVERING_CLUSTER_ID:
            result = emberAfWindowCoveringClusterServerCommandParse(cmd);
            break;
        default:
            // Unrecognized cluster ID, error status will apply.
            break;
        }
    }
    return result;
}

// Cluster specific command parsing

EmberAfStatus emberAfWindowCoveringClusterServerCommandParse(EmberAfClusterCommand * cmd)
{
    bool wasHandled = false;

    if (!cmd->mfgSpecific)
    {
        switch (cmd->commandId)
        {
        case ZCL_WINDOW_COVERING_DOWN_CLOSE_COMMAND_ID: {
            wasHandled = emberAfWindowCoveringClusterWindowCoveringDownCloseCallback(nullptr);
            break;
        }
        case ZCL_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE_COMMAND_ID: {
            uint16_t payloadOffset = cmd->payloadStartIndex;
            uint8_t percentageLiftValue;

            if (cmd->bufLen < payloadOffset + 1)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            percentageLiftValue = emberAfGetInt8u(cmd->buffer, payloadOffset, cmd->bufLen);

            wasHandled = emberAfWindowCoveringClusterWindowCoveringGoToLiftPercentageCallback(nullptr, percentageLiftValue);
            break;
        }
        case ZCL_WINDOW_COVERING_GO_TO_LIFT_VALUE_COMMAND_ID: {
            uint16_t payloadOffset = cmd->payloadStartIndex;
            uint16_t liftValue;

            if (cmd->bufLen < payloadOffset + 2)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            liftValue = emberAfGetInt16u(cmd->buffer, payloadOffset, cmd->bufLen);

            wasHandled = emberAfWindowCoveringClusterWindowCoveringGoToLiftValueCallback(nullptr, liftValue);
            break;
        }
        case ZCL_WINDOW_COVERING_GO_TO_TILT_PERCENTAGE_COMMAND_ID: {
            uint16_t payloadOffset = cmd->payloadStartIndex;
            uint8_t percentageTiltValue;

            if (cmd->bufLen < payloadOffset + 1)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            percentageTiltValue = emberAfGetInt8u(cmd->buffer, payloadOffset, cmd->bufLen);

            wasHandled = emberAfWindowCoveringClusterWindowCoveringGoToTiltPercentageCallback(nullptr, percentageTiltValue);
            break;
        }
        case ZCL_WINDOW_COVERING_GO_TO_TILT_VALUE_COMMAND_ID: {
            uint16_t payloadOffset = cmd->payloadStartIndex;
            uint16_t tiltValue;

            if (cmd->bufLen < payloadOffset + 2)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            tiltValue = emberAfGetInt16u(cmd->buffer, payloadOffset, cmd->bufLen);

            wasHandled = emberAfWindowCoveringClusterWindowCoveringGoToTiltValueCallback(nullptr, tiltValue);
            break;
        }
        case ZCL_WINDOW_COVERING_STOP_COMMAND_ID: {
            wasHandled = emberAfWindowCoveringClusterWindowCoveringStopCallback(nullptr);
            break;
        }
        case ZCL_WINDOW_COVERING_UP_OPEN_COMMAND_ID: {
            wasHandled = emberAfWindowCoveringClusterWindowCoveringUpOpenCallback(nullptr);
            break;
        }
        default: {
            // Unrecognized command ID, error status will apply.
            break;
        }
        }
    }
    return status(wasHandled, true, cmd->mfgSpecific);
}

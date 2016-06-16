/* Copyright 2016 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VR_GVR_CAPI_INCLUDE_GVR_TYPES_H_
#define VR_GVR_CAPI_INCLUDE_GVR_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Primary context for invoking Google VR APIs.
typedef struct gvr_context_ gvr_context;

typedef enum {
  // If this enabled, a separate distortion function for each color channel
  // is used.
  // Disabled by default.
  GVR_CHROMATIC_ABERRATION_CORRECTION_ENABLED = 0,

  // If this is enabled, frames will be collected by the rendering system and
  // re-projected in sync with the scanout of the display. This feature may
  // not be available on every platform, and requires a high-priority render
  // thread with special extensions to function properly.
  //
  // Note: Unless the application is implementing its own scanline-racing
  // thread, this feature should be enabled using the enableScanlineRacing()
  // method in GvrLayout. This parameter can still be used to check
  // whether scanline racing has been enabled.
  //
  // Disabled by default.
  GVR_SCANLINE_RACING_ENABLED = 1,

  // The number of parameters.
  GVR_NUM_BOOL_PARAMETERS = 2,
} gvr_bool_parameter;

// An enum for the left and right eye.
typedef enum {
  GVR_LEFT_EYE = 0,
  GVR_RIGHT_EYE,
  GVR_NUM_EYES
} gvr_eye;

// An integral 2D size. Used for render target and framebuffer sizes.
typedef struct gvr_sizei_ {
  int32_t width;
  int32_t height;
} gvr_sizei;

// An integral 2D rect. Used for window bounds in pixels.
typedef struct gvr_recti_ {
  int32_t left;
  int32_t right;
  int32_t bottom;
  int32_t top;
} gvr_recti;

// A floating point 2D rect. Used for field of view, and also for ranges
// in texture space.
typedef struct gvr_rectf_ {
  float left;
  float right;
  float bottom;
  float top;
} gvr_rectf;

// A floating point 2D vector.
typedef struct gvr_vec2f_ {
  float x;
  float y;
} gvr_vec2f;

// A floating point 3D vector.
typedef struct gvr_vec3f_ {
  float x;
  float y;
  float z;
} gvr_vec3f;

// A floating point 3x3 matrix.
typedef struct gvr_mat3f_ { float m[3][3]; } gvr_mat3f;

// A floating point 4x4 matrix.
typedef struct gvr_mat4f_ { float m[4][4]; } gvr_mat4f;

// A floating point quaternion, in JPL format.
// We use this simple struct in order not to impose a dependency on a
// particular math library. The user of this API is free to encapsulate this
// into any math library they want.
typedef struct gvr_quatf_ {
  // qx, qy, qz are the vector component.
  float qx;
  float qy;
  float qz;
  // qw is the scalar component.
  float qw;
} gvr_quatf;

// A *monotonic system time* representation. On Android, this is equivalent to
// System.nanoTime(), or clock_gettime(CLOCK_MONOTONIC). If there is any doubt
// about how to get the current time for the current platform, simply use
// gvr_get_time_point_now().
typedef struct gvr_clock_time_point_ {
  int64_t monotonic_system_time_nanos;
} gvr_clock_time_point;

// A representation of the head pose.
typedef struct gvr_head_pose_ {
  // The head's rotation.
  gvr_mat3f rotation;
  // The head's position.
  gvr_vec3f position;
  // The transform from the reference space (that the rotation and position of
  // the pose are relative to) to the object space.
  gvr_mat4f object_from_reference_matrix;
} gvr_head_pose;

// Encapsulated information about where to find the data in the client's target
// for a given region (e.g., the region for a particular eye).  Includes the
// the viewport bounds for that region, as well as the field of view, and the
// type of eye that has been rendered.  This latter is used to calculate what
// kind of distortion to apply when the region is distorted to the screen.
//
// A set of these parameters will most often be generated by the API, via
// gvr_get_recommended_render_params_list() or
// gvr_get_screen_render_params_list(). However, the client may also customize
// these values via gvr_set_render_params(), constructing a custom
// gvr_render_params_list for use in the distortion pass.
typedef struct gvr_render_params_ {
  // The viewport bounds on the render target in target coordinates.
  gvr_rectf eye_viewport_bounds;

  // The eye's field of view in degrees.
  gvr_rectf eye_fov;

  // The eye type for the render target. Some calculations are made differently
  // depending the eye type, e.g., viewport, distortion and transformation.
  gvr_eye eye_type;
} gvr_render_params;

// Opaque handle to a list of gvr_render_params.
typedef struct gvr_render_params_list_ gvr_render_params_list;

// Controller initialization options.
typedef struct gvr_controller_api_options_ {
  // Indicates whether to report orientation events.
  bool enable_orientation;
  // Indicates whether to report touchpad events.
  bool enable_touch;
  // Indicates whether to report gyro events.
  bool enable_gyro;
  // Indicates whether to report accelerometer events.
  bool enable_accel;
  // Indicates whether to report gesture events.
  bool enable_gestures;
} gvr_controller_api_options;

// Constants that represent the status of the controller API.
typedef enum {
  // API is happy and healthy. This doesn't mean the controller itself
  // is connected, it just means that the underlying service is working
  // properly.
  GVR_CONTROLLER_API_OK = 0,

  // Any other status represents a permanent failure that requires
  // external action to fix:

  // API failed because this device does not support controllers (API is too
  // low, or other required feature not present).
  GVR_CONTROLLER_API_UNSUPPORTED = 1,
  // This app was not authorized to use the service (e.g., missing permissions,
  // the app is blacklisted by the underlying service, etc).
  GVR_CONTROLLER_API_NOT_AUTHORIZED = 2,
  // The underlying VR service is not present.
  GVR_CONTROLLER_API_UNAVAILABLE = 3,
  // The underlying VR service is too old, needs upgrade.
  GVR_CONTROLLER_API_SERVICE_OBSOLETE = 4,
  // The underlying VR service is too new, is incompatible with current client.
  GVR_CONTROLLER_API_CLIENT_OBSOLETE = 5,
  // The underlying VR service is malfunctioning. Try again later.
  GVR_CONTROLLER_API_MALFUNCTION = 6,
} gvr_controller_api_status;

// Constants that represent the state of the controller.
typedef enum {
  // Controller is disconnected.
  GVR_CONTROLLER_DISCONNECTED = 0,
  // Controller is scanning.
  GVR_CONTROLLER_SCANNING = 1,
  // Controller is connecting.
  GVR_CONTROLLER_CONNECTING = 2,
  // Controller is connected.
  GVR_CONTROLLER_CONNECTED = 3,
} gvr_controller_connection_state;

// Controller buttons.
// TODO(b/28973309): replace with bitmasks, and replace the button state arrays
// in gvr_controller_state with integers.
typedef enum {
  GVR_CONTROLLER_BUTTON_NONE = 0,
  GVR_CONTROLLER_BUTTON_CLICK = 1,  // Touchpad Click.
  GVR_CONTROLLER_BUTTON_HOME = 2,
  GVR_CONTROLLER_BUTTON_APP = 3,
  GVR_CONTROLLER_BUTTON_VOLUME_UP = 4,
  GVR_CONTROLLER_BUTTON_VOLUME_DOWN = 5,

  // Note: there are 5 buttons on the controller, but the state arrays have
  // this many elements due to the inclusion of a dummy "none" button.
  GVR_CONTROLLER_BUTTON_COUNT = 6,
} gvr_controller_button;

// Representation of the controller state in a given moment.
//
// Some fields are "transient". This means they indicate events that
// occurred and they will be true for only one frame.
typedef struct gvr_controller_state_ {
  gvr_controller_api_status api_status;
  gvr_controller_connection_state connection_state;

  // Current controller orientation, in Start Space. The Start Space is the
  // same space as the headset space and has these three axes:
  //   * the positive X axis points to the right
  //   * the positive Y axis points upwards
  //   * the positive Z axis points forward
  // Where "right" and "forward" are set when the controller initializes,
  // and are redefined when a recentering operation is performed.
  //
  // Note that this is the Start Space for the *controller*, which initially
  // coincides with the Start Space for the headset, but they may diverge over
  // time due to controller/headset drift. A recentering operation will bring
  // the two spaces back into sync.
  gvr_quatf orientation;

  // Current controller gyro reading, in Start Space.
  gvr_vec3f gyro;

  // Current (latest) controller accelerometer reading, in Start Space.
  gvr_vec3f accel;

  // Is the user touching the touchpad?
  bool is_touching;

  // If the user is touching the touchpad, this is the touch position in
  // normalized coordinates, where (0,0) is the top-left of the touchpad
  // and (1,1) is the bottom right. If the user is not touching the touchpad,
  // then this is the position of the last touch.
  gvr_vec2f touch_pos;

  // True if user just started touching touchpad (this is a transient event:
  // it is true for only one frame after the event).
  bool touch_down;

  // True if user just stopped touching touchpad (this is a transient event:
  // it is true for only one frame after the event).
  bool touch_up;

  // True if a recenter operation just ended (this is a transient event:
  // it is true only for one frame after the recenter ended). If this is
  // true then the |orientation| field is already relative to the new center.
  bool recentered;

  // This is true on every frame while the recenter flow is in progress.
  //
  // WARNING: Unless you wish to do something WHILE the recentering gesture
  // is in progress, use |recentered| instead. This field may be removed from
  // the API if the recentering flow is moved to VrCore, in which case the app
  // will have no way of knowing that recentering is in progress (since the app
  // will be paused anyway).
  bool recentering;

  // These arrays are related to each of the controller's buttons.
  // They are indexed by button number. Valid indices are from 0
  // to GVR_CONTROLLER_BUTTON_COUNT - 1.

  // whether each button is pressed right now.
  bool button_state[GVR_CONTROLLER_BUTTON_COUNT];
  // whether each button was just pressed (transient).
  bool button_down[GVR_CONTROLLER_BUTTON_COUNT];
  // whether each button was just released (transient).
  bool button_up[GVR_CONTROLLER_BUTTON_COUNT];

  // timestamp (nanos) when the last orientation event was received.
  int64_t last_orientation_timestamp;
  // timestamp (nanos) when the last gyro event was received.
  int64_t last_gyro_timestamp;
  // timestamp (nanos) when the last accelerometer event was received.
  int64_t last_accel_timestamp;
  // timestamp (nanos) when the last touch event was received.
  int64_t last_touch_timestamp;
  // timestamp (nanos) when the last button event was received.
  int64_t last_button_timestamp;

  // TODO(btco): add event queues with historical events.
  // TODO(btco): add gesture detection.
} gvr_controller_state;

// Rendering modes define CPU load / rendering quality balances.
typedef enum {
  // Stereo panning of all Sound Objects. This disables HRTF-based rendering.
  GVR_AUDIO_RENDERING_STEREO_PANNING = 0,
  // HRTF-based rendering over a virtual array of 8 loudspeakers arranged in
  // a cube configuration around the listener’s head.
  GVR_AUDIO_RENDERING_BINAURAL_LOW_QUALITY = 1,
  // HRTF-based rendering over a virtual array of 16 loudspeakers arranged in
  // an approximate equidistribution about the around the listener’s head.
  GVR_AUDIO_RENDERING_BINAURAL_HIGH_QUALITY = 2,
} gvr_audio_rendering_mode;

// Room surface material names, used to set room properties.
typedef enum {
  // Acoustically transparent material, reflects no sound.
  GVR_AUDIO_MATERIAL_TRANSPARENT = 0,
  // Acoustic ceiling tiles, absorbs most frequencies.
  GVR_AUDIO_MATERIAL_ACOUSTIC_CEILING_TILES = 1,
  // Bare brick, relatively reflective.
  GVR_AUDIO_MATERIAL_BRICK_BARE = 2,
  // Painted brick
  GVR_AUDIO_MATERIAL_BRICK_PAINTED = 3,
  // Coarse surface concrete block.
  GVR_AUDIO_MATERIAL_CONCRETE_BLOCK_COARSE = 4,
  // Painted concrete block.
  GVR_AUDIO_MATERIAL_CONCRETE_BLOCK_PAINTED = 5,
  // Heavy curtains.
  GVR_AUDIO_MATERIAL_CURTAIN_HEAVY = 6,
  // Fiber glass insulation.
  GVR_AUDIO_MATERIAL_FIBER_GLASS_INSULATION = 7,
  // Thin glass.
  GVR_AUDIO_MATERIAL_GLASS_THIN = 8,
  // Thick glass.
  GVR_AUDIO_MATERIAL_GLASS_THICK = 9,
  // Grass.
  GVR_AUDIO_MATERIAL_GRASS = 10,
  // Linoleum on concrete.
  GVR_AUDIO_MATERIAL_LINOLEUM_ON_CONCRETE = 11,
  // Marble.
  GVR_AUDIO_MATERIAL_MARBLE = 12,
  // Wooden parquet on concrete.
  GVR_AUDIO_MATERIAL_PARQUET_ON_CONCRETE = 13,
  // Rough plaster surface.
  GVR_AUDIO_MATERIAL_PLASTER_ROUGH = 14,
  // Smooth plaster surface.
  GVR_AUDIO_MATERIAL_PLASTER_SMOOTH = 15,
  // Plywood panel.
  GVR_AUDIO_MATERIAL_PLYWOOD_PANEL = 16,
  // Polished concrete OR tile surface.
  GVR_AUDIO_MATERIAL_POLISHED_CONCRETE_OR_TILE = 17,
  // Sheet rock.
  GVR_AUDIO_MATERIAL_SHEET_ROCK = 18,
  // Surface of water or ice.
  GVR_AUDIO_MATERIAL_WATER_OR_ICE_SURFACE = 19,
  // Wooden ceiling.
  GVR_AUDIO_MATERIAL_WOOD_CEILING = 20,
  // Wood panneling.
  GVR_AUDIO_MATERIAL_WOOD_PANEL = 21,
} gvr_audio_material_type;

// Sound object and sound field identifier.
typedef int gvr_audio_sound_id;

#ifdef __cplusplus
}  // extern "C"
#endif

#ifdef __cplusplus
// These typedefs convert the C-style names to C++-style names.

namespace gvr {

typedef gvr_bool_parameter BoolParameterId;
const BoolParameterId kChromaticAberrationCorrectionEnabled =
    static_cast<BoolParameterId>(GVR_CHROMATIC_ABERRATION_CORRECTION_ENABLED);
const BoolParameterId kScanlineRacingEnabled =
    static_cast<BoolParameterId>(GVR_SCANLINE_RACING_ENABLED);
const BoolParameterId kNumBoolParameters =
    static_cast<BoolParameterId>(GVR_NUM_BOOL_PARAMETERS);

typedef gvr_controller_api_status ControllerApiStatus;
const ControllerApiStatus kControllerApiOk =
    static_cast<ControllerApiStatus>(GVR_CONTROLLER_API_OK);
const ControllerApiStatus kControllerApiUnsupported =
    static_cast<ControllerApiStatus>(GVR_CONTROLLER_API_UNSUPPORTED);
const ControllerApiStatus kControllerApiNotAuthorized =
    static_cast<ControllerApiStatus>(GVR_CONTROLLER_API_NOT_AUTHORIZED);
const ControllerApiStatus kControllerApiUnavailable =
    static_cast<ControllerApiStatus>(GVR_CONTROLLER_API_UNAVAILABLE);
const ControllerApiStatus kControllerApiServiceObsolete =
    static_cast<ControllerApiStatus>(GVR_CONTROLLER_API_SERVICE_OBSOLETE);
const ControllerApiStatus kControllerApiClientObsolete =
    static_cast<ControllerApiStatus>(GVR_CONTROLLER_API_CLIENT_OBSOLETE);
const ControllerApiStatus kControllerApiMalfunction =
    static_cast<ControllerApiStatus>(GVR_CONTROLLER_API_MALFUNCTION);

typedef gvr_controller_connection_state ControllerConnectionState;
const ControllerConnectionState kControllerDisconnected =
    static_cast<ControllerConnectionState>(GVR_CONTROLLER_DISCONNECTED);
const ControllerConnectionState kControllerScanning =
    static_cast<ControllerConnectionState>(GVR_CONTROLLER_SCANNING);
const ControllerConnectionState kControllerConnecting =
    static_cast<ControllerConnectionState>(GVR_CONTROLLER_CONNECTING);
const ControllerConnectionState kControllerConnected =
    static_cast<ControllerConnectionState>(GVR_CONTROLLER_CONNECTED);

typedef gvr_controller_button ControllerButton;
const ControllerButton kControllerButtonNone =
    static_cast<ControllerButton>(GVR_CONTROLLER_BUTTON_NONE);
const ControllerButton kControllerButtonClick =
    static_cast<ControllerButton>(GVR_CONTROLLER_BUTTON_CLICK);
const ControllerButton kControllerButtonHome =
    static_cast<ControllerButton>(GVR_CONTROLLER_BUTTON_HOME);
const ControllerButton kControllerButtonApp =
    static_cast<ControllerButton>(GVR_CONTROLLER_BUTTON_APP);
const ControllerButton kControllerButtonVolumeUp =
    static_cast<ControllerButton>(GVR_CONTROLLER_BUTTON_VOLUME_UP);
const ControllerButton kControllerButtonVolumeDown =
    static_cast<ControllerButton>(GVR_CONTROLLER_BUTTON_VOLUME_DOWN);
const ControllerButton kControllerButtonCount =
    static_cast<ControllerButton>(GVR_CONTROLLER_BUTTON_COUNT);

typedef gvr_eye Eye;
typedef gvr_sizei Sizei;
typedef gvr_recti Recti;
typedef gvr_rectf Rectf;
typedef gvr_vec2f Vec2f;
typedef gvr_vec3f Vec3f;
typedef gvr_mat3f Mat3f;
typedef gvr_mat4f Mat4f;
typedef gvr_quatf Quatf;
typedef gvr_clock_time_point ClockTimePoint;
typedef gvr_head_pose HeadPose;
typedef gvr_render_params RenderParams;

typedef gvr_vec2f ControllerVec2;
typedef gvr_vec3f ControllerVec3;
typedef gvr_quatf ControllerQuat;
typedef gvr_controller_api_options ControllerApiOptions;
typedef gvr_controller_state ControllerState;

typedef gvr_audio_rendering_mode AudioRenderingMode;
typedef gvr_audio_material_type AudioMaterialName;
typedef gvr_audio_sound_id AudioSoundId;

class AudioApi;
class ControllerApi;
class GvrApi;
class OffscreenFramebufferHandle;
class RenderParamsList;

}  // namespace gvr

// Non-member equality operators for convenience.
inline bool operator==(const gvr::Vec2f& lhs, const gvr::Vec2f& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const gvr::Vec2f& lhs, const gvr::Vec2f& rhs) {
  return !(lhs == rhs);
}

inline bool operator==(const gvr::Vec3f& lhs, const gvr::Vec3f& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const gvr::Vec3f& lhs, const gvr::Vec3f& rhs) {
  return !(lhs == rhs);
}

inline bool operator==(const gvr::Recti& lhs, const gvr::Recti& rhs) {
  return lhs.left == rhs.left && lhs.right == rhs.right &&
         lhs.bottom == rhs.bottom && lhs.top == rhs.top;
}

inline bool operator!=(const gvr::Recti& lhs, const gvr::Recti& rhs) {
  return !(lhs == rhs);
}

inline bool operator==(const gvr::Rectf& lhs, const gvr::Rectf& rhs) {
  return lhs.left == rhs.left && lhs.right == rhs.right &&
         lhs.bottom == rhs.bottom && lhs.top == rhs.top;
}

inline bool operator!=(const gvr::Rectf& lhs, const gvr::Rectf& rhs) {
  return !(lhs == rhs);
}

inline bool operator==(const gvr::Sizei& lhs, const gvr::Sizei& rhs) {
  return lhs.width == rhs.width && lhs.height == rhs.height;
}

inline bool operator!=(const gvr::Sizei& lhs, const gvr::Sizei& rhs) {
  return !(lhs == rhs);
}

#endif  // #ifdef __cplusplus

#endif  // VR_GVR_CAPI_INCLUDE_GVR_TYPES_H_

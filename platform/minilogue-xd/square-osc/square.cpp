#include "userosc.h"

typedef struct Square {
  enum {
    flags_clear = 0,
    flags_noteon = 1 << 0,
    flags_noteoff = 1 << 1,
  };
  float phi0;
  float w0;
  uint32_t flags:8;
   
} Square;

static Square gSquare;



void OSC_INIT(uint32_t platform, uint32_t api) {
  gSquare.phi0 = 0.f;
  gSquare.flags = Square::flags_clear;
  gSquare.w0 = 0.f;

}

void OSC_CYCLE(const user_osc_param_t* params,
               int32_t* yn,
               const uint32_t frames) { 

  // Handle the reset flag on NOTEON.
  const uint8_t flags = gSquare.flags;
  gSquare.flags = Square::flags_clear;
  
  const float w0 = gSquare.w0 = osc_w0f_for_note((params->pitch)>>8, params->pitch & 0xFF);
  float phase = (flags & Square::flags_noteon) ? 0.f : gSquare.phi0;

  // Prepare the result buffer.
  q31_t* __restrict y = (q31_t*) yn;
  const q31_t* y_e = y + frames;

  while (y != y_e) {
    // Main signal
    float sig = osc_sqrf(phase);
    sig  = osc_softclipf(0.05f, sig);
    *(y++) = f32_to_q31(sig);

    // Next step.
    phase += w0;
    phase -= (uint32_t) phase; // to keep phase within 0.0-1.0.
  }

  // Store the state.
  gSquare.phi0 = phase;
}

void OSC_NOTEON(const user_osc_param_t * const params) {
  gSquare.flags |= Square::flags_noteon;
}

void OSC_NOTEOFF(const user_osc_param_t * const params) {
  gSquare.flags |= Square::flags_noteoff;
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
  case k_user_osc_param_id1:
  case k_user_osc_param_id2:
  case k_user_osc_param_id3:
  case k_user_osc_param_id4:
  case k_user_osc_param_id5:
  case k_user_osc_param_id6:
    break;
  case k_user_osc_param_shape:
  case k_user_osc_param_shiftshape:
  default:
    break;
  }
}
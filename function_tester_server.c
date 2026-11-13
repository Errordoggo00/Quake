#include <string.h>

#include "QW/server/qwsvdef.h"

double host_frametime;

int ClipVelocity(vec3_t input, vec3_t normal, vec3_t output, float overbounce);

int QW_TestServerClipVelocity(float input[3], float normal[3],
	float output[3], float overbounce)
{
	return ClipVelocity(input, normal, output, overbounce);
}

void QW_TestServerAddGravity(float velocity[3], float gravity,
	float frame_time, float scale)
{
	edict_t ent;

	memset(&ent, 0, sizeof(ent));
	memcpy(ent.v.velocity, velocity, sizeof(ent.v.velocity));
	movevars.gravity = gravity;
	host_frametime = frame_time;
	SV_AddGravity(&ent, scale);
	memcpy(velocity, ent.v.velocity, sizeof(ent.v.velocity));
}

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "QW/client/quakedef.h"

vec3_t vec3_origin = {0, 0, 0};

int PM_ClipVelocity(vec3_t input, vec3_t normal, vec3_t output,
	float overbounce);
void PM_Accelerate(vec3_t wishdir, float wishspeed, float accel);
void PM_AirAccelerate(vec3_t wishdir, float wishspeed, float accel);
void PM_Friction(void);
extern float frametime;

int QW_TestServerClipVelocity(float input[3], float normal[3],
	float output[3], float overbounce);
void QW_TestServerAddGravity(float velocity[3], float gravity,
	float frame_time, float scale);

void Sys_Error(char *format, ...)
{
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputc('\n', stderr);
	abort();
}

void Con_DPrintf(char *format, ...)
{
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

static int read_float(const char *prompt, float *value)
{
	printf("%s", prompt);
	return scanf("%f", value) == 1;
}

static int read_int(const char *prompt, int *value)
{
	printf("%s", prompt);
	return scanf("%d", value) == 1;
}

static int read_vec3(const char *prompt, vec3_t value)
{
	printf("%s (x y z): ", prompt);
	return scanf("%f %f %f", &value[0], &value[1], &value[2]) == 3;
}

static void print_vec3(const char *label, vec3_t value)
{
	printf("%s: (%.3f, %.3f, %.3f)\n", label,
		value[0], value[1], value[2]);
}

int main(void)
{
	int choice, blocked;
	float overbounce, wishspeed;
	vec3_t input, normal, output, direction;

	printf("Choose a function to test:\n");
	printf("1. PM_ClipVelocity\n");
	printf("2. PM_Accelerate\n");
	printf("3. PM_AirAccelerate\n");
	printf("4. PM_Friction\n");
	printf("5. PM_PlayerMove collision trace\n");
	printf("6. Server ClipVelocity\n");
	printf("7. Server gravity\n");
	printf("Selection: ");
	if (!read_int("", &choice))
		goto invalid_input;

	if (choice == 1)
	{
		if (!read_vec3("Input velocity", input)
			|| !read_vec3("Collision normal", normal)
			|| !read_float("Overbounce: ", &overbounce))
			goto invalid_input;

		blocked = PM_ClipVelocity(input, normal, output, overbounce);
		print_vec3("Output velocity", output);
		printf("Blocked flags: %d (1=floor, 2=wall/step)\n", blocked);
	}
	else if (choice == 2 || choice == 3)
	{
		int dead;

		if (!read_vec3("Initial velocity", pmove.velocity)
			|| !read_vec3("Wish direction", direction)
			|| !read_float("Wish speed: ", &wishspeed)
			|| !read_float("Acceleration: ", &movevars.accelerate)
			|| !read_float("Frame time in seconds: ", &frametime)
			|| !read_float("Water-jump time (0 for none): ",
				&pmove.waterjumptime)
			|| !read_int("Dead (0=no, 1=yes): ", &dead))
			goto invalid_input;

		pmove.dead = dead != 0;
		if (choice == 2)
			PM_Accelerate(direction, wishspeed, movevars.accelerate);
		else
			PM_AirAccelerate(direction, wishspeed, movevars.accelerate);
		print_vec3("Resulting velocity", pmove.velocity);
	}
	else if (choice == 4)
	{
		if (!read_vec3("Initial velocity", pmove.velocity)
			|| !read_float("Frame time in seconds: ", &frametime)
			|| !read_float("Ground friction: ", &movevars.friction)
			|| !read_float("Water friction: ", &movevars.waterfriction)
			|| !read_float("Stop speed: ", &movevars.stopspeed)
			|| !read_int("Water level (0-3): ", &waterlevel)
			|| !read_int("On ground (0=no, 1=yes): ", &onground))
			goto invalid_input;

		pmove.waterjumptime = 0;
		onground = onground ? 0 : -1;
		PM_Friction();
		print_vec3("Resulting velocity", pmove.velocity);
	}
	else if (choice == 5)
	{
		pmtrace_t trace;
		physent_t *world = &pmove.physents[0];
		vec3_t start, end;

		if (!read_vec3("Obstacle origin", world->origin)
			|| !read_vec3("Obstacle minimum bounds", world->mins)
			|| !read_vec3("Obstacle maximum bounds", world->maxs)
			|| !read_vec3("Player start position", start)
			|| !read_vec3("Player end position", end))
			goto invalid_input;

		world->model = NULL;
		pmove.numphysent = 1;
		Pmove_Init();
		trace = PM_PlayerMove(start, end);
		printf("Trace fraction: %.3f\n", trace.fraction);
		print_vec3("Trace end position", trace.endpos);
		printf("Start solid: %s\n", trace.startsolid ? "yes" : "no");
		printf("All solid: %s\n", trace.allsolid ? "yes" : "no");
		printf("Hit entity index: %d\n", trace.ent);
	}
	else if (choice == 6)
	{
		if (!read_vec3("Input velocity", input)
			|| !read_vec3("Collision normal", normal)
			|| !read_float("Overbounce: ", &overbounce))
			goto invalid_input;

		blocked = QW_TestServerClipVelocity(input, normal, output, overbounce);
		print_vec3("Output velocity", output);
		printf("Blocked flags: %d (1=floor, 2=wall/step)\n", blocked);
	}
	else if (choice == 7)
	{
		float gravity, frame_time, scale;

		if (!read_vec3("Initial velocity", pmove.velocity)
			|| !read_float("Gravity: ", &gravity)
			|| !read_float("Frame time in seconds: ", &frame_time)
			|| !read_float("Gravity scale: ", &scale))
			goto invalid_input;

		QW_TestServerAddGravity(pmove.velocity, gravity, frame_time, scale);
		print_vec3("Resulting velocity", pmove.velocity);
	}
	else
	{
		printf("Invalid selection.\n");
		return 1;
	}

	return 0;

invalid_input:
	fprintf(stderr, "Invalid or missing input; enter numeric values.\n");
	return 1;
}

#include <stdio.h>

#include "QW/client/pmove.c"

static int read_float(const char *prompt, float *value)
{
	printf("%s", prompt);
	return scanf("%f", value) == 1;
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
	printf("Selection: ");
	if (scanf("%d", &choice) != 1)
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
		if (!read_vec3("Initial velocity", pmove.velocity)
			|| !read_vec3("Wish direction", direction)
			|| !read_float("Wish speed: ", &wishspeed)
			|| !read_float("Acceleration: ", &movevars.accelerate)
			|| !read_float("Frame time in seconds: ", &frametime)
			|| !read_float("Water-jump time (0 for none): ",
				&pmove.waterjumptime))
			goto invalid_input;

		pmove.dead = false;
		if (choice == 2)
			PM_Accelerate(direction, wishspeed, movevars.accelerate);
		else
			PM_AirAccelerate(direction, wishspeed, movevars.accelerate);
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

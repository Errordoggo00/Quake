#include <stdio.h>
#include "pmove.c"
#include "pmove.h"
#include "quakedef.h"
int main(void) {
    int num;
    float var1, var2, var3, var4, var5, var6, var7, var8, var9, var10;
    printf("Enter a number representing the function you want to call: ");
    scanf("%d", &num);
    printf("Enter the parameters for the function (if any):\n");
    scanf("%f %f %f %f %f %f %f %f %f %f", &var1, &var2, &var3, &var4, &var5, &var6, &var7, &var8, &var9, &var10);
    if (num == 1) {
        PM_ClipVelocity((vec3_t){var1, var2, var3}, (vec3_t){var4, var5, var6}, (vec3_t){var7, var8, var9}, var10);
    } else if (num == 2) {
        PM_Accelerate((vec3_t){var1, var2, var3}, var4, var5);
    } else if (num == 3) {
        PM_AirAccelerate((vec3_t){var1, var2, var3}, var4, var5);
    } else if (num == 4) {
        PM_FlyMove();
    } else if (num == 5) {
        PM_GroundMove();
    } else if (num == 6) {
        PM_CatagorizePosition();
    } else if (num == 7) {
        PM_PlayerMove((vec3_t){var1, var2, var3}, (vec3_t){var4, var5, var6});
    } else if (num == 8) {
        PM_WaterMove();
    } else if (num == 9) {
        PM_AirMove();
    } else if (num == 10) {
        PM_Friction();
    } else {
        printf("Invalid number entered.\n");
    }
}
/*
 * Copyright (c) 2015 Sergi Granell (xerpi)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>

#include <vita2d.h>
#include "includes/lcd3x_v.h"
#include "includes/lcd3x_f.h"
#include "includes/texture_v.h"
#include "includes/bicubic_f.h"


/*
 * Symbol of the image.png file
 */
extern unsigned char _binary_psp_png_start;
extern const SceGxmProgram texture_v_gxp_start;


int main()
{
	SceCtrlData pad;
	vita2d_pgf *pgf;
	vita2d_texture *image;
	float rad = 0.0f;

	vita2d_init();

	vita2d_shader* lcd3x_shader = vita2d_create_shader((SceGxmProgram*) lcd3x_v, (SceGxmProgram*) lcd3x_f);
	vita2d_shader* bicubic_shader = vita2d_create_shader((SceGxmProgram*) texture_v, (SceGxmProgram*) bicubic_f);


	
	vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

	pgf = vita2d_load_default_pgf();

	/*
	 * Load the statically compiled image.png file.
	 */
	image = vita2d_load_PNG_buffer(&_binary_psp_png_start);

	memset(&pad, 0, sizeof(pad));
	
	

	while (1) {
		sceCtrlPeekBufferPositive(0, &pad, 1);

		if (pad.buttons & SCE_CTRL_START){
			vita2d_texture_set_program(lcd3x_shader->vertexProgram, lcd3x_shader->fragmentProgram);
			vita2d_texture_set_wvp(lcd3x_shader->wvpParam);
			vita2d_texture_set_texSize(lcd3x_shader->texSizeParam);
		} else if(pad.buttons & SCE_CTRL_SQUARE){
			vita2d_texture_set_program(bicubic_shader->vertexProgram, bicubic_shader->fragmentProgram);
			vita2d_texture_set_wvp(bicubic_shader->wvpParam);
			vita2d_texture_set_texSize(bicubic_shader->texSizeParam);
		} else if(pad.buttons & SCE_CTRL_SELECT){
			break;
		}
		vita2d_start_drawing();
		vita2d_clear_screen();



		vita2d_draw_texture_scale(image, 0, 0,2.0f,2.0f);

		vita2d_end_drawing();
		vita2d_swap_buffers();

		rad += 0.1f;
	}

	/*
	 * vita2d_fini() waits until the GPU has finished rendering,
	 * then we can free the assets freely.
	 */
	vita2d_wait_rendering_done();
	vita2d_free_shader(bicubic_shader);
	vita2d_free_shader(lcd3x_shader);
	vita2d_fini();

	vita2d_free_texture(image);
	vita2d_free_pgf(pgf);

	sceKernelExitProcess(0);
	return 0;
}

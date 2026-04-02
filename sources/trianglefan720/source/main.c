#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

#include "liysys.h"
#include "globals.h"
#include "rendermgr.h"
#include "particle.h"
#include "init.h"
#include "graphicsutililiys.h"
#include "primitives.h"
#include "liyt.h"

int main(int argc, char **argv) {
	guVector cam = {0.0F, -4.0F, 2.0F},
		camrot = {1.0F, 0.0F, 0.0F};

	inits();
	//GX_SetCopyClear((GXColor){255, 255, 255, 255}, 0x00ffffff);
	GX_SetCopyClear((GXColor){0, 0, 0, 255}, 0x00ffffff);
	//GX_SetCopyClear((GXColor){64, 64, 64, 255}, 0x00ffffff);
	PAD_Init();
	resetViewport();
	VIDEO_SetBlack(false);
	GX_SetCullMode(GX_CULL_NONE);
	float camFov = 45;
	guPerspective(perspective, camFov, 4.0f/3.0f, 0.1F, 300.0F);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

	int circlehires = 16; 
	float * circlehiverts = LilyCoolMalloc(32000);
	setupCircle(circlehiverts, circlehires);
	//3 for GX_Begin(), 2 + point count for start nd end, 2 axes, 4 bytes per float
	int circlehiDispListSize = 3 + ((circlehires + 2) * 2 * 4);
	circlehiDispListSize += 32; //padding ..
	circlehiDispListSize += 32 - (circlehiDispListSize % 32);
	void * circlehiDispList = LilyCoolMalloc(circlehiDispListSize);//, 32);
	memset(circlehiDispList, 0, circlehiDispListSize);
	GX_BeginDispList(circlehiDispList, circlehiDispListSize);
	drawCircle(circlehiverts, circlehires);
	circlehiDispListSize = GX_EndDispList();

	GX_SetNumTexGens(0);
	GX_SetNumChans(1);
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);

	while(1) {
		PAD_ScanPads();
		int gbuttonsheld = PAD_ButtonsHeld(0);
		int gbuttonsdown = PAD_ButtonsDown(0);
		if(gbuttonsdown & PAD_BUTTON_START) exit(0);
		if(gbuttonsheld & PAD_BUTTON_LEFT) {cam.x -= 0.01f;}
		float movespeed = 0.04f;
		float looksens = 0.02f;
		float oneslashonetwentyeight = 1.0f / 128.0f;
		camrot.x += (float)PAD_SubStickY(0) * oneslashonetwentyeight * looksens * camFov * 0.02f;
		camrot.z -= (float)PAD_SubStickX(0) * oneslashonetwentyeight * looksens * camFov * 0.02f;
		//updown
		cam.y += movespeed * cos(-camrot.z) * oneslashonetwentyeight * (float)PAD_StickY(0);
		cam.x += movespeed * sin(-camrot.z) * oneslashonetwentyeight * (float)PAD_StickY(0);
		//lr
		cam.y += movespeed * sin(camrot.z) * oneslashonetwentyeight * (float)PAD_StickX(0);
		cam.x += movespeed * cos(camrot.z) * oneslashonetwentyeight * (float)PAD_StickX(0);

		liyt_genMtxPosRotZyx(view, cam.x, cam.y, cam.z, camrot.x, camrot.y, camrot.z);
		guMtxInverse(view, view);

		if((rframe % 5000) == 0) randomizeParticlePos();
		particleTick(16.67f, rframe / 1000); //jump up by 1 every 1000 frames

		for(int i = 0; i < 2; i++) {
		GX_SetViewport(i ? -640 : 0, 0, 720, 480, 0, 1);
		GX_SetScissor(0, 0, i?80:640, 480);
	
		GX_SetNumTevStages(1);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLORNULL);
		GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
		
		//GX_SetBlendMode(GX_BM_SUBTRACT, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP); //multiplacicative
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_INVSRCCLR, GX_LO_OR); //additive

		GX_SetTevColor(GX_TEVREG0, (GXColor){96, 96, 96, 255});
		guMtxIdentity(model);
		guMtxConcat(view, model, model);
		GX_LoadPosMtxImm(model, GX_PNMTX0);
		drawTestaxis();

		//GX_SetTevColor(GX_TEVREG0, (GXColor){255 - 176, 255 - 220, 255 - 254, 255 - 255});
		//GX_SetTevColor(GX_TEVREG0, (GXColor){176, 220, 254, 255});
		GX_SetTevColor(GX_TEVREG0, (GXColor){15, 20, 26, 255});
		guMtxTrans(model, 0.0f, 0.0f, 0.0f);
		c_guMtxConcatTransOnly(view, model, model, 1.0f);
		GX_LoadPosMtxImm(model, GX_PNMTX0);
		configCircle();
		//drawCircle(circlehiverts, circlehires);
		//GX_CallDispList(circlehiDispList, circlehiDispListSize);

		//Lots of circles
		GX_SetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
		//GX_SetTevColor(GX_TEVREG0, (GXColor){9, 11, 13, 255});
		configCircle();
		for(int i = 0; i < PARTICLE_COUNT; i++) {
			int i3 = i * 3;
			guMtxTrans(model, particlePos[i3], particlePos[i3+1], particlePos[i3+2]);
			c_guMtxConcatTransOnly(view, model, model, 0.25f);
			GX_LoadPosMtxImm(model, GX_PNMTX0);
			//drawCircle(circlehiverts, circlehires);
			GX_CallDispList(circlehiDispList, circlehiDispListSize);
			if((i % 100) == 0) {
				GX_DrawDone();
				GX_Flush();
			}
		}

		//Lots of particles
		/*GX_SetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
		GX_SetTevColor(GX_TEVREG0, (GXColor){8, 11, 13, 255});
		//GX_SetTevColor(GX_TEVREG0, (GXColor){176, 220, 254, 255});
		guMtxIdentity(model);
		guMtxConcat(view, model, model);
		GX_LoadPosMtxImm(model, GX_PNMTX0);
		GX_SetPointSize(255, 0);
		liy_VtxDescConfig(LV_VP, 0);
		//maximum 65535 verts per draw call
		int pointsleft = PARTICLE_COUNT;
		int pointssubd = 0;
		while(pointsleft > 0) {
			//int pointsthisround = liym_min(pointsleft, 65535);
			int pointsthisround = pointsleft > 65535 ? 65535 : pointsleft;
			GX_Begin(GX_POINTS, 0, pointsthisround);
			for(int i = pointssubd; i < pointssubd + pointsthisround; i++) {
				int i3 = i*3;
				GX_Position3f32(particlePos[i3], particlePos[i3+1], particlePos[i3+2]);
			}
			pointsleft -= pointsthisround;
			pointssubd += pointsthisround;
		}*/

		GX_DrawDone();
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_SetDispCopySrc(0, 0, i?80:640, 480);
		GX_SetDispCopyDst(720, 480);
		GX_CopyDisp(xfb + (i?1280:0), GX_TRUE);
		GX_Flush();
		} //multivp

		rframe++;
		VIDEO_WaitVSync();
		VIDEO_SetNextFramebuffer(xfb);
		VIDEO_Flush();
	}
}

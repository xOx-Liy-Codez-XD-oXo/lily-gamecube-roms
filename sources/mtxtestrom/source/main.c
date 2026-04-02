#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>

#define DEFAULT_FIFO_SIZE	(256*1024)

static void *frameBuffer = NULL;
GXRModeObj *rmode;

int main( int argc, char **argv ){
	f32 yscale;

	u32 xfbHeight;

	Mtx model;
	Mtx44 perspective;

	GXColor background = {0, 0, 0, 0xff};

	VIDEO_Init();
	rmode = VIDEO_GetPreferredMode(NULL);

	frameBuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer);
	VIDEO_SetBlack(false);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	void *gp_fifo = NULL;
	gp_fifo = memalign(32,DEFAULT_FIFO_SIZE);
	memset(gp_fifo,0,DEFAULT_FIFO_SIZE);

	GX_Init(gp_fifo,DEFAULT_FIFO_SIZE);
	GX_SetCopyClear(background, 0x00ffffff);

	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(frameBuffer,GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	GX_SetNumChans(1);
	GX_SetNumTexGens(0);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A1);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	guPerspective(perspective, 45, 1.333f, 0.1F, 300.0F);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

	// Relevant bit starts here

	guMtxTrans(model, -1.5f,0.0f,-6.0f);
	GX_LoadPosMtxImm(model, GX_PNMTX0);
	guMtxTrans(model, 1.5f,0.0f,-6.0f);
	GX_LoadPosMtxImm(model, GX_PNMTX1);

	while(1) {
		GX_SetCurrentMtx(GX_PNMTX0);

		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_PTNMTXIDX, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_PTNMTXIDX, GX_INDEX8, GX_U8, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);

		GX_Begin(GX_POINTS, GX_VTXFMT0, 1);
			GX_MatrixIndex1x8(3); //pnmtx1
			GX_Position3f32(0.0f, 0.0f, 0.0f);

		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);


		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
			GX_Position3f32( 0.0f, 1.0f, 0.0f);
			GX_Position3f32(-1.0f,-1.0f, 0.0f);
			GX_Position3f32( 1.0f,-1.0f, 0.0f);

		GX_DrawDone();

		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(frameBuffer,GX_TRUE);

		VIDEO_SetNextFramebuffer(frameBuffer);

		VIDEO_Flush();

		VIDEO_WaitVSync();


	}
	return 0;
}


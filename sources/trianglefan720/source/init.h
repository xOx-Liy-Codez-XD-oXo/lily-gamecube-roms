void quitplease() {
	shouldquit = 1;
}

void inits() {
	fullQuadMv[0][0] = 1.0f;
	fullQuadMv[1][0] = 0.0f;
	fullQuadMv[2][0] = 0.0f;

	fullQuadMv[0][1] = 0.0f;
	fullQuadMv[1][1] = 1.0f;
	fullQuadMv[2][1] = 0.0f;
	
	fullQuadMv[0][2] = 0.0f;
	fullQuadMv[1][2] = 0.0f;
	fullQuadMv[2][2] = 1.0f;

	fullQuadMv[0][3] = 0.0f;
	fullQuadMv[1][3] = 0.0f;
	fullQuadMv[2][3] = -1.0f;

	VIDEO_Init();
	//WPAD_Init();
	PAD_Init();

	//SYS_SetPowerCallback(quitplease);
	CON_EnableGecko(1, 0);

	#define DEFAULT_FIFO_SIZE 256*1024*4
	gpfifo = LilyCoolMalloc(DEFAULT_FIFO_SIZE);
	memset(gpfifo, 0, DEFAULT_FIFO_SIZE);

	rmode = VIDEO_GetPreferredMode(NULL);
	//do some determination to see if we can use our cool rmode
	rmode = &TVNtsc480Prog;
	rmode->fbWidth = 720;
	rmode->viWidth = 720;
	xfb = LilyCoolMalloc(VIDEO_GetFrameBufferSize(rmode));
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	printf("%d viTVMode\n%d fbWidth\n%d efbHeight\n%d xfbHeight\n%d viXOrigin\n%d viYOrigin\n%d viWidth\n%d viHeight\n%d xfbMode\n%d field_rendering\n%d aa\n",
	rmode->viTVMode, rmode->fbWidth, rmode->efbHeight, rmode->xfbHeight, rmode->viXOrigin, 
	rmode->viYOrigin, rmode->viWidth, rmode->viHeight, rmode->xfbMode, rmode->field_rendering, rmode->aa);

	GX_Init(gpfifo, DEFAULT_FIFO_SIZE);

	fbwidth = rmode->fbWidth;
	fbheight = rmode->efbHeight;

	resetViewport();
	GX_SetCopyFilter(GX_FALSE, NULL, GX_FALSE, NULL);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	GX_SetDispCopyGamma(GX_GM_1_0);
	GX_CopyDisp(xfb, GX_TRUE);

	initParticle();
	randomizeParticlePos();
}
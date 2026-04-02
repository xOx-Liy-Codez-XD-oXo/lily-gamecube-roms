//Inits h

void liy_initGlobalConsts() {
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
}

void blowpopsiclestand() {
	pleaseexit = 1;
}

void liy_initialSystemConfig() {
	//SYS_SetPowerCallback(blowpopsiclestand);

	//int dolphinfd = IOS_Open("/dev/dolphin", IPC_OPEN_NONE);

	int dolphinfd = -1;

	if(dolphinfd < 0) {
		CON_EnableGecko(1, 0);
		SYS_STDIO_Report(true);
	//	USB_Initialize();
	} else {
		SYS_STDIO_Report(true);
	//	IOS_Close(dolphinfd);
		dolphin = 1;
		#ifdef GUY_DEV
		printf("That's dolpfy\n");
		#endif
	}
}

void liy_initialFlipperConfig() {
	rmode = VIDEO_GetPreferredMode(NULL);
	//rmode = &TVPal528Prog;
	
	// allocate the fifo buffer
	gpfifo = memalign(32,DEFAULT_FIFO_SIZE);
	memset(gpfifo,0,DEFAULT_FIFO_SIZE);

	frameBuffer[0] = memalign(32, VIDEO_GetFrameBufferSize(rmode));
	frameBuffer[1] = memalign(32, VIDEO_GetFrameBufferSize(rmode));

		// configure video
	//if (CONF_GetAspectRatio() == CONF_ASPECT_16_9) {
	//	aspect = 16.0f/9.0f;
	//	widescreen = 1;
	//} else {
		aspect = 4.0f/3.0f;
	//}
	rmode->aa = 0; //no way aa

	rmode->viWidth = 704;
	rmode->viXOrigin = 6;

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	//fb ^= 1; uncomment to break rendering on progressive display

	// init the flipper
	GX_Init(gpfifo,DEFAULT_FIFO_SIZE);

	GX_SetCopyClear(LC_BLACK, 0x00ffffff);

	fbwidth = rmode->fbWidth;
	fbheight = rmode->efbHeight;
	if((fbheight != 240) && (fbheight != 480)) { european = 1; }

	// other gx setup
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	f32 yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	u32 xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);	
}

void liy_initialDrawConfig() {
	GX_SetCullMode(GX_CULL_FRONT);
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);
	GX_SetDispCopyGamma((f32)GX_GM_1_0);

	GX_SetNumTexGens(1);

	// setup texture coordinate generation
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);

	//VIDEO_SetBlack(true);
	VIDEO_SetBlack(false);

	//Oh leave me alone i already gave a spiel about this
	guMtxScale(dandelionUnfertilizedTexmtx, 2.0f, 0.5f, 1.0f);
	guMtxScale(daisyPatchTexmtx, 4.0f, 1.0f, 1.0f);
	guMtxTransApply(daisyPatchTexmtx, idkflowerTexmtx, 0.5f, 0.125f, 0.0f);
	guMtxTransApply(dandelionUnfertilizedTexmtx, dandelionUnfertilizedTexmtx, 0.0f, 0.5f, 0.0f);
}

void liy_unpackPackedTextures() {
	TPLFile tplfileFromMem;
	void *qoiParse;
	qoi_desc qoiParseDesc;

	initdumbrand();

	//liy_unpackTPL(laketex);
	liy_unpackTPL(hallway1);
	liy_unpackTPL(hallwaytiles);
	liy_unpackTPL(chainlink);
	//liy_unpackTPL(riverhair);

	printf("poopy -1\n");
	liy_unpackQOI_RGB565(gilltex);
	//liy_unpackQOI_RGB565(gillnotebooktex);
	liy_unpackQOI_RGB5A3(building); 
	GX_InitTexObjLOD(&buildingTexObj, GX_LINEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_DISABLE, GX_DISABLE, GX_ANISO_1);
	liy_unpackQOI_RGB5A3(foliage1);
	liy_unpackQOI_RGB565(skybox);

	printf("poopy\n");
	//liy_unpackQOI_RGB565(rivershirt);
	liy_unpackQOI_RGB565(bark);
	liy_unpackQOI_RGB565(flutterbytex);
	//liy_unpackQOI_RGB565(leatherMatcap);

	printf("abt to get procedtex\n");
	procedtex_randomtex_generate();
	procedtex_randomtex_update();

	unsigned char * rimbuftemp = malloc(128*128);
	genrimtex(rimbuftemp, 3);
	rimlighttex = memalign(32, 128*128);
	liy_ConvertI8(rimbuftemp, rimlighttex, 128, 128);
	free(rimbuftemp);
	DCFlushRange(rimlighttex, 128*128);
	GX_InitTexObj(&rimlightTexObj, rimlighttex, 128, 128, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);

	GX_InvalidateTexAll();
}
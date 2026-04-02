uint songpos = 0, songsize;
s8 * kickstartbuffer;
s8 * sndbuffer[2];
u8 whichbuffer = 0;
u8 * song = NULL;
#define SNDSIZ 16384

void addmoresound(s32 voice) {
	memcpy(sndbuffer[whichbuffer], &song[songpos], SNDSIZ);
	ASND_AddVoice(voice, sndbuffer[whichbuffer], SNDSIZ);
	songpos += SNDSIZ;
	whichbuffer ^= 1;
}

void song_play() {
	sndbuffer[0] = MEM_K0_TO_K1(LilyCoolMalloc(SNDSIZ));
	sndbuffer[1] = MEM_K0_TO_K1(LilyCoolMalloc(SNDSIZ));
	kickstartbuffer = MEM_K0_TO_K1(LilyCoolMalloc(32));
	for(int i = 0; i < 32; i++) {
		kickstartbuffer[i] = 0;
	}
	
	FILE * songfile = fopen("guydat.bin", "r");
	if(!songfile) exit(-1);
	fseek(songfile, 0, SEEK_END);
	songsize = ftell(songfile);
	fseek(songfile, 0, SEEK_SET);
	printf("abt to malloc %d byte\n", songsize);
	song = malloc(songsize);
	nullcheck(song);
	fread(song, 1, songsize, songfile);

	ASND_SetVoice(0, VOICE_STEREO_8BIT, 32000, 0, kickstartbuffer, 32, 255, 255, addmoresound);
		
}
void * LilyCoolMalloc(uint size) {
	int lala = (int)malloc(size + 31);
	lala += lala % 32;
	//printf("Lily cool malloc to the rescue!!! %d bytes at 0x%x\n", size, lala); 
	return (void *)lala;
}
